#include <stdlib.h>
#include <string.h>
#include <emscripten.h>
#include "gtk-ml.h"

#define GTKMLWEB_VERSION "gtkml-web ver. 0.0.0"

GtkMl_Context *gtk_ml_web_init() {
    return gtk_ml_new_context();
}

void gtk_ml_web_deinit(GtkMl_Context *ctx) {
    gtk_ml_del_context(ctx);
}

const char *gtk_ml_web_version() {
    return GTKMLWEB_VERSION "\n" GTKML_VERSION;
}

char *gtk_ml_web_eval(GtkMl_Context *ctx, const char *line) {
    GtkMl_S *err = NULL;

    GtkMl_S *lambda;
    if (!(lambda = gtk_ml_loads(ctx, &err, line))) {
        gtk_ml_dumpf(ctx, stderr, NULL, err);
        fprintf(stderr, "\n");
        return ":error";
    }

    GtkMl_Builder *builder = gtk_ml_new_builder(ctx);

    if (!gtk_ml_compile_program(ctx, builder, &err, lambda)) {
        gtk_ml_dumpf(ctx, stderr, NULL, err);
        fprintf(stderr, "\n");
        return ":error";
    }

    GtkMl_Program *program = gtk_ml_build(ctx, &err, builder);
    if (!program) {
        gtk_ml_dumpf(ctx, stderr, NULL, err);
        fprintf(stderr, "\n");
        return ":error";
    }

    gtk_ml_load_program(ctx, program);

    GtkMl_S *start = gtk_ml_get_export(ctx, &err, program->start);
    if (!start) {
        gtk_ml_dumpf(ctx, stderr, NULL, err);
        fprintf(stderr, "\n");
        return ":error";
    }

    if (!gtk_ml_run_program(ctx, &err, start, NULL)) {
        gtk_ml_dumpf(ctx, stderr, NULL, err);
        fprintf(stderr, "\n");
        return ":error";
    }

    size_t cap = 1024;
    char *result = malloc(cap);
    result = gtk_ml_dumpsnr(ctx, result, cap, &err, gtk_ml_peek(ctx));
    if (!result) {
        gtk_ml_dumpf(ctx, stderr, NULL, err);
        fprintf(stderr, "\n");
        return ":error";
    }

    return result;
}

int main() {
    EM_ASM(gtk_ml_js_init($0), gtk_ml_new_context());
    return 0;
}
