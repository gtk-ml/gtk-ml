#include <stdlib.h>
#include <gtk/gtk.h>
#include <math.h>
#define GTKML_INCLUDE_INTERNAL
#include "gtk-ml.h"
#include "gtk-ml-internal.h"

GTKML_PRIVATE const char *TYPENAME[] = {
    [GTKML_S_NIL] = "nil",
    [GTKML_S_FALSE] = "false",
    [GTKML_S_TRUE] = "true",
    [GTKML_S_INT] = "int",
    [GTKML_S_FLOAT] = "float",
    [GTKML_S_CHAR] = "char",
    [GTKML_S_SYMBOL] = "symbol",
    [GTKML_S_KEYWORD] = "keyword",
    [GTKML_S_LIST] = "list",
    [GTKML_S_MAP] = "map",
    [GTKML_S_SET] = "set",
    [GTKML_S_ARRAY] = "array",
    [GTKML_S_VAR] = "var",
    [GTKML_S_VARARG] = "vararg",
    [GTKML_S_QUOTE] = "quote",
    [GTKML_S_QUASIQUOTE] = "quasiquote",
    [GTKML_S_UNQUOTE] = "unquote",
    [GTKML_S_LAMBDA] = "lambda",
    [GTKML_S_PROGRAM] = "program",
    [GTKML_S_ADDRESS] = "address",
    [GTKML_S_MACRO] = "macro",
    [GTKML_S_LIGHTDATA] = "lightdata",
    [GTKML_S_USERDATA] = "userdata",
};

GTKML_PRIVATE gboolean (*I_ARITH[])(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction) = {
    [GTKML_IA_NOP] = gtk_ml_ia_nop,
    [GTKML_IA_HALT] = gtk_ml_ia_halt,
    [GTKML_IA_ADD] = gtk_ml_ia_add,
    [GTKML_IA_SUBTRACT] = gtk_ml_ia_subtract,
    [GTKML_IA_SIGNED_MULTIPLY] = gtk_ml_ia_signed_multiply,
    [GTKML_IA_UNSIGNED_MULTIPLY] = gtk_ml_ia_unsigned_multiply,
    [GTKML_IA_SIGNED_DIVIDE] = gtk_ml_ia_signed_divide,
    [GTKML_IA_UNSIGNED_DIVIDE] = gtk_ml_ia_unsigned_divide,
    [GTKML_IA_SIGNED_MODULO] = gtk_ml_ia_signed_modulo,
    [GTKML_IA_UNSIGNED_MODULO] = gtk_ml_ia_unsigned_modulo,
    [GTKML_IA_BIT_AND] = gtk_ml_ia_bit_and,
    [GTKML_IA_BIT_OR] = gtk_ml_ia_bit_or,
    [GTKML_IA_BIT_XOR] = gtk_ml_ia_bit_xor,
    [GTKML_IA_BIT_NAND] = gtk_ml_ia_bit_nand,
    [GTKML_IA_BIT_NOR] = gtk_ml_ia_bit_nor,
    [GTKML_IA_BIT_XNOR] = gtk_ml_ia_bit_xnor,
    [GTKML_IA_CAR] = gtk_ml_ia_car,
    [GTKML_IA_CDR] = gtk_ml_ia_cdr,
    [GTKML_IA_BIND] = gtk_ml_ia_bind,
    [GTKML_IA_DEFINE] = gtk_ml_ia_define,
    [GTKML_IA_BIND_ARGS] = gtk_ml_ia_bind_args,
    [GTKML_IA_LIST] = gtk_ml_ia_list,
    [GTKML_IA_ENTER] = gtk_ml_ia_enter,
    [GTKML_IA_LEAVE] = gtk_ml_ia_leave,
    [GTKML_IA_UNWRAP] = gtk_ml_ia_unwrap,
    [GTKML_IA_TYPEOF] = gtk_ml_ia_typeof,
    [255] = (gboolean (*)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction)) NULL,
};

GTKML_PRIVATE gboolean (*I_IMM[])(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction) = {
    [GTKML_II_PUSH_IMM] = gtk_ml_ii_push_imm,
    [GTKML_II_POP] = gtk_ml_ii_pop,
    [GTKML_II_SETF_IMM] = gtk_ml_ii_setf_imm,
    [GTKML_II_POPF] = gtk_ml_ii_popf,
    [GTKML_II_GET_IMM] = gtk_ml_ii_get_imm,
    [GTKML_II_LIST_IMM] = gtk_ml_ii_list_imm,
    [GTKML_II_MAP_IMM] = gtk_ml_ii_map_imm,
    [GTKML_II_SET_IMM] = gtk_ml_ii_set_imm,
    [GTKML_II_ARRAY_IMM] = gtk_ml_ii_array_imm,
    [GTKML_II_SETMM_IMM] = gtk_ml_ii_setmm_imm,
    [GTKML_II_GETMM_IMM] = gtk_ml_ii_getmm_imm,
    [GTKML_II_VAR_IMM] = gtk_ml_ii_var_imm,
    [GTKML_II_GETVAR_IMM] = gtk_ml_ii_getvar_imm,
    [GTKML_II_ASSIGNVAR_IMM] = gtk_ml_ii_assignvar_imm,
    [GTKML_II_LEN] = gtk_ml_ii_len,
    [GTKML_II_ARRAY_INDEX] = gtk_ml_ii_array_index,
    [GTKML_II_ARRAY_PUSH] = gtk_ml_ii_array_push,
    [GTKML_II_ARRAY_POP] = gtk_ml_ii_array_pop,
    [GTKML_II_MAP_GET] = gtk_ml_ii_map_get,
    [GTKML_II_MAP_INSERT] = gtk_ml_ii_map_insert,
    [GTKML_II_MAP_DELETE] = gtk_ml_ii_map_delete,
    [GTKML_II_SET_CONTAINS] = gtk_ml_ii_set_contains,
    [GTKML_II_SET_INSERT] = gtk_ml_ii_set_insert,
    [GTKML_II_SET_DELETE] = gtk_ml_ii_set_delete,
    [255] = (gboolean (*)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction)) NULL,
};

GTKML_PRIVATE gboolean (*I_BR[])(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction) = {
    [GTKML_IBR_CALL] = gtk_ml_ibr_call,
    [GTKML_IBR_RET] = gtk_ml_ibr_ret,
    [GTKML_IBR_CALL_STD] = gtk_ml_ibr_call_std,
    [GTKML_IBR_BRANCH_ABSOLUTE] = gtk_ml_ibr_branch_absolute,
    [GTKML_IBR_BRANCH_RELATIVE] = gtk_ml_ibr_branch_relative,
    [255] = (gboolean (*)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction)) NULL,
};

GTKML_PRIVATE gboolean (*EI_IMM[])(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction, GtkMl_S *) = {
    [GTKML_EII_PUSH_EXT_IMM] = gtk_ml_eii_push_ext_imm,
    [GTKML_EII_POP_EXT] = gtk_ml_eii_pop_ext,
    [GTKML_EII_SETF_EXT_IMM] = gtk_ml_eii_setf_ext_imm,
    [GTKML_EII_POPF_EXT] = gtk_ml_eii_popf_ext,
    [GTKML_EII_GET_EXT_IMM] = gtk_ml_eii_get_ext_imm,
    [GTKML_EII_LIST_EXT_IMM] = gtk_ml_eii_list_ext_imm,
    [GTKML_EII_MAP_EXT_IMM] = gtk_ml_eii_map_ext_imm,
    [GTKML_EII_SET_EXT_IMM] = gtk_ml_eii_set_ext_imm,
    [GTKML_EII_ARRAY_EXT_IMM] = gtk_ml_eii_array_ext_imm,
    [GTKML_EII_CMP_EXT_IMM] = gtk_ml_eii_cmp_ext_imm,
    [255] = (gboolean (*)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction, GtkMl_S *)) NULL,
};

GTKML_PRIVATE gboolean (*EI_BR[])(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction, GtkMl_S *) = {
    [GTKML_EIBR_CALL_EXT_STD] = gtk_ml_eibr_call_ext_std,
    [GTKML_EIBR_BRANCH_ABSOLUTE_EXT] = gtk_ml_eibr_branch_absolute_ext,
    [GTKML_EIBR_BRANCH_RELATIVE_EXT] = gtk_ml_eibr_branch_relative_ext,
    [255] = (gboolean (*)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction, GtkMl_S *)) NULL,
};

gboolean gtk_ml_ia(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction *instr) {
    gboolean (*opcode)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction) = I_ARITH[instr->arith.opcode];
    if (opcode) {
        return opcode(vm, err, *instr);
    } else {
        *err = gtk_ml_error(vm->ctx, "opcode-error", GTKML_ERR_OPCODE_ERROR, 0, 0, 0, 0);
        return 0;
    }
}

gboolean gtk_ml_ii(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction *instr) {
    gboolean (*opcode)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction) = I_IMM[instr->imm.opcode];
    if (opcode) {
        return opcode(vm, err, *instr);
    } else {
        *err = gtk_ml_error(vm->ctx, "opcode-error", GTKML_ERR_OPCODE_ERROR, 0, 0, 0, 0);
        return 0;
    }
}

gboolean gtk_ml_ibr(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction *instr) {
    gboolean (*opcode)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction) = I_BR[instr->br.opcode];
    if (opcode) {
        return opcode(vm, err, *instr);
    } else {
        *err = gtk_ml_error(vm->ctx, "opcode-error", GTKML_ERR_OPCODE_ERROR, 0, 0, 0, 0);
        return 0;
    }
}

gboolean gtk_ml_eii(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction *instr) {
    gboolean (*opcode)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction, GtkMl_S *) = EI_IMM[instr->imm.opcode];
    if (opcode) {
        return opcode(vm, err, instr[0], vm->program.statics[instr[1].imm64]);
    } else {
        *err = gtk_ml_error(vm->ctx, "opcode-error", GTKML_ERR_OPCODE_ERROR, 0, 0, 0, 0);
        return 0;
    }
}

gboolean gtk_ml_eibr(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction *instr) {
    gboolean (*opcode)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction, GtkMl_S *) = EI_BR[instr->br.opcode];
    if (opcode) {
        return opcode(vm, err, instr[0], vm->program.statics[instr[1].imm64]);
    } else {
        *err = gtk_ml_error(vm->ctx, "opcode-error", GTKML_ERR_OPCODE_ERROR, 0, 0, 0, 0);
        return 0;
    }
}

gboolean gtk_ml_enop(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction *instr) {
    (void) err;
    (void) instr;

    vm->reg[GTKML_R_PC].pc += 16;

    return 1;
}

#define PC_INCREMENT vm->reg[GTKML_R_PC].pc += 8
#define PC_INCREMENT_EXTENDED vm->reg[GTKML_R_PC].pc += 16

gboolean gtk_ml_ia_nop(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_halt(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    vm->reg[GTKML_R_FLAGS].flags |= GTKML_F_HALT;
    PC_INCREMENT;
    return 1;
}

GTKML_PRIVATE void set_flags(GtkMl_Vm *vm, GtkMl_S *lhs, GtkMl_S *rhs, GtkMl_S *result) {
    (void) vm;
    (void) lhs;
    (void) rhs;

    // TODO(walterpi): overflow, carry
    if (result->kind == GTKML_S_INT) {
        if (result->value.s_int.value == 0) {
            vm->reg[GTKML_R_FLAGS].flags |= GTKML_F_ZERO;
        } else if (result->value.s_int.value < 0) {
            vm->reg[GTKML_R_FLAGS].flags |= GTKML_F_SIGN;
        }
    } else if (result->kind == GTKML_S_FLOAT) {
        if (result->value.s_float.value == 0.0) {
            vm->reg[GTKML_R_FLAGS].flags |= GTKML_F_ZERO;
        } else if (result->value.s_float.value < 0.0) {
            vm->reg[GTKML_R_FLAGS].flags |= GTKML_F_SIGN;
        }
    }
}

gboolean gtk_ml_ia_add(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_int(vm->ctx, NULL, lhs->value.s_int.value + rhs->value.s_int.value));
    } else if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_FLOAT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_float(vm->ctx, NULL, (double) lhs->value.s_int.value + rhs->value.s_float.value));
    } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_float(vm->ctx, NULL, lhs->value.s_float.value + (double) rhs->value.s_int.value));
    } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_FLOAT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_float(vm->ctx, NULL, lhs->value.s_float.value + rhs->value.s_float.value));
    } else {
        *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
        return 0;
    }
    set_flags(vm, lhs, rhs, gtk_ml_peek(vm->ctx));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_subtract(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_int(vm->ctx, NULL, lhs->value.s_int.value - rhs->value.s_int.value));
    } else if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_FLOAT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_float(vm->ctx, NULL, (double) lhs->value.s_int.value - rhs->value.s_float.value));
    } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_float(vm->ctx, NULL, lhs->value.s_float.value - (double) rhs->value.s_int.value));
    } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_FLOAT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_float(vm->ctx, NULL, lhs->value.s_float.value - rhs->value.s_float.value));
    } else {
        *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
        return 0;
    }
    set_flags(vm, lhs, rhs, gtk_ml_peek(vm->ctx));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_signed_multiply(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_int(vm->ctx, NULL, lhs->value.s_int.value * rhs->value.s_int.value));
    } else if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_FLOAT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_float(vm->ctx, NULL, (double) lhs->value.s_int.value * rhs->value.s_float.value));
    } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_float(vm->ctx, NULL, lhs->value.s_float.value * (double) rhs->value.s_int.value));
    } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_FLOAT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_float(vm->ctx, NULL, lhs->value.s_float.value * rhs->value.s_float.value));
    } else {
        *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
        return 0;
    }
    set_flags(vm, lhs, rhs, gtk_ml_peek(vm->ctx));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_unsigned_multiply(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_int(vm->ctx, NULL, (uint64_t) lhs->value.s_int.value * (uint64_t) rhs->value.s_int.value));
    } else if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_FLOAT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_float(vm->ctx, NULL, (double) (uint64_t) lhs->value.s_int.value * rhs->value.s_float.value));
    } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_float(vm->ctx, NULL, lhs->value.s_float.value * (double) (uint64_t) rhs->value.s_int.value));
    } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_FLOAT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_float(vm->ctx, NULL, lhs->value.s_float.value * rhs->value.s_float.value));
    } else {
        *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
        return 0;
    }
    set_flags(vm, lhs, rhs, gtk_ml_peek(vm->ctx));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_signed_divide(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_int(vm->ctx, NULL, lhs->value.s_int.value / rhs->value.s_int.value));
    } else if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_FLOAT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_float(vm->ctx, NULL, (double) lhs->value.s_int.value / rhs->value.s_float.value));
    } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_float(vm->ctx, NULL, lhs->value.s_float.value / (double) rhs->value.s_int.value));
    } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_FLOAT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_float(vm->ctx, NULL, lhs->value.s_float.value / rhs->value.s_float.value));
    } else {
        *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
        return 0;
    }
    set_flags(vm, lhs, rhs, gtk_ml_peek(vm->ctx));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_unsigned_divide(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_int(vm->ctx, NULL, (uint64_t) lhs->value.s_int.value / (uint64_t) rhs->value.s_int.value));
    } else if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_FLOAT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_float(vm->ctx, NULL, (double) (uint64_t) lhs->value.s_int.value / rhs->value.s_float.value));
    } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_float(vm->ctx, NULL, lhs->value.s_float.value / (double) (uint64_t) rhs->value.s_int.value));
    } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_FLOAT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_float(vm->ctx, NULL, lhs->value.s_float.value / rhs->value.s_float.value));
    } else {
        *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
        return 0;
    }
    set_flags(vm, lhs, rhs, gtk_ml_peek(vm->ctx));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_signed_modulo(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_int(vm->ctx, NULL, lhs->value.s_int.value % rhs->value.s_int.value));
    } else if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_FLOAT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_float(vm->ctx, NULL, fmod((double) lhs->value.s_int.value, rhs->value.s_float.value)));
    } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_float(vm->ctx, NULL, fmod(lhs->value.s_float.value, (double) rhs->value.s_int.value)));
    } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_FLOAT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_float(vm->ctx, NULL, fmod(lhs->value.s_float.value, rhs->value.s_float.value)));
    } else {
        *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
        return 0;
    }
    set_flags(vm, lhs, rhs, gtk_ml_peek(vm->ctx));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_unsigned_modulo(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_int(vm->ctx, NULL, (uint64_t) lhs->value.s_int.value % (uint64_t) rhs->value.s_int.value));
    } else if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_FLOAT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_float(vm->ctx, NULL, fmod((double) (uint64_t) lhs->value.s_int.value, rhs->value.s_float.value)));
    } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_float(vm->ctx, NULL, fmod(lhs->value.s_float.value, (double) (uint64_t) rhs->value.s_int.value)));
    } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_FLOAT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_float(vm->ctx, NULL, fmod(lhs->value.s_float.value, rhs->value.s_float.value)));
    } else {
        *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
        return 0;
    }
    set_flags(vm, lhs, rhs, gtk_ml_peek(vm->ctx));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_bit_and(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_int(vm->ctx, NULL, lhs->value.s_int.value & rhs->value.s_int.value));
    } else {
        *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
        return 0;
    }
    set_flags(vm, lhs, rhs, gtk_ml_peek(vm->ctx));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_bit_or(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_int(vm->ctx, NULL, lhs->value.s_int.value | rhs->value.s_int.value));
    } else {
        *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
        return 0;
    }
    set_flags(vm, lhs, rhs, gtk_ml_peek(vm->ctx));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_bit_xor(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_int(vm->ctx, NULL, lhs->value.s_int.value ^ rhs->value.s_int.value));
    } else {
        *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
        return 0;
    }
    set_flags(vm, lhs, rhs, gtk_ml_peek(vm->ctx));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_bit_nand(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_int(vm->ctx, NULL, ~(lhs->value.s_int.value & rhs->value.s_int.value)));
    } else {
        *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
        return 0;
    }
    set_flags(vm, lhs, rhs, gtk_ml_peek(vm->ctx));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_bit_nor(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_int(vm->ctx, NULL, ~(lhs->value.s_int.value | rhs->value.s_int.value)));
    } else {
        *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
        return 0;
    }
    set_flags(vm, lhs, rhs, gtk_ml_peek(vm->ctx));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_bit_xnor(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, gtk_ml_new_int(vm->ctx, NULL, ~(lhs->value.s_int.value ^ rhs->value.s_int.value)));
    } else {
        *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
        return 0;
    }
    set_flags(vm, lhs, rhs, gtk_ml_peek(vm->ctx));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_car(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *list = gtk_ml_pop(vm->ctx);
    if (list->kind != GTKML_S_LIST) {
        GtkMl_S *error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 0);

        GtkMl_S *new = gtk_ml_new_map(vm->ctx, NULL, NULL);
        gtk_ml_del_hash_trie(vm->ctx, &new->value.s_map.map, gtk_ml_delete_void_reference);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "list", strlen("list")));
        error = new;

        new = gtk_ml_new_map(vm->ctx, NULL, NULL);
        gtk_ml_del_hash_trie(vm->ctx, &new->value.s_map.map, gtk_ml_delete_void_reference);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), list);
        error = new;

        *err = error;
        return 0;
    }
    gtk_ml_push(vm->ctx, gtk_ml_car(list));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_cdr(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *list = gtk_ml_pop(vm->ctx);
    if (list->kind != GTKML_S_LIST) {
        GtkMl_S *error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 0);

        GtkMl_S *new = gtk_ml_new_map(vm->ctx, NULL, NULL);
        gtk_ml_del_hash_trie(vm->ctx, &new->value.s_map.map, gtk_ml_delete_void_reference);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "list", strlen("list")));
        error = new;

        new = gtk_ml_new_map(vm->ctx, NULL, NULL);
        gtk_ml_del_hash_trie(vm->ctx, &new->value.s_map.map, gtk_ml_delete_void_reference);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), list);
        error = new;

        *err = error;
        return 0;
    }
    gtk_ml_push(vm->ctx, gtk_ml_cdr(list));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_bind(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *key = gtk_ml_pop(vm->ctx);
    GtkMl_S *value = gtk_ml_pop(vm->ctx);
    gtk_ml_bind(vm->ctx, key, value);
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_enter(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    gtk_ml_enter(vm->ctx);
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_leave(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    gtk_ml_leave(vm->ctx);
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_unwrap(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *list = gtk_ml_pop(vm->ctx);
    size_t n = 0;
    while (list->kind != GTKML_S_NIL) {
        ++n;
        gtk_ml_push(vm->ctx, gtk_ml_car(list));
        list = gtk_ml_cdr(list);
    }
    gtk_ml_push(vm->ctx, gtk_ml_new_int(vm->ctx, NULL, n));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_typeof(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *value = gtk_ml_pop(vm->ctx);
    if (value->kind == GTKML_S_ARRAY) {
        if (gtk_ml_array_trie_is_string(&value->value.s_array.array)) {
            gtk_ml_push(vm->ctx, gtk_ml_new_keyword(vm->ctx, NULL, 0, "string", strlen("string")));
        } else {
            gtk_ml_push(vm->ctx, gtk_ml_new_keyword(vm->ctx, NULL, 0, TYPENAME[value->kind], strlen(TYPENAME[value->kind])));
        }
    } else {
        gtk_ml_push(vm->ctx, gtk_ml_new_keyword(vm->ctx, NULL, 0, TYPENAME[value->kind], strlen(TYPENAME[value->kind])));
    }
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_define(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *key = gtk_ml_pop(vm->ctx);
    GtkMl_S *value = gtk_ml_pop(vm->ctx);
    gtk_ml_define(vm->ctx, key, value);
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_bind_args(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_S *revkeys = gtk_ml_pop(vm->ctx);
    size_t n = gtk_ml_pop(vm->ctx)->value.s_int.value;

    GtkMl_S *_revkeys = revkeys;
    size_t n_params = 0;
    while (revkeys->kind != GTKML_S_NIL) {
        ++n_params;
        revkeys = gtk_ml_cdr(revkeys);
    }
    revkeys = _revkeys;

    for (size_t i = 0; i < n_params; i++) {
        GtkMl_S *key = gtk_ml_car(revkeys);
        if (key->kind == GTKML_S_VARARG) {
            GtkMl_S *rest = gtk_ml_new_nil(vm->ctx, NULL);
            for (size_t j = 0; j < n - (n_params - 1); j++) {
                rest = gtk_ml_new_list(vm->ctx, NULL, gtk_ml_pop(vm->ctx), rest);
            }
            gtk_ml_bind(vm->ctx, key->value.s_vararg.expr, rest);
        } else {
            GtkMl_S *value = gtk_ml_pop(vm->ctx);
            gtk_ml_bind(vm->ctx, key, value);
        }
        revkeys = gtk_ml_cdr(revkeys);
    }

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_list(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    int64_t n = gtk_ml_pop(vm->ctx)->value.s_int.value;

    GtkMl_S *list = gtk_ml_new_nil(vm->ctx, NULL);

    while (n--) {
        GtkMl_S *expr = gtk_ml_pop(vm->ctx);
        list = gtk_ml_new_list(vm->ctx, NULL, expr, list);
    }

    gtk_ml_push(vm->ctx, list);

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_push_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    gtk_ml_push(vm->ctx, vm->program.statics[instr.imm.imm]);
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_setf_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    switch (vm->program.statics[instr.imm.imm]->kind) {
    case GTKML_S_TRUE:
        vm->reg[GTKML_R_FLAGS].flags |= GTKML_F_ZERO;
        break;
    case GTKML_S_FALSE:
        vm->reg[GTKML_R_FLAGS].flags &= ~GTKML_F_ZERO;
        break;
    default:
        *err = gtk_ml_error(vm->ctx, "boolean-error", GTKML_ERR_BOOLEAN_ERROR, 0, 0, 0, 0);
        return 0;
    }
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_popf(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    switch (gtk_ml_pop(vm->ctx)->kind) {
    case GTKML_S_TRUE:
        vm->reg[GTKML_R_FLAGS].flags |= GTKML_F_ZERO;
        break;
    case GTKML_S_FALSE:
        vm->reg[GTKML_R_FLAGS].flags &= ~GTKML_F_ZERO;
        break;
    default:
        *err = gtk_ml_error(vm->ctx, "boolean-error", GTKML_ERR_BOOLEAN_ERROR, 0, 0, 0, 0);
        return 0;
    }
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_pop(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    gtk_ml_pop(vm->ctx);
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_get_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *value = gtk_ml_get(vm->ctx, vm->program.statics[instr.imm.imm]);
    if (value) {
        gtk_ml_push(vm->ctx, value);
    } else {
        GtkMl_S *error = gtk_ml_error(vm->ctx, "binding-error", GTKML_ERR_BINDING_ERROR, 0, 0, 0, 0);
        GtkMl_S *new = gtk_ml_new_map(vm->ctx, NULL, NULL);
        gtk_ml_del_hash_trie(vm->ctx, &new->value.s_map.map, gtk_ml_delete_void_reference);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_new_keyword(vm->ctx, NULL, 0, "binding", strlen("binding")), vm->program.statics[instr.imm.imm]);
        error = new;
        *err = error;
        return 0;
    }
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_list_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;

    int64_t n = vm->program.statics[instr.imm.imm]->value.s_int.value;

    GtkMl_S *list = gtk_ml_new_nil(vm->ctx, NULL);

    while (n--) {
        GtkMl_S *expr = gtk_ml_pop(vm->ctx);
        list = gtk_ml_new_list(vm->ctx, NULL, expr, list);
    }

    gtk_ml_push(vm->ctx, list);

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_map_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;

    int64_t n = vm->program.statics[instr.imm.imm]->value.s_int.value;

    GtkMl_S *map = gtk_ml_new_map(vm->ctx, NULL, NULL);

    while (n--) {
        GtkMl_S *value = gtk_ml_pop(vm->ctx);
        GtkMl_S *key = gtk_ml_pop(vm->ctx);
        GtkMl_S *new = gtk_ml_new_map(vm->ctx, NULL, NULL);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &map->value.s_map.map, key, value);
        map = new;
    }

    gtk_ml_push(vm->ctx, map);

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_set_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;

    int64_t n = vm->program.statics[instr.imm.imm]->value.s_int.value;

    GtkMl_S *set = gtk_ml_new_set(vm->ctx, NULL);

    while (n--) {
        GtkMl_S *key = gtk_ml_pop(vm->ctx);
        GtkMl_S *new = gtk_ml_new_set(vm->ctx, NULL);
        gtk_ml_hash_set_insert(&new->value.s_set.set, &set->value.s_set.set, key);
        set = new;
    }

    gtk_ml_push(vm->ctx, set);

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_array_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;

    int64_t n = vm->program.statics[instr.imm.imm]->value.s_int.value;

    GtkMl_S *array = gtk_ml_new_array(vm->ctx, NULL);

    while (n--) {
        GtkMl_S *value = gtk_ml_pop(vm->ctx);
        GtkMl_S *new = gtk_ml_new_array(vm->ctx, NULL);
        gtk_ml_array_trie_push(&new->value.s_array.array, &array->value.s_array.array, value);
        array = new;
    }

    gtk_ml_push(vm->ctx, array);

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_setmm_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_S *metamap = gtk_ml_pop(vm->ctx);
    GtkMl_S *map = gtk_ml_pop(vm->ctx);

    GtkMl_S *result = gtk_ml_new_map(vm->ctx, NULL, metamap);
    gtk_ml_del_hash_trie(vm->ctx, &result->value.s_map.map, gtk_ml_delete_void_reference);
    gtk_ml_hash_trie_copy(&result->value.s_map.map, &map->value.s_map.map);

    gtk_ml_push(vm->ctx, result);

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_getmm_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_S *map = gtk_ml_pop(vm->ctx);

    GtkMl_S *result = gtk_ml_getmetamap(map);
    if (!result) {
        *err = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 0);
        return 0;
    }
    gtk_ml_push(vm->ctx, result);

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_var_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_S *value = gtk_ml_pop(vm->ctx);
    GtkMl_S *result = gtk_ml_new_var(vm->ctx, NULL, value);
    gtk_ml_push(vm->ctx, result);

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_getvar_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_S *var = gtk_ml_pop(vm->ctx);
    gtk_ml_push(vm->ctx, var->value.s_var.expr);

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_assignvar_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_S *newvalue = gtk_ml_pop(vm->ctx);
    GtkMl_S *var = gtk_ml_pop(vm->ctx);
    var->value.s_var.expr = newvalue;
    gtk_ml_push(vm->ctx, var);

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_len(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_S *container = gtk_ml_pop(vm->ctx);
    switch (container->kind) {
    case GTKML_S_ARRAY:
        gtk_ml_push(vm->ctx, gtk_ml_new_int(vm->ctx, NULL, gtk_ml_array_trie_len(&container->value.s_array.array)));
        break;
    case GTKML_S_MAP:
        gtk_ml_push(vm->ctx, gtk_ml_new_int(vm->ctx, NULL, gtk_ml_hash_trie_len(&container->value.s_map.map)));
        break;
    case GTKML_S_SET:
        gtk_ml_push(vm->ctx, gtk_ml_new_int(vm->ctx, NULL, gtk_ml_hash_set_len(&container->value.s_set.set)));
        break;
    default:
        *err = gtk_ml_error(vm->ctx, "container-error", GTKML_ERR_CONTAINER_ERROR, 0, 0, 0, 0);
        return 0;
    }

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_array_index(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_S *container = gtk_ml_pop(vm->ctx);
    GtkMl_S *index = gtk_ml_pop(vm->ctx);

    if (index->kind != GTKML_S_INT) {
        GtkMl_S *error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 0);

        GtkMl_S *new = gtk_ml_new_map(vm->ctx, NULL, NULL);
        gtk_ml_del_hash_trie(vm->ctx, &new->value.s_map.map, gtk_ml_delete_void_reference);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "int", strlen("int")));
        error = new;

        new = gtk_ml_new_map(vm->ctx, NULL, NULL);
        gtk_ml_del_hash_trie(vm->ctx, &new->value.s_map.map, gtk_ml_delete_void_reference);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), index);
        error = new;

        *err = error;
        return 0;
    }

    switch (container->kind) {
    case GTKML_S_ARRAY:
        gtk_ml_push(vm->ctx, gtk_ml_array_trie_get(&container->value.s_array.array, index->value.s_int.value));
        break;
    default: {
        GtkMl_S *error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 0);

        GtkMl_S *new = gtk_ml_new_map(vm->ctx, NULL, NULL);
        gtk_ml_del_hash_trie(vm->ctx, &new->value.s_map.map, gtk_ml_delete_void_reference);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "array", strlen("array")));
        error = new;

        new = gtk_ml_new_map(vm->ctx, NULL, NULL);
        gtk_ml_del_hash_trie(vm->ctx, &new->value.s_map.map, gtk_ml_delete_void_reference);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), container);
        error = new;

        *err = error;
        return 0;
    }
    }

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_array_push(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_S *container = gtk_ml_pop(vm->ctx);
    GtkMl_S *value = gtk_ml_pop(vm->ctx);
    GtkMl_S *result = gtk_ml_new_array(vm->ctx, NULL);
    gtk_ml_del_array_trie(vm->ctx, &result->value.s_array.array, gtk_ml_delete_value_reference);

    switch (container->kind) {
    case GTKML_S_ARRAY:
        gtk_ml_array_trie_push(&result->value.s_array.array, &container->value.s_array.array, value);
        gtk_ml_push(vm->ctx, result);
        break;
    default: {
        GtkMl_S *error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 0);

        GtkMl_S *new = gtk_ml_new_map(vm->ctx, NULL, NULL);
        gtk_ml_del_hash_trie(vm->ctx, &new->value.s_map.map, gtk_ml_delete_void_reference);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "array", strlen("array")));
        error = new;

        new = gtk_ml_new_map(vm->ctx, NULL, NULL);
        gtk_ml_del_hash_trie(vm->ctx, &new->value.s_map.map, gtk_ml_delete_void_reference);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), container);
        error = new;

        *err = error;
        return 0;
    }
    }

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_array_pop(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_S *container = gtk_ml_pop(vm->ctx);
    GtkMl_S *result = gtk_ml_new_array(vm->ctx, NULL);
    gtk_ml_del_array_trie(vm->ctx, &result->value.s_array.array, gtk_ml_delete_value_reference);

    switch (container->kind) {
    case GTKML_S_ARRAY:
        gtk_ml_array_trie_pop(&result->value.s_array.array, &container->value.s_array.array);
        gtk_ml_push(vm->ctx, result);
        break;
    default: {
        GtkMl_S *error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 0);

        GtkMl_S *new = gtk_ml_new_map(vm->ctx, NULL, NULL);
        gtk_ml_del_hash_trie(vm->ctx, &new->value.s_map.map, gtk_ml_delete_void_reference);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "array", strlen("array")));
        error = new;

        new = gtk_ml_new_map(vm->ctx, NULL, NULL);
        gtk_ml_del_hash_trie(vm->ctx, &new->value.s_map.map, gtk_ml_delete_void_reference);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), container);
        error = new;

        *err = error;
        return 0;
    }
    }

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_map_get(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_S *container = gtk_ml_pop(vm->ctx);
    GtkMl_S *key = gtk_ml_pop(vm->ctx);

    switch (container->kind) {
    case GTKML_S_MAP:
        gtk_ml_push(vm->ctx, gtk_ml_hash_trie_get(&container->value.s_map.map, key));
        break;
    default: {
        GtkMl_S *error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 0);

        GtkMl_S *new = gtk_ml_new_map(vm->ctx, NULL, NULL);
        gtk_ml_del_hash_trie(vm->ctx, &new->value.s_map.map, gtk_ml_delete_void_reference);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "map", strlen("map")));
        error = new;

        new = gtk_ml_new_map(vm->ctx, NULL, NULL);
        gtk_ml_del_hash_trie(vm->ctx, &new->value.s_map.map, gtk_ml_delete_void_reference);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), container);
        error = new;

        *err = error;
        return 0;
    }
    }

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_map_insert(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_S *container = gtk_ml_pop(vm->ctx);
    GtkMl_S *key = gtk_ml_pop(vm->ctx);
    GtkMl_S *value = gtk_ml_pop(vm->ctx);
    GtkMl_S *result = gtk_ml_new_map(vm->ctx, NULL, container->value.s_map.metamap);
    gtk_ml_del_hash_trie(vm->ctx, &result->value.s_map.map, gtk_ml_delete_void_reference);

    switch (container->kind) {
    case GTKML_S_ARRAY:
        gtk_ml_hash_trie_insert(&result->value.s_map.map, &container->value.s_map.map, key, value);
        gtk_ml_push(vm->ctx, result);
        break;
    default: {
        GtkMl_S *error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 0);

        GtkMl_S *new = gtk_ml_new_map(vm->ctx, NULL, NULL);
        gtk_ml_del_hash_trie(vm->ctx, &new->value.s_map.map, gtk_ml_delete_void_reference);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "map", strlen("map")));
        error = new;

        new = gtk_ml_new_map(vm->ctx, NULL, NULL);
        gtk_ml_del_hash_trie(vm->ctx, &new->value.s_map.map, gtk_ml_delete_void_reference);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), container);
        error = new;

        *err = error;
        return 0;
    }
    }

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_map_delete(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_S *container = gtk_ml_pop(vm->ctx);
    GtkMl_S *key = gtk_ml_pop(vm->ctx);
    GtkMl_S *result = gtk_ml_new_map(vm->ctx, NULL, container->value.s_map.metamap);
    gtk_ml_del_hash_trie(vm->ctx, &result->value.s_map.map, gtk_ml_delete_void_reference);

    switch (container->kind) {
    case GTKML_S_ARRAY:
        gtk_ml_hash_trie_delete(&result->value.s_map.map, &container->value.s_map.map, key);
        gtk_ml_push(vm->ctx, result);
        break;
    default: {
        GtkMl_S *error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 0);

        GtkMl_S *new = gtk_ml_new_map(vm->ctx, NULL, NULL);
        gtk_ml_del_hash_trie(vm->ctx, &new->value.s_map.map, gtk_ml_delete_void_reference);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "map", strlen("map")));
        error = new;

        new = gtk_ml_new_map(vm->ctx, NULL, NULL);
        gtk_ml_del_hash_trie(vm->ctx, &new->value.s_map.map, gtk_ml_delete_void_reference);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), container);
        error = new;

        *err = error;
        return 0;
    }
    }

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_set_contains(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_S *container = gtk_ml_pop(vm->ctx);
    GtkMl_S *key = gtk_ml_pop(vm->ctx);

    switch (container->kind) {
    case GTKML_S_SET:
        gtk_ml_push(vm->ctx, gtk_ml_hash_set_contains(&container->value.s_set.set, key)? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL));
        break;
    default: {
        GtkMl_S *error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 0);

        GtkMl_S *new = gtk_ml_new_map(vm->ctx, NULL, NULL);
        gtk_ml_del_hash_trie(vm->ctx, &new->value.s_map.map, gtk_ml_delete_void_reference);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "set", strlen("set")));
        error = new;

        new = gtk_ml_new_map(vm->ctx, NULL, NULL);
        gtk_ml_del_hash_trie(vm->ctx, &new->value.s_map.map, gtk_ml_delete_void_reference);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), container);
        error = new;

        *err = error;
        return 0;
    }
    }

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_set_insert(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_S *container = gtk_ml_pop(vm->ctx);
    GtkMl_S *key = gtk_ml_pop(vm->ctx);
    GtkMl_S *result = gtk_ml_new_set(vm->ctx, NULL);
    gtk_ml_del_hash_set(vm->ctx, &result->value.s_set.set, gtk_ml_delete_void_reference);

    switch (container->kind) {
    case GTKML_S_ARRAY:
        gtk_ml_hash_set_insert(&result->value.s_set.set, &container->value.s_set.set, key);
        gtk_ml_push(vm->ctx, result);
        break;
    default: {
        GtkMl_S *error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 0);

        GtkMl_S *new = gtk_ml_new_map(vm->ctx, NULL, NULL);
        gtk_ml_del_hash_trie(vm->ctx, &new->value.s_map.map, gtk_ml_delete_void_reference);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "set", strlen("set")));
        error = new;

        new = gtk_ml_new_map(vm->ctx, NULL, NULL);
        gtk_ml_del_hash_trie(vm->ctx, &new->value.s_map.map, gtk_ml_delete_void_reference);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), container);
        error = new;

        *err = error;
        return 0;
    }
    }

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_set_delete(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_S *container = gtk_ml_pop(vm->ctx);
    GtkMl_S *key = gtk_ml_pop(vm->ctx);
    GtkMl_S *result = gtk_ml_new_set(vm->ctx, NULL);
    gtk_ml_del_hash_set(vm->ctx, &result->value.s_set.set, gtk_ml_delete_void_reference);

    switch (container->kind) {
    case GTKML_S_ARRAY:
        gtk_ml_hash_set_delete(&result->value.s_set.set, &container->value.s_set.set, key);
        gtk_ml_push(vm->ctx, result);
        break;
    default: {
        GtkMl_S *error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 0);

        GtkMl_S *new = gtk_ml_new_map(vm->ctx, NULL, NULL);
        gtk_ml_del_hash_trie(vm->ctx, &new->value.s_map.map, gtk_ml_delete_void_reference);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "set", strlen("set")));
        error = new;

        new = gtk_ml_new_map(vm->ctx, NULL, NULL);
        gtk_ml_del_hash_trie(vm->ctx, &new->value.s_map.map, gtk_ml_delete_void_reference);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), container);
        error = new;

        *err = error;
        return 0;
    }
    }

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ibr_call(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_S *program = gtk_ml_pop(vm->ctx);

    uint64_t flags = vm->reg[GTKML_R_FLAGS].flags & GTKML_F_TOPCALL;
    vm->call_stack[vm->call_stack_ptr++] = flags;
    vm->reg[GTKML_R_FLAGS].flags &= ~GTKML_F_TOPCALL;

    PC_INCREMENT;
    uint64_t pc = vm->reg[GTKML_R_PC].pc;
    vm->call_stack[vm->call_stack_ptr++] = pc;
    vm->reg[GTKML_R_PC].pc = program->value.s_program.addr;

    gtk_ml_enter(vm->ctx);

    return 1;
}

gboolean gtk_ml_ibr_ret(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    if (vm->reg[GTKML_R_FLAGS].flags & GTKML_F_TOPCALL) {
        vm->reg[GTKML_R_FLAGS].flags |= GTKML_F_HALT;
        PC_INCREMENT;
    } else {
        gtk_ml_leave(vm->ctx);

        uint64_t pc = vm->call_stack[--vm->call_stack_ptr];
        uint64_t flags = vm->call_stack[--vm->call_stack_ptr];
        vm->reg[GTKML_R_PC].pc = pc;
        vm->reg[GTKML_R_FLAGS].flags |= flags;
    }

    return 1;
}

gboolean gtk_ml_ibr_call_std(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) instr;
    GtkMl_S *(*function)(GtkMl_Context *, GtkMl_S **, GtkMl_S *) = vm->std[vm->program.statics[instr.br.imm]->value.s_int.value];
    GtkMl_S *expr = gtk_ml_pop(vm->ctx);
    GtkMl_S *value = function(vm->ctx, err, expr);
    if (value) {
        gtk_ml_push(vm->ctx, value);
    } else {
        return 0;
    }
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ibr_branch_absolute(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    vm->reg[GTKML_R_FLAGS].flags &= ~GTKML_F_GENERIC;
    GtkMl_S *addr = vm->program.statics[instr.br.imm];
    vm->reg[GTKML_R_PC].pc = addr->value.s_address.addr;
    return 1;
}

gboolean gtk_ml_ibr_branch_relative(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    vm->reg[GTKML_R_FLAGS].flags &= ~GTKML_F_GENERIC;
    GtkMl_S *addr = vm->program.statics[instr.br.imm];
    vm->reg[GTKML_R_PC].pc += 8 + addr->value.s_address.addr;
    return 1;
}

gboolean gtk_ml_eii_push_ext_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64) {
    (void) err;
    (void) instr;
    gtk_ml_push(vm->ctx, imm64);
    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eii_setf_ext_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64) {
    (void) instr;
    switch (imm64->kind) {
    case GTKML_S_TRUE:
        vm->reg[GTKML_R_FLAGS].flags |= GTKML_F_ZERO;
        break;
    case GTKML_S_FALSE:
        vm->reg[GTKML_R_FLAGS].flags &= ~GTKML_F_ZERO;
        break;
    default:
        *err = gtk_ml_error(vm->ctx, "boolean-error", GTKML_ERR_BOOLEAN_ERROR, 0, 0, 0, 0);
        return 0;
    }
    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eii_popf_ext(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64) {
    (void) err;
    (void) instr;
    (void) imm64;
    switch (gtk_ml_pop(vm->ctx)->kind) {
    case GTKML_S_TRUE:
        vm->reg[GTKML_R_FLAGS].flags |= GTKML_F_ZERO;
        break;
    case GTKML_S_FALSE:
        vm->reg[GTKML_R_FLAGS].flags &= ~GTKML_F_ZERO;
        break;
    default:
        *err = gtk_ml_error(vm->ctx, "boolean-error", GTKML_ERR_BOOLEAN_ERROR, 0, 0, 0, 0);
        return 0;
    }
    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eii_pop_ext(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64) {
    (void) err;
    (void) instr;
    (void) imm64;
    gtk_ml_pop(vm->ctx);
    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eii_get_ext_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64) {
    (void) err;
    (void) instr;
    GtkMl_S *value = gtk_ml_get(vm->ctx, imm64);
    if (value) {
        gtk_ml_push(vm->ctx, value);
    } else {
        GtkMl_S *error = gtk_ml_error(vm->ctx, "binding-error", GTKML_ERR_BINDING_ERROR, 0, 0, 0, 0);
        GtkMl_S *new = gtk_ml_new_map(vm->ctx, NULL, NULL);
        gtk_ml_del_hash_trie(vm->ctx, &new->value.s_map.map, gtk_ml_delete_void_reference);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_new_keyword(vm->ctx, NULL, 0, "binding", strlen("binding")), imm64);
        error = new;
        *err = error;
        return 0;
    }
    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eii_list_ext_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64) {
    (void) err;
    (void) instr;

    int64_t n = imm64->value.s_int.value;

    GtkMl_S *list = gtk_ml_new_nil(vm->ctx, NULL);

    while (n--) {
        GtkMl_S *expr = gtk_ml_pop(vm->ctx);
        list = gtk_ml_new_list(vm->ctx, NULL, expr, list);
    }

    gtk_ml_push(vm->ctx, list);

    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eii_map_ext_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64) {
    (void) err;
    (void) instr;

    int64_t n = imm64->value.s_int.value;

    GtkMl_S *map = gtk_ml_new_map(vm->ctx, NULL, NULL);

    while (n--) {
        GtkMl_S *key = gtk_ml_pop(vm->ctx);
        GtkMl_S *value = gtk_ml_pop(vm->ctx);
        GtkMl_S *new = gtk_ml_new_map(vm->ctx, NULL, NULL);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &map->value.s_map.map, key, value);
        map = new;
    }

    gtk_ml_push(vm->ctx, map);

    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eii_set_ext_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64) {
    (void) err;
    (void) instr;

    int64_t n = imm64->value.s_int.value;

    GtkMl_S *set = gtk_ml_new_set(vm->ctx, NULL);

    while (n--) {
        GtkMl_S *key = gtk_ml_pop(vm->ctx);
        GtkMl_S *new = gtk_ml_new_set(vm->ctx, NULL);
        gtk_ml_hash_set_insert(&new->value.s_set.set, &set->value.s_set.set, key);
        set = new;
    }

    gtk_ml_push(vm->ctx, set);

    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eii_array_ext_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64) {
    (void) err;
    (void) instr;

    int64_t n = imm64->value.s_int.value;

    GtkMl_S *array = gtk_ml_new_array(vm->ctx, NULL);

    while (n--) {
        GtkMl_S *value = gtk_ml_pop(vm->ctx);
        GtkMl_S *new = gtk_ml_new_array(vm->ctx, NULL);
        gtk_ml_array_trie_push(&new->value.s_array.array, &array->value.s_array.array, value);
        array = new;
    }

    gtk_ml_push(vm->ctx, array);

    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eii_cmp_ext_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64) {
    (void) err;
    (void) instr;

    GtkMl_Cmp cmp = imm64->value.s_int.value;

    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);

    switch (cmp) {
    case GTKML_CMP_EQUAL:
        gtk_ml_push(vm->ctx, gtk_ml_equal(lhs, rhs)? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL));
        break;
    case GTKML_CMP_NOT_EQUAL:
        gtk_ml_push(vm->ctx, (!gtk_ml_equal(lhs, rhs))? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL));
        break;
    case GTKML_CMP_LESS:
        if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
            gtk_ml_push(vm->ctx, (lhs->value.s_int.value < rhs->value.s_int.value)? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL));
        } else if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_FLOAT) {
            gtk_ml_push(vm->ctx, ((double) lhs->value.s_int.value < rhs->value.s_float.value)? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL));
        } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_INT) {
            gtk_ml_push(vm->ctx, (lhs->value.s_float.value < (double) rhs->value.s_int.value)? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL));
        } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_FLOAT) {
            gtk_ml_push(vm->ctx, (lhs->value.s_float.value < rhs->value.s_float.value)? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL));
        } else {
            *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
            return 0;
        }
        break;
    case GTKML_CMP_GREATER:
        if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
            gtk_ml_push(vm->ctx, (lhs->value.s_int.value > rhs->value.s_int.value)? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL));
        } else if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_FLOAT) {
            gtk_ml_push(vm->ctx, ((double) lhs->value.s_int.value > rhs->value.s_float.value)? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL));
        } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_INT) {
            gtk_ml_push(vm->ctx, (lhs->value.s_float.value > (double) rhs->value.s_int.value)? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL));
        } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_FLOAT) {
            gtk_ml_push(vm->ctx, (lhs->value.s_float.value > rhs->value.s_float.value)? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL));
        } else {
            *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
            return 0;
        }
        break;
    case GTKML_CMP_LESS_EQUAL:
        if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
            gtk_ml_push(vm->ctx, (lhs->value.s_int.value <= rhs->value.s_int.value)? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL));
        } else if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_FLOAT) {
            gtk_ml_push(vm->ctx, ((double) lhs->value.s_int.value <= rhs->value.s_float.value)? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL));
        } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_INT) {
            gtk_ml_push(vm->ctx, (lhs->value.s_float.value <= (double) rhs->value.s_int.value)? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL));
        } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_FLOAT) {
            gtk_ml_push(vm->ctx, (lhs->value.s_float.value <= rhs->value.s_float.value)? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL));
        } else {
            *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
            return 0;
        }
        break;
    case GTKML_CMP_GREATER_EQUAL:
        if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
            gtk_ml_push(vm->ctx, (lhs->value.s_int.value >= rhs->value.s_int.value)? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL));
        } else if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_FLOAT) {
            gtk_ml_push(vm->ctx, ((double) lhs->value.s_int.value >= rhs->value.s_float.value)? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL));
        } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_INT) {
            gtk_ml_push(vm->ctx, (lhs->value.s_float.value >= (double) rhs->value.s_int.value)? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL));
        } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_FLOAT) {
            gtk_ml_push(vm->ctx, (lhs->value.s_float.value >= rhs->value.s_float.value)? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL));
        } else {
            *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
            return 0;
        }
        break;
    default:
        *err = gtk_ml_error(vm->ctx, "cmp-error", GTKML_ERR_CMP_ERROR, 0, 0, 0, 0);
        return 0;
    }

    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eibr_call_ext_std(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64) {
    (void) instr;
    GtkMl_S *(*function)(GtkMl_Context *, GtkMl_S **, GtkMl_S *) = vm->std[imm64->value.s_int.value];
    GtkMl_S *expr = gtk_ml_pop(vm->ctx);
    GtkMl_S *value = function(vm->ctx, err, expr);
    if (value) {
        gtk_ml_push(vm->ctx, value);
    } else {
        return 0;
    }
    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eibr_branch_absolute_ext(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64) {
    (void) err;
    (void) instr;
    vm->reg[GTKML_R_FLAGS].flags = 0;
    GtkMl_S *addr = imm64;
    vm->reg[GTKML_R_PC].pc = addr->value.s_address.addr;
    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eibr_branch_relative_ext(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64) {
    (void) err;
    (void) instr;
    vm->reg[GTKML_R_FLAGS].flags = 0;
    GtkMl_S *addr = imm64;
    vm->reg[GTKML_R_PC].pc += 8 + addr->value.s_address.addr;
    PC_INCREMENT_EXTENDED;
    return 1;
}
