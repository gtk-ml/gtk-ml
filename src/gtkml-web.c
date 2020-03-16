#include <stdlib.h>
#include <string.h>
#include <emscripten.h>
#include "gtk-ml.h"

#define GTKMLWEB_VERSION "gtkml-web ver. 0.0.0"

GtkMl_Context *gtk_ml_web_init() {
    return gtk_ml_new_context();
}

void gtk_ml_web_deinit(GtkMl_Context *ctx, GtkMl_Program **previous_program, size_t *n_previous) {
    free(previous_program);
    free(n_previous);
    gtk_ml_del_context(ctx);
}

const char *gtk_ml_web_version() {
    return GTKMLWEB_VERSION "\n" GTKML_VERSION;
}

char *gtk_ml_web_eval(GtkMl_Context *ctx, const char *line, GtkMl_Builder *builder, GtkMl_Program **previous_program, size_t *n_previous) {
    GtkMl_SObj err = NULL;

    GtkMl_SObj lambda;
    if (!(lambda = gtk_ml_loads(ctx, &err, line))) {
        (void) gtk_ml_dumpf(ctx, stderr, NULL, err);
        fprintf(stderr, "\n");
        return ":error";
    }

    if (!gtk_ml_compile_program(ctx, builder, &err, lambda)) {
        (void) gtk_ml_dumpf(ctx, stderr, NULL, err);
        fprintf(stderr, "\n");
        return ":error";
    }

    GtkMl_Program *program = gtk_ml_build(ctx, &err, builder, previous_program, *n_previous);
    if (!program) {
        (void) gtk_ml_dumpf(ctx, stderr, NULL, err);
        fprintf(stderr, "\n");
        return ":error";
    }

    *previous_program = program;
    *n_previous = 1;

    gtk_ml_load_program(ctx, program);

    GtkMl_SObj start = gtk_ml_get_export(ctx, &err, program->start);
    if (!start) {
        (void) gtk_ml_dumpf(ctx, stderr, NULL, err);
        fprintf(stderr, "\n");
        return ":error";
    }

    if (!gtk_ml_run_program(ctx, &err, start, NULL)) {
        (void) gtk_ml_dumpf(ctx, stderr, NULL, err);
        fprintf(stderr, "\n");
        return ":error";
    }

    size_t cap = 1024;
    char *result = malloc(cap);
    result = gtk_ml_dumpsnr(ctx, result, cap, &err, gtk_ml_peek(ctx).value.sobj);
    if (!result) {
        (void) gtk_ml_dumpf(ctx, stderr, NULL, err);
        fprintf(stderr, "\n");
        return ":error";
    }

    return result;
}

int main() {
    fprintf(stdout, "%s\n", GTKMLWEB_VERSION);
    GtkMl_Program **previous_program = malloc(sizeof(GtkMl_Program *));
    size_t *n_previous = malloc(sizeof(size_t));
    *n_previous = 0;
    GtkMl_Context *ctx = gtk_ml_new_context();
    EM_ASM({ gtk_ml_js_init($0, $1, $2, $3) }, ctx, gtk_ml_new_builder(ctx), previous_program, n_previous);
    return 0;
}
