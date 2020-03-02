#include <stdlib.h>
#include <gtk/gtk.h>
#include <math.h>
#include "gtk-ml.h"

struct GtkMl_Context {
    gboolean gc_enabled;
    size_t n_values;
    size_t m_values;
    GtkMl_S *first;
    GtkMl_S *last;

    GtkMl_S *bindings;
    GtkMl_S **top_scope;

    GtkMl_Vm *vm;
};

struct GtkMl_Vm {
    GtkMl_Register reg[GTKML_REGISTER_COUNT];
    GtkMl_Register *stack;
    size_t stack_len;
    GtkMl_Instruction *exec;
    size_t exec_len;

    GtkMl_Context *ctx;
};

static const char *S_I_ARITH[] = {
    [GTKML_IA_NOP] = GTKML_SIA_NOP,
    [GTKML_IA_HALT] = GTKML_SIA_HALT,
    [GTKML_IA_INTEGER_ADD] = GTKML_SIA_INTEGER_ADD,
    [GTKML_IA_INTEGER_SUBTRACT] = GTKML_SIA_INTEGER_SUBTRACT,
    [GTKML_IA_INTEGER_SIGNED_MULTIPLY] = GTKML_SIA_INTEGER_SIGNED_MULTIPLY,
    [GTKML_IA_INTEGER_UNSIGNED_MULTIPLY] = GTKML_SIA_INTEGER_UNSIGNED_MULTIPLY,
    [GTKML_IA_INTEGER_SIGNED_DIVIDE] = GTKML_SIA_INTEGER_SIGNED_DIVIDE,
    [GTKML_IA_INTEGER_UNSIGNED_DIVIDE] = GTKML_SIA_INTEGER_UNSIGNED_DIVIDE,
    [GTKML_IA_INTEGER_SIGNED_MODULO] = GTKML_SIA_INTEGER_SIGNED_MODULO,
    [GTKML_IA_INTEGER_UNSIGNED_MODULO] = GTKML_SIA_INTEGER_UNSIGNED_MODULO,
    [GTKML_IA_FLOAT_ADD] = GTKML_SIA_FLOAT_ADD,
    [GTKML_IA_FLOAT_SUBTRACT] = GTKML_SIA_FLOAT_SUBTRACT,
    [GTKML_IA_FLOAT_MULTIPLY] = GTKML_SIA_FLOAT_MULTIPLY,
    [GTKML_IA_FLOAT_DIVIDE] = GTKML_SIA_FLOAT_DIVIDE,
    [GTKML_IA_FLOAT_MODULO] = GTKML_SIA_FLOAT_MODULO,
    [GTKML_IA_BIT_AND] = GTKML_SIA_BIT_AND,
    [GTKML_IA_BIT_OR] = GTKML_SIA_BIT_OR,
    [GTKML_IA_BIT_XOR] = GTKML_SIA_BIT_XOR,
    [GTKML_IA_BIT_NAND] = GTKML_SIA_BIT_NAND,
    [GTKML_IA_BIT_NOR] = GTKML_SIA_BIT_NOR,
    [GTKML_IA_BIT_XNOR] = GTKML_SIA_BIT_XNOR,
    [GTKML_IA_BRANCH_ABSOLUTE] = GTKML_SIA_BRANCH_ABSOLUTE,
    [GTKML_IA_BRANCH_RELATIVE] = GTKML_SIA_BRANCH_RELATIVE,
    [GTKML_IA_BIND] = GTKML_SIA_BIND,
    [GTKML_IA_DEFINE] = GTKML_SIA_DEFINE,
    [255] = NULL,
};

static const char *S_I_IMM[] = {
    [GTKML_II_PUSH_IMM] = GTKML_SII_PUSH_IMM,
    [GTKML_II_POP] = GTKML_SII_POP,
    [GTKML_II_GET_IMM] = GTKML_SII_GET_IMM,
    [255] = NULL,
};

static const char *S_I_IMM_EXTERN[] = {
    [GTKML_II_PUSH_IMM] = GTKML_SII_PUSH_IMM_EXTERN,
    [GTKML_II_POP] = GTKML_SII_POP,
    [GTKML_II_GET_IMM] = GTKML_SII_GET_IMM,
    [255] = NULL,
};

static const char *S_I_BR[] = {
    [GTKML_IBR_CALL_FFI] = GTKML_SIBR_CALL_FFI,
    [GTKML_IBR_CALL] = GTKML_SIBR_CALL,
    [GTKML_IBR_RET] = GTKML_SIBR_RET,
    [255] = NULL,
};

static const char *S_EI_IMM[] = {
    [GTKML_EII_PUSH_EXT_IMM] = GTKML_SEII_PUSH_EXT_IMM,
    [GTKML_EII_POP_EXT] = GTKML_SEII_POP_EXT,
    [GTKML_EII_GET_EXT_IMM] = GTKML_SEII_GET_EXT_IMM,
    [255] = NULL,
};

static const char *S_EI_IMM_EXTERN[] = {
    [GTKML_EII_PUSH_EXT_IMM] = GTKML_SEII_PUSH_EXT_IMM_EXTERN,
    [GTKML_EII_POP_EXT] = GTKML_SEII_POP_EXT,
    [GTKML_EII_GET_EXT_IMM] = GTKML_SEII_GET_EXT_IMM,
    [255] = NULL,
};

static const char *S_EI_BR[] = {
    [GTKML_EIBR_CALL_EXT_FFI] = GTKML_SEIBR_CALL_EXT_FFI,
    [GTKML_EIBR_CALL_EXT] = GTKML_SEIBR_CALL_EXT,
    [GTKML_EIBR_RET_EXT] = GTKML_SEIBR_RET_EXT,
    [255] = NULL,
};

static const char **S_CATEGORY[] = {
    [0] = NULL,
    [GTKML_I_ARITH] = S_I_ARITH,
    [GTKML_I_IMM] = S_I_IMM,
    [GTKML_I_BR] = S_I_BR,
    [4] = NULL,
    [5] = NULL,
    [GTKML_I_IMM | GTKML_I_IMM_EXTERN] = S_I_IMM_EXTERN,
    [7] = NULL,
    [8] = NULL,
    [9] = NULL,
    [GTKML_EI_IMM] = S_EI_IMM,
    [GTKML_EI_BR] = S_EI_BR,
    [12] = NULL,
    [13] = NULL,
    [GTKML_EI_IMM | GTKML_EI_IMM_EXTERN] = S_EI_IMM_EXTERN,
    [15] = NULL,
};

GTKML_PRIVATE GtkMl_S *new_value(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_SKind kind);
GTKML_PRIVATE GtkMl_S *new_nil(GtkMl_Context *ctx, GtkMl_Span *span);
GTKML_PRIVATE GtkMl_S *new_true(GtkMl_Context *ctx, GtkMl_Span *span);
GTKML_PRIVATE GtkMl_S *new_false(GtkMl_Context *ctx, GtkMl_Span *span);
GTKML_PRIVATE GtkMl_S *new_int(GtkMl_Context *ctx, GtkMl_Span *span, int64_t value);
GTKML_PRIVATE GtkMl_S *new_float(GtkMl_Context *ctx, GtkMl_Span *span, float value);
GTKML_PRIVATE GtkMl_S *new_string(GtkMl_Context *ctx, GtkMl_Span *span, const char *ptr, size_t len);
GTKML_PRIVATE GtkMl_S *new_symbol(GtkMl_Context *ctx, GtkMl_Span *span, const char *ptr, size_t len);
GTKML_PRIVATE GtkMl_S *new_keyword(GtkMl_Context *ctx, GtkMl_Span *span, const char *ptr, size_t len);
GTKML_PRIVATE GtkMl_S *new_list(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *car, GtkMl_S *cdr);
GTKML_PRIVATE GtkMl_S *new_map(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *car, GtkMl_S *cdr);
GTKML_PRIVATE GtkMl_S *new_variadic(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *expr);
GTKML_PRIVATE GtkMl_S *new_lambda(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *args, GtkMl_S *body, GtkMl_S *capture);
GTKML_PRIVATE GtkMl_S *new_program(GtkMl_Context *ctx, GtkMl_Span *span, const char *linkage_name, uint64_t addr, GtkMl_S *args, GtkMl_S *body, GtkMl_S *capture);
GTKML_PRIVATE GtkMl_S *new_macro(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *args, GtkMl_S *body, GtkMl_S *capture);
GTKML_PRIVATE GtkMl_S *new_ffi(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *(*function)(GtkMl_Context *, const char **, GtkMl_S *));
GTKML_PRIVATE GtkMl_S *new_lightdata(GtkMl_Context *ctx, GtkMl_Span *span, void *data);
GTKML_PRIVATE GtkMl_S *new_userdata(GtkMl_Context *ctx, GtkMl_Span *span, void *data, void (*del)(GtkMl_Context *ctx, void *));
GTKML_PRIVATE void delete(GtkMl_Context *ctx, GtkMl_S *s);
GTKML_PRIVATE void del(GtkMl_Context *ctx, GtkMl_S *s);
GTKML_PRIVATE void gtk_ml_object_unref(GtkMl_Context *ctx, void *obj);

GTKML_PRIVATE GtkMl_S *builtin_lambda(GtkMl_Context *ctx, const char **err, GtkMl_S *expr);
GTKML_PRIVATE GtkMl_S *builtin_macro(GtkMl_Context *ctx, const char **err, GtkMl_S *expr);
GTKML_PRIVATE GtkMl_S *builtin_define(GtkMl_Context *ctx, const char **err, GtkMl_S *expr);
GTKML_PRIVATE GtkMl_S *builtin_define_macro(GtkMl_Context *ctx, const char **err, GtkMl_S *expr);
GTKML_PRIVATE GtkMl_S *builtin_application(GtkMl_Context *ctx, const char **err, GtkMl_S *expr);
GTKML_PRIVATE GtkMl_S *builtin_new_window(GtkMl_Context *ctx, const char **err, GtkMl_S *expr);

GTKML_PRIVATE GtkMl_S *vm_map(GtkMl_Context *ctx, const char **err, GtkMl_S *expr);
GTKML_PRIVATE GtkMl_S *vm_application(GtkMl_Context *ctx, const char **err, GtkMl_S *expr);
GTKML_PRIVATE GtkMl_S *vm_new_window(GtkMl_Context *ctx, const char **err, GtkMl_S *expr);

GTKML_PRIVATE gboolean gtk_ml_vm_run(GtkMl_Vm *vm, const char **err);

GTKML_PRIVATE gboolean gtk_ml_ia(GtkMl_Vm *vm, const char **err, GtkMl_Instruction *instr);
GTKML_PRIVATE gboolean gtk_ml_ii(GtkMl_Vm *vm, const char **err, GtkMl_Instruction *instr);
GTKML_PRIVATE gboolean gtk_ml_ibr(GtkMl_Vm *vm, const char **err, GtkMl_Instruction *instr);
GTKML_PRIVATE gboolean gtk_ml_eii(GtkMl_Vm *vm, const char **err, GtkMl_Instruction *instr);
GTKML_PRIVATE gboolean gtk_ml_eibr(GtkMl_Vm *vm, const char **err, GtkMl_Instruction *instr);
GTKML_PRIVATE gboolean gtk_ml_enop(GtkMl_Vm *vm, const char **err, GtkMl_Instruction *instr);

GTKML_PRIVATE gboolean gtk_ml_ia_nop(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_halt(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_integer_add(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_integer_subtract(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_integer_signed_multiply(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_integer_unsigned_multiply(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_integer_signed_divide(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_integer_unsigned_divide(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_integer_signed_modulo(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_integer_unsigned_modulo(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_float_add(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_float_subtract(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_float_multiply(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_float_divide(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_float_modulo(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_bit_and(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_bit_or(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_bit_xor(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_bit_nand(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_bit_nor(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_bit_xnor(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_branch_absolute(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_branch_relative(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_bind(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_define(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr);

GTKML_PRIVATE gboolean gtk_ml_ii_push_imm(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ii_pop(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ii_get_imm(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr);

GTKML_PRIVATE gboolean gtk_ml_ibr_call_ffi(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ibr_call(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ibr_ret(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr);

GTKML_PRIVATE gboolean gtk_ml_eii_push_ext_imm(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr, GtkMl_S *imm64);
GTKML_PRIVATE gboolean gtk_ml_eii_pop_ext(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr, GtkMl_S *imm64);
GTKML_PRIVATE gboolean gtk_ml_eii_get_ext_imm(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr, GtkMl_S *imm64);

GTKML_PRIVATE gboolean gtk_ml_eibr_call_ext_ffi(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr, GtkMl_S *imm64);
GTKML_PRIVATE gboolean gtk_ml_eibr_call_ext(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr, GtkMl_S *imm64);
GTKML_PRIVATE gboolean gtk_ml_eibr_ret_ext(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr, GtkMl_S *imm64);

GtkMl_Context *gtk_ml_new_context() {
    GtkMl_Context *ctx = malloc(sizeof(GtkMl_Context));
    ctx->gc_enabled = 1;
    ctx->n_values = 0;
    ctx->m_values = GTKML_GC_THRESHOLD;
    ctx->first = NULL;
    ctx->last = NULL;
    ctx->vm = gtk_ml_new_vm(ctx);

    gtk_ml_disable_gc(ctx);
    // root value
    // the root doesn't get sweeped by gc, so it doesn't need to be pushed to stack
    new_nil(ctx, NULL);
    // ({'#nil #nil '#t #t '#f #f 'flags-none G_APPLICATION_FLAGS_NONE})
    ctx->bindings = new_list(ctx, NULL, new_nil(ctx, NULL), new_nil(ctx, NULL));
    ctx->top_scope = &gtk_ml_car(ctx->bindings);

    gtk_ml_define(ctx, new_symbol(ctx, NULL, "#nil", 4), new_nil(ctx, NULL));
    gtk_ml_define(ctx, new_symbol(ctx, NULL, "#t", 2), new_true(ctx, NULL));
    gtk_ml_define(ctx, new_symbol(ctx, NULL, "#f", 2), new_false(ctx, NULL));
    gtk_ml_define(ctx, new_symbol(ctx, NULL, "flags-none", 10), new_int(ctx, NULL, G_APPLICATION_FLAGS_NONE));

    gtk_ml_define(ctx, new_symbol(ctx, NULL, "lambda", 6), new_ffi(ctx, NULL, builtin_lambda));
    gtk_ml_define(ctx, new_symbol(ctx, NULL, "macro", 5), new_ffi(ctx, NULL, builtin_macro));
    gtk_ml_define(ctx, new_symbol(ctx, NULL, "define", 6), new_ffi(ctx, NULL, builtin_define));
    gtk_ml_define(ctx, new_symbol(ctx, NULL, "define-macro", 12), new_ffi(ctx, NULL, builtin_define_macro));
    gtk_ml_define(ctx, new_symbol(ctx, NULL, "Application", 11), new_ffi(ctx, NULL, builtin_application));
    gtk_ml_define(ctx, new_symbol(ctx, NULL, "new-window", 10), new_ffi(ctx, NULL, builtin_new_window));

    gtk_ml_enable_gc(ctx, 1);
    return ctx;
}

void gtk_ml_del_context(GtkMl_Context *ctx) {
    gtk_ml_del_vm(ctx->vm);

    GtkMl_S *value = ctx->first;
    while (value) {
        GtkMl_S *next = value->next;
        del(ctx, value);
        value = next;
    }

    free(ctx);
}

void gtk_ml_load_program(GtkMl_Context *ctx, GtkMl_Program program) {
    ctx->vm->exec = malloc(sizeof(GtkMl_Instruction) * program.n_exec);
    ctx->vm->exec_len = program.n_exec;
    memcpy(ctx->vm->exec, program.exec, sizeof(GtkMl_Instruction) * program.n_exec);
}

gboolean gtk_ml_run_program(GtkMl_Context *ctx, const char **err, GtkMl_S *program, GtkMl_S *args) {
    gtk_ml_enter(ctx);

    GtkMl_S *params = program->value.s_program.args;

    while (params->kind != GTKML_S_NIL && args && args->kind != GTKML_S_NIL) {
        GtkMl_S *arg = gtk_ml_exec(ctx, err, gtk_ml_car(args));
        if (!arg) {
            break;
        }
        gtk_ml_bind(ctx, gtk_ml_car(params), arg);
        params = gtk_ml_cdr(params);
        args = gtk_ml_cdr(args);
    }

    if (params->kind == GTKML_S_LIST) {
        GtkMl_S *param = gtk_ml_car(params);
        if (param->kind == GTKML_S_VARIADIC) {
            gtk_ml_bind(ctx, param->value.s_var.expr, new_nil(ctx, NULL));
        } else {
            *err = GTKML_ERR_ARITY_ERROR;
            return 0;
        }
    }

    ctx->vm->reg[GTKML_R_PC].pc = program->value.s_program.addr;
    gboolean result = gtk_ml_vm_run(ctx->vm, err);

    gtk_ml_leave(ctx);

    return result;
}

GtkMl_S *gtk_ml_get_export(GtkMl_Context *ctx, const char **err, const char *linkage_name) {
    for (size_t i = 0; i < ctx->vm->exec_len;) {
        GtkMl_Instruction instr = ctx->vm->exec[i];
        if (instr.gen.category == GTKML_EI_EXPORT) {
            GtkMl_S *program = ctx->vm->exec[i + 1].imm64;
            const char *export = program->value.s_program.linkage_name;
            if (strcmp(linkage_name, export) == 0) {
                return program;
            }
        }
        if (instr.gen.category & GTKML_I_EXTENDED) {
            i += 2;
        } else {
            ++i;
        }
    }

    *err = GTKML_ERR_BINDING_ERROR;
    return NULL;
}

GtkMl_Builder *gtk_ml_new_builder() {
    GtkMl_Builder *b = malloc(sizeof(GtkMl_Builder));

    b->basic_blocks = malloc(sizeof(GtkMl_BasicBlock) * 64);
    b->len_bb = 0;
    b->cap_bb = 64;
    b->counter = 0;

    return b;
}

GtkMl_Program gtk_ml_build(GtkMl_Context *ctx, const char **err, GtkMl_Builder *b) {
    (void) ctx;

    size_t n = 0;
    for (size_t i = 0; i < b->len_bb; i++) {
        n += b->basic_blocks[i].len_exec;
    }

    GtkMl_Instruction *result = malloc(sizeof(GtkMl_Instruction) * n);
    uint64_t pc = 0;

    for (size_t i = 0; i < b->len_bb; i++) {
        size_t n = b->basic_blocks[i].len_exec;
        memcpy(result + pc, b->basic_blocks[i].exec, sizeof(GtkMl_Instruction) * n);
        pc += n;
    }

    for (size_t i = 0; i < n;) {
        GtkMl_Instruction instr = result[i];
        if (instr.gen.category == GTKML_EI_EXPORT) {
            GtkMl_S *program = result[i + 1].imm64;
            program->value.s_program.addr = 8 * i;
        }
        if (instr.gen.category & GTKML_I_EXTENDED) {
            i += 2;
        } else {
            ++i;
        }
    }

    for (size_t i = 0; i < n;) {
        GtkMl_Instruction instr = result[i];
        if (instr.gen.category != GTKML_EI_EXPORT && (instr.gen.category & GTKML_EI_IMM_EXTERN)) {
            GtkMl_S *ext;
            if (instr.gen.category & GTKML_I_EXTENDED) {
                ext = result[i + 1].imm64;
            } else if ((instr.gen.category & ~GTKML_EI_IMM_EXTERN) == GTKML_I_IMM) {
                ext = (GtkMl_S *) instr.imm.imm;
            } else if ((instr.gen.category & ~GTKML_EI_IMM_EXTERN) == GTKML_I_BR) {
                ext = (GtkMl_S *) instr.br.imm;
            } else {
                *err = GTKML_ERR_CATEGORY_ERROR;
                GtkMl_Program program;
                program.start = NULL;
                program.exec = NULL;
                program.n_exec = 0;
                return program;
            }
            const char *bb1 = ext->value.s_string.ptr;
            gboolean found = 0;

            for (size_t l = 0; l < n;) {
                GtkMl_Instruction instr = result[l];
                if (instr.gen.category == GTKML_EI_EXPORT) {
                    GtkMl_S *program = result[l + 1].imm64;
                    const char *bb2 = program->value.s_program.linkage_name;
                    if (strcmp(bb1, bb2) == 0) {
                        result[i].gen.category &= ~GTKML_EI_IMM_EXTERN;
                        if (result[i].gen.category & GTKML_I_EXTENDED) {
                            result[i + 1].imm64 = program;
                        } else if (result[i].gen.category == GTKML_I_IMM) {
                            result[i].imm.imm = (uintptr_t) program;
                        } else if (result[i].gen.category == GTKML_I_BR) {
                            result[i].br.imm = (uintptr_t) program;
                        } else {
                            *err = GTKML_ERR_CATEGORY_ERROR;
                            GtkMl_Program program;
                            program.start = NULL;
                            program.exec = NULL;
                            program.n_exec = 0;
                            return program;
                        }
                        found = 1;
                        break;
                    }
                }
                if (instr.gen.category & GTKML_I_EXTENDED) {
                    l += 2;
                } else {
                    ++l;
                }
            }

            if (!found) {
                *err = GTKML_ERR_LINKAGE_ERROR;
                GtkMl_Program program;
                program.start = NULL;
                program.exec = NULL;
                program.n_exec = 0;
                return program;
            }
        }
        if (instr.gen.category & GTKML_I_EXTENDED) {
            i += 2;
        } else {
            ++i;
        }
    }

    for (size_t i = 0; i < b->len_bb; i++) {
        free(b->basic_blocks[i].exec);
    }
    free(b->basic_blocks);
    free(b);

    GtkMl_Program program;
    char *start = malloc(strlen("_start") + 1);
    strcpy(start, "_start");
    program.start = start;
    program.exec = result;
    program.n_exec = n;

    return program;
}

GTKML_PUBLIC void gtk_ml_del_program(GtkMl_Program program) {
    free((void *) program.start);
    free(program.exec);
}

GtkMl_BasicBlock *gtk_ml_append_basic_block(GtkMl_Builder *b, const char *name) {
    if (b->len_bb == b->cap_bb) {
        b->cap_bb *= 2;
        b->basic_blocks = realloc(b->basic_blocks, sizeof(GtkMl_BasicBlock) * b->cap_bb);
    }

    GtkMl_BasicBlock *basic_block = b->basic_blocks + b->len_bb;
    ++b->len_bb;

    basic_block->name = name;
    basic_block->exec = malloc(sizeof(GtkMl_Instruction) * 256);
    basic_block->len_exec = 0;
    basic_block->cap_exec = 256;

    return basic_block;
}

GTKML_PRIVATE gboolean IDENT_BEGIN[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

GTKML_PRIVATE gboolean IDENT_CONT[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

GTKML_PRIVATE gboolean is_ident_begin(unsigned char c) {
    return IDENT_BEGIN[c];
}

GTKML_PRIVATE gboolean is_ident_cont(unsigned char c) {
    return IDENT_CONT[c];
}

GTKML_PRIVATE gboolean lex(GtkMl_Token **tokenv, size_t *tokenc, const char **err, const char *src) {
    *tokenv = malloc(sizeof(GtkMl_Token) * 64);
    *tokenc = 0;
    size_t cap = 64;

    int line = 1;
    int col = 1;

    while (*src) {
        if (*tokenc == cap) {
            cap *= 2;
            *tokenv = realloc(*tokenv, cap);
        }

        switch (*src) {
        case '\n':
            col = 1;
            ++line;
            ++src;
            continue;
        case '\t':
            ++col;
            ++src;
            continue;
        case ' ':
            ++col;
            ++src;
            continue;
        case ';':
            while (*src && *src != '\n') {
                ++col;
                ++src;
            }
            continue;
        case '.':
            if (*(src + 1) == '.' && *(src + 2) == '.') {
                (*tokenv)[*tokenc].kind = GTKML_TOK_ELLIPSIS;
                (*tokenv)[*tokenc].span.ptr = src;
                (*tokenv)[*tokenc].span.len = 3;
                (*tokenv)[*tokenc].span.line = line;
                (*tokenv)[*tokenc].span.col = col;
                ++*tokenc;
                col += 3;
                src += 3;
                continue;
            }
            (*tokenv)[*tokenc].kind = GTKML_TOK_DOT;
            (*tokenv)[*tokenc].span.ptr = src;
            (*tokenv)[*tokenc].span.len = 1;
            (*tokenv)[*tokenc].span.line = line;
            (*tokenv)[*tokenc].span.col = col;
            ++*tokenc;
            break;
        case '(':
            (*tokenv)[*tokenc].kind = GTKML_TOK_PARENL;
            (*tokenv)[*tokenc].span.ptr = src;
            (*tokenv)[*tokenc].span.len = 1;
            (*tokenv)[*tokenc].span.line = line;
            (*tokenv)[*tokenc].span.col = col;
            ++*tokenc;
            break;
        case ')':
            (*tokenv)[*tokenc].kind = GTKML_TOK_PARENR;
            (*tokenv)[*tokenc].span.ptr = src;
            (*tokenv)[*tokenc].span.len = 1;
            (*tokenv)[*tokenc].span.line = line;
            (*tokenv)[*tokenc].span.col = col;
            ++*tokenc;
            break;
        case '{':
            (*tokenv)[*tokenc].kind = GTKML_TOK_CURLYL;
            (*tokenv)[*tokenc].span.ptr = src;
            (*tokenv)[*tokenc].span.len = 1;
            (*tokenv)[*tokenc].span.line = line;
            (*tokenv)[*tokenc].span.col = col;
            ++*tokenc;
            break;
        case '}':
            (*tokenv)[*tokenc].kind = GTKML_TOK_CURLYR;
            (*tokenv)[*tokenc].span.ptr = src;
            (*tokenv)[*tokenc].span.len = 1;
            (*tokenv)[*tokenc].span.line = line;
            (*tokenv)[*tokenc].span.col = col;
            ++*tokenc;
            break;
        case '"': {
            ++col;
            ++src;
            // TODO(walterpi): escape codes
            const char *str_ptr = src - 1;
            int str_line = line;
            int str_col = col - 1;
            size_t str_len = 1;
            do {
                ++str_len;
                ++col;
                ++src;
            } while (*src != '"');
            (*tokenv)[*tokenc].kind = GTKML_TOK_STRING;
            (*tokenv)[*tokenc].span.ptr = str_ptr;
            (*tokenv)[*tokenc].span.len = str_len + 1;
            (*tokenv)[*tokenc].span.line = str_line;
            (*tokenv)[*tokenc].span.col = str_col;
            ++*tokenc;
        } break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': {
            char *endptr;
            int64_t intval = strtoll(src, &endptr, 10);
            if (*endptr == ' ' || *endptr == '\n' || *endptr == '\t'
                    || *endptr == '(' || *endptr == ')'
                    || *endptr == '{' || *endptr == '}') {
                (*tokenv)[*tokenc].kind = GTKML_TOK_INT;
                (*tokenv)[*tokenc].value.intval = intval;
                (*tokenv)[*tokenc].span.ptr = src;
                (*tokenv)[*tokenc].span.len = 1;
                (*tokenv)[*tokenc].span.line = line;
                (*tokenv)[*tokenc].span.col = col;
                ++*tokenc;
                src = endptr;
                continue;
            } else if (*endptr == '.') {
                double floatval = strtod(src, &endptr);
                if (*endptr == ' ' || *endptr == '\n' || *endptr == '\t'
                        || *endptr == '(' || *endptr == ')'
                        || *endptr == '{' || *endptr == '}') {
                    (*tokenv)[*tokenc].kind = GTKML_TOK_FLOAT;
                    (*tokenv)[*tokenc].value.floatval = floatval;
                    (*tokenv)[*tokenc].span.ptr = src;
                    (*tokenv)[*tokenc].span.len = 1;
                    (*tokenv)[*tokenc].span.line = line;
                    (*tokenv)[*tokenc].span.col = col;
                    ++*tokenc;
                    src = endptr;
                    continue;
                } else {
                    *err = GTKML_ERR_CHARACTER_ERROR;
                    return 0;
                }
            } else {
                *err = GTKML_ERR_CHARACTER_ERROR;
                return 0;
            }
        } break;
        case ':':
            ++col;
            ++src;
            if (is_ident_begin(*src)) {
                const char *kw_ptr = src - 1;
                int kw_line = line;
                int kw_col = col - 1;
                size_t kw_len = 1;
                while (is_ident_cont(*src)) {
                    ++kw_len;
                    ++col;
                    ++src;
                }
                (*tokenv)[*tokenc].kind = GTKML_TOK_KEYWORD;
                (*tokenv)[*tokenc].span.ptr = kw_ptr;
                (*tokenv)[*tokenc].span.len = kw_len;
                (*tokenv)[*tokenc].span.line = kw_line;
                (*tokenv)[*tokenc].span.col = kw_col;
                ++*tokenc;
                continue;
            } else {
                *err = GTKML_ERR_CHARACTER_ERROR;
                return 0;
            }
            break;
        default:
            if (is_ident_begin(*src)) {
                const char *ident_ptr = src;
                int ident_line = line;
                int ident_col = col;
                size_t ident_len = 0;
                while (is_ident_cont(*src)) {
                    ++ident_len;
                    ++col;
                    ++src;
                }
                (*tokenv)[*tokenc].kind = GTKML_TOK_IDENT;
                (*tokenv)[*tokenc].span.ptr = ident_ptr;
                (*tokenv)[*tokenc].span.len = ident_len;
                (*tokenv)[*tokenc].span.line = ident_line;
                (*tokenv)[*tokenc].span.col = ident_col;
                ++*tokenc;
                continue;
            } else {
                *err = GTKML_ERR_CHARACTER_ERROR;
                return 0;
            }
            break;
        }

        ++col;
        ++src;
    }

    return 1;
}

GTKML_PRIVATE GtkMl_S *new_value(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_SKind kind) {
    ++ctx->n_values;

    GtkMl_S *s = malloc(sizeof(GtkMl_S));
    if (!ctx->last) {
        ctx->first = s;
        ctx->last = s;
    } else {
        ctx->last->next = s;
        ctx->last = s;
    }

    s->next = NULL;
    s->flags = GTKML_FLAG_NONE;
    s->kind = kind;
    if (span) {
        s->span = *span;
    } else {
        s->span = (GtkMl_Span) {0};
    }

    return s;
}

GTKML_PRIVATE GtkMl_S *new_nil(GtkMl_Context *ctx, GtkMl_Span *span) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_NIL);
    return s;
}

GTKML_PRIVATE GtkMl_S *new_true(GtkMl_Context *ctx, GtkMl_Span *span) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_TRUE);
    return s;
}

GTKML_PRIVATE GtkMl_S *new_false(GtkMl_Context *ctx, GtkMl_Span *span) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_FALSE);
    return s;
}

GTKML_PRIVATE GtkMl_S *new_int(GtkMl_Context *ctx, GtkMl_Span *span, int64_t value) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_INT);
    s->value.s_int.value = value;
    return s;
}

GTKML_PRIVATE GtkMl_S *new_float(GtkMl_Context *ctx, GtkMl_Span *span, float value) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_FLOAT);
    s->value.s_float.value = value;
    return s;
}

GTKML_PRIVATE GtkMl_S *new_string(GtkMl_Context *ctx, GtkMl_Span *span, const char *ptr, size_t len) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_STRING);
    s->value.s_string.ptr = ptr;
    s->value.s_string.len = len;
    return s;
}

GTKML_PRIVATE GtkMl_S *new_symbol(GtkMl_Context *ctx, GtkMl_Span *span, const char *ptr, size_t len) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_SYMBOL);
    s->value.s_symbol.ptr = ptr;
    s->value.s_symbol.len = len;
    return s;
}

GTKML_PRIVATE GtkMl_S *new_keyword(GtkMl_Context *ctx, GtkMl_Span *span, const char *ptr, size_t len) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_KEYWORD);
    s->value.s_keyword.ptr = ptr;
    s->value.s_keyword.len = len;
    return s;
}

GTKML_PRIVATE GtkMl_S *new_list(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *car, GtkMl_S *cdr) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_LIST);
    s->value.s_list.car = car;
    s->value.s_list.cdr = cdr;
    return s;
}

GTKML_PRIVATE GtkMl_S *new_map(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *car, GtkMl_S *cdr) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_MAP);
    s->value.s_map.car = car;
    s->value.s_map.cdr = cdr;
    return s;
}

GTKML_PRIVATE GtkMl_S *new_variadic(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *expr) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_VARIADIC);
    s->value.s_var.expr = expr;
    return s;
}

GTKML_PRIVATE GtkMl_S *new_lambda(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *args, GtkMl_S *body, GtkMl_S *capture) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_LAMBDA);
    s->value.s_lambda.args = args;
    s->value.s_lambda.body = body;
    s->value.s_lambda.capture = capture;
    return s;
}

GTKML_PRIVATE GtkMl_S *new_program(GtkMl_Context *ctx, GtkMl_Span *span, const char *linkage_name, uint64_t addr, GtkMl_S *args, GtkMl_S *body, GtkMl_S *capture) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_PROGRAM);
    s->value.s_program.linkage_name = linkage_name;
    s->value.s_program.addr = addr;
    s->value.s_program.args = args;
    s->value.s_program.body = body;
    s->value.s_program.capture = capture;
    return s;
}

GTKML_PRIVATE GtkMl_S *new_macro(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *args, GtkMl_S *body, GtkMl_S *capture) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_MACRO);
    s->value.s_macro.args = args;
    s->value.s_macro.body = body;
    s->value.s_macro.capture = capture;
    return s;
}

GTKML_PRIVATE GtkMl_S *new_ffi(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *(*function)(GtkMl_Context *, const char **, GtkMl_S *)) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_FFI);
    s->value.s_ffi.function = function;
    return s;
}

GTKML_PRIVATE GtkMl_S *new_lightdata(GtkMl_Context *ctx, GtkMl_Span *span, void *data) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_LIGHTDATA);
    s->value.s_lightdata.userdata = data;
    return s;
}

GTKML_PRIVATE GtkMl_S *new_userdata(GtkMl_Context *ctx, GtkMl_Span *span, void *data, void (*del)(GtkMl_Context *ctx, void *)) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_USERDATA);
    s->value.s_userdata.userdata = data;
    s->value.s_userdata.del = del;
    s->value.s_userdata.keep = new_nil(ctx, span);
    return s;
}

GTKML_PRIVATE void span_add(GtkMl_Span *dest, GtkMl_Span *lhs, GtkMl_Span *rhs) {
    size_t len = rhs->ptr - lhs->ptr + rhs->len;
    const char *ptr = lhs->ptr;
    int line = lhs->line;
    int col = lhs->col;
    dest->ptr = ptr;
    dest->len = len;
    dest->line = line;
    dest->col = col;
}

GTKML_PRIVATE GtkMl_S *parse(GtkMl_Context *ctx, const char **err, GtkMl_Token **tokenv, size_t *tokenc);

GTKML_PRIVATE GtkMl_S *parse_variadic(GtkMl_Context *ctx, const char **err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_ELLIPSIS) {
        *err = GTKML_ERR_TOKEN_ERROR;
        return NULL;
    }

    GtkMl_Span span = (*tokenv)[0].span;

    ++*tokenv;
    --*tokenc;

    GtkMl_S *expr = parse(ctx, err, tokenv, tokenc);
    if (!expr) {
        return NULL;
    }

    span_add(&span, &span, &expr->span);

    return new_variadic(ctx, &span, expr);
}

GTKML_PRIVATE GtkMl_S *parse_list_rest(GtkMl_Context *ctx, const char **err, GtkMl_Token **tokenv, size_t *tokenc) {
    if (*tokenc == 0) {
        *err = GTKML_ERR_EOF_ERROR;
        return NULL;
    }

    GtkMl_Span span = (*tokenv)[0].span;

    if ((*tokenv)[0].kind == GTKML_TOK_PARENR) {
        span_add(&span, &span, &(*tokenv)[0].span);

        ++*tokenv;
        --*tokenc;

        return new_nil(ctx, &span);
    }

    GtkMl_S *car = parse(ctx, err, tokenv, tokenc);
    GtkMl_S *cdr = parse_list_rest(ctx, err, tokenv, tokenc);
    if (!cdr) {
        return NULL;
    }

    span_add(&span, &span, &cdr->span);

    return new_list(ctx, &span, car, cdr);
}

GTKML_PRIVATE GtkMl_S *parse_list(GtkMl_Context *ctx, const char **err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_PARENL) {
        *err = GTKML_ERR_TOKEN_ERROR;
        return NULL;
    }

    ++*tokenv;
    --*tokenc;

    return parse_list_rest(ctx, err, tokenv, tokenc);
}

GTKML_PRIVATE GtkMl_S *parse_map_rest(GtkMl_Context *ctx, const char **err, GtkMl_Token **tokenv, size_t *tokenc) {
    if (*tokenc == 0) {
        *err = GTKML_ERR_EOF_ERROR;
        return NULL;
    }

    GtkMl_Span span = (*tokenv)[0].span;

    if ((*tokenv)[0].kind == GTKML_TOK_CURLYR) {
        span_add(&span, &span, &(*tokenv)[0].span);

        ++*tokenv;
        --*tokenc;

        return new_nil(ctx, &span);
    }

    GtkMl_S *car = parse(ctx, err, tokenv, tokenc);
    GtkMl_S *cdr = parse_map_rest(ctx, err, tokenv, tokenc);
    if (!cdr) {
        return NULL;
    }

    span_add(&span, &span, &cdr->span);

    return new_map(ctx, &span, car, cdr);
}

GTKML_PRIVATE GtkMl_S *parse_map(GtkMl_Context *ctx, const char **err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_CURLYL) {
        *err = GTKML_ERR_TOKEN_ERROR;
        return NULL;
    }

    ++*tokenv;
    --*tokenc;

    return parse_map_rest(ctx, err, tokenv, tokenc);
}

GTKML_PRIVATE GtkMl_S *parse_int(GtkMl_Context *ctx, const char **err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_INT) {
        *err = GTKML_ERR_TOKEN_ERROR;
        return NULL;
    }
    GtkMl_S *result = new_int(ctx, &(*tokenv)[0].span, (*tokenv)[0].value.intval);
    ++*tokenv;
    --*tokenc;
    return result;
}

GTKML_PRIVATE GtkMl_S *parse_float(GtkMl_Context *ctx, const char **err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_FLOAT) {
        *err = GTKML_ERR_TOKEN_ERROR;
        return NULL;
    }
    GtkMl_S *result = new_float(ctx, &(*tokenv)[0].span, (*tokenv)[0].value.floatval);
    ++*tokenv;
    --*tokenc;
    return result;
}

GTKML_PRIVATE GtkMl_S *parse_string(GtkMl_Context *ctx, const char **err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_STRING) {
        *err = GTKML_ERR_TOKEN_ERROR;
        return NULL;
    }
    size_t len = (*tokenv)[0].span.len - 2;
    char *ptr = malloc(len + 1);
    memcpy(ptr, (*tokenv)[0].span.ptr + 1, len);
    ptr[len] = 0;
    GtkMl_S *result = new_string(ctx, &(*tokenv)[0].span, ptr, len);
    ++*tokenv;
    --*tokenc;
    return result;
}

GTKML_PRIVATE GtkMl_S *parse_symbol(GtkMl_Context *ctx, const char **err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_IDENT) {
        *err = GTKML_ERR_TOKEN_ERROR;
        return NULL;
    }
    GtkMl_S *result = new_symbol(ctx, &(*tokenv)[0].span, (*tokenv)[0].span.ptr, (*tokenv)[0].span.len);
    ++*tokenv;
    --*tokenc;
    return result;
}

GTKML_PRIVATE GtkMl_S *parse_keyword(GtkMl_Context *ctx, const char **err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_KEYWORD) {
        *err = GTKML_ERR_TOKEN_ERROR;
        return NULL;
    }
    GtkMl_S *result = new_keyword(ctx, &(*tokenv)[0].span, (*tokenv)[0].span.ptr + 1, (*tokenv)[0].span.len - 1);
    ++*tokenv;
    --*tokenc;
    return result;
}

GTKML_PRIVATE GtkMl_S *parse(GtkMl_Context *ctx, const char **err, GtkMl_Token **tokenv, size_t *tokenc) {
    switch ((*tokenv)[0].kind) {
    case GTKML_TOK_ELLIPSIS:
        return parse_variadic(ctx, err, tokenv, tokenc);
    case GTKML_TOK_PARENL:
        return parse_list(ctx, err, tokenv, tokenc);
    case GTKML_TOK_CURLYL:
        return parse_map(ctx, err, tokenv, tokenc);
    case GTKML_TOK_INT:
        return parse_int(ctx, err, tokenv, tokenc);
    case GTKML_TOK_FLOAT:
        return parse_float(ctx, err, tokenv, tokenc);
    case GTKML_TOK_STRING:
        return parse_string(ctx, err, tokenv, tokenc);
    case GTKML_TOK_IDENT:
        return parse_symbol(ctx, err, tokenv, tokenc);
    case GTKML_TOK_KEYWORD:
        return parse_keyword(ctx, err, tokenv, tokenc);
    default:
        *err = GTKML_ERR_TOKEN_ERROR;
        return NULL;
    }
}

GtkMl_S *gtk_ml_load(GtkMl_Context *ctx, char **src, const char **err, const char *file) {
    FILE *stream = fopen(file, "r");
    if (!stream) {
        *err = GTKML_ERR_IO_ERROR;
        return NULL;
    }
    GtkMl_S *result = gtk_ml_loadf(ctx, src, err, stream);
    fclose(stream);
    return result;
}

GtkMl_S *gtk_ml_loadf(GtkMl_Context *ctx, char **src, const char **err, FILE *stream) {
    fseek(stream, 0l, SEEK_END);
    size_t size = ftell(stream);
    fseek(stream, 0l, SEEK_SET);
    *src = malloc(size + 1);
    size_t read = fread(*src, 1, size, stream);
    if (read != size) {
        *err = GTKML_ERR_IO_ERROR;
        return NULL;
    }
    (*src)[size] = 0;
    return gtk_ml_loads(ctx, err, *src);
}

GtkMl_S *gtk_ml_loads(GtkMl_Context *ctx, const char **err, const char *src) {
    GtkMl_Token *tokenv;
    size_t tokenc;
    if (!lex(&tokenv, &tokenc, err, src)) {
        return NULL;
    }
    GtkMl_Token *_tokenv = tokenv;
    gboolean enabled = gtk_ml_disable_gc(ctx);

    GtkMl_S *body = new_nil(ctx, NULL);
    GtkMl_S **last = &body;

    while (tokenc) {
        GtkMl_S *line = parse(ctx, err, &_tokenv, &tokenc);
        if (!line) {
            return NULL;
        }
        GtkMl_S *new = new_list(ctx, NULL, line, *last);
        *last = new;
        last = &gtk_ml_cdr(new);
    }

    GtkMl_S *result = new_lambda(ctx, NULL, new_nil(ctx, NULL), body, new_nil(ctx, NULL));

    gtk_ml_enable_gc(ctx, enabled);
    free(tokenv);
    return result;
}

GTKML_PRIVATE gboolean compile_call(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, const char **err, GtkMl_S *(*vm_function)(GtkMl_Context *ctx, const char **err, GtkMl_S *expr), GtkMl_S *args, gboolean compile_first);
GTKML_PRIVATE gboolean compile_expression(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, const char **err, GtkMl_S *stmt);
GTKML_PRIVATE gboolean compile_body(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, const char **err, GtkMl_S *stmt);
GTKML_PRIVATE gboolean compile_program(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, const char **err, const char *linkage_name, GtkMl_S *stmt, gboolean ret);

gboolean compile_call(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, const char **err, GtkMl_S *(*vm_function)(GtkMl_Context *ctx, const char **err, GtkMl_S *expr), GtkMl_S *args, gboolean compile_first) {
    int64_t n = 0;
    if (!compile_first) {
        if (!gtk_ml_build_push_extended_imm(ctx, b, basic_block, err, gtk_ml_car(args))) {
            return 0;
        }
        args = gtk_ml_cdr(args);
        ++n;
    }
    while (args->kind != GTKML_S_NIL) {
        if (!compile_expression(ctx, b, basic_block, err, gtk_ml_car(args))) {
            return 0;
        }
        args = gtk_ml_cdr(args);
        ++n;
    }

    if (!gtk_ml_build_push_extended_imm(ctx, b, basic_block, err, new_int(ctx, NULL, n))) {
        return 0;
    }
    if (!gtk_ml_build_call_extended_ffi(ctx, b, basic_block, err, new_ffi(ctx, NULL, vm_function))) {
        return 0;
    }

    return 1;
}

gboolean compile_expression(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, const char **err, GtkMl_S *stmt) {
    switch (stmt->kind) {
    case GTKML_S_NIL:
    case GTKML_S_TRUE:
    case GTKML_S_FALSE:
    case GTKML_S_INT:
    case GTKML_S_FLOAT:
    case GTKML_S_STRING:
    case GTKML_S_KEYWORD:
    case GTKML_S_PROGRAM:
    case GTKML_S_ADDRESS:
    case GTKML_S_LIGHTDATA:
    case GTKML_S_USERDATA:
    case GTKML_S_FFI:
        return gtk_ml_build_push_extended_imm(ctx, b, basic_block, err, stmt);
    case GTKML_S_SYMBOL:
        return gtk_ml_build_get_extended_imm(ctx, b, basic_block, err, stmt);
    case GTKML_S_LAMBDA: {
        char *linkage_name = malloc(strlen("lambda$") + 16);
        snprintf(linkage_name, strlen("lambda$") + 16, "lambda$%u", b->counter++);
        GtkMl_BasicBlock *bb = gtk_ml_append_basic_block(b, linkage_name);
        if (!compile_program(ctx, b, bb, err, linkage_name, stmt, 1)) {
            return 0;
        }
        return gtk_ml_build_push_extended_addr(ctx, b, basic_block, err, new_string(ctx, NULL, linkage_name, strlen(linkage_name)));
    }
    case GTKML_S_MACRO:
        *err = GTKML_ERR_UNIMPLEMENTED;
        return 0;
    case GTKML_S_MAP:
        return compile_call(ctx, b, basic_block, err, vm_map, stmt, 1);
    case GTKML_S_VARIADIC:
        *err = GTKML_ERR_UNIMPLEMENTED;
        return 0;
    case GTKML_S_LIST: {
        GtkMl_S *function = gtk_ml_car(stmt);

        GtkMl_S *args = gtk_ml_cdr(stmt);

        if (function->kind == GTKML_S_SYMBOL) {
            const char *symbol_application = "Application";
            const char *symbol_new_window = "new-window";
            const char *symbol_lambda = "lambda";

            if (function->value.s_symbol.len == strlen(symbol_application)
                    && memcmp(function->value.s_symbol.ptr, symbol_application, function->value.s_symbol.len) == 0) {
                if (args->kind == GTKML_S_NIL
                        || gtk_ml_cdr(args)->kind == GTKML_S_NIL
                        || gtk_ml_cddr(args)->kind == GTKML_S_NIL) {
                    *err = GTKML_ERR_ARITY_ERROR;
                    return 0;
                }

                return compile_call(ctx, b, basic_block, err, vm_application, stmt, 0);
            } else if (function->value.s_symbol.len == strlen(symbol_new_window)
                    && memcmp(function->value.s_symbol.ptr, symbol_new_window, function->value.s_symbol.len) == 0) {
                if (args->kind == GTKML_S_NIL
                        || gtk_ml_cdr(args)->kind == GTKML_S_NIL
                        || gtk_ml_cddr(args)->kind == GTKML_S_NIL) {
                    *err = GTKML_ERR_ARITY_ERROR;
                    return 0;
                }

                return compile_call(ctx, b, basic_block, err, vm_new_window, stmt, 0);
            } else if (function->value.s_symbol.len == strlen(symbol_lambda)
                    && memcmp(function->value.s_symbol.ptr, symbol_lambda, function->value.s_symbol.len) == 0) {
                GtkMl_S *lambda = builtin_lambda(ctx, err, stmt);
                if (!lambda) {
                    return 0;
                }

                return compile_expression(ctx, b, basic_block, err, lambda);
            }
        }

        *err = GTKML_ERR_UNIMPLEMENTED;
        return 0;
    }
    }
}

gboolean compile_body(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, const char **err, GtkMl_S *body) {
    while (body->kind != GTKML_S_NIL) {
        GtkMl_S *stmt = gtk_ml_car(body);
        if (!compile_expression(ctx, b, basic_block, err, stmt)) {
            return 0;
        }
        body = gtk_ml_cdr(body);
    }

    return 1;
}

gboolean compile_program(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, const char **err, const char *linkage_name, GtkMl_S *lambda, gboolean ret) {
    if (lambda->kind != GTKML_S_LAMBDA) {
        *err = GTKML_ERR_PROGRAM_ERROR;
        return 0;
    }

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = 0;
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_EI_EXPORT;
    ++basic_block->len_exec;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    char *name = malloc(strlen(linkage_name) + 1);
    strcpy(name, linkage_name);
    GtkMl_S *program = new_program(ctx, &lambda->span, name, 0, lambda->value.s_lambda.args, lambda->value.s_lambda.body, lambda->value.s_lambda.capture);
    basic_block->exec[basic_block->len_exec].imm64 = program;
    ++basic_block->len_exec;

    if (!compile_body(ctx, b, basic_block, err, lambda->value.s_lambda.body)) {
        return 0;
    }

    if (ret) {
        if (!gtk_ml_build_ret(ctx, b, basic_block, err)) {
            return 0;
        }
    }

    return 1;
}

gboolean gtk_ml_build_halt(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, const char **err) {
    (void) ctx;
    (void) b;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].arith.cond = 0;
    basic_block->exec[basic_block->len_exec].arith.category = GTKML_I_ARITH;
    basic_block->exec[basic_block->len_exec].arith.opcode = GTKML_IA_HALT;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_push_extended_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, const char **err, GtkMl_S *imm64) {
    if ((uintptr_t) imm64 < ((uintptr_t) 1 << 32)) {
        return gtk_ml_build_push_imm(ctx, b, basic_block, err, imm64);
    }

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = 0;
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_EI_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_EII_PUSH_EXT_IMM;
    ++basic_block->len_exec;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].imm64 = imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_push_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, const char **err, GtkMl_S *imm64) {
    (void) ctx;
    (void) b;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = 0;
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_PUSH_IMM;
    basic_block->exec[basic_block->len_exec].imm.imm = (uintptr_t) imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_push_extended_addr(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, const char **err, GtkMl_S *linkage_name) {
    if ((uintptr_t) linkage_name < ((uintptr_t) 1 << 32)) {
        return gtk_ml_build_push_addr(ctx, b, basic_block, err, linkage_name);
    }

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = 0;
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_EI_IMM | GTKML_EI_IMM_EXTERN;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_EII_PUSH_EXT_IMM;
    ++basic_block->len_exec;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].imm64 = linkage_name;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_push_addr(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, const char **err, GtkMl_S *linkage_name) {
    (void) ctx;
    (void) b;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = 0;
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM | GTKML_I_IMM_EXTERN;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_PUSH_IMM;

    basic_block->exec[basic_block->len_exec].imm.imm = (uintptr_t) linkage_name;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_get_extended_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, const char **err, GtkMl_S *imm64) {
    if ((uintptr_t) imm64 < ((uintptr_t) 1 << 32)) {
        return gtk_ml_build_get_imm(ctx, b, basic_block, err, imm64);
    }

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = 0;
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_EI_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_EII_GET_EXT_IMM;
    ++basic_block->len_exec;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].imm64 = imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_get_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, const char **err, GtkMl_S *imm64) {
    (void) ctx;
    (void) b;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = 0;
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_GET_IMM;
    basic_block->exec[basic_block->len_exec].imm.imm = (uintptr_t) imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_call_extended_ffi(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, const char **err, GtkMl_S *imm64) {
    if ((uintptr_t) imm64 < ((uintptr_t) 1 << 48)) {
        return gtk_ml_build_call_ffi(ctx, b, basic_block, err, imm64);
    }

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].br.cond = 0;
    basic_block->exec[basic_block->len_exec].br.category = GTKML_EI_BR;
    basic_block->exec[basic_block->len_exec].br.opcode = GTKML_EIBR_CALL_EXT_FFI;
    ++basic_block->len_exec;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].imm64 = imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_call_ffi(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, const char **err, GtkMl_S *imm64) {
    (void) ctx;
    (void) b;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].br.cond = 0;
    basic_block->exec[basic_block->len_exec].br.category = GTKML_I_BR;
    basic_block->exec[basic_block->len_exec].br.opcode = GTKML_IBR_CALL_FFI;
    basic_block->exec[basic_block->len_exec].br.imm = (uintptr_t) imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_call_extended(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, const char **err, GtkMl_S *program) {
    if ((uintptr_t) program < ((uintptr_t) 1 << 48)) {
        return gtk_ml_build_call(ctx, b, basic_block, err, program);
    }

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].imm64 = 0;
    basic_block->exec[basic_block->len_exec].br.cond = 0;
    basic_block->exec[basic_block->len_exec].br.category = GTKML_EI_BR;
    basic_block->exec[basic_block->len_exec].br.opcode = GTKML_EIBR_CALL_EXT;
    ++basic_block->len_exec;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].imm64 = program;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_call(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, const char **err, GtkMl_S *program) {
    (void) ctx;
    (void) b;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].imm64 = 0;
    basic_block->exec[basic_block->len_exec].br.cond = 0;
    basic_block->exec[basic_block->len_exec].br.category = GTKML_I_BR;
    basic_block->exec[basic_block->len_exec].br.opcode = GTKML_IBR_CALL;
    basic_block->exec[basic_block->len_exec].br.imm = (uintptr_t) program;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_ret(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, const char **err) {
    (void) ctx;
    (void) b;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].br.cond = 0;
    basic_block->exec[basic_block->len_exec].br.category = GTKML_I_BR;
    basic_block->exec[basic_block->len_exec].br.opcode = GTKML_IBR_RET;
    ++basic_block->len_exec;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    return 1;
}

gboolean gtk_ml_compile(GtkMl_Context *ctx, GtkMl_Builder *b, const char **err, GtkMl_S *lambda) {
    const char *linkage_name = "_start";
    GtkMl_BasicBlock *basic_block = gtk_ml_append_basic_block(b, linkage_name);

    if (!compile_program(ctx, b, basic_block, err, linkage_name, lambda, 0)) {
        return 0;
    }

    if (!gtk_ml_build_halt(ctx, b, basic_block, err)) {
        return 0;
    }
    
    return 1;
}

void gtk_ml_push(GtkMl_Context *ctx, GtkMl_S *value) {
    ctx->vm->stack[ctx->vm->reg[GTKML_R_SP].sp].s_expr = value;
    ctx->vm->reg[GTKML_R_SP].sp++;
}

GtkMl_S *gtk_ml_pop(GtkMl_Context *ctx) {
    --ctx->vm->reg[GTKML_R_SP].sp;
    return ctx->vm->stack[ctx->vm->reg[GTKML_R_SP].sp].s_expr;
}

GtkMl_S *gtk_ml_peek(GtkMl_Context *ctx) {
    return ctx->vm->stack[ctx->vm->reg[GTKML_R_SP].sp - 1].s_expr;
}

void gtk_ml_enter(GtkMl_Context *ctx) {
    gboolean enabled = gtk_ml_disable_gc(ctx);
    ctx->bindings = new_list(ctx, NULL, new_nil(ctx, NULL), ctx->bindings);
    gtk_ml_enable_gc(ctx, enabled);
}

void gtk_ml_leave(GtkMl_Context *ctx) {
    ctx->bindings = gtk_ml_cdr(ctx->bindings);
}

void gtk_ml_define(GtkMl_Context *ctx, GtkMl_S *key, GtkMl_S *value) {
    gboolean enabled = gtk_ml_disable_gc(ctx);
    GtkMl_S *new_context = new_map(ctx, NULL, key, new_map(ctx, NULL, value,
        *ctx->top_scope));
    *ctx->top_scope = new_context;
    gtk_ml_enable_gc(ctx, enabled);
}

void gtk_ml_bind(GtkMl_Context *ctx, GtkMl_S *key, GtkMl_S *value) {
    gboolean enabled = gtk_ml_disable_gc(ctx);
    GtkMl_S *new_context = new_map(ctx, NULL, key, new_map(ctx, NULL, value,
        ctx->bindings->value.s_map.car));
    ctx->bindings->value.s_map.car = new_context;
    gtk_ml_enable_gc(ctx, enabled);
}

GTKML_PRIVATE GtkMl_S *map_find(GtkMl_S *map, GtkMl_S *key) {
    if (map->kind == GTKML_S_NIL) {
        return NULL;
    }

    if (gtk_ml_equal(gtk_ml_car(map), key)) {
        return gtk_ml_cdar(map);
    }

    return map_find(gtk_ml_cddr(map), key);
}

GTKML_PRIVATE GtkMl_S *get_inner(GtkMl_S *bindings, GtkMl_S *key) {
    if (bindings->kind == GTKML_S_NIL) {
        return NULL;
    }

    GtkMl_S *value = map_find(gtk_ml_car(bindings), key);
    if (value) {
        return value;
    }

    return get_inner(gtk_ml_cdr(bindings), key);
}

GtkMl_S *gtk_ml_get(GtkMl_Context *ctx, GtkMl_S *key) {
    return get_inner(ctx->bindings, key);
}

GTKML_PRIVATE GtkMl_S *local_scope(GtkMl_Context *ctx) {
    GtkMl_S *local = new_nil(ctx, NULL);
    GtkMl_S **last = &local;

    GtkMl_S *bindings = ctx->bindings;
    while (bindings->kind != GTKML_S_NIL) {
        GtkMl_S *scope = gtk_ml_car(bindings);
        while (scope->kind != GTKML_S_NIL) {
            GtkMl_S *value = gtk_ml_car(scope);
            GtkMl_S *new = new_map(ctx, NULL, value, *last);
            *last = new;
            last = &gtk_ml_cdr(new);
            scope = gtk_ml_cdr(scope);
        }
        bindings = gtk_ml_cdr(bindings);
    }

    return local;
}

GTKML_PRIVATE void mark_value(GtkMl_S *s) {
    if (s->flags & GTKML_FLAG_REACHABLE) {
        return;
    }

    s->flags |= GTKML_FLAG_REACHABLE;
    switch (s->kind) {
    case GTKML_S_NIL:
    case GTKML_S_TRUE:
    case GTKML_S_FALSE:
    case GTKML_S_INT:
    case GTKML_S_FLOAT:
    case GTKML_S_STRING:
    case GTKML_S_KEYWORD:
    case GTKML_S_SYMBOL:
    case GTKML_S_ADDRESS:
    case GTKML_S_LIGHTDATA:
    case GTKML_S_FFI:
        break;
    case GTKML_S_USERDATA:
        mark_value(s->value.s_userdata.keep);
        break;
    case GTKML_S_LIST:
    case GTKML_S_MAP:
        mark_value(gtk_ml_car(s));
        mark_value(gtk_ml_cdr(s));
        break;
    case GTKML_S_VARIADIC:
        mark_value(s->value.s_var.expr);
        break;
    case GTKML_S_PROGRAM:
        mark_value(s->value.s_program.args);
        mark_value(s->value.s_program.capture);
        break;
    case GTKML_S_LAMBDA:
        mark_value(s->value.s_lambda.args);
        mark_value(s->value.s_lambda.body);
        mark_value(s->value.s_lambda.capture);
        break;
    case GTKML_S_MACRO:
        mark_value(s->value.s_macro.args);
        mark_value(s->value.s_macro.body);
        mark_value(s->value.s_macro.capture);
        break;
    }
}

GTKML_PRIVATE void mark(GtkMl_Context *ctx) {
    for (size_t sp = 0; sp < ctx->vm->reg[GTKML_R_SP].sp; sp++) {
        mark_value(ctx->vm->stack[sp].s_expr);
    }
    mark_value(ctx->bindings);
}

GTKML_PRIVATE void delete(GtkMl_Context *ctx, GtkMl_S *s) {
    if ((s->flags & GTKML_FLAG_REACHABLE) || (s->flags & GTKML_FLAG_DELETE)) {
        return;
    }

    s->flags |= GTKML_FLAG_DELETE;

    switch (s->kind) {
    case GTKML_S_NIL:
    case GTKML_S_TRUE:
    case GTKML_S_FALSE:
    case GTKML_S_INT:
    case GTKML_S_FLOAT:
    case GTKML_S_KEYWORD:
    case GTKML_S_SYMBOL:
    case GTKML_S_ADDRESS:
    case GTKML_S_LIGHTDATA:
    case GTKML_S_FFI:
        break;
    case GTKML_S_STRING:
        // const cast required
        free((void *) s->value.s_string.ptr);
        break;
    case GTKML_S_USERDATA:
        s->value.s_userdata.del(ctx, s->value.s_userdata.userdata);
        break;
    case GTKML_S_LIST:
    case GTKML_S_MAP:
        delete(ctx, gtk_ml_cdr(s));
        delete(ctx, gtk_ml_car(s));
        break;
    case GTKML_S_VARIADIC:
        delete(ctx, s->value.s_var.expr);
        break;
    case GTKML_S_PROGRAM:
        free((void *) s->value.s_program.linkage_name);
        delete(ctx, s->value.s_program.args);
        delete(ctx, s->value.s_program.capture);
        break;
    case GTKML_S_LAMBDA:
        delete(ctx, s->value.s_lambda.args);
        delete(ctx, s->value.s_lambda.body);
        delete(ctx, s->value.s_lambda.capture);
        break;
    case GTKML_S_MACRO:
        delete(ctx, s->value.s_macro.args);
        delete(ctx, s->value.s_macro.body);
        delete(ctx, s->value.s_macro.capture);
        break;
    }
    free(s);
    --ctx->n_values;
}

GTKML_PRIVATE void del(GtkMl_Context *ctx, GtkMl_S *s) {
    switch (s->kind) {
    case GTKML_S_NIL:
    case GTKML_S_TRUE:
    case GTKML_S_FALSE:
    case GTKML_S_INT:
    case GTKML_S_FLOAT:
    case GTKML_S_KEYWORD:
    case GTKML_S_SYMBOL:
    case GTKML_S_LIGHTDATA:
    case GTKML_S_LIST:
    case GTKML_S_MAP:
    case GTKML_S_VARIADIC:
    case GTKML_S_LAMBDA:
    case GTKML_S_ADDRESS:
    case GTKML_S_MACRO:
    case GTKML_S_FFI:
        break;
    case GTKML_S_STRING:
        // const cast required
        free((void *) s->value.s_string.ptr);
        break;
    case GTKML_S_PROGRAM:
        free((void *) s->value.s_program.linkage_name);
        break;
    case GTKML_S_USERDATA:
        s->value.s_userdata.del(ctx, s->value.s_userdata.userdata);
        break;
    }
    free(s);
    --ctx->n_values;
}

GTKML_PRIVATE void sweep(GtkMl_Context *ctx) {
    GtkMl_S *prev = ctx->first;
    GtkMl_S *value = prev->next;
    while (value) {
        if (value->flags & GTKML_FLAG_REACHABLE) {
            value->flags &= ~GTKML_FLAG_REACHABLE;
            prev = value;
            value = value->next;
        } else {
            prev->next = value->next;
            delete(ctx, value);
            value = prev->next;
        }
    }
}

// simple mark & sweep gc
void gtk_ml_collect(GtkMl_Context *ctx) {
    if (!ctx->gc_enabled) {
        return;
    }

    size_t n_values = ctx->n_values;
    mark(ctx);
    sweep(ctx);
    ctx->m_values = 2 * n_values;
}

gboolean gtk_ml_disable_gc(GtkMl_Context *ctx) {
    gboolean enabled = ctx->gc_enabled;
    ctx->gc_enabled = 0;
    return enabled;
}

void gtk_ml_enable_gc(GtkMl_Context *ctx, gboolean enabled) {
    ctx->gc_enabled = enabled;
}

gboolean gtk_ml_dumpf(FILE *stream, const char **err, GtkMl_S *expr) {
    switch (expr->kind) {
    case GTKML_S_NIL:
        fprintf(stream, "#nil");
        return 1;
    case GTKML_S_TRUE:
        fprintf(stream, "#t");
        return 1;
    case GTKML_S_FALSE:
        fprintf(stream, "#f");
        return 1;
    case GTKML_S_INT:
        fprintf(stream, "%ld", expr->value.s_int.value);
        return 1;
    case GTKML_S_FLOAT:
        fprintf(stream, "%f", expr->value.s_float.value);
        return 1;
    case GTKML_S_STRING:
        fprintf(stream, "\"%.*s\"", (int) expr->value.s_string.len, expr->value.s_string.ptr);
        return 1;
    case GTKML_S_KEYWORD:
        fprintf(stream, ":%.*s", (int) expr->value.s_keyword.len, expr->value.s_keyword.ptr);
        return 1;
    case GTKML_S_SYMBOL:
        fprintf(stream, "'%.*s", (int) expr->value.s_symbol.len, expr->value.s_symbol.ptr);
        return 1;
    case GTKML_S_LIST:
        fprintf(stream, "(");
        while (expr->kind != GTKML_S_NIL) {
            gtk_ml_dumpf(stream, err, gtk_ml_car(expr));
            expr = gtk_ml_cdr(expr);
            if (expr->kind != GTKML_S_NIL) {
                fprintf(stream, " ");
            }
        }
        fprintf(stream, ")");
        return 1;
    case GTKML_S_MAP:
        fprintf(stream, "{");
        while (expr->kind != GTKML_S_NIL) {
            gtk_ml_dumpf(stream, err, gtk_ml_car(expr));
            expr = gtk_ml_cdr(expr);
            if (expr->kind != GTKML_S_NIL) {
                fprintf(stream, " ");
            }
        }
        fprintf(stream, "}");
        return 1;
    case GTKML_S_VARIADIC:
        fprintf(stream, "...");
        return gtk_ml_dumpf(stream, err, expr->value.s_var.expr);
    case GTKML_S_LAMBDA: {
        fprintf(stream, "(lambda ");
        if (!gtk_ml_dumpf(stream, err, expr->value.s_lambda.args)) {
            return 0;
        }
        fprintf(stream, " ");
        GtkMl_S *body = expr->value.s_lambda.body;
        while (body->kind != GTKML_S_NIL) {
            if (!gtk_ml_dumpf(stream, err, gtk_ml_car(body))) {
                return 0;
            }
            body = gtk_ml_cdr(body);
            if (body->kind != GTKML_S_NIL) {
                fprintf(stream, " ");
            }
        }
        fprintf(stream, ")");
        return 1;
    }
    case GTKML_S_PROGRAM:
        fprintf(stream, "(program \"%s\" 0x%lx ", expr->value.s_program.linkage_name, expr->value.s_program.addr);
        if (!gtk_ml_dumpf(stream, err, expr->value.s_program.args)) {
            return 0;
        }
        fprintf(stream, " ");
        GtkMl_S *body = expr->value.s_program.body;
        while (body->kind != GTKML_S_NIL) {
            if (!gtk_ml_dumpf(stream, err, gtk_ml_car(body))) {
                return 0;
            }
            body = gtk_ml_cdr(body);
            if (body->kind != GTKML_S_NIL) {
                fprintf(stream, " ");
            }
        }
        fprintf(stream, ")");
        return 1;
    case GTKML_S_ADDRESS:
        fprintf(stream, "(address 0x%lx)", expr->value.s_address.addr);
        return 1;
    case GTKML_S_MACRO: {
        fprintf(stream, "(macro ");
        gtk_ml_dumpf(stream, err, expr->value.s_macro.args);
        fprintf(stream, " ");
        GtkMl_S *body = expr->value.s_macro.body;
        while (body->kind != GTKML_S_NIL) {
            gtk_ml_dumpf(stream, err, gtk_ml_car(body));
            body = gtk_ml_cdr(body);
            if (body->kind != GTKML_S_NIL) {
                fprintf(stream, " ");
            }
        }
        fprintf(stream, ")");
        return 1;
    }
    case GTKML_S_FFI:
        fprintf(stream, "%p", (void *) expr->value.s_ffi.function);
        return 1;
    case GTKML_S_LIGHTDATA:
        fprintf(stream, "%p", expr->value.s_lightdata.userdata);
        return 1;
    case GTKML_S_USERDATA:
        fprintf(stream, "%p", expr->value.s_lightdata.userdata);
        return 1;
    default:
        *err = GTKML_ERR_INVALID_SEXPR;
        return 0;
    }
}

char *gtk_ml_dumpsn(char *ptr, size_t n, const char **err, GtkMl_S *expr) {
    (void) ptr;
    (void) n;
    (void) err;
    (void) expr;
    return NULL;
}

char *gtk_ml_dumpsnr(char *ptr, size_t n, const char **err, GtkMl_S *expr) {
    (void) ptr;
    (void) n;
    (void) err;
    (void) expr;
    return NULL;
}

gboolean gtk_ml_dumpf_program(GtkMl_Context *ctx, FILE *stream, const char **err) {
    if (!ctx->vm->exec) {
        return 0;
    }

    for (size_t pc = 0; pc < ctx->vm->exec_len;) {
        GtkMl_Instruction instr = ctx->vm->exec[pc];
        printf("%zx ", 8 * pc);
        if (S_CATEGORY[instr.gen.category]) {
            fprintf(stream, "%s ", S_CATEGORY[instr.gen.category][instr.arith.opcode]);
            if (instr.gen.category & GTKML_I_EXTENDED) {
                if (ctx->vm->exec[pc + 1].imm64 && !gtk_ml_dumpf(stream, err, ctx->vm->exec[pc + 1].imm64)) {
                    return 0;
                }
            } else if (instr.gen.category == GTKML_I_IMM) {
                if (instr.imm.imm && !gtk_ml_dumpf(stream, err, (GtkMl_S *) instr.imm.imm)) {
                    return 0;
                }
            } else if (instr.gen.category == GTKML_I_BR) {
                
                if (instr.br.imm && !gtk_ml_dumpf(stream, err, (GtkMl_S *) instr.br.imm)) {
                    return 0;
                }
            }
            fprintf(stream, "\n");
        } else if (instr.gen.category == GTKML_EI_EXPORT) {
            fprintf(stream, "EXPORT ");
            if (!gtk_ml_dumpf(stream, err, (GtkMl_S *) ctx->vm->exec[pc + 1].imm64)) {
                return 0;
            }
            fprintf(stream, "\n");
        } else if (instr.gen.category & GTKML_EI_IMM_EXTERN) {
            fprintf(stream, "UNKNOWN EXTERN %lx ", instr.instr);
            if (!gtk_ml_dumpf(stream, err, ctx->vm->exec[pc + 1].imm64)) {
                return 0;
            }
            fprintf(stream, "\n");
        } else {
            if (instr.gen.category & GTKML_I_EXTENDED) {
                fprintf(stream, "UNKNOWN %lx ", instr.instr);
                if (!gtk_ml_dumpf(stream, err, ctx->vm->exec[pc + 1].imm64)) {
                    return 0;
                }
                fprintf(stream, "\n");
            } else {
                fprintf(stream, "%lx\n", instr.instr);
            }
        }
        if (instr.gen.category & GTKML_I_EXTENDED) {
            pc += 2;
        } else {
            ++pc;
        }
    }

    return 1;
}

char *gtk_ml_dumpsn_program(GtkMl_Context *ctx, char *ptr, size_t n, const char **err) {
    (void) ctx;
    (void) ptr;
    (void) n;
    (void) err;
    return NULL;
}

char *gtk_ml_dumpsnr_program(GtkMl_Context *ctx, char *ptr, size_t n, const char **err) {
    (void) ctx;
    (void) ptr;
    (void) n;
    (void) err;
    return NULL;
}

GTKML_PRIVATE GtkMl_S *map_finds(GtkMl_S *map, const char *keyword) {
    if (map->kind == GTKML_S_NIL) {
        return NULL;
    }

    if (gtk_ml_car(map)->kind == GTKML_S_KEYWORD) {
        int cmp = strncmp(
            gtk_ml_car(map)->value.s_keyword.ptr,
            keyword,
            gtk_ml_car(map)->value.s_keyword.len
        );
        if (cmp == 0) {
            return gtk_ml_cdar(map);
        }
    }

    return map_finds(gtk_ml_cddr(map), keyword);
}

GTKML_PRIVATE void activate_lambda(GtkApplication* app, gpointer userdata) {
    (void) app;

    GtkMl_S *args = userdata;
    GtkMl_S *ctx_expr = gtk_ml_car(args);
    GtkMl_S *app_expr = gtk_ml_cdar(args);
    GtkMl_S *lambda_expr = gtk_ml_cddar(args);

    GtkMl_Context *ctx = ctx_expr->value.s_lightdata.userdata;

    gboolean enabled = gtk_ml_disable_gc(ctx);
    const char *err;
    GtkMl_S *result = gtk_ml_call(ctx, &err, lambda_expr, new_list(ctx, NULL, lambda_expr, new_list(ctx, NULL, app_expr, new_nil(ctx, NULL))));
    if (result) {
        app_expr->value.s_userdata.keep = new_list(ctx, NULL, result, app_expr->value.s_userdata.keep);
    }
    gtk_ml_enable_gc(ctx, enabled);
}

GTKML_PRIVATE void activate_program(GtkApplication* app, gpointer userdata) {
    (void) app;

    GtkMl_S *args = userdata;
    GtkMl_S *ctx_expr = gtk_ml_car(args);
    GtkMl_S *app_expr = gtk_ml_cdar(args);
    GtkMl_S *program_expr = gtk_ml_cddar(args);

    GtkMl_Context *ctx = ctx_expr->value.s_lightdata.userdata;

    gboolean enabled = gtk_ml_disable_gc(ctx);
    const char *err;
    uint64_t pc = ctx->vm->reg[GTKML_R_PC].pc;
    uint64_t flags = ctx->vm->reg[GTKML_R_FLAGS].flags & GTKML_F_TOPCALL;
    if (gtk_ml_run_program(ctx, &err, program_expr, new_list(ctx, NULL, app_expr, new_nil(ctx, NULL)))) {
        GtkMl_S *result = gtk_ml_pop(ctx);
        app_expr->value.s_userdata.keep = new_list(ctx, NULL, result, app_expr->value.s_userdata.keep);
    }
    ctx->vm->reg[GTKML_R_FLAGS].flags &= ~GTKML_F_TOPCALL;
    ctx->vm->reg[GTKML_R_FLAGS].flags |= flags;
    ctx->vm->reg[GTKML_R_PC].pc = pc;
    gtk_ml_enable_gc(ctx, enabled);
}

GTKML_PRIVATE GtkMl_S *builtin_lambda(GtkMl_Context *ctx, const char **err, GtkMl_S *expr) {
    GtkMl_S *args = gtk_ml_cdr(expr);

    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = GTKML_ERR_ARITY_ERROR;
        return NULL;
    }

    GtkMl_S *lambda_args = gtk_ml_car(args);
    GtkMl_S *lambda_body = gtk_ml_cdr(args);

    return new_lambda(ctx, &expr->span, lambda_args, lambda_body, local_scope(ctx));
}

GTKML_PRIVATE GtkMl_S *builtin_macro(GtkMl_Context *ctx, const char **err, GtkMl_S *expr) {
    GtkMl_S *args = gtk_ml_cdr(expr);

    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = GTKML_ERR_ARITY_ERROR;
        return NULL;
    }

    GtkMl_S *macro_args = gtk_ml_car(args);
    GtkMl_S *macro_body = gtk_ml_cdr(args);

    return new_macro(ctx, &expr->span, macro_args, macro_body, local_scope(ctx));
}

GTKML_PRIVATE GtkMl_S *builtin_define(GtkMl_Context *ctx, const char **err, GtkMl_S *expr) {
    GtkMl_S *args = gtk_ml_cdr(expr);

    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = GTKML_ERR_ARITY_ERROR;
        return NULL;
    }

    GtkMl_S *key = gtk_ml_car(args);
    if (key->kind == GTKML_S_SYMBOL) {
        GtkMl_S *value = gtk_ml_cdar(args);

        gtk_ml_define(ctx, key, value);
    } else if (key->kind == GTKML_S_LIST) {
        GtkMl_S *lambda_name = gtk_ml_car(key);
        GtkMl_S *lambda_args = gtk_ml_cdr(key);
        GtkMl_S *lambda_body = gtk_ml_cdr(args);
        GtkMl_S *value = new_lambda(ctx, &expr->span, lambda_args, lambda_body, new_nil(ctx, NULL));

        gtk_ml_define(ctx, lambda_name, value);
    } else {
        *err = GTKML_ERR_TYPE_ERROR;
        return NULL;
    }
    return new_nil(ctx, &expr->span);
}

GTKML_PRIVATE GtkMl_S *builtin_define_macro(GtkMl_Context *ctx, const char **err, GtkMl_S *expr) {
    GtkMl_S *args = gtk_ml_cdr(expr);

    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = GTKML_ERR_ARITY_ERROR;
        return NULL;
    }

    GtkMl_S *key = gtk_ml_car(args);
    GtkMl_S *macro_name = gtk_ml_car(key);
    GtkMl_S *macro_args = gtk_ml_cdr(key);
    GtkMl_S *macro_body = gtk_ml_cdr(args);
    GtkMl_S *value = new_macro(ctx, &expr->span, macro_args, macro_body, new_nil(ctx, NULL));

    gtk_ml_define(ctx, macro_name, value);
    return new_nil(ctx, &expr->span);
}

GTKML_PRIVATE GtkMl_S *builtin_application(GtkMl_Context *ctx, const char **err, GtkMl_S *expr) {
    GtkMl_S *args = gtk_ml_cdr(expr);

    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL
            || gtk_ml_cddr(args)->kind == GTKML_S_NIL) {
        *err = GTKML_ERR_ARITY_ERROR;
        return NULL;
    }

    GtkMl_S *id_expr = gtk_ml_exec(ctx, err, gtk_ml_car(args));
    if (!id_expr) {
        return NULL;
    }
    GtkMl_S *flags_expr = gtk_ml_exec(ctx, err, gtk_ml_cdar(args));
    if (!flags_expr) {
        return NULL;
    }

    gboolean enabled = gtk_ml_disable_gc(ctx);
    GtkApplication *app = gtk_application_new(id_expr->value.s_string.ptr, flags_expr->value.s_int.value);
    GtkMl_S *app_expr = new_userdata(ctx, &expr->span, app, gtk_ml_object_unref);

    GtkMl_S *map_expr = gtk_ml_exec(ctx, err, gtk_ml_cddar(args));
    if (!map_expr) {
        return NULL;
    }
    GtkMl_S *activate = map_finds(map_expr, "activate");
    if (activate) {
        GtkMl_S *ctx_expr = new_lightdata(ctx, NULL, ctx);
        GtkMl_S *userdata = new_list(ctx, NULL, ctx_expr, new_list(ctx, NULL, app_expr, new_list(ctx, NULL, activate, new_nil(ctx, NULL))));
        app_expr->value.s_userdata.keep = new_list(ctx, &app_expr->span, userdata, app_expr->value.s_userdata.keep);
        g_signal_connect(app, "activate", G_CALLBACK(activate_lambda), userdata);
    }
    gtk_ml_enable_gc(ctx, enabled);

    return app_expr;
}

GTKML_PRIVATE GtkMl_S *builtin_new_window(GtkMl_Context *ctx, const char **err, GtkMl_S *expr) {
    (void) ctx;

    GtkMl_S *args = gtk_ml_cdr(expr);

    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL
            || gtk_ml_cddr(args)->kind == GTKML_S_NIL
            || gtk_ml_cdddr(args)->kind == GTKML_S_NIL) {
        *err = GTKML_ERR_ARITY_ERROR;
        return NULL;
    }

    GtkMl_S *app_expr = gtk_ml_exec(ctx, err, gtk_ml_car(args));
    if (!app_expr) {
        return NULL;
    }
    GtkMl_S *title_expr = gtk_ml_exec(ctx, err, gtk_ml_cdar(args));
    if (!title_expr) {
        return NULL;
    }
    GtkMl_S *width_expr = gtk_ml_exec(ctx, err, gtk_ml_cddar(args));
    if (!width_expr) {
        return NULL;
    }
    GtkMl_S *height_expr = gtk_ml_exec(ctx, err, gtk_ml_cdddar(args));
    if (!height_expr) {
        return NULL;
    }

    GtkWidget *window = gtk_application_window_new(app_expr->value.s_userdata.userdata);
    gtk_window_set_title(GTK_WINDOW(window), title_expr->value.s_string.ptr);
    gtk_window_set_default_size(GTK_WINDOW(window), width_expr->value.s_int.value, height_expr->value.s_int.value);
    gtk_widget_show_all(window);

    return new_lightdata(ctx, &expr->span, window);
}

GTKML_PRIVATE GtkMl_S *vm_map(GtkMl_Context *ctx, const char **err, GtkMl_S *expr) {
    (void) err;

    int64_t n = expr->value.s_int.value;

    GtkMl_S *map = new_nil(ctx, NULL);

    while (n--) {
        GtkMl_S *expr = gtk_ml_pop(ctx);
        map = new_map(ctx, NULL, expr, map);
    }

    return map;
}

GTKML_PRIVATE GtkMl_S *vm_application(GtkMl_Context *ctx, const char **err, GtkMl_S *expr) {
    (void) err;

    int64_t n = expr->value.s_int.value;

    GtkMl_S *args = new_nil(ctx, NULL);

    while (n--) {
        args = new_list(ctx, NULL, gtk_ml_pop(ctx), args);
    }

    args = gtk_ml_cdr(args);

    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL
            || gtk_ml_cddr(args)->kind == GTKML_S_NIL) {
        *err = GTKML_ERR_ARITY_ERROR;
        return NULL;
    }

    GtkMl_S *id_expr = gtk_ml_car(args);
    if (!id_expr) {
        return NULL;
    }
    GtkMl_S *flags_expr = gtk_ml_cdar(args);
    if (!flags_expr) {
        return NULL;
    }

    gboolean enabled = gtk_ml_disable_gc(ctx);
    GtkApplication *app = gtk_application_new(id_expr->value.s_string.ptr, flags_expr->value.s_int.value);
    GtkMl_S *app_expr = new_userdata(ctx, &expr->span, app, gtk_ml_object_unref);

    GtkMl_S *map_expr = gtk_ml_cddar(args);
    if (!map_expr) {
        return NULL;
    }

    GtkMl_S *activate = map_finds(map_expr, "activate");
    if (activate) {
        GtkMl_S *ctx_expr = new_lightdata(ctx, NULL, ctx);
        GtkMl_S *userdata = new_list(ctx, NULL, ctx_expr, new_list(ctx, NULL, app_expr, new_list(ctx, NULL, activate, new_nil(ctx, NULL))));
        app_expr->value.s_userdata.keep = new_list(ctx, &app_expr->span, userdata, app_expr->value.s_userdata.keep);
        g_signal_connect(app, "activate", G_CALLBACK(activate_program), userdata);
    }
    gtk_ml_enable_gc(ctx, enabled);

    return app_expr;
}

GTKML_PRIVATE GtkMl_S *vm_new_window(GtkMl_Context *ctx, const char **err, GtkMl_S *expr) {
    (void) ctx;
    (void) err;

    int64_t n = expr->value.s_int.value;

    GtkMl_S *args = new_nil(ctx, NULL);

    while (n--) {
        args = new_list(ctx, NULL, gtk_ml_pop(ctx), args);
    }

    args = gtk_ml_cdr(args);

    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL
            || gtk_ml_cddr(args)->kind == GTKML_S_NIL
            || gtk_ml_cdddr(args)->kind == GTKML_S_NIL) {
        *err = GTKML_ERR_ARITY_ERROR;
        return NULL;
    }

    GtkMl_S *app_expr = gtk_ml_car(args);
    if (!app_expr) {
        return NULL;
    }
    GtkMl_S *title_expr = gtk_ml_cdar(args);
    if (!title_expr) {
        return NULL;
    }
    GtkMl_S *width_expr = gtk_ml_cddar(args);
    if (!width_expr) {
        return NULL;
    }
    GtkMl_S *height_expr = gtk_ml_cdddar(args);
    if (!height_expr) {
        return NULL;
    }

    GtkWidget *window = gtk_application_window_new(app_expr->value.s_userdata.userdata);
    gtk_window_set_title(GTK_WINDOW(window), title_expr->value.s_string.ptr);
    gtk_window_set_default_size(GTK_WINDOW(window), width_expr->value.s_int.value, height_expr->value.s_int.value);
    gtk_widget_show_all(window);

    return new_lightdata(ctx, &expr->span, window);
}

GtkMl_S *gtk_ml_exec(GtkMl_Context *ctx, const char **err, GtkMl_S *expr) {
    if (!expr) {
        *err = GTKML_ERR_NULL_ERROR;
        return NULL;
    }

    switch (expr->kind) {
    case GTKML_S_NIL:
    case GTKML_S_TRUE:
    case GTKML_S_FALSE:
    case GTKML_S_INT:
    case GTKML_S_FLOAT:
    case GTKML_S_STRING:
    case GTKML_S_KEYWORD:
    case GTKML_S_LIGHTDATA:
    case GTKML_S_USERDATA:
    case GTKML_S_LAMBDA:
    case GTKML_S_PROGRAM:
    case GTKML_S_ADDRESS:
    case GTKML_S_MACRO:
    case GTKML_S_FFI:
        return expr;
    case GTKML_S_SYMBOL: {
        GtkMl_S *result = gtk_ml_get(ctx, expr);
        if (!result) {
            *err = GTKML_ERR_BINDING_ERROR;
            return NULL;
        }
        return result;
    }
    case GTKML_S_LIST: {
        GtkMl_S *function = gtk_ml_exec(ctx, err, gtk_ml_car(expr));
        if (!function) {
            return NULL;
        }

        return gtk_ml_call(ctx, err, function, expr);
    }
    case GTKML_S_MAP: {
        GtkMl_S *car = gtk_ml_exec(ctx, err, gtk_ml_car(expr));
        if (!car) {
            return NULL;
        }
        GtkMl_S *cdr = gtk_ml_exec(ctx, err, gtk_ml_cdr(expr));
        if (!cdr) {
            return NULL;
        }
        return new_map(ctx, &expr->span, car, cdr);
    }
    case GTKML_S_VARIADIC:
        *err = GTKML_ERR_VARIADIC_ERROR;
        return NULL;
    default:
        *err = GTKML_ERR_INVALID_SEXPR;
        return NULL;
    }
}

GTKML_PRIVATE GtkMl_S *call_lambda(GtkMl_Context *ctx, const char **err, GtkMl_S *lambda, GtkMl_S *expr) {
    GtkMl_S *args;
    if (expr->kind == GTKML_S_NIL) {
        args = expr;
    } else {
        args = gtk_ml_cdr(expr);
    }
    GtkMl_S *capture = lambda->value.s_lambda.capture;
    ctx->bindings = new_list(ctx, NULL, capture, ctx->bindings);

    gtk_ml_enter(ctx);

    GtkMl_S *params = lambda->value.s_lambda.args;

    while (params->kind != GTKML_S_NIL && args->kind != GTKML_S_NIL) {
        GtkMl_S *arg = gtk_ml_exec(ctx, err, gtk_ml_car(args));
        if (!arg) {
            break;
        }
        gtk_ml_bind(ctx, gtk_ml_car(params), arg);
        params = gtk_ml_cdr(params);
        args = gtk_ml_cdr(args);
    }

    if (params->kind == GTKML_S_LIST) {
        GtkMl_S *param = gtk_ml_car(params);
        if (param->kind == GTKML_S_VARIADIC) {
            gtk_ml_bind(ctx, param->value.s_var.expr, new_nil(ctx, NULL));
        } else {
            *err = GTKML_ERR_ARITY_ERROR;
            return NULL;
        }
    }

    if (args->kind == GTKML_S_LIST) {
        *err = GTKML_ERR_ARITY_ERROR;
        return NULL;
    }

    GtkMl_S *result = new_nil(ctx, NULL);
    GtkMl_S *body = lambda->value.s_lambda.body;
    while (body->kind != GTKML_S_NIL) {
        result = gtk_ml_exec(ctx, err, gtk_ml_car(body));
        if (!result) {
            gtk_ml_leave(ctx);
            gtk_ml_leave(ctx);
            return NULL;
        }
        body = gtk_ml_cdr(body);
    }

    gtk_ml_leave(ctx);
    gtk_ml_leave(ctx);

    return result;
}

GTKML_PRIVATE GtkMl_S *call_macro(GtkMl_Context *ctx, const char **err, GtkMl_S *macro, GtkMl_S *expr) {
    GtkMl_S *args;
    if (expr->kind == GTKML_S_NIL) {
        args = expr;
    } else {
        args = gtk_ml_cdr(expr);
    }
    GtkMl_S *capture = macro->value.s_macro.capture;
    ctx->bindings = new_list(ctx, NULL, capture, ctx->bindings);

    gtk_ml_enter(ctx);

    GtkMl_S *params = macro->value.s_macro.args;

    while (params->kind != GTKML_S_NIL && args->kind != GTKML_S_NIL) {
        GtkMl_S *param = gtk_ml_car(params);
        if (param->kind == GTKML_S_VARIADIC) {
            gtk_ml_bind(ctx, param->value.s_var.expr, args);
            params = gtk_ml_cdr(params);
            while (args->kind != GTKML_S_NIL) {
                args = gtk_ml_cdr(args);
            }
        } else {
            GtkMl_S *arg = gtk_ml_car(args);
            gtk_ml_bind(ctx, param, arg);
            params = gtk_ml_cdr(params);
            args = gtk_ml_cdr(args);
        }
    }

    if (params->kind == GTKML_S_LIST) {
        GtkMl_S *param = gtk_ml_car(params);
        if (param->kind == GTKML_S_VARIADIC) {
            gtk_ml_bind(ctx, param->value.s_var.expr, new_nil(ctx, NULL));
        } else {
            *err = GTKML_ERR_ARITY_ERROR;
            return NULL;
        }
    }

    if (args->kind == GTKML_S_LIST) {
        *err = GTKML_ERR_ARITY_ERROR;
        return NULL;
    }

    GtkMl_S *result = new_nil(ctx, NULL);
    GtkMl_S *body = macro->value.s_macro.body;
    while (body->kind != GTKML_S_NIL) {
        result = gtk_ml_exec(ctx, err, gtk_ml_car(body));
        if (!result) {
            gtk_ml_leave(ctx);
            gtk_ml_leave(ctx);
            return NULL;
        }
        body = gtk_ml_cdr(body);
    }

    gtk_ml_leave(ctx);
    gtk_ml_leave(ctx);

    return result;
}

GTKML_PRIVATE GtkMl_S *call_ffi(GtkMl_Context *ctx, const char **err, GtkMl_S *ffi, GtkMl_S *expr) {
    return ffi->value.s_ffi.function(ctx, err, expr);
}

GtkMl_S *gtk_ml_call(GtkMl_Context *ctx, const char **err, GtkMl_S *function, GtkMl_S *expr) {
    GtkMl_S *args = new_nil(ctx, NULL);
    GtkMl_S **last = &args;

    while (expr->kind != GTKML_S_NIL) {
        GtkMl_S *arg = gtk_ml_car(expr);
        if (arg->kind == GTKML_S_VARIADIC) {
            expr = gtk_ml_exec(ctx, err, arg->value.s_var.expr);
            if (!expr) {
                return NULL;
            }
            continue;
        }
        GtkMl_S *new = new_list(ctx, NULL, arg, *last);
        *last = new;
        last = &gtk_ml_cdr(new);
        expr = gtk_ml_cdr(expr);
    }

    if (function->kind == GTKML_S_LAMBDA) {
        return call_lambda(ctx, err, function, args);
    } else if (function->kind == GTKML_S_MACRO) {
        return call_macro(ctx, err, function, args);
    } else if (function->kind == GTKML_S_FFI) {
        return call_ffi(ctx, err, function, args);
    } else if (function->kind == GTKML_S_PROGRAM) {
        *err = GTKML_ERR_UNIMPLEMENTED;
        return NULL;
    } else {
        *err = GTKML_ERR_TYPE_ERROR;
        return NULL;
    }
}

gboolean gtk_ml_equal(GtkMl_S *lhs, GtkMl_S *rhs) {
    if (lhs == rhs) {
        return 1;
    }

    if (lhs->kind != rhs->kind) {
        return 0;
    }

    switch (lhs->kind) {
    case GTKML_S_NIL:
        return 1;
    case GTKML_S_TRUE:
        return 1;
    case GTKML_S_FALSE:
        return 1;
    case GTKML_S_INT:
        return lhs->value.s_int.value == rhs->value.s_int.value;
    case GTKML_S_FLOAT:
        return lhs->value.s_float.value == rhs->value.s_float.value;
    case GTKML_S_STRING:
        return lhs->value.s_string.len == rhs->value.s_string.len && memcmp(lhs->value.s_string.ptr, rhs->value.s_string.ptr, lhs->value.s_string.len) == 0;
    case GTKML_S_KEYWORD:
        return lhs->value.s_keyword.len == rhs->value.s_keyword.len && memcmp(lhs->value.s_keyword.ptr, rhs->value.s_keyword.ptr, lhs->value.s_keyword.len) == 0;
    case GTKML_S_SYMBOL:
        return lhs->value.s_symbol.len == rhs->value.s_symbol.len && memcmp(lhs->value.s_symbol.ptr, rhs->value.s_symbol.ptr, lhs->value.s_symbol.len) == 0;
    case GTKML_S_LIGHTDATA:
        return lhs->value.s_lightdata.userdata == rhs->value.s_lightdata.userdata;
    case GTKML_S_USERDATA:
        return lhs->value.s_userdata.userdata == rhs->value.s_userdata.userdata;
    case GTKML_S_LAMBDA:
        if (gtk_ml_equal(lhs->value.s_lambda.args, rhs->value.s_lambda.args)) {
            return gtk_ml_equal(lhs->value.s_lambda.body, rhs->value.s_lambda.body);
        }
        break;
    case GTKML_S_PROGRAM:
        if (gtk_ml_equal(lhs->value.s_program.args, rhs->value.s_program.args)) {
            return lhs->value.s_program.addr == rhs->value.s_program.addr;
        }
        break;
    case GTKML_S_ADDRESS:
        return lhs->value.s_address.addr == rhs->value.s_address.addr;
    case GTKML_S_MACRO:
        if (gtk_ml_equal(lhs->value.s_macro.args, rhs->value.s_macro.args)) {
            return gtk_ml_equal(lhs->value.s_macro.body, rhs->value.s_macro.body);
        }
        break;
    case GTKML_S_FFI:
        return lhs->value.s_ffi.function == rhs->value.s_ffi.function;
    case GTKML_S_LIST:
    case GTKML_S_MAP:
        if (gtk_ml_equal(gtk_ml_car(lhs), gtk_ml_car(rhs))) {
            return gtk_ml_equal(gtk_ml_cdr(lhs), gtk_ml_cdr(rhs));
        }
        break;
    case GTKML_S_VARIADIC:
        return gtk_ml_equal(lhs->value.s_var.expr, rhs->value.s_var.expr);
    }

    return 0;
}

GtkMl_S *gtk_ml_nil(GtkMl_Context *ctx) {
    return new_nil(ctx, NULL);
}

void gtk_ml_delete_reference(GtkMl_Context *ctx, void *reference) {
    (void) ctx;
    (void) reference;
}

void gtk_ml_delete_value(GtkMl_Context *ctx, void *s) {
    delete(ctx, s);
}

void gtk_ml_object_unref(GtkMl_Context *ctx, void *obj) {
    (void) ctx;
    g_object_unref(obj);
}

GtkMl_Vm *gtk_ml_new_vm(GtkMl_Context *ctx) {
    GtkMl_Vm *vm = malloc(sizeof(GtkMl_Vm));

    vm->stack = malloc(sizeof(GtkMl_Register) * GTKML_VM_STACK);
    vm->stack_len = GTKML_VM_STACK;

    vm->exec = NULL;

    vm->reg[GTKML_R_ZERO].value = 0;
    vm->reg[GTKML_R_FLAGS].flags = GTKML_F_NONE;
    vm->reg[GTKML_R_SP].sp = 0;
    vm->reg[GTKML_R_BP].sp = 0;

    vm->ctx = ctx;

    return vm;
}

void gtk_ml_del_vm(GtkMl_Vm *vm) {
    free(vm->stack);
    if (vm->exec) {
        free(vm->exec);
    }
    free(vm);
}

static gboolean (*CATEGORY[])(GtkMl_Vm *, const char **, GtkMl_Instruction *) = {
    [0] = (gboolean (*)(GtkMl_Vm *, const char **, GtkMl_Instruction *)) NULL,
    [GTKML_I_ARITH] = gtk_ml_ia,
    [GTKML_I_IMM] = gtk_ml_ii,
    [GTKML_I_BR] = gtk_ml_ibr,
    [4] = (gboolean (*)(GtkMl_Vm *, const char **, GtkMl_Instruction *)) NULL,
    [5] = (gboolean (*)(GtkMl_Vm *, const char **, GtkMl_Instruction *)) NULL,
    [6] = (gboolean (*)(GtkMl_Vm *, const char **, GtkMl_Instruction *)) NULL,
    [7] = (gboolean (*)(GtkMl_Vm *, const char **, GtkMl_Instruction *)) NULL,
    [8] = (gboolean (*)(GtkMl_Vm *, const char **, GtkMl_Instruction *)) NULL,
    [9] = (gboolean (*)(GtkMl_Vm *, const char **, GtkMl_Instruction *)) NULL,
    [GTKML_EI_IMM] = gtk_ml_eii,
    [GTKML_EI_BR] = gtk_ml_eibr,
    [GTKML_EI_EXPORT] = gtk_ml_enop,
    [13] = (gboolean (*)(GtkMl_Vm *, const char **, GtkMl_Instruction *)) NULL,
    [14] = (gboolean (*)(GtkMl_Vm *, const char **, GtkMl_Instruction *)) NULL,
    [15] = (gboolean (*)(GtkMl_Vm *, const char **, GtkMl_Instruction *)) NULL,
};

static gboolean (*I_ARITH[])(GtkMl_Vm *, const char **, GtkMl_Instruction) = {
    [GTKML_IA_NOP] = gtk_ml_ia_nop,
    [GTKML_IA_HALT] = gtk_ml_ia_halt,
    [GTKML_IA_INTEGER_ADD] = gtk_ml_ia_integer_add,
    [GTKML_IA_INTEGER_SUBTRACT] = gtk_ml_ia_integer_subtract,
    [GTKML_IA_INTEGER_SIGNED_MULTIPLY] = gtk_ml_ia_integer_signed_multiply,
    [GTKML_IA_INTEGER_UNSIGNED_MULTIPLY] = gtk_ml_ia_integer_unsigned_multiply,
    [GTKML_IA_INTEGER_SIGNED_DIVIDE] = gtk_ml_ia_integer_signed_divide,
    [GTKML_IA_INTEGER_UNSIGNED_DIVIDE] = gtk_ml_ia_integer_unsigned_divide,
    [GTKML_IA_INTEGER_SIGNED_MODULO] = gtk_ml_ia_integer_signed_modulo,
    [GTKML_IA_INTEGER_UNSIGNED_MODULO] = gtk_ml_ia_integer_unsigned_modulo,
    [GTKML_IA_FLOAT_ADD] = gtk_ml_ia_float_add,
    [GTKML_IA_FLOAT_SUBTRACT] = gtk_ml_ia_float_subtract,
    [GTKML_IA_FLOAT_MULTIPLY] = gtk_ml_ia_float_multiply,
    [GTKML_IA_FLOAT_DIVIDE] = gtk_ml_ia_float_divide,
    [GTKML_IA_FLOAT_MODULO] = gtk_ml_ia_float_modulo,
    [GTKML_IA_BIT_AND] = gtk_ml_ia_bit_and,
    [GTKML_IA_BIT_OR] = gtk_ml_ia_bit_or,
    [GTKML_IA_BIT_XOR] = gtk_ml_ia_bit_xor,
    [GTKML_IA_BIT_NAND] = gtk_ml_ia_bit_nand,
    [GTKML_IA_BIT_NOR] = gtk_ml_ia_bit_nor,
    [GTKML_IA_BIT_XNOR] = gtk_ml_ia_bit_xnor,
    [GTKML_IA_BRANCH_ABSOLUTE] = gtk_ml_ia_branch_absolute,
    [GTKML_IA_BRANCH_RELATIVE] = gtk_ml_ia_branch_relative,
    [GTKML_IA_BIND] = gtk_ml_ia_bind,
    [GTKML_IA_DEFINE] = gtk_ml_ia_define,
    [255] = (gboolean (*)(GtkMl_Vm *, const char **, GtkMl_Instruction)) NULL,
};

static gboolean (*I_IMM[])(GtkMl_Vm *, const char **, GtkMl_Instruction) = {
    [GTKML_II_PUSH_IMM] = gtk_ml_ii_push_imm,
    [GTKML_II_POP] = gtk_ml_ii_pop,
    [GTKML_II_GET_IMM] = gtk_ml_ii_get_imm,
    [255] = (gboolean (*)(GtkMl_Vm *, const char **, GtkMl_Instruction)) NULL,
};

static gboolean (*I_BR[])(GtkMl_Vm *, const char **, GtkMl_Instruction) = {
    [GTKML_IBR_CALL_FFI] = gtk_ml_ibr_call_ffi,
    [GTKML_IBR_CALL] = gtk_ml_ibr_call,
    [GTKML_IBR_RET] = gtk_ml_ibr_ret,
    [255] = (gboolean (*)(GtkMl_Vm *, const char **, GtkMl_Instruction)) NULL,
};

static gboolean (*EI_IMM[])(GtkMl_Vm *, const char **, GtkMl_Instruction, GtkMl_S *) = {
    [GTKML_EII_PUSH_EXT_IMM] = gtk_ml_eii_push_ext_imm,
    [GTKML_EII_POP_EXT] = gtk_ml_eii_pop_ext,
    [GTKML_EII_GET_EXT_IMM] = gtk_ml_eii_get_ext_imm,
    [255] = (gboolean (*)(GtkMl_Vm *, const char **, GtkMl_Instruction, GtkMl_S *)) NULL,
};

static gboolean (*EI_BR[])(GtkMl_Vm *, const char **, GtkMl_Instruction, GtkMl_S *) = {
    [GTKML_EIBR_CALL_EXT_FFI] = gtk_ml_eibr_call_ext_ffi,
    [GTKML_EIBR_CALL_EXT] = gtk_ml_eibr_call_ext,
    [GTKML_EIBR_RET_EXT] = gtk_ml_eibr_ret_ext,
    [255] = (gboolean (*)(GtkMl_Vm *, const char **, GtkMl_Instruction, GtkMl_S *)) NULL,
};

gboolean gtk_ml_vm_step(GtkMl_Vm *vm, const char **err, GtkMl_Instruction *instr) {
    if (!instr->gen.cond || (instr->gen.cond && (vm->reg[GTKML_R_FLAGS].flags & instr->gen.cond))) {
        gboolean (*category)(GtkMl_Vm *, const char **, GtkMl_Instruction *) = CATEGORY[instr->gen.category];
        if (category) {
            return category(vm, err, instr);
        } else {
            *err = GTKML_ERR_CATEGORY_ERROR;
            return 0;
        }
    } else {
        if (instr->gen.category & GTKML_I_EXTENDED) {
            vm->reg[GTKML_R_PC].pc += 16;
        } else {
            vm->reg[GTKML_R_PC].pc += 8;
        }
        return 1;
    }
}

gboolean gtk_ml_vm_run(GtkMl_Vm *vm, const char **err) {
    vm->reg[GTKML_R_FLAGS].flags |= GTKML_F_TOPCALL;
    vm->reg[GTKML_R_FLAGS].flags &= ~GTKML_F_HALT;
    size_t gc_counter = 0;
    while (!(vm->reg[GTKML_R_FLAGS].flags & GTKML_F_HALT)) {
        if (!gtk_ml_vm_step(vm, err, vm->exec + (vm->reg[GTKML_R_PC].pc >> 3))) {
            return 0;
        }
        if (gc_counter++ == GTKML_GC_THRESHOLD) {
            gtk_ml_collect(vm->ctx);
            gc_counter = 0;
        }
        vm->reg[GTKML_R_ZERO].flags = 0;
    }
    return 1;
}

GTKML_PRIVATE gboolean gtk_ml_ia(GtkMl_Vm *vm, const char **err, GtkMl_Instruction *instr) {
    gboolean (*opcode)(GtkMl_Vm *, const char **, GtkMl_Instruction) = I_ARITH[instr->arith.opcode];
    if (opcode) {
        return opcode(vm, err, *instr);
    } else {
        *err = GTKML_ERR_OPCODE_ERROR;
        return 0;
    }
}

GTKML_PRIVATE gboolean gtk_ml_ii(GtkMl_Vm *vm, const char **err, GtkMl_Instruction *instr) {
    gboolean (*opcode)(GtkMl_Vm *, const char **, GtkMl_Instruction) = I_IMM[instr->imm.opcode];
    if (opcode) {
        return opcode(vm, err, *instr);
    } else {
        *err = GTKML_ERR_OPCODE_ERROR;
        return 0;
    }
}

GTKML_PRIVATE gboolean gtk_ml_ibr(GtkMl_Vm *vm, const char **err, GtkMl_Instruction *instr) {
    gboolean (*opcode)(GtkMl_Vm *, const char **, GtkMl_Instruction) = I_BR[instr->br.opcode];
    if (opcode) {
        return opcode(vm, err, *instr);
    } else {
        *err = GTKML_ERR_OPCODE_ERROR;
        return 0;
    }
}

GTKML_PRIVATE gboolean gtk_ml_eii(GtkMl_Vm *vm, const char **err, GtkMl_Instruction *instr) {
    gboolean (*opcode)(GtkMl_Vm *, const char **, GtkMl_Instruction, GtkMl_S *) = EI_IMM[instr->imm.opcode];
    if (opcode) {
        return opcode(vm, err, instr[0], instr[1].imm64);
    } else {
        *err = GTKML_ERR_OPCODE_ERROR;
        return 0;
    }
}

GTKML_PRIVATE gboolean gtk_ml_eibr(GtkMl_Vm *vm, const char **err, GtkMl_Instruction *instr) {
    gboolean (*opcode)(GtkMl_Vm *, const char **, GtkMl_Instruction, GtkMl_S *) = EI_BR[instr->br.opcode];
    if (opcode) {
        return opcode(vm, err, instr[0], instr[1].imm64);
    } else {
        *err = GTKML_ERR_OPCODE_ERROR;
        return 0;
    }
}

GTKML_PRIVATE gboolean gtk_ml_enop(GtkMl_Vm *vm, const char **err, GtkMl_Instruction *instr) {
    (void) err;
    (void) instr;

    vm->reg[GTKML_R_PC].pc += 16;

    return 1;
}

#define PC_INCREMENT vm->reg[GTKML_R_PC].pc += 8
#define PC_INCREMENT_EXTENDED vm->reg[GTKML_R_PC].pc += 16

gboolean gtk_ml_ia_nop(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_halt(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    vm->reg[GTKML_R_FLAGS].flags |= GTKML_F_HALT;
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_integer_add(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    // TODO(walterpi): flags
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    gtk_ml_push(vm->ctx, new_int(vm->ctx, NULL, lhs->value.s_int.value + rhs->value.s_int.value));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_integer_subtract(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    // TODO(walterpi): flags
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    gtk_ml_push(vm->ctx, new_int(vm->ctx, NULL, lhs->value.s_int.value - rhs->value.s_int.value));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_integer_signed_multiply(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    // TODO(walterpi): flags
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    gtk_ml_push(vm->ctx, new_int(vm->ctx, NULL, lhs->value.s_int.value * rhs->value.s_int.value));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_integer_unsigned_multiply(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    // TODO(walterpi): flags
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    gtk_ml_push(vm->ctx, new_int(vm->ctx, NULL, (uint64_t) lhs->value.s_int.value * (uint64_t) rhs->value.s_int.value));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_integer_signed_divide(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    // TODO(walterpi): flags
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    gtk_ml_push(vm->ctx, new_int(vm->ctx, NULL, lhs->value.s_int.value / rhs->value.s_int.value));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_integer_unsigned_divide(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    // TODO(walterpi): flags
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    gtk_ml_push(vm->ctx, new_int(vm->ctx, NULL, (uint64_t) lhs->value.s_int.value / (uint64_t) rhs->value.s_int.value));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_integer_signed_modulo(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    // TODO(walterpi): flags
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    gtk_ml_push(vm->ctx, new_int(vm->ctx, NULL, lhs->value.s_int.value % rhs->value.s_int.value));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_integer_unsigned_modulo(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    // TODO(walterpi): flags
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    gtk_ml_push(vm->ctx, new_int(vm->ctx, NULL, (uint64_t) lhs->value.s_int.value % (uint64_t) rhs->value.s_int.value));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_float_add(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    // TODO(walterpi): flags
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    gtk_ml_push(vm->ctx, new_float(vm->ctx, NULL, lhs->value.s_float.value + rhs->value.s_float.value));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_float_subtract(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    // TODO(walterpi): flags
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    gtk_ml_push(vm->ctx, new_float(vm->ctx, NULL, lhs->value.s_float.value - rhs->value.s_float.value));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_float_multiply(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    // TODO(walterpi): flags
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    gtk_ml_push(vm->ctx, new_float(vm->ctx, NULL, lhs->value.s_float.value * rhs->value.s_float.value));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_float_divide(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    // TODO(walterpi): flags
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    gtk_ml_push(vm->ctx, new_float(vm->ctx, NULL, lhs->value.s_float.value / rhs->value.s_float.value));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_float_modulo(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    // TODO(walterpi): flags
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    gtk_ml_push(vm->ctx, new_float(vm->ctx, NULL, fmod(lhs->value.s_float.value, rhs->value.s_float.value)));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_bit_and(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    // TODO(walterpi): flags
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    gtk_ml_push(vm->ctx, new_int(vm->ctx, NULL, lhs->value.s_int.value & rhs->value.s_int.value));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_bit_or(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    // TODO(walterpi): flags
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    gtk_ml_push(vm->ctx, new_int(vm->ctx, NULL, lhs->value.s_int.value | rhs->value.s_int.value));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_bit_xor(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    // TODO(walterpi): flags
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    gtk_ml_push(vm->ctx, new_int(vm->ctx, NULL, lhs->value.s_int.value ^ rhs->value.s_int.value));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_bit_nand(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    // TODO(walterpi): flags
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    gtk_ml_push(vm->ctx, new_int(vm->ctx, NULL, ~(lhs->value.s_int.value & rhs->value.s_int.value)));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_bit_nor(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    // TODO(walterpi): flags
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    gtk_ml_push(vm->ctx, new_int(vm->ctx, NULL, ~(lhs->value.s_int.value | rhs->value.s_int.value)));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_bit_xnor(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    // TODO(walterpi): flags
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    gtk_ml_push(vm->ctx, new_int(vm->ctx, NULL, ~(lhs->value.s_int.value ^ rhs->value.s_int.value)));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_branch_absolute(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    // TODO(walterpi): flags
    GtkMl_S *addr = gtk_ml_pop(vm->ctx);
    vm->reg[GTKML_R_PC].pc = addr->value.s_address.addr;
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_branch_relative(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    // TODO(walterpi): flags
    GtkMl_S *addr = gtk_ml_pop(vm->ctx);
    vm->reg[GTKML_R_PC].pc += 8 + addr->value.s_address.addr;
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_bind(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *key = gtk_ml_pop(vm->ctx);
    GtkMl_S *value = gtk_ml_pop(vm->ctx);
    gtk_ml_bind(vm->ctx, key, value);
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_define(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *key = gtk_ml_pop(vm->ctx);
    GtkMl_S *value = gtk_ml_pop(vm->ctx);
    gtk_ml_define(vm->ctx, key, value);
    return 1;
}

gboolean gtk_ml_ii_push_imm(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    gtk_ml_push(vm->ctx, (GtkMl_S *) instr.imm.imm);
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_pop(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    gtk_ml_pop(vm->ctx);
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_get_imm(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *value = gtk_ml_get(vm->ctx, (GtkMl_S *) instr.imm.imm);
    if (value) {
        gtk_ml_push(vm->ctx, value);
    } else {
        *err = GTKML_ERR_BINDING_ERROR;
        return 0;
    }
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ibr_call_ffi(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr) {
    (void) instr;
    GtkMl_S *(*function)(GtkMl_Context *, const char **, GtkMl_S *) = ((GtkMl_S *) instr.br.imm)->value.s_ffi.function;
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

gboolean gtk_ml_ibr_call(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    uint64_t flags = vm->reg[GTKML_R_FLAGS].flags & GTKML_F_TOPCALL;
    gtk_ml_push(vm->ctx, new_int(vm->ctx, NULL, flags));
    vm->reg[GTKML_R_FLAGS].flags &= ~GTKML_F_TOPCALL;

    uint64_t pc = vm->reg[GTKML_R_PC].pc;
    gtk_ml_push(vm->ctx, new_int(vm->ctx, NULL, pc));
    vm->reg[GTKML_R_PC].pc = ((GtkMl_S *) instr.br.imm)->value.s_program.addr;

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ibr_ret(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    if (vm->reg[GTKML_R_FLAGS].flags & GTKML_F_TOPCALL) {
        vm->reg[GTKML_R_FLAGS].flags |= GTKML_F_HALT;
    } else {
        GtkMl_S *pc = gtk_ml_pop(vm->ctx);
        GtkMl_S *flags = gtk_ml_pop(vm->ctx);
        vm->reg[GTKML_R_PC].pc = pc->value.s_int.value;
        vm->reg[GTKML_R_FLAGS].flags &= ~GTKML_F_TOPCALL;
        vm->reg[GTKML_R_FLAGS].flags |= flags->value.s_int.value;
    }
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_eii_push_ext_imm(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr, GtkMl_S *imm64) {
    (void) err;
    (void) instr;
    printf("push ext imm %p\n", (void *) imm64);
    gtk_ml_push(vm->ctx, imm64);
    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eii_pop_ext(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr, GtkMl_S *imm64) {
    (void) err;
    (void) instr;
    (void) imm64;
    gtk_ml_pop(vm->ctx);
    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eii_get_ext_imm(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr, GtkMl_S *imm64) {
    (void) err;
    (void) instr;
    GtkMl_S *value = gtk_ml_get(vm->ctx, imm64);
    if (value) {
        gtk_ml_push(vm->ctx, value);
    } else {
        *err = GTKML_ERR_BINDING_ERROR;
        return 0;
    }
    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eibr_call_ext_ffi(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr, GtkMl_S *imm64) {
    (void) instr;
    GtkMl_S *(*function)(GtkMl_Context *, const char **, GtkMl_S *) = imm64->value.s_ffi.function;
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

gboolean gtk_ml_eibr_call_ext(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr, GtkMl_S *imm64) {
    (void) err;
    (void) instr;

    uint64_t flags = vm->reg[GTKML_R_FLAGS].flags & GTKML_F_TOPCALL;
    gtk_ml_push(vm->ctx, new_int(vm->ctx, NULL, flags));
    vm->reg[GTKML_R_FLAGS].flags &= ~GTKML_F_TOPCALL;

    uint64_t pc = vm->reg[GTKML_R_PC].pc;
    gtk_ml_push(vm->ctx, new_int(vm->ctx, NULL, pc));
    vm->reg[GTKML_R_PC].pc = (uint64_t) vm->exec + imm64->value.s_program.addr;

    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eibr_ret_ext(GtkMl_Vm *vm, const char **err, GtkMl_Instruction instr, GtkMl_S *imm64) {
    (void) err;
    (void) instr;
    (void) imm64;
    if (vm->reg[GTKML_R_FLAGS].flags & GTKML_F_TOPCALL) {
        vm->reg[GTKML_R_FLAGS].flags |= GTKML_F_HALT;
    } else {
        GtkMl_S *pc = gtk_ml_pop(vm->ctx);
        GtkMl_S *flags = gtk_ml_pop(vm->ctx);
        vm->reg[GTKML_R_PC].pc = pc->value.s_int.value;
        vm->reg[GTKML_R_FLAGS].flags &= ~GTKML_F_TOPCALL;
        vm->reg[GTKML_R_FLAGS].flags |= flags->value.s_int.value;
    }
    PC_INCREMENT_EXTENDED;
    return 1;
}
