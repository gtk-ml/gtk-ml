#include <string.h>
#include <dlfcn.h>
#include "gtk-ml.h"

static void *LIB;
static int (*foo)(int);

static void call_foo(GtkMl_TaggedValue *result, GtkMl_Context *ctx, GtkMl_SObj *err) {
    (void) err;

    GtkMl_TaggedValue n_args = gtk_ml_pop(ctx);
    (void) n_args;

    GtkMl_TaggedValue arg0 = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    if (!gtk_ml_has_value(arg0)) {
        *result = gtk_ml_value_none();
        return;
    }

    *result = gtk_ml_value_int(foo(arg0.value.s64));
}

void gtk_ml_init(GtkMl_TaggedValue *result, GtkMl_Context *ctx, GtkMl_SObj *err) {
    LIB = dlopen("source.so", RTLD_LAZY | RTLD_LOCAL);
    char *error;

    if (!LIB) {
        error = dlerror();
        *err = gtk_ml_error(ctx, "dynlib-error", error, 0, 0, 0, 0);
        *result = gtk_ml_value_none();
        return;
    }

    *(void **) (&foo) = dlsym(LIB, "foo");

    error = dlerror();
    if (error) {
        *err = gtk_ml_error(ctx, "dynlib-error", error, 0, 0, 0, 0);
        *result = gtk_ml_value_none();
        return;
    }

    GtkMl_TaggedValue n_args = gtk_ml_pop(ctx);
    (void) n_args;

    GtkMl_TaggedValue key = gtk_ml_value_sobject(gtk_ml_new_symbol(ctx, NULL, 0, "foo", strlen("foo")));
    GtkMl_TaggedValue foo = gtk_ml_value_ffi(call_foo);
    GtkMl_SObj empty = gtk_ml_new_map(ctx, NULL, NULL);
    GtkMl_SObj module = gtk_ml_new_map(ctx, NULL, NULL);
    gtk_ml_hash_trie_insert(&module->value.s_map.map, &empty->value.s_map.map, key, foo);
    *result = gtk_ml_value_sobject(module);
}
