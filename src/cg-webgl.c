#define GTKML_INCLUDE_INTERNAL
#include "gtk-ml.h"
#include "gtk-ml-internal.h"

#ifndef __EMSCRIPTEN__
#error "cg-webgl.c isn't meant to be compiled without emscripten"
#endif

gboolean gtk_ml_builder_webgl_create_shader(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_WEBGL_CREATE_SHADER, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_webgl_shader_source(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL
        || gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_WEBGL_SHADER_SOURCE, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_webgl_compile_shader(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_WEBGL_COMPILE_SHADER, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_webgl_get_shader_parameter(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL
        || gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_WEBGL_GET_SHADER_PARAMETER, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_webgl_get_shader_info_log(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_WEBGL_GET_SHADER_INFO_LOG, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_webgl_delete_shader(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_WEBGL_DELETE_SHADER, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_webgl_create_program(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    if (args->kind != GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_WEBGL_CREATE_PROGRAM, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_webgl_attach_shader(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL
        || gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_WEBGL_ATTACH_SHADER, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_webgl_link_program(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_WEBGL_LINK_PROGRAM, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_webgl_get_program_parameter(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL
        || gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_WEBGL_GET_PROGRAM_PARAMETER, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_webgl_get_program_info_log(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_WEBGL_GET_PROGRAM_INFO_LOG, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_webgl_delete_program(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_WEBGL_DELETE_PROGRAM, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_webgl_get_uniform_location(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL
        || gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_WEBGL_GET_UNIFORM_LOCATION, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_webgl_get_attrib_location(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL
        || gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_WEBGL_GET_ATTRIB_LOCATION, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_webgl_create_buffer(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    if (args->kind != GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_WEBGL_CREATE_BUFFER, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_webgl_bind_buffer(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL
        || gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_WEBGL_BIND_BUFFER, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_webgl_buffer_data(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL
        || gtk_ml_cdr(args)->kind == GTKML_S_NIL
        || gtk_ml_cddr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_WEBGL_BUFFER_DATA, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_webgl_create_vertex_array(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    if (args->kind != GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_WEBGL_CREATE_VERTEX_ARRAY, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_webgl_bind_vertex_array(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_WEBGL_BIND_VERTEX_ARRAY, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_webgl_enable_vertex_attrib_array(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_WEBGL_ENABLE_VERTEX_ATTRIB_ARRAY, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_webgl_vertex_attrib_pointer(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL
        || gtk_ml_cdr(args)->kind == GTKML_S_NIL
        || gtk_ml_cddr(args)->kind == GTKML_S_NIL
        || gtk_ml_cdddr(args)->kind == GTKML_S_NIL
        || gtk_ml_cddddr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_WEBGL_VERTEX_ATTRIB_POINTER, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_webgl_viewport(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL
        || gtk_ml_cdr(args)->kind == GTKML_S_NIL
        || gtk_ml_cddr(args)->kind == GTKML_S_NIL
        || gtk_ml_cdddr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_WEBGL_VIEWPORT, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_webgl_clear_color(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL
        || gtk_ml_cdr(args)->kind == GTKML_S_NIL
        || gtk_ml_cddr(args)->kind == GTKML_S_NIL
        || gtk_ml_cdddr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_WEBGL_CLEAR_COLOR, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_webgl_clear(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_WEBGL_CLEAR, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_webgl_use_program(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_WEBGL_USE_PROGRAM, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_webgl_uniform_4f(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL
        || gtk_ml_cdr(args)->kind == GTKML_S_NIL
        || gtk_ml_cddr(args)->kind == GTKML_S_NIL
        || gtk_ml_cdddr(args)->kind == GTKML_S_NIL
        || gtk_ml_cddddr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_WEBGL_UNIFORM_4F, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_webgl_draw_arrays(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL
        || gtk_ml_cdr(args)->kind == GTKML_S_NIL
        || gtk_ml_cddr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_WEBGL_DRAW_ARRAYS, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_webgl_canvas_width(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    if (args->kind != GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_WEBGL_CANVAS_WIDTH, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean gtk_ml_builder_webgl_canvas_height(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    if (args->kind != GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_core_call(ctx, b, basic_block, err, GTKML_CORE_WEBGL_CANVAS_HEIGHT, *stmt, 0, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}
