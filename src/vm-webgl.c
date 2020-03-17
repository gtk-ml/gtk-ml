#define GTKML_INCLUDE_INTERNAL
#include "gtk-ml.h"
#include "gtk-ml-internal.h"

#ifndef __EMSCRIPTEN__
#error "vm-webgl.c isn't meant to be compiled without emscripten"
#endif

#include <emscripten.h>

GTKML_PRIVATE GtkMl_TaggedValue vm_core_webgl_create_shader(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_webgl_shader_source(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_webgl_compile_shader(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_webgl_get_shader_parameter(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_webgl_get_shader_info_log(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_webgl_delete_shader(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_webgl_create_program(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_webgl_attach_shader(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_webgl_link_program(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_webgl_get_program_parameter(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_webgl_get_program_info_log(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_webgl_delete_program(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_webgl_get_uniform_location(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_webgl_get_attrib_location(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_webgl_create_buffer(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_webgl_bind_buffer(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_webgl_buffer_data(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_webgl_create_vertex_array(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_webgl_enable_vertex_attrib_array(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_webgl_vertex_attrib_pointer(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_webgl_viewport(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_webgl_clear_color(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_webgl_clear(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_webgl_use_program(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_webgl_bind_vertex_array(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_webgl_uniform_4f(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_webgl_draw_arrays(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);

EM_JS(int, gtk_ml_js_webgl_create_shader, (int type), {
    let shader = gtk_web_gl.createShader(type);
    let handle = gtk_web_gl_objects.length;
    gtk_web_gl_objects.push(shader);
    return handle;
});

GtkMl_TaggedValue vm_core_webgl_create_shader(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) err;
    (void) expr;

    GtkMl_TaggedValue type = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    (void) gtk_ml_pop(ctx);

    int shader = gtk_ml_js_webgl_create_shader(type.value.s64);

    return gtk_ml_value_int(shader);
}

EM_JS(void, gtk_ml_js_webgl_shader_source, (int shader, const char *src), {
    gtk_web_gl.shaderSource(gtk_web_gl_objects[shader], UTF8ToString(src));
});

GtkMl_TaggedValue vm_core_webgl_shader_source(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) err;
    (void) expr;

    GtkMl_SObj source = gtk_ml_pop(ctx).value.sobj;
    GtkMl_TaggedValue shader = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    (void) gtk_ml_pop(ctx);

    char *c_str = gtk_ml_to_c_str(source);
    gtk_ml_js_webgl_shader_source(shader.value.s64, c_str);
    free(c_str);

    return gtk_ml_value_nil();
}

EM_JS(void, gtk_ml_js_webgl_compile_shader, (int shader), {
    gtk_web_gl.compileShader(gtk_web_gl_objects[shader])
});

GtkMl_TaggedValue vm_core_webgl_compile_shader(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) err;
    (void) expr;

    GtkMl_TaggedValue shader = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    (void) gtk_ml_pop(ctx);

    gtk_ml_js_webgl_compile_shader(shader.value.s64);

    return gtk_ml_value_nil();
}

EM_JS(int, gtk_ml_js_webgl_get_shader_parameter, (int shader, int parameter, int *type), {
    let result = gtk_web_gl.getShaderParameter(gtk_web_gl_objects[shader], parameter);
    switch (typeof(result)) {
    case "boolean":
        HEAP32[type >> 2] = 0;
        break;
    case "number":
        HEAP32[type >> 2] = 1;
        break;
    case "string":
        HEAP32[type >> 2] = 2;
        break;
    default:
        HEAP32[type >> 2] = 4;
        break;
    }
    return result;
});

GtkMl_TaggedValue vm_core_webgl_get_shader_parameter(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) err;
    (void) expr;

    GtkMl_TaggedValue param = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    GtkMl_TaggedValue shader = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    (void) gtk_ml_pop(ctx);

    int type;
    int value = gtk_ml_js_webgl_get_shader_parameter(shader.value.s64, param.value.u64, (int *) &type);

    switch (type) {
    case 0:
        return value? gtk_ml_value_true() : gtk_ml_value_false();
    case 1:
        return gtk_ml_value_int(value);
    case 2:
        return gtk_ml_value_sobject(gtk_ml_new_string(ctx, NULL, (char *) value, strlen((char *) value)));
    default:
        return gtk_ml_value_nil();
    }
}

EM_JS(const char *, gtk_ml_js_webgl_get_shader_info_log, (int shader), {
    let log = gtk_web_gl.getShaderInfoLog(gtk_web_gl_objects[shader]);
    let len = lengthBytesUTF8(log);
    let ptr = _malloc(len + 1);
    stringToUTF8(log, ptr, len + 1);
    return ptr;
});

GtkMl_TaggedValue vm_core_webgl_get_shader_info_log(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) err;
    (void) expr;

    GtkMl_TaggedValue shader = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    (void) gtk_ml_pop(ctx);

    const char *msg = gtk_ml_js_webgl_get_shader_info_log(shader.value.s64);

    GtkMl_TaggedValue result = gtk_ml_value_sobject(gtk_ml_new_string(ctx, NULL, msg, strlen(msg)));
    free((void *) msg);
    return result;
}

EM_JS(void, gtk_ml_js_webgl_delete_shader, (int shader), {
    gtk_web_gl.deleteShader(gtk_web_gl_objects[shader]);
});

GtkMl_TaggedValue vm_core_webgl_delete_shader(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) err;
    (void) expr;

    GtkMl_TaggedValue shader = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    (void) gtk_ml_pop(ctx);

    gtk_ml_js_webgl_delete_shader(shader.value.s64);

    return gtk_ml_value_nil();
}

EM_JS(int, gtk_ml_js_webgl_create_program, (), {
    let program = gtk_web_gl.createProgram();
    let handle = gtk_web_gl_objects.length;
    gtk_web_gl_objects.push(program);
    return handle;
});

GtkMl_TaggedValue vm_core_webgl_create_program(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) err;
    (void) expr;

    (void) gtk_ml_pop(ctx);

    int program = gtk_ml_js_webgl_create_program();

    return gtk_ml_value_int(program);
}

EM_JS(void, gtk_ml_js_webgl_attach_shader, (int program, int shader), {
    gtk_web_gl.attachShader(gtk_web_gl_objects[program], gtk_web_gl_objects[shader]);
});

GtkMl_TaggedValue vm_core_webgl_attach_shader(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) err;
    (void) expr;

    GtkMl_TaggedValue shader = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    GtkMl_TaggedValue program = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    (void) gtk_ml_pop(ctx);

    gtk_ml_js_webgl_attach_shader(program.value.s64, shader.value.s64);

    return gtk_ml_value_nil();
}

EM_JS(void, gtk_ml_js_webgl_link_program, (int program), {
    gtk_web_gl.linkProgram(gtk_web_gl_objects[program]);
});

GtkMl_TaggedValue vm_core_webgl_link_program(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) err;
    (void) expr;

    GtkMl_TaggedValue program = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    (void) gtk_ml_pop(ctx);

    gtk_ml_js_webgl_link_program(program.value.s64);

    return gtk_ml_value_nil();
}

EM_JS(int, gtk_ml_js_webgl_get_program_parameter, (int program, int parameter, int *type), {
    let result = gtk_web_gl.getProgramParameter(gtk_web_gl_objects[program], parameter);
    switch (typeof(result)) {
    case "boolean":
        HEAP32[type >> 2] = 0;
        break;
    case "number":
        HEAP32[type >> 2] = 1;
        break;
    case "string":
        HEAP32[type >> 2] = 2;
        break;
    default:
        HEAP32[type >> 2] = 4;
        break;
    }
    return result;
});

GtkMl_TaggedValue vm_core_webgl_get_program_parameter(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) err;
    (void) expr;

    GtkMl_TaggedValue param = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    GtkMl_TaggedValue program = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    (void) gtk_ml_pop(ctx);

    int type;
    int value = gtk_ml_js_webgl_get_program_parameter(program.value.s64, param.value.u64, &type);

    switch (type) {
    case 0:
        return value? gtk_ml_value_true() : gtk_ml_value_false();
    case 1:
        return gtk_ml_value_int(value);
    case 2:
        return gtk_ml_value_sobject(gtk_ml_new_string(ctx, NULL, (char *) value, strlen((char *) value)));
    default:
        return gtk_ml_value_nil();
    }
}

EM_JS(const char *, gtk_ml_js_webgl_get_program_info_log, (int program), {
    let log = gtk_web_gl.getProgramInfoLog(gtk_web_gl_objects[program]);
    let len = lengthBytesUTF8(log);
    let ptr = _malloc(len + 1);
    stringToUTF8(log, ptr, len + 1);
    return ptr;
});

GtkMl_TaggedValue vm_core_webgl_get_program_info_log(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) err;
    (void) expr;

    GtkMl_TaggedValue program = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    (void) gtk_ml_pop(ctx);

    const char *msg = gtk_ml_js_webgl_get_program_info_log(program.value.s64);

    GtkMl_TaggedValue result = gtk_ml_value_sobject(gtk_ml_new_string(ctx, NULL, msg, strlen(msg)));
    free((void *) msg);
    return result;
}

EM_JS(void, gtk_ml_js_webgl_delete_program, (int program), {
    gtk_web_gl.deleteProgram(gtk_web_gl_objects[program]);
});

GtkMl_TaggedValue vm_core_webgl_delete_program(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) err;
    (void) expr;

    GtkMl_TaggedValue program = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    (void) gtk_ml_pop(ctx);

    gtk_ml_js_webgl_delete_program(program.value.s64);

    return gtk_ml_value_nil();
}

EM_JS(int, gtk_ml_js_webgl_get_uniform_location, (int program, const char *name), {
    let location = gtk_web_gl.getUniformLocation(gtk_web_gl_objects[program], UTF8ToString(name));
    let handle = gtk_web_gl_objects.length;
    gtk_web_gl_objects.push(location);
    return handle;
});

GtkMl_TaggedValue vm_core_webgl_get_uniform_location(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) err;
    (void) expr;

    GtkMl_SObj name = gtk_ml_pop(ctx).value.sobj;
    GtkMl_TaggedValue program = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    (void) gtk_ml_pop(ctx);

    char *c_str = gtk_ml_to_c_str(name);
    int location = gtk_ml_js_webgl_get_uniform_location(program.value.s64, c_str);
    free(c_str);

    return gtk_ml_value_int(location);
}

EM_JS(int, gtk_ml_js_webgl_get_attrib_location, (int program, const char *name), {
    let location = gtk_web_gl.getAttribLocation(gtk_web_gl_objects[program], UTF8ToString(name));
    let handle = gtk_web_gl_objects.length;
    gtk_web_gl_objects.push(location);
    return handle;
});

GtkMl_TaggedValue vm_core_webgl_get_attrib_location(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) err;
    (void) expr;

    GtkMl_SObj name = gtk_ml_pop(ctx).value.sobj;
    GtkMl_TaggedValue program = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    (void) gtk_ml_pop(ctx);

    char *c_str = gtk_ml_to_c_str(name);
    int location = gtk_ml_js_webgl_get_attrib_location(program.value.s64, c_str);
    free(c_str);

    return gtk_ml_value_int(location);
}

EM_JS(int, gtk_ml_js_webgl_create_buffer, (), {
    let buffer = gtk_web_gl.createBuffer();
    let handle = gtk_web_gl_objects.length;
    gtk_web_gl_objects.push(buffer);
    return handle;
});

GtkMl_TaggedValue vm_core_webgl_create_buffer(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) err;
    (void) expr;

    (void) gtk_ml_pop(ctx);

    int buffer = gtk_ml_js_webgl_create_buffer();

    return gtk_ml_value_int(buffer);
}

EM_JS(void, gtk_ml_js_webgl_bind_buffer, (int target, int buffer), {
    gtk_web_gl.bindBuffer(target, gtk_web_gl_objects[buffer]);
});

GtkMl_TaggedValue vm_core_webgl_bind_buffer(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) err;
    (void) expr;

    GtkMl_TaggedValue buffer = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    GtkMl_TaggedValue target = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    (void) gtk_ml_pop(ctx);

    gtk_ml_js_webgl_bind_buffer(target.value.s64, buffer.value.s64);

    return gtk_ml_value_nil();
}

EM_JS(void, gtk_ml_js_webgl_buffer_data, (int target, int data, int draw_mode), {
    gtk_web_gl.bufferData(target, gtk_web_objects[data], draw_mode);
});

GtkMl_TaggedValue vm_core_webgl_buffer_data(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) err;
    (void) expr;

    GtkMl_TaggedValue draw_mode = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    GtkMl_TaggedValue buffer = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    GtkMl_TaggedValue target = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    (void) gtk_ml_pop(ctx);

    gtk_ml_js_webgl_buffer_data(target.value.s64, buffer.value.s64, draw_mode.value.s64);

    return gtk_ml_value_nil();
}

EM_JS(int, gtk_ml_js_webgl_create_vertex_array, (), {
    let buffer = gtk_web_gl.createVertexArray();
    let handle = gtk_web_gl_objects.length;
    gtk_web_gl_objects.push(buffer);
    return handle;
});

GtkMl_TaggedValue vm_core_webgl_create_vertex_array(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) err;
    (void) expr;

    (void) gtk_ml_pop(ctx);

    int vao = gtk_ml_js_webgl_create_vertex_array();

    return gtk_ml_value_int(vao);
}

EM_JS(void, gtk_ml_js_webgl_bind_vertex_array, (int vao), {
    gtk_web_gl.bindVertexArray(gtk_web_gl_objects[vao]);
});

GtkMl_TaggedValue vm_core_webgl_bind_vertex_array(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) err;
    (void) expr;

    GtkMl_TaggedValue vao = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    (void) gtk_ml_pop(ctx);

    gtk_ml_js_webgl_bind_vertex_array(vao.value.s64);

    return gtk_ml_value_nil();
}

EM_JS(void, gtk_ml_js_webgl_enable_vertex_attrib_array, (int location), {
    gtk_web_gl.enableVertexAttribArray(gtk_web_gl_objects[location]);
});

GtkMl_TaggedValue vm_core_webgl_enable_vertex_attrib_array(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) err;
    (void) expr;

    GtkMl_TaggedValue location = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    (void) gtk_ml_pop(ctx);

    gtk_ml_js_webgl_enable_vertex_attrib_array(location.value.s64);

    return gtk_ml_value_nil();
}

EM_JS(void, gtk_ml_js_webgl_vertex_attrib_pointer, (int location, int size, int type, gboolean normalize, int stride, int offset), {
    gtk_web_gl.vertexAttribPointer(gtk_web_gl_objects[location], size, type, Boolean(normalize), stride, offset);
});

GtkMl_TaggedValue vm_core_webgl_vertex_attrib_pointer(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) err;
    (void) expr;

    GtkMl_TaggedValue offset = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    GtkMl_TaggedValue stride = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    GtkMl_TaggedValue normalize = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    GtkMl_TaggedValue type = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    GtkMl_TaggedValue size = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    GtkMl_TaggedValue location = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    (void) gtk_ml_pop(ctx);

    gtk_ml_js_webgl_vertex_attrib_pointer(location.value.s64, size.value.s64, type.value.s64, normalize.value.boolean, stride.value.s64, offset.value.s64);

    return gtk_ml_value_nil();
}

EM_JS(void, gtk_ml_js_webgl_viewport, (float a, float b, float c, float d), {
    gtk_web_gl.viewport(a, b, c, d);
});

GtkMl_TaggedValue vm_core_webgl_viewport(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) err;
    (void) expr;

    GtkMl_TaggedValue w = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    GtkMl_TaggedValue z = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    GtkMl_TaggedValue y = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    GtkMl_TaggedValue x = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    (void) gtk_ml_pop(ctx);

    float fw = gtk_ml_prim_to_float(w);
    float fz = gtk_ml_prim_to_float(z);
    float fy = gtk_ml_prim_to_float(y);
    float fx = gtk_ml_prim_to_float(x);

    gtk_ml_js_webgl_viewport(fx, fy, fz, fw);

    return gtk_ml_value_nil();
}

EM_JS(void, gtk_ml_js_webgl_clear_color, (float r, float g, float b, float a), {
    gtk_web_gl.clearColor(r, g, b, a);
});

GtkMl_TaggedValue vm_core_webgl_clear_color(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) err;
    (void) expr;

    GtkMl_TaggedValue w = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    GtkMl_TaggedValue z = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    GtkMl_TaggedValue y = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    GtkMl_TaggedValue x = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    (void) gtk_ml_pop(ctx);

    float fw = gtk_ml_prim_to_float(w);
    float fz = gtk_ml_prim_to_float(z);
    float fy = gtk_ml_prim_to_float(y);
    float fx = gtk_ml_prim_to_float(x);

    gtk_ml_js_webgl_clear_color(fx, fy, fz, fw);

    return gtk_ml_value_nil();
}

EM_JS(void, gtk_ml_js_webgl_clear, (int bit), {
    gtk_web_gl.clear(bit);
});

GtkMl_TaggedValue vm_core_webgl_clear(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) err;
    (void) expr;

    GtkMl_TaggedValue bit = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    (void) gtk_ml_pop(ctx);

    gtk_ml_js_webgl_clear(bit.value.s64);

    return gtk_ml_value_nil();
}

EM_JS(void, gtk_ml_js_webgl_use_program, (int program), {
    gtk_web_gl.useProgram(gtk_web_gl_objects[program]);
});

GtkMl_TaggedValue vm_core_webgl_use_program(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) err;
    (void) expr;

    GtkMl_TaggedValue program = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    (void) gtk_ml_pop(ctx);

    gtk_ml_js_webgl_use_program(program.value.s64);

    return gtk_ml_value_nil();
}

EM_JS(void, gtk_ml_js_webgl_uniform_4f, (int location, float x, float y, float z, float w), {
    gtk_web_gl.uniform4f(gtk_web_gl_objects[location], x, y, z, w);
});

GtkMl_TaggedValue vm_core_webgl_uniform_4f(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) err;
    (void) expr;

    GtkMl_TaggedValue w = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    GtkMl_TaggedValue z = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    GtkMl_TaggedValue y = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    GtkMl_TaggedValue x = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    GtkMl_TaggedValue location = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    (void) gtk_ml_pop(ctx);

    float fw = gtk_ml_prim_to_float(w);
    float fz = gtk_ml_prim_to_float(z);
    float fy = gtk_ml_prim_to_float(y);
    float fx = gtk_ml_prim_to_float(x);

    gtk_ml_js_webgl_uniform_4f(location.value.s64, fx, fy, fz, fw);

    return gtk_ml_value_nil();
}

EM_JS(void, gtk_ml_js_webgl_draw_arrays, (int primitive, int offset, int count), {
    gtk_web_gl.drawArrays(primitive, offset, count);
});

GtkMl_TaggedValue vm_core_webgl_draw_arrays(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) err;
    (void) expr;

    GtkMl_TaggedValue count = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    GtkMl_TaggedValue offset = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    GtkMl_TaggedValue primitive = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    (void) gtk_ml_pop(ctx);

    gtk_ml_js_webgl_draw_arrays(primitive.value.s64, offset.value.s64, count.value.s64);

    return gtk_ml_value_nil();
}

EM_JS(int, gtk_ml_js_webgl_canvas_width, (), {
    return gtk_web_gl.canvas.width;
});

GtkMl_TaggedValue vm_core_webgl_canvas_width(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) err;
    (void) expr;

    (void) gtk_ml_pop(ctx);

    int width = gtk_ml_js_webgl_canvas_width();

    return gtk_ml_value_int(width);
}

EM_JS(int, gtk_ml_js_webgl_canvas_height, (), {
    return gtk_web_gl.canvas.height;
});

GtkMl_TaggedValue vm_core_webgl_canvas_height(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) err;
    (void) expr;

    (void) gtk_ml_pop(ctx);

    int height = gtk_ml_js_webgl_canvas_height();

    return gtk_ml_value_int(height);
}
