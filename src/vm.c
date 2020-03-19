#include <stdlib.h>
#include <string.h>
#ifdef GTKML_ENABLE_POSIX
#include <sys/ptrace.h>
#include <sys/wait.h>
#endif /* GTKML_ENABLE_POSIX */
#ifdef GTKML_ENABLE_GTK
#include <gtk/gtk.h>
#endif /* GTKML_ENABLE_GTK */
#include <math.h>
#define GTKML_INCLUDE_INTERNAL
#include "gtk-ml.h"
#include "gtk-ml-internal.h"
#ifdef GTKML_ENABLE_WEB
#include "libs/em_gles3/core-libs.c"
#include "vm-web.c"
#endif /* GTKML_ENABLE_WEB */

GTKML_PRIVATE gboolean (*OPCODES[])(GtkMl_Vm *, GtkMl_SObj *, GtkMl_Data) = {
    [GTKML_I_NOP] = gtk_ml_i_nop,
    [GTKML_I_ADD] = gtk_ml_i_signed_add,
    [GTKML_I_SUBTRACT] = gtk_ml_i_signed_subtract,
    [GTKML_I_SIGNED_MULTIPLY] = gtk_ml_i_signed_multiply,
    [GTKML_I_UNSIGNED_MULTIPLY] = gtk_ml_i_unsigned_multiply,
    [GTKML_I_SIGNED_DIVIDE] = gtk_ml_i_signed_divide,
    [GTKML_I_UNSIGNED_DIVIDE] = gtk_ml_i_unsigned_divide,
    [GTKML_I_SIGNED_MODULO] = gtk_ml_i_signed_modulo,
    [GTKML_I_UNSIGNED_MODULO] = gtk_ml_i_unsigned_modulo,
    [GTKML_I_BIT_AND] = gtk_ml_i_bit_and,
    [GTKML_I_BIT_OR] = gtk_ml_i_bit_or,
    [GTKML_I_BIT_XOR] = gtk_ml_i_bit_xor,
    [GTKML_I_BIT_NAND] = gtk_ml_i_bit_nand,
    [GTKML_I_BIT_NOR] = gtk_ml_i_bit_nor,
    [GTKML_I_BIT_XNOR] = gtk_ml_i_bit_xnor,
    [GTKML_I_CMP_IMM] = gtk_ml_i_cmp_imm,
    [GTKML_I_CAR] = gtk_ml_i_car,
    [GTKML_I_CDR] = gtk_ml_i_cdr,
    [GTKML_I_CONS] = gtk_ml_i_cons,
    [GTKML_I_MAP] = gtk_ml_i_map,
    [GTKML_I_SET] = gtk_ml_i_set,
    [GTKML_I_ARRAY] = gtk_ml_i_array,
    [GTKML_I_DEFINE] = gtk_ml_i_define,
    [GTKML_I_LIST] = gtk_ml_i_list,
    [GTKML_I_BIND] = gtk_ml_i_bind,
    [GTKML_I_ENTER_BIND_ARGS] = gtk_ml_i_enter_bind_args,
    [GTKML_I_LOCAL_IMM] = gtk_ml_i_local_imm,
    [GTKML_I_ENTER] = gtk_ml_i_enter,
    [GTKML_I_LEAVE] = gtk_ml_i_leave,
    [GTKML_I_UNWRAP] = gtk_ml_i_unwrap,
    [GTKML_I_TYPEOF] = gtk_ml_i_typeof,
    [GTKML_I_TO_SOBJ] = gtk_ml_i_to_sobj,
    [GTKML_I_TO_PRIM] = gtk_ml_i_to_prim,
    [GTKML_I_PUSH_IMM] = gtk_ml_i_push_imm,
    [GTKML_I_POP] = gtk_ml_i_pop,
    [GTKML_I_SETF_IMM] = gtk_ml_i_setf_imm,
    [GTKML_I_POPF] = gtk_ml_i_popf,
    [GTKML_I_GET_IMM] = gtk_ml_i_get_imm,
    [GTKML_I_LIST_IMM] = gtk_ml_i_list_imm,
    [GTKML_I_MAP_IMM] = gtk_ml_i_map_imm,
    [GTKML_I_SET_IMM] = gtk_ml_i_set_imm,
    [GTKML_I_ARRAY_IMM] = gtk_ml_i_array_imm,
    [GTKML_I_SETMM_IMM] = gtk_ml_i_setmm_imm,
    [GTKML_I_GETMM_IMM] = gtk_ml_i_getmm_imm,
    [GTKML_I_VAR] = gtk_ml_i_var,
    [GTKML_I_GETVAR] = gtk_ml_i_getvar,
    [GTKML_I_ASSIGNVAR] = gtk_ml_i_assignvar,
    [GTKML_I_LEN] = gtk_ml_i_len,
    [GTKML_I_INDEX] = gtk_ml_i_index,
    [GTKML_I_ARRAY_PUSH] = gtk_ml_i_array_push,
    [GTKML_I_ARRAY_POP] = gtk_ml_i_array_pop,
    [GTKML_I_ARRAY_CONCAT] = gtk_ml_i_array_concat,
    [GTKML_I_MAP_GET] = gtk_ml_i_map_get,
    [GTKML_I_MAP_INSERT] = gtk_ml_i_map_insert,
    [GTKML_I_MAP_RAWGET] = gtk_ml_i_map_rawget,
    [GTKML_I_MAP_RAWINSERT] = gtk_ml_i_map_rawinsert,
    [GTKML_I_MAP_DELETE] = gtk_ml_i_map_delete,
    [GTKML_I_SET_CONTAINS] = gtk_ml_i_set_contains,
    [GTKML_I_SET_INSERT] = gtk_ml_i_set_insert,
    [GTKML_I_SET_DELETE] = gtk_ml_i_set_delete,
    [GTKML_I_CALL] = gtk_ml_i_call,
    [GTKML_I_LEAVE_RET] = gtk_ml_i_leave_ret,
    [GTKML_I_CALL_CORE] = gtk_ml_i_call_core,
    [GTKML_I_BRANCH_ABSOLUTE] = gtk_ml_i_branch_absolute,
    [GTKML_I_BRANCH_RELATIVE] = gtk_ml_i_branch_relative,
    [255] = (gboolean (*)(GtkMl_Vm *, GtkMl_SObj *, GtkMl_Data)) NULL,
};

GTKML_PRIVATE GtkMl_TaggedValue vm_core_load(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
#ifdef GTKML_ENABLE_GTK
GTKML_PRIVATE GtkMl_TaggedValue vm_core_application(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_new_window(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
#endif /* GTKML_ENABLE_GTK */
GTKML_PRIVATE GtkMl_TaggedValue vm_core_error(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_dbg(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_string_to_symbol(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_allocate(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_to_cstr(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_to_buffer(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_to_array(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_to_string(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_compile_expr(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_emit_bytecode(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_bind_symbol(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_export_symbol(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_append_basic_block(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_global_counter(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_basic_block_name(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
#ifdef GTKML_ENABLE_POSIX
GTKML_PRIVATE GtkMl_TaggedValue vm_core_dbg_run(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_dbg_cont(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_dbg_step(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_dbg_disasm(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_dbg_stack(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
GTKML_PRIVATE GtkMl_TaggedValue vm_core_dbg_backtrace(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr);
#endif /* GTKML_ENABLE_POSIX */

GTKML_PRIVATE GtkMl_TaggedValue (*CORE[])(GtkMl_Context *, GtkMl_SObj *, GtkMl_TaggedValue) = {
    [GTKML_CORE_LOAD] = vm_core_load,
#ifdef GTKML_ENABLE_GTK
    [GTKML_CORE_APPLICATION] = vm_core_application,
    [GTKML_CORE_NEW_WINDOW] = vm_core_new_window,
#endif /* GTKML_ENABLE_GTK */
    [GTKML_CORE_ERROR] = vm_core_error,
    [GTKML_CORE_DBG] = vm_core_dbg,
    [GTKML_CORE_STRING_TO_SYMBOL] = vm_core_string_to_symbol,
    [GTKML_CORE_ALLOCATE] = vm_core_allocate,
    [GTKML_CORE_TO_CSTR] = vm_core_to_cstr,
    [GTKML_CORE_TO_BUFFER] = vm_core_to_buffer,
    [GTKML_CORE_TO_ARRAY] = vm_core_to_array,
    [GTKML_CORE_TO_STRING] = vm_core_to_string,
    [GTKML_CORE_COMPILE_EXPR] = vm_core_compile_expr,
    [GTKML_CORE_EMIT_BYTECODE] = vm_core_emit_bytecode,
    [GTKML_CORE_BIND_SYMBOL] = vm_core_bind_symbol,
    [GTKML_CORE_EXPORT_SYMBOL] = vm_core_export_symbol,
    [GTKML_CORE_APPEND_BASIC_BLOCK] = vm_core_append_basic_block,
    [GTKML_CORE_GLOBAL_COUNTER] = vm_core_global_counter,
    [GTKML_CORE_BASIC_BLOCK_NAME] = vm_core_basic_block_name,
#ifdef GTKML_ENABLE_POSIX
    [GTKML_CORE_DBG_RUN] = vm_core_dbg_run,
    [GTKML_CORE_DBG_CONT] = vm_core_dbg_cont,
    [GTKML_CORE_DBG_STEP] = vm_core_dbg_step,
    [GTKML_CORE_DBG_DISASM] = vm_core_dbg_disasm,
    [GTKML_CORE_DBG_STACK] = vm_core_dbg_stack,
    [GTKML_CORE_DBG_BACKTRACE] = vm_core_dbg_backtrace,
#endif /* GTKML_ENABLE_POSIX */
#ifdef GTKML_ENABLE_WEB
    [GTKML_CORE_WEB_LOG] = vm_core_web_log,
#include "libs/em_gles3/bind-core-libs.h"
#endif /* GTKML_ENABLE_WEB */
};

void gtk_ml_bind(GtkMl_Context *ctx, GtkMl_SObj key, GtkMl_TaggedValue value) {
    GtkMl_SObj new_scope = gtk_ml_new_map(ctx, NULL, NULL);
    gtk_ml_hash_trie_insert(&new_scope->value.s_map.map, &ctx->bindings->value.s_var.expr->value.s_map.map, gtk_ml_value_sobject(key), value);
    ctx->bindings->value.s_var.expr = new_scope;
}

GtkMl_TaggedValue gtk_ml_get(GtkMl_Context *ctx, GtkMl_SObj key) {
    return gtk_ml_hash_trie_get(&ctx->bindings->value.s_var.expr->value.s_map.map, gtk_ml_value_sobject(key));
}

GtkMl_TaggedValue vm_core_load(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) expr;

    GtkMl_SObj module_expr = gtk_ml_pop(ctx).value.sobj;
    (void) gtk_ml_pop(ctx);

    // TODO(walterpi): don't leak this
    char *filename = gtk_ml_to_c_str(module_expr);
    char *src;
    GtkMl_TaggedValue program = gtk_ml_load(ctx, &src, err, filename);
    if (!gtk_ml_has_value(program)) {
        return gtk_ml_value_none();
    }

    if (!gtk_ml_execute(ctx, err, program, GTKML_NO_ARGS, 0)) {
        return gtk_ml_value_none();
    }

    return gtk_ml_pop(ctx);
}

#ifdef GTKML_ENABLE_GTK
GTKML_PRIVATE void activate_program(GtkApplication* app, gpointer userdata) {
    (void) app;

    GtkMl_SObj args = userdata;
    GtkMl_SObj ctx_expr = gtk_ml_car(args);
    GtkMl_SObj app_expr = gtk_ml_cdar(args);
    GtkMl_SObj program_expr = gtk_ml_cddar(args);

    GtkMl_Context *ctx = ctx_expr->value.s_lightdata.userdata;

    GtkMl_SObj err;
    GtkMl_TaggedValue arg = gtk_ml_value_sobject(app_expr);
    if (gtk_ml_execute(ctx, &err, gtk_ml_value_sobject(program_expr), &arg, 1)) {
        GtkMl_SObj result = gtk_ml_pop(ctx).value.sobj;
        app_expr->value.s_userdata.keep = gtk_ml_new_list(ctx, NULL, result, app_expr->value.s_userdata.keep);
    } else {
        (void) gtk_ml_dumpf(ctx, stderr, NULL, err);
    }
}

GtkMl_TaggedValue vm_core_application(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) err;
    (void) expr;

    GtkMl_SObj map_expr = gtk_ml_pop(ctx).value.sobj;
    GtkMl_SObj flags_expr = gtk_ml_pop(ctx).value.sobj;
    GtkMl_SObj id_expr = gtk_ml_pop(ctx).value.sobj;
    GtkMl_SObj application = gtk_ml_pop(ctx).value.sobj;
    (void) application;

    if (id_expr->kind != GTKML_S_ARRAY || !gtk_ml_array_trie_is_string(&id_expr->value.s_array.array)) {
        *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
                gtk_ml_new_keyword(ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(ctx, NULL, 0, "string", strlen("string")),
                gtk_ml_new_keyword(ctx, NULL, 0, "got", strlen("got")), id_expr);
        return gtk_ml_value_none();
    }

    char *id = gtk_ml_to_c_str(id_expr);
    GtkApplication *app = gtk_application_new(id, flags_expr->value.s_int.value);
    free(id);
    GtkMl_SObj app_expr = gtk_ml_new_userdata(ctx, NULL, app, gtk_ml_object_unref);

    GtkMl_TaggedValue activate = gtk_ml_hash_trie_get(&map_expr->value.s_map.map, gtk_ml_value_sobject(gtk_ml_new_keyword(ctx, NULL, 0, "activate", strlen("activate"))));
    if (gtk_ml_has_value(activate)) {
        GtkMl_SObj ctx_expr = gtk_ml_new_lightdata(ctx, NULL, ctx);
        GtkMl_SObj userdata = gtk_ml_new_list(ctx, NULL, ctx_expr, gtk_ml_new_list(ctx, NULL, app_expr, gtk_ml_new_list(ctx, NULL, activate.value.sobj, gtk_ml_new_nil(ctx, NULL))));
        app_expr->value.s_userdata.keep = gtk_ml_new_list(ctx, &app_expr->span, userdata, app_expr->value.s_userdata.keep);
        g_signal_connect(app, "activate", G_CALLBACK(activate_program), userdata);
    }

    return gtk_ml_value_sobject(app_expr);
}

GtkMl_TaggedValue vm_core_new_window(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) err;
    (void) expr;

    GtkMl_SObj height_expr = gtk_ml_pop(ctx).value.sobj;
    GtkMl_SObj width_expr = gtk_ml_pop(ctx).value.sobj;
    GtkMl_SObj title_expr = gtk_ml_pop(ctx).value.sobj;
    GtkMl_SObj app_expr = gtk_ml_pop(ctx).value.sobj;
    GtkMl_SObj new_window = gtk_ml_pop(ctx).value.sobj;
    (void) new_window;

    if (app_expr->kind != GTKML_S_USERDATA) {
        *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 1, gtk_ml_new_keyword(ctx, NULL, 0, "got", strlen("got")), app_expr);
        return gtk_ml_value_none();
    }
    if (!app_expr->value.s_userdata.userdata) {
        *err = gtk_ml_error(ctx, "null-error", GTKML_ERR_NULL_ERROR, 0, 0, 0, 0);
        return gtk_ml_value_none();
    }
    GtkWidget *window = gtk_application_window_new(app_expr->value.s_userdata.userdata);
    char *title = gtk_ml_to_c_str(title_expr);
    gtk_window_set_title(GTK_WINDOW(window), title);
    free(title);
    gtk_window_set_default_size(GTK_WINDOW(window), width_expr->value.s_int.value, height_expr->value.s_int.value);
    gtk_widget_show_all(window);

    return gtk_ml_value_userdata(window);
}
#endif /* GTKML_ENABLE_GTK */

GtkMl_TaggedValue vm_core_error(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) expr;

    GtkMl_SObj error_expr = gtk_ml_pop(ctx).value.sobj;
    GtkMl_SObj error = gtk_ml_pop(ctx).value.sobj;
    (void) error;

    GtkMl_SObj s_has_loc = gtk_ml_new_false(ctx, NULL);
    GtkMl_SObj s_loc = gtk_ml_new_array(ctx, NULL);
    GtkMl_SObj new_loc = gtk_ml_new_array(ctx, NULL);
    gtk_ml_array_trie_push(&new_loc->value.s_array.array, &s_loc->value.s_array.array, gtk_ml_value_sobject(s_has_loc));
    s_loc = new_loc;

    GtkMl_SObj new = gtk_ml_new_map(ctx, NULL, NULL);
    gtk_ml_hash_trie_insert(&new->value.s_map.map, &error_expr->value.s_map.map, gtk_ml_value_sobject(gtk_ml_new_keyword(ctx, NULL, 0, "loc", 3)), gtk_ml_value_sobject(s_loc));
    error_expr = new;

    *err = error_expr;

    return gtk_ml_value_none();
}

GtkMl_TaggedValue vm_core_dbg(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) expr;

    GtkMl_TaggedValue value = gtk_ml_pop(ctx);
    GtkMl_SObj dbg = gtk_ml_pop(ctx).value.sobj;
    (void) dbg;

    gboolean mm_success = 0;
    if (gtk_ml_is_sobject(value) && value.value.sobj->kind == GTKML_S_MAP) {
        GtkMl_TaggedValue metakey = gtk_ml_value_sobject(gtk_ml_new_keyword(ctx, NULL, 0, "__dbg", strlen("__dbg")));
        GtkMl_TaggedValue metafn = gtk_ml_value_none();
        if (value.value.sobj->value.s_map.metamap) {
            metafn = gtk_ml_hash_trie_get(&value.value.sobj->value.s_map.metamap->value.s_map.map, metakey);
        }
        if (gtk_ml_has_value(metafn)) {
            GtkMl_TaggedValue metaarg = value;
            if (!gtk_ml_execute(ctx, err, metafn, &metaarg, 1)) {
                return gtk_ml_value_none();
            }
            gtk_ml_pop(ctx);
            mm_success = 1;
        }
    }

    if (!mm_success) {
        if (!gtk_ml_dumpf_value(ctx, stderr, err, value)) {
            return gtk_ml_value_none();
        }
        fprintf(stderr, "\n");
    }

    return value;
}

GtkMl_TaggedValue vm_core_string_to_symbol(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) expr;
    (void) err;

    GtkMl_SObj str = gtk_ml_pop(ctx).value.sobj;
    if (str->kind != GTKML_S_ARRAY) {
        *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
                gtk_ml_new_keyword(ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(ctx, NULL, 0, "string", strlen("string")),
                gtk_ml_new_keyword(ctx, NULL, 0, "got", strlen("got")), str);
        return gtk_ml_value_none();
    }
    char *c_str = gtk_ml_to_c_str(str);

    GtkMl_SObj string_to_symbol = gtk_ml_pop(ctx).value.sobj;
    (void) string_to_symbol;

    return gtk_ml_value_sobject(gtk_ml_new_symbol(ctx, NULL, 1, c_str, strlen(c_str)));
}

GtkMl_TaggedValue vm_core_allocate(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) expr;
    (void) err;

    GtkMl_TaggedValue tmp = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    size_t size = gtk_ml_prim_to_int(tmp);
    GtkMl_SObj allocate = gtk_ml_pop(ctx).value.sobj;
    (void) allocate;

    void *ptr = malloc(size);

    return gtk_ml_value_sobject(gtk_ml_new_userdata(ctx, NULL, ptr, gtk_ml_free));
}

GtkMl_TaggedValue vm_core_to_cstr(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) expr;
    (void) err;

    GtkMl_SObj array = gtk_ml_pop(ctx).value.sobj;
    GtkMl_SObj to_cstr = gtk_ml_pop(ctx).value.sobj;
    (void) to_cstr;

    char *c_str = gtk_ml_to_c_str(array);

    return gtk_ml_value_sobject(gtk_ml_new_userdata(ctx, NULL, c_str, gtk_ml_free));
}

GtkMl_TaggedValue vm_core_to_buffer(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) expr;

    GtkMl_TaggedValue tmp = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    size_t size = gtk_ml_prim_to_int(tmp);
    GtkMl_SObj array = gtk_ml_pop(ctx).value.sobj;
    GtkMl_SObj to_buffer = gtk_ml_pop(ctx).value.sobj;
    (void) to_buffer;

    size_t len = gtk_ml_array_trie_len(&array->value.s_array.array);
    char *buffer = malloc(size * len);

    for (size_t i = 0; i < gtk_ml_array_trie_len(&array->value.s_array.array); i++) {
        GtkMl_TaggedValue elem = gtk_ml_array_trie_get(&array->value.s_array.array, i);
        if (gtk_ml_is_sobject(elem)) {
            switch (elem.value.sobj->kind) {
            case GTKML_S_NIL:
                break;
            case GTKML_S_TRUE:
                buffer[i * size] = 1;
                break;
            case GTKML_S_FALSE:
                buffer[i * size] = 0;
                break;
            case GTKML_S_CHAR:
                memcpy(buffer + i * size, &elem.value.sobj->value.s_char.value, size);
                break;
            case GTKML_S_INT:
                memcpy(buffer + i * size, &elem.value.sobj->value.s_int.value, size);
                break;
            case GTKML_S_FLOAT:
                if (size == 4) {
                    float flt = elem.value.sobj->value.s_float.value;
                    memcpy(buffer + i * size, &flt, size);
                } else if (size == 8) {
                    double flt = elem.value.sobj->value.s_float.value;
                    memcpy(buffer + i * size, &flt, size);
                } else {
                    memcpy(buffer + i * size, &elem.value.sobj->value.s_float.value, size);
                }
                break;
            case GTKML_S_LIGHTDATA:
                memcpy(buffer + i * size, &elem.value.sobj->value.s_lightdata.userdata, size);
                break;
            case GTKML_S_USERDATA:
                memcpy(buffer + i * size, &elem.value.sobj->value.s_userdata.userdata, size);
                break;
            case GTKML_S_FFI:
                memcpy(buffer + i * size, &elem.value.sobj->value.s_ffi.ffi, size);
                break;
            default:
                *err = gtk_ml_error(ctx, "primitive-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 0);
                return gtk_ml_value_none();
            }
        } else {
            memcpy(buffer + i * size, &elem.value.u64, size);
        }
    }

    return gtk_ml_value_sobject(gtk_ml_new_userdata(ctx, NULL, buffer, gtk_ml_free));
}

GtkMl_TaggedValue vm_core_to_array(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) expr;

    GtkMl_TaggedValue tmp = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    size_t len = gtk_ml_prim_to_int(tmp);
    tmp = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    size_t size = gtk_ml_prim_to_int(tmp);
    GtkMl_SObj type = gtk_ml_pop(ctx).value.sobj;
    GtkMl_TaggedValue buffer = gtk_ml_pop(ctx);
    GtkMl_SObj to_array = gtk_ml_pop(ctx).value.sobj;
    (void) to_array;

    char *ptr;
    if (gtk_ml_is_sobject(buffer)) {
        switch (buffer.value.sobj->kind) {
        case GTKML_S_USERDATA:
            ptr = buffer.value.sobj->value.s_userdata.userdata;
            break;
        case GTKML_S_LIGHTDATA:
            ptr = buffer.value.sobj->value.s_lightdata.userdata;
            break;
        default:
            *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 0);
            return gtk_ml_value_none();
        }
    } else {
        if (buffer.tag != GTKML_TAG_USERDATA) {
            *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 0);
            return gtk_ml_value_none();
        }
        ptr = buffer.value.userdata;
    }

    uint32_t kind;
    if (strncmp(type->value.s_keyword.ptr, "nil", type->value.s_keyword.len) == 0) {
        kind = GTKML_TAG_NIL;
    } else if (strncmp(type->value.s_keyword.ptr, "bool", type->value.s_keyword.len) == 0) {
        kind = GTKML_TAG_BOOL;
    } else if (strncmp(type->value.s_keyword.ptr, "char", type->value.s_keyword.len) == 0) {
        kind = GTKML_TAG_CHAR;
    } else if (strncmp(type->value.s_keyword.ptr, "float", type->value.s_keyword.len) == 0) {
        kind = GTKML_TAG_FLOAT;
    } else if (strncmp(type->value.s_keyword.ptr, "int", type->value.s_keyword.len) == 0) {
        kind = GTKML_TAG_INT;
    } else if (strncmp(type->value.s_keyword.ptr, "userdata", type->value.s_keyword.len) == 0) {
        kind = GTKML_TAG_USERDATA;
    } else if (strncmp(type->value.s_keyword.ptr, "ffi", type->value.s_keyword.len) == 0) {
        kind = GTKML_TAG_FFI;
    } else {
        *err = gtk_ml_error(ctx, "primitive-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 0);
        return gtk_ml_value_none();
    }
    GtkMl_SObj array = gtk_ml_new_array(ctx, NULL);

    for (size_t i = 0; i < len; i++) {
        GtkMl_SObj new = gtk_ml_new_array(ctx, NULL);
        gtk_ml_del_array_trie(ctx, &new->value.s_array.array, gtk_ml_delete_value);
        switch (kind) {
        case GTKML_TAG_NIL:
            gtk_ml_array_trie_push(&new->value.s_array.array, &array->value.s_array.array, gtk_ml_value_nil());
            break;
        case GTKML_TAG_BOOL: {
            gboolean value = *(gboolean *) (ptr + i * size);
            gtk_ml_array_trie_push(&new->value.s_array.array, &array->value.s_array.array, value? gtk_ml_value_true() : gtk_ml_value_false());
        } break;
        case GTKML_TAG_CHAR: {
            if (size == 1) {
                uint8_t value = *(uint8_t *) (ptr + i * size);
                gtk_ml_array_trie_push(&new->value.s_array.array, &array->value.s_array.array, gtk_ml_value_char(value));
            } else if (size == 2) {
                uint16_t value = *(uint16_t *) (ptr + i * size);
                gtk_ml_array_trie_push(&new->value.s_array.array, &array->value.s_array.array, gtk_ml_value_char(value));
            } else {
                uint32_t value = *(uint32_t *) (ptr + i * size);
                gtk_ml_array_trie_push(&new->value.s_array.array, &array->value.s_array.array, gtk_ml_value_char(value));
            }
        } break;
        case GTKML_TAG_INT: {
            if (size == 1) {
                int8_t value = *(int8_t *) (ptr + i * size);
                gtk_ml_array_trie_push(&new->value.s_array.array, &array->value.s_array.array, gtk_ml_value_int(value));
            } else if (size == 2) {
                int16_t value = *(int16_t *) (ptr + i * size);
                gtk_ml_array_trie_push(&new->value.s_array.array, &array->value.s_array.array, gtk_ml_value_int(value));
            } else if (size == 4) {
                int32_t value = *(int32_t *) (ptr + i * size);
                gtk_ml_array_trie_push(&new->value.s_array.array, &array->value.s_array.array, gtk_ml_value_int(value));
            } else {
                int64_t value = *(int64_t *) (ptr + i * size);
                gtk_ml_array_trie_push(&new->value.s_array.array, &array->value.s_array.array, gtk_ml_value_int(value));
            }
        } break;
        case GTKML_TAG_FLOAT:
            if (size == 4) {
                float value = *(float *) (ptr + i * size);
                gtk_ml_array_trie_push(&new->value.s_array.array, &array->value.s_array.array, gtk_ml_value_float(value));
            } else {
                double value = *(double *) (ptr + i * size);
                gtk_ml_array_trie_push(&new->value.s_array.array, &array->value.s_array.array, gtk_ml_value_float(value));
            }
            break;
        case GTKML_TAG_USERDATA: {
            void *value = *(void **) (ptr + i * size);
            gtk_ml_array_trie_push(&new->value.s_array.array, &array->value.s_array.array, gtk_ml_value_userdata(value));
        } break;
        case GTKML_TAG_FFI: {
            GtkMl_Ffi value;
            *(void **) (&value) = *(void **) (ptr + i * size);
            gtk_ml_array_trie_push(&new->value.s_array.array, &array->value.s_array.array, gtk_ml_value_ffi(value));
        } break;
        }
        array = new;
    }

    return gtk_ml_value_sobject(array);
}

GtkMl_TaggedValue vm_core_to_string(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) expr;

    GtkMl_TaggedValue tmp = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    size_t len = gtk_ml_prim_to_int(tmp);
    GtkMl_TaggedValue buffer = gtk_ml_pop(ctx);
    GtkMl_SObj to_array = gtk_ml_pop(ctx).value.sobj;
    (void) to_array;

    char *ptr;
    if (gtk_ml_is_sobject(buffer)) {
        switch (buffer.value.sobj->kind) {
        case GTKML_S_USERDATA:
            ptr = buffer.value.sobj->value.s_userdata.userdata;
            break;
        case GTKML_S_LIGHTDATA:
            ptr = buffer.value.sobj->value.s_lightdata.userdata;
            break;
        default:
            *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 0);
            return gtk_ml_value_none();
        }
    } else {
        if (buffer.tag != GTKML_TAG_USERDATA) {
            *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 0);
            return gtk_ml_value_none();
        }
        ptr = buffer.value.userdata;
    }

    GtkMl_SObj string = gtk_ml_new_string(ctx, NULL, ptr, len);

    return gtk_ml_value_sobject(string);
}

GtkMl_TaggedValue vm_core_compile_expr(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) expr;
    GtkMl_SObj arg = gtk_ml_pop(ctx).value.sobj;
    GtkMl_TaggedValue tv_basic_block = gtk_ml_pop(ctx);
    if (!gtk_ml_is_sobject(tv_basic_block) || tv_basic_block.value.sobj->kind != GTKML_S_VAR) {
        *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 1,
                gtk_ml_new_keyword(ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(ctx, NULL, 0, "var", strlen("var")));
        return gtk_ml_value_none();
    }
    GtkMl_SObj arg_bb_var = tv_basic_block.value.sobj;
    if (arg_bb_var->value.s_var.expr->kind != GTKML_S_LIGHTDATA) {
        *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
                gtk_ml_new_keyword(ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(ctx, NULL, 0, "lightdata", strlen("lightdata")),
                gtk_ml_new_keyword(ctx, NULL, 0, "got", strlen("got")), arg_bb_var->value.s_var.expr);
        return gtk_ml_value_none();
    }
    GtkMl_BasicBlock *arg_basic_block = arg_bb_var->value.s_var.expr->value.s_lightdata.userdata;
    GtkMl_TaggedValue tv_b = gtk_ml_pop(ctx);
    GtkMl_Builder *arg_b;
    if (gtk_ml_is_primitive(tv_b)) {
        arg_b = tv_b.value.userdata;
    } else {
        if (tv_b.value.sobj->kind != GTKML_S_LIGHTDATA) {
            *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
                gtk_ml_new_keyword(ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(ctx, NULL, 0, "lightdata", strlen("lightdata")),
                gtk_ml_new_keyword(ctx, NULL, 0, "got", strlen("got")), tv_b.value.sobj);
            return gtk_ml_value_none();
        }
        arg_b = tv_b.value.sobj->value.s_lightdata.userdata;
    }
    GtkMl_TaggedValue tv_ctx = gtk_ml_pop(ctx);
    GtkMl_Context *arg_ctx;
    if (gtk_ml_is_primitive(tv_ctx)) {
        arg_ctx = tv_ctx.value.userdata;
    } else {
        if (tv_ctx.value.sobj->kind != GTKML_S_LIGHTDATA) {
            *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
                gtk_ml_new_keyword(ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(ctx, NULL, 0, "lightdata", strlen("lightdata")),
                gtk_ml_new_keyword(ctx, NULL, 0, "got", strlen("got")), tv_ctx.value.sobj);
            return gtk_ml_value_none();
        }
        arg_ctx = tv_ctx.value.sobj->value.s_lightdata.userdata;
    }
    GtkMl_SObj compile_expr = gtk_ml_pop(ctx).value.sobj;
    (void) compile_expr;

    GtkMl_BasicBlock *_bb = arg_basic_block;
    if (!gtk_ml_compile_expression(arg_ctx, arg_b, &_bb, err, &arg, 0, 0, 1, 1)) {
        return gtk_ml_value_none();
    }

    if (_bb != arg_basic_block) {
        arg_bb_var->value.s_var.expr = gtk_ml_new_lightdata(ctx, NULL, _bb);
    }

    return gtk_ml_value_true();
}

GtkMl_TaggedValue vm_core_emit_bytecode(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    GtkMl_TaggedValue imm = gtk_ml_value_none();
    if (expr.value.u64 == 7) {
        imm = gtk_ml_pop(ctx);
    }

    GtkMl_SObj bc = gtk_ml_pop(ctx).value.sobj;
    unsigned int arg_cond = gtk_ml_pop(ctx).value.sobj->value.s_int.value;
    GtkMl_TaggedValue tv_basic_block = gtk_ml_pop(ctx);
    GtkMl_BasicBlock *arg_basic_block;
    if (gtk_ml_is_primitive(tv_basic_block)) {
        arg_basic_block = tv_basic_block.value.userdata;
    } else {
        if (tv_basic_block.value.sobj->kind != GTKML_S_LIGHTDATA) {
            *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
                gtk_ml_new_keyword(ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(ctx, NULL, 0, "lightdata", strlen("lightdata")),
                gtk_ml_new_keyword(ctx, NULL, 0, "got", strlen("got")), tv_basic_block.value.sobj);
            return gtk_ml_value_none();
        }
        arg_basic_block = tv_basic_block.value.sobj->value.s_lightdata.userdata;
    }
    GtkMl_TaggedValue tv_b = gtk_ml_pop(ctx);
    GtkMl_Builder *arg_b;
    if (gtk_ml_is_primitive(tv_b)) {
        arg_b = tv_b.value.userdata;
    } else {
        if (tv_b.value.sobj->kind != GTKML_S_LIGHTDATA) {
            *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
                gtk_ml_new_keyword(ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(ctx, NULL, 0, "lightdata", strlen("lightdata")),
                gtk_ml_new_keyword(ctx, NULL, 0, "got", strlen("got")), tv_b.value.sobj);
            return gtk_ml_value_none();
        }
        arg_b = tv_b.value.sobj->value.s_lightdata.userdata;
    }
    GtkMl_TaggedValue tv_ctx = gtk_ml_pop(ctx);
    GtkMl_Context *arg_ctx;
    if (gtk_ml_is_primitive(tv_ctx)) {
        arg_ctx = tv_ctx.value.userdata;
    } else {
        if (tv_ctx.value.sobj->kind != GTKML_S_LIGHTDATA) {
            *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
                gtk_ml_new_keyword(ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(ctx, NULL, 0, "lightdata", strlen("lightdata")),
                gtk_ml_new_keyword(ctx, NULL, 0, "got", strlen("got")), tv_ctx.value.sobj);
            return gtk_ml_value_none();
        }
        arg_ctx = tv_ctx.value.sobj->value.s_lightdata.userdata;
    }
    const char *ptr = bc->value.s_keyword.ptr;
    size_t len = bc->value.s_keyword.len;
    GtkMl_SObj emit_bytecode = gtk_ml_pop(ctx).value.sobj;
    (void) emit_bytecode;

    GtkMl_TaggedValue data = gtk_ml_value_none();
    if (expr.value.u64 == 7 && gtk_ml_is_sobject(imm)) {
        data = gtk_ml_value_sobject((GtkMl_SObj) gtk_ml_append_static(arg_b, imm.value.sobj));
    }

    gtk_ml_builder_set_cond(arg_b, arg_cond);
    if (strlen(GTKML_SI_PUSH_IMM) == len && strncmp(ptr, GTKML_SI_PUSH_IMM, len) == 0) {
        if (!gtk_ml_has_value(data)) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return gtk_ml_value_none();
        }
        return gtk_ml_build_push_imm(arg_ctx, arg_b, arg_basic_block, err, gtk_ml_append_data(arg_b, data))? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_PUSH_IMM) == len && strncmp(ptr, GTKML_SI_PUSH_IMM, len) == 0) {
        if (!gtk_ml_has_value(data)) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return gtk_ml_value_none();
        }
        return gtk_ml_build_push_addr(arg_ctx, arg_b, arg_basic_block, err, gtk_ml_append_data(arg_b, data))? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_POP) == len && strncmp(ptr, GTKML_SI_POP, len) == 0) {
        return gtk_ml_build_pop(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_SETF_IMM) == len && strncmp(ptr, GTKML_SI_SETF_IMM, len) == 0) {
        if (!gtk_ml_has_value(data)) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return gtk_ml_value_none();
        }
        return gtk_ml_build_setf_imm(arg_ctx, arg_b, arg_basic_block, err, gtk_ml_append_data(arg_b, data))? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_POPF) == len && strncmp(ptr, GTKML_SI_POPF, len) == 0) {
        return gtk_ml_build_popf(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_DEFINE) == len && strncmp(ptr, GTKML_SI_DEFINE, len) == 0) {
        return gtk_ml_build_define(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_CAR) == len && strncmp(ptr, GTKML_SI_CAR, len) == 0) {
        return gtk_ml_build_car(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_CDR) == len && strncmp(ptr, GTKML_SI_CDR, len) == 0) {
        return gtk_ml_build_cdr(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_CONS) == len && strncmp(ptr, GTKML_SI_CONS, len) == 0) {
        return gtk_ml_build_cons(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_MAP) == len && strncmp(ptr, GTKML_SI_MAP, len) == 0) {
        return gtk_ml_build_map(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_SET) == len && strncmp(ptr, GTKML_SI_SET, len) == 0) {
        return gtk_ml_build_set(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_ARRAY) == len && strncmp(ptr, GTKML_SI_ARRAY, len) == 0) {
        return gtk_ml_build_array(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_LIST) == len && strncmp(ptr, GTKML_SI_LIST, len) == 0) {
        return gtk_ml_build_list(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_ENTER) == len && strncmp(ptr, GTKML_SI_ENTER, len) == 0) {
        return gtk_ml_build_enter(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_LEAVE) == len && strncmp(ptr, GTKML_SI_LEAVE, len) == 0) {
        return gtk_ml_build_leave(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_TYPEOF) == len && strncmp(ptr, GTKML_SI_TYPEOF, len) == 0) {
        return gtk_ml_build_typeof(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_TO_SOBJ) == len && strncmp(ptr, GTKML_SI_TO_SOBJ, len) == 0) {
        return gtk_ml_build_to_sobject(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_TO_PRIM) == len && strncmp(ptr, GTKML_SI_TO_PRIM, len) == 0) {
        return gtk_ml_build_to_prim(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_LOCAL_IMM) == len && strncmp(ptr, GTKML_SI_LOCAL_IMM, len) == 0) {
        if (!gtk_ml_has_value(data)) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return gtk_ml_value_none();
        }
        return gtk_ml_build_local_imm(arg_ctx, arg_b, arg_basic_block, err, gtk_ml_append_data(arg_b, data))? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_BIND) == len && strncmp(ptr, GTKML_SI_BIND, len) == 0) {
        return gtk_ml_build_bind(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_ENTER_BIND_ARGS) == len && strncmp(ptr, GTKML_SI_ENTER_BIND_ARGS, len) == 0) {
        return gtk_ml_build_bind_args(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_GET_IMM) == len && strncmp(ptr, GTKML_SI_GET_IMM, len) == 0) {
        if (!gtk_ml_has_value(data)) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return gtk_ml_value_none();
        }
        return gtk_ml_build_get_imm(arg_ctx, arg_b, arg_basic_block, err, gtk_ml_append_data(arg_b, data))? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_LIST_IMM) == len && strncmp(ptr, GTKML_SI_LIST_IMM, len) == 0) {
        if (!gtk_ml_has_value(data)) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return gtk_ml_value_none();
        }
        return gtk_ml_build_list_imm(arg_ctx, arg_b, arg_basic_block, err, gtk_ml_append_data(arg_b, data))? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_MAP_IMM) == len && strncmp(ptr, GTKML_SI_MAP_IMM, len) == 0) {
        if (!gtk_ml_has_value(data)) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return gtk_ml_value_none();
        }
        return gtk_ml_build_map_imm(arg_ctx, arg_b, arg_basic_block, err, gtk_ml_append_data(arg_b, data))? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_SET_IMM) == len && strncmp(ptr, GTKML_SI_SET_IMM, len) == 0) {
        if (!gtk_ml_has_value(data)) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return gtk_ml_value_none();
        }
        return gtk_ml_build_set_imm(arg_ctx, arg_b, arg_basic_block, err, gtk_ml_append_data(arg_b, data))? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_ARRAY_IMM) == len && strncmp(ptr, GTKML_SI_ARRAY_IMM, len) == 0) {
        if (!gtk_ml_has_value(data)) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return gtk_ml_value_none();
        }
        return gtk_ml_build_array_imm(arg_ctx, arg_b, arg_basic_block, err, gtk_ml_append_data(arg_b, data))? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_SETMM_IMM) == len && strncmp(ptr, GTKML_SI_SETMM_IMM, len) == 0) {
        return gtk_ml_build_setmm_imm(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_GETMM_IMM) == len && strncmp(ptr, GTKML_SI_GETMM_IMM, len) == 0) {
        return gtk_ml_build_getmm_imm(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_VAR) == len && strncmp(ptr, GTKML_SI_VAR, len) == 0) {
        return gtk_ml_build_var(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_GETVAR) == len && strncmp(ptr, GTKML_SI_GETVAR, len) == 0) {
        return gtk_ml_build_getvar(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_ASSIGNVAR) == len && strncmp(ptr, GTKML_SI_ASSIGNVAR, len) == 0) {
        return gtk_ml_build_assignvar(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_LEN) == len && strncmp(ptr, GTKML_SI_LEN, len) == 0) {
        return gtk_ml_build_len(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_INDEX) == len && strncmp(ptr, GTKML_SI_INDEX, len) == 0) {
        return gtk_ml_build_index(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_ARRAY_PUSH) == len && strncmp(ptr, GTKML_SI_ARRAY_PUSH, len) == 0) {
        return gtk_ml_build_array_push(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_ARRAY_POP) == len && strncmp(ptr, GTKML_SI_ARRAY_POP, len) == 0) {
        return gtk_ml_build_array_pop(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_ARRAY_CONCAT) == len && strncmp(ptr, GTKML_SI_ARRAY_CONCAT, len) == 0) {
        return gtk_ml_build_array_concat(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_MAP_GET) == len && strncmp(ptr, GTKML_SI_MAP_GET, len) == 0) {
        return gtk_ml_build_map_get(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_MAP_INSERT) == len && strncmp(ptr, GTKML_SI_MAP_INSERT, len) == 0) {
        return gtk_ml_build_map_insert(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_MAP_RAWGET) == len && strncmp(ptr, GTKML_SI_MAP_RAWGET, len) == 0) {
        return gtk_ml_build_map_rawget(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_MAP_RAWINSERT) == len && strncmp(ptr, GTKML_SI_MAP_RAWINSERT, len) == 0) {
        return gtk_ml_build_map_rawinsert(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_MAP_DELETE) == len && strncmp(ptr, GTKML_SI_MAP_DELETE, len) == 0) {
        return gtk_ml_build_map_delete(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_SET_CONTAINS) == len && strncmp(ptr, GTKML_SI_SET_CONTAINS, len) == 0) {
        return gtk_ml_build_set_contains(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_SET_INSERT) == len && strncmp(ptr, GTKML_SI_SET_INSERT, len) == 0) {
        return gtk_ml_build_set_insert(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_SET_DELETE) == len && strncmp(ptr, GTKML_SI_SET_DELETE, len) == 0) {
        return gtk_ml_build_set_delete(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_CALL_CORE) == len && strncmp(ptr, GTKML_SI_CALL_CORE, len) == 0) {
        if (!gtk_ml_has_value(data)) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return gtk_ml_value_none();
        }
        return gtk_ml_build_call_core(arg_ctx, arg_b, arg_basic_block, err, gtk_ml_append_data(arg_b, data))? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_CALL) == len && strncmp(ptr, GTKML_SI_CALL, len) == 0) {
        return gtk_ml_build_call(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_BRANCH_ABSOLUTE) == len && strncmp(ptr, GTKML_SI_BRANCH_ABSOLUTE, len) == 0) {
        if (!gtk_ml_has_value(data)) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return gtk_ml_value_none();
        }
        return gtk_ml_build_branch_absolute(arg_ctx, arg_b, arg_basic_block, err, gtk_ml_append_data(arg_b, data))? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_LEAVE_RET) == len && strncmp(ptr, GTKML_SI_LEAVE_RET, len) == 0) {
        return gtk_ml_build_ret(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_ADD) == len && strncmp(ptr, GTKML_SI_ADD, len) == 0) {
        return gtk_ml_build_add(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_SUBTRACT) == len && strncmp(ptr, GTKML_SI_SUBTRACT, len) == 0) {
        return gtk_ml_build_sub(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_SIGNED_MULTIPLY) == len && strncmp(ptr, GTKML_SI_SIGNED_MULTIPLY, len) == 0) {
        return gtk_ml_build_mul(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_SIGNED_DIVIDE) == len && strncmp(ptr, GTKML_SI_SIGNED_DIVIDE, len) == 0) {
        return gtk_ml_build_div(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_SIGNED_MODULO) == len && strncmp(ptr, GTKML_SI_SIGNED_MULTIPLY, len) == 0) {
        return gtk_ml_build_mod(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_BIT_AND) == len && strncmp(ptr, GTKML_SI_BIT_AND, len) == 0) {
        return gtk_ml_build_bitand(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_BIT_OR) == len && strncmp(ptr, GTKML_SI_BIT_OR, len) == 0) {
        return gtk_ml_build_bitor(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_BIT_XOR) == len && strncmp(ptr, GTKML_SI_BIT_XOR, len) == 0) {
        return gtk_ml_build_bitxor(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_value_true() : gtk_ml_value_none();
    } else if (strlen(GTKML_SI_CMP_IMM) == len && strncmp(ptr, GTKML_SI_CMP_IMM, len) == 0) {
        if (!gtk_ml_has_value(imm)) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return gtk_ml_value_none();
        }
        return gtk_ml_build_cmp(arg_ctx, arg_b, arg_basic_block, err, gtk_ml_append_data(arg_b, gtk_ml_to_prim(ctx, err, imm)))? gtk_ml_value_true() : gtk_ml_value_none();
    } else {
        *err = gtk_ml_error(ctx, "bytecode-error", GTKML_ERR_BYTECODE_ERROR, bc->span.ptr != NULL, bc->span.line, bc->span.col, 0);
        return gtk_ml_value_none();
    }
}

GtkMl_TaggedValue vm_core_bind_symbol(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) expr;

    GtkMl_TaggedValue tv_key = gtk_ml_pop(ctx);
    GtkMl_TaggedValue tv_b = gtk_ml_pop(ctx);
    GtkMl_Builder *arg_b;
    if (gtk_ml_is_primitive(tv_b)) {
        arg_b = tv_b.value.userdata;
    } else {
        if (tv_b.value.sobj->kind != GTKML_S_LIGHTDATA) {
            *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
                gtk_ml_new_keyword(ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(ctx, NULL, 0, "lightdata", strlen("lightdata")),
                gtk_ml_new_keyword(ctx, NULL, 0, "got", strlen("got")), tv_b.value.sobj);
            return gtk_ml_value_none();
        }
        arg_b = tv_b.value.sobj->value.s_lightdata.userdata;
    }
    GtkMl_TaggedValue tv_ctx = gtk_ml_pop(ctx);
    GtkMl_Context *arg_ctx;
    if (gtk_ml_is_primitive(tv_ctx)) {
        arg_ctx = tv_ctx.value.userdata;
    } else {
        if (tv_ctx.value.sobj->kind != GTKML_S_LIGHTDATA) {
            *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
                gtk_ml_new_keyword(ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(ctx, NULL, 0, "lightdata", strlen("lightdata")),
                gtk_ml_new_keyword(ctx, NULL, 0, "got", strlen("got")), tv_ctx.value.sobj);
            return gtk_ml_value_none();
        }
        arg_ctx = tv_ctx.value.sobj->value.s_lightdata.userdata;
    }
    GtkMl_SObj bind_symbol = gtk_ml_pop(ctx).value.sobj;
    (void) bind_symbol;

    gtk_ml_builder_bind(arg_ctx, arg_b, gtk_ml_to_sobj(ctx, err, tv_key).value.sobj, gtk_ml_builder_alloca(arg_ctx, arg_b));

    return gtk_ml_value_true();
}

GtkMl_TaggedValue vm_core_export_symbol(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) err;
    (void) expr;

    GtkMl_TaggedValue tv_basic_block = gtk_ml_pop(ctx);
    GtkMl_BasicBlock *arg_basic_block;
    if (gtk_ml_is_primitive(tv_basic_block)) {
        arg_basic_block = tv_basic_block.value.userdata;
    } else {
        if (tv_basic_block.value.sobj->kind != GTKML_S_LIGHTDATA) {
            *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
                gtk_ml_new_keyword(ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(ctx, NULL, 0, "lightdata", strlen("lightdata")),
                gtk_ml_new_keyword(ctx, NULL, 0, "got", strlen("got")), tv_basic_block.value.sobj);
            return gtk_ml_value_none();
        }
        arg_basic_block = tv_basic_block.value.sobj->value.s_lightdata.userdata;
    }
    GtkMl_TaggedValue tv_b = gtk_ml_pop(ctx);
    GtkMl_Builder *arg_b;
    if (gtk_ml_is_primitive(tv_b)) {
        arg_b = tv_b.value.userdata;
    } else {
        if (tv_b.value.sobj->kind != GTKML_S_LIGHTDATA) {
            *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
                gtk_ml_new_keyword(ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(ctx, NULL, 0, "lightdata", strlen("lightdata")),
                gtk_ml_new_keyword(ctx, NULL, 0, "got", strlen("got")), tv_b.value.sobj);
            return gtk_ml_value_none();
        }
        arg_b = tv_b.value.sobj->value.s_lightdata.userdata;
    }
    GtkMl_TaggedValue tv_ctx = gtk_ml_pop(ctx);
    GtkMl_Context *arg_ctx;
    if (gtk_ml_is_primitive(tv_ctx)) {
        arg_ctx = tv_ctx.value.userdata;
    } else {
        if (tv_ctx.value.sobj->kind != GTKML_S_LIGHTDATA) {
            *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
                gtk_ml_new_keyword(ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(ctx, NULL, 0, "lightdata", strlen("lightdata")),
                gtk_ml_new_keyword(ctx, NULL, 0, "got", strlen("got")), tv_ctx.value.sobj);
            return gtk_ml_value_none();
        }
        arg_ctx = tv_ctx.value.sobj->value.s_lightdata.userdata;
    }
    (void) arg_ctx;
    GtkMl_SObj export_symbol = gtk_ml_pop(ctx).value.sobj;
    (void) export_symbol;

    if (arg_basic_block->len_text == arg_basic_block->cap_text) {
        arg_basic_block->cap_text *= 2;
        arg_basic_block->text = realloc(arg_basic_block->text, sizeof(GtkMl_Instruction) * arg_basic_block->cap_text);
    }

    GtkMl_SObj address = gtk_ml_new_address(ctx, NULL, arg_basic_block->name, 0);
    arg_basic_block->text[arg_basic_block->len_text].cond = 0;
    arg_basic_block->text[arg_basic_block->len_text].category = GTKML_I_EXPORT;
    arg_basic_block->text[arg_basic_block->len_text].opcode = 0;
    arg_basic_block->text[arg_basic_block->len_text].data = gtk_ml_append_static_data(arg_b, address);
    ++arg_basic_block->len_text;

    return gtk_ml_value_sobject(address);
}

GtkMl_TaggedValue vm_core_append_basic_block(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) expr;
    (void) err;

    GtkMl_SObj arg_name = gtk_ml_pop(ctx).value.sobj;
    GtkMl_TaggedValue tv_b = gtk_ml_pop(ctx);
    GtkMl_Builder *arg_b;
    if (gtk_ml_is_primitive(tv_b)) {
        arg_b = tv_b.value.userdata;
    } else {
        if (tv_b.value.sobj->kind != GTKML_S_LIGHTDATA) {
            *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
                gtk_ml_new_keyword(ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(ctx, NULL, 0, "lightdata", strlen("lightdata")),
                gtk_ml_new_keyword(ctx, NULL, 0, "got", strlen("got")), tv_b.value.sobj);
            return gtk_ml_value_none();
        }
        arg_b = tv_b.value.sobj->value.s_lightdata.userdata;
    }
    char *name = gtk_ml_to_c_str(arg_name);
    GtkMl_SObj append_basic_block = gtk_ml_pop(ctx).value.sobj;
    (void) append_basic_block;

    GtkMl_BasicBlock *bb = gtk_ml_append_basic_block(arg_b, name);

    return gtk_ml_value_userdata(bb);
}

GtkMl_TaggedValue vm_core_global_counter(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) expr;
    (void) err;

    GtkMl_TaggedValue tv_b = gtk_ml_pop(ctx);
    GtkMl_Builder *arg_b;
    if (gtk_ml_is_primitive(tv_b)) {
        arg_b = tv_b.value.userdata;
    } else {
        if (tv_b.value.sobj->kind != GTKML_S_LIGHTDATA) {
            *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
                gtk_ml_new_keyword(ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(ctx, NULL, 0, "lightdata", strlen("lightdata")),
                gtk_ml_new_keyword(ctx, NULL, 0, "got", strlen("got")), tv_b.value.sobj);
            return gtk_ml_value_none();
        }
        arg_b = tv_b.value.sobj->value.s_lightdata.userdata;
    }

    GtkMl_SObj global_counter = gtk_ml_pop(ctx).value.sobj;
    (void) global_counter;

    return gtk_ml_value_sobject(arg_b->counter);
}

GtkMl_TaggedValue vm_core_basic_block_name(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) expr;
    (void) err;

    GtkMl_TaggedValue tv_bb = gtk_ml_pop(ctx);
    GtkMl_BasicBlock *arg_bb;
    if (gtk_ml_is_primitive(tv_bb)) {
        arg_bb = tv_bb.value.userdata;
    } else {
        if (tv_bb.value.sobj->kind != GTKML_S_LIGHTDATA) {
            *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
                gtk_ml_new_keyword(ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(ctx, NULL, 0, "lightdata", strlen("lightdata")),
                gtk_ml_new_keyword(ctx, NULL, 0, "got", strlen("got")), tv_bb.value.sobj);
            return gtk_ml_value_none();
        }
        arg_bb = tv_bb.value.sobj->value.s_lightdata.userdata;
    }

    GtkMl_SObj basic_block_name = gtk_ml_pop(ctx).value.sobj;
    (void) basic_block_name;

    return gtk_ml_value_sobject(gtk_ml_new_string(ctx, NULL, arg_bb->name, strlen(arg_bb->name)));
}

#ifdef GTKML_ENABLE_POSIX
GtkMl_TaggedValue vm_core_dbg_run(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) expr;

    if (!ctx->is_debugger) {
        *err = gtk_ml_error(ctx, "debugger-error", GTKML_ERR_DEBUGGER_ERROR, 0, 0, 0, 0);
        return gtk_ml_value_none();
    }

    gtk_ml_dbg_write_boolean(ctx, err, &ctx->dbg_ctx->enable_breakpoint, 0);
    if (*err) {
        return gtk_ml_value_none();
    }

    ptrace(PTRACE_CONT, ctx->dbg_process, NULL, NULL);

    int wait_status;
    waitpid(ctx->dbg_process, &wait_status, 0);

    return gtk_ml_value_nil();
}

GtkMl_TaggedValue vm_core_dbg_cont(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) expr;

    if (!ctx->is_debugger) {
        *err = gtk_ml_error(ctx, "debugger-error", GTKML_ERR_DEBUGGER_ERROR, 0, 0, 0, 0);
        return gtk_ml_value_none();
    }

    gtk_ml_dbg_write_boolean(ctx, err, &ctx->dbg_ctx->enable_breakpoint, 0);
    if (*err) {
        return gtk_ml_value_none();
    }

    ptrace(PTRACE_CONT, ctx->dbg_process, NULL, NULL);

    int wait_status;
    waitpid(ctx->dbg_process, &wait_status, 0);

    return gtk_ml_value_nil();
}

GtkMl_TaggedValue vm_core_dbg_step(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) expr;

    if (!ctx->is_debugger) {
        *err = gtk_ml_error(ctx, "debugger-error", GTKML_ERR_DEBUGGER_ERROR, 0, 0, 0, 0);
        return gtk_ml_value_none();
    }

    gtk_ml_dbg_write_boolean(ctx, err, &ctx->dbg_ctx->enable_breakpoint, 1);
    if (*err) {
        return gtk_ml_value_none();
    }

    ptrace(PTRACE_CONT, ctx->dbg_process, NULL, NULL);

    int wait_status;
    waitpid(ctx->dbg_process, &wait_status, 0);

    GtkMl_SObj _err;
    if (!gtk_ml_dumpf_line_debug(ctx, stderr, &_err)) {
        if (!gtk_ml_dumpf(ctx, stderr, err, _err)) {
            return gtk_ml_value_none();
        }
        fprintf(stderr, "\n");
    }

    return gtk_ml_value_nil();
}

GtkMl_TaggedValue vm_core_dbg_disasm(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) expr;

    if (!ctx->is_debugger) {
        *err = gtk_ml_error(ctx, "debugger-error", GTKML_ERR_DEBUGGER_ERROR, 0, 0, 0, 0);
        return gtk_ml_value_none();
    }

    GtkMl_SObj _err;
    if (!gtk_ml_dumpf_program_debug(ctx, stderr, &_err)) {
        if (!gtk_ml_dumpf(ctx, stderr, err, _err)) {
            return gtk_ml_value_none();
        }
        fprintf(stderr, "\n");
    }

    return gtk_ml_value_nil();
}

GtkMl_TaggedValue vm_core_dbg_stack(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) expr;

    if (!ctx->is_debugger) {
        *err = gtk_ml_error(ctx, "debugger-error", GTKML_ERR_DEBUGGER_ERROR, 0, 0, 0, 0);
        return gtk_ml_value_none();
    }

    GtkMl_SObj _err;
    if (!gtk_ml_dumpf_stack_debug(ctx, stderr, &_err)) {
        if (!gtk_ml_dumpf(ctx, stderr, err, _err)) {
            return gtk_ml_value_none();
        }
        fprintf(stderr, "\n");
    }

    return gtk_ml_value_nil();
}

GtkMl_TaggedValue vm_core_dbg_backtrace(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    (void) expr;

    if (!ctx->is_debugger) {
        *err = gtk_ml_error(ctx, "debugger-error", GTKML_ERR_DEBUGGER_ERROR, 0, 0, 0, 0);
        return gtk_ml_value_none();
    }

    GtkMl_SObj _err;
    if (!gtk_ml_backtracef_debug(ctx, stderr, &_err)) {
        if (!gtk_ml_dumpf(ctx, stderr, err, _err)) {
            return gtk_ml_value_none();
        }
        fprintf(stderr, "\n");
    }

    return gtk_ml_value_nil();
}
#endif /* GTKML_ENABLE_POSIX */

GtkMl_Vm *gtk_ml_new_vm(GtkMl_Context *ctx) {
    GtkMl_Vm *vm = malloc(sizeof(GtkMl_Vm));

    vm->stack = malloc(sizeof(GtkMl_TaggedValue) * GTKML_VM_STACK);
    vm->stack_len = 0;
    vm->stack_cap = GTKML_VM_STACK;

    vm->local = malloc(sizeof(GtkMl_TaggedValue) * GTKML_VM_STACK);
    vm->local_len = 0;
    vm->local_cap = GTKML_VM_STACK;
    vm->local_base = 0;

    vm->base_stack = malloc(sizeof(uint64_t) * GTKML_VM_STACK);
    vm->base_stack_ptr = 0;
    vm->base_stack_cap = GTKML_VM_STACK;

    vm->call_stack = malloc(sizeof(uint64_t) * GTKML_VM_CALL_STACK);
    vm->call_stack_ptr = 0;
    vm->call_stack_cap = GTKML_VM_CALL_STACK;

    vm->program = NULL;

    vm->flags = GTKML_F_NONE;
    vm->pc = 0;

    vm->core = CORE;

    vm->ctx = ctx;

    return vm;
}

void gtk_ml_del_vm(GtkMl_Vm *vm) {
    free(vm);
}

void gtk_ml_vm_push(GtkMl_Vm *vm, GtkMl_TaggedValue value) {
    if (vm->stack_len == vm->stack_cap) {
        vm->stack_cap *= 2;
        vm->stack = realloc(vm->stack, sizeof(GtkMl_TaggedValue) * vm->stack_cap);
    }
    vm->stack[vm->stack_len++] = value;
}

GtkMl_TaggedValue gtk_ml_vm_pop(GtkMl_Vm *vm) {
    return vm->stack[--vm->stack_len];
}

GTKML_PRIVATE gboolean gtk_ml_vm_step(GtkMl_Vm *vm, GtkMl_SObj *err, size_t pc, GtkMl_Instruction instr) {
#ifdef GTKML_ENABLE_ASM
    gtk_ml_breakpoint(vm->ctx);
#endif /* GTKML_ENABLE_ASM */

    if (!instr.cond || (instr.cond && (vm->flags & instr.cond))) {
        if (instr.cond) {
            vm->flags &= GTKML_F_TOPCALL;
        }
        if (instr.category == GTKML_I_GENERIC) {
            gboolean (*function)(GtkMl_Vm *, GtkMl_SObj *, GtkMl_Data) = OPCODES[instr.opcode];
            if (function) {
                return function(vm, err, instr.data);
            } else {
                *err = gtk_ml_error(vm->ctx, "opcode-error", GTKML_ERR_OPCODE_ERROR, 0, 0, 0, 1, gtk_ml_new_keyword(vm->ctx, NULL, 0, "pc", strlen("pc")), gtk_ml_new_int(vm->ctx, NULL, pc));
                return 0;
            }
        } else if (instr.category == GTKML_I_RESERVED) {
            *err = gtk_ml_error(vm->ctx, "category-error", GTKML_ERR_CATEGORY_ERROR, 0, 0, 0, 1, gtk_ml_new_keyword(vm->ctx, NULL, 0, "pc", strlen("pc")), gtk_ml_new_int(vm->ctx, NULL, pc));
            return 0;
        } else if (instr.category == GTKML_I_EXTERN) {
            *err = gtk_ml_error(vm->ctx, "category-error", GTKML_ERR_CATEGORY_ERROR, 0, 0, 0, 1, gtk_ml_new_keyword(vm->ctx, NULL, 0, "pc", strlen("pc")), gtk_ml_new_int(vm->ctx, NULL, pc));
            return 0;
        } else { // exports are ignored
            vm->pc += 8;
            return 1;
        }
    } else {
        vm->pc += 8;
        return 1;
    }
}

gboolean gtk_ml_vm_run(GtkMl_Vm *vm, GtkMl_SObj *err, gboolean brk) {
#ifdef GTKML_ENABLE_ASM
    if (brk && getenv("GTKML_ENABLE_DEBUG") && strcmp(getenv("GTKML_ENABLE_DEBUG"), "0") != 0) {
        gtk_ml_breakpoint_internal(vm->ctx, !vm->ctx->dbg_done);
        vm->ctx->dbg_done = 1;
    }
#else
    (void) brk;
#endif /* GTKML_ENABLE_ASM */

    vm->flags |= GTKML_F_TOPCALL;
    vm->flags &= ~GTKML_F_HALT;
    size_t gc_counter = 0;
    while (!(vm->flags & GTKML_F_HALT)) {
        if ((vm->pc >> 3) >= vm->program->n_text) {
            *err = gtk_ml_error(vm->ctx, "index-out-of-bounds", GTKML_ERR_INDEX_ERROR, 0, 0, 0, 1,
                gtk_ml_new_keyword(vm->ctx, NULL, 0, "pc", strlen("pc")), gtk_ml_new_int(vm->ctx, NULL, vm->pc));
            return 0;
        }
        if (!gtk_ml_vm_step(vm, err, vm->pc, vm->program->text[vm->pc >> 3])) {
            return 0;
        }
        if (gc_counter++ == GTKML_GC_STEP_THRESHOLD) {
            if (gtk_ml_collect(vm->ctx)) {
                gc_counter = 0;
            }
        }
    }
    return 1;
}
