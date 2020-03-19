#include <stdlib.h>
#include <string.h>
#ifdef GTKML_ENABLE_GTK
#include <gtk/gtk.h>
#endif /* GTKML_ENABLE_GTK */
#include <math.h>
#define GTKML_INCLUDE_INTERNAL
#include "gtk-ml.h"
#include "gtk-ml-internal.h"

GTKML_PRIVATE gboolean compile_core_call(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, uint64_t function, GtkMl_SObj args, gboolean compile_first, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PRIVATE gboolean compile_runtime_program(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, const char *linkage_name, GtkMl_SObj stmt) GTKML_MUST_USE;

#ifdef GTKML_ENABLE_WEB
#include "libs/em_gles3/code-gen.c"
#include "cg-web.c"
#endif /* GTKML_ENABLE_WEB */

gboolean gtk_ml_builder_load(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_LOAD, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

#ifdef GTKML_ENABLE_GTK
gboolean gtk_ml_builder_application(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL
            || gtk_ml_cddr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_APPLICATION, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_new_window(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL
            || gtk_ml_cddr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_NEW_WINDOW, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}
#endif /* GTKML_ENABLE_GTK */

#ifdef GTKML_ENABLE_POSIX
gboolean gtk_ml_builder_dbg_run(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_DBG_RUN, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_dbg_cont(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_DBG_RUN, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_dbg_step(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_DBG_STEP, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_dbg_disasm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_DBG_DISASM, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_dbg_stack(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_DBG_STACK, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_dbg_backtrace(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_DBG_BACKTRACE, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}
#endif /* GTKML_ENABLE_POSIX */

gboolean gtk_ml_builder_setmetamap(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL || gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj map = gtk_ml_car(args);
    GtkMl_SObj metamap = gtk_ml_cdar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, &map, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, &metamap, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_setmm_imm(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_getmetamap(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj map = gtk_ml_car(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, &map, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_getmm_imm(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_getvar(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj map = gtk_ml_car(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, &map, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_getvar(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_assignvar(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL || gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj var = gtk_ml_car(args);
    GtkMl_SObj newvalue = gtk_ml_cdar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, &var, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, &newvalue, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_assignvar(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_error(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    (void) allow_intr;
    (void) allow_macro;
    (void) allow_runtime;
    (void) allow_macro_expansion;

    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_ERROR, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_dbg(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    (void) allow_intr;
    (void) allow_macro;
    (void) allow_runtime;
    (void) allow_macro_expansion;

    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_DBG, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_define(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj definition = gtk_ml_car(args);
    if (definition->kind == GTKML_S_SYMBOL) {
        GtkMl_SObj name = definition;
        GtkMl_SObj *value = &gtk_ml_cdar(args);
        if (!gtk_ml_compile_expression(ctx, b, basic_block, err, value, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
            return 0;
        }
        if (!gtk_ml_build_push_imm(ctx, b, *basic_block, err, gtk_ml_append_static_data(b, name))) {
            return 0;
        }
        return gtk_ml_build_define(ctx, b, *basic_block, err);
    } else if (definition->kind == GTKML_S_LIST) {
        GtkMl_SObj name = gtk_ml_car(definition);

        if (name->kind != GTKML_S_SYMBOL) {
            *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, definition->span.ptr != NULL, definition->span.line, definition->span.col, 0);
            return 0;
        }

        GtkMl_SObj params = gtk_ml_cdr(definition);
        GtkMl_SObj body = gtk_ml_cdr(args);

        GtkMl_SObj lambda = gtk_ml_new_lambda(ctx, &(*stmt)->span, params, body, gtk_ml_new_map(ctx, NULL, NULL));

        size_t len = name->value.s_symbol.len;
        char *linkage_name = malloc(len + 1);
        memcpy(linkage_name, name->value.s_symbol.ptr, len);
        linkage_name[len] = 0;
        GtkMl_BasicBlock *bb = gtk_ml_append_basic_block(b, linkage_name);
        if (!compile_runtime_program(ctx, b, &bb, err, linkage_name, lambda)) {
            return 0;
        }
        if (!gtk_ml_build_push_addr(ctx, b, *basic_block, err, gtk_ml_append_static_data(b, gtk_ml_new_string(ctx, NULL, linkage_name, strlen(linkage_name))))) {
            return 0;
        }
        if (!gtk_ml_build_push_imm(ctx, b, *basic_block, err, gtk_ml_append_static_data(b, name))) {
            return 0;
        }
        return gtk_ml_build_define(ctx, b, *basic_block, err);
    } else {
        *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, definition->span.ptr != NULL, definition->span.line, definition->span.col, 0);
        return 0;
    }
}


gboolean gtk_ml_builder_define_macro(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    (void) ctx;
    (void) b;
    (void) basic_block;
    (void) err;
    (void) stmt;
    (void) allow_intr;
    (void) allow_macro;
    (void) allow_runtime;
    (void) allow_macro_expansion;
    return gtk_ml_build_push_imm(ctx, b, *basic_block, err, gtk_ml_append_data(b, gtk_ml_value_nil()));
}

gboolean gtk_ml_builder_define_intrinsic(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    (void) ctx;
    (void) b;
    (void) basic_block;
    (void) err;
    (void) stmt;
    (void) allow_intr;
    (void) allow_macro;
    (void) allow_runtime;
    (void) allow_macro_expansion;
    return gtk_ml_build_push_imm(ctx, b, *basic_block, err, gtk_ml_append_data(b, gtk_ml_value_nil()));
}

gboolean gtk_ml_builder_intr_apply(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    (void) allow_intr;
    (void) allow_macro;
    (void) allow_runtime;
    (void) allow_macro_expansion;

    GtkMl_Program *program = gtk_ml_build_intr_apply(ctx, err, b, b->intrinsics, b->len_intrinsics);
    if (!program) {
        return 0;
    }

    GtkMl_Context *tmp_ctx = gtk_ml_new_context_with_gc_builder(gtk_ml_gc_copy(ctx->gc), ctx->default_builder);
    tmp_ctx->dbg_done = 1;
    gtk_ml_load_program(tmp_ctx, program);
    GtkMl_SObj intr_program = gtk_ml_get_export(tmp_ctx, err, program->start);
    if (!intr_program) {
        return 0;
    }
    if (!gtk_ml_execute(tmp_ctx, err, gtk_ml_value_sobject(intr_program), GTKML_NO_ARGS, 0)) {
        return 0;
    }

    GtkMl_SObj *function = &gtk_ml_cdar(*stmt);

    if ((*function)->kind == GTKML_S_SYMBOL) {
        size_t len = (*function)->value.s_symbol.len;
        char *linkage_name = malloc(len + 1);
        memcpy(linkage_name, (*function)->value.s_symbol.ptr, len);
        linkage_name[len] = 0;

        GtkMl_SObj _err = NULL;
        GtkMl_SObj program = gtk_ml_get_export(tmp_ctx, &_err, linkage_name);
        free(linkage_name);
        if (program && program->value.s_program.kind == GTKML_PROG_INTRINSIC) {
            GtkMl_SObj _args = gtk_ml_cddr(*stmt);

            size_t cap = 8;
            size_t n_args = 0;
            GtkMl_TaggedValue *args = malloc(sizeof(GtkMl_TaggedValue) * cap);

            while (_args->kind != GTKML_S_NIL) {
                args[n_args++] = gtk_ml_value_sobject(gtk_ml_car(_args));
            }

            GtkMl_BasicBlock *bb = *basic_block;
            GtkMl_SObj bb_ld = gtk_ml_new_lightdata(b->intr_ctx, NULL, bb);
            GtkMl_SObj bb_var = gtk_ml_new_var(b->intr_ctx, NULL, bb_ld);
            gtk_ml_bind(b->intr_ctx, gtk_ml_new_symbol(ctx, NULL, 0, "CTX", strlen("CTX")), gtk_ml_value_userdata(ctx));
            gtk_ml_bind(b->intr_ctx, gtk_ml_new_symbol(ctx, NULL, 0, "CODE-BUILDER", strlen("CODE-BUILDER")), gtk_ml_value_userdata(b));
            gtk_ml_bind(tmp_ctx, gtk_ml_new_symbol(ctx, NULL, 0, "BASIC-BLOCK", strlen("BASIC-BLOCK")), gtk_ml_value_sobject(bb_var));
            if (!gtk_ml_execute(tmp_ctx, err, gtk_ml_value_sobject(program), args, n_args)) {
                return 0;
            }
            *basic_block = bb_var->value.s_var.expr->value.s_lightdata.userdata;
        }
    }

    gtk_ml_del_context(tmp_ctx);

    return gtk_ml_build_push_imm(ctx, b, *basic_block, err, gtk_ml_append_data(b, gtk_ml_value_nil()));
}

gboolean gtk_ml_builder_compile_expr(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    (void) b;
    (void) basic_block;
    (void) allow_intr;
    (void) allow_macro;
    (void) allow_runtime;
    (void) allow_macro_expansion;
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL
            || gtk_ml_cddr(args)->kind == GTKML_S_NIL
            || gtk_ml_cdddr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_COMPILE_EXPR, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_emit_bytecode(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    (void) b;
    (void) basic_block;
    (void) allow_intr;
    (void) allow_macro;
    (void) allow_runtime;
    (void) allow_macro_expansion;
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL
            || gtk_ml_cddr(args)->kind == GTKML_S_NIL
            || gtk_ml_cdddr(args)->kind == GTKML_S_NIL
            || gtk_ml_cddddr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_EMIT_BYTECODE, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_bind_symbol(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    (void) b;
    (void) basic_block;
    (void) allow_intr;
    (void) allow_macro;
    (void) allow_runtime;
    (void) allow_macro_expansion;
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL
            || gtk_ml_cddr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_BIND_SYMBOL, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_export_symbol(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    (void) b;
    (void) basic_block;
    (void) allow_intr;
    (void) allow_macro;
    (void) allow_runtime;
    (void) allow_macro_expansion;
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL
            || gtk_ml_cddr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_EXPORT_SYMBOL, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_append_basic_block(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    (void) b;
    (void) basic_block;
    (void) allow_intr;
    (void) allow_macro;
    (void) allow_runtime;
    (void) allow_macro_expansion;
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_APPEND_BASIC_BLOCK, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_global_counter(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    (void) b;
    (void) basic_block;
    (void) allow_intr;
    (void) allow_macro;
    (void) allow_runtime;
    (void) allow_macro_expansion;
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_GLOBAL_COUNTER, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_basic_block_name(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    (void) b;
    (void) basic_block;
    (void) allow_intr;
    (void) allow_macro;
    (void) allow_runtime;
    (void) allow_macro_expansion;
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_BASIC_BLOCK_NAME, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_string_to_symbol(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    (void) b;
    (void) basic_block;
    (void) allow_intr;
    (void) allow_macro;
    (void) allow_runtime;
    (void) allow_macro_expansion;
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_STRING_TO_SYMBOL, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_do(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        GtkMl_SObj nil = gtk_ml_new_nil(ctx, &(*stmt)->span);
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

gboolean gtk_ml_builder_let(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    return gtk_ml_builder_let_star(ctx, b, basic_block, err, stmt, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_let_star(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj bindings = gtk_ml_car(args);
    if (bindings->kind != GTKML_S_ARRAY) {
        *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    size_t len = gtk_ml_array_trie_len(&bindings->value.s_array.array);
    if (len & 1) {
        *err = gtk_ml_error(ctx, "let-error", GTKML_ERR_LET_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    gtk_ml_builder_enter(ctx, b, 1);
    if (!gtk_ml_build_enter(ctx, b, *basic_block, err)) {
        return 0;
    }

    for (size_t i = 0; i < len; i += 2) {
        GtkMl_SObj key = gtk_ml_array_trie_get(&bindings->value.s_array.array, i).value.sobj;
        GtkMl_SObj value = gtk_ml_array_trie_get(&bindings->value.s_array.array, i + 1).value.sobj;
        if (!gtk_ml_compile_expression(ctx, b, basic_block, err, &value, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
            return 0;
        }
        int64_t offset = gtk_ml_builder_alloca(ctx, b);
        gtk_ml_builder_bind(ctx, b, key, offset);
        if (!gtk_ml_build_bind(ctx, b, *basic_block, err)) {
            return 0;
        }
    }

    GtkMl_SObj body = gtk_ml_cdr(args);

    if (body->kind == GTKML_S_NIL) {
        GtkMl_SObj nil = gtk_ml_new_nil(ctx, &(*stmt)->span);
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

    gtk_ml_builder_leave(ctx, b);
    if (!gtk_ml_build_leave(ctx, b, *basic_block, err)) {
        return 0;
    }

    return 1;
}

gboolean gtk_ml_builder_lambda(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj lambda_args = gtk_ml_car(args);
    GtkMl_SObj lambda_body = gtk_ml_cdr(args);

    GtkMl_SObj lambda = gtk_ml_new_lambda(ctx, &(*stmt)->span, lambda_args, lambda_body, gtk_ml_new_map(ctx, NULL, NULL));
    if (!lambda) {
        return 0;
    }

    return gtk_ml_compile_expression(ctx, b, basic_block, err, &lambda, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_macro(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj macro_args = gtk_ml_car(args);
    GtkMl_SObj macro_body = gtk_ml_cdr(args);

    GtkMl_SObj macro = gtk_ml_new_macro(ctx, &(*stmt)->span, macro_args, macro_body, gtk_ml_new_map(ctx, NULL, NULL));
    if (!macro) {
        return 0;
    }

    return gtk_ml_compile_expression(ctx, b, basic_block, err, &macro, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_cond(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    GtkMl_SObj _args = args;

    if (args->kind == GTKML_S_NIL) {
        return gtk_ml_build_push_imm(ctx, b, *basic_block, err, gtk_ml_append_data(b, gtk_ml_value_nil()));
    }

    size_t n = 0;
    while (args->kind != GTKML_S_NIL) {
        if (gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
            return 0;
        }

        ++n;

        GtkMl_SObj cond = gtk_ml_car(args);
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

    if (bb->len_text == bb->cap_text) {
        bb->cap_text *= 2;
        bb->text = realloc(bb->text, sizeof(GtkMl_Instruction) * bb->cap_text);
    }


    char *name = malloc(strlen(linkage_name) + 1);
    strcpy(name, linkage_name);
    GtkMl_SObj addr = gtk_ml_new_address(ctx, NULL, name, 0);
    bb->text[bb->len_text].cond = GTKML_F_NONE;
    bb->text[bb->len_text].category = GTKML_I_EXPORT_LOCAL;
    bb->text[bb->len_text].opcode = 0;
    bb->text[bb->len_text].data = gtk_ml_append_static_data(b, addr);
    ++bb->len_text;

    branches[n] = bb;

    uint32_t i = 0;
    args = _args;
    while (args->kind != GTKML_S_NIL) {
        char *linkage_name = malloc(strlen("cond$$") + 32);
        snprintf(linkage_name, strlen("cond$$") + 32, "cond$%u$%u", cond_number, i);
        GtkMl_BasicBlock *bb = gtk_ml_append_basic_block(b, linkage_name);

        if (bb->len_text == bb->cap_text) {
            bb->cap_text *= 2;
            bb->text = realloc(bb->text, sizeof(GtkMl_Instruction) * bb->cap_text);
        }

        char *name = malloc(strlen(linkage_name) + 1);
        strcpy(name, linkage_name);
        GtkMl_SObj addr = gtk_ml_new_address(ctx, NULL, name, 0);
        bb->text[bb->len_text].cond = GTKML_F_NONE;
        bb->text[bb->len_text].category = GTKML_I_EXPORT_LOCAL;
        bb->text[bb->len_text].opcode = 0;
        bb->text[bb->len_text].data = gtk_ml_append_static_data(b, addr);
        ++bb->len_text;

        branches[i] = bb;

        GtkMl_SObj cond = gtk_ml_car(args);
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
        GtkMl_SObj *cond = &gtk_ml_car(args);

        char *branch_name = malloc(strlen("cond$$") + 32);
        snprintf(branch_name, strlen("cond$$") + 32, "cond$%u$%u", cond_number, i);

        if ((*cond)->kind == GTKML_S_KEYWORD
                && (*cond)->value.s_keyword.len == 4
                && strncmp((*cond)->value.s_keyword.ptr, "else", 4) == 0) {
            gtk_ml_builder_clear_cond(b);
            if (!gtk_ml_build_branch_absolute(ctx, b, *basic_block, err, gtk_ml_append_static_data(b, gtk_ml_new_string(ctx, NULL, branch_name, strlen(branch_name))))) {
                return 0;
            }
            has_else = 1;
            break;
        }

        if (!gtk_ml_compile_cond_expression(ctx, b, basic_block, err, cond, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
            return 0;
        }
        gtk_ml_builder_set_cond(b, GTKML_F_EQUAL);
        if (!gtk_ml_build_branch_absolute(ctx, b, *basic_block, err, gtk_ml_append_static_data(b, gtk_ml_new_string(ctx, NULL, branch_name, strlen(branch_name))))) {
            return 0;
        }

        ++i;

        args = gtk_ml_cddr(args);
    }

    if (!has_else) {
        char *end_name = malloc(strlen("cond$$end") + 16);
        snprintf(end_name, strlen("cond$$end") + 16, "cond$%u$end", cond_number);
        gtk_ml_builder_clear_cond(b);
        if (!gtk_ml_build_push_imm(ctx, b, *basic_block, err, gtk_ml_append_data(b, gtk_ml_value_nil()))) {
            return 0;
        }
        if (!gtk_ml_build_branch_absolute(ctx, b, *basic_block, err, gtk_ml_append_static_data(b, gtk_ml_new_string(ctx, NULL, end_name, strlen(end_name))))) {
            return 0;
        }
    }

    i = 0;
    args = _args;
    while (args->kind != GTKML_S_NIL) {
        GtkMl_SObj cond = gtk_ml_car(args);
        GtkMl_SObj *body = &gtk_ml_cdar(args);

        if (!gtk_ml_compile_expression(ctx, b, &branches[i], err, body, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
            return 0;
        }

        char *end_name = malloc(strlen("cond$$end") + 16);
        snprintf(end_name, strlen("cond$$end") + 16, "cond$%u$end", cond_number);
        gtk_ml_builder_clear_cond(b);
        if (!gtk_ml_build_branch_absolute(ctx, b, branches[i], err, gtk_ml_append_static_data(b, gtk_ml_new_string(ctx, NULL, end_name, strlen(end_name))))) {
            return 0;
        }

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

gboolean gtk_ml_builder_while(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    uint32_t while_number = gtk_ml_builder_get_and_inc(ctx, b)->value.s_int.value;

    char *linkage_name = malloc(strlen("while$$cond") + 16);
    snprintf(linkage_name, strlen("while$$cond") + 16, "while$%u$cond", while_number);
    GtkMl_BasicBlock *cond = gtk_ml_append_basic_block(b, linkage_name);

    if (cond->len_text == cond->cap_text) {
        cond->cap_text *= 2;
        cond->text = realloc(cond->text, sizeof(GtkMl_Instruction) * cond->cap_text);
    }

    char *name = malloc(strlen(linkage_name) + 1);
    strcpy(name, linkage_name);
    GtkMl_SObj cond_addr = gtk_ml_new_address(ctx, NULL, name, 0);
    cond->text[cond->len_text].cond = GTKML_F_NONE;
    cond->text[cond->len_text].category = GTKML_I_EXPORT_LOCAL;
    cond->text[cond->len_text].opcode = 0;
    cond->text[cond->len_text].data = gtk_ml_append_static_data(b, cond_addr);
    ++cond->len_text;

    linkage_name = malloc(strlen("while$$end") + 16);
    snprintf(linkage_name, strlen("while$$end") + 16, "while$%u$end", while_number);
    GtkMl_BasicBlock *end = gtk_ml_append_basic_block(b, linkage_name);

    if (end->len_text == end->cap_text) {
        end->cap_text *= 2;
        end->text = realloc(end->text, sizeof(GtkMl_Instruction) * end->cap_text);
    }

    name = malloc(strlen(linkage_name) + 1);
    strcpy(name, linkage_name);
    GtkMl_SObj end_addr = gtk_ml_new_address(ctx, NULL, name, 0);
    end->text[end->len_text].cond = GTKML_F_NONE;
    end->text[end->len_text].category = GTKML_I_EXPORT_LOCAL;
    end->text[end->len_text].opcode = 0;
    end->text[end->len_text].data = gtk_ml_append_static_data(b, end_addr);
    ++end->len_text;

    linkage_name = malloc(strlen("while$$") + 32);
    snprintf(linkage_name, strlen("while$$") + 32, "while$%u$body", while_number);
    GtkMl_BasicBlock *body = gtk_ml_append_basic_block(b, linkage_name);

    if (body->len_text == body->cap_text) {
        body->cap_text *= 2;
        body->text = realloc(body->text, sizeof(GtkMl_Instruction) * body->cap_text);
    }

    name = malloc(strlen(linkage_name) + 1);
    strcpy(name, linkage_name);
    GtkMl_SObj body_addr = gtk_ml_new_address(ctx, NULL, name, 0);
    body->text[body->len_text].cond = GTKML_F_NONE;
    body->text[body->len_text].category = GTKML_I_EXPORT_LOCAL;
    body->text[body->len_text].opcode = 0;
    body->text[body->len_text].data = gtk_ml_append_static_data(b, body_addr);
    ++body->len_text;

    char *body_name = malloc(strlen("while$$body") + 16);
    snprintf(body_name, strlen("while$$body") + 16, "while$%u$body", while_number);
    char *end_name = malloc(strlen("while$$end") + 16);
    snprintf(end_name, strlen("while$$end") + 16, "while$%u$end", while_number);

    if (!gtk_ml_compile_cond_expression(ctx, b, &cond, err, &gtk_ml_car(args), allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    // if true: jump to body
    gtk_ml_builder_set_cond(b, GTKML_F_EQUAL);
    if (!gtk_ml_build_branch_absolute(ctx, b, cond, err, gtk_ml_append_static_data(b, gtk_ml_new_string(ctx, NULL, body_name, strlen(body_name))))) {
        return 0;
    }
    // else: jump to end
    if (!gtk_ml_build_branch_absolute(ctx, b, cond, err, gtk_ml_append_static_data(b, gtk_ml_new_string(ctx, NULL, end_name, strlen(end_name))))) {
        return 0;
    }

    args = gtk_ml_cdr(args);
    while (args->kind != GTKML_S_NIL) {
        char *cond_name = malloc(strlen("while$$cond") + 16);
        snprintf(cond_name, strlen("while$$cond") + 16, "while$%u$cond", while_number);

        if (!gtk_ml_compile_expression(ctx, b, &body, err, &gtk_ml_car(args), allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
            return 0;
        }
        if (!gtk_ml_build_branch_absolute(ctx, b, body, err, gtk_ml_append_static_data(b, gtk_ml_new_string(ctx, NULL, cond_name, strlen(cond_name))))) {
            return 0;
        }
        args = gtk_ml_cdr(args);
    }

    *basic_block = end;

    return gtk_ml_build_push_imm(ctx, b, *basic_block, err, gtk_ml_append_data(b, gtk_ml_value_nil()));
}

gboolean gtk_ml_builder_len(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj *container = &gtk_ml_car(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, container, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_len(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_car(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj *list = &gtk_ml_car(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, list, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_car(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_cdr(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj *list = &gtk_ml_car(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, list, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_cdr(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_cons(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (gtk_ml_list_len(args) != 2) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj *car = &gtk_ml_car(args);
    GtkMl_SObj *cdr = &gtk_ml_cdar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, car, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, cdr, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_cons(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_map(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if ((gtk_ml_list_len(args) & 1) == 0) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    int64_t n = 0;
    while (gtk_ml_car(args)->kind != GTKML_S_NIL) {
        GtkMl_SObj *key = &gtk_ml_car(args);
        GtkMl_SObj *value = &gtk_ml_cdar(args);

        if (!gtk_ml_compile_expression(ctx, b, basic_block, err, key, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
            return 0;
        }
        if (!gtk_ml_compile_expression(ctx, b, basic_block, err, value, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
            return 0;
        }

        ++n;
    }

    if (!gtk_ml_build_push_imm(ctx, b, *basic_block, err, gtk_ml_append_data(b, gtk_ml_value_int(n)))) {
        return 0;
    }
    return gtk_ml_build_map(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_set(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if ((gtk_ml_list_len(args) & 1) == 0) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    int64_t n = 0;
    while (gtk_ml_car(args)->kind != GTKML_S_NIL) {
        GtkMl_SObj *key = &gtk_ml_car(args);

        if (!gtk_ml_compile_expression(ctx, b, basic_block, err, key, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
            return 0;
        }

        ++n;
    }

    if (!gtk_ml_build_push_imm(ctx, b, *basic_block, err, gtk_ml_append_data(b, gtk_ml_value_int(n)))) {
        return 0;
    }
    return gtk_ml_build_set(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_array(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if ((gtk_ml_list_len(args) & 1) == 0) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    int64_t n = 0;
    while (gtk_ml_car(args)->kind != GTKML_S_NIL) {
        GtkMl_SObj *value = &gtk_ml_car(args);

        if (!gtk_ml_compile_expression(ctx, b, basic_block, err, value, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
            return 0;
        }

        ++n;
    }

    if (!gtk_ml_build_push_imm(ctx, b, *basic_block, err, gtk_ml_append_data(b, gtk_ml_value_int(n)))) {
        return 0;
    }
    return gtk_ml_build_array(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_typeof(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj *value = &gtk_ml_car(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, value, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_typeof(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_to_sobject(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj *value = &gtk_ml_car(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, value, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_to_sobject(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_to_prim(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj *value = &gtk_ml_car(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, value, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_to_prim(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_index(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj *array = &gtk_ml_car(args);
    GtkMl_SObj *index = &gtk_ml_cdar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, index, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, array, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_index(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_push(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj *array = &gtk_ml_car(args);
    GtkMl_SObj *value = &gtk_ml_cdar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, value, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, array, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_array_push(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_pop(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj *array = &gtk_ml_car(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, array, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_array_pop(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_concat(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj *a1 = &gtk_ml_car(args);
    GtkMl_SObj *a2 = &gtk_ml_cdar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, a2, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, a1, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_array_concat(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_map_get(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj *map = &gtk_ml_car(args);
    GtkMl_SObj *key = &gtk_ml_cdar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, key, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, map, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_map_get(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_map_insert(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj *map = &gtk_ml_car(args);
    GtkMl_SObj *key = &gtk_ml_cdar(args);
    GtkMl_SObj *val = &gtk_ml_cddar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, val, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, key, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, map, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_map_insert(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_map_rawget(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj *map = &gtk_ml_car(args);
    GtkMl_SObj *key = &gtk_ml_cdar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, key, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, map, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_map_rawget(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_map_rawinsert(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj *map = &gtk_ml_car(args);
    GtkMl_SObj *key = &gtk_ml_cdar(args);
    GtkMl_SObj *val = &gtk_ml_cddar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, val, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, key, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, map, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_map_rawinsert(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_map_delete(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj *map = &gtk_ml_car(args);
    GtkMl_SObj *key = &gtk_ml_cdar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, key, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, map, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_map_delete(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_map_concat(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj *m1 = &gtk_ml_car(args);
    GtkMl_SObj *m2 = &gtk_ml_cdar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, m2, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, m1, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_map_concat(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_set_contains(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj *set = &gtk_ml_car(args);
    GtkMl_SObj *key = &gtk_ml_cdar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, key, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, set, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_set_contains(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_set_insert(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj *set = &gtk_ml_car(args);
    GtkMl_SObj *key = &gtk_ml_cdar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, key, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, set, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_set_insert(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_set_delete(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj *set = &gtk_ml_car(args);
    GtkMl_SObj *key = &gtk_ml_cdar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, key, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, set, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_set_delete(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_set_concat(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj *m1 = &gtk_ml_car(args);
    GtkMl_SObj *m2 = &gtk_ml_cdar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, m2, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, m1, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_set_concat(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_add(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj *lhs = &gtk_ml_car(args);
    GtkMl_SObj *rhs = &gtk_ml_cdar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, rhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, lhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_add(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_sub(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind != GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        GtkMl_SObj *value = &gtk_ml_car(args);
        GtkMl_SObj zero = gtk_ml_new_int(ctx, &(*stmt)->span, 0);

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

    GtkMl_SObj *lhs = &gtk_ml_car(args);
    GtkMl_SObj *rhs = &gtk_ml_cdar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, rhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, lhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_sub(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_mul(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj *lhs = &gtk_ml_car(args);
    GtkMl_SObj *rhs = &gtk_ml_cdar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, rhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, lhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_mul(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_div(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj *lhs = &gtk_ml_car(args);
    GtkMl_SObj *rhs = &gtk_ml_cdar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, rhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, lhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_div(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_mod(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj *lhs = &gtk_ml_car(args);
    GtkMl_SObj *rhs = &gtk_ml_cdar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, rhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, lhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_mod(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_bitnot(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj *value = &gtk_ml_car(args);
    GtkMl_SObj ff = gtk_ml_new_int(ctx, &(*stmt)->span, 0xfffffffffffffffful);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, &ff, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, value, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_bitxor(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_bitand(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj *lhs = &gtk_ml_car(args);
    GtkMl_SObj *rhs = &gtk_ml_cdar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, rhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, lhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_bitand(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_bitor(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj *lhs = &gtk_ml_car(args);
    GtkMl_SObj *rhs = &gtk_ml_cdar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, rhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, lhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_bitor(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_bitxor(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj *lhs = &gtk_ml_car(args);
    GtkMl_SObj *rhs = &gtk_ml_cdar(args);

    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, rhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!gtk_ml_compile_expression(ctx, b, basic_block, err, lhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_bitxor(ctx, b, *basic_block, err);
}

gboolean gtk_ml_builder_cmp(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL
            || gtk_ml_cddr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj cmp = gtk_ml_car(args);
    GtkMl_SObj *lhs = &gtk_ml_cdar(args);
    GtkMl_SObj *rhs = &gtk_ml_cddar(args);

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
    return gtk_ml_build_cmp(ctx, b, *basic_block, err, gtk_ml_append_data(b, gtk_ml_value_int(cmp->value.s_int.value)));
}

gboolean gtk_ml_builder_var(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj var = gtk_ml_new_var(ctx, &(*stmt)->span, gtk_ml_car(args));
    if (!var) {
        return 0;
    }

    return gtk_ml_compile_expression(ctx, b, basic_block, err, &var, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_vararg(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj vararg = gtk_ml_new_vararg(ctx, &(*stmt)->span, gtk_ml_car(args));
    if (!vararg) {
        return 0;
    }

    return gtk_ml_compile_expression(ctx, b, basic_block, err, &vararg, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_quote(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj quote = gtk_ml_new_quote(ctx, &(*stmt)->span, gtk_ml_car(args));
    if (!quote) {
        return 0;
    }

    return gtk_ml_compile_expression(ctx, b, basic_block, err, &quote, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_quasiquote(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj quasiquote = gtk_ml_new_quasiquote(ctx, &(*stmt)->span, gtk_ml_car(args));
    if (!quasiquote) {
        return 0;
    }

    return gtk_ml_compile_expression(ctx, b, basic_block, err, &quasiquote, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_unquote(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_SObj unquote = gtk_ml_new_unquote(ctx, &(*stmt)->span, gtk_ml_car(args));
    if (!unquote) {
        return 0;
    }

    return gtk_ml_compile_expression(ctx, b, basic_block, err, &unquote, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_allocate(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (gtk_ml_list_len(args) != 1) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_ALLOCATE, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_to_cstr(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (gtk_ml_list_len(args) != 1) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_TO_CSTR, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_to_buffer(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (gtk_ml_list_len(args) != 2) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_TO_BUFFER, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_to_array(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (gtk_ml_list_len(args) != 4) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_TO_ARRAY, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_to_string(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);

    if (gtk_ml_list_len(args) != 2) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_TO_STRING, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_compile_cond_expression(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    switch ((*stmt)->kind) {
    case GTKML_S_FALSE:
    case GTKML_S_TRUE:
        return gtk_ml_build_setf_imm(ctx, b, *basic_block, err, gtk_ml_append_static_data(b, *stmt));
    case GTKML_S_NIL:
        return gtk_ml_build_setf_imm(ctx, b, *basic_block, err, gtk_ml_append_data(b, gtk_ml_value_false()));
    case GTKML_S_INT:
    case GTKML_S_FLOAT:
    case GTKML_S_CHAR:
    case GTKML_S_KEYWORD:
    case GTKML_S_PROGRAM:
    case GTKML_S_ADDRESS:
    case GTKML_S_LIGHTDATA:
    case GTKML_S_USERDATA:
    case GTKML_S_FFI:
    case GTKML_S_LAMBDA: 
    case GTKML_S_MACRO:
    case GTKML_S_MAP:
    case GTKML_S_SET:
    case GTKML_S_ARRAY: 
    case GTKML_S_VAR:
        return gtk_ml_build_setf_imm(ctx, b, *basic_block, err, gtk_ml_append_data(b, gtk_ml_value_true()));
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
            return gtk_ml_build_setf_imm(ctx, b, *basic_block, err, gtk_ml_append_data(b, gtk_ml_value_true()));
        } else {
            return gtk_ml_compile_cond_expression(ctx, b, basic_block, err, &(*stmt)->value.s_quote.expr, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
        }
    case GTKML_S_QUASIQUOTE:
        if ((*stmt)->value.s_quote.expr->kind == GTKML_S_SYMBOL || (*stmt)->value.s_quote.expr->kind == GTKML_S_LIST) {
            return gtk_ml_build_setf_imm(ctx, b, *basic_block, err, gtk_ml_append_data(b, gtk_ml_value_true()));
        } else {
            return gtk_ml_compile_cond_expression(ctx, b, basic_block, err, &(*stmt)->value.s_quote.expr, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
        }
    case GTKML_S_UNQUOTE:
        *err = gtk_ml_error(ctx, "unquote-error", GTKML_ERR_UNQUOTE_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }
}

gboolean compile_core_call(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, uint64_t function, GtkMl_SObj args, gboolean compile_first, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    int64_t n = 0;
    if (!compile_first) {
        if (!gtk_ml_build_push_imm(ctx, b, *basic_block, err, gtk_ml_append_static_data(b, gtk_ml_car(args)))) {
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

    if (!gtk_ml_build_push_imm(ctx, b, *basic_block, err, gtk_ml_append_data(b, gtk_ml_value_int(n)))) {
        return 0;
    }
    if (!gtk_ml_build_call_core(ctx, b, *basic_block, err, gtk_ml_append_data(b, gtk_ml_value_int(function)))) {
        return 0;
    }

    return 1;
}

struct CompileData {
    GtkMl_Context *ctx;
    GtkMl_Builder *b;
    GtkMl_BasicBlock **basic_block;
    GtkMl_SObj *err;
    GtkMl_SObj stmt;
    gboolean allow_intr;
    gboolean allow_macro;
    gboolean allow_runtime;
    gboolean allow_macro_expansion;
    gboolean *unwrapped;
    gboolean result;
};

GTKML_MUST_USE
GTKML_PRIVATE GtkMl_VisitResult compile_quasi_map(GtkMl_HashTrie *ht, GtkMl_TaggedValue key, GtkMl_TaggedValue value, GtkMl_TaggedValue _data) {
    (void) ht;

    struct CompileData *data = _data.value.userdata;
    if (!gtk_ml_compile_quasi_expression(data->ctx, data->b, data->basic_block, data->err, key.value.sobj, data->allow_intr, data->allow_macro, data->allow_runtime, data->allow_macro_expansion, data->unwrapped)) {
        data->result = 0;
        return GTKML_VISIT_BREAK;
    }
    if (!gtk_ml_compile_quasi_expression(data->ctx, data->b, data->basic_block, data->err, value.value.sobj, data->allow_intr, data->allow_macro, data->allow_runtime, data->allow_macro_expansion, data->unwrapped)) {
        data->result = 0;
        return GTKML_VISIT_BREAK;
    }
    return GTKML_VISIT_RECURSE;
}

GTKML_MUST_USE
GTKML_PRIVATE GtkMl_VisitResult compile_quasi_set(GtkMl_HashSet *hs, GtkMl_TaggedValue key, GtkMl_TaggedValue _data) {
    (void) hs;

    struct CompileData *data = _data.value.userdata;
    if (!gtk_ml_compile_quasi_expression(data->ctx, data->b, data->basic_block, data->err, key.value.sobj, data->allow_intr, data->allow_macro, data->allow_runtime, data->allow_macro_expansion, data->unwrapped)) {
        data->result = 0;
        return GTKML_VISIT_BREAK;
    }
    return GTKML_VISIT_RECURSE;
}

GTKML_MUST_USE
GTKML_PRIVATE GtkMl_VisitResult compile_quasi_array(GtkMl_Array *array, size_t idx, GtkMl_TaggedValue value, GtkMl_TaggedValue _data) {
    (void) array;
    (void) idx;

    struct CompileData *data = _data.value.userdata;
    if (!gtk_ml_compile_quasi_expression(data->ctx, data->b, data->basic_block, data->err, value.value.sobj, data->allow_intr, data->allow_macro, data->allow_runtime, data->allow_macro_expansion, data->unwrapped)) {
        data->result = 0;
        return GTKML_VISIT_BREAK;
    }
    return GTKML_VISIT_RECURSE;
}

GTKML_MUST_USE
GTKML_PRIVATE GtkMl_VisitResult compile_map(GtkMl_HashTrie *ht, GtkMl_TaggedValue key_ptr, GtkMl_TaggedValue value_ptr, GtkMl_TaggedValue _data) {
    (void) ht;
    GtkMl_SObj key = key_ptr.value.sobj;
    GtkMl_SObj value = value_ptr.value.sobj;

    struct CompileData *data = _data.value.userdata;
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

GTKML_MUST_USE
GTKML_PRIVATE GtkMl_VisitResult compile_set(GtkMl_HashSet *hs, GtkMl_TaggedValue key_ptr, GtkMl_TaggedValue _data) {
    (void) hs;
    GtkMl_SObj key = key_ptr.value.sobj;

    struct CompileData *data = _data.value.userdata;
    if (!gtk_ml_compile_expression(data->ctx, data->b, data->basic_block, data->err, &key, data->allow_intr, data->allow_macro, data->allow_runtime, data->allow_macro_expansion)) {
        data->result = 0;
        return GTKML_VISIT_BREAK;
    }
    return GTKML_VISIT_RECURSE;
}

GTKML_MUST_USE
GTKML_PRIVATE GtkMl_VisitResult compile_array(GtkMl_Array *array, size_t idx, GtkMl_TaggedValue value, GtkMl_TaggedValue _data) {
    (void) array;
    (void) idx;

    struct CompileData *data = _data.value.userdata;
    if (!gtk_ml_compile_expression(data->ctx, data->b, data->basic_block, data->err, &value.value.sobj, data->allow_intr, data->allow_macro, data->allow_runtime, data->allow_macro_expansion)) {
        data->result = 0;
        return GTKML_VISIT_BREAK;
    }
    return GTKML_VISIT_RECURSE;
}

gboolean compile_runtime_program(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, const char *linkage_name, GtkMl_SObj lambda) {
    return gtk_ml_compile_program_internal(ctx, b, basic_block, err, linkage_name, lambda, 0, 0, 1, 1, GTKML_PROG_RUNTIME);
}

gboolean gtk_ml_compile_quasi_expression(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion, gboolean *unwrapped) {
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
    case GTKML_S_FFI:
    case GTKML_S_SYMBOL:
    case GTKML_S_LAMBDA:
    case GTKML_S_MACRO:
    case GTKML_S_VAR:
    case GTKML_S_VARARG:
    case GTKML_S_QUOTE:
    case GTKML_S_QUASIQUOTE:
        return gtk_ml_build_push_imm(ctx, b, *basic_block, err, gtk_ml_append_static_data(b, stmt));
    case GTKML_S_MAP: {
        int64_t n = gtk_ml_hash_trie_len(&stmt->value.s_map.map);
        struct CompileData data = { ctx, b, basic_block, err, stmt, allow_intr, allow_macro, allow_runtime, allow_macro_expansion, unwrapped, 1 }; 
        gtk_ml_hash_trie_foreach(&stmt->value.s_map.map, compile_quasi_map, gtk_ml_value_userdata(&data));
        if (!data.result) {
            return 0;
        }
        return gtk_ml_build_map_imm(ctx, b, *basic_block, err, gtk_ml_append_data(b, gtk_ml_value_int(n)));
    }
    case GTKML_S_SET: {
        int64_t n = gtk_ml_hash_set_len(&stmt->value.s_set.set);
        struct CompileData data = { ctx, b, basic_block, err, stmt, allow_intr, allow_macro, allow_runtime, allow_macro_expansion, unwrapped, 1 }; 
        gtk_ml_hash_set_foreach(&stmt->value.s_set.set, compile_quasi_set, gtk_ml_value_userdata(&data));
        if (!data.result) {
            return 0;
        }
        return gtk_ml_build_set_imm(ctx, b, *basic_block, err, gtk_ml_append_data(b, gtk_ml_value_int(n)));
    }
    case GTKML_S_ARRAY: {
        if (gtk_ml_array_trie_is_string(&stmt->value.s_array.array)) {
            return gtk_ml_build_push_imm(ctx, b, *basic_block, err, gtk_ml_append_static_data(b, stmt));
        } else {
            int64_t n = gtk_ml_array_trie_len(&stmt->value.s_array.array);
            struct CompileData data = { ctx, b, basic_block, err, stmt, allow_intr, allow_macro, allow_runtime, allow_macro_expansion, unwrapped, 1 }; 
            gtk_ml_array_trie_foreach_rev(&stmt->value.s_array.array, compile_quasi_array, gtk_ml_value_userdata(&data));
            if (!data.result) {
                return 0;
            }
            return gtk_ml_build_array_imm(ctx, b, *basic_block, err, gtk_ml_append_data(b, gtk_ml_value_int(n)));
        }
    }
    case GTKML_S_UNQUOTE: {
        GtkMl_SObj *expr = &stmt->value.s_unquote.expr;
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
        GtkMl_SObj function = gtk_ml_car(stmt);

        GtkMl_SObj args = gtk_ml_cdr(stmt);

        if (function->kind == GTKML_S_SYMBOL) {
            const char *symbol_unquote = "unquote";

            if (function->value.s_symbol.len == strlen(symbol_unquote)
                    && memcmp(function->value.s_symbol.ptr, symbol_unquote, function->value.s_symbol.len) == 0) {
                if (args->kind == GTKML_S_NIL) {
                    *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, stmt->span.ptr != NULL, stmt->span.line, stmt->span.col, 0);
                    return 0;
                }

                GtkMl_SObj *expr = &gtk_ml_car(args);
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
            if (!gtk_ml_build_push_imm(ctx, b, *basic_block, err, gtk_ml_append_data(b, gtk_ml_value_int(n - 1)))) {
                return 0;
            }
            if (!gtk_ml_build_add(ctx, b, *basic_block, err)) {
                return 0;
            }
            return gtk_ml_build_list(ctx, b, *basic_block, err);
        } else {
            return gtk_ml_build_list_imm(ctx, b, *basic_block, err, gtk_ml_append_data(b, gtk_ml_value_int(n)));
        }
    }
    }
}

gboolean gtk_ml_compile_expression(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
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
    case GTKML_S_FFI:
        return gtk_ml_build_push_imm(ctx, b, *basic_block, err, gtk_ml_append_static_data(b, *stmt));
    case GTKML_S_SYMBOL: {
        GtkMl_TaggedValue local = gtk_ml_builder_get(b, *stmt);
        if (gtk_ml_has_value(local)) {
            return gtk_ml_build_local_imm(ctx, b, *basic_block, err, gtk_ml_append_data(b, local));
        }
        return gtk_ml_build_get_imm(ctx, b, *basic_block, err, gtk_ml_append_static_data(b, *stmt));
    }
    case GTKML_S_LAMBDA: {
        char *linkage_name = malloc(strlen("lambda$") + 16);
        snprintf(linkage_name, strlen("lambda$") + 16, "lambda$%"GTKML_FMT_64"u", gtk_ml_builder_get_and_inc(ctx, b)->value.s_int.value);
        GtkMl_BasicBlock *bb = gtk_ml_append_basic_block(b, linkage_name);
        if (!gtk_ml_compile_program_internal(ctx, b, &bb, err, linkage_name, *stmt, allow_intr, allow_macro, allow_runtime, allow_macro_expansion, GTKML_PROG_RUNTIME)) {
            return 0;
        }
        return gtk_ml_build_push_addr(ctx, b, *basic_block, err, gtk_ml_append_static_data(b, gtk_ml_new_string(ctx, NULL, linkage_name, strlen(linkage_name))));
    }
    case GTKML_S_MACRO: if (allow_macro) {
        char *linkage_name = malloc(strlen("macro$") + 16);
        snprintf(linkage_name, strlen("macro$") + 16, "macro$%"GTKML_FMT_64"u", gtk_ml_builder_get_and_inc(ctx, b)->value.s_int.value);
        GtkMl_BasicBlock *bb = gtk_ml_append_basic_block(b, linkage_name);
        if (!gtk_ml_compile_program_internal(ctx, b, &bb, err, linkage_name, *stmt, allow_intr, allow_macro, allow_runtime, allow_macro_expansion, GTKML_PROG_MACRO)) {
            return 0;
        }
        return gtk_ml_build_push_addr(ctx, b, *basic_block, err, gtk_ml_append_static_data(b, gtk_ml_new_string(ctx, NULL, linkage_name, strlen(linkage_name))));
    }
    case GTKML_S_MAP: {
        int64_t n = gtk_ml_hash_trie_len(&(*stmt)->value.s_map.map);
        struct CompileData data = { ctx, b, basic_block, err, (*stmt), allow_intr, allow_macro, allow_runtime, allow_macro_expansion, NULL, 1 }; 
        gtk_ml_hash_trie_foreach(&(*stmt)->value.s_map.map, compile_map, gtk_ml_value_userdata(&data));
        if (!data.result) {
            return 0;
        }
        return gtk_ml_build_map_imm(ctx, b, *basic_block, err, gtk_ml_append_data(b, gtk_ml_value_int(n)));
    }
    case GTKML_S_SET: {
        int64_t n = gtk_ml_hash_set_len(&(*stmt)->value.s_set.set);
        struct CompileData data = { ctx, b, basic_block, err, (*stmt), allow_intr, allow_macro, allow_runtime, allow_macro_expansion, NULL, 1 }; 
        gtk_ml_hash_set_foreach(&(*stmt)->value.s_set.set, compile_set, gtk_ml_value_userdata(&data));
        if (!data.result) {
            return 0;
        }
        return gtk_ml_build_set_imm(ctx, b, *basic_block, err, gtk_ml_append_data(b, gtk_ml_value_int(n)));
    }
    case GTKML_S_ARRAY: {
        if (gtk_ml_array_trie_is_string(&(*stmt)->value.s_array.array)) {
            return gtk_ml_build_push_imm(ctx, b, *basic_block, err, gtk_ml_append_static_data(b, *stmt));
        } else {
            int64_t n = gtk_ml_array_trie_len(&(*stmt)->value.s_array.array);
            struct CompileData data = { ctx, b, basic_block, err, (*stmt), allow_intr, allow_macro, allow_runtime, allow_macro_expansion, NULL, 1 }; 
            gtk_ml_array_trie_foreach_rev(&(*stmt)->value.s_array.array, compile_array, gtk_ml_value_userdata(&data));
            if (!data.result) {
                return 0;
            }
            return gtk_ml_build_array_imm(ctx, b, *basic_block, err, gtk_ml_append_data(b, gtk_ml_value_int(n)));
        }
    }
    case GTKML_S_VAR:
        if (!gtk_ml_compile_expression(ctx, b, basic_block, err, &(*stmt)->value.s_var.expr, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
            return 0;
        }
        return gtk_ml_build_var(ctx, b, *basic_block, err);
    case GTKML_S_VARARG:
        *err = gtk_ml_error(ctx, "vararg-error", GTKML_ERR_VARARG_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    case GTKML_S_QUOTE:
        return gtk_ml_build_push_imm(ctx, b, *basic_block, err, gtk_ml_append_static_data(b, (*stmt)->value.s_quote.expr));
    case GTKML_S_QUASIQUOTE:
        return gtk_ml_compile_quasi_expression(ctx, b, basic_block, err, (*stmt)->value.s_quasiquote.expr, allow_intr, allow_macro, allow_runtime, allow_macro_expansion, NULL);
    case GTKML_S_UNQUOTE:
        *err = gtk_ml_error(ctx, "unquote-error", GTKML_ERR_UNQUOTE_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    case GTKML_S_LIST: {
        GtkMl_SObj *function = &gtk_ml_car(*stmt);

        GtkMl_SObj _args = gtk_ml_cdr(*stmt);

        size_t cap = 8;
        size_t n_args = 0;
        GtkMl_TaggedValue *args = malloc(sizeof(GtkMl_TaggedValue) * cap);

        while (_args->kind != GTKML_S_NIL) {
            if (n_args == cap) {
                cap *= 2;
                args = realloc(args, sizeof(GtkMl_TaggedValue) * cap);
            }
            args[n_args++] = gtk_ml_value_sobject(gtk_ml_car(_args));
            _args = gtk_ml_cdr(_args);
        }

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

            GtkMl_SObj _err = NULL;
            GtkMl_SObj program = gtk_ml_get_export(b->intr_ctx, &_err, linkage_name);
            free(linkage_name);
            if (program && program->value.s_program.kind == GTKML_PROG_INTRINSIC) {

                GtkMl_BasicBlock *bb = *basic_block;
                GtkMl_SObj bb_ld = gtk_ml_new_lightdata(b->intr_ctx, NULL, bb);
                GtkMl_SObj bb_var = gtk_ml_new_var(b->intr_ctx, NULL, bb_ld);
                gtk_ml_bind(b->intr_ctx, gtk_ml_new_symbol(ctx, NULL, 0, "CTX", strlen("CTX")), gtk_ml_value_userdata(ctx));
                gtk_ml_bind(b->intr_ctx, gtk_ml_new_symbol(ctx, NULL, 0, "CODE-BUILDER", strlen("CODE-BUILDER")), gtk_ml_value_userdata(b));
                gtk_ml_bind(b->intr_ctx, gtk_ml_new_symbol(ctx, NULL, 0, "BASIC-BLOCK", strlen("BASIC-BLOCK")), gtk_ml_value_sobject(bb_var));
                if (!gtk_ml_execute(b->intr_ctx, err, gtk_ml_value_sobject(program), args, n_args)) {
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

            GtkMl_SObj _err = NULL;
            GtkMl_SObj program = gtk_ml_get_export(b->macro_ctx, &_err, linkage_name);
            free(linkage_name);
            if (program && program->value.s_program.kind == GTKML_PROG_MACRO) {
                if (!gtk_ml_execute(b->macro_ctx, err, gtk_ml_value_sobject(program), args, n_args)) {
                    return 0;
                }

                GtkMl_SObj result = gtk_ml_pop(b->macro_ctx).value.sobj;

                if (!result) {
                    return 0;
                }

                *stmt = result;

                return gtk_ml_compile_expression(ctx, b, basic_block, err, stmt, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
            }
        }

        gboolean unwrapped = 0;
        n_args = 0;
        _args = gtk_ml_cdr(*stmt);
        while (_args->kind != GTKML_S_NIL) {
            if (unwrapped) {
                *err = gtk_ml_error(ctx, "unwrap-error", GTKML_ERR_UNWRAP_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
                return 0;
            }
            GtkMl_SObj *arg = &gtk_ml_car(_args);
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
            _args = gtk_ml_cdr(_args);
        }

        if (!gtk_ml_build_push_imm(ctx, b, *basic_block, err, gtk_ml_append_data(b, gtk_ml_value_int(n_args)))) {
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

gboolean gtk_ml_compile_body(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj body, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    while (body->kind != GTKML_S_NIL) {
        GtkMl_SObj *stmt = &gtk_ml_car(body);
        if (!gtk_ml_compile_expression(ctx, b, basic_block, err, stmt, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
            return 0;
        }
        body = gtk_ml_cdr(body);
    }

    return 1;
}

gboolean gtk_ml_compile_program_internal(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, const char *linkage_name, GtkMl_SObj lambda, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion, GtkMl_ProgramKind kind) {
    if ((!allow_macro || lambda->kind != GTKML_S_MACRO) && lambda->kind != GTKML_S_LAMBDA) {
        *err = gtk_ml_error(ctx, "program-error", GTKML_ERR_PROGRAM_ERROR, lambda->span.ptr != NULL, lambda->span.line, lambda->span.col, 0);
        return 0;
    }

    if ((*basic_block)->len_text == (*basic_block)->cap_text) {
        (*basic_block)->cap_text *= 2;
        (*basic_block)->text = realloc((*basic_block)->text, sizeof(GtkMl_Instruction) * (*basic_block)->cap_text);
    }

    char *name = malloc(strlen(linkage_name) + 1);
    strcpy(name, linkage_name);
    GtkMl_SObj program = gtk_ml_new_program(ctx, &lambda->span, name, 0, lambda->value.s_lambda.args, lambda->value.s_lambda.body, lambda->value.s_lambda.capture, kind);
    (*basic_block)->text[(*basic_block)->len_text].cond = GTKML_F_NONE;
    (*basic_block)->text[(*basic_block)->len_text].category = GTKML_I_EXPORT;
    (*basic_block)->text[(*basic_block)->len_text].opcode = 0;
    (*basic_block)->text[(*basic_block)->len_text].data = gtk_ml_append_static_data(b, program);
    ++(*basic_block)->len_text;

    GtkMl_SObj params = lambda->value.s_lambda.args;

    gtk_ml_builder_enter(ctx, b, 0);

    size_t len = 0;
    while (params->kind != GTKML_S_NIL) {
        ++len;
        params = gtk_ml_cdr(params);
    }

    int64_t *offsets = malloc(sizeof(int64_t) * len);
    GtkMl_SObj revparams = gtk_ml_new_nil(ctx, NULL);
    params = lambda->value.s_lambda.args;
    size_t i = 0;
    while (params->kind != GTKML_S_NIL) {
        GtkMl_SObj param = gtk_ml_car(params);
        offsets[i] = gtk_ml_builder_alloca(ctx, b);
        revparams = gtk_ml_new_list(ctx, NULL, param, revparams);
        params = gtk_ml_cdr(params);
        ++i;
    }

    params = lambda->value.s_lambda.args;
    for (size_t i = 0; i < len; i++) {
        GtkMl_SObj param = gtk_ml_car(params);
        int64_t offset = offsets[len - i - 1];
        if (param->kind == GTKML_S_VARARG) {
            gtk_ml_builder_bind(ctx, b, param->value.s_vararg.expr, offset);
        } else {
            gtk_ml_builder_bind(ctx, b, param, offset);
        }
        params = gtk_ml_cdr(params);
    }

    free(offsets);

    if (!gtk_ml_build_push_imm(ctx, b, *basic_block, err, gtk_ml_append_static_data(b, revparams))) {
        return 0;
    }
    if (!gtk_ml_build_bind_args(ctx, b, *basic_block, err)) {
        return 0;
    }

    gtk_ml_builder_enter(ctx, b, 1);

    if (!gtk_ml_compile_body(ctx, b, basic_block, err, lambda->value.s_lambda.body, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }

    gtk_ml_builder_leave(ctx, b);
    gtk_ml_builder_leave(ctx, b);

    if (!gtk_ml_build_ret(ctx, b, *basic_block, err)) {
        return 0;
    }

    return 1;
}

struct CollectData {
    gboolean change;
    GtkMl_Context *ctx;
    GtkMl_Builder *b;
    gboolean *has_intr;
};

GTKML_PRIVATE gboolean collect_intrinsics_inner(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_SObj expr, gboolean *has_intr) GTKML_MUST_USE;

GTKML_MUST_USE
GTKML_PRIVATE GtkMl_VisitResult collect_intrinsics_map(GtkMl_HashTrie *ht, GtkMl_TaggedValue key, GtkMl_TaggedValue value, GtkMl_TaggedValue data) {
    (void) ht;

    struct CollectData *col = data.value.userdata;

    col->change |= collect_intrinsics_inner(col->ctx, col->b, key.value.sobj, col->has_intr);
    col->change |= collect_intrinsics_inner(col->ctx, col->b, value.value.sobj, col->has_intr);

    return GTKML_VISIT_RECURSE;
}

GTKML_MUST_USE
GTKML_PRIVATE GtkMl_VisitResult collect_intrinsics_set(GtkMl_HashSet *hs, GtkMl_TaggedValue key, GtkMl_TaggedValue data) {
    (void) hs;

    struct CollectData *col = data.value.userdata;

    col->change |= collect_intrinsics_inner(col->ctx, col->b, key.value.sobj, col->has_intr);

    return GTKML_VISIT_RECURSE;
}

GTKML_MUST_USE
GTKML_PRIVATE GtkMl_VisitResult collect_intrinsics_array(GtkMl_Array *array, size_t idx, GtkMl_TaggedValue value, GtkMl_TaggedValue data) {
    (void) array;
    (void) idx;

    struct CollectData *col = data.value.userdata;

    col->change |= collect_intrinsics_inner(col->ctx, col->b, value.value.sobj, col->has_intr);

    return GTKML_VISIT_RECURSE;
}

gboolean collect_intrinsics_inner(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_SObj expr, gboolean *has_intr) {
    switch (expr->kind) {
    case GTKML_S_LAMBDA: {
        gboolean change = 0;
        GtkMl_SObj body = expr->value.s_lambda.body;
        while (body->kind != GTKML_S_NIL) {
            change |= collect_intrinsics_inner(ctx, b, gtk_ml_car(body), has_intr);
            body = gtk_ml_cdr(body);
        }
        return change;
    }
    case GTKML_S_MACRO: {
        gboolean change = 0;
        GtkMl_SObj body = expr->value.s_macro.body;
        while (body->kind != GTKML_S_NIL) {
            change |= collect_intrinsics_inner(ctx, b, gtk_ml_car(body), has_intr);
            body = gtk_ml_cdr(body);
        }
        return change;
    }
    case GTKML_S_LIST: {
        gboolean change = 0;
        GtkMl_SObj fn = gtk_ml_car(expr);
        if (fn->kind == GTKML_S_SYMBOL) {
            const char *symbol_define_intrinsic = "define-intrinsic";
            const char *symbol_define_macro = "define-macro";
            const char *symbol_define = "define";

            if (fn->value.s_symbol.len == strlen(symbol_define_intrinsic)
                    && memcmp(fn->value.s_symbol.ptr, symbol_define_intrinsic, fn->value.s_symbol.len) == 0) {
                GtkMl_SObj def = gtk_ml_cdar(expr);
                GtkMl_SObj body = gtk_ml_cddr(expr);
                GtkMl_SObj name = gtk_ml_car(def);

                if (gtk_ml_hash_set_contains(&b->intr_fns, gtk_ml_value_sobject(name))) {
                    return 0;
                }

                GtkMl_HashSet new;
                gtk_ml_hash_set_insert(&new, &b->intr_fns, gtk_ml_value_sobject(name));
                gtk_ml_del_hash_set(ctx, &b->intr_fns, gtk_ml_delete_value);
                b->intr_fns = new;
                *has_intr = 1;
                change = 1;

                while (body->kind != GTKML_S_NIL) {
                    (void) collect_intrinsics_inner(ctx, b, gtk_ml_car(body), has_intr);
                    body = gtk_ml_cdr(body);
                }
            } else if (fn->value.s_symbol.len == strlen(symbol_define_macro)
                    && memcmp(fn->value.s_symbol.ptr, symbol_define_macro, fn->value.s_symbol.len) == 0) {
                GtkMl_SObj def = gtk_ml_cdar(expr);
                GtkMl_SObj body = gtk_ml_cddr(expr);
                GtkMl_SObj name = gtk_ml_car(def);

                if (gtk_ml_hash_set_contains(&b->intr_fns, gtk_ml_value_sobject(name))) {
                    return 0;
                }

                while (body->kind != GTKML_S_NIL) {
                    change |= collect_intrinsics_inner(ctx, b, gtk_ml_car(body), has_intr);
                    body = gtk_ml_cdr(body);
                }

                if (change) {
                    GtkMl_HashSet new;
                    gtk_ml_hash_set_insert(&new, &b->intr_fns, gtk_ml_value_sobject(name));
                    gtk_ml_del_hash_set(ctx, &b->intr_fns, gtk_ml_delete_value);
                    b->intr_fns = new;
                    *has_intr = 1;
                }
            } else if (fn->value.s_symbol.len == strlen(symbol_define)
                    && memcmp(fn->value.s_symbol.ptr, symbol_define, fn->value.s_symbol.len) == 0) {
                GtkMl_SObj def = gtk_ml_cdar(expr);
                if (def->kind != GTKML_S_SYMBOL) {
                    GtkMl_SObj body = gtk_ml_cddr(expr);
                    GtkMl_SObj name = gtk_ml_car(def);

                    if (gtk_ml_hash_set_contains(&b->intr_fns, gtk_ml_value_sobject(name))) {
                        return 0;
                    }

                    while (body->kind != GTKML_S_NIL) {
                        change |= collect_intrinsics_inner(ctx, b, gtk_ml_car(body), has_intr);
                        body = gtk_ml_cdr(body);
                    }

                    if (change) {
                        GtkMl_HashSet new;
                        gtk_ml_hash_set_insert(&new, &b->intr_fns, gtk_ml_value_sobject(name));
                        gtk_ml_del_hash_set(ctx, &b->intr_fns, gtk_ml_delete_value);
                        b->intr_fns = new;
                        *has_intr = 1;
                    }
                }
            } else {
                if (gtk_ml_hash_set_contains(&b->intr_fns, gtk_ml_value_sobject(fn))) {
                    return 0;
                }
            }
        }
        return change;
    }
    case GTKML_S_ARRAY: {
        struct CollectData col = { 0, ctx, b, has_intr };
        gtk_ml_array_trie_foreach(&expr->value.s_array.array, collect_intrinsics_array, gtk_ml_value_userdata(&col));
        return col.change;
    }
    case GTKML_S_MAP: {
        struct CollectData col = { 0, ctx, b, has_intr };
        gtk_ml_hash_trie_foreach(&expr->value.s_map.map, collect_intrinsics_map, gtk_ml_value_userdata(&col));
        return col.change;
    }
    case GTKML_S_SET: {
        struct CollectData col = { 0, ctx, b, has_intr };
        gtk_ml_hash_set_foreach(&expr->value.s_set.set, collect_intrinsics_set, gtk_ml_value_userdata(&col));
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

GTKML_PRIVATE void collect_intrinsics(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_SObj expr) {
    gboolean has_intr = 0;
    while (collect_intrinsics_inner(ctx, b, expr, &has_intr)) {}
}

GTKML_PRIVATE gboolean collect_macros_inner(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_SObj expr, gboolean *has_macro) GTKML_MUST_USE;

GTKML_PRIVATE GtkMl_VisitResult collect_macros_map(GtkMl_HashTrie *ht, GtkMl_TaggedValue key, GtkMl_TaggedValue value, GtkMl_TaggedValue data) {
    (void) ht;

    struct CollectData *col = data.value.userdata;

    col->change |= collect_macros_inner(col->ctx, col->b, key.value.sobj, col->has_intr);
    col->change |= collect_macros_inner(col->ctx, col->b, value.value.sobj, col->has_intr);

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult collect_macros_set(GtkMl_HashSet *hs, GtkMl_TaggedValue key, GtkMl_TaggedValue data) {
    (void) hs;

    struct CollectData *col = data.value.userdata;

    col->change |= collect_macros_inner(col->ctx, col->b, key.value.sobj, col->has_intr);

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult collect_macros_array(GtkMl_Array *array, size_t idx, GtkMl_TaggedValue value, GtkMl_TaggedValue data) {
    (void) array;
    (void) idx;

    struct CollectData *col = data.value.userdata;

    col->change |= collect_macros_inner(col->ctx, col->b, value.value.sobj, col->has_intr);

    return GTKML_VISIT_RECURSE;
}

gboolean collect_macros_inner(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_SObj expr, gboolean *has_macro) {
    switch (expr->kind) {
    case GTKML_S_LAMBDA: {
        gboolean change = 0;
        GtkMl_SObj body = expr->value.s_lambda.body;
        while (body->kind != GTKML_S_NIL) {
            change |= collect_macros_inner(ctx, b, gtk_ml_car(body), has_macro);
            body = gtk_ml_cdr(body);
        }
        return change;
    }
    case GTKML_S_MACRO: {
        gboolean change = 0;
        GtkMl_SObj body = expr->value.s_macro.body;
        while (body->kind != GTKML_S_NIL) {
            change |= collect_macros_inner(ctx, b, gtk_ml_car(body), has_macro);
            body = gtk_ml_cdr(body);
        }
        return change;
    }
    case GTKML_S_LIST: {
        gboolean change = 0;
        GtkMl_SObj fn = gtk_ml_car(expr);
        if (fn->kind == GTKML_S_SYMBOL) {
            const char *symbol_define_macro = "define-macro";
            const char *symbol_define = "define";

            if (fn->value.s_symbol.len == strlen(symbol_define_macro)
                    && memcmp(fn->value.s_symbol.ptr, symbol_define_macro, fn->value.s_symbol.len) == 0) {
                GtkMl_SObj def = gtk_ml_cdar(expr);
                GtkMl_SObj body = gtk_ml_cddr(expr);
                GtkMl_SObj name = gtk_ml_car(def);

                if (gtk_ml_hash_set_contains(&b->macro_fns, gtk_ml_value_sobject(name))) {
                    return 0;
                }

                while (body->kind != GTKML_S_NIL) {
                    change |= collect_macros_inner(ctx, b, gtk_ml_car(body), has_macro);
                    body = gtk_ml_cdr(body);
                }

                if (change) {
                    GtkMl_HashSet new;
                    gtk_ml_hash_set_insert(&new, &b->macro_fns, gtk_ml_value_sobject(name));
                    gtk_ml_del_hash_set(ctx, &b->macro_fns, gtk_ml_delete_value);
                    b->macro_fns = new;
                    *has_macro = 1;
                }
            } else if (fn->value.s_symbol.len == strlen(symbol_define)
                    && memcmp(fn->value.s_symbol.ptr, symbol_define, fn->value.s_symbol.len) == 0) {
                GtkMl_SObj def = gtk_ml_cdar(expr);
                if (def->kind != GTKML_S_SYMBOL) {
                    GtkMl_SObj body = gtk_ml_cddr(expr);
                    GtkMl_SObj name = gtk_ml_car(def);

                    if (gtk_ml_hash_set_contains(&b->macro_fns, gtk_ml_value_sobject(name))) {
                        return 0;
                    }

                    while (body->kind != GTKML_S_NIL) {
                        change |= collect_macros_inner(ctx, b, gtk_ml_car(body), has_macro);
                        body = gtk_ml_cdr(body);
                    }

                    if (change) {
                        GtkMl_HashSet new;
                        gtk_ml_hash_set_insert(&new, &b->macro_fns, gtk_ml_value_sobject(name));
                        gtk_ml_del_hash_set(ctx, &b->macro_fns, gtk_ml_delete_value);
                        b->macro_fns = new;
                        *has_macro = 1;
                    }
                }
            } else {
                if (gtk_ml_hash_set_contains(&b->macro_fns, gtk_ml_value_sobject(fn))) {
                    return 0;
                }
            }
        }
        return change;
    }
    case GTKML_S_ARRAY: {
        struct CollectData col = { 0, ctx, b, has_macro };
        gtk_ml_array_trie_foreach(&expr->value.s_array.array, collect_macros_array, gtk_ml_value_userdata(&col));
        return col.change;
    }
    case GTKML_S_MAP: {
        struct CollectData col = { 0, ctx, b, has_macro };
        gtk_ml_hash_trie_foreach(&expr->value.s_map.map, collect_macros_map, gtk_ml_value_userdata(&col));
        return col.change;
    }
    case GTKML_S_SET: {
        struct CollectData col = { 0, ctx, b, has_macro };
        gtk_ml_hash_set_foreach(&expr->value.s_set.set, collect_macros_set, gtk_ml_value_userdata(&col));
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

GTKML_PRIVATE void collect_macros(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_SObj expr) {
    gboolean has_macro = 0;
    while (collect_macros_inner(ctx, b, expr, &has_macro)) {}
}

gboolean gtk_ml_compile_intrinsics(GtkMl_Builder *b, GtkMl_BasicBlock **start, GtkMl_SObj *err, GtkMl_SObj lambda) {
    GtkMl_SObj prev = lambda->value.s_lambda.body;
    GtkMl_SObj next = prev;

    GtkMl_SObj revparams = gtk_ml_new_nil(b->intr_ctx, NULL);
    GtkMl_SObj params = lambda->value.s_lambda.args;
    while (params->kind != GTKML_S_NIL) {
        GtkMl_SObj param = gtk_ml_car(params);
        revparams = gtk_ml_new_list(b->intr_ctx, NULL, param, revparams);
        params = gtk_ml_cdr(params);
    }

    if (!gtk_ml_build_push_imm(b->intr_ctx, b, *start, err, gtk_ml_append_static_data(b, revparams))) {
        return 0;
    }
    if (!gtk_ml_build_bind_args(b->intr_ctx, b, *start, err)) {
        return 0;
    }

    while (next->kind != GTKML_S_NIL) {
        GtkMl_SObj stmt = gtk_ml_car(next);

        if (stmt->kind != GTKML_S_LIST) {
            next = gtk_ml_cdr(next);
            continue;
        }

        GtkMl_SObj function = gtk_ml_car(stmt);

        GtkMl_SObj args = gtk_ml_cdr(stmt);

        if (function->kind == GTKML_S_SYMBOL) {
            const char *symbol_define_intrinsic = "define-intrinsic";
            const char *symbol_define = "define";

            if (function->value.s_symbol.len == strlen(symbol_define_intrinsic)
                    && memcmp(function->value.s_symbol.ptr, symbol_define_intrinsic, function->value.s_symbol.len) == 0) {
                GtkMl_SObj intrinsic_definition = gtk_ml_car(args);
                GtkMl_SObj intrinsic_name = gtk_ml_car(intrinsic_definition);
                GtkMl_SObj intrinsic_args = gtk_ml_cdr(intrinsic_definition);
                GtkMl_SObj intrinsic_body = gtk_ml_cdr(args);

                GtkMl_SObj intrinsic = gtk_ml_new_lambda(b->intr_ctx, NULL, intrinsic_args, intrinsic_body, gtk_ml_new_map(b->intr_ctx, NULL, NULL));
                if (!intrinsic) {
                    return 0;
                }

                size_t len = intrinsic_name->value.s_symbol.len;
                char *linkage_name = malloc(len + 1);
                memcpy(linkage_name, intrinsic_name->value.s_symbol.ptr, len);
                linkage_name[len] = 0;

                GtkMl_BasicBlock *basic_block = gtk_ml_append_basic_block(b, linkage_name);

                if (!gtk_ml_compile_program_internal(b->intr_ctx, b, &basic_block, err, linkage_name, intrinsic, 1, 0, 0, 0, GTKML_PROG_INTRINSIC)) {
                    free(linkage_name);
                    return 0;
                }

                if (!gtk_ml_build_push_addr(b->intr_ctx, b, *start, err, gtk_ml_append_static_data(b, gtk_ml_new_string(b->intr_ctx, NULL, linkage_name, strlen(linkage_name))))) {
                    return 0;
                }
                if (!gtk_ml_build_push_imm(b->intr_ctx, b, *start, err, gtk_ml_append_static_data(b, intrinsic_name))) {
                    return 0;
                }
                if (!gtk_ml_build_define(b->intr_ctx, b, *start, err)) {
                    return 0;
                }
            } else if (function->value.s_symbol.len == strlen(symbol_define)
                    && memcmp(function->value.s_symbol.ptr, symbol_define, function->value.s_symbol.len) == 0) {
                GtkMl_SObj intrinsic_definition = gtk_ml_car(args);
                if (intrinsic_definition->kind == GTKML_S_SYMBOL) {
                    GtkMl_SObj name = intrinsic_definition;
                    GtkMl_SObj *value = &gtk_ml_cdar(args);
                    if (!gtk_ml_compile_expression(b->intr_ctx, b, start, err, value, 1, 0, 0, 0)) {
                        return 0;
                    }
                    if (!gtk_ml_build_push_imm(b->intr_ctx, b, *start, err, gtk_ml_append_static_data(b, name))) {
                        return 0;
                    }
                    if (!gtk_ml_build_define(b->intr_ctx, b, *start, err)) {
                        return 0;
                    }
                } else {
                    GtkMl_SObj intrinsic_name = gtk_ml_car(intrinsic_definition);
                    if (!gtk_ml_hash_set_contains(&b->intr_fns, gtk_ml_value_sobject(intrinsic_name)) && !gtk_ml_hash_set_contains(&b->macro_fns, gtk_ml_value_sobject(intrinsic_name))) {
                        GtkMl_SObj intrinsic_args = gtk_ml_cdr(intrinsic_definition);
                        GtkMl_SObj intrinsic_body = gtk_ml_cdr(args);

                        GtkMl_SObj intrinsic = gtk_ml_new_lambda(b->intr_ctx, NULL, intrinsic_args, intrinsic_body, gtk_ml_new_map(b->intr_ctx, NULL, NULL));
                        if (!intrinsic) {
                            return 0;
                        }

                        size_t len = intrinsic_name->value.s_symbol.len;
                        char *linkage_name = malloc(len + 1);
                        memcpy(linkage_name, intrinsic_name->value.s_symbol.ptr, len);
                        linkage_name[len] = 0;

                        GtkMl_BasicBlock *basic_block = gtk_ml_append_basic_block(b, linkage_name);

                        if (!gtk_ml_compile_program_internal(b->intr_ctx, b, &basic_block, err, linkage_name, intrinsic, 1, 0, 0, 0, GTKML_PROG_RUNTIME)) {
                            free(linkage_name);
                            return 0;
                        }

                        if (!gtk_ml_build_push_addr(b->intr_ctx, b, *start, err, gtk_ml_append_static_data(b, gtk_ml_new_string(b->intr_ctx, NULL, linkage_name, strlen(linkage_name))))) {
                            return 0;
                        }
                        if (!gtk_ml_build_push_imm(b->intr_ctx, b, *start, err, gtk_ml_append_static_data(b, intrinsic_name))) {
                            return 0;
                        }
                        if (!gtk_ml_build_define(b->intr_ctx, b, *start, err)) {
                            return 0;
                        }
                    }
                }
            }
        }
        next = gtk_ml_cdr(next);
    }

    if (!gtk_ml_build_ret(b->intr_ctx, b, *start, err)) {
        return 0;
    }
    
    return 1;
}

gboolean gtk_ml_compile_macros(GtkMl_Builder *b, GtkMl_BasicBlock **start, GtkMl_SObj *err, GtkMl_SObj lambda) {
    GtkMl_SObj prev = lambda->value.s_lambda.body;
    GtkMl_SObj next = prev;

    GtkMl_SObj revparams = gtk_ml_new_nil(b->intr_ctx, NULL);
    GtkMl_SObj params = lambda->value.s_lambda.args;
    while (params->kind != GTKML_S_NIL) {
        GtkMl_SObj param = gtk_ml_car(params);
        revparams = gtk_ml_new_list(b->intr_ctx, NULL, param, revparams);
        params = gtk_ml_cdr(params);
    }

    if (!gtk_ml_build_push_imm(b->intr_ctx, b, *start, err, gtk_ml_append_static_data(b, revparams))) {
        return 0;
    }
    if (!gtk_ml_build_bind_args(b->intr_ctx, b, *start, err)) {
        return 0;
    }

    while (next->kind != GTKML_S_NIL) {
        GtkMl_SObj stmt = gtk_ml_car(next);

        if (stmt->kind != GTKML_S_LIST) {
            next = gtk_ml_cdr(next);
            continue;
        }

        GtkMl_SObj function = gtk_ml_car(stmt);

        GtkMl_SObj args = gtk_ml_cdr(stmt);

        if (function->kind == GTKML_S_SYMBOL) {
            const char *symbol_define_macro = "define-macro";
            const char *symbol_define = "define";

            if (function->value.s_symbol.len == strlen(symbol_define_macro)
                    && memcmp(function->value.s_symbol.ptr, symbol_define_macro, function->value.s_symbol.len) == 0) {
                GtkMl_SObj macro_definition = gtk_ml_car(args);
                GtkMl_SObj macro_name = gtk_ml_car(macro_definition);
                GtkMl_SObj macro_args = gtk_ml_cdr(macro_definition);
                GtkMl_SObj macro_body = gtk_ml_cdr(args);

                GtkMl_SObj macro = gtk_ml_new_macro(b->macro_ctx, NULL, macro_args, macro_body, gtk_ml_new_map(b->macro_ctx, NULL, NULL));
                if (!macro) {
                    return 0;
                }

                size_t len = macro_name->value.s_symbol.len;
                char *linkage_name = malloc(len + 1);
                memcpy(linkage_name, macro_name->value.s_symbol.ptr, len);
                linkage_name[len] = 0;

                GtkMl_BasicBlock *basic_block = gtk_ml_append_basic_block(b, linkage_name);

                if (!gtk_ml_compile_program_internal(b->macro_ctx, b, &basic_block, err, linkage_name, macro, 0, 1, 0, 0, GTKML_PROG_MACRO)) {
                    free(linkage_name);
                    return 0;
                }

                if (!gtk_ml_build_push_addr(b->macro_ctx, b, *start, err, gtk_ml_append_static_data(b, gtk_ml_new_string(b->macro_ctx, NULL, linkage_name, strlen(linkage_name))))) {
                    return 0;
                }
                if (!gtk_ml_build_push_imm(b->macro_ctx, b, *start, err, gtk_ml_append_static_data(b, macro_name))) {
                    return 0;
                }
                if (!gtk_ml_build_define(b->macro_ctx, b, *start, err)) {
                    return 0;
                }
            } else if (function->value.s_symbol.len == strlen(symbol_define)
                    && memcmp(function->value.s_symbol.ptr, symbol_define, function->value.s_symbol.len) == 0) {
                GtkMl_SObj intrinsic_definition = gtk_ml_car(args);
                if (intrinsic_definition->kind == GTKML_S_SYMBOL) {
                    GtkMl_SObj name = intrinsic_definition;
                    GtkMl_SObj *value = &gtk_ml_cdar(args);
                    if (!gtk_ml_compile_expression(b->intr_ctx, b, start, err, value, 0, 1, 0, 0)) {
                        return 0;
                    }
                    if (!gtk_ml_build_push_imm(b->intr_ctx, b, *start, err, gtk_ml_append_static_data(b, name))) {
                        return 0;
                    }
                    if (!gtk_ml_build_define(b->intr_ctx, b, *start, err)) {
                        return 0;
                    }
                } else {
                    GtkMl_SObj intrinsic_name = gtk_ml_car(intrinsic_definition);
                    if (!gtk_ml_hash_set_contains(&b->intr_fns, gtk_ml_value_sobject(intrinsic_name)) && !gtk_ml_hash_set_contains(&b->macro_fns, gtk_ml_value_sobject(intrinsic_name))) {
                        GtkMl_SObj intrinsic_args = gtk_ml_cdr(intrinsic_definition);
                        GtkMl_SObj intrinsic_body = gtk_ml_cdr(args);

                        GtkMl_SObj intrinsic = gtk_ml_new_lambda(b->intr_ctx, NULL, intrinsic_args, intrinsic_body, gtk_ml_new_map(b->intr_ctx, NULL, NULL));
                        if (!intrinsic) {
                            return 0;
                        }

                        size_t len = intrinsic_name->value.s_symbol.len;
                        char *linkage_name = malloc(len + 1);
                        memcpy(linkage_name, intrinsic_name->value.s_symbol.ptr, len);
                        linkage_name[len] = 0;

                        GtkMl_BasicBlock *basic_block = gtk_ml_append_basic_block(b, linkage_name);

                        if (!gtk_ml_compile_program_internal(b->intr_ctx, b, &basic_block, err, linkage_name, intrinsic, 0, 1, 0, 0, GTKML_PROG_RUNTIME)) {
                            free(linkage_name);
                            return 0;
                        }

                        if (!gtk_ml_build_push_addr(b->intr_ctx, b, *start, err, gtk_ml_append_static_data(b, gtk_ml_new_string(b->intr_ctx, NULL, linkage_name, strlen(linkage_name))))) {
                            return 0;
                        }
                        if (!gtk_ml_build_push_imm(b->intr_ctx, b, *start, err, gtk_ml_append_static_data(b, intrinsic_name))) {
                            return 0;
                        }
                        if (!gtk_ml_build_define(b->intr_ctx, b, *start, err)) {
                            return 0;
                        }
                    }
                }
            }
        }
        next = gtk_ml_cdr(next);
    }

    if (!gtk_ml_build_ret(b->intr_ctx, b, *start, err)) {
        return 0;
    }
    
    return 1;
}

gboolean gtk_ml_compile(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_SObj *err, GtkMl_SObj lambda) {
    GtkMl_SObj counter = gtk_ml_builder_get_and_inc(ctx, b);
    char *linkage_name = malloc(strlen("_start.") + 16);
    sprintf(linkage_name, "_start.%"GTKML_FMT_64"u", counter->value.s_int.value);
    GtkMl_BasicBlock *basic_block = gtk_ml_append_basic_block(b, linkage_name);

    if (!compile_runtime_program(ctx, b, &basic_block, err, linkage_name, lambda)) {
        return 0;
    }

    return 1;
}

gboolean gtk_ml_compile_program(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_SObj *err, GtkMl_SObj lambda) {
    collect_intrinsics(ctx, b, lambda);

    if (!gtk_ml_execute_internal(ctx, err, gtk_ml_value_sobject(lambda), GTKML_NO_ARGS, 0, GTKML_STAGE_INTR)) {
        return 0;
    }

    collect_macros(ctx, b, lambda);

    if (!gtk_ml_execute_internal(ctx, err, gtk_ml_value_sobject(lambda), GTKML_NO_ARGS, 0, GTKML_STAGE_MACRO)) {
        return 0;
    }

    return gtk_ml_compile(ctx, b, err, lambda);
}
