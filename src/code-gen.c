#include <stdlib.h>
#include <gtk/gtk.h>
#include <math.h>
#define GTKML_INCLUDE_INTERNAL
#include "gtk-ml.h"
#include "gtk-ml-internal.h"

GTKML_PRIVATE gboolean compile_std_call(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, uint64_t function, GtkMl_S *args, gboolean compile_first, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PRIVATE gboolean compile_runtime_program(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, const char *linkage_name, GtkMl_S *stmt, gboolean ret);

gboolean gtk_ml_builder_application(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL
            || gtk_ml_cddr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_std_call(ctx, b, basic_block, err, GTKML_STD_APPLICATION, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_new_window(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    (void) allow_intr;
    (void) allow_macro;
    (void) allow_runtime;
    (void) allow_macro_expansion;

    GtkMl_S *args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL
            || gtk_ml_cddr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_std_call(ctx, b, basic_block, err, GTKML_STD_NEW_WINDOW, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_setmetamap(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL || gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S *map = gtk_ml_car(args);
    GtkMl_S *metamap = gtk_ml_cdar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, &map, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, &metamap, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_build_setmm_imm(ctx, b, *basic_block, err)) {
        return 0;
    }
    return gtk_ml_build_push_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, gtk_ml_new_nil(ctx, NULL)));
}

gboolean gtk_ml_builder_getmetamap(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S *map = gtk_ml_car(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, &map, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_getmm_imm(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_getvar(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S *map = gtk_ml_car(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, &map, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_getvar_imm(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_assignvar(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL || gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S *var = gtk_ml_car(args);
    GtkMl_S *newvalue = gtk_ml_cdar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, &var, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, &newvalue, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_build_assignvar_imm(ctx, b, *basic_block, err)) {
        return 0;
    }
    return gtk_ml_build_push_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, gtk_ml_new_nil(ctx, NULL)));
}

gboolean gtk_ml_builder_error(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    (void) allow_intr;
    (void) allow_macro;
    (void) allow_runtime;
    (void) allow_macro_expansion;

    GtkMl_S *args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_std_call(ctx, b, basic_block, err, GTKML_STD_ERROR, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_dbg(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    (void) allow_intr;
    (void) allow_macro;
    (void) allow_runtime;
    (void) allow_macro_expansion;

    GtkMl_S *args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_std_call(ctx, b, basic_block, err, GTKML_STD_DBG, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_define(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S *definition = gtk_ml_car(args);
    if (definition->kind == GTKML_S_SYMBOL) {
        GtkMl_S *name = definition;
        GtkMl_S **value = &gtk_ml_cdar(args);
        if (!gtk_ml_compile_expression(ctx, b, basic_block, err, value, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
            return 0;
        }
        if (!gtk_ml_build_push_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, name))) {
            return 0;
        }
        return gtk_ml_build_define(ctx, b, *basic_block, err);
    } else if (definition->kind == GTKML_S_LIST) {
        GtkMl_S *name = gtk_ml_car(definition);

        if (name->kind != GTKML_S_SYMBOL) {
            *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, definition->span.ptr != NULL, definition->span.line, definition->span.col, 0);
            return 0;
        }

        GtkMl_S *params = gtk_ml_cdr(definition);
        GtkMl_S *body = gtk_ml_cdr(args);

        GtkMl_S *lambda = gtk_ml_new_lambda(ctx, &(*stmt)->span, params, body, gtk_ml_new_map(ctx, NULL, NULL));

        size_t len = name->value.s_symbol.len;
        char *linkage_name = malloc(len + 1);
        memcpy(linkage_name, name->value.s_symbol.ptr, len);
        linkage_name[len] = 0;
        GtkMl_BasicBlock *bb = gtk_ml_append_basic_block(b, linkage_name);
        if (!compile_runtime_program(ctx, b, &bb, err, linkage_name, lambda, 1)) {
            return 0;
        }
        if (!gtk_ml_build_push_extended_addr(ctx, b, *basic_block, err, gtk_ml_append_static(b, gtk_ml_new_string(ctx, NULL, linkage_name, strlen(linkage_name))))) {
            return 0;
        }
        if (!gtk_ml_build_push_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, name))) {
            return 0;
        }
        return gtk_ml_build_define(ctx, b, *basic_block, err);
    } else {
        *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, definition->span.ptr != NULL, definition->span.line, definition->span.col, 0);
        return 0;
    }
}


gboolean gtk_ml_builder_define_macro(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    (void) ctx;
    (void) b;
    (void) basic_block;
    (void) err;
    (void) stmt;
    (void) allow_intr;
    (void) allow_macro;
    (void) allow_runtime;
    (void) allow_macro_expansion;
    return gtk_ml_build_push_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, gtk_ml_new_nil(ctx, NULL)));
}

gboolean gtk_ml_builder_define_intrinsic(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    (void) ctx;
    (void) b;
    (void) basic_block;
    (void) err;
    (void) stmt;
    (void) allow_intr;
    (void) allow_macro;
    (void) allow_runtime;
    (void) allow_macro_expansion;
    return gtk_ml_build_push_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, gtk_ml_new_nil(ctx, NULL)));
}

gboolean gtk_ml_builder_intr_apply(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    (void) allow_intr;
    (void) allow_macro;
    (void) allow_runtime;
    (void) allow_macro_expansion;

    GtkMl_Program program;
    if (!gtk_ml_build_intr_apply(&program, err, b)) {
        return 0;
    }

    GtkMl_Context *tmp_ctx = gtk_ml_new_context();
    gtk_ml_load_program(tmp_ctx, &program);
    gtk_ml_del_program(&program);
    GtkMl_S *intr_program = gtk_ml_get_export(tmp_ctx, err, "_start");
    if (!intr_program) {
        return 0;
    }
    if (!gtk_ml_run_program(tmp_ctx, err, intr_program, gtk_ml_new_nil(tmp_ctx, NULL))) {
        return 0;
    }

    GtkMl_S **function = &gtk_ml_cdar(*stmt);

    GtkMl_S *args = gtk_ml_cddr(*stmt);

    if ((*function)->kind == GTKML_S_SYMBOL) {
        size_t len = (*function)->value.s_symbol.len;
        char *linkage_name = malloc(len + 1);
        memcpy(linkage_name, (*function)->value.s_symbol.ptr, len);
        linkage_name[len] = 0;

        GtkMl_S *_err = NULL;
        GtkMl_S *program = gtk_ml_get_export(tmp_ctx, &_err, linkage_name);
        free(linkage_name);
        if (program && program->value.s_program.kind == GTKML_PROG_INTRINSIC) {
            GtkMl_BasicBlock *bb = *basic_block;
            GtkMl_S *bb_ld = gtk_ml_new_lightdata(b->intr_ctx, NULL, bb);
            GtkMl_S *bb_var = gtk_ml_new_var(b->intr_ctx, NULL, bb_ld);
            gtk_ml_define(tmp_ctx, gtk_ml_new_symbol(ctx, NULL, 0, "CTX", strlen("CTX")), gtk_ml_new_lightdata(b->intr_ctx, NULL, ctx));
            gtk_ml_define(tmp_ctx, gtk_ml_new_symbol(ctx, NULL, 0, "CODE-BUILDER", strlen("CODE-BUILDER")), gtk_ml_new_lightdata(b->intr_ctx, NULL, b));
            gtk_ml_define(tmp_ctx, gtk_ml_new_symbol(ctx, NULL, 0, "BASIC-BLOCK", strlen("BASIC-BLOCK")), bb_var);
            if (!gtk_ml_run_program(tmp_ctx, err, program, args)) {
                return 0;
            }
            *basic_block = bb_var->value.s_var.expr->value.s_lightdata.userdata;
        }
    }

    // first we transfer the values from the temporary context into the real context
    GtkMl_S *values = tmp_ctx->first;
    tmp_ctx->first = NULL;
    GtkMl_S **prev = &b->intr_ctx->first;
    while (*prev) {
        prev = &(*prev)->next;
    }
    *prev = values;
    // then it's safe to delete the temporary context and vm
    gtk_ml_del_context(tmp_ctx);

    return gtk_ml_build_push_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, gtk_ml_new_nil(ctx, NULL)));
}

gboolean gtk_ml_builder_compile_expr(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    (void) b;
    (void) basic_block;
    (void) allow_intr;
    (void) allow_macro;
    (void) allow_runtime;
    (void) allow_macro_expansion;
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL
            || gtk_ml_cddr(args)->kind == GTKML_S_NIL
            || gtk_ml_cdddr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_std_call(ctx, b, basic_block, err, GTKML_STD_COMPILE_EXPR, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_emit_bytecode(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    (void) b;
    (void) basic_block;
    (void) allow_intr;
    (void) allow_macro;
    (void) allow_runtime;
    (void) allow_macro_expansion;
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL
            || gtk_ml_cddr(args)->kind == GTKML_S_NIL
            || gtk_ml_cdddr(args)->kind == GTKML_S_NIL
            || gtk_ml_cddddr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_std_call(ctx, b, basic_block, err, GTKML_STD_EMIT_BYTECODE, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_append_basic_block(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    (void) b;
    (void) basic_block;
    (void) allow_intr;
    (void) allow_macro;
    (void) allow_runtime;
    (void) allow_macro_expansion;
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_std_call(ctx, b, basic_block, err, GTKML_STD_APPEND_BASIC_BLOCK, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_global_counter(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    (void) b;
    (void) basic_block;
    (void) allow_intr;
    (void) allow_macro;
    (void) allow_runtime;
    (void) allow_macro_expansion;
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_std_call(ctx, b, basic_block, err, GTKML_STD_GLOBAL_COUNTER, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_string_to_symbol(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    (void) b;
    (void) basic_block;
    (void) allow_intr;
    (void) allow_macro;
    (void) allow_runtime;
    (void) allow_macro_expansion;
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_std_call(ctx, b, basic_block, err, GTKML_STD_STRING_TO_SYMBOL, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_do(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        GtkMl_S *nil = gtk_ml_new_nil(ctx, &(*stmt)->span);
        return gtk_ml_compile_expression(ctx, b, basic_block, err, &nil, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
    }

    while (args->kind != GTKML_S_NIL) {
        if (!gtk_ml_compile_expression(ctx, b, basic_block, err, &gtk_ml_car(args), allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
            return 0;
        }
        args = gtk_ml_cdr(args);
        if (args->kind != GTKML_S_NIL) {
            if (!gtk_ml_build_pop(ctx, b, *basic_block, err)) {
                return 0;
            }
        }
    }

    return 1;
}

gboolean gtk_ml_builder_let(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    return gtk_ml_builder_let_star(ctx, b, basic_block, err, stmt, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_let_star(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S *bindings = gtk_ml_car(args);
    if (bindings->kind != GTKML_S_ARRAY) {
        *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    size_t len = gtk_ml_array_trie_len(&bindings->value.s_array.array);
    if (len & 1) {
        *err = gtk_ml_error(ctx, "let-error", GTKML_ERR_LET_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    if (!gtk_ml_build_enter(ctx, b, *basic_block, err)) {
        return 0;
    }

    for (size_t i = 0; i < len; i += 2) {
        GtkMl_S *key = gtk_ml_array_trie_get(&bindings->value.s_array.array, i);
        GtkMl_S *value = gtk_ml_array_trie_get(&bindings->value.s_array.array, i + 1);
        if (!gtk_ml_compile_expression(ctx, b, basic_block, err, &value, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
            return 0;
        }
        if (!gtk_ml_build_push_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, key))) {
            return 0;
        }
        if (!gtk_ml_build_bind(ctx, b, *basic_block, err)) {
            return 0;
        }
    }

    GtkMl_S *body = gtk_ml_cdr(args);

    if (body->kind == GTKML_S_NIL) {
        GtkMl_S *nil = gtk_ml_new_nil(ctx, &(*stmt)->span);
        if (!gtk_ml_compile_expression(ctx, b, basic_block, err, &nil, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
            return 0;
        }
    }

    while (body->kind != GTKML_S_NIL) {
        if (!gtk_ml_compile_expression(ctx, b, basic_block, err, &gtk_ml_car(body), allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
            return 0;
        }
        body = gtk_ml_cdr(body);
        if (body->kind != GTKML_S_NIL) {
            if (!gtk_ml_build_pop(ctx, b, *basic_block, err)) {
                return 0;
            }
        }
    }

    if (!gtk_ml_build_leave(ctx, b, *basic_block, err)) {
        return 0;
    }

    return 1;
}

gboolean gtk_ml_builder_lambda(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S *lambda_args = gtk_ml_car(args);
    GtkMl_S *lambda_body = gtk_ml_cdr(args);

    GtkMl_S *lambda = gtk_ml_new_lambda(ctx, &(*stmt)->span, lambda_args, lambda_body, gtk_ml_new_map(ctx, NULL, NULL));
    if (!lambda) {
        return 0;
    }

    return gtk_ml_compile_expression(ctx, b, basic_block, err, &lambda, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_macro(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S *macro_args = gtk_ml_car(args);
    GtkMl_S *macro_body = gtk_ml_cdr(args);

    GtkMl_S *macro = gtk_ml_new_macro(ctx, &(*stmt)->span, macro_args, macro_body, gtk_ml_new_map(ctx, NULL, NULL));
    if (!macro) {
        return 0;
    }

    return gtk_ml_compile_expression(ctx, b, basic_block, err, &macro, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_cond(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);
    GtkMl_S *_args = args;

    if (args->kind == GTKML_S_NIL) {
        return gtk_ml_build_push_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, gtk_ml_new_nil(ctx, &(*stmt)->span)));
    }

    size_t n = 0;
    while (args->kind != GTKML_S_NIL) {
        if (gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
            return 0;
        }

        ++n;

        GtkMl_S *cond = gtk_ml_car(args);
        if (cond->kind == GTKML_S_KEYWORD
                && cond->value.s_keyword.len == 4
                && strncmp(cond->value.s_keyword.ptr, "else", 4) == 0) {
            break;
        }

        args = gtk_ml_cddr(args);
    }

    GtkMl_BasicBlock **branches = malloc(sizeof(GtkMl_BasicBlock *) * (n + 1));
    uint32_t cond_number = gtk_ml_builder_get_and_inc(ctx, b)->value.s_int.value;

    char *linkage_name = malloc(strlen("cond$$end") + 16);
    snprintf(linkage_name, strlen("cond$$end") + 16, "cond$%u$end", cond_number);
    GtkMl_BasicBlock *bb = gtk_ml_append_basic_block(b, linkage_name);

    if (bb->len_exec == bb->cap_exec) {
        bb->cap_exec *= 2;
        bb->exec = realloc(bb->exec, sizeof(GtkMl_Instruction) * bb->cap_exec);
    }

    bb->exec[bb->len_exec].instr = 0;
    bb->exec[bb->len_exec].imm.cond = 0;
    bb->exec[bb->len_exec].imm.category = GTKML_EI_EXPORT;
    bb->exec[bb->len_exec].imm.imm = GTKML_EI_EXPORT_FLAG_LOCAL;
    ++bb->len_exec;

    if (bb->len_exec == bb->cap_exec) {
        bb->cap_exec *= 2;
        bb->exec = realloc(bb->exec, sizeof(GtkMl_Instruction) * bb->cap_exec);
    }

    char *name = malloc(strlen(linkage_name) + 1);
    strcpy(name, linkage_name);
    GtkMl_S *addr = gtk_ml_new_address(ctx, NULL, name, 0);
    bb->exec[bb->len_exec].imm64 = gtk_ml_append_static(b, addr);
    ++bb->len_exec;

    branches[n] = bb;

    uint32_t i = 0;
    args = _args;
    while (args->kind != GTKML_S_NIL) {
        char *linkage_name = malloc(strlen("cond$$") + 32);
        snprintf(linkage_name, strlen("cond$$") + 32, "cond$%u$%u", cond_number, i);
        GtkMl_BasicBlock *bb = gtk_ml_append_basic_block(b, linkage_name);

        if (bb->len_exec == bb->cap_exec) {
            bb->cap_exec *= 2;
            bb->exec = realloc(bb->exec, sizeof(GtkMl_Instruction) * bb->cap_exec);
        }

        bb->exec[bb->len_exec].instr = 0;
        bb->exec[bb->len_exec].imm.cond = 0;
        bb->exec[bb->len_exec].imm.category = GTKML_EI_EXPORT;
        bb->exec[bb->len_exec].imm.imm = GTKML_EI_EXPORT_FLAG_LOCAL;
        ++bb->len_exec;

        if (bb->len_exec == bb->cap_exec) {
            bb->cap_exec *= 2;
            bb->exec = realloc(bb->exec, sizeof(GtkMl_Instruction) * bb->cap_exec);
        }

        char *name = malloc(strlen(linkage_name) + 1);
        strcpy(name, linkage_name);
        GtkMl_S *addr = gtk_ml_new_address(ctx, NULL, name, 0);
        bb->exec[bb->len_exec].imm64 = gtk_ml_append_static(b, addr);
        ++bb->len_exec;

        branches[i] = bb;

        GtkMl_S *cond = gtk_ml_car(args);
        if (cond->kind == GTKML_S_KEYWORD
                && cond->value.s_keyword.len == 4
                && strncmp(cond->value.s_keyword.ptr, "else", 4) == 0) {
            break;
        }

        ++i;

        args = gtk_ml_cddr(args);
    }

    gboolean has_else = 0;
    i = 0;
    args = _args;
    while (args->kind != GTKML_S_NIL) {
        GtkMl_S **cond = &gtk_ml_car(args);

        char *branch_name = malloc(strlen("cond$$") + 32);
        snprintf(branch_name, strlen("cond$$") + 32, "cond$%u$%u", cond_number, i);

        if ((*cond)->kind == GTKML_S_KEYWORD
                && (*cond)->value.s_keyword.len == 4
                && strncmp((*cond)->value.s_keyword.ptr, "else", 4) == 0) {
            gtk_ml_builder_clear_cond(b);
            gtk_ml_build_branch_absolute_extended(ctx, b, *basic_block, err, gtk_ml_append_static(b, gtk_ml_new_string(ctx, NULL, branch_name, strlen(branch_name))));
            has_else = 1;
            break;
        }

        if (!gtk_ml_compile_cond_expression(ctx, b, basic_block, err, cond, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
            return 0;
        }
        gtk_ml_builder_set_cond(b, GTKML_F_ZERO);
        gtk_ml_build_branch_absolute_extended(ctx, b, *basic_block, err, gtk_ml_append_static(b, gtk_ml_new_string(ctx, NULL, branch_name, strlen(branch_name))));

        ++i;

        args = gtk_ml_cddr(args);
    }

    if (!has_else) {
        char *end_name = malloc(strlen("cond$$end") + 16);
        snprintf(end_name, strlen("cond$$end") + 16, "cond$%u$end", cond_number);
        gtk_ml_builder_clear_cond(b);
        gtk_ml_build_push_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, gtk_ml_new_nil(ctx, NULL)));
        gtk_ml_build_branch_absolute_extended(ctx, b, *basic_block, err, gtk_ml_append_static(b, gtk_ml_new_string(ctx, NULL, end_name, strlen(end_name))));
    }

    i = 0;
    args = _args;
    while (args->kind != GTKML_S_NIL) {
        GtkMl_S *cond = gtk_ml_car(args);
        GtkMl_S **body = &gtk_ml_cdar(args);

        if (!gtk_ml_compile_expression(ctx, b, &branches[i], err, body, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
            return 0;
        }

        char *end_name = malloc(strlen("cond$$end") + 16);
        snprintf(end_name, strlen("cond$$end") + 16, "cond$%u$end", cond_number);
        gtk_ml_builder_clear_cond(b);
        gtk_ml_build_branch_absolute_extended(ctx, b, branches[i], err, gtk_ml_append_static(b, gtk_ml_new_string(ctx, NULL, end_name, strlen(end_name))));

        if (cond->kind == GTKML_S_KEYWORD
                && cond->value.s_keyword.len == 4
                && strncmp(cond->value.s_keyword.ptr, "else", 4) == 0) {
            break;
        }

        i++;

        args = gtk_ml_cddr(args);
    }

    *basic_block = branches[n];
    free(branches);

    return 1;
}

gboolean gtk_ml_builder_while(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    uint32_t while_number = gtk_ml_builder_get_and_inc(ctx, b)->value.s_int.value;

    char *linkage_name = malloc(strlen("while$$cond") + 16);
    snprintf(linkage_name, strlen("while$$cond") + 16, "while$%u$cond", while_number);
    GtkMl_BasicBlock *cond = gtk_ml_append_basic_block(b, linkage_name);

    if (cond->len_exec == cond->cap_exec) {
        cond->cap_exec *= 2;
        cond->exec = realloc(cond->exec, sizeof(GtkMl_Instruction) * cond->cap_exec);
    }

    cond->exec[cond->len_exec].instr = 0;
    cond->exec[cond->len_exec].imm.cond = 0;
    cond->exec[cond->len_exec].imm.category = GTKML_EI_EXPORT;
    cond->exec[cond->len_exec].imm.imm = GTKML_EI_EXPORT_FLAG_LOCAL;
    ++cond->len_exec;

    if (cond->len_exec == cond->cap_exec) {
        cond->cap_exec *= 2;
        cond->exec = realloc(cond->exec, sizeof(GtkMl_Instruction) * cond->cap_exec);
    }

    char *name = malloc(strlen(linkage_name) + 1);
    strcpy(name, linkage_name);
    GtkMl_S *cond_addr = gtk_ml_new_address(ctx, NULL, name, 0);
    cond->exec[cond->len_exec].imm64 = gtk_ml_append_static(b, cond_addr);
    ++cond->len_exec;

    linkage_name = malloc(strlen("while$$end") + 16);
    snprintf(linkage_name, strlen("while$$end") + 16, "while$%u$end", while_number);
    GtkMl_BasicBlock *end = gtk_ml_append_basic_block(b, linkage_name);

    if (end->len_exec == end->cap_exec) {
        end->cap_exec *= 2;
        end->exec = realloc(end->exec, sizeof(GtkMl_Instruction) * end->cap_exec);
    }

    end->exec[end->len_exec].instr = 0;
    end->exec[end->len_exec].imm.cond = 0;
    end->exec[end->len_exec].imm.category = GTKML_EI_EXPORT;
    end->exec[end->len_exec].imm.imm = GTKML_EI_EXPORT_FLAG_LOCAL;
    ++end->len_exec;

    if (end->len_exec == end->cap_exec) {
        end->cap_exec *= 2;
        end->exec = realloc(end->exec, sizeof(GtkMl_Instruction) * end->cap_exec);
    }

    name = malloc(strlen(linkage_name) + 1);
    strcpy(name, linkage_name);
    GtkMl_S *end_addr = gtk_ml_new_address(ctx, NULL, name, 0);
    end->exec[end->len_exec].imm64 = gtk_ml_append_static(b, end_addr);
    ++end->len_exec;

    linkage_name = malloc(strlen("while$$") + 32);
    snprintf(linkage_name, strlen("while$$") + 32, "while$%u$body", while_number);
    GtkMl_BasicBlock *body = gtk_ml_append_basic_block(b, linkage_name);

    if (body->len_exec == body->cap_exec) {
        body->cap_exec *= 2;
        body->exec = realloc(body->exec, sizeof(GtkMl_Instruction) * body->cap_exec);
    }

    body->exec[body->len_exec].instr = 0;
    body->exec[body->len_exec].imm.cond = 0;
    body->exec[body->len_exec].imm.category = GTKML_EI_EXPORT;
    body->exec[body->len_exec].imm.imm = GTKML_EI_EXPORT_FLAG_LOCAL;
    ++body->len_exec;

    if (body->len_exec == body->cap_exec) {
        body->cap_exec *= 2;
        body->exec = realloc(body->exec, sizeof(GtkMl_Instruction) * body->cap_exec);
    }

    name = malloc(strlen(linkage_name) + 1);
    strcpy(name, linkage_name);
    GtkMl_S *body_addr = gtk_ml_new_address(ctx, NULL, name, 0);
    body->exec[body->len_exec].imm64 = gtk_ml_append_static(b, body_addr);
    ++body->len_exec;

    char *body_name = malloc(strlen("while$$body") + 16);
    snprintf(body_name, strlen("while$$body") + 16, "while$%u$body", while_number);
    char *end_name = malloc(strlen("while$$end") + 16);
    snprintf(end_name, strlen("while$$end") + 16, "while$%u$end", while_number);

    gtk_ml_compile_cond_expression(ctx, b, &cond, err, &gtk_ml_car(args), allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
    // if true: jump to body
    gtk_ml_builder_set_cond(b, GTKML_F_ZERO);
    gtk_ml_build_branch_absolute_extended(ctx, b, cond, err, gtk_ml_append_static(b, gtk_ml_new_string(ctx, NULL, body_name, strlen(body_name))));
    // else: jump to end
    gtk_ml_build_branch_absolute_extended(ctx, b, cond, err, gtk_ml_append_static(b, gtk_ml_new_string(ctx, NULL, end_name, strlen(end_name))));

    args = gtk_ml_cdr(args);
    while (args->kind != GTKML_S_NIL) {
        char *cond_name = malloc(strlen("while$$cond") + 16);
        snprintf(cond_name, strlen("while$$cond") + 16, "while$%u$cond", while_number);

        gtk_ml_compile_expression(ctx, b, &body, err, &gtk_ml_car(args), allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
        gtk_ml_build_branch_absolute_extended(ctx, b, body, err, gtk_ml_append_static(b, gtk_ml_new_string(ctx, NULL, cond_name, strlen(cond_name))));
        args = gtk_ml_cdr(args);
    }

    *basic_block = end;

    return gtk_ml_build_push_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, gtk_ml_new_nil(ctx, NULL)));
}

gboolean gtk_ml_builder_len(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S **container = &gtk_ml_car(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, container, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_len(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_car(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S **list = &gtk_ml_car(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, list, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_car(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_cdr(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S **list = &gtk_ml_car(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, list, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_cdr(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_typeof(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S **value = &gtk_ml_car(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, value, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_typeof(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_index(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S **array = &gtk_ml_car(args);
    GtkMl_S **index = &gtk_ml_cdar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, index, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, array, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_array_index(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_push(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S **array = &gtk_ml_car(args);
    GtkMl_S **value = &gtk_ml_cdar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, value, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, array, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_array_push(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_pop(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S **array = &gtk_ml_car(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, array, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_array_pop(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_concat(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S **a1 = &gtk_ml_car(args);
    GtkMl_S **a2 = &gtk_ml_cdar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, a2, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, a1, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_array_concat(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_add(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S **lhs = &gtk_ml_car(args);
    GtkMl_S **rhs = &gtk_ml_cdar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, rhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, lhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_add(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_sub(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind != GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        GtkMl_S **value = &gtk_ml_car(args);
        GtkMl_S *zero = gtk_ml_new_int(ctx, &(*stmt)->span, 0);

        if (!gtk_ml_compile_expression(ctx, b, basic_block, err, value, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
            return 0;
        }
        if (!gtk_ml_compile_expression(ctx, b, basic_block, err, &zero, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
            return 0;
        }
        return gtk_ml_build_sub(ctx, b, *basic_block, err);
    }

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S **lhs = &gtk_ml_car(args);
    GtkMl_S **rhs = &gtk_ml_cdar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, rhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, lhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_sub(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_mul(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S **lhs = &gtk_ml_car(args);
    GtkMl_S **rhs = &gtk_ml_cdar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, rhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, lhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_mul(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_div(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S **lhs = &gtk_ml_car(args);
    GtkMl_S **rhs = &gtk_ml_cdar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, rhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, lhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_div(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_mod(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S **lhs = &gtk_ml_car(args);
    GtkMl_S **rhs = &gtk_ml_cdar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, rhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, lhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_mod(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_bitnot(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S **value = &gtk_ml_car(args);
    GtkMl_S *ff = gtk_ml_new_int(ctx, &(*stmt)->span, 0xfffffffffffffffful);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, &ff, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, value, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_bitxor(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_bitand(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S **lhs = &gtk_ml_car(args);
    GtkMl_S **rhs = &gtk_ml_cdar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, rhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, lhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_bitand(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_bitor(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S **lhs = &gtk_ml_car(args);
    GtkMl_S **rhs = &gtk_ml_cdar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, rhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, lhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_bitor(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_bitxor(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S **lhs = &gtk_ml_car(args);
    GtkMl_S **rhs = &gtk_ml_cdar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, rhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, lhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_bitxor(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_cmp(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL
            || gtk_ml_cddr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S *cmp = gtk_ml_car(args);
    GtkMl_S **lhs = &gtk_ml_cdar(args);
    GtkMl_S **rhs = &gtk_ml_cddar(args);

    if (cmp->kind != GTKML_S_INT) {
        *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }
    switch ((GtkMl_Cmp) cmp->value.s_int.value) {
    case GTKML_CMP_EQUAL:
    case GTKML_CMP_NOT_EQUAL:
    case GTKML_CMP_LESS:
    case GTKML_CMP_GREATER:
    case GTKML_CMP_LESS_EQUAL:
    case GTKML_CMP_GREATER_EQUAL:
        break;
    default:
        *err = gtk_ml_error(ctx, "cmp-error", GTKML_ERR_CMP_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, rhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, lhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_cmp(ctx, b, *basic_block, err, gtk_ml_append_static(b, cmp));
}

gboolean gtk_ml_builder_var(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S *var = gtk_ml_new_var(ctx, &(*stmt)->span, gtk_ml_car(args));
    if (!var) {
        return 0;
    }

    return gtk_ml_compile_expression(ctx, b, basic_block, err, &var, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_vararg(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S *vararg = gtk_ml_new_vararg(ctx, &(*stmt)->span, gtk_ml_car(args));
    if (!vararg) {
        return 0;
    }

    return gtk_ml_compile_expression(ctx, b, basic_block, err, &vararg, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_quote(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S *quote = gtk_ml_new_quote(ctx, &(*stmt)->span, gtk_ml_car(args));
    if (!quote) {
        return 0;
    }

    return gtk_ml_compile_expression(ctx, b, basic_block, err, &quote, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_quasiquote(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S *quasiquote = gtk_ml_new_quasiquote(ctx, &(*stmt)->span, gtk_ml_car(args));
    if (!quasiquote) {
        return 0;
    }

    return gtk_ml_compile_expression(ctx, b, basic_block, err, &quasiquote, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_unquote(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S *unquote = gtk_ml_new_unquote(ctx, &(*stmt)->span, gtk_ml_car(args));
    if (!unquote) {
        return 0;
    }

    return gtk_ml_compile_expression(ctx, b, basic_block, err, &unquote, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_compile_cond_expression(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    switch ((*stmt)->kind) {
    case GTKML_S_FALSE:
    case GTKML_S_TRUE:
        return gtk_ml_build_setf_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, *stmt));
    case GTKML_S_NIL:
        return gtk_ml_build_setf_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, gtk_ml_new_false(ctx, &(*stmt)->span)));
    case GTKML_S_INT:
    case GTKML_S_FLOAT:
    case GTKML_S_CHAR:
    case GTKML_S_KEYWORD:
    case GTKML_S_PROGRAM:
    case GTKML_S_ADDRESS:
    case GTKML_S_LIGHTDATA:
    case GTKML_S_USERDATA:
    case GTKML_S_LAMBDA: 
    case GTKML_S_MACRO:
    case GTKML_S_MAP:
    case GTKML_S_SET:
    case GTKML_S_ARRAY: 
    case GTKML_S_VAR:
        return gtk_ml_build_setf_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, gtk_ml_new_true(ctx, &(*stmt)->span)));
    case GTKML_S_LIST:
    case GTKML_S_SYMBOL:
        if (!gtk_ml_compile_expression(ctx, b, basic_block, err, stmt, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
            return 0;
        }
        return gtk_ml_build_popf(ctx, b, *basic_block, err);
    case GTKML_S_VARARG:
        *err = gtk_ml_error(ctx, "vararg-error", GTKML_ERR_VARARG_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    case GTKML_S_QUOTE:
        if ((*stmt)->value.s_quote.expr->kind == GTKML_S_SYMBOL || (*stmt)->value.s_quote.expr->kind == GTKML_S_LIST) {
            return gtk_ml_build_setf_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, gtk_ml_new_true(ctx, &(*stmt)->span)));
        } else {
            return gtk_ml_compile_cond_expression(ctx, b, basic_block, err, &(*stmt)->value.s_quote.expr, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
        }
    case GTKML_S_QUASIQUOTE:
        if ((*stmt)->value.s_quote.expr->kind == GTKML_S_SYMBOL || (*stmt)->value.s_quote.expr->kind == GTKML_S_LIST) {
            return gtk_ml_build_setf_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, gtk_ml_new_true(ctx, &(*stmt)->span)));
        } else {
            return gtk_ml_compile_cond_expression(ctx, b, basic_block, err, &(*stmt)->value.s_quote.expr, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
        }
    case GTKML_S_UNQUOTE:
        *err = gtk_ml_error(ctx, "unquote-error", GTKML_ERR_UNQUOTE_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }
}

gboolean compile_std_call(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, uint64_t function, GtkMl_S *args, gboolean compile_first, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    int64_t n = 0;
    if (!compile_first) {
        if (!gtk_ml_build_push_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, gtk_ml_car(args)))) {
            return 0;
        }
        args = gtk_ml_cdr(args);
        ++n;
    }
    while (args->kind != GTKML_S_NIL) {
        if (!gtk_ml_compile_expression(ctx, b, basic_block, err, &gtk_ml_car(args), allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
            return 0;
        }
        args = gtk_ml_cdr(args);
        ++n;
    }

    if (!gtk_ml_build_push_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, gtk_ml_new_int(ctx, NULL, n)))) {
        return 0;
    }
    if (!gtk_ml_build_call_extended_std(ctx, b, *basic_block, err, gtk_ml_append_static(b, gtk_ml_new_int(ctx, NULL, function)))) {
        return 0;
    }

    return 1;
}

struct CompileData {
    GtkMl_Context *ctx;
    GtkMl_Builder *b;
    GtkMl_BasicBlock **basic_block;
    GtkMl_S **err;
    GtkMl_S *stmt;
    gboolean allow_intr;
    gboolean allow_macro;
    gboolean allow_runtime;
    gboolean allow_macro_expansion;
    gboolean *unwrapped;
    gboolean result;
};

GTKML_PRIVATE GtkMl_VisitResult compile_quasi_map(GtkMl_HashTrie *ht, void *key, void *value, void *_data) {
    (void) ht;

    struct CompileData *data = _data;
    if (!gtk_ml_compile_quasi_expression(data->ctx, data->b, data->basic_block, data->err, key, data->allow_intr, data->allow_macro, data->allow_runtime, data->allow_macro_expansion, data->unwrapped)) {
        data->result = 0;
        return GTKML_VISIT_BREAK;
    }
    if (!gtk_ml_compile_quasi_expression(data->ctx, data->b, data->basic_block, data->err, value, data->allow_intr, data->allow_macro, data->allow_runtime, data->allow_macro_expansion, data->unwrapped)) {
        data->result = 0;
        return GTKML_VISIT_BREAK;
    }
    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult compile_quasi_set(GtkMl_HashSet *hs, void *key, void *_data) {
    (void) hs;

    struct CompileData *data = _data;
    if (!gtk_ml_compile_quasi_expression(data->ctx, data->b, data->basic_block, data->err, key, data->allow_intr, data->allow_macro, data->allow_runtime, data->allow_macro_expansion, data->unwrapped)) {
        data->result = 0;
        return GTKML_VISIT_BREAK;
    }
    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult compile_quasi_array(GtkMl_Array *array, size_t idx, GtkMl_S *value, void *_data) {
    (void) array;
    (void) idx;

    struct CompileData *data = _data;
    if (!gtk_ml_compile_quasi_expression(data->ctx, data->b, data->basic_block, data->err, value, data->allow_intr, data->allow_macro, data->allow_runtime, data->allow_macro_expansion, data->unwrapped)) {
        data->result = 0;
        return GTKML_VISIT_BREAK;
    }
    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult compile_map(GtkMl_HashTrie *ht, void *key_ptr, void *value_ptr, void *_data) {
    (void) ht;
    GtkMl_S *key = key_ptr;
    GtkMl_S *value = value_ptr;

    struct CompileData *data = _data;
    if (!gtk_ml_compile_expression(data->ctx, data->b, data->basic_block, data->err, &key, data->allow_intr, data->allow_macro, data->allow_runtime, data->allow_macro_expansion)) {
        data->result = 0;
        return GTKML_VISIT_BREAK;
    }
    if (!gtk_ml_compile_expression(data->ctx, data->b, data->basic_block, data->err, &value, data->allow_intr, data->allow_macro, data->allow_runtime, data->allow_macro_expansion)) {
        data->result = 0;
        return GTKML_VISIT_BREAK;
    }
    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult compile_set(GtkMl_HashSet *hs, void *key_ptr, void *_data) {
    (void) hs;
    GtkMl_S *key = key_ptr;

    struct CompileData *data = _data;
    if (!gtk_ml_compile_expression(data->ctx, data->b, data->basic_block, data->err, &key, data->allow_intr, data->allow_macro, data->allow_runtime, data->allow_macro_expansion)) {
        data->result = 0;
        return GTKML_VISIT_BREAK;
    }
    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult compile_array(GtkMl_Array *array, size_t idx, GtkMl_S *value, void *_data) {
    (void) array;
    (void) idx;

    struct CompileData *data = _data;
    if (!gtk_ml_compile_expression(data->ctx, data->b, data->basic_block, data->err, &value, data->allow_intr, data->allow_macro, data->allow_runtime, data->allow_macro_expansion)) {
        data->result = 0;
        return GTKML_VISIT_BREAK;
    }
    return GTKML_VISIT_RECURSE;
}

gboolean compile_runtime_program(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, const char *linkage_name, GtkMl_S *lambda, gboolean ret) {
    return gtk_ml_compile_program_internal(ctx, b, basic_block, err, linkage_name, lambda, ret, 0, 0, 1, 1, GTKML_PROG_RUNTIME);
}

gboolean gtk_ml_compile_quasi_expression(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion, gboolean *unwrapped) {
    switch (stmt->kind) {
    case GTKML_S_NIL:
    case GTKML_S_TRUE:
    case GTKML_S_FALSE:
    case GTKML_S_INT:
    case GTKML_S_FLOAT:
    case GTKML_S_CHAR:
    case GTKML_S_KEYWORD:
    case GTKML_S_PROGRAM:
    case GTKML_S_ADDRESS:
    case GTKML_S_LIGHTDATA:
    case GTKML_S_USERDATA:
    case GTKML_S_SYMBOL:
    case GTKML_S_LAMBDA:
    case GTKML_S_MACRO:
    case GTKML_S_VAR:
    case GTKML_S_VARARG:
    case GTKML_S_QUOTE:
    case GTKML_S_QUASIQUOTE:
        return gtk_ml_build_push_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, stmt));
    case GTKML_S_MAP: {
        int64_t n = gtk_ml_hash_trie_len(&stmt->value.s_map.map);
        struct CompileData data = { ctx, b, basic_block, err, stmt, allow_intr, allow_macro, allow_runtime, allow_macro_expansion, unwrapped, 1 }; 
        gtk_ml_hash_trie_foreach(&stmt->value.s_map.map, compile_quasi_map, &data);
        return gtk_ml_build_map_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, gtk_ml_new_int(ctx, NULL, n)));
    }
    case GTKML_S_SET: {
        int64_t n = gtk_ml_hash_set_len(&stmt->value.s_set.set);
        struct CompileData data = { ctx, b, basic_block, err, stmt, allow_intr, allow_macro, allow_runtime, allow_macro_expansion, unwrapped, 1 }; 
        gtk_ml_hash_set_foreach(&stmt->value.s_set.set, compile_quasi_set, &data);
        return gtk_ml_build_set_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, gtk_ml_new_int(ctx, NULL, n)));
    }
    case GTKML_S_ARRAY: {
        int64_t n = gtk_ml_array_trie_len(&stmt->value.s_array.array);
        struct CompileData data = { ctx, b, basic_block, err, stmt, allow_intr, allow_macro, allow_runtime, allow_macro_expansion, unwrapped, 1 }; 
        gtk_ml_array_trie_foreach_rev(&stmt->value.s_array.array, compile_quasi_array, &data);
        return gtk_ml_build_array_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, gtk_ml_new_int(ctx, NULL, n)));
    }
    case GTKML_S_UNQUOTE: {
        GtkMl_S **expr = &stmt->value.s_unquote.expr;
        if ((*expr)->kind == GTKML_S_VARARG) {
            if (!gtk_ml_compile_expression(ctx, b, basic_block, err, &(*expr)->value.s_vararg.expr, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
                return 0;
            }
            if (!gtk_ml_build_unwrap(ctx, b, *basic_block, err)) {
                return 0;
            }
            if (unwrapped) {
                *unwrapped = 1;
            }
            return 1;
        } else {
            return gtk_ml_compile_expression(ctx, b, basic_block, err, expr, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);

        }

    } break;
    case GTKML_S_LIST: {
        GtkMl_S *function = gtk_ml_car(stmt);

        GtkMl_S *args = gtk_ml_cdr(stmt);

        if (function->kind == GTKML_S_SYMBOL) {
            const char *symbol_unquote = "unquote";

            if (function->value.s_symbol.len == strlen(symbol_unquote)
                    && memcmp(function->value.s_symbol.ptr, symbol_unquote, function->value.s_symbol.len) == 0) {
                if (args->kind == GTKML_S_NIL) {
                    *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, stmt->span.ptr != NULL, stmt->span.line, stmt->span.col, 0);
                    return 0;
                }

                GtkMl_S **expr = &gtk_ml_car(args);
                if ((*expr)->kind == GTKML_S_VARARG) {
                    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, &(*expr)->value.s_vararg.expr, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
                        return 0;
                    }
                    if (!gtk_ml_build_unwrap(ctx, b, *basic_block, err)) {
                        return 0;
                    }
                    if (unwrapped) {
                        *unwrapped = 1;
                    }
                    return 1;
                } else {
                    return gtk_ml_compile_expression(ctx, b, basic_block, err, expr, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);

                }

            }
        }

        gboolean unwrapped = 0;
        int64_t n = 0;
        while (stmt->kind != GTKML_S_NIL) {
            if (unwrapped) {
                stmt = gtk_ml_cdr(stmt);
                *err = gtk_ml_error(ctx, "unwrap-error", GTKML_ERR_UNWRAP_ERROR, stmt->span.ptr != NULL, stmt->span.line, stmt->span.col, 0);
                return 0;
            }
            if (!gtk_ml_compile_quasi_expression(ctx, b, basic_block, err, gtk_ml_car(stmt), allow_intr, allow_macro, allow_runtime, allow_macro_expansion, &unwrapped)) {
                return 0;
            }
            stmt = gtk_ml_cdr(stmt);
            ++n;
        }
        if (unwrapped) {
            if (!gtk_ml_build_push_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, gtk_ml_new_int(ctx, NULL, n - 1)))) {
                return 0;
            }
            if (!gtk_ml_build_add(ctx, b, *basic_block, err)) {
                return 0;
            }
            return gtk_ml_build_list(ctx, b, *basic_block, err);
        } else {
            return gtk_ml_build_list_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, gtk_ml_new_int(ctx, NULL, n)));
        }
    }
    }
}

gboolean gtk_ml_compile_expression(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    switch ((*stmt)->kind) {
    case GTKML_S_NIL:
    case GTKML_S_TRUE:
    case GTKML_S_FALSE:
    case GTKML_S_INT:
    case GTKML_S_FLOAT:
    case GTKML_S_CHAR:
    case GTKML_S_KEYWORD:
    case GTKML_S_PROGRAM:
    case GTKML_S_ADDRESS:
    case GTKML_S_LIGHTDATA:
    case GTKML_S_USERDATA:
        return gtk_ml_build_push_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, *stmt));
    case GTKML_S_SYMBOL:
        return gtk_ml_build_get_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, *stmt));
    case GTKML_S_LAMBDA: {
        char *linkage_name = malloc(strlen("lambda$") + 16);
        snprintf(linkage_name, strlen("lambda$") + 16, "lambda$%lu", gtk_ml_builder_get_and_inc(ctx, b)->value.s_int.value);
        GtkMl_BasicBlock *bb = gtk_ml_append_basic_block(b, linkage_name);
        if (!gtk_ml_compile_program_internal(ctx, b, &bb, err, linkage_name, *stmt, 1, allow_intr, allow_macro, allow_runtime, allow_macro_expansion, GTKML_PROG_RUNTIME)) {
            return 0;
        }
        return gtk_ml_build_push_extended_addr(ctx, b, *basic_block, err, gtk_ml_append_static(b, gtk_ml_new_string(ctx, NULL, linkage_name, strlen(linkage_name))));
    }
    case GTKML_S_MACRO: if (allow_macro) {
        char *linkage_name = malloc(strlen("macro$") + 16);
        snprintf(linkage_name, strlen("macro$") + 16, "macro$%lu", gtk_ml_builder_get_and_inc(ctx, b)->value.s_int.value);
        GtkMl_BasicBlock *bb = gtk_ml_append_basic_block(b, linkage_name);
        if (!gtk_ml_compile_program_internal(ctx, b, &bb, err, linkage_name, *stmt, 1, allow_intr, allow_macro, allow_runtime, allow_macro_expansion, GTKML_PROG_MACRO)) {
            return 0;
        }
        return gtk_ml_build_push_extended_addr(ctx, b, *basic_block, err, gtk_ml_append_static(b, gtk_ml_new_string(ctx, NULL, linkage_name, strlen(linkage_name))));
    }
    case GTKML_S_MAP: {
        int64_t n = gtk_ml_hash_trie_len(&(*stmt)->value.s_map.map);
        struct CompileData data = { ctx, b, basic_block, err, (*stmt), allow_intr, allow_macro, allow_runtime, allow_macro_expansion, NULL, 1 }; 
        gtk_ml_hash_trie_foreach(&(*stmt)->value.s_map.map, compile_map, &data);
        return gtk_ml_build_map_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, gtk_ml_new_int(ctx, NULL, n)));
    }
    case GTKML_S_SET: {
        int64_t n = gtk_ml_hash_set_len(&(*stmt)->value.s_set.set);
        struct CompileData data = { ctx, b, basic_block, err, (*stmt), allow_intr, allow_macro, allow_runtime, allow_macro_expansion, NULL, 1 }; 
        gtk_ml_hash_set_foreach(&(*stmt)->value.s_set.set, compile_set, &data);
        return gtk_ml_build_set_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, gtk_ml_new_int(ctx, NULL, n)));
    }
    case GTKML_S_ARRAY: {
        int64_t n = gtk_ml_array_trie_len(&(*stmt)->value.s_array.array);
        struct CompileData data = { ctx, b, basic_block, err, (*stmt), allow_intr, allow_macro, allow_runtime, allow_macro_expansion, NULL, 1 }; 
        gtk_ml_array_trie_foreach_rev(&(*stmt)->value.s_array.array, compile_array, &data);
        return gtk_ml_build_array_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, gtk_ml_new_int(ctx, NULL, n)));
    }
    case GTKML_S_VAR:
        gtk_ml_compile_expression(ctx, b, basic_block, err, &(*stmt)->value.s_var.expr, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
        return gtk_ml_build_var_imm(ctx, b, *basic_block, err);
    case GTKML_S_VARARG:
        *err = gtk_ml_error(ctx, "vararg-error", GTKML_ERR_VARARG_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    case GTKML_S_QUOTE:
        return gtk_ml_build_push_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, (*stmt)->value.s_quote.expr));
    case GTKML_S_QUASIQUOTE:
        return gtk_ml_compile_quasi_expression(ctx, b, basic_block, err, (*stmt)->value.s_quasiquote.expr, allow_intr, allow_macro, allow_runtime, allow_macro_expansion, NULL);
    case GTKML_S_UNQUOTE:
        *err = gtk_ml_error(ctx, "unquote-error", GTKML_ERR_UNQUOTE_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    case GTKML_S_LIST: {
        GtkMl_S **function = &gtk_ml_car(*stmt);
        GtkMl_S *args = gtk_ml_cdr(*stmt);

        if ((*function)->kind == GTKML_S_SYMBOL) {
            size_t len = (*function)->value.s_symbol.len;
            const char *ptr = (*function)->value.s_symbol.ptr;

            for (size_t i = 0; i < b->len_builder; i++) {
                GtkMl_BuilderMacro *bm = b->builders + i;
                if (strlen(bm->name) == len && strncmp(bm->name, ptr, len) == 0) {
                    if (bm->require_intrinsic) {
                        if (allow_intr) {
                            return bm->fn(ctx, b, basic_block, err, stmt, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
                        }
                    } else if (bm->require_macro) {
                        if (allow_macro) {
                            return bm->fn(ctx, b, basic_block, err, stmt, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
                        }
                    } else if (bm->require_runtime) {
                        if (allow_runtime) {
                            return bm->fn(ctx, b, basic_block, err, stmt, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
                        }
                    } else {
                        return bm->fn(ctx, b, basic_block, err, stmt, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
                    }
                }
            }
        }

        if (!allow_intr && (*function)->kind == GTKML_S_SYMBOL) {
            size_t len = (*function)->value.s_symbol.len;
            char *linkage_name = malloc(len + 1);
            memcpy(linkage_name, (*function)->value.s_symbol.ptr, len);
            linkage_name[len] = 0;

            GtkMl_S *_err = NULL;
            GtkMl_S *program = gtk_ml_get_export(b->intr_ctx, &_err, linkage_name);
            free(linkage_name);
            if (program && program->value.s_program.kind == GTKML_PROG_INTRINSIC) {
                GtkMl_BasicBlock *bb = *basic_block;
                GtkMl_S *bb_ld = gtk_ml_new_lightdata(b->intr_ctx, NULL, bb);
                GtkMl_S *bb_var = gtk_ml_new_var(b->intr_ctx, NULL, bb_ld);
                gtk_ml_define(b->intr_ctx, gtk_ml_new_symbol(ctx, NULL, 0, "CTX", strlen("CTX")), gtk_ml_new_lightdata(b->intr_ctx, NULL, ctx));
                gtk_ml_define(b->intr_ctx, gtk_ml_new_symbol(ctx, NULL, 0, "CODE-BUILDER", strlen("CODE-BUILDER")), gtk_ml_new_lightdata(b->intr_ctx, NULL, b));
                gtk_ml_define(b->intr_ctx, gtk_ml_new_symbol(ctx, NULL, 0, "BASIC-BLOCK", strlen("BASIC-BLOCK")), bb_var);
                if (!gtk_ml_run_program(b->intr_ctx, err, program, args)) {
                    return 0;
                }
                *basic_block = bb_var->value.s_var.expr->value.s_lightdata.userdata;
                return 1;
            }
        }

        if (allow_macro_expansion && (*function)->kind == GTKML_S_SYMBOL) {
            size_t len = (*function)->value.s_symbol.len;
            char *linkage_name = malloc(len + 1);
            memcpy(linkage_name, (*function)->value.s_symbol.ptr, len);
            linkage_name[len] = 0;

            GtkMl_S *_err = NULL;
            GtkMl_S *program = gtk_ml_get_export(b->macro_ctx, &_err, linkage_name);
            free(linkage_name);
            if (program && program->value.s_program.kind == GTKML_PROG_MACRO) {
                if (!gtk_ml_run_program(b->macro_ctx, err, program, args)) {
                    return 0;
                }

                GtkMl_S *result = gtk_ml_peek(b->macro_ctx);

                if (!result) {
                    return 0;
                }

                *stmt = result;

                return gtk_ml_compile_expression(ctx, b, basic_block, err, stmt, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
            }
        }

        gboolean unwrapped = 0;
        int64_t n_args = 0;
        while (args->kind != GTKML_S_NIL) {
            if (unwrapped) {
                *err = gtk_ml_error(ctx, "unwrap-error", GTKML_ERR_UNWRAP_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
                return 0;
            }
            GtkMl_S **arg = &gtk_ml_car(args);
            if ((*arg)->kind == GTKML_S_VARARG) { // unwrap
                if (!gtk_ml_compile_expression(ctx, b, basic_block, err, &(*arg)->value.s_vararg.expr, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
                    return 0;
                }
                if (!gtk_ml_build_unwrap(ctx, b, *basic_block, err)) {
                    return 0;
                }
                unwrapped = 1;
            } else {
                if (!gtk_ml_compile_expression(ctx, b, basic_block, err, arg, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
                    return 0;
                }
                ++n_args;
            }
            args = gtk_ml_cdr(args);
        }

        if (!gtk_ml_build_push_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, gtk_ml_new_int(ctx, NULL, n_args)))) {
            return 0;
        }

        if (unwrapped) {
            if (!gtk_ml_build_add(ctx, b, *basic_block, err)) {
                return 0;
            }
        }

        if (!gtk_ml_compile_expression(ctx, b, basic_block, err, function, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
            return 0;
        }

        return gtk_ml_build_call(ctx, b, *basic_block, err);
    }
    }
}

gboolean gtk_ml_compile_body(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S *body, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    while (body->kind != GTKML_S_NIL) {
        GtkMl_S **stmt = &gtk_ml_car(body);
        if (!gtk_ml_compile_expression(ctx, b, basic_block, err, stmt, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
            return 0;
        }
        body = gtk_ml_cdr(body);
    }

    return 1;
}

gboolean gtk_ml_compile_program_internal(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, const char *linkage_name, GtkMl_S *lambda, gboolean ret, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion, GtkMl_ProgramKind kind) {
    if ((!allow_macro || lambda->kind != GTKML_S_MACRO) && lambda->kind != GTKML_S_LAMBDA) {
        *err = gtk_ml_error(ctx, "program-error", GTKML_ERR_PROGRAM_ERROR, lambda->span.ptr != NULL, lambda->span.line, lambda->span.col, 0);
        return 0;
    }

    if ((*basic_block)->len_exec == (*basic_block)->cap_exec) {
        (*basic_block)->cap_exec *= 2;
        (*basic_block)->exec = realloc((*basic_block)->exec, sizeof(GtkMl_Instruction) * (*basic_block)->cap_exec);
    }

    (*basic_block)->exec[(*basic_block)->len_exec].instr = 0;
    (*basic_block)->exec[(*basic_block)->len_exec].imm.cond = 0;
    (*basic_block)->exec[(*basic_block)->len_exec].imm.category = GTKML_EI_EXPORT;
    (*basic_block)->exec[(*basic_block)->len_exec].imm.imm = GTKML_EI_EXPORT_FLAG_NONE;
    ++(*basic_block)->len_exec;

    if ((*basic_block)->len_exec == (*basic_block)->cap_exec) {
        (*basic_block)->cap_exec *= 2;
        (*basic_block)->exec = realloc((*basic_block)->exec, sizeof(GtkMl_Instruction) * (*basic_block)->cap_exec);
    }

    char *name = malloc(strlen(linkage_name) + 1);
    strcpy(name, linkage_name);
    GtkMl_S *program = gtk_ml_new_program(ctx, &lambda->span, name, 0, lambda->value.s_lambda.args, lambda->value.s_lambda.body, lambda->value.s_lambda.capture, kind);
    (*basic_block)->exec[(*basic_block)->len_exec].imm64 = gtk_ml_append_static(b, program);
    ++(*basic_block)->len_exec;

    GtkMl_S *params = lambda->value.s_lambda.args;
    GtkMl_S *revparams = gtk_ml_new_nil(ctx, NULL);
    while (params->kind != GTKML_S_NIL) {
        revparams = gtk_ml_new_list(ctx, NULL, gtk_ml_car(params), revparams);
        params = gtk_ml_cdr(params);
    }
    if (!gtk_ml_build_push_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, revparams))) {
        return 0;
    }

    if (!gtk_ml_build_bind_args(ctx, b, *basic_block, err)) {
        return 0;
    }

    if (!gtk_ml_compile_body(ctx, b, basic_block, err, lambda->value.s_lambda.body, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }

    if (ret) {
        if (!gtk_ml_build_ret(ctx, b, *basic_block, err)) {
            return 0;
        }
    }

    return 1;
}

struct CollectData {
    gboolean change;
    GtkMl_Context *ctx;
    GtkMl_Builder *b;
    gboolean *has_intr;
};

GTKML_PRIVATE gboolean collect_intrinsics_inner(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_S *expr, gboolean *has_intr);

GTKML_PRIVATE GtkMl_VisitResult collect_intrinsics_map(GtkMl_HashTrie *ht, void *key, void *value, void *data) {
    (void) ht;

    struct CollectData *col = data;

    col->change |= collect_intrinsics_inner(col->ctx, col->b, key, col->has_intr);
    col->change |= collect_intrinsics_inner(col->ctx, col->b, value, col->has_intr);

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult collect_intrinsics_set(GtkMl_HashSet *hs, void *key, void *data) {
    (void) hs;

    struct CollectData *col = data;

    col->change |= collect_intrinsics_inner(col->ctx, col->b, key, col->has_intr);

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult collect_intrinsics_array(GtkMl_Array *array, size_t idx, GtkMl_S *value, void *data) {
    (void) array;
    (void) idx;

    struct CollectData *col = data;

    col->change |= collect_intrinsics_inner(col->ctx, col->b, value, col->has_intr);

    return GTKML_VISIT_RECURSE;
}

gboolean collect_intrinsics_inner(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_S *expr, gboolean *has_intr) {
    switch (expr->kind) {
    case GTKML_S_LAMBDA: {
        gboolean change = 0;
        GtkMl_S *body = expr->value.s_lambda.body;
        while (body->kind != GTKML_S_NIL) {
            change |= collect_intrinsics_inner(ctx, b, gtk_ml_car(body), has_intr);
            body = gtk_ml_cdr(body);
        }
        return change;
    }
    case GTKML_S_MACRO: {
        gboolean change = 0;
        GtkMl_S *body = expr->value.s_macro.body;
        while (body->kind != GTKML_S_NIL) {
            change |= collect_intrinsics_inner(ctx, b, gtk_ml_car(body), has_intr);
            body = gtk_ml_cdr(body);
        }
        return change;
    }
    case GTKML_S_LIST: {
        gboolean change = 0;
        GtkMl_S *fn = gtk_ml_car(expr);
        if (fn->kind == GTKML_S_SYMBOL) {
            const char *symbol_define_intrinsic = "define-intrinsic";
            const char *symbol_define_macro = "define-macro";
            const char *symbol_define = "define";

            if (fn->value.s_symbol.len == strlen(symbol_define_intrinsic)
                    && memcmp(fn->value.s_symbol.ptr, symbol_define_intrinsic, fn->value.s_symbol.len) == 0) {
                GtkMl_S *def = gtk_ml_cdar(expr);
                GtkMl_S *body = gtk_ml_cddr(expr);
                GtkMl_S *name = gtk_ml_car(def);

                if (gtk_ml_hash_set_contains(&b->intr_fns, name)) {
                    return 0;
                }

                GtkMl_HashSet new;
                gtk_ml_hash_set_insert(&new, &b->intr_fns, name);
                gtk_ml_del_hash_set(ctx, &b->intr_fns, gtk_ml_delete_void_reference);
                b->intr_fns = new;
                *has_intr = 1;
                change = 1;

                while (body->kind != GTKML_S_NIL) {
                    collect_intrinsics_inner(ctx, b, gtk_ml_car(body), has_intr);
                    body = gtk_ml_cdr(body);
                }
            } else if (fn->value.s_symbol.len == strlen(symbol_define_macro)
                    && memcmp(fn->value.s_symbol.ptr, symbol_define_macro, fn->value.s_symbol.len) == 0) {
                GtkMl_S *def = gtk_ml_cdar(expr);
                GtkMl_S *body = gtk_ml_cddr(expr);
                GtkMl_S *name = gtk_ml_car(def);

                if (gtk_ml_hash_set_contains(&b->intr_fns, name)) {
                    return 0;
                }

                while (body->kind != GTKML_S_NIL) {
                    change |= collect_intrinsics_inner(ctx, b, gtk_ml_car(body), has_intr);
                    body = gtk_ml_cdr(body);
                }

                if (change) {
                    GtkMl_HashSet new;
                    gtk_ml_hash_set_insert(&new, &b->intr_fns, name);
                    gtk_ml_del_hash_set(ctx, &b->intr_fns, gtk_ml_delete_void_reference);
                    b->intr_fns = new;
                    *has_intr = 1;
                }
            } else if (fn->value.s_symbol.len == strlen(symbol_define)
                    && memcmp(fn->value.s_symbol.ptr, symbol_define, fn->value.s_symbol.len) == 0) {
                GtkMl_S *def = gtk_ml_cdar(expr);
                GtkMl_S *body = gtk_ml_cddr(expr);
                GtkMl_S *name = gtk_ml_car(def);

                if (gtk_ml_hash_set_contains(&b->intr_fns, name)) {
                    return 0;
                }

                while (body->kind != GTKML_S_NIL) {
                    change |= collect_intrinsics_inner(ctx, b, gtk_ml_car(body), has_intr);
                    body = gtk_ml_cdr(body);
                }

                if (change) {
                    GtkMl_HashSet new;
                    gtk_ml_hash_set_insert(&new, &b->intr_fns, name);
                    gtk_ml_del_hash_set(ctx, &b->intr_fns, gtk_ml_delete_void_reference);
                    b->intr_fns = new;
                    *has_intr = 1;
                }
            } else {
                if (gtk_ml_hash_set_contains(&b->intr_fns, fn)) {
                    return 0;
                }
            }
        }
        return change;
    }
    case GTKML_S_ARRAY: {
        struct CollectData col = { 0, ctx, b, has_intr };
        gtk_ml_array_trie_foreach(&expr->value.s_array.array, collect_intrinsics_array, &col);
        return col.change;
    }
    case GTKML_S_MAP: {
        struct CollectData col = { 0, ctx, b, has_intr };
        gtk_ml_hash_trie_foreach(&expr->value.s_map.map, collect_intrinsics_map, &col);
        return col.change;
    }
    case GTKML_S_SET: {
        struct CollectData col = { 0, ctx, b, has_intr };
        gtk_ml_hash_set_foreach(&expr->value.s_set.set, collect_intrinsics_set, &col);
        return col.change;
    }
    case GTKML_S_VAR:
        return collect_intrinsics_inner(ctx, b, expr->value.s_var.expr, has_intr);
    case GTKML_S_VARARG:
        return collect_intrinsics_inner(ctx, b, expr->value.s_vararg.expr, has_intr);
    case GTKML_S_QUOTE:
        return collect_intrinsics_inner(ctx, b, expr->value.s_quote.expr, has_intr);
    case GTKML_S_QUASIQUOTE:
        return collect_intrinsics_inner(ctx, b, expr->value.s_quasiquote.expr, has_intr);
    case GTKML_S_UNQUOTE:
        return collect_intrinsics_inner(ctx, b, expr->value.s_unquote.expr, has_intr);
    default:
        return 0;
    }
}

GTKML_PRIVATE void collect_intrinsics(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_S *expr) {
    gboolean has_intr = 0;
    while (collect_intrinsics_inner(ctx, b, expr, &has_intr)) {}
}

GTKML_PRIVATE gboolean collect_macros_inner(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_S *expr, gboolean *has_macro);

GTKML_PRIVATE GtkMl_VisitResult collect_macros_map(GtkMl_HashTrie *ht, void *key, void *value, void *data) {
    (void) ht;

    struct CollectData *col = data;

    col->change |= collect_macros_inner(col->ctx, col->b, key, col->has_intr);
    col->change |= collect_macros_inner(col->ctx, col->b, value, col->has_intr);

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult collect_macros_set(GtkMl_HashSet *hs, void *key, void *data) {
    (void) hs;

    struct CollectData *col = data;

    col->change |= collect_macros_inner(col->ctx, col->b, key, col->has_intr);

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult collect_macros_array(GtkMl_Array *array, size_t idx, GtkMl_S *value, void *data) {
    (void) array;
    (void) idx;

    struct CollectData *col = data;

    col->change |= collect_macros_inner(col->ctx, col->b, value, col->has_intr);

    return GTKML_VISIT_RECURSE;
}

gboolean collect_macros_inner(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_S *expr, gboolean *has_macro) {
    switch (expr->kind) {
    case GTKML_S_LAMBDA: {
        gboolean change = 0;
        GtkMl_S *body = expr->value.s_lambda.body;
        while (body->kind != GTKML_S_NIL) {
            change |= collect_macros_inner(ctx, b, gtk_ml_car(body), has_macro);
            body = gtk_ml_cdr(body);
        }
        return change;
    }
    case GTKML_S_MACRO: {
        gboolean change = 0;
        GtkMl_S *body = expr->value.s_macro.body;
        while (body->kind != GTKML_S_NIL) {
            change |= collect_macros_inner(ctx, b, gtk_ml_car(body), has_macro);
            body = gtk_ml_cdr(body);
        }
        return change;
    }
    case GTKML_S_LIST: {
        gboolean change = 0;
        GtkMl_S *fn = gtk_ml_car(expr);
        if (fn->kind == GTKML_S_SYMBOL) {
            const char *symbol_define_macro = "define-macro";
            const char *symbol_define = "define";

            if (fn->value.s_symbol.len == strlen(symbol_define_macro)
                    && memcmp(fn->value.s_symbol.ptr, symbol_define_macro, fn->value.s_symbol.len) == 0) {
                GtkMl_S *def = gtk_ml_cdar(expr);
                GtkMl_S *body = gtk_ml_cddr(expr);
                GtkMl_S *name = gtk_ml_car(def);

                if (gtk_ml_hash_set_contains(&b->macro_fns, name)) {
                    return 0;
                }

                while (body->kind != GTKML_S_NIL) {
                    change |= collect_macros_inner(ctx, b, gtk_ml_car(body), has_macro);
                    body = gtk_ml_cdr(body);
                }

                if (change) {
                    GtkMl_HashSet new;
                    gtk_ml_hash_set_insert(&new, &b->macro_fns, name);
                    gtk_ml_del_hash_set(ctx, &b->macro_fns, gtk_ml_delete_void_reference);
                    b->macro_fns = new;
                    *has_macro = 1;
                }
            } else if (fn->value.s_symbol.len == strlen(symbol_define)
                    && memcmp(fn->value.s_symbol.ptr, symbol_define, fn->value.s_symbol.len) == 0) {
                GtkMl_S *def = gtk_ml_cdar(expr);
                GtkMl_S *body = gtk_ml_cddr(expr);
                GtkMl_S *name = gtk_ml_car(def);

                if (gtk_ml_hash_set_contains(&b->macro_fns, name)) {
                    return 0;
                }

                while (body->kind != GTKML_S_NIL) {
                    change |= collect_macros_inner(ctx, b, gtk_ml_car(body), has_macro);
                    body = gtk_ml_cdr(body);
                }

                if (change) {
                    GtkMl_HashSet new;
                    gtk_ml_hash_set_insert(&new, &b->macro_fns, name);
                    gtk_ml_del_hash_set(ctx, &b->macro_fns, gtk_ml_delete_void_reference);
                    b->macro_fns = new;
                    *has_macro = 1;
                }
            } else {
                if (gtk_ml_hash_set_contains(&b->macro_fns, fn)) {
                    return 0;
                }
            }
        }
        return change;
    }
    case GTKML_S_ARRAY: {
        struct CollectData col = { 0, ctx, b, has_macro };
        gtk_ml_array_trie_foreach(&expr->value.s_array.array, collect_macros_array, &col);
        return col.change;
    }
    case GTKML_S_MAP: {
        struct CollectData col = { 0, ctx, b, has_macro };
        gtk_ml_hash_trie_foreach(&expr->value.s_map.map, collect_macros_map, &col);
        return col.change;
    }
    case GTKML_S_SET: {
        struct CollectData col = { 0, ctx, b, has_macro };
        gtk_ml_hash_set_foreach(&expr->value.s_set.set, collect_macros_set, &col);
        return col.change;
    }
    case GTKML_S_VAR:
        return collect_macros_inner(ctx, b, expr->value.s_var.expr, has_macro);
    case GTKML_S_VARARG:
        return collect_macros_inner(ctx, b, expr->value.s_vararg.expr, has_macro);
    case GTKML_S_QUOTE:
        return collect_macros_inner(ctx, b, expr->value.s_quote.expr, has_macro);
    case GTKML_S_QUASIQUOTE:
        return collect_macros_inner(ctx, b, expr->value.s_quasiquote.expr, has_macro);
    case GTKML_S_UNQUOTE:
        return collect_macros_inner(ctx, b, expr->value.s_unquote.expr, has_macro);
    default:
        return 0;
    }
}

GTKML_PRIVATE void collect_macros(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_S *expr) {
    gboolean has_macro = 0;
    while (collect_macros_inner(ctx, b, expr, &has_macro)) {}
}

gboolean gtk_ml_compile_intrinsics(GtkMl_Builder *b, GtkMl_BasicBlock **start, GtkMl_S **err, GtkMl_S *lambda) {
    GtkMl_S *prev = lambda->value.s_lambda.body;
    GtkMl_S *next = prev;
    while (next->kind != GTKML_S_NIL) {
        GtkMl_S *stmt = gtk_ml_car(next);

        GtkMl_S *function = gtk_ml_car(stmt);

        GtkMl_S *args = gtk_ml_cdr(stmt);

        if (function->kind == GTKML_S_SYMBOL) {
            const char *symbol_define_intrinsic = "define-intrinsic";
            const char *symbol_define = "define";

            if (function->value.s_symbol.len == strlen(symbol_define_intrinsic)
                    && memcmp(function->value.s_symbol.ptr, symbol_define_intrinsic, function->value.s_symbol.len) == 0) {
                GtkMl_S *intrinsic_definition = gtk_ml_car(args);
                GtkMl_S *intrinsic_name = gtk_ml_car(intrinsic_definition);
                GtkMl_S *intrinsic_args = gtk_ml_cdr(intrinsic_definition);
                GtkMl_S *intrinsic_body = gtk_ml_cdr(args);

                GtkMl_S *intrinsic = gtk_ml_new_lambda(b->intr_ctx, NULL, intrinsic_args, intrinsic_body, gtk_ml_new_map(b->intr_ctx, NULL, NULL));
                if (!intrinsic) {
                    return 0;
                }

                size_t len = intrinsic_name->value.s_symbol.len;
                char *linkage_name = malloc(len + 1);
                memcpy(linkage_name, intrinsic_name->value.s_symbol.ptr, len);
                linkage_name[len] = 0;

                GtkMl_BasicBlock *basic_block = gtk_ml_append_basic_block(b, linkage_name);

                if (!gtk_ml_compile_program_internal(b->intr_ctx, b, &basic_block, err, linkage_name, intrinsic, 1, 1, 0, 0, 0, GTKML_PROG_INTRINSIC)) {
                    free(linkage_name);
                    return 0;
                }

                if (!gtk_ml_build_push_extended_addr(b->intr_ctx, b, *start, err, gtk_ml_append_static(b, gtk_ml_new_string(b->intr_ctx, NULL, linkage_name, strlen(linkage_name))))) {
                    return 0;
                }
                if (!gtk_ml_build_push_extended_imm(b->intr_ctx, b, *start, err, gtk_ml_append_static(b, intrinsic_name))) {
                    return 0;
                }
                if (!gtk_ml_build_define(b->intr_ctx, b, *start, err)) {
                    return 0;
                }
            } else if (function->value.s_symbol.len == strlen(symbol_define)
                    && memcmp(function->value.s_symbol.ptr, symbol_define, function->value.s_symbol.len) == 0) {
                GtkMl_S *intrinsic_definition = gtk_ml_car(args);
                GtkMl_S *intrinsic_name = gtk_ml_car(intrinsic_definition);
                if (!gtk_ml_hash_set_contains(&b->intr_fns, intrinsic_name) && !gtk_ml_hash_set_contains(&b->macro_fns, intrinsic_name)) {
                    GtkMl_S *intrinsic_args = gtk_ml_cdr(intrinsic_definition);
                    GtkMl_S *intrinsic_body = gtk_ml_cdr(args);

                    GtkMl_S *intrinsic = gtk_ml_new_lambda(b->intr_ctx, NULL, intrinsic_args, intrinsic_body, gtk_ml_new_map(b->intr_ctx, NULL, NULL));
                    if (!intrinsic) {
                        return 0;
                    }

                    size_t len = intrinsic_name->value.s_symbol.len;
                    char *linkage_name = malloc(len + 1);
                    memcpy(linkage_name, intrinsic_name->value.s_symbol.ptr, len);
                    linkage_name[len] = 0;

                    GtkMl_BasicBlock *basic_block = gtk_ml_append_basic_block(b, linkage_name);

                    if (!gtk_ml_compile_program_internal(b->intr_ctx, b, &basic_block, err, linkage_name, intrinsic, 1, 1, 0, 0, 0, GTKML_PROG_RUNTIME)) {
                        free(linkage_name);
                        return 0;
                    }

                    if (!gtk_ml_build_push_extended_addr(b->intr_ctx, b, *start, err, gtk_ml_append_static(b, gtk_ml_new_string(b->intr_ctx, NULL, linkage_name, strlen(linkage_name))))) {
                        return 0;
                    }
                    if (!gtk_ml_build_push_extended_imm(b->intr_ctx, b, *start, err, gtk_ml_append_static(b, intrinsic_name))) {
                        return 0;
                    }
                    if (!gtk_ml_build_define(b->intr_ctx, b, *start, err)) {
                        return 0;
                    }
                }
            }
        }
        next = gtk_ml_cdr(next);
    }
    
    return 1;
}

gboolean gtk_ml_compile_macros(GtkMl_Builder *b, GtkMl_BasicBlock **start, GtkMl_S **err, GtkMl_S *lambda) {
    GtkMl_S *prev = lambda->value.s_lambda.body;
    GtkMl_S *next = prev;
    while (next->kind != GTKML_S_NIL) {
        GtkMl_S *stmt = gtk_ml_car(next);

        GtkMl_S *function = gtk_ml_car(stmt);

        GtkMl_S *args = gtk_ml_cdr(stmt);

        if (function->kind == GTKML_S_SYMBOL) {
            const char *symbol_define_macro = "define-macro";
            const char *symbol_define = "define";

            if (function->value.s_symbol.len == strlen(symbol_define_macro)
                    && memcmp(function->value.s_symbol.ptr, symbol_define_macro, function->value.s_symbol.len) == 0) {
                GtkMl_S *macro_definition = gtk_ml_car(args);
                GtkMl_S *macro_name = gtk_ml_car(macro_definition);
                GtkMl_S *macro_args = gtk_ml_cdr(macro_definition);
                GtkMl_S *macro_body = gtk_ml_cdr(args);

                GtkMl_S *macro = gtk_ml_new_macro(b->macro_ctx, NULL, macro_args, macro_body, gtk_ml_new_map(b->macro_ctx, NULL, NULL));
                if (!macro) {
                    return 0;
                }

                size_t len = macro_name->value.s_symbol.len;
                char *linkage_name = malloc(len + 1);
                memcpy(linkage_name, macro_name->value.s_symbol.ptr, len);
                linkage_name[len] = 0;

                GtkMl_BasicBlock *basic_block = gtk_ml_append_basic_block(b, linkage_name);

                if (!gtk_ml_compile_program_internal(b->macro_ctx, b, &basic_block, err, linkage_name, macro, 1, 0, 1, 0, 0, GTKML_PROG_MACRO)) {
                    free(linkage_name);
                    return 0;
                }

                if (!gtk_ml_build_push_extended_addr(b->macro_ctx, b, *start, err, gtk_ml_append_static(b, gtk_ml_new_string(b->macro_ctx, NULL, linkage_name, strlen(linkage_name))))) {
                    return 0;
                }
                if (!gtk_ml_build_push_extended_imm(b->macro_ctx, b, *start, err, gtk_ml_append_static(b, macro_name))) {
                    return 0;
                }
                if (!gtk_ml_build_define(b->macro_ctx, b, *start, err)) {
                    return 0;
                }
            } else if (function->value.s_symbol.len == strlen(symbol_define)
                    && memcmp(function->value.s_symbol.ptr, symbol_define, function->value.s_symbol.len) == 0) {
                GtkMl_S *intrinsic_definition = gtk_ml_car(args);
                GtkMl_S *intrinsic_name = gtk_ml_car(intrinsic_definition);
                if (!gtk_ml_hash_set_contains(&b->intr_fns, intrinsic_name) && !gtk_ml_hash_set_contains(&b->macro_fns, intrinsic_name)) {
                    GtkMl_S *intrinsic_args = gtk_ml_cdr(intrinsic_definition);
                    GtkMl_S *intrinsic_body = gtk_ml_cdr(args);

                    GtkMl_S *intrinsic = gtk_ml_new_lambda(b->intr_ctx, NULL, intrinsic_args, intrinsic_body, gtk_ml_new_map(b->intr_ctx, NULL, NULL));
                    if (!intrinsic) {
                        return 0;
                    }

                    size_t len = intrinsic_name->value.s_symbol.len;
                    char *linkage_name = malloc(len + 1);
                    memcpy(linkage_name, intrinsic_name->value.s_symbol.ptr, len);
                    linkage_name[len] = 0;

                    GtkMl_BasicBlock *basic_block = gtk_ml_append_basic_block(b, linkage_name);

                    if (!gtk_ml_compile_program_internal(b->intr_ctx, b, &basic_block, err, linkage_name, intrinsic, 1, 1, 0, 0, 0, GTKML_PROG_RUNTIME)) {
                        free(linkage_name);
                        return 0;
                    }

                    if (!gtk_ml_build_push_extended_addr(b->intr_ctx, b, *start, err, gtk_ml_append_static(b, gtk_ml_new_string(b->intr_ctx, NULL, linkage_name, strlen(linkage_name))))) {
                        return 0;
                    }
                    if (!gtk_ml_build_push_extended_imm(b->intr_ctx, b, *start, err, gtk_ml_append_static(b, intrinsic_name))) {
                        return 0;
                    }
                    if (!gtk_ml_build_define(b->intr_ctx, b, *start, err)) {
                        return 0;
                    }
                }
            }
        }
        next = gtk_ml_cdr(next);
    }
    
    return 1;
}

gboolean gtk_ml_compile(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_S **err, GtkMl_S *lambda) {
    const char *linkage_name = "_start";
    GtkMl_BasicBlock *basic_block = gtk_ml_append_basic_block(b, linkage_name);

    if (!compile_runtime_program(ctx, b, &basic_block, err, linkage_name, lambda, 0)) {
        return 0;
    }

    if (!gtk_ml_build_halt(ctx, b, basic_block, err)) {
        return 0;
    }
    
    return 1;
}

gboolean gtk_ml_compile_program(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_S **err, GtkMl_S *lambda) {
    collect_intrinsics(ctx, b, lambda);

    GtkMl_BasicBlock *intr_start = gtk_ml_append_basic_block(b, "_start");

    if (intr_start->len_exec == intr_start->cap_exec) {
        intr_start->cap_exec *= 2;
        intr_start->exec = realloc(intr_start->exec, sizeof(GtkMl_Instruction) * intr_start->cap_exec);
    }

    intr_start->exec[intr_start->len_exec].instr = 0;
    intr_start->exec[intr_start->len_exec].imm.cond = 0;
    intr_start->exec[intr_start->len_exec].imm.category = GTKML_EI_EXPORT;
    intr_start->exec[intr_start->len_exec].imm.imm = GTKML_EI_EXPORT_FLAG_NONE;
    ++intr_start->len_exec;

    if (intr_start->len_exec == intr_start->cap_exec) {
        intr_start->cap_exec *= 2;
        intr_start->exec = realloc(intr_start->exec, sizeof(GtkMl_Instruction) * intr_start->cap_exec);
    }

    char *name = malloc(strlen("_start") + 1);
    strcpy(name, "_start");
    GtkMl_S *program = gtk_ml_new_program(b->intr_ctx, &lambda->span, name, 0, lambda->value.s_lambda.args, lambda->value.s_lambda.body, lambda->value.s_lambda.capture, GTKML_PROG_INTRINSIC);
    intr_start->exec[intr_start->len_exec].imm64 = gtk_ml_append_static(b, program);
    ++intr_start->len_exec;

    if (!gtk_ml_compile_intrinsics(b, &intr_start, err, lambda)) {
        return 0;
    }

    if (!gtk_ml_build_halt(ctx, b, intr_start, err)) {
        return 0;
    }

    GtkMl_Program intrinsics;
    if (!gtk_ml_build_intrinsics(&intrinsics, err, b)) {
        return 0;
    }
    gtk_ml_load_program(b->intr_ctx, &intrinsics);
    gtk_ml_del_program(&intrinsics);
    GtkMl_S *intr_program = gtk_ml_get_export(b->intr_ctx, err, "_start");
    if (!intr_program) {
        return 0;
    }
    if (!gtk_ml_run_program(b->intr_ctx, err, intr_program, gtk_ml_new_nil(b->intr_ctx, NULL))) {
        return 0;
    }

    collect_macros(ctx, b, lambda);

    GtkMl_BasicBlock *macro_start = gtk_ml_append_basic_block(b, "_start");

    if (macro_start->len_exec == macro_start->cap_exec) {
        macro_start->cap_exec *= 2;
        macro_start->exec = realloc(macro_start->exec, sizeof(GtkMl_Instruction) * macro_start->cap_exec);
    }

    macro_start->exec[macro_start->len_exec].instr = 0;
    macro_start->exec[macro_start->len_exec].imm.cond = 0;
    macro_start->exec[macro_start->len_exec].imm.category = GTKML_EI_EXPORT;
    macro_start->exec[macro_start->len_exec].imm.imm = GTKML_EI_EXPORT_FLAG_NONE;
    ++macro_start->len_exec;

    if (macro_start->len_exec == macro_start->cap_exec) {
        macro_start->cap_exec *= 2;
        macro_start->exec = realloc(macro_start->exec, sizeof(GtkMl_Instruction) * macro_start->cap_exec);
    }

    name = malloc(strlen("_start") + 1);
    strcpy(name, "_start");
    program = gtk_ml_new_program(b->macro_ctx, &lambda->span, name, 0, lambda->value.s_lambda.args, lambda->value.s_lambda.body, lambda->value.s_lambda.capture, GTKML_PROG_MACRO);
    macro_start->exec[macro_start->len_exec].imm64 = gtk_ml_append_static(b, program);
    ++macro_start->len_exec;

    if (!gtk_ml_compile_macros(b, &macro_start, err, lambda)) {
        return 0;
    }

    if (!gtk_ml_build_halt(ctx, b, macro_start, err)) {
        return 0;
    }

    GtkMl_Program macros;
    if (!gtk_ml_build_macros(&macros, err, b)) {
        return 0;
    }
    gtk_ml_load_program(b->macro_ctx, &macros);
    gtk_ml_del_program(&macros);
    GtkMl_S *macro_program = gtk_ml_get_export(b->macro_ctx, err, "_start");
    if (!macro_program) {
        return 0;
    }
    if (!gtk_ml_run_program(b->macro_ctx, err, macro_program, gtk_ml_new_nil(b->macro_ctx, NULL))) {
        return 0;
    }

    return gtk_ml_compile(ctx, b, err, lambda);
}
