#include <gtk/gtk.h>
#include "gtk-ml.h"

#define GUI "examples/gui.gtkml"

int main() {
    const char *err;

    GtkMl_Context *ctx = gtk_ml_new_context();

    char *src;
    GtkMl_S *gui;
    if (!(gui = gtk_ml_load(ctx, &src, &err, GUI))) {
        gtk_ml_del_context(ctx);
        fprintf(stderr, "%s\n", err);
        return 1;
    }

    gtk_ml_push(ctx, gui);

    GtkMl_S *app;
    if (!(app = gtk_ml_call(ctx, &err, gui, gtk_ml_nil(ctx)))) {
        gtk_ml_del_context(ctx);
        fprintf(stderr, "%s\n", err);
        return 1;
    }

    gtk_ml_push(ctx, app);

    int status = g_application_run(G_APPLICATION(app->value.s_userdata.userdata), 0, NULL);

    gtk_ml_del_context(ctx);
    free(src);

    return status;
}
