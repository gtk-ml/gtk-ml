#include <stdlib.h>
#include <string.h>
#ifdef GTKML_ENABLE_GTK
#include <gtk/gtk.h>
#endif /* GTKML_ENABLE_GTK */
#include <math.h>
#define GTKML_INCLUDE_INTERNAL
#include "gtk-ml.h"
#include "gtk-ml-internal.h"

GTKML_PRIVATE const char *TYPENAME[] = {
    [GTKML_S_NIL] = "nil",
    [GTKML_S_FALSE] = "bool",
    [GTKML_S_TRUE] = "bool",
    [GTKML_S_INT] = "int",
    [GTKML_S_FLOAT] = "float",
    [GTKML_S_CHAR] = "char",
    [GTKML_S_SYMBOL] = "symbol",
    [GTKML_S_KEYWORD] = "keyword",
    [GTKML_S_LIST] = "list",
    [GTKML_S_MAP] = "map",
    [GTKML_S_SET] = "set",
    [GTKML_S_ARRAY] = "array",
    [GTKML_S_VAR] = "var",
    [GTKML_S_VARARG] = "vararg",
    [GTKML_S_QUOTE] = "quote",
    [GTKML_S_QUASIQUOTE] = "quasiquote",
    [GTKML_S_UNQUOTE] = "unquote",
    [GTKML_S_LAMBDA] = "lambda",
    [GTKML_S_PROGRAM] = "program",
    [GTKML_S_ADDRESS] = "address",
    [GTKML_S_MACRO] = "macro",
    [GTKML_S_LIGHTDATA] = "lightdata",
    [GTKML_S_USERDATA] = "userdata",
    [GTKML_S_FFI] = "ffi",
};

GTKML_PRIVATE const char *PRIMNAME[] = {
    [GTKML_TAG_NIL] = "nil",
    [GTKML_TAG_BOOL] = "bool",
    [GTKML_TAG_INT] = "int",
    [GTKML_TAG_FLOAT] = "float",
    [GTKML_TAG_CHAR] = "char",
    [GTKML_TAG_USERDATA] = "lightdata",
    [GTKML_TAG_FFI] = "ffi",
};

GTKML_PRIVATE GtkMl_TaggedValue gtk_ml_get_data(GtkMl_Program *program, GtkMl_Data data) {
    GtkMl_TaggedValue value = program->data[data];
    if (!gtk_ml_has_value(value)) {
        return gtk_ml_value_none();
    } else if (gtk_ml_is_primitive(value)) {
        return value;
    } else {
        return gtk_ml_value_sobject(program->statics[value.value.u64]);
    }
}

#define ENTER(vm, gc) \
    do { \
        if (vm->base_stack_ptr == vm->base_stack_cap) { \
            vm->base_stack_cap *= 2; \
            vm->base_stack = realloc(vm->base_stack, sizeof(uint64_t) * vm->base_stack_cap); \
        } \
        vm->base_stack[vm->base_stack_ptr++] = vm->local_base; \
        vm->local_base = vm->local_len; \
        gc->base_stack[gc->base_stack_ptr++] = gc->local_base; \
        gc->local_base = gc->local_len; \
    } while (0)

#define LEAVE(vm, gc) \
    do { \
        vm->local_len = vm->local_base; \
        vm->local_base = vm->base_stack[--vm->base_stack_ptr]; \
        gc->local_len = gc->local_base; \
        gc->local_base = gc->base_stack[--gc->base_stack_ptr]; \
    } while (0)

#define PC_INCREMENT vm->pc += 8

gboolean gtk_ml_i_nop(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;
    PC_INCREMENT;
    return 1;
}

GTKML_PRIVATE void set_flags(GtkMl_Vm *vm, GtkMl_TaggedValue result) {
    // TODO(walterpi): overflow, carry
    if (gtk_ml_is_primitive(result)) {
        if ((result.tag & GTKML_TAG_INT) == GTKML_TAG_INT) { \
            if (result.value.s64 == 0) {
                vm->flags |= GTKML_F_EQUAL;
            } else {
                vm->flags |= GTKML_F_NEQUAL;
            }
        } else if (result.tag == GTKML_TAG_FLOAT) { \
            if (result.value.flt == 0.0) {
                vm->flags |= GTKML_F_EQUAL;
            } else {
                vm->flags |= GTKML_F_NEQUAL;
            }
        }
    } else {
        if (result.value.sobj->kind == GTKML_S_INT) {
            if (result.value.sobj->value.s_int.value == 0) {
                vm->flags |= GTKML_F_EQUAL;
            } else {
                vm->flags |= GTKML_F_NEQUAL;
            }
        } else if (result.value.sobj->kind == GTKML_S_FLOAT) {
            if (result.value.sobj->value.s_float.value == 0.0) {
                vm->flags |= GTKML_F_EQUAL;
            } else {
                vm->flags |= GTKML_F_NEQUAL;
            }
        }
    }
}

#define gtk_ml_i_signed_binary(name, operator, metasym) \
    gboolean gtk_ml_i_signed_##name(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) { \
        (void) data; \
        GtkMl_TaggedValue tv_lhs = gtk_ml_pop(vm->ctx); \
        GtkMl_TaggedValue tv_rhs = gtk_ml_pop(vm->ctx); \
        double flhs; \
        double frhs; \
        int64_t ilhs; \
        int64_t irhs; \
        gboolean sobj = gtk_ml_is_sobject(tv_lhs) || gtk_ml_is_sobject(tv_rhs); \
        gboolean try_mm = 0; \
        gboolean flt_lhs; \
        gboolean flt_rhs; \
        if (gtk_ml_is_primitive(tv_lhs)) { \
            if ((tv_lhs.tag & GTKML_TAG_INT) == GTKML_TAG_INT) { \
                ilhs = tv_lhs.value.s64; \
                flt_lhs = 0; \
            } else if (tv_lhs.tag == GTKML_TAG_FLOAT) { \
                flhs = tv_lhs.value.flt; \
                flt_lhs = 1; \
            } else { \
                try_mm = 1; \
            } \
        } else if (tv_lhs.value.sobj->kind == GTKML_S_INT) { \
            ilhs = tv_lhs.value.sobj->value.s_int.value; \
            flt_lhs = 0; \
        } else if (tv_lhs.value.sobj->kind == GTKML_S_FLOAT) { \
            flhs = tv_lhs.value.sobj->value.s_float.value; \
            flt_lhs = 1; \
        } else { \
            try_mm = 1; \
        } \
        if (gtk_ml_is_primitive(tv_rhs)) { \
            if ((tv_rhs.tag & GTKML_TAG_INT) == GTKML_TAG_INT) { \
                irhs = tv_rhs.value.s64; \
                flt_rhs = 0; \
            } else if (tv_rhs.tag == GTKML_TAG_FLOAT) { \
                frhs = tv_rhs.value.flt; \
                flt_rhs = 1; \
            } else { \
                try_mm = 1; \
            } \
        } else if (tv_rhs.value.sobj->kind == GTKML_S_INT) { \
            irhs = tv_rhs.value.sobj->value.s_int.value; \
            flt_rhs = 0; \
        } else if (tv_rhs.value.sobj->kind == GTKML_S_FLOAT) { \
            frhs = tv_rhs.value.sobj->value.s_float.value; \
            flt_rhs = 1; \
        } else { \
            try_mm = 1; \
        } \
        gboolean mm_success = 0; \
        GtkMl_TaggedValue metaargs[2]; \
        GtkMl_TaggedValue metakey; \
        if (try_mm) { \
            metaargs[0] = tv_lhs; \
            metaargs[1] = tv_rhs; \
            metakey = gtk_ml_value_sobject(gtk_ml_new_keyword(vm->ctx, NULL, 0, metasym, strlen(metasym))); \
            if (gtk_ml_is_sobject(tv_lhs)) { \
                if (tv_lhs.value.sobj->kind == GTKML_S_MAP) { \
                    GtkMl_TaggedValue metafn = gtk_ml_value_none(); \
                    if (tv_lhs.value.sobj->value.s_map.metamap) { \
                        metafn = gtk_ml_hash_trie_get(&tv_lhs.value.sobj->value.s_map.metamap->value.s_map.map, metakey); \
                    } \
                    if (gtk_ml_has_value(metafn)) { \
                        if (gtk_ml_is_sobject(metafn)) { \
                            if (!gtk_ml_execute(vm->ctx, err, metafn, metaargs, 2)) { \
                                return 0; \
                            } \
                            mm_success = 1; \
                            try_mm = 0; \
                        } else { \
                            GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 1, \
                                gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "function", strlen("function"))); \
                            *err = error; \
                            return 0; \
                        } \
                    } \
                } \
            } \
        } \
        if (try_mm) { \
            if (gtk_ml_is_sobject(tv_rhs)) { \
                if (tv_rhs.value.sobj->kind == GTKML_S_MAP) { \
                    GtkMl_TaggedValue metafn = gtk_ml_value_none(); \
                    if (tv_rhs.value.sobj->value.s_map.metamap) { \
                        metafn = gtk_ml_hash_trie_get(&tv_rhs.value.sobj->value.s_map.metamap->value.s_map.map, metakey); \
                    } \
                    if (gtk_ml_has_value(metafn)) { \
                        if (gtk_ml_is_sobject(metafn)) { \
                            if (!gtk_ml_execute(vm->ctx, err, metafn, metaargs, 2)) { \
                                return 0; \
                            } \
                            mm_success = 1; \
                            try_mm = 0; \
                        } else { \
                            GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 1, \
                                gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "function", strlen("function"))); \
                            *err = error; \
                            return 0; \
                        } \
                    } \
                } \
            } \
        } \
        if (!mm_success) { \
            if (!flt_lhs && !flt_rhs) { \
                if (sobj) { \
                    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_int(vm->ctx, NULL, ilhs operator irhs))); \
                } else { \
                    gtk_ml_push(vm->ctx, gtk_ml_value_int(ilhs operator irhs)); \
                } \
            } else if (!flt_lhs && flt_rhs) { \
                if (sobj) { \
                    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_float(vm->ctx, NULL, (double) ilhs operator frhs))); \
                } else { \
                    gtk_ml_push(vm->ctx, gtk_ml_value_float((double) ilhs operator frhs)); \
                } \
            } else if (flt_lhs && !flt_rhs) { \
                if (sobj) { \
                    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_float(vm->ctx, NULL, flhs operator (double) irhs))); \
                } else { \
                    gtk_ml_push(vm->ctx, gtk_ml_value_float(flhs operator (double) irhs)); \
                } \
            } else if (flt_lhs && flt_rhs) { \
                if (sobj) { \
                    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_float(vm->ctx, NULL, flhs operator frhs))); \
                } else { \
                    gtk_ml_push(vm->ctx, gtk_ml_value_float(flhs operator frhs)); \
                } \
            } else { \
                *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0); \
                return 0; \
            } \
            set_flags(vm, gtk_ml_peek(vm->ctx)); \
        } \
        PC_INCREMENT; \
        return 1; \
    }

#define gtk_ml_i_unsigned_binary(name, operator, metasym) \
    gboolean gtk_ml_i_unsigned_##name(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) { \
        (void) data; \
        GtkMl_TaggedValue tv_lhs = gtk_ml_pop(vm->ctx); \
        GtkMl_TaggedValue tv_rhs = gtk_ml_pop(vm->ctx); \
        double flhs; \
        double frhs; \
        uint64_t ilhs; \
        uint64_t irhs; \
        gboolean sobj = gtk_ml_is_sobject(tv_lhs) || gtk_ml_is_sobject(tv_rhs); \
        gboolean try_mm = 0; \
        gboolean flt_lhs; \
        gboolean flt_rhs; \
        if (gtk_ml_is_primitive(tv_lhs)) { \
            if ((tv_lhs.tag & GTKML_TAG_INT) == GTKML_TAG_INT) { \
                ilhs = tv_lhs.value.s64; \
                flt_lhs = 0; \
            } else if (tv_lhs.tag == GTKML_TAG_FLOAT) { \
                flhs = tv_lhs.value.flt; \
                flt_lhs = 1; \
            } else { \
                try_mm = 1; \
            } \
        } else if (tv_lhs.value.sobj->kind == GTKML_S_INT) { \
            ilhs = tv_lhs.value.sobj->value.s_int.value; \
            flt_lhs = 0; \
        } else if (tv_lhs.value.sobj->kind == GTKML_S_FLOAT) { \
            flhs = tv_lhs.value.sobj->value.s_float.value; \
            flt_lhs = 1; \
        } else { \
            try_mm = 1; \
        } \
        if (gtk_ml_is_primitive(tv_rhs)) { \
            if ((tv_rhs.tag & GTKML_TAG_INT) == GTKML_TAG_INT) { \
                irhs = tv_rhs.value.s64; \
                flt_rhs = 0; \
            } else if (tv_rhs.tag == GTKML_TAG_FLOAT) { \
                frhs = tv_rhs.value.flt; \
                flt_rhs = 1; \
            } else { \
                try_mm = 1; \
            } \
        } else if (tv_rhs.value.sobj->kind == GTKML_S_INT) { \
            irhs = tv_rhs.value.sobj->value.s_int.value; \
            flt_rhs = 0; \
        } else if (tv_rhs.value.sobj->kind == GTKML_S_FLOAT) { \
            frhs = tv_rhs.value.sobj->value.s_float.value; \
            flt_rhs = 1; \
        } else { \
            try_mm = 1; \
        } \
        gboolean mm_success = 0; \
        GtkMl_TaggedValue metaargs[2]; \
        GtkMl_TaggedValue metakey; \
        if (try_mm) { \
            metaargs[0] = tv_lhs; \
            metaargs[1] = tv_rhs; \
            metakey = gtk_ml_value_sobject(gtk_ml_new_keyword(vm->ctx, NULL, 0, metasym, strlen(metasym))); \
            if (gtk_ml_is_sobject(tv_lhs)) { \
                if (tv_lhs.value.sobj->kind == GTKML_S_MAP) { \
                    GtkMl_TaggedValue metafn = gtk_ml_value_none(); \
                    if (tv_lhs.value.sobj->value.s_map.metamap) { \
                        metafn = gtk_ml_hash_trie_get(&tv_lhs.value.sobj->value.s_map.metamap->value.s_map.map, metakey); \
                    } \
                    if (gtk_ml_has_value(metafn)) { \
                        if (gtk_ml_is_sobject(metafn)) { \
                            if (!gtk_ml_execute(vm->ctx, err, metafn, metaargs, 2)) { \
                                return 0; \
                            } \
                            mm_success = 1; \
                            try_mm = 0; \
                        } else { \
                            GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 1, \
                                gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "function", strlen("function"))); \
                            *err = error; \
                            return 0; \
                        } \
                    } \
                } \
            } \
        } \
        if (try_mm) { \
            if (gtk_ml_is_sobject(tv_rhs)) { \
                if (tv_rhs.value.sobj->kind == GTKML_S_MAP) { \
                    GtkMl_TaggedValue metafn = gtk_ml_value_none(); \
                    if (tv_rhs.value.sobj->value.s_map.metamap) { \
                        metafn = gtk_ml_hash_trie_get(&tv_rhs.value.sobj->value.s_map.metamap->value.s_map.map, metakey); \
                    } \
                    if (gtk_ml_has_value(metafn)) { \
                        if (gtk_ml_is_sobject(metafn)) { \
                            if (!gtk_ml_execute(vm->ctx, err, metafn, metaargs, 2)) { \
                                return 0; \
                            } \
                            mm_success = 1; \
                            try_mm = 0; \
                        } else { \
                            GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 1, \
                                gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "function", strlen("function"))); \
                            *err = error; \
                            return 0; \
                        } \
                    } \
                } \
            } \
        } \
        if (!mm_success) { \
            if (!flt_lhs && !flt_rhs) { \
                if (sobj) { \
                    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_int(vm->ctx, NULL, ilhs operator irhs))); \
                } else { \
                    gtk_ml_push(vm->ctx, gtk_ml_value_int(ilhs operator irhs)); \
                } \
            } else if (!flt_lhs && flt_rhs) { \
                if (sobj) { \
                    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_float(vm->ctx, NULL, (double) ilhs operator frhs))); \
                } else { \
                    gtk_ml_push(vm->ctx, gtk_ml_value_float((double) ilhs operator frhs)); \
                } \
            } else if (flt_lhs && !flt_rhs) { \
                if (sobj) { \
                    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_float(vm->ctx, NULL, flhs operator (double) irhs))); \
                } else { \
                    gtk_ml_push(vm->ctx, gtk_ml_value_float(flhs operator (double) irhs)); \
                } \
            } else if (flt_lhs && flt_rhs) { \
                if (sobj) { \
                    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_float(vm->ctx, NULL, flhs operator frhs))); \
                } else { \
                    gtk_ml_push(vm->ctx, gtk_ml_value_float(flhs operator frhs)); \
                } \
            } else { \
                *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0); \
                return 0; \
            } \
            set_flags(vm, gtk_ml_peek(vm->ctx)); \
        } \
        PC_INCREMENT; \
        return 1; \
    }

#define gtk_ml_i_signed_binaryf(name, operator, fn, metasym) \
    gboolean gtk_ml_i_signed_##name(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) { \
        (void) data; \
        GtkMl_TaggedValue tv_lhs = gtk_ml_pop(vm->ctx); \
        GtkMl_TaggedValue tv_rhs = gtk_ml_pop(vm->ctx); \
        double flhs; \
        double frhs; \
        int64_t ilhs; \
        int64_t irhs; \
        gboolean sobj = gtk_ml_is_sobject(tv_lhs) || gtk_ml_is_sobject(tv_rhs); \
        gboolean try_mm = 0; \
        gboolean flt_lhs; \
        gboolean flt_rhs; \
        if (gtk_ml_is_primitive(tv_lhs)) { \
            if ((tv_lhs.tag & GTKML_TAG_INT) == GTKML_TAG_INT) { \
                ilhs = tv_lhs.value.s64; \
                flt_lhs = 0; \
            } else if (tv_lhs.tag == GTKML_TAG_FLOAT) { \
                flhs = tv_lhs.value.flt; \
                flt_lhs = 1; \
            } else { \
                try_mm = 1; \
            } \
        } else if (tv_lhs.value.sobj->kind == GTKML_S_INT) { \
            ilhs = tv_lhs.value.sobj->value.s_int.value; \
            flt_lhs = 0; \
        } else if (tv_lhs.value.sobj->kind == GTKML_S_FLOAT) { \
            flhs = tv_lhs.value.sobj->value.s_float.value; \
            flt_lhs = 1; \
        } else { \
            try_mm = 1; \
        } \
        if (gtk_ml_is_primitive(tv_rhs)) { \
            if ((tv_rhs.tag & GTKML_TAG_INT) == GTKML_TAG_INT) { \
                irhs = tv_rhs.value.s64; \
                flt_rhs = 0; \
            } else if (tv_rhs.tag == GTKML_TAG_FLOAT) { \
                frhs = tv_rhs.value.flt; \
                flt_rhs = 1; \
            } else { \
                try_mm = 1; \
            } \
        } else if (tv_rhs.value.sobj->kind == GTKML_S_INT) { \
            irhs = tv_rhs.value.sobj->value.s_int.value; \
            flt_rhs = 0; \
        } else if (tv_rhs.value.sobj->kind == GTKML_S_FLOAT) { \
            frhs = tv_rhs.value.sobj->value.s_float.value; \
            flt_rhs = 1; \
        } else { \
            try_mm = 1; \
        } \
        gboolean mm_success = 0; \
        GtkMl_TaggedValue metaargs[2]; \
        GtkMl_TaggedValue metakey; \
        if (try_mm) { \
            metaargs[0] = tv_lhs; \
            metaargs[1] = tv_rhs; \
            metakey = gtk_ml_value_sobject(gtk_ml_new_keyword(vm->ctx, NULL, 0, metasym, strlen(metasym))); \
            if (gtk_ml_is_sobject(tv_lhs)) { \
                if (tv_lhs.value.sobj->kind == GTKML_S_MAP) { \
                    GtkMl_TaggedValue metafn = gtk_ml_value_none(); \
                    if (tv_lhs.value.sobj->value.s_map.metamap) { \
                        metafn = gtk_ml_hash_trie_get(&tv_lhs.value.sobj->value.s_map.metamap->value.s_map.map, metakey); \
                    } \
                    if (gtk_ml_has_value(metafn)) { \
                        if (gtk_ml_is_sobject(metafn)) { \
                            if (!gtk_ml_execute(vm->ctx, err, metafn, metaargs, 2)) { \
                                return 0; \
                            } \
                            mm_success = 1; \
                            try_mm = 0; \
                        } else { \
                            GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 1, \
                                gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "function", strlen("function"))); \
                            *err = error; \
                            return 0; \
                        } \
                    } \
                } \
            } \
        } \
        if (try_mm) { \
            if (gtk_ml_is_sobject(tv_rhs)) { \
                if (tv_rhs.value.sobj->kind == GTKML_S_MAP) { \
                    GtkMl_TaggedValue metafn = gtk_ml_value_none(); \
                    if (tv_rhs.value.sobj->value.s_map.metamap) { \
                        metafn = gtk_ml_hash_trie_get(&tv_rhs.value.sobj->value.s_map.metamap->value.s_map.map, metakey); \
                    } \
                    if (gtk_ml_has_value(metafn)) { \
                        if (gtk_ml_is_sobject(metafn)) { \
                            if (!gtk_ml_execute(vm->ctx, err, metafn, metaargs, 2)) { \
                                return 0; \
                            } \
                            mm_success = 1; \
                            try_mm = 0; \
                        } else { \
                            GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 1, \
                                gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "function", strlen("function"))); \
                            *err = error; \
                            return 0; \
                        } \
                    } \
                } \
            } \
        } \
        if (!mm_success) { \
            if (!flt_lhs && !flt_rhs) { \
                if (sobj) { \
                    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_int(vm->ctx, NULL, ilhs operator irhs))); \
                } else { \
                    gtk_ml_push(vm->ctx, gtk_ml_value_int(ilhs operator irhs)); \
                } \
            } else if (!flt_lhs && flt_rhs) { \
                if (sobj) { \
                    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_float(vm->ctx, NULL, fn((double) ilhs, frhs)))); \
                } else { \
                    gtk_ml_push(vm->ctx, gtk_ml_value_float(fn((double) ilhs, frhs))); \
                } \
            } else if (flt_lhs && !flt_rhs) { \
                if (sobj) { \
                    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_float(vm->ctx, NULL, fn(flhs, (double) irhs)))); \
                } else { \
                    gtk_ml_push(vm->ctx, gtk_ml_value_float(fn(flhs, (double) irhs))); \
                } \
            } else if (flt_lhs && flt_rhs) { \
                if (sobj) { \
                    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_float(vm->ctx, NULL, fn(flhs, frhs)))); \
                } else { \
                    gtk_ml_push(vm->ctx, gtk_ml_value_float(fn(flhs, frhs))); \
                } \
            } else { \
                *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0); \
                return 0; \
            } \
            set_flags(vm, gtk_ml_peek(vm->ctx)); \
        } \
        PC_INCREMENT; \
        return 1; \
    }

#define gtk_ml_i_unsigned_binaryf(name, operator, fn, metasym) \
    gboolean gtk_ml_i_unsigned_##name(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) { \
        (void) data; \
        GtkMl_TaggedValue tv_lhs = gtk_ml_pop(vm->ctx); \
        GtkMl_TaggedValue tv_rhs = gtk_ml_pop(vm->ctx); \
        double flhs; \
        double frhs; \
        uint64_t ilhs; \
        uint64_t irhs; \
        gboolean sobj = gtk_ml_is_sobject(tv_lhs) || gtk_ml_is_sobject(tv_rhs); \
        gboolean try_mm = 0; \
        gboolean flt_lhs; \
        gboolean flt_rhs; \
        if (gtk_ml_is_primitive(tv_lhs)) { \
            if ((tv_lhs.tag & GTKML_TAG_INT) == GTKML_TAG_INT) { \
                ilhs = tv_lhs.value.s64; \
                flt_lhs = 0; \
            } else if (tv_lhs.tag == GTKML_TAG_FLOAT) { \
                flhs = tv_lhs.value.flt; \
                flt_lhs = 1; \
            } else { \
                try_mm = 1; \
            } \
        } else if (tv_lhs.value.sobj->kind == GTKML_S_INT) { \
            ilhs = tv_lhs.value.sobj->value.s_int.value; \
            flt_lhs = 0; \
        } else if (tv_lhs.value.sobj->kind == GTKML_S_FLOAT) { \
            flhs = tv_lhs.value.sobj->value.s_float.value; \
            flt_lhs = 1; \
        } else { \
            try_mm = 1; \
        } \
        if (gtk_ml_is_primitive(tv_rhs)) { \
            if ((tv_rhs.tag & GTKML_TAG_INT) == GTKML_TAG_INT) { \
                irhs = tv_rhs.value.s64; \
                flt_rhs = 0; \
            } else if (tv_rhs.tag == GTKML_TAG_FLOAT) { \
                frhs = tv_rhs.value.flt; \
                flt_rhs = 1; \
            } else { \
                try_mm = 1; \
            } \
        } else if (tv_rhs.value.sobj->kind == GTKML_S_INT) { \
            irhs = tv_rhs.value.sobj->value.s_int.value; \
            flt_rhs = 0; \
        } else if (tv_rhs.value.sobj->kind == GTKML_S_FLOAT) { \
            frhs = tv_rhs.value.sobj->value.s_float.value; \
            flt_rhs = 1; \
        } else { \
            try_mm = 1; \
        } \
        gboolean mm_success = 0; \
        GtkMl_TaggedValue metaargs[2]; \
        GtkMl_TaggedValue metakey; \
        if (try_mm) { \
            metaargs[0] = tv_lhs; \
            metaargs[1] = tv_rhs; \
            metakey = gtk_ml_value_sobject(gtk_ml_new_keyword(vm->ctx, NULL, 0, metasym, strlen(metasym))); \
            if (gtk_ml_is_sobject(tv_lhs)) { \
                if (tv_lhs.value.sobj->kind == GTKML_S_MAP) { \
                    GtkMl_TaggedValue metafn = gtk_ml_value_none(); \
                    if (tv_lhs.value.sobj->value.s_map.metamap) { \
                        metafn = gtk_ml_hash_trie_get(&tv_lhs.value.sobj->value.s_map.metamap->value.s_map.map, metakey); \
                    } \
                    if (gtk_ml_has_value(metafn)) { \
                        if (gtk_ml_is_sobject(metafn)) { \
                            if (!gtk_ml_execute(vm->ctx, err, metafn, metaargs, 2)) { \
                                return 0; \
                            } \
                            mm_success = 1; \
                            try_mm = 0; \
                        } else { \
                            GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 1, \
                                gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "function", strlen("function"))); \
                            *err = error; \
                            return 0; \
                        } \
                    } \
                } \
            } \
        } \
        if (try_mm) { \
            if (gtk_ml_is_sobject(tv_rhs)) { \
                if (tv_rhs.value.sobj->kind == GTKML_S_MAP) { \
                    GtkMl_TaggedValue metafn = gtk_ml_value_none(); \
                    if (tv_rhs.value.sobj->value.s_map.metamap) { \
                        metafn = gtk_ml_hash_trie_get(&tv_rhs.value.sobj->value.s_map.metamap->value.s_map.map, metakey); \
                    } \
                    if (gtk_ml_has_value(metafn)) { \
                        if (gtk_ml_is_sobject(metafn)) { \
                            if (!gtk_ml_execute(vm->ctx, err, metafn, metaargs, 2)) { \
                                return 0; \
                            } \
                            mm_success = 1; \
                            try_mm = 0; \
                        } else { \
                            GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 1, \
                                gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "function", strlen("function"))); \
                            *err = error; \
                            return 0; \
                        } \
                    } \
                } \
            } \
        } \
        if (!mm_success) { \
            if (!flt_lhs && !flt_rhs) { \
                if (sobj) { \
                    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_int(vm->ctx, NULL, ilhs operator irhs))); \
                } else { \
                    gtk_ml_push(vm->ctx, gtk_ml_value_int(ilhs operator irhs)); \
                } \
            } else if (!flt_lhs && flt_rhs) { \
                if (sobj) { \
                    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_float(vm->ctx, NULL, fn((double) ilhs, frhs)))); \
                } else { \
                    gtk_ml_push(vm->ctx, gtk_ml_value_float(fn((double) ilhs, frhs))); \
                } \
            } else if (flt_lhs && !flt_rhs) { \
                if (sobj) { \
                    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_float(vm->ctx, NULL, fn(flhs, (double) irhs)))); \
                } else { \
                    gtk_ml_push(vm->ctx, gtk_ml_value_float(fn(flhs, (double) irhs))); \
                } \
            } else if (flt_lhs && flt_rhs) { \
                if (sobj) { \
                    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_float(vm->ctx, NULL, fn(flhs, frhs)))); \
                } else { \
                    gtk_ml_push(vm->ctx, gtk_ml_value_float(fn(flhs, frhs))); \
                } \
            } else { \
                *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0); \
                return 0; \
            } \
            set_flags(vm, gtk_ml_peek(vm->ctx)); \
        } \
        PC_INCREMENT; \
        return 1; \
    }

#define gtk_ml_i_bitwise(name, operation, metasym) \
    gboolean gtk_ml_i_bit_##name(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) { \
        (void) data; \
        GtkMl_TaggedValue tv_lhs = gtk_ml_pop(vm->ctx); \
        GtkMl_TaggedValue tv_rhs = gtk_ml_pop(vm->ctx); \
        uint64_t ilhs; \
        uint64_t irhs; \
        gboolean sobj = gtk_ml_is_sobject(tv_lhs) || gtk_ml_is_sobject(tv_rhs); \
        gboolean try_mm = 0; \
        if (gtk_ml_is_primitive(tv_lhs)) { \
            if ((tv_lhs.tag & GTKML_TAG_INT) == GTKML_TAG_INT) { \
                ilhs = tv_lhs.value.s64; \
            } else { \
                try_mm = 1; \
            } \
        } else if (tv_lhs.value.sobj->kind == GTKML_S_INT) { \
            ilhs = tv_lhs.value.sobj->value.s_int.value; \
        } else { \
            try_mm = 1; \
        } \
        if (gtk_ml_is_primitive(tv_rhs)) { \
            if ((tv_rhs.tag & GTKML_TAG_INT) == GTKML_TAG_INT) { \
                irhs = tv_rhs.value.s64; \
            } else { \
                try_mm = 1; \
            } \
        } else if (tv_rhs.value.sobj->kind == GTKML_S_INT) { \
            irhs = tv_rhs.value.sobj->value.s_int.value; \
        } else { \
            try_mm = 1; \
        } \
        gboolean mm_success = 0; \
        GtkMl_TaggedValue metaargs[2]; \
        GtkMl_TaggedValue metakey; \
        if (try_mm) { \
            metaargs[0] = tv_lhs; \
            metaargs[1] = tv_rhs; \
            metakey = gtk_ml_value_sobject(gtk_ml_new_keyword(vm->ctx, NULL, 0, metasym, strlen(metasym))); \
            if (gtk_ml_is_sobject(tv_lhs)) { \
                if (tv_lhs.value.sobj->kind == GTKML_S_MAP) { \
                    GtkMl_TaggedValue metafn = gtk_ml_value_none(); \
                    if (tv_lhs.value.sobj->value.s_map.metamap) { \
                        metafn = gtk_ml_hash_trie_get(&tv_lhs.value.sobj->value.s_map.metamap->value.s_map.map, metakey); \
                    } \
                    if (gtk_ml_has_value(metafn)) { \
                        if (gtk_ml_is_sobject(metafn)) { \
                            if (!gtk_ml_execute(vm->ctx, err, metafn, metaargs, 2)) { \
                                return 0; \
                            } \
                            mm_success = 1; \
                            try_mm = 0; \
                        } else { \
                            GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 1, \
                                gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "function", strlen("function"))); \
                            *err = error; \
                            return 0; \
                        } \
                    } \
                } \
            } \
        } \
        if (try_mm) { \
            if (gtk_ml_is_sobject(tv_rhs)) { \
                if (tv_rhs.value.sobj->kind == GTKML_S_MAP) { \
                    GtkMl_TaggedValue metafn = gtk_ml_value_none(); \
                    if (tv_rhs.value.sobj->value.s_map.metamap) { \
                        metafn = gtk_ml_hash_trie_get(&tv_rhs.value.sobj->value.s_map.metamap->value.s_map.map, metakey); \
                    } \
                    if (gtk_ml_has_value(metafn)) { \
                        if (gtk_ml_is_sobject(metafn)) { \
                            if (!gtk_ml_execute(vm->ctx, err, metafn, metaargs, 2)) { \
                                return 0; \
                            } \
                            mm_success = 1; \
                            try_mm = 0; \
                        } else { \
                            GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 1, \
                                gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "function", strlen("function"))); \
                            *err = error; \
                            return 0; \
                        } \
                    } \
                } \
            } \
        } \
        if (!mm_success) { \
            if (sobj) { \
                gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_int(vm->ctx, NULL, operation))); \
            } else { \
                gtk_ml_push(vm->ctx, gtk_ml_value_int(operation)); \
            } \
            set_flags(vm, gtk_ml_peek(vm->ctx)); \
        } \
        PC_INCREMENT; \
        return 1; \
    }

gtk_ml_i_signed_binary(add, +, "__add")
gtk_ml_i_signed_binary(subtract, -, "__sub")
gtk_ml_i_signed_binary(multiply, *, "__smul")
gtk_ml_i_unsigned_binary(multiply, *, "__umul")
gtk_ml_i_signed_binary(divide, /, "__sdiv")
gtk_ml_i_unsigned_binary(divide, /, "__udiv")
gtk_ml_i_signed_binaryf(modulo, %, fmod, "__smod")
gtk_ml_i_unsigned_binaryf(modulo, %, fmod, "__umod")
gtk_ml_i_bitwise(and, ilhs & irhs, "__bitand")
gtk_ml_i_bitwise(or, ilhs | irhs, "__bitor")
gtk_ml_i_bitwise(xor, ilhs ^ irhs, "__bitxor")
gtk_ml_i_bitwise(nand, ~(ilhs & irhs), "__bitnand")
gtk_ml_i_bitwise(nor, ~(ilhs | irhs), "__bitnor")
gtk_ml_i_bitwise(xnor, ~(ilhs ^ irhs), "__bitxnor")

gboolean gtk_ml_i_cmp_imm(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    GtkMl_Cmp cmp = gtk_ml_get_data(vm->program, data).value.u64;

    GtkMl_TaggedValue tv_lhs = gtk_ml_pop(vm->ctx);
    GtkMl_TaggedValue tv_rhs = gtk_ml_pop(vm->ctx);

    double flhs;
    double frhs;
    int64_t ilhs;
    int64_t irhs;
    gboolean flt_lhs;
    gboolean flt_rhs;
    if (gtk_ml_is_primitive(tv_lhs)) {
        if ((tv_lhs.tag & GTKML_TAG_INT) == GTKML_TAG_INT) {
            ilhs = tv_lhs.value.s64;
            flt_lhs = 0;
        } else if (tv_lhs.tag == GTKML_TAG_FLOAT) {
            flhs = tv_lhs.value.flt;
            flt_lhs = 1;
        }
    } else if (tv_lhs.value.sobj->kind == GTKML_S_INT) {
        ilhs = tv_lhs.value.sobj->value.s_int.value;
        flt_lhs = 0;
    } else if (tv_lhs.value.sobj->kind == GTKML_S_FLOAT) {
        flhs = tv_lhs.value.sobj->value.s_float.value;
        flt_lhs = 1;
    }
    if (gtk_ml_is_primitive(tv_rhs)) {
        if ((tv_rhs.tag & GTKML_TAG_INT) == GTKML_TAG_INT) {
            irhs = tv_rhs.value.s64;
            flt_rhs = 0;
        } else if (tv_rhs.tag == GTKML_TAG_FLOAT) {
            frhs = tv_rhs.value.flt;
            flt_rhs = 1;
        }
    } else if (tv_rhs.value.sobj->kind == GTKML_S_INT) {
        irhs = tv_rhs.value.sobj->value.s_int.value;
        flt_rhs = 0;
    } else if (tv_rhs.value.sobj->kind == GTKML_S_FLOAT) {
        frhs = tv_rhs.value.sobj->value.s_float.value;
        flt_rhs = 1;
    }

    switch (cmp) {
    case GTKML_CMP_EQUAL: {
        gboolean mm_success = 0;
        if (gtk_ml_is_sobject(tv_lhs)) {
            if (tv_lhs.value.sobj->kind == GTKML_S_MAP) {
                GtkMl_TaggedValue metakey = gtk_ml_value_sobject(gtk_ml_new_keyword(vm->ctx, NULL, 0, "__eq", strlen("__eq")));
                GtkMl_TaggedValue metafn = gtk_ml_value_none();
                if (tv_lhs.value.sobj->value.s_map.metamap) {
                    metafn = gtk_ml_hash_trie_get(&tv_lhs.value.sobj->value.s_map.metamap->value.s_map.map, metakey);
                }
                if (gtk_ml_has_value(metafn)) {
                    if (gtk_ml_is_sobject(metafn)) {
                        GtkMl_TaggedValue metaargs[] = { tv_lhs, tv_rhs };
                        if (!gtk_ml_execute(vm->ctx, err, metafn, metaargs, 2)) {
                            return 0;
                        }
                        mm_success = 1;
                    } else {
                        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 1,
                            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "function", strlen("function")));
                        *err = error;
                        return 0;
                    }
                }
            }
        }
        if (!mm_success && gtk_ml_is_sobject(tv_rhs)) {
            if (tv_rhs.value.sobj->kind == GTKML_S_MAP) {
                GtkMl_TaggedValue metakey = gtk_ml_value_sobject(gtk_ml_new_keyword(vm->ctx, NULL, 0, "__eq", strlen("__eq")));
                GtkMl_TaggedValue metafn = gtk_ml_value_none();
                if (tv_rhs.value.sobj->value.s_map.metamap) {
                    metafn = gtk_ml_hash_trie_get(&tv_rhs.value.sobj->value.s_map.metamap->value.s_map.map, metakey);
                }
                if (gtk_ml_has_value(metafn)) {
                    if (gtk_ml_is_sobject(metafn)) {
                        GtkMl_TaggedValue metaargs[] = { tv_lhs, tv_rhs };
                        if (!gtk_ml_execute(vm->ctx, err, metafn, metaargs, 2)) {
                            return 0;
                        }
                        mm_success = 1;
                    } else {
                        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 1,
                            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "function", strlen("function")));
                        *err = error;
                        return 0;
                    }
                }
            }
        }
        if (!mm_success) {
            gtk_ml_push(vm->ctx, gtk_ml_equal_value(tv_lhs, tv_rhs)? gtk_ml_value_true() : gtk_ml_value_false());
        }
    } break;
    case GTKML_CMP_NOT_EQUAL: {
        gboolean mm_success = 0;
        if (gtk_ml_is_sobject(tv_lhs)) {
            if (tv_lhs.value.sobj->kind == GTKML_S_MAP) {
                GtkMl_TaggedValue metakey = gtk_ml_value_sobject(gtk_ml_new_keyword(vm->ctx, NULL, 0, "__ne", strlen("__ne")));
                GtkMl_TaggedValue metafn = gtk_ml_value_none();
                if (tv_lhs.value.sobj->value.s_map.metamap) {
                    metafn = gtk_ml_hash_trie_get(&tv_lhs.value.sobj->value.s_map.metamap->value.s_map.map, metakey);
                }
                if (gtk_ml_has_value(metafn)) {
                    if (gtk_ml_is_sobject(metafn)) {
                        GtkMl_TaggedValue metaargs[] = { tv_lhs, tv_rhs };
                        if (!gtk_ml_execute(vm->ctx, err, metafn, metaargs, 2)) {
                            return 0;
                        }
                        mm_success = 1;
                    } else {
                        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 1,
                            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "function", strlen("function")));
                        *err = error;
                        return 0;
                    }
                }
            }
        }
        if (!mm_success && gtk_ml_is_sobject(tv_rhs)) {
            if (tv_rhs.value.sobj->kind == GTKML_S_MAP) {
                GtkMl_TaggedValue metakey = gtk_ml_value_sobject(gtk_ml_new_keyword(vm->ctx, NULL, 0, "__ne", strlen("__ne")));
                GtkMl_TaggedValue metafn = gtk_ml_value_none();
                if (tv_rhs.value.sobj->value.s_map.metamap) {
                    metafn = gtk_ml_hash_trie_get(&tv_rhs.value.sobj->value.s_map.metamap->value.s_map.map, metakey);
                }
                if (gtk_ml_has_value(metafn)) {
                    if (gtk_ml_is_sobject(metafn)) {
                        GtkMl_TaggedValue metaargs[] = { tv_lhs, tv_rhs };
                        if (!gtk_ml_execute(vm->ctx, err, metafn, metaargs, 2)) {
                            return 0;
                        }
                        mm_success = 1;
                    } else {
                        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 1,
                            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "function", strlen("function")));
                        *err = error;
                        return 0;
                    }
                }
            }
        }
        if (!mm_success) {
            gtk_ml_push(vm->ctx, (!gtk_ml_equal_value(tv_lhs, tv_rhs))? gtk_ml_value_true() : gtk_ml_value_false());
        }
    } break;
    case GTKML_CMP_LESS: {
        gboolean mm_success = 0;
        if (gtk_ml_is_sobject(tv_lhs)) {
            if (tv_lhs.value.sobj->kind == GTKML_S_MAP) {
                GtkMl_TaggedValue metakey = gtk_ml_value_sobject(gtk_ml_new_keyword(vm->ctx, NULL, 0, "__lt", strlen("__lt")));
                GtkMl_TaggedValue metafn = gtk_ml_value_none();
                if (tv_lhs.value.sobj->value.s_map.metamap) {
                    metafn = gtk_ml_hash_trie_get(&tv_lhs.value.sobj->value.s_map.metamap->value.s_map.map, metakey);
                }
                if (gtk_ml_has_value(metafn)) {
                    if (gtk_ml_is_sobject(metafn)) {
                        GtkMl_TaggedValue metaargs[] = { tv_lhs, tv_rhs };
                        if (!gtk_ml_execute(vm->ctx, err, metafn, metaargs, 2)) {
                            return 0;
                        }
                        mm_success = 1;
                    } else {
                        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 1,
                            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "function", strlen("function")));
                        *err = error;
                        return 0;
                    }
                }
            }
        }
        if (!mm_success && gtk_ml_is_sobject(tv_rhs)) {
            if (tv_rhs.value.sobj->kind == GTKML_S_MAP) {
                GtkMl_TaggedValue metakey = gtk_ml_value_sobject(gtk_ml_new_keyword(vm->ctx, NULL, 0, "__lt", strlen("__lt")));
                GtkMl_TaggedValue metafn = gtk_ml_value_none();
                if (tv_rhs.value.sobj->value.s_map.metamap) {
                    metafn = gtk_ml_hash_trie_get(&tv_rhs.value.sobj->value.s_map.metamap->value.s_map.map, metakey);
                }
                if (gtk_ml_has_value(metafn)) {
                    if (gtk_ml_is_sobject(metafn)) {
                        GtkMl_TaggedValue metaargs[] = { tv_lhs, tv_rhs };
                        if (!gtk_ml_execute(vm->ctx, err, metafn, metaargs, 2)) {
                            return 0;
                        }
                        mm_success = 1;
                    } else {
                        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 1,
                            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "function", strlen("function")));
                        *err = error;
                        return 0;
                    }
                }
            }
        }
        if (!mm_success) {
            if (!flt_lhs && !flt_rhs) {
                gtk_ml_push(vm->ctx, (ilhs < irhs)? gtk_ml_value_true() : gtk_ml_value_false());
            } else if (!flt_lhs && flt_rhs) {
                gtk_ml_push(vm->ctx, ((double) ilhs < frhs)? gtk_ml_value_true() : gtk_ml_value_false());
            } else if (flt_lhs && !flt_rhs) {
                gtk_ml_push(vm->ctx, (flhs < (double) ilhs)? gtk_ml_value_true() : gtk_ml_value_false());
            } else if (flt_lhs && flt_rhs) {
                gtk_ml_push(vm->ctx, (flhs < frhs)? gtk_ml_value_true() : gtk_ml_value_false());
            } else {
                *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
                return 0;
            }
        }
    } break;
    case GTKML_CMP_GREATER: {
        gboolean mm_success = 0;
        if (gtk_ml_is_sobject(tv_lhs)) {
            if (tv_lhs.value.sobj->kind == GTKML_S_MAP) {
                GtkMl_TaggedValue metakey = gtk_ml_value_sobject(gtk_ml_new_keyword(vm->ctx, NULL, 0, "__gt", strlen("__gt")));
                GtkMl_TaggedValue metafn = gtk_ml_value_none();
                if (tv_lhs.value.sobj->value.s_map.metamap) {
                    metafn = gtk_ml_hash_trie_get(&tv_lhs.value.sobj->value.s_map.metamap->value.s_map.map, metakey);
                }
                if (gtk_ml_has_value(metafn)) {
                    if (gtk_ml_is_sobject(metafn)) {
                        GtkMl_TaggedValue metaargs[] = { tv_lhs, tv_rhs };
                        if (!gtk_ml_execute(vm->ctx, err, metafn, metaargs, 2)) {
                            return 0;
                        }
                        mm_success = 1;
                    } else {
                        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 1,
                            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "function", strlen("function")));
                        *err = error;
                        return 0;
                    }
                }
            }
        }
        if (!mm_success && gtk_ml_is_sobject(tv_rhs)) {
            if (tv_rhs.value.sobj->kind == GTKML_S_MAP) {
                GtkMl_TaggedValue metakey = gtk_ml_value_sobject(gtk_ml_new_keyword(vm->ctx, NULL, 0, "__gt", strlen("__gt")));
                GtkMl_TaggedValue metafn = gtk_ml_value_none();
                if (tv_rhs.value.sobj->value.s_map.metamap) {
                    metafn = gtk_ml_hash_trie_get(&tv_rhs.value.sobj->value.s_map.metamap->value.s_map.map, metakey);
                }
                if (gtk_ml_has_value(metafn)) {
                    if (gtk_ml_is_sobject(metafn)) {
                        GtkMl_TaggedValue metaargs[] = { tv_lhs, tv_rhs };
                        if (!gtk_ml_execute(vm->ctx, err, metafn, metaargs, 2)) {
                            return 0;
                        }
                        mm_success = 1;
                    } else {
                        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 1,
                            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "function", strlen("function")));
                        *err = error;
                        return 0;
                    }
                }
            }
        }
        if (!mm_success) {
            if (!flt_lhs && !flt_rhs) {
                gtk_ml_push(vm->ctx, (ilhs > irhs)? gtk_ml_value_true() : gtk_ml_value_false());
            } else if (!flt_lhs && flt_rhs) {
                gtk_ml_push(vm->ctx, ((double) ilhs > frhs)? gtk_ml_value_true() : gtk_ml_value_false());
            } else if (flt_lhs && !flt_rhs) {
                gtk_ml_push(vm->ctx, (flhs > (double) ilhs)? gtk_ml_value_true() : gtk_ml_value_false());
            } else if (flt_lhs && flt_rhs) {
                gtk_ml_push(vm->ctx, (flhs > frhs)? gtk_ml_value_true() : gtk_ml_value_false());
            } else {
                *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
                return 0;
            }
        }
    } break;
    case GTKML_CMP_LESS_EQUAL: {
        gboolean mm_success = 0;
        if (gtk_ml_is_sobject(tv_lhs)) {
            if (tv_lhs.value.sobj->kind == GTKML_S_MAP) {
                GtkMl_TaggedValue metakey = gtk_ml_value_sobject(gtk_ml_new_keyword(vm->ctx, NULL, 0, "__le", strlen("__le")));
                GtkMl_TaggedValue metafn = gtk_ml_value_none();
                if (tv_lhs.value.sobj->value.s_map.metamap) {
                    metafn = gtk_ml_hash_trie_get(&tv_lhs.value.sobj->value.s_map.metamap->value.s_map.map, metakey);
                }
                if (gtk_ml_has_value(metafn)) {
                    if (gtk_ml_is_sobject(metafn)) {
                        GtkMl_TaggedValue metaargs[] = { tv_lhs, tv_rhs };
                        if (!gtk_ml_execute(vm->ctx, err, metafn, metaargs, 2)) {
                            return 0;
                        }
                        mm_success = 1;
                    } else {
                        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 1,
                            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "function", strlen("function")));
                        *err = error;
                        return 0;
                    }
                }
            }
        }
        if (!mm_success && gtk_ml_is_sobject(tv_rhs)) {
            if (tv_rhs.value.sobj->kind == GTKML_S_MAP) {
                GtkMl_TaggedValue metakey = gtk_ml_value_sobject(gtk_ml_new_keyword(vm->ctx, NULL, 0, "__le", strlen("__le")));
                GtkMl_TaggedValue metafn = gtk_ml_value_none();
                if (tv_rhs.value.sobj->value.s_map.metamap) {
                    metafn = gtk_ml_hash_trie_get(&tv_rhs.value.sobj->value.s_map.metamap->value.s_map.map, metakey);
                }
                if (gtk_ml_has_value(metafn)) {
                    if (gtk_ml_is_sobject(metafn)) {
                        GtkMl_TaggedValue metaargs[] = { tv_lhs, tv_rhs };
                        if (!gtk_ml_execute(vm->ctx, err, metafn, metaargs, 2)) {
                            return 0;
                        }
                        mm_success = 1;
                    } else {
                        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 1,
                            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "function", strlen("function")));
                        *err = error;
                        return 0;
                    }
                }
            }
        }
        if (!mm_success) {
            if (!flt_lhs && !flt_rhs) {
                gtk_ml_push(vm->ctx, (ilhs <= irhs)? gtk_ml_value_true() : gtk_ml_value_false());
            } else if (!flt_lhs && flt_rhs) {
                gtk_ml_push(vm->ctx, ((double) ilhs <= frhs)? gtk_ml_value_true() : gtk_ml_value_false());
            } else if (flt_lhs && !flt_rhs) {
                gtk_ml_push(vm->ctx, (flhs <= (double) ilhs)? gtk_ml_value_true() : gtk_ml_value_false());
            } else if (flt_lhs && flt_rhs) {
                gtk_ml_push(vm->ctx, (flhs <= frhs)? gtk_ml_value_true() : gtk_ml_value_false());
            } else {
                *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
                return 0;
            }
        }
    } break;
    case GTKML_CMP_GREATER_EQUAL: {
        gboolean mm_success = 0;
        if (gtk_ml_is_sobject(tv_lhs)) {
            if (tv_lhs.value.sobj->kind == GTKML_S_MAP) {
                GtkMl_TaggedValue metakey = gtk_ml_value_sobject(gtk_ml_new_keyword(vm->ctx, NULL, 0, "__ge", strlen("__ge")));
                GtkMl_TaggedValue metafn = gtk_ml_value_none();
                if (tv_lhs.value.sobj->value.s_map.metamap) {
                    metafn = gtk_ml_hash_trie_get(&tv_lhs.value.sobj->value.s_map.metamap->value.s_map.map, metakey);
                }
                if (gtk_ml_has_value(metafn)) {
                    if (gtk_ml_is_sobject(metafn)) {
                        GtkMl_TaggedValue metaargs[] = { tv_lhs, tv_rhs };
                        if (!gtk_ml_execute(vm->ctx, err, metafn, metaargs, 2)) {
                            return 0;
                        }
                        mm_success = 1;
                    } else {
                        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 1,
                            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "function", strlen("function")));
                        *err = error;
                        return 0;
                    }
                }
            }
        }
        if (!mm_success && gtk_ml_is_sobject(tv_rhs)) {
            if (tv_rhs.value.sobj->kind == GTKML_S_MAP) {
                GtkMl_TaggedValue metakey = gtk_ml_value_sobject(gtk_ml_new_keyword(vm->ctx, NULL, 0, "__ge", strlen("__ge")));
                GtkMl_TaggedValue metafn = gtk_ml_value_none();
                if (tv_rhs.value.sobj->value.s_map.metamap) {
                    metafn = gtk_ml_hash_trie_get(&tv_rhs.value.sobj->value.s_map.metamap->value.s_map.map, metakey);
                }
                if (gtk_ml_has_value(metafn)) {
                    if (gtk_ml_is_sobject(metafn)) {
                        GtkMl_TaggedValue metaargs[] = { tv_lhs, tv_rhs };
                        if (!gtk_ml_execute(vm->ctx, err, metafn, metaargs, 2)) {
                            return 0;
                        }
                        mm_success = 1;
                    } else {
                        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 1,
                            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "function", strlen("function")));
                        *err = error;
                        return 0;
                    }
                }
            }
        }
        if (!mm_success) {
            if (!flt_lhs && !flt_rhs) {
                gtk_ml_push(vm->ctx, (ilhs >= irhs)? gtk_ml_value_true() : gtk_ml_value_false());
            } else if (!flt_lhs && flt_rhs) {
                gtk_ml_push(vm->ctx, ((double) ilhs >= frhs)? gtk_ml_value_true() : gtk_ml_value_false());
            } else if (flt_lhs && !flt_rhs) {
                gtk_ml_push(vm->ctx, (flhs >= (double) ilhs)? gtk_ml_value_true() : gtk_ml_value_false());
            } else if (flt_lhs && flt_rhs) {
                gtk_ml_push(vm->ctx, (flhs >= frhs)? gtk_ml_value_true() : gtk_ml_value_false());
            } else {
                *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
                return 0;
            }
        }
    } break;
    default:
        *err = gtk_ml_error(vm->ctx, "cmp-error", GTKML_ERR_CMP_ERROR, 0, 0, 0, 0);
        return 0;
    }

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_car(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;
    GtkMl_SObj list = gtk_ml_pop(vm->ctx).value.sobj;
    if (list->kind != GTKML_S_LIST) {
        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
                gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "list", strlen("list")),
                gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), list);
        *err = error;
        return 0;
    }
    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_car(list)));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_cdr(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;
    GtkMl_SObj list = gtk_ml_pop(vm->ctx).value.sobj;
    if (list->kind != GTKML_S_LIST) {
        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
                gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "list", strlen("list")),
                gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), list);
        *err = error;
        return 0;
    }
    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_cdr(list)));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_cons(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;
    GtkMl_SObj cdr = gtk_ml_to_sobj(vm->ctx, err, gtk_ml_pop(vm->ctx)).value.sobj;
    GtkMl_SObj car = gtk_ml_to_sobj(vm->ctx, err, gtk_ml_pop(vm->ctx)).value.sobj;
    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_list(vm->ctx, NULL, car, cdr)));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_map(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;

    uint64_t n = gtk_ml_to_prim(vm->ctx, err, gtk_ml_pop(vm->ctx)).value.u64;

    GtkMl_SObj map = gtk_ml_new_map(vm->ctx, NULL, NULL);

    while (n--) {
        GtkMl_TaggedValue value = gtk_ml_to_sobj(vm->ctx, err, gtk_ml_pop(vm->ctx));
        GtkMl_TaggedValue key = gtk_ml_to_sobj(vm->ctx, err, gtk_ml_pop(vm->ctx));
        GtkMl_SObj new = gtk_ml_new_map(vm->ctx, NULL, NULL);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &map->value.s_map.map, key, value);
        map = new;
    }

    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(map));

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_set(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;

    uint64_t n = gtk_ml_to_prim(vm->ctx, err, gtk_ml_pop(vm->ctx)).value.u64;

    GtkMl_SObj set = gtk_ml_new_set(vm->ctx, NULL);

    while (n--) {
        GtkMl_TaggedValue key = gtk_ml_to_sobj(vm->ctx, err, gtk_ml_pop(vm->ctx));
        GtkMl_SObj new = gtk_ml_new_set(vm->ctx, NULL);
        gtk_ml_hash_set_insert(&new->value.s_set.set, &set->value.s_set.set, key);
        set = new;
    }

    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(set));

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_array(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;

    uint64_t n = gtk_ml_to_prim(vm->ctx, err, gtk_ml_pop(vm->ctx)).value.u64;

    GtkMl_SObj array = gtk_ml_new_array(vm->ctx, NULL);

    while (n--) {
        GtkMl_TaggedValue value = gtk_ml_to_sobj(vm->ctx, err, gtk_ml_pop(vm->ctx));
        GtkMl_SObj new = gtk_ml_new_array(vm->ctx, NULL);
        gtk_ml_array_trie_push(&new->value.s_array.array, &array->value.s_array.array, value);
        array = new;
    }

    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(array));

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_bind(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;
    gtk_ml_set_local(vm->ctx, gtk_ml_pop(vm->ctx));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_enter_bind_args(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;

    ENTER(vm, vm->ctx->gc);

    GtkMl_SObj params = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_SObj _params = params;
    size_t n_params = 0;
    size_t n_args = gtk_ml_pop(vm->ctx).value.u64;

    while (params->kind != GTKML_S_NIL) {
        ++n_params;
        params = gtk_ml_cdr(params);
    }

    params = _params;
    while (params->kind != GTKML_S_NIL) {
        GtkMl_SObj key = gtk_ml_car(params);
        if (key->kind == GTKML_S_VARARG) {
            GtkMl_SObj rest = gtk_ml_new_nil(vm->ctx, NULL);
            for (size_t j = 0; j < n_args - (n_params - 1); j++) {
                rest = gtk_ml_new_list(vm->ctx, NULL, gtk_ml_pop(vm->ctx).value.sobj, rest);
            }
            gtk_ml_set_local(vm->ctx, gtk_ml_value_sobject(rest));
        } else {
            GtkMl_TaggedValue value = gtk_ml_pop(vm->ctx);
            gtk_ml_set_local(vm->ctx, value);
        }
        params = gtk_ml_cdr(params);
    }

    ENTER(vm, vm->ctx->gc);

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_enter(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;
    ENTER(vm, vm->ctx->gc);
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_leave(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;

    if (vm->ctx->bindings->value.s_var.expr->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(vm->ctx, "scope-error", GTKML_ERR_SCOPE_ERROR, 0, 0, 0, 0);
        return 0;
    }

    LEAVE(vm, vm->ctx->gc);
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_unwrap(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;
    GtkMl_SObj list = gtk_ml_pop(vm->ctx).value.sobj;
    size_t n = 0;
    while (list->kind != GTKML_S_NIL) {
        ++n;
        gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_car(list)));
        list = gtk_ml_cdr(list);
    }
    gtk_ml_push(vm->ctx, gtk_ml_value_int(n));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_typeof(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;
    GtkMl_TaggedValue value = gtk_ml_pop(vm->ctx);
    if (gtk_ml_is_primitive(value)) {
        gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_keyword(vm->ctx, NULL, 0, PRIMNAME[value.tag], strlen(PRIMNAME[value.tag]))));
    } else {
        GtkMl_SObj sobj = value.value.sobj;
        if (sobj->kind == GTKML_S_ARRAY) {
            if (gtk_ml_array_trie_is_string(&sobj->value.s_array.array)) {
                gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_keyword(vm->ctx, NULL, 0, "string", strlen("string"))));
            } else {
                gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_keyword(vm->ctx, NULL, 0, TYPENAME[sobj->kind], strlen(TYPENAME[sobj->kind]))));
            }
        } else {
            gtk_ml_push(vm->ctx, gtk_ml_value_sobject(gtk_ml_new_keyword(vm->ctx, NULL, 0, TYPENAME[sobj->kind], strlen(TYPENAME[sobj->kind]))));
        }
    }
    PC_INCREMENT;
    return 1;
}

GtkMl_TaggedValue gtk_ml_to_sobj(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue value) {
    if (!gtk_ml_has_value(value)) {
        *err = gtk_ml_error(ctx, "value-error", GTKML_ERR_VALUE_ERROR, 0, 0, 0, 0);
        return gtk_ml_value_none();
    }
    if (gtk_ml_is_sobject(value)) {
        return value;
    } else {
        switch (value.tag) {
        case GTKML_TAG_NIL:
            return gtk_ml_value_sobject(gtk_ml_new_nil(ctx, NULL));
        case GTKML_TAG_BOOL:
            return gtk_ml_value_sobject(value.value.boolean? gtk_ml_new_true(ctx, NULL) : gtk_ml_new_false(ctx, NULL));
        case GTKML_TAG_CHAR:
            return gtk_ml_value_sobject(gtk_ml_new_char(ctx, NULL, value.value.unicode));
        case GTKML_TAG_INT64:
        case GTKML_TAG_UINT64:
            return gtk_ml_value_sobject(gtk_ml_new_int(ctx, NULL, value.value.s64));
        case GTKML_TAG_FLOAT:
            return gtk_ml_value_sobject(gtk_ml_new_float(ctx, NULL, value.value.flt));
        case GTKML_TAG_USERDATA:
            return gtk_ml_value_sobject(gtk_ml_new_lightdata(ctx, NULL, value.value.userdata));
        case GTKML_TAG_FFI:
            return gtk_ml_value_sobject(gtk_ml_new_ffi(ctx, NULL, value.value.ffi));
        default:
            *err = gtk_ml_error(ctx, "primitive-error", GTKML_ERR_TAG_ERROR, 0, 0, 0, 0);
            return gtk_ml_value_none();
        }
    }
}

GtkMl_TaggedValue gtk_ml_to_prim(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue sobj) {
    if (!gtk_ml_has_value(sobj)) {
        *err = gtk_ml_error(ctx, "value-error", GTKML_ERR_VALUE_ERROR, 0, 0, 0, 0);
        return gtk_ml_value_none();
    }
    if (gtk_ml_is_primitive(sobj)) {
        return sobj;
    } else {
        switch (sobj.value.sobj->kind) {
        case GTKML_S_NIL:
            return gtk_ml_value_nil();
        case GTKML_S_TRUE:
            return gtk_ml_value_true();
        case GTKML_S_FALSE:
            return gtk_ml_value_false();
        case GTKML_S_CHAR:
            return gtk_ml_value_char(sobj.value.sobj->value.s_char.value);
        case GTKML_S_INT:
            return gtk_ml_value_int(sobj.value.sobj->value.s_int.value);
        case GTKML_S_FLOAT:
            return gtk_ml_value_float(sobj.value.sobj->value.s_float.value);
        case GTKML_S_LIGHTDATA:
            return gtk_ml_value_userdata(sobj.value.sobj->value.s_lightdata.userdata);
        default:
            *err = gtk_ml_error(ctx, "primitive-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 0);
            return gtk_ml_value_none();
        }
    }
}

gboolean gtk_ml_i_to_sobj(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;
    GtkMl_TaggedValue value = gtk_ml_pop(vm->ctx);
    GtkMl_TaggedValue sobj = gtk_ml_to_sobj(vm->ctx, err, value);
    if (gtk_ml_has_value(sobj)) {
        gtk_ml_push(vm->ctx, sobj);
    } else {
        return 0;
    }
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_to_prim(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;
    GtkMl_TaggedValue value = gtk_ml_pop(vm->ctx);
    GtkMl_TaggedValue prim = gtk_ml_to_prim(vm->ctx, err, value);
    if (gtk_ml_has_value(prim)) {
        gtk_ml_push(vm->ctx, prim);
    } else {
        return 0;
    }
    PC_INCREMENT;

    return 1;
}

gboolean gtk_ml_i_define(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;
    GtkMl_SObj key = gtk_ml_to_sobj(vm->ctx, err, gtk_ml_pop(vm->ctx)).value.sobj;
    GtkMl_TaggedValue value = gtk_ml_pop(vm->ctx);
    gtk_ml_bind(vm->ctx, key, value);
    gtk_ml_push(vm->ctx, gtk_ml_value_nil());
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_list(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;

    uint64_t n = gtk_ml_pop(vm->ctx).value.u64;

    GtkMl_SObj list = gtk_ml_new_nil(vm->ctx, NULL);

    while (n--) {
        GtkMl_SObj expr = gtk_ml_pop(vm->ctx).value.sobj;
        list = gtk_ml_new_list(vm->ctx, NULL, expr, list);
    }

    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(list));

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_push_imm(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;
    gtk_ml_push(vm->ctx, gtk_ml_get_data(vm->program, data));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_setf_imm(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;
    GtkMl_TaggedValue value = gtk_ml_get_data(vm->program, data);
    if (gtk_ml_is_primitive(value)) {
        switch (value.tag) {
        case GTKML_TAG_BOOL:
            if (value.value.boolean) {
                vm->flags |= GTKML_F_EQUAL;
                vm->flags &= ~GTKML_F_NEQUAL;
            } else {
                vm->flags |= GTKML_F_NEQUAL;
                vm->flags &= ~GTKML_F_EQUAL;
            }
            break;
        case GTKML_TAG_NIL:
            vm->flags |= GTKML_F_NEQUAL;
            vm->flags &= ~GTKML_F_EQUAL;
            break;
        default:
            vm->flags |= GTKML_F_EQUAL;
            vm->flags &= ~GTKML_F_NEQUAL;
            break;
        }
    } else {
        switch (value.value.sobj->kind) {
        case GTKML_S_FALSE:
        case GTKML_S_NIL:
            vm->flags |= GTKML_F_NEQUAL;
            vm->flags &= ~GTKML_F_EQUAL;
            break;
        default:
            vm->flags |= GTKML_F_EQUAL;
            vm->flags &= ~GTKML_F_NEQUAL;
            break;
        }
    }
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_popf(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;
    GtkMl_TaggedValue value = gtk_ml_pop(vm->ctx);
    if (gtk_ml_is_primitive(value)) {
        switch (value.tag) {
        case GTKML_TAG_BOOL:
            if (value.value.boolean) {
                vm->flags |= GTKML_F_EQUAL;
                vm->flags &= ~GTKML_F_NEQUAL;
            } else {
                vm->flags |= GTKML_F_NEQUAL;
                vm->flags &= ~GTKML_F_EQUAL;
            }
            break;
        case GTKML_TAG_NIL:
            vm->flags |= GTKML_F_NEQUAL;
            vm->flags &= ~GTKML_F_EQUAL;
            break;
        default:
            vm->flags |= GTKML_F_EQUAL;
            vm->flags &= ~GTKML_F_NEQUAL;
            break;
        }
    } else {
        switch (value.value.sobj->kind) {
        case GTKML_S_FALSE:
        case GTKML_S_NIL:
            vm->flags |= GTKML_F_NEQUAL;
            vm->flags &= ~GTKML_F_EQUAL;
            break;
        default:
            vm->flags |= GTKML_F_EQUAL;
            vm->flags &= ~GTKML_F_NEQUAL;
            break;
        }
    }
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_pop(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;
    gtk_ml_pop(vm->ctx);
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_get_imm(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;
    GtkMl_SObj key = gtk_ml_to_sobj(vm->ctx, err, gtk_ml_get_data(vm->program, data)).value.sobj;
    GtkMl_TaggedValue value = gtk_ml_get(vm->ctx, key);
    if (gtk_ml_has_value(value)) {
        gtk_ml_push(vm->ctx, value);
    } else {
        GtkMl_SObj error = gtk_ml_error(vm->ctx, "binding-error", GTKML_ERR_BINDING_ERROR, 0, 0, 0, 1, gtk_ml_new_keyword(vm->ctx, NULL, 0, "binding", strlen("binding")), key);
        *err = error;
        return 0;
    }
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_local_imm(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;
    int64_t offset = gtk_ml_get_data(vm->program, data).value.s64;
    GtkMl_TaggedValue value = gtk_ml_get_local(vm->ctx, offset);
    if (gtk_ml_has_value(value)) {
        gtk_ml_push(vm->ctx, value);
    } else {
        GtkMl_SObj error = gtk_ml_error(vm->ctx, "binding-error", GTKML_ERR_BINDING_ERROR, 0, 0, 0, 0);
        *err = error;
        return 0;
    }
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_list_imm(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;

    uint64_t n = gtk_ml_get_data(vm->program, data).value.u64;

    GtkMl_SObj list = gtk_ml_new_nil(vm->ctx, NULL);

    while (n--) {
        GtkMl_SObj expr = gtk_ml_to_sobj(vm->ctx, err, gtk_ml_pop(vm->ctx)).value.sobj;
        list = gtk_ml_new_list(vm->ctx, NULL, expr, list);
    }

    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(list));

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_map_imm(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;

    uint64_t n = gtk_ml_get_data(vm->program, data).value.u64;

    GtkMl_SObj map = gtk_ml_new_map(vm->ctx, NULL, NULL);

    while (n--) {
        GtkMl_TaggedValue value = gtk_ml_to_sobj(vm->ctx, err, gtk_ml_pop(vm->ctx));
        GtkMl_TaggedValue key = gtk_ml_to_sobj(vm->ctx, err, gtk_ml_pop(vm->ctx));
        GtkMl_SObj new = gtk_ml_new_map(vm->ctx, NULL, NULL);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &map->value.s_map.map, key, value);
        map = new;
    }

    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(map));

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_set_imm(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;

    uint64_t n = gtk_ml_get_data(vm->program, data).value.u64;

    GtkMl_SObj set = gtk_ml_new_set(vm->ctx, NULL);

    while (n--) {
        GtkMl_TaggedValue key = gtk_ml_to_sobj(vm->ctx, err, gtk_ml_pop(vm->ctx));
        GtkMl_SObj new = gtk_ml_new_set(vm->ctx, NULL);
        gtk_ml_hash_set_insert(&new->value.s_set.set, &set->value.s_set.set, key);
        set = new;
    }

    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(set));

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_array_imm(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;

    uint64_t n = gtk_ml_get_data(vm->program, data).value.u64;

    GtkMl_SObj array = gtk_ml_new_array(vm->ctx, NULL);

    while (n--) {
        GtkMl_TaggedValue value = gtk_ml_to_sobj(vm->ctx, err, gtk_ml_pop(vm->ctx));
        GtkMl_SObj new = gtk_ml_new_array(vm->ctx, NULL);
        gtk_ml_array_trie_push(&new->value.s_array.array, &array->value.s_array.array, value);
        array = new;
    }

    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(array));

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_setmm_imm(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;

    GtkMl_SObj metamap = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_SObj map = gtk_ml_pop(vm->ctx).value.sobj;

    GtkMl_SObj result = gtk_ml_new_map(vm->ctx, NULL, metamap);
    gtk_ml_del_hash_trie(vm->ctx, &result->value.s_map.map, gtk_ml_delete_value);
    gtk_ml_hash_trie_copy(&result->value.s_map.map, &map->value.s_map.map);

    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(result));

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_getmm_imm(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;

    GtkMl_SObj map = gtk_ml_pop(vm->ctx).value.sobj;

    GtkMl_SObj result = gtk_ml_getmetamap(map);
    if (!result) {
        *err = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 0);
        return 0;
    }
    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(result));

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_var(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;

    GtkMl_SObj value = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_SObj result = gtk_ml_new_var(vm->ctx, NULL, value);
    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(result));

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_getvar(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;

    GtkMl_TaggedValue tv_var = gtk_ml_pop(vm->ctx);
    if (!gtk_ml_is_sobject(tv_var)) {
        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "var", strlen("var")),
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), gtk_ml_to_sobj(vm->ctx, err, tv_var).value.sobj);
        *err = error;
        return 0;
    }
    GtkMl_SObj var = tv_var.value.sobj;
    if (var->kind != GTKML_S_VAR) {
        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "var", strlen("var")),
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), var);
        *err = error;
        return 0;
    }
    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(var->value.s_var.expr));

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_assignvar(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;

    GtkMl_SObj newvalue = gtk_ml_to_sobj(vm->ctx, err, gtk_ml_pop(vm->ctx)).value.sobj;
    GtkMl_TaggedValue tv_var = gtk_ml_pop(vm->ctx);
    if (!gtk_ml_is_sobject(tv_var)) {
        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "var", strlen("var")),
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), gtk_ml_to_sobj(vm->ctx, err, tv_var).value.sobj);
        *err = error;
        return 0;
    }
    GtkMl_SObj var = tv_var.value.sobj;
    if (var->kind != GTKML_S_VAR) {
        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "var", strlen("var")),
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), var);
        *err = error;
        return 0;
    }
    var->value.s_var.expr = newvalue;
    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(var));

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_len(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;

    GtkMl_SObj container = gtk_ml_pop(vm->ctx).value.sobj;
    switch (container->kind) {
    case GTKML_S_ARRAY:
        gtk_ml_push(vm->ctx, gtk_ml_value_int(gtk_ml_array_trie_len(&container->value.s_array.array)));
        break;
    case GTKML_S_MAP: {
        GtkMl_TaggedValue metakey = gtk_ml_value_sobject(gtk_ml_new_keyword(vm->ctx, NULL, 0, "__len", strlen("__len")));
        GtkMl_TaggedValue metafn = gtk_ml_value_none();
        if (container->value.s_map.metamap) {
            metafn = gtk_ml_hash_trie_get(&container->value.s_map.metamap->value.s_map.map, metakey);
        }
        if (gtk_ml_has_value(metafn)) {
            if (gtk_ml_is_sobject(metafn)) {
                GtkMl_TaggedValue metaargs[] = { gtk_ml_value_sobject(container) };
                if (!gtk_ml_execute(vm->ctx, err, metafn, metaargs, 1)) {
                    return 0;
                }
            } else {
                GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 1,
                    gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "function", strlen("function")));
                *err = error;
                return 0;
            }
        } else {
            gtk_ml_push(vm->ctx, gtk_ml_value_int(gtk_ml_hash_trie_len(&container->value.s_map.map)));
        }
    } break;
    case GTKML_S_SET:
        gtk_ml_push(vm->ctx, gtk_ml_value_int(gtk_ml_hash_set_len(&container->value.s_set.set)));
        break;
    default:
        *err = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_CONTAINER_ERROR, 0, 0, 0, 0);
        return 0;
    }

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_index(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;

    GtkMl_SObj container = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_SObj index = gtk_ml_pop(vm->ctx).value.sobj;

    if (index->kind != GTKML_S_INT) {
        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "int", strlen("int")),
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), index);
        *err = error;
        return 0;
    }

    switch (container->kind) {
    case GTKML_S_ARRAY: {
        GtkMl_TaggedValue opt = gtk_ml_array_trie_get(&container->value.s_array.array, index->value.s_int.value);
        if (gtk_ml_has_value(opt)) {
            gtk_ml_push(vm->ctx, opt);
        } else {
            size_t len = gtk_ml_array_trie_len(&container->value.s_array.array);
            GtkMl_SObj error = gtk_ml_error(vm->ctx, "index-out-of-bounds", GTKML_ERR_INDEX_ERROR, 0, 0, 0, 2,
                gtk_ml_new_keyword(vm->ctx, NULL, 0, "len", strlen("len")), gtk_ml_new_int(vm->ctx, NULL, len),
                gtk_ml_new_keyword(vm->ctx, NULL, 0, "got", strlen("got")), index);
            *err = error;
            return 0;
        }
    } break;
    case GTKML_S_MAP: {
        GtkMl_TaggedValue metakey = gtk_ml_value_sobject(gtk_ml_new_keyword(vm->ctx, NULL, 0, "__index", strlen("__index")));
        GtkMl_TaggedValue metafn = gtk_ml_value_none();
        if (container->value.s_map.metamap) {
            metafn = gtk_ml_hash_trie_get(&container->value.s_map.metamap->value.s_map.map, metakey);
        }
        if (gtk_ml_has_value(metafn)) {
            if (gtk_ml_is_sobject(metafn)) {
                GtkMl_TaggedValue metaargs[] = { gtk_ml_value_sobject(container), gtk_ml_value_sobject(index) };
                if (!gtk_ml_execute(vm->ctx, err, metafn, metaargs, 2)) {
                    return 0;
                }
            } else {
                GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 1,
                    gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "function", strlen("function")));
                *err = error;
                return 0;
            }
        } else {
            GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 1,
                gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "function", strlen("function")));
            *err = error;
            return 0;
        }
    } break;
    default: {
        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "array", strlen("array")),
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), container);
        *err = error;
        return 0;
    }
    }

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_array_push(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;

    GtkMl_SObj container = gtk_ml_pop(vm->ctx).value.sobj;

    if (container->kind != GTKML_S_ARRAY) {
        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "array", strlen("array")),
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), container);
        *err = error;
        return 0;
    }

    GtkMl_SObj result = gtk_ml_new_array(vm->ctx, NULL);
    gtk_ml_del_array_trie(vm->ctx, &result->value.s_array.array, gtk_ml_delete_value);
    GtkMl_TaggedValue elem = gtk_ml_pop(vm->ctx);

    gtk_ml_array_trie_push(&result->value.s_array.array, &container->value.s_array.array, elem);
    gtk_ml_push(vm->ctx, gtk_ml_value_sobject(result));

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_array_pop(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;

    GtkMl_SObj container = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_SObj result = gtk_ml_new_array(vm->ctx, NULL);
    gtk_ml_del_array_trie(vm->ctx, &result->value.s_array.array, gtk_ml_delete_value);

    switch (container->kind) {
    case GTKML_S_ARRAY:
        gtk_ml_array_trie_pop(&result->value.s_array.array, &container->value.s_array.array);
        gtk_ml_push(vm->ctx, gtk_ml_value_sobject(result));
        break;
    default: {
        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "array", strlen("array")),
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), container);
        *err = error;
        return 0;
    }
    }

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_array_concat(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;

    GtkMl_SObj a1 = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_SObj a2 = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_SObj result = gtk_ml_new_array(vm->ctx, NULL);
    gtk_ml_del_array_trie(vm->ctx, &result->value.s_array.array, gtk_ml_delete_value);

    switch (a1->kind) {
    case GTKML_S_ARRAY:
        switch (a2->kind) {
        case GTKML_S_ARRAY:
            gtk_ml_array_trie_concat(&result->value.s_array.array, &a1->value.s_array.array, &a2->value.s_array.array);
            gtk_ml_push(vm->ctx, gtk_ml_value_sobject(result));
            break;
        default: {
            GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
                gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "array", strlen("array")),
                gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), a2);
            *err = error;
            return 0;
        }
        }
        break;
    default: {
        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "array", strlen("array")),
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), a1);
        *err = error;
        return 0;
    }
    }

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_map_get(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;

    GtkMl_SObj container = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_TaggedValue key = gtk_ml_pop(vm->ctx);

    switch (container->kind) {
    case GTKML_S_MAP: {
        gboolean mm_success = 0;
        GtkMl_TaggedValue metakey = gtk_ml_value_sobject(gtk_ml_new_keyword(vm->ctx, NULL, 0, "__get", strlen("__get")));
        GtkMl_TaggedValue metafn = gtk_ml_value_none();
        if (container->value.s_map.metamap) {
            metafn = gtk_ml_hash_trie_get(&container->value.s_map.metamap->value.s_map.map, metakey);
        }
        if (gtk_ml_has_value(metafn)) {
            if (gtk_ml_is_sobject(metafn)) {
                GtkMl_TaggedValue metaargs[] = { gtk_ml_value_sobject(container), key };
                if (!gtk_ml_execute(vm->ctx, err, metafn, metaargs, 2)) {
                    return 0;
                }
                mm_success = 1;
            } else {
                GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 1,
                    gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "function", strlen("function")));
                *err = error;
                return 0;
            }
        }
        if (!mm_success) {
            GtkMl_TaggedValue opt = gtk_ml_hash_trie_get(&container->value.s_map.map, key);
            if (gtk_ml_has_value(opt)) {
                gtk_ml_push(vm->ctx, opt);
            } else {
                GtkMl_SObj error = gtk_ml_error(vm->ctx, "index-out-of-bounds", GTKML_ERR_INDEX_ERROR, 0, 0, 0, 1,
                    gtk_ml_new_keyword(vm->ctx, NULL, 0, "key", strlen("key")), key);
                *err = error;
                return 0;
            }
        }
    } break;
    default: {
        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "map", strlen("map")),
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), container);
        *err = error;
        return 0;
    }
    }

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_map_insert(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;

    GtkMl_SObj container = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_TaggedValue key = gtk_ml_pop(vm->ctx);
    GtkMl_TaggedValue value = gtk_ml_pop(vm->ctx);

    switch (container->kind) {
    case GTKML_S_MAP: {
        gboolean mm_success = 0;
        GtkMl_TaggedValue metakey = gtk_ml_value_sobject(gtk_ml_new_keyword(vm->ctx, NULL, 0, "__insert", strlen("__insert")));
        GtkMl_TaggedValue metafn = gtk_ml_value_none();
        if (container->value.s_map.metamap) {
            metafn = gtk_ml_hash_trie_get(&container->value.s_map.metamap->value.s_map.map, metakey);
        }
        if (gtk_ml_has_value(metafn)) {
            if (gtk_ml_is_sobject(metafn)) {
                GtkMl_TaggedValue metaargs[] = { gtk_ml_value_sobject(container), key, value };
                if (!gtk_ml_execute(vm->ctx, err, metafn, metaargs, 3)) {
                    return 0;
                }
                mm_success = 1;
            } else {
                GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 1,
                    gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "function", strlen("function")));
                *err = error;
                return 0;
            }
        }
        if (!mm_success) {
            GtkMl_SObj result = gtk_ml_new_map(vm->ctx, NULL, container->value.s_map.metamap);
            gtk_ml_del_hash_trie(vm->ctx, &result->value.s_map.map, gtk_ml_delete_value);
            gtk_ml_hash_trie_insert(&result->value.s_map.map, &container->value.s_map.map, key, value);
            gtk_ml_push(vm->ctx, gtk_ml_value_sobject(result));
        }
    } break;
    default: {
        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "map", strlen("map")),
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), container);
        *err = error;
        return 0;
    }
    }

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_map_rawget(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;

    GtkMl_SObj container = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_TaggedValue key = gtk_ml_pop(vm->ctx);

    switch (container->kind) {
    case GTKML_S_MAP: {
        GtkMl_TaggedValue opt = gtk_ml_hash_trie_get(&container->value.s_map.map, key);
        if (gtk_ml_has_value(opt)) {
            gtk_ml_push(vm->ctx, opt);
        } else {
            GtkMl_SObj error = gtk_ml_error(vm->ctx, "index-out-of-bounds", GTKML_ERR_INDEX_ERROR, 0, 0, 0, 1,
                gtk_ml_new_keyword(vm->ctx, NULL, 0, "key", strlen("key")), key);
            *err = error;
            return 0;
        }
        break;
    }
    default: {
        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "map", strlen("map")),
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), container);
        *err = error;
        return 0;
    }
    }

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_map_rawinsert(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;

    GtkMl_SObj container = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_TaggedValue key = gtk_ml_pop(vm->ctx);
    GtkMl_TaggedValue value = gtk_ml_pop(vm->ctx);
    GtkMl_SObj result = gtk_ml_new_map(vm->ctx, NULL, container->value.s_map.metamap);
    gtk_ml_del_hash_trie(vm->ctx, &result->value.s_map.map, gtk_ml_delete_value);

    switch (container->kind) {
    case GTKML_S_MAP:
        gtk_ml_hash_trie_insert(&result->value.s_map.map, &container->value.s_map.map, key, value);
        gtk_ml_push(vm->ctx, gtk_ml_value_sobject(result));
        break;
    default: {
        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "map", strlen("map")),
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), container);
        *err = error;
        return 0;
    }
    }

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_map_delete(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;

    GtkMl_SObj container = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_TaggedValue key = gtk_ml_pop(vm->ctx);
    GtkMl_SObj result = gtk_ml_new_map(vm->ctx, NULL, container->value.s_map.metamap);
    gtk_ml_del_hash_trie(vm->ctx, &result->value.s_map.map, gtk_ml_delete_value);

    switch (container->kind) {
    case GTKML_S_MAP:
        gtk_ml_hash_trie_delete(&result->value.s_map.map, &container->value.s_map.map, key);
        gtk_ml_push(vm->ctx, gtk_ml_value_sobject(result));
        break;
    default: {
        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "map", strlen("map")),
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), container);
        *err = error;
        return 0;
    }
    }

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_set_contains(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;

    GtkMl_SObj container = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_TaggedValue key = gtk_ml_pop(vm->ctx);

    switch (container->kind) {
    case GTKML_S_SET:
        gtk_ml_push(vm->ctx, gtk_ml_hash_set_contains(&container->value.s_set.set, key)?
                gtk_ml_value_true() : gtk_ml_value_false());
        break;
    default: {
        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "set", strlen("set")),
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), container);
        *err = error;
        return 0;
    }
    }

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_set_insert(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;

    GtkMl_SObj container = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_TaggedValue key = gtk_ml_pop(vm->ctx);
    GtkMl_SObj result = gtk_ml_new_set(vm->ctx, NULL);
    gtk_ml_del_hash_set(vm->ctx, &result->value.s_set.set, gtk_ml_delete_value);

    switch (container->kind) {
    case GTKML_S_SET:
        gtk_ml_hash_set_insert(&result->value.s_set.set, &container->value.s_set.set, key);
        gtk_ml_push(vm->ctx, gtk_ml_value_sobject(result));
        break;
    default: {
        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "set", strlen("set")),
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), container);
        *err = error;
        return 0;
    }
    }

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_set_delete(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;

    GtkMl_SObj container = gtk_ml_pop(vm->ctx).value.sobj;
    GtkMl_TaggedValue key = gtk_ml_pop(vm->ctx);
    GtkMl_SObj result = gtk_ml_new_set(vm->ctx, NULL);
    gtk_ml_del_hash_set(vm->ctx, &result->value.s_set.set, gtk_ml_delete_value);

    switch (container->kind) {
    case GTKML_S_SET:
        gtk_ml_hash_set_delete(&result->value.s_set.set, &container->value.s_set.set, key);
        gtk_ml_push(vm->ctx, gtk_ml_value_sobject(result));
        break;
    default: {
        GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "set", strlen("set")),
            gtk_ml_new_keyword(vm->ctx, NULL, 0, "got-value", strlen("got-value")), container);
        *err = error;
        return 0;
    }
    }

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_call(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;

    GtkMl_TaggedValue program = gtk_ml_pop(vm->ctx);

    if (gtk_ml_is_sobject(program)) {
        switch (program.value.sobj->kind) {
        case GTKML_S_MAP: {
            GtkMl_TaggedValue metakey = gtk_ml_value_sobject(gtk_ml_new_keyword(vm->ctx, NULL, 0, "__call", strlen("__call")));
            GtkMl_TaggedValue metafn = gtk_ml_value_none();
            if (program.value.sobj->value.s_map.metamap) {
                metafn = gtk_ml_hash_trie_get(&program.value.sobj->value.s_map.metamap->value.s_map.map, metakey);
            }
            if (gtk_ml_has_value(metafn)) {
                if (!gtk_ml_execute(vm->ctx, err, metafn, NULL, 0)) {
                    return 0;
                }
            } else {
                GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 1,
                    gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "function", strlen("function")));
                *err = error;
                return 0;
            }
            PC_INCREMENT;
        } break;
        case GTKML_S_PROGRAM: {
            if (vm->call_stack_ptr == vm->call_stack_cap) {
                *err = gtk_ml_error(vm->ctx, "stack-overflow", GTKML_ERR_STACK_ERROR, 0, 0, 0, 0);
                return 0;
            }

            uint64_t flags = vm->flags & GTKML_F_TOPCALL;
            vm->call_stack[vm->call_stack_ptr++] = flags;
            vm->flags &= ~GTKML_F_TOPCALL;

            PC_INCREMENT;
            uint64_t pc = vm->pc;
            vm->call_stack[vm->call_stack_ptr++] = pc;
            vm->pc = program.value.sobj->value.s_program.addr;
        } break;
        case GTKML_S_FFI: {
            GtkMl_Ffi ffi = program.value.sobj->value.s_ffi.ffi;

            GtkMl_TaggedValue result = gtk_ml_value_none();
            ffi(&result, vm->ctx, err);
            if (!gtk_ml_has_value(result)) {
                if (!*err) {
                    GtkMl_SObj error = gtk_ml_error(vm->ctx, "unknown-error", GTKML_ERR_UNKNOWN_ERROR, 0, 0, 0, 0);
                    *err = error;
                }
                return 0;
            }
            gtk_ml_push(vm->ctx, result);

            PC_INCREMENT;
        }
        default: {
            GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 1,
                gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "function", strlen("function")));
            *err = error;
            return 0;
        }
        }
    } else {
        if (program.tag == GTKML_TAG_FFI) {
            GtkMl_Ffi ffi = program.value.ffi;

            GtkMl_TaggedValue result = gtk_ml_value_none();
            ffi(&result, vm->ctx, err);
            if (!gtk_ml_has_value(result)) {
                if (!*err) {
                    GtkMl_SObj error = gtk_ml_error(vm->ctx, "unknown-error", GTKML_ERR_UNKNOWN_ERROR, 0, 0, 0, 0);
                    *err = error;
                }
                return 0;
            }
            gtk_ml_push(vm->ctx, result);

            PC_INCREMENT;
        } else {
            GtkMl_SObj error = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 1,
                gtk_ml_new_keyword(vm->ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(vm->ctx, NULL, 0, "function", strlen("function")));
            *err = error;
            return 0;
        }
    }

    return 1;
}

gboolean gtk_ml_i_leave_ret(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;

    if (vm->flags & GTKML_F_TOPCALL) {
        LEAVE(vm, vm->ctx->gc);
        LEAVE(vm, vm->ctx->gc);

        vm->flags |= GTKML_F_HALT;
        PC_INCREMENT;
    } else {
        if (vm->ctx->bindings->value.s_var.expr->kind == GTKML_S_NIL) {
            *err = gtk_ml_error(vm->ctx, "scope-error", GTKML_ERR_SCOPE_ERROR, 0, 0, 0, 0);
            return 0;
        }

        LEAVE(vm, vm->ctx->gc);
        LEAVE(vm, vm->ctx->gc);

        uint64_t pc = vm->call_stack[--vm->call_stack_ptr];
        uint64_t flags = vm->call_stack[--vm->call_stack_ptr];
        vm->pc = pc;
        vm->flags |= flags;
    }

    return 1;
}

gboolean gtk_ml_i_call_core(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) data;
    GtkMl_TaggedValue (*function)(GtkMl_Context *, GtkMl_SObj *, GtkMl_TaggedValue) = vm->core[gtk_ml_get_data(vm->program, data).value.u64];
    GtkMl_TaggedValue expr = gtk_ml_pop(vm->ctx);
    GtkMl_TaggedValue value = function(vm->ctx, err, expr);
    if (gtk_ml_has_value(value)) {
        gtk_ml_push(vm->ctx, value);
    } else {
        return 0;
    }
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_i_branch_absolute(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;
    vm->flags &= ~GTKML_F_GENERIC;
    GtkMl_SObj addr = gtk_ml_to_sobj(vm->ctx, err, gtk_ml_get_data(vm->program, data)).value.sobj;
    vm->pc = addr->value.s_address.addr;
    return 1;
}

gboolean gtk_ml_i_branch_relative(GtkMl_Vm *vm, GtkMl_SObj *err, GtkMl_Data data) {
    (void) err;
    (void) data;
    vm->flags &= ~GTKML_F_GENERIC;
    GtkMl_SObj addr = gtk_ml_to_sobj(vm->ctx, err, gtk_ml_get_data(vm->program, data)).value.sobj;
    vm->pc += 8 + addr->value.s_address.addr;
    return 1;
}

