#include <stdlib.h>
#include <string.h>
#ifdef GTKML_ENABLE_GTK
#include <gtk/gtk.h>
#endif /* GTKML_ENABLE_GTK */
#include <math.h>
#define GTKML_INCLUDE_INTERNAL
#include "gtk-ml.h"
#include "gtk-ml-internal.h"

GtkMl_S *gtk_ml_new_value(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_SKind kind) {
    ++ctx->gc->n_values;

    GtkMl_S *s = malloc(sizeof(GtkMl_S));
    s->next = ctx->gc->first;
    ctx->gc->first = s;

    s->flags = GTKML_FLAG_NONE;
    s->kind = kind;
    if (span) {
        s->span = *span;
    } else {
        s->span = (GtkMl_Span) {0};
    }

    return s;
}

GtkMl_S *gtk_ml_new_nil(GtkMl_Context *ctx, GtkMl_Span *span) {
    GtkMl_S *s = gtk_ml_new_value(ctx, span, GTKML_S_NIL);
    return s;
}

GtkMl_S *gtk_ml_new_true(GtkMl_Context *ctx, GtkMl_Span *span) {
    GtkMl_S *s = gtk_ml_new_value(ctx, span, GTKML_S_TRUE);
    return s;
}

GtkMl_S *gtk_ml_new_false(GtkMl_Context *ctx, GtkMl_Span *span) {
    GtkMl_S *s = gtk_ml_new_value(ctx, span, GTKML_S_FALSE);
    return s;
}

GtkMl_S *gtk_ml_new_int(GtkMl_Context *ctx, GtkMl_Span *span, int64_t value) {
    GtkMl_S *s = gtk_ml_new_value(ctx, span, GTKML_S_INT);
    s->value.s_int.value = value;
    return s;
}

GtkMl_S *gtk_ml_new_float(GtkMl_Context *ctx, GtkMl_Span *span, float value) {
    GtkMl_S *s = gtk_ml_new_value(ctx, span, GTKML_S_FLOAT);
    s->value.s_float.value = value;
    return s;
}

GtkMl_S *gtk_ml_new_char(GtkMl_Context *ctx, GtkMl_Span *span, uint32_t value) {
    GtkMl_S *s = gtk_ml_new_value(ctx, span, GTKML_S_CHAR);
    s->value.s_char.value = value;
    return s;
}

GtkMl_S *gtk_ml_new_string(GtkMl_Context *ctx, GtkMl_Span *span, const char *ptr, size_t len) {
    GtkMl_S *s = gtk_ml_new_value(ctx, span, GTKML_S_ARRAY);
    GtkMl_Array array;
    gtk_ml_new_array_trie(&array);
    for (size_t i = 0; i < len; i++) {
        GtkMl_Array new;
        // TODO: unicode codepoints
        gtk_ml_array_trie_push(&new, &array, gtk_ml_new_char(ctx, span, ptr[i]));
        array = new;
    }
    s->value.s_array.array = array;
    return s;
}

GtkMl_S *gtk_ml_new_symbol(GtkMl_Context *ctx, GtkMl_Span *span, gboolean owned, const char *ptr, size_t len) {
    GtkMl_S *s = gtk_ml_new_value(ctx, span, GTKML_S_SYMBOL);
    s->value.s_symbol.owned = owned;
    s->value.s_symbol.ptr = ptr;
    s->value.s_symbol.len = len;
    return s;
}

GtkMl_S *gtk_ml_new_keyword(GtkMl_Context *ctx, GtkMl_Span *span, gboolean owned, const char *ptr, size_t len) {
    GtkMl_S *s = gtk_ml_new_value(ctx, span, GTKML_S_KEYWORD);
    s->value.s_keyword.owned = owned;
    s->value.s_keyword.ptr = ptr;
    s->value.s_keyword.len = len;
    return s;
}

GtkMl_S *gtk_ml_new_list(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *car, GtkMl_S *cdr) {
    GtkMl_S *s = gtk_ml_new_value(ctx, span, GTKML_S_LIST);
    s->value.s_list.car = car;
    s->value.s_list.cdr = cdr;
    return s;
}

GtkMl_S *gtk_ml_new_map(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *metamap) {
    GtkMl_S *s = gtk_ml_new_value(ctx, span, GTKML_S_MAP);
    gtk_ml_new_hash_trie(&s->value.s_map.map, &GTKML_DEFAULT_HASHER);
    gtk_ml_setmetamap(s, metamap);
    return s;
}

GtkMl_S *gtk_ml_new_set(GtkMl_Context *ctx, GtkMl_Span *span) {
    GtkMl_S *s = gtk_ml_new_value(ctx, span, GTKML_S_SET);
    gtk_ml_new_hash_set(&s->value.s_set.set, &GTKML_DEFAULT_HASHER);
    return s;
}

GtkMl_S *gtk_ml_new_array(GtkMl_Context *ctx, GtkMl_Span *span) {
    GtkMl_S *s = gtk_ml_new_value(ctx, span, GTKML_S_ARRAY);
    gtk_ml_new_array_trie(&s->value.s_array.array);
    return s;
}

GtkMl_S *gtk_ml_new_var(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *expr) {
    GtkMl_S *s = gtk_ml_new_value(ctx, span, GTKML_S_VAR);
    s->value.s_var.expr = expr;
    return s;
}

GtkMl_S *gtk_ml_new_vararg(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *expr) {
    GtkMl_S *s = gtk_ml_new_value(ctx, span, GTKML_S_VARARG);
    s->value.s_vararg.expr = expr;
    return s;
}

GtkMl_S *gtk_ml_new_quote(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *expr) {
    GtkMl_S *s = gtk_ml_new_value(ctx, span, GTKML_S_QUOTE);
    s->value.s_vararg.expr = expr;
    return s;
}

GtkMl_S *gtk_ml_new_quasiquote(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *expr) {
    GtkMl_S *s = gtk_ml_new_value(ctx, span, GTKML_S_QUASIQUOTE);
    s->value.s_vararg.expr = expr;
    return s;
}

GtkMl_S *gtk_ml_new_unquote(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *expr) {
    GtkMl_S *s = gtk_ml_new_value(ctx, span, GTKML_S_UNQUOTE);
    s->value.s_vararg.expr = expr;
    return s;
}

GtkMl_S *gtk_ml_new_lambda(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *args, GtkMl_S *body, GtkMl_S *capture) {
    GtkMl_S *s = gtk_ml_new_value(ctx, span, GTKML_S_LAMBDA);
    s->value.s_lambda.args = args;
    s->value.s_lambda.body = body;
    s->value.s_lambda.capture = capture;
    return s;
}

GtkMl_S *gtk_ml_new_program(GtkMl_Context *ctx, GtkMl_Span *span, const char *linkage_name, uint64_t addr, GtkMl_S *args, GtkMl_S *body, GtkMl_S *capture, GtkMl_ProgramKind kind) {
    GtkMl_S *s = gtk_ml_new_value(ctx, span, GTKML_S_PROGRAM);
    s->value.s_program.linkage_name = gtk_ml_new_string(ctx, span, linkage_name, strlen(linkage_name));
    s->value.s_program.addr = addr;
    s->value.s_program.args = args;
    s->value.s_program.body = body;
    s->value.s_program.capture = capture;
    s->value.s_program.kind = kind;
    return s;
}

GtkMl_S *gtk_ml_new_address(GtkMl_Context *ctx, GtkMl_Span *span, const char *linkage_name, uint64_t addr) {
    GtkMl_S *s = gtk_ml_new_value(ctx, span, GTKML_S_ADDRESS);
    s->value.s_address.linkage_name = gtk_ml_new_string(ctx, span, linkage_name, strlen(linkage_name));
    s->value.s_address.addr = addr;
    return s;
}

GtkMl_S *gtk_ml_new_macro(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *args, GtkMl_S *body, GtkMl_S *capture) {
    GtkMl_S *s = gtk_ml_new_value(ctx, span, GTKML_S_MACRO);
    s->value.s_macro.args = args;
    s->value.s_macro.body = body;
    s->value.s_macro.capture = capture;
    return s;
}

GtkMl_S *gtk_ml_new_lightdata(GtkMl_Context *ctx, GtkMl_Span *span, void *data) {
    GtkMl_S *s = gtk_ml_new_value(ctx, span, GTKML_S_LIGHTDATA);
    s->value.s_lightdata.userdata = data;
    return s;
}

GtkMl_S *gtk_ml_new_userdata(GtkMl_Context *ctx, GtkMl_Span *span, void *data, void (*del)(GtkMl_Context *ctx, void *)) {
    GtkMl_S *s = gtk_ml_new_value(ctx, span, GTKML_S_USERDATA);
    s->value.s_userdata.userdata = data;
    s->value.s_userdata.del = del;
    s->value.s_userdata.keep = gtk_ml_new_nil(ctx, span);
    return s;
}
