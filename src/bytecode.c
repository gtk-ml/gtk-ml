#include <stdlib.h>
#include <string.h>
#ifdef GTKML_ENABLE_GTK
#include <gtk/gtk.h>
#endif /* GTKML_ENABLE_GTK */
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

GTKML_PRIVATE gboolean (*I_ARITH[])(GtkMl_Vm *, GtkMl_SObj *, GtkMl_Instruction) = {
    [GTKML_IA_NOP] = gtk_ml_ia_nop,
    [GTKML_IA_HALT] = gtk_ml_ia_halt,
    [GTKML_IA_ADD] = gtk_ml_ia_signed_add,
    [GTKML_IA_SUBTRACT] = gtk_ml_ia_signed_subtract,
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
    [GTKML_IA_TO_SOBJ] = gtk_ml_ia_to_sobj,
    [GTKML_IA_TO_PRIM] = gtk_ml_ia_to_prim,
    [255] = (gboolean (*)(GtkMl_Vm *, GtkMl_SObj *, GtkMl_Instruction)) NULL,
};

GTKML_PRIVATE gboolean (*I_IMM[])(GtkMl_Vm *, GtkMl_SObj *, GtkMl_Instruction) = {
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
    [GTKML_II_ARRAY_CONCAT] = gtk_ml_ii_array_concat,
    [GTKML_II_MAP_GET] = gtk_ml_ii_map_get,
    [GTKML_II_MAP_INSERT] = gtk_ml_ii_map_insert,
    [GTKML_II_MAP_DELETE] = gtk_ml_ii_map_delete,
    [GTKML_II_SET_CONTAINS] = gtk_ml_ii_set_contains,
    [GTKML_II_SET_INSERT] = gtk_ml_ii_set_insert,
    [GTKML_II_SET_DELETE] = gtk_ml_ii_set_delete,
    [255] = (gboolean (*)(GtkMl_Vm *, GtkMl_SObj *, GtkMl_Instruction)) NULL,
};

GTKML_PRIVATE gboolean (*I_BR[])(GtkMl_Vm *, GtkMl_SObj *, GtkMl_Instruction) = {
    [GTKML_IBR_CALL] = gtk_ml_ibr_call,
    [GTKML_IBR_RET] = gtk_ml_ibr_ret,
    [GTKML_IBR_CALL_STD] = gtk_ml_ibr_call_std,
    [GTKML_IBR_BRANCH_ABSOLUTE] = gtk_ml_ibr_branch_absolute,
    [GTKML_IBR_BRANCH_RELATIVE] = gtk_ml_ibr_branch_relative,
    [255] = (gboolean (*)(GtkMl_Vm *, GtkMl_SObj *, GtkMl_Instruction)) NULL,
};

GTKML_PRIVATE gboolean (*EI_IMM[])(GtkMl_Vm *, GtkMl_SObj *, GtkMl_Instruction, GtkMl_SObj ) = {
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
    [255] = (gboolean (*)(GtkMl_Vm *, GtkMl_SObj *, GtkMl_Instruction, GtkMl_SObj )) NULL,
};

GTKML_PRIVATE gboolean (*EI_BR[])(GtkMl_Vm *, GtkMl_SObj *, GtkMl_Instruction, GtkMl_SObj ) = {
    [GTKML_EIBR_CALL_EXT_STD] = gtk_ml_eibr_call_ext_std,
    [GTKML_EIBR_BRANCH_ABSOLUTE_EXT] = gtk_ml_eibr_branch_absolute_ext,
    [GTKML_EIBR_BRANCH_RELATIVE_EXT] = gtk_ml_eibr_branch_relative_ext,
    [255] = (gboolean (*)(GtkMl_Vm *, GtkMl_SObj *, GtkMl_Instruction, GtkMl_SObj )) NULL,
};

gboolean gtk_ml_ia(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction *instr) {
    gboolean (*opcode)(GtkMl_Vm *, GtkMl_SObj *, GtkMl_Instruction) = I_ARITH[instr->arith.opcode];
    if (opcode) {
        return opcode(vm, err, *instr);
    } else {
        *err = gtk_ml_error(vm->ctx, "opcode-error", GTKML_ERR_OPCODE_ERROR, 0, 0, 0, 0);
        return 0;
    }
}

gboolean gtk_ml_ii(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction *instr) {
    gboolean (*opcode)(GtkMl_Vm *, GtkMl_SObj *, GtkMl_Instruction) = I_IMM[instr->imm.opcode];
    if (opcode) {
        return opcode(vm, err, *instr);
    } else {
        *err = gtk_ml_error(vm->ctx, "opcode-error", GTKML_ERR_OPCODE_ERROR, 0, 0, 0, 0);
        return 0;
    }
}

gboolean gtk_ml_ibr(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction *instr) {
    gboolean (*opcode)(GtkMl_Vm *, GtkMl_SObj *, GtkMl_Instruction) = I_BR[instr->br.opcode];
    if (opcode) {
        return opcode(vm, err, *instr);
    } else {
        *err = gtk_ml_error(vm->ctx, "opcode-error", GTKML_ERR_OPCODE_ERROR, 0, 0, 0, 0);
        return 0;
    }
}

gboolean gtk_ml_eii(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction *instr) {
    gboolean (*opcode)(GtkMl_Vm *, GtkMl_SObj *, GtkMl_Instruction, GtkMl_SObj ) = EI_IMM[instr->imm.opcode];
    if (opcode) {
        return opcode(vm, err, instr[0], vm->program->statics[instr[1].imm64]);
    } else {
        *err = gtk_ml_error(vm->ctx, "opcode-error", GTKML_ERR_OPCODE_ERROR, 0, 0, 0, 0);
        return 0;
    }
}

gboolean gtk_ml_eibr(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction *instr) {
    gboolean (*opcode)(GtkMl_Vm *, GtkMl_SObj *, GtkMl_Instruction, GtkMl_SObj ) = EI_BR[instr->br.opcode];
    if (opcode) {
        return opcode(vm, err, instr[0], vm->program->statics[instr[1].imm64]);
    } else {
        *err = gtk_ml_error(vm->ctx, "opcode-error", GTKML_ERR_OPCODE_ERROR, 0, 0, 0, 0);
        return 0;
    }
}

#define PC_INCREMENT vm->pc += 8
#define PC_INCREMENT_EXTENDED vm->pc += 16

gboolean gtk_ml_enop(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction *instr) {
    (void) err;
    (void) instr;

    PC_INCREMENT_EXTENDED;

    return 1;
}

gboolean gtk_ml_ia_nop(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_halt(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    vm->flags |= GTKML_F_HALT;
    PC_INCREMENT;
    return 1;
}

GTKML_PRIVATE void set_flags(GtkMl_Vm *vm, GtkMl_TaggedValue result) {
    // TODO(walterpi): overflow, carry
    if (gtk_ml_is_primitive(result)) {
        if ((result.tag & GTKML_TAG_INT) == GTKML_TAG_INT) { \
            if (result.value.s64 == 0) {
                vm->flags |= GTKML_F_ZERO;
            } else if (result.value.s64 < 0) {
                vm->flags |= GTKML_F_SIGN;
            }
        } else if (result.tag == GTKML_TAG_FLOAT) { \
            if (result.value.flt == 0.0) {
                vm->flags |= GTKML_F_ZERO;
            } else if (result.value.flt < 0.0) {
                vm->flags |= GTKML_F_SIGN;
            }
        }
    } else {
        if (result.value.sobj->kind == GTKML_S_INT) {
            if (result.value.sobj->value.s_int.value == 0) {
                vm->flags |= GTKML_F_ZERO;
            } else if (result.value.sobj->value.s_int.value < 0) {
                vm->flags |= GTKML_F_SIGN;
            }
        } else if (result.value.sobj->kind == GTKML_S_FLOAT) {
            if (result.value.sobj->value.s_float.value == 0.0) {
                vm->flags |= GTKML_F_ZERO;
            } else if (result.value.sobj->value.s_float.value < 0.0) {
                vm->flags |= GTKML_F_SIGN;
            }
        }
    }
}

#define gtk_ml_ia_signed_binary(name, operator) \
    gboolean gtk_ml_ia_signed_##name(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) { \
        (void) instr; \
        GtkMl_TaggedValue tv_lhs = gtk_ml_pop(vm->ctx); \
        GtkMl_TaggedValue tv_rhs = gtk_ml_pop(vm->ctx); \
        double flhs; \
        double frhs; \
        int64_t ilhs; \
        int64_t irhs; \
        gboolean sobj = gtk_ml_is_sobject(tv_lhs) || gtk_ml_is_sobject(tv_rhs); \
        gboolean flt_lhs; \
        gboolean flt_rhs; \
        if (gtk_ml_is_primitive(tv_lhs)) { \
            if ((tv_lhs.tag & GTKML_TAG_INT) == GTKML_TAG_INT) { \
                ilhs = tv_lhs.value.s64; \
                flt_lhs = 0; \
            } else if (tv_lhs.tag == GTKML_TAG_FLOAT) { \
                flhs = tv_lhs.value.flt; \
                flt_lhs = 1; \
            } else { \
                *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0); \
                return 0; \
            } \
        } else if (tv_lhs.value.sobj->kind == GTKML_S_INT) { \
            ilhs = tv_lhs.value.sobj->value.s_int.value; \
            flt_lhs = 0; \
        } else if (tv_lhs.value.sobj->kind == GTKML_S_FLOAT) { \
            flhs = tv_lhs.value.sobj->value.s_float.value; \
            flt_lhs = 1; \
        } else { \
            *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0); \
            return 0; \
        } \
        if (gtk_ml_is_primitive(tv_rhs)) { \
            if ((tv_rhs.tag & GTKML_TAG_INT) == GTKML_TAG_INT) { \
                irhs = tv_rhs.value.s64; \
                flt_rhs = 0; \
            } else if (tv_rhs.tag == GTKML_TAG_FLOAT) { \
                frhs = tv_rhs.value.flt; \
                flt_rhs = 1; \
            } else { \
                *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0); \
                return 0; \
            } \
        } else if (tv_rhs.value.sobj->kind == GTKML_S_INT) { \
            irhs = tv_rhs.value.sobj->value.s_int.value; \
            flt_rhs = 0; \
        } else if (tv_rhs.value.sobj->kind == GTKML_S_FLOAT) { \
            frhs = tv_rhs.value.sobj->value.s_float.value; \
            flt_rhs = 1; \
        } else { \
            *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0); \
            return 0; \
        } \
        if (!flt_lhs && !flt_rhs) { \
            if (sobj) { \
                gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_int(vm->ctx, NULL, ilhs operator irhs))); \
            } else { \
                gtk_ml_push(vm->ctx, gtk_ml_value_int(ilhs operator irhs)); \
            } \
        } else if (!flt_lhs && flt_rhs) { \
            if (sobj) { \
                gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_float(vm->ctx, NULL, (double) ilhs operator frhs))); \
            } else { \
                gtk_ml_push(vm->ctx, gtk_ml_value_float((double) ilhs operator frhs)); \
            } \
        } else if (flt_lhs && !flt_rhs) { \
            if (sobj) { \
                gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_float(vm->ctx, NULL, flhs operator (double) irhs))); \
            } else { \
                gtk_ml_push(vm->ctx, gtk_ml_value_float(flhs operator (double) irhs)); \
            } \
        } else if (flt_lhs && flt_rhs) { \
            if (sobj) { \
                gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_float(vm->ctx, NULL, flhs operator frhs))); \
            } else { \
                gtk_ml_push(vm->ctx, gtk_ml_value_float(flhs operator frhs)); \
            } \
        } else { \
            *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 2); \
            return 0; \
        } \
        set_flags(vm, gtk_ml_peek(vm->ctx)); \
        PC_INCREMENT; \
        return 1; \
    }

#define gtk_ml_ia_unsigned_binary(name, operator) \
    gboolean gtk_ml_ia_unsigned_##name(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) { \
        (void) instr; \
        GtkMl_TaggedValue tv_lhs = gtk_ml_pop(vm->ctx); \
        GtkMl_TaggedValue tv_rhs = gtk_ml_pop(vm->ctx); \
        double flhs; \
        double frhs; \
        uint64_t ilhs; \
        uint64_t irhs; \
        gboolean sobj = gtk_ml_is_sobject(tv_lhs) || gtk_ml_is_sobject(tv_rhs); \
        gboolean flt_lhs; \
        gboolean flt_rhs; \
        if (gtk_ml_is_primitive(tv_lhs)) { \
            if ((tv_lhs.tag & GTKML_TAG_INT) == GTKML_TAG_INT) { \
                ilhs = tv_lhs.value.s64; \
                flt_lhs = 0; \
            } else if (tv_lhs.tag == GTKML_TAG_FLOAT) { \
                flhs = tv_lhs.value.flt; \
                flt_lhs = 1; \
            } else { \
                *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0); \
                return 0; \
            } \
        } else if (tv_lhs.value.sobj->kind == GTKML_S_INT) { \
            ilhs = tv_lhs.value.sobj->value.s_int.value; \
            flt_lhs = 0; \
        } else if (tv_lhs.value.sobj->kind == GTKML_S_FLOAT) { \
            flhs = tv_lhs.value.sobj->value.s_float.value; \
            flt_lhs = 1; \
        } else { \
            *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0); \
            return 0; \
        } \
        if (gtk_ml_is_primitive(tv_rhs)) { \
            if ((tv_rhs.tag & GTKML_TAG_INT) == GTKML_TAG_INT) { \
                irhs = tv_rhs.value.s64; \
                flt_rhs = 0; \
            } else if (tv_rhs.tag == GTKML_TAG_FLOAT) { \
                frhs = tv_rhs.value.flt; \
                flt_rhs = 1; \
            } else { \
                *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0); \
                return 0; \
            } \
        } else if (tv_rhs.value.sobj->kind == GTKML_S_INT) { \
            irhs = tv_rhs.value.sobj->value.s_int.value; \
            flt_rhs = 0; \
        } else if (tv_rhs.value.sobj->kind == GTKML_S_FLOAT) { \
            frhs = tv_rhs.value.sobj->value.s_float.value; \
            flt_rhs = 1; \
        } else { \
            *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0); \
            return 0; \
        } \
        if (!flt_lhs && !flt_rhs) { \
            if (sobj) { \
                gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_int(vm->ctx, NULL, ilhs operator irhs))); \
            } else { \
                gtk_ml_push(vm->ctx, gtk_ml_value_int(ilhs operator irhs)); \
            } \
        } else if (!flt_lhs && flt_rhs) { \
            if (sobj) { \
                gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_float(vm->ctx, NULL, (double) ilhs operator frhs))); \
            } else { \
                gtk_ml_push(vm->ctx, gtk_ml_value_float((double) ilhs operator frhs)); \
            } \
        } else if (flt_lhs && !flt_rhs) { \
            if (sobj) { \
                gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_float(vm->ctx, NULL, flhs operator (double) irhs))); \
            } else { \
                gtk_ml_push(vm->ctx, gtk_ml_value_float(flhs operator (double) irhs)); \
            } \
        } else if (flt_lhs && flt_rhs) { \
            if (sobj) { \
                gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_float(vm->ctx, NULL, flhs operator frhs))); \
            } else { \
                gtk_ml_push(vm->ctx, gtk_ml_value_float(flhs operator frhs)); \
            } \
        } else { \
            *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0); \
            return 0; \
        } \
        set_flags(vm, gtk_ml_peek(vm->ctx)); \
        PC_INCREMENT; \
        return 1; \
    }

#define gtk_ml_ia_signed_binaryf(name, operator, fn) \
    gboolean gtk_ml_ia_signed_##name(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) { \
        (void) instr; \
        GtkMl_TaggedValue tv_lhs = gtk_ml_pop(vm->ctx); \
        GtkMl_TaggedValue tv_rhs = gtk_ml_pop(vm->ctx); \
        double flhs; \
        double frhs; \
        int64_t ilhs; \
        int64_t irhs; \
        gboolean sobj = gtk_ml_is_sobject(tv_lhs) || gtk_ml_is_sobject(tv_rhs); \
        gboolean flt_lhs; \
        gboolean flt_rhs; \
        if (gtk_ml_is_primitive(tv_lhs)) { \
            if ((tv_lhs.tag & GTKML_TAG_INT) == GTKML_TAG_INT) { \
                ilhs = tv_lhs.value.s64; \
                flt_lhs = 0; \
            } else if (tv_lhs.tag == GTKML_TAG_FLOAT) { \
                flhs = tv_lhs.value.flt; \
                flt_lhs = 1; \
            } else { \
                *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0); \
                return 0; \
            } \
        } else if (tv_lhs.value.sobj->kind == GTKML_S_INT) { \
            ilhs = tv_lhs.value.sobj->value.s_int.value; \
            flt_lhs = 0; \
        } else if (tv_lhs.value.sobj->kind == GTKML_S_FLOAT) { \
            flhs = tv_lhs.value.sobj->value.s_float.value; \
            flt_lhs = 1; \
        } else { \
            *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0); \
            return 0; \
        } \
        if (gtk_ml_is_primitive(tv_rhs)) { \
            if ((tv_rhs.tag & GTKML_TAG_INT) == GTKML_TAG_INT) { \
                irhs = tv_rhs.value.s64; \
                flt_rhs = 0; \
            } else if (tv_rhs.tag == GTKML_TAG_FLOAT) { \
                frhs = tv_rhs.value.flt; \
                flt_rhs = 1; \
            } else { \
                *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0); \
                return 0; \
            } \
        } else if (tv_rhs.value.sobj->kind == GTKML_S_INT) { \
            irhs = tv_rhs.value.sobj->value.s_int.value; \
            flt_rhs = 0; \
        } else if (tv_rhs.value.sobj->kind == GTKML_S_FLOAT) { \
            frhs = tv_rhs.value.sobj->value.s_float.value; \
            flt_rhs = 1; \
        } else { \
            *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0); \
            return 0; \
        } \
        if (!flt_lhs && !flt_rhs) { \
            if (sobj) { \
                gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_int(vm->ctx, NULL, ilhs operator irhs))); \
            } else { \
                gtk_ml_push(vm->ctx, gtk_ml_value_int(ilhs operator irhs)); \
            } \
        } else if (!flt_lhs && flt_rhs) { \
            if (sobj) { \
                gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_float(vm->ctx, NULL, fn((double) ilhs, frhs)))); \
            } else { \
                gtk_ml_push(vm->ctx, gtk_ml_value_float(fn((double) ilhs, frhs))); \
            } \
        } else if (flt_lhs && !flt_rhs) { \
            if (sobj) { \
                gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_float(vm->ctx, NULL, fn(flhs, (double) irhs)))); \
            } else { \
                gtk_ml_push(vm->ctx, gtk_ml_value_float(fn(flhs, (double) irhs))); \
            } \
        } else if (flt_lhs && flt_rhs) { \
            if (sobj) { \
                gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_float(vm->ctx, NULL, fn(flhs, frhs)))); \
            } else { \
                gtk_ml_push(vm->ctx, gtk_ml_value_float(fn(flhs, frhs))); \
            } \
        } else { \
            *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 2); \
            return 0; \
        } \
        set_flags(vm, gtk_ml_peek(vm->ctx)); \
        PC_INCREMENT; \
        return 1; \
    }

#define gtk_ml_ia_unsigned_binaryf(name, operator, fn) \
    gboolean gtk_ml_ia_unsigned_##name(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) { \
        (void) instr; \
        GtkMl_TaggedValue tv_lhs = gtk_ml_pop(vm->ctx); \
        GtkMl_TaggedValue tv_rhs = gtk_ml_pop(vm->ctx); \
        double flhs; \
        double frhs; \
        uint64_t ilhs; \
        uint64_t irhs; \
        gboolean sobj = gtk_ml_is_sobject(tv_lhs) || gtk_ml_is_sobject(tv_rhs); \
        gboolean flt_lhs; \
        gboolean flt_rhs; \
        if (gtk_ml_is_primitive(tv_lhs)) { \
            if ((tv_lhs.tag & GTKML_TAG_INT) == GTKML_TAG_INT) { \
                ilhs = tv_lhs.value.s64; \
                flt_lhs = 0; \
            } else if (tv_lhs.tag == GTKML_TAG_FLOAT) { \
                flhs = tv_lhs.value.flt; \
                flt_lhs = 1; \
            } else { \
                *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0); \
                return 0; \
            } \
        } else if (tv_lhs.value.sobj->kind == GTKML_S_INT) { \
            ilhs = tv_lhs.value.sobj->value.s_int.value; \
            flt_lhs = 0; \
        } else if (tv_lhs.value.sobj->kind == GTKML_S_FLOAT) { \
            flhs = tv_lhs.value.sobj->value.s_float.value; \
            flt_lhs = 1; \
        } else { \
            *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0); \
            return 0; \
        } \
        if (gtk_ml_is_primitive(tv_rhs)) { \
            if ((tv_rhs.tag & GTKML_TAG_INT) == GTKML_TAG_INT) { \
                irhs = tv_rhs.value.s64; \
                flt_rhs = 0; \
            } else if (tv_rhs.tag == GTKML_TAG_FLOAT) { \
                frhs = tv_rhs.value.flt; \
                flt_rhs = 1; \
            } else { \
                *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0); \
                return 0; \
            } \
        } else if (tv_rhs.value.sobj->kind == GTKML_S_INT) { \
            irhs = tv_rhs.value.sobj->value.s_int.value; \
            flt_rhs = 0; \
        } else if (tv_rhs.value.sobj->kind == GTKML_S_FLOAT) { \
            frhs = tv_rhs.value.sobj->value.s_float.value; \
            flt_rhs = 1; \
        } else { \
            *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0); \
            return 0; \
        } \
        if (!flt_lhs && !flt_rhs) { \
            if (sobj) { \
                gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_int(vm->ctx, NULL, ilhs operator irhs))); \
            } else { \
                gtk_ml_push(vm->ctx, gtk_ml_value_int(ilhs operator irhs)); \
            } \
        } else if (!flt_lhs && flt_rhs) { \
            if (sobj) { \
                gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_float(vm->ctx, NULL, fn((double) ilhs, frhs)))); \
            } else { \
                gtk_ml_push(vm->ctx, gtk_ml_value_float(fn((double) ilhs, frhs))); \
            } \
        } else if (flt_lhs && !flt_rhs) { \
            if (sobj) { \
                gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_float(vm->ctx, NULL, fn(flhs, (double) irhs)))); \
            } else { \
                gtk_ml_push(vm->ctx, gtk_ml_value_float(fn(flhs, (double) irhs))); \
            } \
        } else if (flt_lhs && flt_rhs) { \
            if (sobj) { \
                gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_float(vm->ctx, NULL, fn(flhs, frhs)))); \
            } else { \
                gtk_ml_push(vm->ctx, gtk_ml_value_float(fn(flhs, frhs))); \
            } \
        } else { \
            *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 2); \
            return 0; \
        } \
        set_flags(vm, gtk_ml_peek(vm->ctx)); \
        PC_INCREMENT; \
        return 1; \
    }

#define gtk_ml_ia_bitwise(name, operation) \
    gboolean gtk_ml_ia_bit_##name(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) { \
        (void) instr; \
        GtkMl_TaggedValue tv_lhs = gtk_ml_pop(vm->ctx); \
        GtkMl_TaggedValue tv_rhs = gtk_ml_pop(vm->ctx); \
        uint64_t ilhs; \
        uint64_t irhs; \
        gboolean sobj = gtk_ml_is_sobject(tv_lhs) || gtk_ml_is_sobject(tv_rhs); \
        if (gtk_ml_is_primitive(tv_lhs)) { \
            if ((tv_lhs.tag & GTKML_TAG_INT) == GTKML_TAG_INT) { \
                ilhs = tv_lhs.value.s64; \
            } else { \
                *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0); \
                return 0; \
            } \
        } else if (tv_lhs.value.sobj->kind == GTKML_S_INT) { \
            ilhs = tv_lhs.value.sobj->value.s_int.value; \
        } else { \
            *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0); \
            return 0; \
        } \
        if (gtk_ml_is_primitive(tv_rhs)) { \
            if ((tv_rhs.tag & GTKML_TAG_INT) == GTKML_TAG_INT) { \
                irhs = tv_rhs.value.s64; \
            } else { \
                *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0); \
                return 0; \
            } \
        } else if (tv_rhs.value.sobj->kind == GTKML_S_INT) { \
            irhs = tv_rhs.value.sobj->value.s_int.value; \
        } else { \
            *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0); \
            return 0; \
        } \
        if (sobj) { \
            gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_int(vm->ctx, NULL, operation))); \
        } else { \
            gtk_ml_push(vm->ctx, gtk_ml_value_int(operation)); \
        } \
        set_flags(vm, gtk_ml_peek(vm->ctx)); \
        PC_INCREMENT; \
        return 1; \
    }

gtk_ml_ia_signed_binary(add, +)
gtk_ml_ia_signed_binary(subtract, -)
gtk_ml_ia_signed_binary(multiply, *)
gtk_ml_ia_unsigned_binary(multiply, *)
gtk_ml_ia_signed_binary(divide, /)
gtk_ml_ia_unsigned_binary(divide, /)
gtk_ml_ia_signed_binaryf(modulo, %, fmod)
gtk_ml_ia_unsigned_binaryf(modulo, %, fmod)
gtk_ml_ia_bitwise(and, ilhs & irhs)
gtk_ml_ia_bitwise(or, ilhs | irhs)
gtk_ml_ia_bitwise(xor, ilhs ^ irhs)
gtk_ml_ia_bitwise(nand, ~(ilhs & irhs))
gtk_ml_ia_bitwise(nor, ~(ilhs | irhs))
gtk_ml_ia_bitwise(xnor, ~(ilhs ^ irhs))

gboolean gtk_ml_ia_car(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_SObj list = gtk_ml_pop(vm->ctx).value.sobj;
    if (list->kind != GTKML_S_LIST) {
        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
                gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "list", strlen("list")),
                gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), list);
        *err = error;
        return 0;
    }
    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_car(list)));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_cdr(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_SObj list = gtk_ml_pop(vm->ctx).value.sobj;
    if (list->kind != GTKML_S_LIST) {
        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
                gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "list", strlen("list")),
                gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), list);
        *err = error;
        return 0;
    }
    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_cdr(list)));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_bind(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_SObj key = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_TaggedValue value = gtk_ml_pop(vm->ctx);
    gtk_ml_bind(vm->ctx, key, value);
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_enter(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    gtk_ml_enter(vm->ctx);
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_leave(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    if (vm->ctx->bindings->value.s_var.expr->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(vm->ctx, "scope-error", GTKML_ERR_SCOPE_ERROR, 0, 0, 0, 0);
        return 0;
    }

    gtk_ml_leave(vm->ctx);
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_unwrap(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_SObj list = gtk_ml_pop(vm->ctx).value.sobj;
    size_t n = 0;
    while (list->kind != GTKML_S_NIL) {
        ++n;
        gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_car(list)));
        list = gtk_ml_cdr(list);
    }
    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_int(vm->ctx, NULL, n)));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_typeof(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_SObj value = gtk_ml_pop(vm->ctx).value.sobj;
    if (value->kind == GTKML_S_ARRAY) {
        if (gtk_ml_array_trie_is_string(&value->value.s_array.array)) {
            gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_keyword(vm->ctx, NULL, 0, "string", strlen("string"))));
        } else {
            gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_keyword(vm->ctx, NULL, 0, TYPENAME[value->kind], strlen(TYPENAME[value->kind]))));
        }
    } else {
        gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_keyword(vm->ctx, NULL, 0, TYPENAME[value->kind], strlen(TYPENAME[value->kind]))));
    }
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_to_sobj(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_TaggedValue value = gtk_ml_pop(vm->ctx);
    switch (value.tag) {
    case GTKML_TAG_NIL:
        gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_nil(vm->ctx, NULL)));
        break;
    case GTKML_TAG_BOOL:
        gtk_ml_push(vm->ctx, gtk_ml_value_sobject(value.value.boolean? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL)));
        break;
    case GTKML_TAG_CHAR:
        gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_char(vm->ctx, NULL, value.value.unicode)));
        break;
    case GTKML_TAG_INT64:
    case GTKML_TAG_UINT64:
        gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_int(vm->ctx, NULL, value.value.s64)));
        break;
    case GTKML_TAG_FLOAT:
        gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_float(vm->ctx, NULL, value.value.flt)));
        break;
    case GTKML_TAG_USERDATA:
        gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_lightdata(vm->ctx, NULL, value.value.userdata)));
        break;
    default:
        *err = gtk_ml_error(vm->ctx, "primitive-error", GTKML_ERR_TAG_ERROR, 0, 0, 0, 0);
        return 0;
    }
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_to_prim(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_SObj value = gtk_ml_pop(vm->ctx).value.sobj;
    switch (value->kind) {
    case GTKML_S_NIL:
        gtk_ml_push(vm->ctx, gtk_ml_value_nil());
        break;
    case GTKML_S_TRUE:
        gtk_ml_push(vm->ctx, gtk_ml_value_true());
        break;
    case GTKML_S_FALSE:
        gtk_ml_push(vm->ctx, gtk_ml_value_false());
        break;
    case GTKML_S_CHAR:
        gtk_ml_push(vm->ctx, gtk_ml_value_char(value->value.s_char.value));
        break;
    case GTKML_S_INT:
        gtk_ml_push(vm->ctx, gtk_ml_value_int(value->value.s_int.value));
        break;
    case GTKML_S_FLOAT:
        gtk_ml_push(vm->ctx, gtk_ml_value_float(value->value.s_float.value));
        break;
    case GTKML_S_LIGHTDATA:
        gtk_ml_push(vm->ctx, gtk_ml_value_userdata(value->value.s_lightdata.userdata));
        break;
    default:
        *err = gtk_ml_error(vm->ctx, "primitive-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 0);
        return 0;
    }
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_define(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_SObj key = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_TaggedValue value = gtk_ml_pop(vm->ctx);
    gtk_ml_define(vm->ctx, key, value);
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_bind_args(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_SObj revkeys = gtk_ml_pop(vm->ctx).value.sobj;
    size_t n = gtk_ml_pop(vm->ctx).value.sobj->value.s_int.value;

    GtkMl_SObj _revkeys = revkeys;
    size_t n_params = 0;
    while (revkeys->kind != GTKML_S_NIL) {
        ++n_params;
        revkeys = gtk_ml_cdr(revkeys);
    }
    revkeys = _revkeys;

    for (size_t i = 0; i < n_params; i++) {
        GtkMl_SObj key = gtk_ml_car(revkeys);
        if (key->kind == GTKML_S_VARARG) {
            GtkMl_SObj rest = gtk_ml_new_nil(vm->ctx, NULL);
            for (size_t j = 0; j < n - (n_params - 1); j++) {
                rest = gtk_ml_new_list(vm->ctx, NULL, gtk_ml_pop(vm->ctx).value.sobj, rest);
            }
            gtk_ml_bind(vm->ctx, key->value.s_vararg.expr, gtk_ml_value_sobject(rest));
        } else {
            GtkMl_TaggedValue value = gtk_ml_pop(vm->ctx);
            gtk_ml_bind(vm->ctx, key, value);
        }
        revkeys = gtk_ml_cdr(revkeys);
    }

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_list(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    int64_t n = gtk_ml_pop(vm->ctx).value.sobj->value.s_int.value;

    GtkMl_SObj list = gtk_ml_new_nil(vm->ctx, NULL);

    while (n--) {
        GtkMl_SObj expr = gtk_ml_pop(vm->ctx).value.sobj;
        list = gtk_ml_new_list(vm->ctx, NULL, expr, list);
    }

    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(list));

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_push_imm(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(vm->program->statics[instr.imm.imm]));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_setf_imm(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    switch (vm->program->statics[instr.imm.imm]->kind) {
    case GTKML_S_TRUE:
        vm->flags |= GTKML_F_ZERO;
        break;
    case GTKML_S_FALSE:
        vm->flags &= ~GTKML_F_ZERO;
        break;
    default:
        *err = gtk_ml_error(vm->ctx, "boolean-error", GTKML_ERR_BOOLEAN_ERROR, 0, 0, 0, 0);
        return 0;
    }
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_popf(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    switch (gtk_ml_pop(vm->ctx).value.sobj->kind) {
    case GTKML_S_TRUE:
        vm->flags |= GTKML_F_ZERO;
        break;
    case GTKML_S_FALSE:
        vm->flags &= ~GTKML_F_ZERO;
        break;
    default:
        *err = gtk_ml_error(vm->ctx, "boolean-error", GTKML_ERR_BOOLEAN_ERROR, 0, 0, 0, 0);
        return 0;
    }
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_pop(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    gtk_ml_pop(vm->ctx);
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_get_imm(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_TaggedValue value = gtk_ml_get(vm->ctx, vm->program->statics[instr.imm.imm]);
    if (gtk_ml_has_value(value)) {
        gtk_ml_push(vm->ctx, value);
    } else {
        GtkMl_SObj error = gtk_ml_error(vm->ctx, "binding-error", GTKML_ERR_BINDING_ERROR, 0, 0, 0, 1, gtk_ml_new_keyword(vm->ctx, NULL, 0, "binding", strlen("binding")), vm->program->statics[instr.imm.imm]);
        *err = error;
        return 0;
    }
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_list_imm(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;

    int64_t n = vm->program->statics[instr.imm.imm]->value.s_int.value;

    GtkMl_SObj list = gtk_ml_new_nil(vm->ctx, NULL);

    while (n--) {
        GtkMl_SObj expr = gtk_ml_pop(vm->ctx).value.sobj;
        list = gtk_ml_new_list(vm->ctx, NULL, expr, list);
    }

    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(list));

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_map_imm(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;

    int64_t n = vm->program->statics[instr.imm.imm]->value.s_int.value;

    GtkMl_SObj map = gtk_ml_new_map(vm->ctx, NULL, NULL);

    while (n--) {
        GtkMl_TaggedValue value = gtk_ml_pop(vm->ctx);
        GtkMl_TaggedValue key = gtk_ml_pop(vm->ctx);
        GtkMl_SObj new = gtk_ml_new_map(vm->ctx, NULL, NULL);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &map->value.s_map.map, key, value);
        map = new;
    }

    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(map));

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_set_imm(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;

    int64_t n = vm->program->statics[instr.imm.imm]->value.s_int.value;

    GtkMl_SObj set = gtk_ml_new_set(vm->ctx, NULL);

    while (n--) {
        GtkMl_TaggedValue key = gtk_ml_pop(vm->ctx);
        GtkMl_SObj new = gtk_ml_new_set(vm->ctx, NULL);
        gtk_ml_hash_set_insert(&new->value.s_set.set, &set->value.s_set.set, key);
        set = new;
    }

    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(set));

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_array_imm(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;

    int64_t n = vm->program->statics[instr.imm.imm]->value.s_int.value;

    GtkMl_SObj array = gtk_ml_new_array(vm->ctx, NULL);

    while (n--) {
        GtkMl_TaggedValue value = gtk_ml_pop(vm->ctx);
        GtkMl_SObj new = gtk_ml_new_array(vm->ctx, NULL);
        gtk_ml_array_trie_push(&new->value.s_array.array, &array->value.s_array.array, value);
        array = new;
    }

    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(array));

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_setmm_imm(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_SObj metamap = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_SObj map = gtk_ml_pop(vm->ctx).value.sobj;

    GtkMl_SObj result = gtk_ml_new_map(vm->ctx, NULL, metamap);
    gtk_ml_del_hash_trie(vm->ctx, &result->value.s_map.map, gtk_ml_delete_value);
    gtk_ml_hash_trie_copy(&result->value.s_map.map, &map->value.s_map.map);

    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(result));

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_getmm_imm(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_SObj map = gtk_ml_pop(vm->ctx).value.sobj;

    GtkMl_SObj result = gtk_ml_getmetamap(map);
    if (!result) {
        *err = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 0);
        return 0;
    }
    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(result));

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_var_imm(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_SObj value = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_SObj result = gtk_ml_new_var(vm->ctx, NULL, value);
    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(result));

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_getvar_imm(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_SObj var = gtk_ml_pop(vm->ctx).value.sobj;
    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(var->value.s_var.expr));

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_assignvar_imm(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_SObj newvalue = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_SObj var = gtk_ml_pop(vm->ctx).value.sobj;
    var->value.s_var.expr = newvalue;
    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(var));

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_len(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_SObj container = gtk_ml_pop(vm->ctx).value.sobj;
    switch (container->kind) {
    case GTKML_S_ARRAY:
        gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_int(vm->ctx, NULL, gtk_ml_array_trie_len(&container->value.s_array.array))));
        break;
    case GTKML_S_MAP:
        gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_int(vm->ctx, NULL, gtk_ml_hash_trie_len(&container->value.s_map.map))));
        break;
    case GTKML_S_SET:
        gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_int(vm->ctx, NULL, gtk_ml_hash_set_len(&container->value.s_set.set))));
        break;
    default:
        *err = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_CONTAINER_ERROR, 0, 0, 0, 0);
        return 0;
    }

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_array_index(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_SObj container = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_SObj index = gtk_ml_pop(vm->ctx).value.sobj;

    if (index->kind != GTKML_S_INT) {
        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "int", strlen("int")),
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), index);
        *err = error;
        return 0;
    }

    switch (container->kind) {
    case GTKML_S_ARRAY: {
        GtkMl_TaggedValue opt = gtk_ml_array_trie_get(&container->value.s_array.array, index->value.s_int.value);
        if (gtk_ml_has_value(opt)) {
            gtk_ml_push(vm->ctx, opt);
        } else {
            size_t len = gtk_ml_array_trie_len(&container->value.s_array.array);
            GtkMl_SObj error = gtk_ml_error(vm->ctx, "index-out-of-bounds", GTKML_ERR_INDEX_ERROR, 0, 0, 0, 2,
                gtk_ml_new_keyword(vm->ctx, NULL, 0, "len", strlen("len")), gtk_ml_new_int(vm->ctx, NULL, len),
                gtk_ml_new_keyword(vm->ctx, NULL, 0, "got", strlen("got")), index);
            *err = error;
            return 0;
        }
        break;
    }
    default: {
        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "array", strlen("array")),
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), container);
        *err = error;
        return 0;
    }
    }

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_array_push(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_SObj container = gtk_ml_pop(vm->ctx).value.sobj;

    if (container->kind != GTKML_S_ARRAY) {
        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "array", strlen("array")),
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), container);
        *err = error;
        return 0;
    }

    GtkMl_SObj result = gtk_ml_new_array(vm->ctx, NULL);
    gtk_ml_del_array_trie(vm->ctx, &result->value.s_array.array, gtk_ml_delete_value);
    GtkMl_TaggedValue elem = gtk_ml_pop(vm->ctx);

    gtk_ml_array_trie_push(&result->value.s_array.array, &container->value.s_array.array, elem);
    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(result));

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_array_pop(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_SObj container = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_SObj result = gtk_ml_new_array(vm->ctx, NULL);
    gtk_ml_del_array_trie(vm->ctx, &result->value.s_array.array, gtk_ml_delete_value);

    switch (container->kind) {
    case GTKML_S_ARRAY:
        gtk_ml_array_trie_pop(&result->value.s_array.array, &container->value.s_array.array);
        gtk_ml_push(vm->ctx, gtk_ml_value_sobject(result));
        break;
    default: {
        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "array", strlen("array")),
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), container);
        *err = error;
        return 0;
    }
    }

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_array_concat(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_SObj a1 = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_SObj a2 = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_SObj result = gtk_ml_new_array(vm->ctx, NULL);
    gtk_ml_del_array_trie(vm->ctx, &result->value.s_array.array, gtk_ml_delete_value);

    switch (a1->kind) {
    case GTKML_S_ARRAY:
        switch (a2->kind) {
        case GTKML_S_ARRAY:
            gtk_ml_array_trie_concat(&result->value.s_array.array, &a1->value.s_array.array, &a2->value.s_array.array);
            gtk_ml_push(vm->ctx, gtk_ml_value_sobject(result));
            break;
        default: {
            GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
                gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "array", strlen("array")),
                gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), a2);
            *err = error;
            return 0;
        }
        }
        break;
    default: {
        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "array", strlen("array")),
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), a1);
        *err = error;
        return 0;
    }
    }

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_map_get(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_SObj container = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_SObj key = gtk_ml_pop(vm->ctx).value.sobj;

    switch (container->kind) {
    case GTKML_S_MAP: {
        GtkMl_TaggedValue opt = gtk_ml_hash_trie_get(&container->value.s_map.map, gtk_ml_value_sobject(key));
        if (gtk_ml_has_value(opt)) {
            gtk_ml_push(vm->ctx, opt);
        } else {
            GtkMl_SObj error = gtk_ml_error(vm->ctx, "index-out-of-bounds", GTKML_ERR_INDEX_ERROR, 0, 0, 0, 1,
                gtk_ml_new_keyword(vm->ctx, NULL, 0, "key", strlen("key")), key);
            *err = error;
            return 0;
        }
        break;
    }
    default: {
        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "map", strlen("map")),
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), container);
        *err = error;
        return 0;
    }
    }

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_map_insert(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_SObj container = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_SObj key = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_SObj value = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_SObj result = gtk_ml_new_map(vm->ctx, NULL, container->value.s_map.metamap);
    gtk_ml_del_hash_trie(vm->ctx, &result->value.s_map.map, gtk_ml_delete_value);

    switch (container->kind) {
    case GTKML_S_ARRAY:
        gtk_ml_hash_trie_insert(&result->value.s_map.map, &container->value.s_map.map, gtk_ml_value_sobject(key), gtk_ml_value_sobject(value));
        gtk_ml_push(vm->ctx, gtk_ml_value_sobject(result));
        break;
    default: {
        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "map", strlen("map")),
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), container);
        *err = error;
        return 0;
    }
    }

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_map_delete(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_SObj container = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_SObj key = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_SObj result = gtk_ml_new_map(vm->ctx, NULL, container->value.s_map.metamap);
    gtk_ml_del_hash_trie(vm->ctx, &result->value.s_map.map, gtk_ml_delete_value);

    switch (container->kind) {
    case GTKML_S_ARRAY:
        gtk_ml_hash_trie_delete(&result->value.s_map.map, &container->value.s_map.map, gtk_ml_value_sobject(key));
        gtk_ml_push(vm->ctx, gtk_ml_value_sobject(result));
        break;
    default: {
        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "map", strlen("map")),
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), container);
        *err = error;
        return 0;
    }
    }

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_set_contains(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_SObj container = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_SObj key = gtk_ml_pop(vm->ctx).value.sobj;

    switch (container->kind) {
    case GTKML_S_SET:
        gtk_ml_push(vm->ctx, gtk_ml_hash_set_contains(&container->value.s_set.set, gtk_ml_value_sobject(key))?
                gtk_ml_value_sobject(gtk_ml_new_true(vm->ctx, NULL)) : gtk_ml_value_sobject(gtk_ml_new_false(vm->ctx, NULL)));
        break;
    default: {
        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "set", strlen("set")),
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), container);
        *err = error;
        return 0;
    }
    }

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_set_insert(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_SObj container = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_SObj key = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_SObj result = gtk_ml_new_set(vm->ctx, NULL);
    gtk_ml_del_hash_set(vm->ctx, &result->value.s_set.set, gtk_ml_delete_value);

    switch (container->kind) {
    case GTKML_S_ARRAY:
        gtk_ml_hash_set_insert(&result->value.s_set.set, &container->value.s_set.set, gtk_ml_value_sobject(key));
        gtk_ml_push(vm->ctx, gtk_ml_value_sobject(result));
        break;
    default: {
        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "set", strlen("set")),
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), container);
        *err = error;
        return 0;
    }
    }

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_set_delete(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_SObj container = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_SObj key = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_SObj result = gtk_ml_new_set(vm->ctx, NULL);
    gtk_ml_del_hash_set(vm->ctx, &result->value.s_set.set, gtk_ml_delete_value);

    switch (container->kind) {
    case GTKML_S_ARRAY:
        gtk_ml_hash_set_delete(&result->value.s_set.set, &container->value.s_set.set, gtk_ml_value_sobject(key));
        gtk_ml_push(vm->ctx, gtk_ml_value_sobject(result));
        break;
    default: {
        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "set", strlen("set")),
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), container);
        *err = error;
        return 0;
    }
    }

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ibr_call(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_SObj program = gtk_ml_pop(vm->ctx).value.sobj;

    if (vm->call_stack_ptr == vm->call_stack_cap) {
        *err = gtk_ml_error(vm->ctx, "stack-overflow", GTKML_ERR_STACK_ERROR, 0, 0, 0, 0);
        return 0;
    }

    uint64_t flags = vm->flags & GTKML_F_TOPCALL;
    vm->call_stack[vm->call_stack_ptr++] = flags;
    vm->flags &= ~GTKML_F_TOPCALL;

    PC_INCREMENT;
    uint64_t pc = vm->pc;
    vm->call_stack[vm->call_stack_ptr++] = pc;
    vm->pc = program->value.s_program.addr;

    gtk_ml_enter(vm->ctx);

    return 1;
}

gboolean gtk_ml_ibr_ret(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    if (vm->flags & GTKML_F_TOPCALL) {
        vm->flags |= GTKML_F_HALT;
        PC_INCREMENT;
    } else {
        if (vm->ctx->bindings->value.s_var.expr->kind == GTKML_S_NIL) {
            *err = gtk_ml_error(vm->ctx, "scope-error", GTKML_ERR_SCOPE_ERROR, 0, 0, 0, 0);
            return 0;
        }

        gtk_ml_leave(vm->ctx);

        uint64_t pc = vm->call_stack[--vm->call_stack_ptr];
        uint64_t flags = vm->call_stack[--vm->call_stack_ptr];
        vm->pc = pc;
        vm->flags |= flags;
    }

    return 1;
}

gboolean gtk_ml_ibr_call_std(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) instr;
    GtkMl_SObj (*function)(GtkMl_Context *, GtkMl_SObj *, GtkMl_SObj ) = vm->std[vm->program->statics[instr.br.imm]->value.s_int.value];
    GtkMl_SObj expr = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_SObj value = function(vm->ctx, err, expr);
    if (value) {
        gtk_ml_push(vm->ctx, gtk_ml_value_sobject(value));
    } else {
        return 0;
    }
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ibr_branch_absolute(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    vm->flags &= ~GTKML_F_GENERIC;
    GtkMl_SObj addr = vm->program->statics[instr.br.imm];
    vm->pc = addr->value.s_address.addr;
    return 1;
}

gboolean gtk_ml_ibr_branch_relative(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    vm->flags &= ~GTKML_F_GENERIC;
    GtkMl_SObj addr = vm->program->statics[instr.br.imm];
    vm->pc += 8 + addr->value.s_address.addr;
    return 1;
}

gboolean gtk_ml_eii_push_ext_imm(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr, GtkMl_SObj imm64) {
    (void) err;
    (void) instr;
    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(imm64));
    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eii_setf_ext_imm(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr, GtkMl_SObj imm64) {
    (void) instr;
    switch (imm64->kind) {
    case GTKML_S_TRUE:
        vm->flags |= GTKML_F_ZERO;
        break;
    case GTKML_S_FALSE:
        vm->flags &= ~GTKML_F_ZERO;
        break;
    default:
        *err = gtk_ml_error(vm->ctx, "boolean-error", GTKML_ERR_BOOLEAN_ERROR, 0, 0, 0, 0);
        return 0;
    }
    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eii_popf_ext(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr, GtkMl_SObj imm64) {
    (void) err;
    (void) instr;
    (void) imm64;
    switch (gtk_ml_pop(vm->ctx).value.sobj->kind) {
    case GTKML_S_TRUE:
        vm->flags |= GTKML_F_ZERO;
        break;
    case GTKML_S_FALSE:
        vm->flags &= ~GTKML_F_ZERO;
        break;
    default:
        *err = gtk_ml_error(vm->ctx, "boolean-error", GTKML_ERR_BOOLEAN_ERROR, 0, 0, 0, 0);
        return 0;
    }
    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eii_pop_ext(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr, GtkMl_SObj imm64) {
    (void) err;
    (void) instr;
    (void) imm64;
    gtk_ml_pop(vm->ctx);
    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eii_get_ext_imm(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr, GtkMl_SObj imm64) {
    (void) err;
    (void) instr;
    GtkMl_TaggedValue value = gtk_ml_get(vm->ctx, imm64);
    if (gtk_ml_has_value(value)) {
        gtk_ml_push(vm->ctx, value);
    } else {
        GtkMl_SObj error = gtk_ml_error(vm->ctx, "binding-error", GTKML_ERR_BINDING_ERROR, 0, 0, 0, 1,
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "binding", strlen("binding")), imm64);
        *err = error;
        return 0;
    }
    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eii_list_ext_imm(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr, GtkMl_SObj imm64) {
    (void) err;
    (void) instr;

    int64_t n = imm64->value.s_int.value;

    GtkMl_SObj list = gtk_ml_new_nil(vm->ctx, NULL);

    while (n--) {
        GtkMl_SObj expr = gtk_ml_pop(vm->ctx).value.sobj;
        list = gtk_ml_new_list(vm->ctx, NULL, expr, list);
    }

    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(list));

    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eii_map_ext_imm(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr, GtkMl_SObj imm64) {
    (void) err;
    (void) instr;

    int64_t n = imm64->value.s_int.value;

    GtkMl_SObj map = gtk_ml_new_map(vm->ctx, NULL, NULL);

    while (n--) {
        GtkMl_SObj key = gtk_ml_pop(vm->ctx).value.sobj;
        GtkMl_SObj value = gtk_ml_pop(vm->ctx).value.sobj;
        GtkMl_SObj new = gtk_ml_new_map(vm->ctx, NULL, NULL);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &map->value.s_map.map, gtk_ml_value_sobject(key), gtk_ml_value_sobject(value));
        map = new;
    }

    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(map));

    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eii_set_ext_imm(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr, GtkMl_SObj imm64) {
    (void) err;
    (void) instr;

    int64_t n = imm64->value.s_int.value;

    GtkMl_SObj set = gtk_ml_new_set(vm->ctx, NULL);

    while (n--) {
        GtkMl_SObj key = gtk_ml_pop(vm->ctx).value.sobj;
        GtkMl_SObj new = gtk_ml_new_set(vm->ctx, NULL);
        gtk_ml_hash_set_insert(&new->value.s_set.set, &set->value.s_set.set, gtk_ml_value_sobject(key));
        set = new;
    }

    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(set));

    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eii_array_ext_imm(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr, GtkMl_SObj imm64) {
    (void) err;
    (void) instr;

    int64_t n = imm64->value.s_int.value;

    GtkMl_SObj array = gtk_ml_new_array(vm->ctx, NULL);

    while (n--) {
        GtkMl_SObj value = gtk_ml_pop(vm->ctx).value.sobj;
        GtkMl_SObj new = gtk_ml_new_array(vm->ctx, NULL);
        gtk_ml_array_trie_push(&new->value.s_array.array, &array->value.s_array.array, gtk_ml_value_sobject(value));
        array = new;
    }

    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(array));

    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eii_cmp_ext_imm(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr, GtkMl_SObj imm64) {
    (void) err;
    (void) instr;

    GtkMl_Cmp cmp = imm64->value.s_int.value;

    GtkMl_SObj lhs = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_SObj rhs = gtk_ml_pop(vm->ctx).value.sobj;

    switch (cmp) {
    case GTKML_CMP_EQUAL:
        gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_equal(lhs, rhs)? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL)));
        break;
    case GTKML_CMP_NOT_EQUAL:
        gtk_ml_push(vm->ctx, gtk_ml_value_sobject((!gtk_ml_equal(lhs, rhs))? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL)));
        break;
    case GTKML_CMP_LESS:
        if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
            gtk_ml_push(vm->ctx, gtk_ml_value_sobject((lhs->value.s_int.value < rhs->value.s_int.value)? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL)));
        } else if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_FLOAT) {
            gtk_ml_push(vm->ctx, gtk_ml_value_sobject(((double) lhs->value.s_int.value < rhs->value.s_float.value)? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL)));
        } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_INT) {
            gtk_ml_push(vm->ctx, gtk_ml_value_sobject((lhs->value.s_float.value < (double) rhs->value.s_int.value)? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL)));
        } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_FLOAT) {
            gtk_ml_push(vm->ctx, gtk_ml_value_sobject((lhs->value.s_float.value < rhs->value.s_float.value)? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL)));
        } else {
            *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 2, gtk_ml_new_keyword(vm->ctx, NULL, 0, "lhs", strlen("lhs")), lhs, gtk_ml_new_keyword(vm->ctx, NULL, 0, "rhs", strlen("rhs")), rhs);
            return 0;
        }
        break;
    case GTKML_CMP_GREATER:
        if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
            gtk_ml_push(vm->ctx, gtk_ml_value_sobject((lhs->value.s_int.value > rhs->value.s_int.value)? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL)));
        } else if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_FLOAT) {
            gtk_ml_push(vm->ctx, gtk_ml_value_sobject(((double) lhs->value.s_int.value > rhs->value.s_float.value)? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL)));
        } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_INT) {
            gtk_ml_push(vm->ctx, gtk_ml_value_sobject((lhs->value.s_float.value > (double) rhs->value.s_int.value)? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL)));
        } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_FLOAT) {
            gtk_ml_push(vm->ctx, gtk_ml_value_sobject((lhs->value.s_float.value > rhs->value.s_float.value)? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL)));
        } else {
            *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 2, gtk_ml_new_keyword(vm->ctx, NULL, 0, "lhs", strlen("lhs")), lhs, gtk_ml_new_keyword(vm->ctx, NULL, 0, "rhs", strlen("rhs")), rhs);
            return 0;
        }
        break;
    case GTKML_CMP_LESS_EQUAL:
        if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
            gtk_ml_push(vm->ctx, gtk_ml_value_sobject((lhs->value.s_int.value <= rhs->value.s_int.value)? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL)));
        } else if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_FLOAT) {
            gtk_ml_push(vm->ctx, gtk_ml_value_sobject(((double) lhs->value.s_int.value <= rhs->value.s_float.value)? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL)));
        } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_INT) {
            gtk_ml_push(vm->ctx, gtk_ml_value_sobject((lhs->value.s_float.value <= (double) rhs->value.s_int.value)? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL)));
        } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_FLOAT) {
            gtk_ml_push(vm->ctx, gtk_ml_value_sobject((lhs->value.s_float.value <= rhs->value.s_float.value)? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL)));
        } else {
            *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 2, gtk_ml_new_keyword(vm->ctx, NULL, 0, "lhs", strlen("lhs")), lhs, gtk_ml_new_keyword(vm->ctx, NULL, 0, "rhs", strlen("rhs")), rhs);
            return 0;
        }
        break;
    case GTKML_CMP_GREATER_EQUAL:
        if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
            gtk_ml_push(vm->ctx, gtk_ml_value_sobject((lhs->value.s_int.value >= rhs->value.s_int.value)? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL)));
        } else if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_FLOAT) {
            gtk_ml_push(vm->ctx, gtk_ml_value_sobject(((double) lhs->value.s_int.value >= rhs->value.s_float.value)? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL)));
        } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_INT) {
            gtk_ml_push(vm->ctx, gtk_ml_value_sobject((lhs->value.s_float.value >= (double) rhs->value.s_int.value)? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL)));
        } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_FLOAT) {
            gtk_ml_push(vm->ctx, gtk_ml_value_sobject((lhs->value.s_float.value >= rhs->value.s_float.value)? gtk_ml_new_true(vm->ctx, NULL) : gtk_ml_new_false(vm->ctx, NULL)));
        } else {
            *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 2, gtk_ml_new_keyword(vm->ctx, NULL, 0, "lhs", strlen("lhs")), lhs, gtk_ml_new_keyword(vm->ctx, NULL, 0, "rhs", strlen("rhs")), rhs);
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

gboolean gtk_ml_eibr_call_ext_std(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr, GtkMl_SObj imm64) {
    (void) instr;
    GtkMl_SObj (*function)(GtkMl_Context *, GtkMl_SObj *, GtkMl_SObj ) = vm->std[imm64->value.s_int.value];
    GtkMl_SObj expr = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_SObj value = function(vm->ctx, err, expr);
    if (value) {
        gtk_ml_push(vm->ctx, gtk_ml_value_sobject(value));
    } else {
        return 0;
    }
    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eibr_branch_absolute_ext(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr, GtkMl_SObj imm64) {
    (void) err;
    (void) instr;
    vm->flags = 0;
    GtkMl_SObj addr = imm64;
    vm->pc = addr->value.s_address.addr;
    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eibr_branch_relative_ext(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Instruction instr, GtkMl_SObj imm64) {
    (void) err;
    (void) instr;
    vm->flags = 0;
    GtkMl_SObj addr = imm64;
    vm->pc += 8 + addr->value.s_address.addr;
    PC_INCREMENT_EXTENDED;
    return 1;
}
