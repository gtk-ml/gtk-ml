#include <stdlib.h>
#include <string.h>
#include <emscripten.h>
#include <emscripten/html5.h>
#define GTKML_INCLUDE_INTERNAL
#include "gtk-ml.h"
#include "gtk-ml-internal.h"

#define GTKMLWEB_VERSION "gtkml-web ver. 0.0.0"

int gtk_ml_web_init_gl() {
    EmscriptenWebGLContextAttributes attributes;
    emscripten_webgl_init_context_attributes(&attributes);
    attributes.majorVersion = 3;
    attributes.minorVersion = 2;
    int context = emscripten_webgl_create_context("#gtkml-canvas", &attributes);
    emscripten_webgl_make_context_current(context);
    return context;
}

const char *gtk_ml_web_version() {
    return GTKMLWEB_VERSION "\n" GTKML_VERSION;
}

char *gtk_ml_web_eval(GtkMl_Index *index, GtkMl_Context *ctx, const char *line) {
    GtkMl_SObj err = NULL;

    GtkMl_TaggedValue program = gtk_ml_loads(index, ctx, &err, line);

    if (!gtk_ml_execute(index, ctx, &err, program, GTKML_NO_ARGS, 0)) {
        (void) gtk_ml_dumpf(ctx, stderr, NULL, err);
        fprintf(stderr, "\n");
        return ":error";
    }

    size_t cap = 1024;
    char *result = malloc(cap);
    result = gtk_ml_dumpsnr_value(ctx, result, cap, &err, gtk_ml_peek(ctx));
    if (!result) {
        (void) gtk_ml_dumpf(ctx, stderr, NULL, err);
        fprintf(stderr, "\n");
        return ":error";
    }

    return result;
}

int main() {
    fprintf(stdout, "%s\n", GTKMLWEB_VERSION);
    GtkMl_Index *index = gtk_ml_new_index();
    GtkMl_Context *ctx = gtk_ml_new_context(index);
    EM_ASM({ gtk_ml_js_init($0, $1, true) }, index, ctx);
    return 0;
}
