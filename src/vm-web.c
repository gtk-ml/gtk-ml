#define GTKML_INCLUDE_INTERNAL
#include "gtk-ml.h"
#include "gtk-ml-internal.h"

#ifndef __EMSCRIPTEN__
#error "vm-web.c isn't meant to be compiled without emscripten"
#endif

#include <emscripten.h>

GTKML_PRIVATE GtkMl_TaggedValue vm_core_web_log(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_web_new_float32_array(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);

EM_JS(int, gtk_ml_js_web_log, (const char *c_msg), {
    let msg = UTF8ToString(c_msg);
    console.log(msg);
    return lengthBytesUTF8(msg);
});

GtkMl_TaggedValue vm_core_web_log(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) err;
    (void) expr;

    GtkMl_SObj msg = gtk_ml_pop(ctx).value.sobj;
    (void) gtk_ml_pop(ctx);

    char *c_str = gtk_ml_to_c_str(msg);
    int len = gtk_ml_js_web_log(c_str);
    free(c_str);

    return gtk_ml_value_int(len);
}

EM_JS(int, gtk_ml_js_web_new_float32_array, (float *ptr, size_t len), {
    let buffer = new ArrayBuffer(4 * len); // sizeof(float) * len
    let view = new Int32Array(buffer);
    for (let i = 0; i < len; i++) {
        view[i] = HEAP32[(ptr >> 2) + i];
    }
    let array = new Float32Array(buffer);
    let handle = gtk_web_objects.length;
    gtk_web_objects.push(array);
    return handle;
});

GtkMl_TaggedValue vm_core_web_new_float32_array(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) err;
    (void) expr;

    GtkMl_SObj array = gtk_ml_pop(ctx).value.sobj;
    (void) gtk_ml_pop(ctx);

    size_t len = gtk_ml_array_trie_len(&array->value.s_array.array);
    float *buffer = malloc(sizeof(float) * len);
    for (size_t i = 0; i < len; i++) {
        GtkMl_TaggedValue elem = gtk_ml_array_trie_get(&array->value.s_array.array, i);
        if (gtk_ml_is_sobject(elem)) {
            if (elem.value.sobj->kind == GTKML_S_FLOAT) {
                buffer[i] = (float) elem.value.sobj->value.s_float.value;
            } else if (elem.value.sobj->kind == GTKML_S_INT) {
                buffer[i] = (float) elem.value.sobj->value.s_int.value;
            } else {
                *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 0);
                return gtk_ml_value_none();
            }
        } else {
            if (elem.tag == GTKML_TAG_FLOAT) {
                buffer[i] = (float) elem.value.flt;
            } else if (elem.tag == GTKML_TAG_INT64) {
                buffer[i] = (float) elem.value.s64;
            } else if (elem.tag == GTKML_TAG_UINT64) {
                buffer[i] = (float) elem.value.u64;
            } else {
                *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 0);
                return gtk_ml_value_none();
            }
        }
    }

    int handle = gtk_ml_js_web_new_float32_array(buffer, len);

    return gtk_ml_value_int(handle);
}
