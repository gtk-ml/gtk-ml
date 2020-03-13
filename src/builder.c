#include <stdlib.h>
#define GTKML_INCLUDE_INTERNAL
#include "gtk-ml.h"
#include "gtk-ml-internal.h"

#define gtk_ml_add_builder(b, _builder_name, builder, _require_intrinsic, _require_macro, _require_runtime) \
    do { \
        char *_name = _builder_name; \
        char *name = malloc(strlen(_name) + 1); \
        strcpy(name, _name); \
        b->builders[b->len_builder].name = name; \
        b->builders[b->len_builder].fn = builder; \
        b->builders[b->len_builder].require_intrinsic = _require_intrinsic; \
        b->builders[b->len_builder].require_macro = _require_macro; \
        b->builders[b->len_builder].require_runtime = _require_runtime; \
        ++b->len_builder; \
    } while (0);

GtkMl_Builder *gtk_ml_new_builder(GtkMl_Context *ctx) {
    GtkMl_Builder *b = malloc(sizeof(GtkMl_Builder));

    b->basic_blocks = malloc(sizeof(GtkMl_BasicBlock *) * 64);
    b->len_bb = 0;
    b->cap_bb = 64;

    b->statics = malloc(sizeof(GtkMl_S *) * 64);
    b->statics[0] = NULL;
    b->len_static = 1;
    b->cap_static = 64;

    b->counter = gtk_ml_new_var(ctx, NULL, gtk_ml_new_int(ctx, NULL, 0));
    b->flags = 0;

    b->intr_ctx = gtk_ml_new_context();
    b->intr_vm = b->intr_ctx->vm;

    b->macro_ctx = gtk_ml_new_context();
    b->macro_vm = b->macro_ctx->vm;

    b->builders = malloc(sizeof(GtkMl_BuilderMacro) * 64);
    b->len_builder = 0;
    b->cap_builder = 64;

    gtk_ml_add_builder(b, "compile-expr", gtk_ml_builder_compile_expr, 1, 0, 0);
    gtk_ml_add_builder(b, "emit-bytecode", gtk_ml_builder_emit_bytecode, 1, 0, 0);
    gtk_ml_add_builder(b, "append-basic-block", gtk_ml_builder_append_basic_block, 1, 0, 0);
    gtk_ml_add_builder(b, "global-counter", gtk_ml_builder_global_counter, 1, 0, 0);
    gtk_ml_add_builder(b, "string->symbol", gtk_ml_builder_string_to_symbol, 0, 0, 0);
    gtk_ml_add_builder(b, "do", gtk_ml_builder_do, 0, 0, 0);
    gtk_ml_add_builder(b, "let", gtk_ml_builder_let, 0, 0, 0);
    gtk_ml_add_builder(b, "let*", gtk_ml_builder_let_star, 0, 0, 0);
    gtk_ml_add_builder(b, "lambda", gtk_ml_builder_lambda, 0, 0, 0);
    gtk_ml_add_builder(b, "macro", gtk_ml_builder_macro, 0, 1, 0);
    gtk_ml_add_builder(b, "cond", gtk_ml_builder_cond, 0, 0, 0);
    gtk_ml_add_builder(b, "while", gtk_ml_builder_while, 0, 0, 0);
    gtk_ml_add_builder(b, "len", gtk_ml_builder_len, 0, 0, 0);
    gtk_ml_add_builder(b, "car", gtk_ml_builder_car, 0, 0, 0);
    gtk_ml_add_builder(b, "cdr", gtk_ml_builder_cdr, 0, 0, 0);
    gtk_ml_add_builder(b, "typeof", gtk_ml_builder_typeof, 0, 0, 0);
    gtk_ml_add_builder(b, "index", gtk_ml_builder_index, 0, 0, 0);
    gtk_ml_add_builder(b, "push", gtk_ml_builder_push, 0, 0, 0);
    gtk_ml_add_builder(b, "pop", gtk_ml_builder_pop, 0, 0, 0);
    gtk_ml_add_builder(b, "concat", gtk_ml_builder_concat, 0, 0, 0);
    gtk_ml_add_builder(b, "+", gtk_ml_builder_add, 0, 0, 0);
    gtk_ml_add_builder(b, "-", gtk_ml_builder_sub, 0, 0, 0);
    gtk_ml_add_builder(b, "*", gtk_ml_builder_mul, 0, 0, 0);
    gtk_ml_add_builder(b, "/", gtk_ml_builder_div, 0, 0, 0);
    gtk_ml_add_builder(b, "%", gtk_ml_builder_mod, 0, 0, 0);
    gtk_ml_add_builder(b, "bit-not", gtk_ml_builder_bitnot, 0, 0, 0);
    gtk_ml_add_builder(b, "bit-and", gtk_ml_builder_bitand, 0, 0, 0);
    gtk_ml_add_builder(b, "bit-or", gtk_ml_builder_bitor, 0, 0, 0);
    gtk_ml_add_builder(b, "bit-xor", gtk_ml_builder_bitxor, 0, 0, 0);
    gtk_ml_add_builder(b, "cmp", gtk_ml_builder_cmp, 0, 0, 0);
    gtk_ml_add_builder(b, "var", gtk_ml_builder_var, 0, 0, 0);
    gtk_ml_add_builder(b, "vararg", gtk_ml_builder_vararg, 0, 0, 0);
    gtk_ml_add_builder(b, "quote", gtk_ml_builder_quote, 0, 0, 0);
    gtk_ml_add_builder(b, "quasiquote", gtk_ml_builder_quasiquote, 0, 0, 0);
    gtk_ml_add_builder(b, "unquote", gtk_ml_builder_unquote, 0, 0, 0);
    gtk_ml_add_builder(b, "define", gtk_ml_builder_define, 0, 0, 0);
    gtk_ml_add_builder(b, "define-macro", gtk_ml_builder_define_macro, 0, 0, 0);
    gtk_ml_add_builder(b, "define-intrinsic", gtk_ml_builder_define_intrinsic, 0, 0, 0);
    gtk_ml_add_builder(b, "intr-apply", gtk_ml_builder_intr_apply, 1, 0, 0);
    gtk_ml_add_builder(b, "Application", gtk_ml_builder_application, 0, 0, 1);
    gtk_ml_add_builder(b, "new-window", gtk_ml_builder_new_window, 0, 0, 1);
    gtk_ml_add_builder(b, "setmetamap", gtk_ml_builder_setmetamap, 0, 0, 0);
    gtk_ml_add_builder(b, "getmetamap", gtk_ml_builder_getmetamap, 0, 0, 0);
    gtk_ml_add_builder(b, "get", gtk_ml_builder_getvar, 0, 0, 0);
    gtk_ml_add_builder(b, "assign", gtk_ml_builder_assignvar, 0, 0, 0);
    gtk_ml_add_builder(b, "error", gtk_ml_builder_error, 0, 0, 0);
    gtk_ml_add_builder(b, "dbg", gtk_ml_builder_dbg, 0, 0, 0);

    gtk_ml_new_hash_set(&b->intr_fns, &GTKML_DEFAULT_HASHER);
    gtk_ml_new_hash_set(&b->macro_fns, &GTKML_DEFAULT_HASHER);

    return b;
}

void gtk_ml_builder_set_cond(GtkMl_Builder *b, unsigned int flags) {
    b->flags = flags;
}

unsigned int gtk_ml_builder_clear_cond(GtkMl_Builder *b) {
    unsigned int flags = b->flags;
    b->flags = 0;
    return flags;
}

GtkMl_S *gtk_ml_builder_get_and_inc(GtkMl_Context *ctx, GtkMl_Builder *b) {
    GtkMl_S *value = b->counter->value.s_var.expr;
    b->counter->value.s_var.expr = gtk_ml_new_int(ctx, NULL, value->value.s_int.value + 1);
    return value;
}

gboolean gtk_ml_build_halt(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].arith.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].arith.category = GTKML_I_ARITH;
    basic_block->exec[basic_block->len_exec].arith.opcode = GTKML_IA_HALT;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_push_extended_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64) {
    if (imm64 < ((GtkMl_Static) 1 << 32)) {
        return gtk_ml_build_push_imm(ctx, b, basic_block, err, imm64);
    }

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
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

gboolean gtk_ml_build_push_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_PUSH_IMM;
    basic_block->exec[basic_block->len_exec].imm.imm = imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_setf_extended_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64) {
    if (imm64 < ((GtkMl_Static) 1 << 32)) {
        return gtk_ml_build_setf_imm(ctx, b, basic_block, err, imm64);
    }

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_EI_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_EII_SETF_EXT_IMM;
    ++basic_block->len_exec;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].imm64 = imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_setf_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_SETF_IMM;
    basic_block->exec[basic_block->len_exec].imm.imm = imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_popf(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_POPF;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_push_extended_addr(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static linkage_name) {
    if (linkage_name < ((GtkMl_Static) 1 << 32)) {
        return gtk_ml_build_push_addr(ctx, b, basic_block, err, linkage_name);
    }

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
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

gboolean gtk_ml_build_push_addr(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static linkage_name) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM | GTKML_I_IMM_EXTERN;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_PUSH_IMM;

    basic_block->exec[basic_block->len_exec].imm.imm = linkage_name;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_pop(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_POP;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_define(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].arith.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].arith.category = GTKML_I_ARITH;
    basic_block->exec[basic_block->len_exec].arith.opcode = GTKML_IA_DEFINE;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_car(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].arith.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].arith.category = GTKML_I_ARITH;
    basic_block->exec[basic_block->len_exec].arith.opcode = GTKML_IA_CAR;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_cdr(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].arith.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].arith.category = GTKML_I_ARITH;
    basic_block->exec[basic_block->len_exec].arith.opcode = GTKML_IA_CDR;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_bind(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].arith.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].arith.category = GTKML_I_ARITH;
    basic_block->exec[basic_block->len_exec].arith.opcode = GTKML_IA_BIND;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_bind_args(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].arith.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].arith.category = GTKML_I_ARITH;
    basic_block->exec[basic_block->len_exec].arith.opcode = GTKML_IA_BIND_ARGS;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_list(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].arith.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].arith.category = GTKML_I_ARITH;
    basic_block->exec[basic_block->len_exec].arith.opcode = GTKML_IA_LIST;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_enter(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].arith.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].arith.category = GTKML_I_ARITH;
    basic_block->exec[basic_block->len_exec].arith.opcode = GTKML_IA_ENTER;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_leave(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].arith.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].arith.category = GTKML_I_ARITH;
    basic_block->exec[basic_block->len_exec].arith.opcode = GTKML_IA_LEAVE;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_unwrap(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].arith.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].arith.category = GTKML_I_ARITH;
    basic_block->exec[basic_block->len_exec].arith.opcode = GTKML_IA_UNWRAP;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_typeof(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].arith.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].arith.category = GTKML_I_ARITH;
    basic_block->exec[basic_block->len_exec].arith.opcode = GTKML_IA_TYPEOF;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_get_extended_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64) {
    if (imm64 < ((GtkMl_Static) 1 << 32)) {
        return gtk_ml_build_get_imm(ctx, b, basic_block, err, imm64);
    }

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
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

gboolean gtk_ml_build_get_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_GET_IMM;
    basic_block->exec[basic_block->len_exec].imm.imm = imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_list_extended_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64) {
    if (imm64 < ((GtkMl_Static) 1 << 32)) {
        return gtk_ml_build_list_imm(ctx, b, basic_block, err, imm64);
    }

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_EI_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_EII_LIST_EXT_IMM;
    ++basic_block->len_exec;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].imm64 = imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_list_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_LIST_IMM;
    basic_block->exec[basic_block->len_exec].imm.imm = imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_map_extended_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64) {
    if (imm64 < ((GtkMl_Static) 1 << 32)) {
        return gtk_ml_build_map_imm(ctx, b, basic_block, err, imm64);
    }

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_EI_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_EII_MAP_EXT_IMM;
    ++basic_block->len_exec;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].imm64 = imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_map_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_MAP_IMM;
    basic_block->exec[basic_block->len_exec].imm.imm = imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_set_extended_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64) {
    if (imm64 < ((GtkMl_Static) 1 << 32)) {
        return gtk_ml_build_set_imm(ctx, b, basic_block, err, imm64);
    }

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_EI_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_EII_SET_EXT_IMM;
    ++basic_block->len_exec;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].imm64 = imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_set_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_SET_IMM;
    basic_block->exec[basic_block->len_exec].imm.imm = imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_array_extended_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64) {
    if (imm64 < ((GtkMl_Static) 1 << 32)) {
        return gtk_ml_build_array_imm(ctx, b, basic_block, err, imm64);
    }

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_EI_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_EII_ARRAY_EXT_IMM;
    ++basic_block->len_exec;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].imm64 = imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_array_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_ARRAY_IMM;
    basic_block->exec[basic_block->len_exec].imm.imm = imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_setmm_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_SETMM_IMM;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_getmm_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_GETMM_IMM;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_var_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_VAR_IMM;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_getvar_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_GETVAR_IMM;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_assignvar_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_ASSIGNVAR_IMM;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_len(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_LEN;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_array_index(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_ARRAY_INDEX;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_array_push(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_ARRAY_PUSH;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_array_pop(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_ARRAY_POP;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_array_concat(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_ARRAY_CONCAT;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_map_get(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_MAP_GET;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_map_insert(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_MAP_INSERT;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_map_delete(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_MAP_DELETE;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_set_contains(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_SET_CONTAINS;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_set_insert(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_SET_INSERT;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_set_delete(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_SET_DELETE;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_call_extended_std(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64) {
    if (imm64 < ((GtkMl_Static) 1 << 48)) {
        return gtk_ml_build_call_std(ctx, b, basic_block, err, imm64);
    }

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].br.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].br.category = GTKML_EI_BR;
    basic_block->exec[basic_block->len_exec].br.opcode = GTKML_EIBR_CALL_EXT_STD;
    ++basic_block->len_exec;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].imm64 = imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_call_std(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].br.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].br.category = GTKML_I_BR;
    basic_block->exec[basic_block->len_exec].br.opcode = GTKML_IBR_CALL_STD;
    basic_block->exec[basic_block->len_exec].br.imm = imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_call(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].imm64 = 0;
    basic_block->exec[basic_block->len_exec].br.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].br.category = GTKML_I_BR;
    basic_block->exec[basic_block->len_exec].br.opcode = GTKML_IBR_CALL;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_branch_absolute_extended(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static linkage_name) {
    if (linkage_name < ((GtkMl_Static) 1 << 32)) {
        return gtk_ml_build_branch_absolute(ctx, b, basic_block, err, linkage_name);
    }

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].br.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].br.category = GTKML_EI_BR | GTKML_EI_BR_EXTERN;
    basic_block->exec[basic_block->len_exec].br.opcode = GTKML_EIBR_BRANCH_ABSOLUTE_EXT;
    ++basic_block->len_exec;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].imm64 = linkage_name;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_branch_absolute(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static linkage_name) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].br.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].br.category = GTKML_I_BR | GTKML_I_BR_EXTERN;
    basic_block->exec[basic_block->len_exec].br.opcode = GTKML_IBR_BRANCH_ABSOLUTE;
    basic_block->exec[basic_block->len_exec].br.imm = linkage_name;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_ret(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].br.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].br.category = GTKML_I_BR;
    basic_block->exec[basic_block->len_exec].br.opcode = GTKML_IBR_RET;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_add(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].br.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].br.category = GTKML_I_ARITH;
    basic_block->exec[basic_block->len_exec].br.opcode = GTKML_IA_ADD;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_sub(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].br.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].br.category = GTKML_I_ARITH;
    basic_block->exec[basic_block->len_exec].br.opcode = GTKML_IA_SUBTRACT;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_mul(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].br.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].br.category = GTKML_I_ARITH;
    basic_block->exec[basic_block->len_exec].br.opcode = GTKML_IA_SIGNED_MULTIPLY;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_div(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].br.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].br.category = GTKML_I_ARITH;
    basic_block->exec[basic_block->len_exec].br.opcode = GTKML_IA_SIGNED_DIVIDE;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_mod(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].br.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].br.category = GTKML_I_ARITH;
    basic_block->exec[basic_block->len_exec].br.opcode = GTKML_IA_SIGNED_MODULO;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_cmp(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_EI_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_EII_CMP_EXT_IMM;
    ++basic_block->len_exec;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].imm64 = imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_bitand(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].br.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].br.category = GTKML_I_ARITH;
    basic_block->exec[basic_block->len_exec].br.opcode = GTKML_IA_BIT_AND;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_bitor(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].br.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].br.category = GTKML_I_ARITH;
    basic_block->exec[basic_block->len_exec].br.opcode = GTKML_IA_BIT_AND;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_bitxor(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].br.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].br.category = GTKML_I_ARITH;
    basic_block->exec[basic_block->len_exec].br.opcode = GTKML_IA_BIT_XOR;
    ++basic_block->len_exec;

    return 1;
}

GtkMl_BasicBlock *gtk_ml_append_basic_block(GtkMl_Builder *b, const char *name) {
    if (b->len_bb == b->cap_bb) {
        b->cap_bb *= 2;
        b->basic_blocks = realloc(b->basic_blocks, sizeof(GtkMl_BasicBlock *) * b->cap_bb);
    }

    GtkMl_BasicBlock *basic_block = malloc(sizeof(GtkMl_BasicBlock));
    b->basic_blocks[b->len_bb] = basic_block;
    ++b->len_bb;

    basic_block->name = name;
    basic_block->exec = malloc(sizeof(GtkMl_Instruction) * 256);
    basic_block->len_exec = 0;
    basic_block->cap_exec = 256;

    return basic_block;
}

GtkMl_Static gtk_ml_append_static(GtkMl_Builder *b, GtkMl_S *value) {
    for (GtkMl_Static i = 0; i < b->len_static; i++) {
        if (b->statics[i] == value) {
            return i;
        }
    }

    if (b->len_static == b->cap_static) {
        b->cap_static *= 2;
        b->statics = realloc(b->statics, sizeof(GtkMl_S *) * b->cap_static);
    }

    GtkMl_Static handle = b->len_static;
    b->statics[handle] = value;
    ++b->len_static;

    return handle;
}

GTKML_PRIVATE gboolean build(GtkMl_Context *ctx, GtkMl_Program *out, GtkMl_S **err, GtkMl_Builder *b, GtkMl_Stage stage, gboolean complete) {
    size_t n = 0;
    size_t n_static = b->len_static;
    for (size_t i = 0; i < b->len_bb; i++) {
        n += b->basic_blocks[i]->len_exec;
    }

    if (!complete) {
        n += 1;
    }

    GtkMl_Instruction *result = malloc(sizeof(GtkMl_Instruction) * n);
    GtkMl_S **statics = malloc(sizeof(GtkMl_S *) * n_static);
    memcpy(statics, b->statics, sizeof(GtkMl_S *) * n_static);
    uint64_t pc = 0;

    for (size_t i = 0; i < b->len_bb; i++) {
        size_t n = b->basic_blocks[i]->len_exec;
        memcpy(result + pc, b->basic_blocks[i]->exec, sizeof(GtkMl_Instruction) * n);
        pc += n;

        // add a halt instruction to the end of _start
        if (!complete && i == 0) {
            result[pc].instr = 0;
            result[pc].arith.cond = 0;
            result[pc].arith.category = GTKML_I_ARITH;
            result[pc].arith.opcode = GTKML_IA_HALT;
            ++pc;
        }
    }

    for (size_t i = 0; i < n;) {
        GtkMl_Instruction instr = result[i];
        if (instr.gen.category == GTKML_EI_EXPORT) {
            GtkMl_S *addr = statics[result[i + 1].imm64];
            if (addr->kind == GTKML_S_PROGRAM) {
                addr->value.s_program.addr = 8 * i;
            } else if (addr->kind == GTKML_S_ADDRESS) {
                addr->value.s_address.addr = 8 * i;
            } else {
                *err = gtk_ml_error(ctx, "linkage-error", GTKML_ERR_LINKAGE_ERROR, 0, 0, 0, 0);
                return 0;
            }
        }
        if (instr.gen.category & GTKML_I_EXTENDED) {
            i += 2;
        } else {
            ++i;
        }
    }

    for (size_t i = 0; i < n;) {
        GtkMl_Instruction instr = result[i];
        if (instr.gen.category != GTKML_EI_EXPORT && (instr.gen.category & GTKML_I_IMM_EXTERN)) {
            GtkMl_S *ext;
            if (instr.gen.category & GTKML_I_EXTENDED) {
                ext = statics[result[i + 1].imm64];
            } else if (instr.gen.category == (GTKML_I_IMM | GTKML_I_IMM_EXTERN)) {
                ext = statics[instr.imm.imm];
            } else if (instr.gen.category == (GTKML_I_BR | GTKML_I_BR_EXTERN)) {
                ext = statics[instr.br.imm];
            } else {
                *err = gtk_ml_error(ctx, "category-error", GTKML_ERR_CATEGORY_ERROR, 0, 0, 0, 0);
                return 0;
            }
            if (ext->kind != GTKML_S_ARRAY) {
                *err = gtk_ml_error(ctx, "linkage-error", GTKML_ERR_LINKAGE_ERROR, 0, 0, 0, 0);
                return 0;
            }
            gboolean found = 0;

            for (size_t l = 0; l < n;) {
                GtkMl_Instruction instr = result[l];
                if (instr.gen.category == GTKML_EI_EXPORT) {
                    GtkMl_S *addr = statics[result[l + 1].imm64];
                    GtkMl_S *exp;
                    if (addr->kind == GTKML_S_PROGRAM) {
                        exp = addr->value.s_program.linkage_name;
                    } else if (addr->kind == GTKML_S_ADDRESS) {
                        exp = addr->value.s_address.linkage_name;
                    } else {
                        *err = gtk_ml_error(ctx, "linkage-error", GTKML_ERR_LINKAGE_ERROR, 0, 0, 0, 0);
                        return 0;
                    }
                    if (gtk_ml_equal(ext, exp)) {
                        result[i].gen.category &= ~GTKML_EI_IMM_EXTERN;
                        if (result[i].gen.category & GTKML_I_EXTENDED) {
                            result[i + 1].imm64 = result[l + 1].imm64;
                        } else if (result[i].gen.category == GTKML_I_IMM) {
                            result[i].imm.imm = result[l + 1].imm64;
                        } else if (result[i].gen.category == GTKML_I_BR) {
                            result[i].br.imm = result[l + 1].imm64;
                        } else {
                            *err = gtk_ml_error(ctx, "category-error", GTKML_ERR_CATEGORY_ERROR, 0, 0, 0, 0);
                            return 0;
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
                GtkMl_S *error = gtk_ml_error(ctx, "linkage-error", GTKML_ERR_LINKAGE_ERROR, 0, 0, 0, 0);
                GtkMl_S *new = gtk_ml_new_map(ctx, NULL, NULL);
                gtk_ml_del_hash_trie(ctx, &new->value.s_map.map, gtk_ml_delete_void_reference);
                gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_new_keyword(ctx, NULL, 0, "linkage-name", strlen("linkage-name")), ext);
                error = new;
                *err = error;
                return 0;
            }
        }
        if (instr.gen.category & GTKML_I_EXTENDED) {
            i += 2;
        } else {
            ++i;
        }
    }

    if (!complete) {
        char *start = malloc(strlen("_start") + 1);
        strcpy(start, "_start");
        out->start = start;
        out->exec = result;
        out->n_exec = n;
        out->statics = statics;
        out->n_static = n_static;
    } else {
        switch (stage) {
        case GTKML_STAGE_INTR:
            for (size_t i = 0; i < b->len_bb; i++) {
                free(b->basic_blocks[i]->exec);
                free(b->basic_blocks[i]);
            }
            b->len_bb = 0;
            b->len_static = 1;

            char *start = malloc(strlen("_start") + 1);
            strcpy(start, "_start");
            out->start = start;
            out->exec = result;
            out->n_exec = n;
            out->statics = statics;
            out->n_static = n_static;
            break;
        case GTKML_STAGE_MACRO: {
            // first we transfer the values from the intr context into the macro context
            GtkMl_S *intr_values = b->intr_ctx->first;
            b->intr_ctx->first = NULL;
            GtkMl_S **prev = &b->macro_ctx->first;
            while (*prev) {
                prev = &(*prev)->next;
            }
            *prev = intr_values;
            // then it's safe to delete the intr context and vm

            for (size_t i = 0; i < b->len_bb; i++) {
                free(b->basic_blocks[i]->exec);
                free(b->basic_blocks[i]);
            }
            b->len_bb = 0;
            b->len_static = 1;

            char *start = malloc(strlen("_start") + 1);
            strcpy(start, "_start");
            out->start = start;
            out->exec = result;
            out->n_exec = n;
            out->statics = statics;
            out->n_static = n_static;
        } break;
        case GTKML_STAGE_RUNTIME: {
            // first we transfer the values from the macro context into the real context
            GtkMl_S *macro_values = b->macro_ctx->first;
            b->macro_ctx->first = NULL;
            GtkMl_S **prev = &ctx->first;
            while (*prev) {
                prev = &(*prev)->next;
            }
            *prev = macro_values;
            // then it's safe to delete the macro context and vm
            gtk_ml_del_context(b->macro_ctx);

            // and the values from the intr context into the real context
            GtkMl_S *intr_values = b->intr_ctx->first;
            b->intr_ctx->first = NULL;
            prev = &ctx->first;
            while (*prev) {
                prev = &(*prev)->next;
            }
            *prev = intr_values;
            // and the intr context
            gtk_ml_del_context(b->intr_ctx);

            for (size_t i = 0; i < b->len_bb; i++) {
                free(b->basic_blocks[i]->exec);
                free(b->basic_blocks[i]);
            }
            for (size_t i = 0; i < b->len_builder; i++) {
                free((void *) b->builders[i].name);
            }
            free(b->builders);
            free(b->statics);
            free(b->basic_blocks);
            free(b);

            char *start = malloc(strlen("_start") + 1);
            strcpy(start, "_start");
            out->start = start;
            out->exec = result;
            out->n_exec = n;
            out->statics = statics;
            out->n_static = n_static;
        } break;
        }
    }

    return 1;
}

gboolean gtk_ml_build_intr_apply(GtkMl_Program *out, GtkMl_S **err, GtkMl_Builder *b) {
    return build(NULL, out, err, b, GTKML_STAGE_INTR, 0);
}

gboolean gtk_ml_build_intrinsics(GtkMl_Program *out, GtkMl_S **err, GtkMl_Builder *b) {
    return build(NULL, out, err, b, GTKML_STAGE_INTR, 1);
}

gboolean gtk_ml_build_macros(GtkMl_Program *out, GtkMl_S **err, GtkMl_Builder *b) {
    return build(NULL, out, err, b, GTKML_STAGE_MACRO, 1);
}

gboolean gtk_ml_build(GtkMl_Context *ctx, GtkMl_Program *out, GtkMl_S **err, GtkMl_Builder *b) {
    return build(ctx, out, err, b, GTKML_STAGE_RUNTIME, 1);
}
