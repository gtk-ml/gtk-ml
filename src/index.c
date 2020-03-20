#define GTKML_INCLUDE_INTERNAL
#include "gtk-ml.h"
#include "gtk-ml-internal.h"

GtkMl_Index *gtk_ml_new_index() {
    GtkMl_Index *index = malloc(sizeof(GtkMl_Index));
    gtk_ml_new_debug_data(&index->debug_data);
    return index;
}

void gtk_ml_del_index(GtkMl_Index *index) {
    gtk_ml_del_debug_data(&index->debug_data);
    free(index);
}

void gtk_ml_new_debug_data(GtkMl_DebugData *debug_data) {
    debug_data->cap_debug = 64;
    debug_data->len_debug = 0;
    debug_data->debug = malloc(sizeof(GtkMl_Span) * debug_data->cap_debug);

    GtkMl_Span nil = {0};
    (void) gtk_ml_add_location(debug_data, &nil);
}

void gtk_ml_del_debug_data(GtkMl_DebugData *debug_data) {
    free(debug_data->debug);
}

GtkMl_Location gtk_ml_add_location(GtkMl_DebugData *debug_data, GtkMl_Span *span) {
    if (debug_data->len_debug == debug_data->cap_debug) {
        debug_data->cap_debug *= 2;
        debug_data->debug = realloc(debug_data->debug, sizeof(GtkMl_Span) * debug_data->cap_debug );
    }

    GtkMl_Location handle = debug_data->len_debug;
    debug_data->debug[debug_data->len_debug++] = *span;
    return handle;
}

GtkMl_Span *gtk_ml_get_span(GtkMl_DebugData *debug_data, GtkMl_Location location) {
    return &debug_data->debug[location];
}
