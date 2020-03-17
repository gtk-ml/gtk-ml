#define GTKML_INCLUDE_INTERNAL
#include "gtk-ml.h"
#include "gtk-ml-internal.h"

#ifndef __EMSCRIPTEN__
#error "cg-web.c isn't meant to be compiled without emscripten"
#endif

gboolean gtk_ml_builder_web_log(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_WEB_LOG, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_web_new_float32_array(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_WEB_NEW_FLOAT32_ARRAY, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}
