#include <gtk/gtk.h>
#include "gtk-ml.h"

#define GUI "examples/hello.gtkml"

int main() {
    GtkMl_SObj err = NULL;

    GtkMl_Context *ctx = gtk_ml_new_context(NULL, 0);

    char *src;
    GtkMl_SObj gui;
    if (!(gui = gtk_ml_load(ctx, &src, &err, GUI))) {
        (void) gtk_ml_dumpf(ctx, stderr, NULL, err);
        fprintf(stderr, "\n");
        gtk_ml_del_context(ctx);
        return 1;
    }

    gtk_ml_push(ctx, gtk_ml_value_sobject(gui));

    GtkMl_Builder *builder = gtk_ml_new_builder(ctx);

    if (!gtk_ml_compile_program(ctx, builder, &err, gui)) {
        free(src);
        (void) gtk_ml_dumpf(ctx, stderr, NULL, err);
        fprintf(stderr, "\n");
        gtk_ml_del_context(ctx);
        return 1;
    }

    GtkMl_Program *linked = gtk_ml_build(ctx, &err, builder, NULL, 0);
    if (!linked) {
        free(src);
        (void) gtk_ml_dumpf(ctx, stderr, NULL, err);
        fprintf(stderr, "\n");
        gtk_ml_del_context(ctx);
        return 1;
    }

    GtkMl_Serializer serf;
    gtk_ml_new_serializer(&serf);
    FILE *bgtkml = fopen("hello.bgtkml", "wb");
    if (!gtk_ml_serf_program(&serf, ctx, bgtkml, &err, linked)) {
        free(src);
        (void) gtk_ml_dumpf(ctx, stderr, NULL, err);
        fprintf(stderr, "\n");
        gtk_ml_del_context(ctx);
        return 1;
    }

    GtkMl_Deserializer deserf;
    gtk_ml_new_deserializer(&deserf);
    bgtkml = freopen("hello.bgtkml", "r", bgtkml);
    GtkMl_Program *loaded = gtk_ml_deserf_program(&deserf, ctx, bgtkml, &err);
    if (!loaded) {
        free(src);
        (void) gtk_ml_dumpf(ctx, stderr, NULL, err);
        fprintf(stderr, "\n");
        gtk_ml_del_context(ctx);
        return 1;
    }

    fclose(bgtkml);

    gtk_ml_load_program(ctx, loaded);

    if (!gtk_ml_dumpf_program(ctx, stdout, &err)) {
        free(src);
        (void) gtk_ml_dumpf(ctx, stderr, NULL, err);
        fprintf(stderr, "\n");
        gtk_ml_del_context(ctx);
        return 1;
    }

    GtkMl_SObj program = gtk_ml_get_export(ctx, &err, loaded->start);
    if (!program) {
        free(src);
        (void) gtk_ml_dumpf(ctx, stderr, NULL, err);
        fprintf(stderr, "\n");
        gtk_ml_del_context(ctx);
        return 1;
    }

    if (!gtk_ml_run_program(ctx, &err, program, NULL)) {
        free(src);
        (void) gtk_ml_dumpf(ctx, stderr, NULL, err);
        fprintf(stderr, "\n");
        gtk_ml_del_context(ctx);
        return 1;
    }

    GtkMl_SObj app = gtk_ml_peek(ctx).value.sobj;

    if (!app) {
        free(src);
        (void) gtk_ml_dumpf(ctx, stderr, NULL, err);
        fprintf(stderr, "\n");
        gtk_ml_del_context(ctx);
        return 1;
    }

    int status = g_application_run(G_APPLICATION(app->value.s_userdata.userdata), 0, NULL);

    gtk_ml_del_context(ctx);
    free(src);

    return status;
}
