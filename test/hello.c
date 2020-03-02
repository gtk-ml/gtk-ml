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

    GtkMl_Program linked = gtk_ml_build(ctx, &err, builder);
    if (!linked.exec) {
        gtk_ml_del_context(ctx);
        free(src);
        fprintf(stderr, "%s\n", err);
        return 1;
    }

    gtk_ml_load_program(ctx, linked);

    GtkMl_S *program = gtk_ml_get_export(ctx, &err, linked.start);
    if (!program) {
        gtk_ml_del_context(ctx);
        free(src);
        fprintf(stderr, "%s\n", err);
        return 1;
    }

    if (!(gtk_ml_run_program(ctx, &err, program, NULL))) {
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
    gtk_ml_del_program(linked);

    return status;
}
