#include <gtk/gtk.h>
#include "gtk-ml.h"

#define GUI "examples/hello.gtkml"

int main() {
    const char *err = NULL;

    GtkMl_Context *ctx = gtk_ml_new_context(NULL, 0);

    char *src;
    GtkMl_S *gui;
    if (!(gui = gtk_ml_load(ctx, &src, &err, GUI))) {
        gtk_ml_del_context(ctx);
        fprintf(stderr, "%s\n", err);
        return 1;
    }

    gtk_ml_push(ctx, gui);

    GtkMl_Builder *builder = gtk_ml_new_builder();

    if (!gtk_ml_compile(ctx, builder, &err, gui)) {
        gtk_ml_del_context(ctx);
        free(src);
        fprintf(stderr, "%s\n", err);
        return 1;
    }

    GtkMl_Program linked;
    if (!gtk_ml_build(ctx, &linked, &err, builder)) {
        gtk_ml_del_context(ctx);
        free(src);
        fprintf(stderr, "%s\n", err);
        return 1;
    }

    FILE *bgtkml = fopen("hello.bgtkml", "wb+");
    if (!gtk_ml_serf_program(bgtkml, &err, &linked)) {
        gtk_ml_del_context(ctx);
        free(src);
        fprintf(stderr, "%s\n", err);
        return 1;
    }
    gtk_ml_del_program(&linked);

    bgtkml = freopen("hello.bgtkml", "r", bgtkml);
    GtkMl_Program loaded;
    if (!gtk_ml_deserf_program(ctx, &loaded, bgtkml, &err)) {
        gtk_ml_del_context(ctx);
        free(src);
        fprintf(stderr, "%s\n", err);
        return 1;
    }

    fclose(bgtkml);

    gtk_ml_load_program(ctx, &loaded);

    if (!gtk_ml_dumpf_program(ctx, stdout, &err)) {
        gtk_ml_del_context(ctx);
        free(src);
        fprintf(stderr, "%s\n", err);
        return 1;
    }

    GtkMl_S *program = gtk_ml_get_export(ctx, &err, loaded.start);
    if (!program) {
        gtk_ml_del_context(ctx);
        free(src);
        fprintf(stderr, "%s\n", err);
        return 1;
    }
    gtk_ml_del_program(&loaded);

    if (!gtk_ml_run_program(ctx, &err, program, NULL)) {
        gtk_ml_del_context(ctx);
        free(src);
        fprintf(stderr, "%s\n", err);
        return 1;
    }

    GtkMl_S *app = gtk_ml_peek(ctx);

    if (!app) {
        gtk_ml_del_context(ctx);
        free(src);
        fprintf(stderr, "%s\n", err);
        return 1;
    }

    int status = g_application_run(G_APPLICATION(app->value.s_userdata.userdata), 0, NULL);

    gtk_ml_del_context(ctx);
    free(src);

    return status;
}
