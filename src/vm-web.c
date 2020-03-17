#define GTKML_INCLUDE_INTERNAL
#include "gtk-ml.h"
#include "gtk-ml-internal.h"

#ifndef __EMSCRIPTEN__
#error "vm-web.c isn't meant to be compiled without emscripten"
#endif

#include <emscripten.h>

GTKML_PRIVATE GtkMl_TaggedValue vm_core_web_log(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);

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
