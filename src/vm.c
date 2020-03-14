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

GTKML_PRIVATE gboolean (*CATEGORY[])(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction *) = {
    [0] = (gboolean (*)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction *)) NULL,
    [GTKML_I_ARITH] = gtk_ml_ia,
    [GTKML_I_IMM] = gtk_ml_ii,
    [GTKML_I_BR] = gtk_ml_ibr,
    [4] = (gboolean (*)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction *)) NULL,
    [5] = (gboolean (*)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction *)) NULL,
    [6] = (gboolean (*)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction *)) NULL,
    [7] = (gboolean (*)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction *)) NULL,
    [8] = (gboolean (*)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction *)) NULL,
    [9] = (gboolean (*)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction *)) NULL,
    [GTKML_EI_IMM] = gtk_ml_eii,
    [GTKML_EI_BR] = gtk_ml_eibr,
    [GTKML_EI_EXPORT] = gtk_ml_enop,
    [13] = (gboolean (*)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction *)) NULL,
    [14] = (gboolean (*)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction *)) NULL,
    [15] = (gboolean (*)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction *)) NULL,
};

#ifdef GTKML_ENABLE_GTK
GTKML_PRIVATE GtkMl_S *vm_std_application(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr);
GTKML_PRIVATE GtkMl_S *vm_std_new_window(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr);
#endif /* GTKML_ENABLE_GTK */
GTKML_PRIVATE GtkMl_S *vm_std_error(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr);
GTKML_PRIVATE GtkMl_S *vm_std_dbg(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr);
GTKML_PRIVATE GtkMl_S *vm_std_string_to_symbol(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr);
GTKML_PRIVATE GtkMl_S *vm_std_compile_expr(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr);
GTKML_PRIVATE GtkMl_S *vm_std_emit_bytecode(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr);
GTKML_PRIVATE GtkMl_S *vm_std_export_symbol(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr);
GTKML_PRIVATE GtkMl_S *vm_std_append_basic_block(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr);
GTKML_PRIVATE GtkMl_S *vm_std_global_counter(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr);
GTKML_PRIVATE GtkMl_S *vm_std_basic_block_name(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr);
#ifdef GTKML_ENABLE_POSIX
GTKML_PRIVATE GtkMl_S *vm_std_dbg_run(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr);
GTKML_PRIVATE GtkMl_S *vm_std_dbg_cont(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr);
GTKML_PRIVATE GtkMl_S *vm_std_dbg_step(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr);
GTKML_PRIVATE GtkMl_S *vm_std_dbg_disasm(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr);
GTKML_PRIVATE GtkMl_S *vm_std_dbg_stack(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr);
GTKML_PRIVATE GtkMl_S *vm_std_dbg_backtrace(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr);
#endif /* GTKML_ENABLE_POSIX */

GTKML_PRIVATE GtkMl_S *(*STD[])(GtkMl_Context *, GtkMl_S **, GtkMl_S *) = {
#ifdef GTKML_ENABLE_GTK
    [GTKML_STD_APPLICATION] = vm_std_application,
    [GTKML_STD_NEW_WINDOW] = vm_std_new_window,
#endif /* GTKML_ENABLE_GTK */
    [GTKML_STD_ERROR] = vm_std_error,
    [GTKML_STD_DBG] = vm_std_dbg,
    [GTKML_STD_STRING_TO_SYMBOL] = vm_std_string_to_symbol,
    [GTKML_STD_COMPILE_EXPR] = vm_std_compile_expr,
    [GTKML_STD_EMIT_BYTECODE] = vm_std_emit_bytecode,
    [GTKML_STD_EXPORT_SYMBOL] = vm_std_export_symbol,
    [GTKML_STD_APPEND_BASIC_BLOCK] = vm_std_append_basic_block,
    [GTKML_STD_GLOBAL_COUNTER] = vm_std_global_counter,
    [GTKML_STD_BASIC_BLOCK_NAME] = vm_std_basic_block_name,
#ifdef GTKML_ENABLE_POSIX
    [GTKML_STD_DBG_RUN] = vm_std_dbg_run,
    [GTKML_STD_DBG_CONT] = vm_std_dbg_cont,
    [GTKML_STD_DBG_STEP] = vm_std_dbg_step,
    [GTKML_STD_DBG_DISASM] = vm_std_dbg_disasm,
    [GTKML_STD_DBG_STACK] = vm_std_dbg_stack,
    [GTKML_STD_DBG_BACKTRACE] = vm_std_dbg_backtrace,
#endif /* GTKML_ENABLE_POSIX */
};

void gtk_ml_enter(GtkMl_Context *ctx) {
    ctx->bindings->value.s_var.expr = gtk_ml_new_list(ctx, NULL, gtk_ml_new_map(ctx, NULL, NULL), ctx->bindings->value.s_var.expr);
}

void gtk_ml_leave(GtkMl_Context *ctx) {
    ctx->bindings->value.s_var.expr = gtk_ml_cdr(ctx->bindings->value.s_var.expr);
}

void gtk_ml_define(GtkMl_Context *ctx, GtkMl_S *key, GtkMl_S *value) {
    GtkMl_S *new_context = gtk_ml_new_map(ctx, NULL, NULL);
    gtk_ml_hash_trie_insert(&new_context->value.s_map.map, &(*ctx->top_scope)->value.s_map.map, key, value);
    *ctx->top_scope = new_context;
}

void gtk_ml_bind(GtkMl_Context *ctx, GtkMl_S *key, GtkMl_S *value) {
    GtkMl_S *new_context = gtk_ml_new_map(ctx, NULL, NULL);
    gtk_ml_hash_trie_insert(&new_context->value.s_map.map, &gtk_ml_car(ctx->bindings->value.s_var.expr)->value.s_map.map, key, value);
    gtk_ml_car(ctx->bindings->value.s_var.expr) = new_context;
}

GTKML_PRIVATE GtkMl_S *map_find(GtkMl_S *map, GtkMl_S *key) {
    return gtk_ml_hash_trie_get(&map->value.s_map.map, key);
}

GTKML_PRIVATE GtkMl_S *get_inner(GtkMl_S *bindings, GtkMl_S *key) {
    if (bindings->kind == GTKML_S_NIL) {
        return NULL;
    }

    GtkMl_S *value = map_find(gtk_ml_car(bindings), key);
    if (value) {
        return value;
    }

    return get_inner(gtk_ml_cdr(bindings), key);
}

GtkMl_S *gtk_ml_get(GtkMl_Context *ctx, GtkMl_S *key) {
    return get_inner(ctx->bindings->value.s_var.expr, key);
}

#ifdef GTKML_ENABLE_GTK
GTKML_PRIVATE void activate_program(GtkApplication* app, gpointer userdata) {
    (void) app;

    GtkMl_S *args = userdata;
    GtkMl_S *ctx_expr = gtk_ml_car(args);
    GtkMl_S *app_expr = gtk_ml_cdar(args);
    GtkMl_S *program_expr = gtk_ml_cddar(args);

    GtkMl_Context *ctx = ctx_expr->value.s_lightdata.userdata;

    GtkMl_S *err;
    uint64_t pc = ctx->vm->reg[GTKML_R_PC].pc;
    uint64_t flags = ctx->vm->reg[GTKML_R_FLAGS].flags & GTKML_F_TOPCALL;
    if (gtk_ml_run_program_internal(ctx, &err, program_expr, gtk_ml_new_list(ctx, NULL, app_expr, gtk_ml_new_nil(ctx, NULL)), 0)) {
        GtkMl_S *result = gtk_ml_pop(ctx);
        app_expr->value.s_userdata.keep = gtk_ml_new_list(ctx, NULL, result, app_expr->value.s_userdata.keep);
    } else {
        gtk_ml_dumpf(ctx, stderr, NULL, err);
    }
    ctx->vm->reg[GTKML_R_FLAGS].flags &= ~GTKML_F_TOPCALL;
    ctx->vm->reg[GTKML_R_FLAGS].flags |= flags;
    ctx->vm->reg[GTKML_R_PC].pc = pc;
}

GtkMl_S *vm_std_application(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr) {
    (void) err;

    GtkMl_S *map_expr = gtk_ml_pop(ctx);
    GtkMl_S *flags_expr = gtk_ml_pop(ctx);
    GtkMl_S *id_expr = gtk_ml_pop(ctx);
    GtkMl_S *application = gtk_ml_pop(ctx);
    (void) application;

    char *id = gtk_ml_to_c_str(id_expr);
    GtkApplication *app = gtk_application_new(id, flags_expr->value.s_int.value);
    free(id);
    GtkMl_S *app_expr = gtk_ml_new_userdata(ctx, &expr->span, app, gtk_ml_object_unref);

    GtkMl_S *activate = map_find(map_expr, gtk_ml_new_keyword(ctx, NULL, 0, "activate", strlen("activate")));
    if (activate) {
        GtkMl_S *ctx_expr = gtk_ml_new_lightdata(ctx, NULL, ctx);
        GtkMl_S *userdata = gtk_ml_new_list(ctx, NULL, ctx_expr, gtk_ml_new_list(ctx, NULL, app_expr, gtk_ml_new_list(ctx, NULL, activate, gtk_ml_new_nil(ctx, NULL))));
        app_expr->value.s_userdata.keep = gtk_ml_new_list(ctx, &app_expr->span, userdata, app_expr->value.s_userdata.keep);
        g_signal_connect(app, "activate", G_CALLBACK(activate_program), userdata);
    }

    return app_expr;
}

GtkMl_S *vm_std_new_window(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr) {
    (void) err;

    GtkMl_S *height_expr = gtk_ml_pop(ctx);
    GtkMl_S *width_expr = gtk_ml_pop(ctx);
    GtkMl_S *title_expr = gtk_ml_pop(ctx);
    GtkMl_S *app_expr = gtk_ml_pop(ctx);
    GtkMl_S *new_window = gtk_ml_pop(ctx);
    (void) new_window;

    if (app_expr->kind != GTKML_S_USERDATA) {
        *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 1, gtk_ml_new_keyword(ctx, NULL, 0, "got", strlen("got")), app_expr);
        return NULL;
    }
    if (!app_expr->value.s_userdata.userdata) {
        *err = gtk_ml_error(ctx, "null-error", GTKML_ERR_NULL_ERROR, 0, 0, 0, 0);
        return NULL;
    }
    GtkWidget *window = gtk_application_window_new(app_expr->value.s_userdata.userdata);
    char *title = gtk_ml_to_c_str(title_expr);
    gtk_window_set_title(GTK_WINDOW(window), title);
    free(title);
    gtk_window_set_default_size(GTK_WINDOW(window), width_expr->value.s_int.value, height_expr->value.s_int.value);
    gtk_widget_show_all(window);

    return gtk_ml_new_lightdata(ctx, &expr->span, window);
}
#endif /* GTKML_ENABLE_GTK */

GtkMl_S *vm_std_error(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr) {
    GtkMl_S *error_expr = gtk_ml_pop(ctx);
    GtkMl_S *error = gtk_ml_pop(ctx);
    (void) error;

    gboolean has_loc = expr->span.ptr != NULL;
    GtkMl_S *s_line = has_loc? gtk_ml_new_int(ctx, NULL, expr->span.line) : NULL;
    GtkMl_S *s_col = has_loc? gtk_ml_new_int(ctx, NULL, expr->span.col) : NULL;

    GtkMl_S *s_has_loc = has_loc? gtk_ml_new_true(ctx, NULL) : gtk_ml_new_false(ctx, NULL);
    GtkMl_S *s_loc = gtk_ml_new_array(ctx, NULL);
    GtkMl_S *new_loc = gtk_ml_new_array(ctx, NULL);
    gtk_ml_array_trie_push(&new_loc->value.s_array.array, &s_loc->value.s_array.array, s_has_loc);
    s_loc = new_loc;
    if (has_loc) {
        new_loc = gtk_ml_new_array(ctx, NULL);

        gtk_ml_array_trie_push(&new_loc->value.s_array.array, &s_loc->value.s_array.array, s_line);
        s_loc = new_loc;

        new_loc = gtk_ml_new_array(ctx, NULL);
        gtk_ml_array_trie_push(&new_loc->value.s_array.array, &s_loc->value.s_array.array, s_col);
        s_loc = new_loc;
    }

    GtkMl_S *new = gtk_ml_new_map(ctx, NULL, NULL);
    gtk_ml_hash_trie_insert(&new->value.s_map.map, &error_expr->value.s_map.map, gtk_ml_new_keyword(ctx, NULL, 0, "loc", 3), s_loc);
    error_expr = new;

    *err = error_expr;

    return NULL;
}

GtkMl_S *vm_std_dbg(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr) {
    (void) expr;

    GtkMl_S *value = gtk_ml_pop(ctx);
    if (!gtk_ml_dumpf(ctx, stderr, err, value)) {
        return NULL;
    }
    fprintf(stderr, "\n");

    GtkMl_S *dbg = gtk_ml_pop(ctx);
    (void) dbg;

    return value;
}

GtkMl_S *vm_std_string_to_symbol(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr) {
    (void) expr;
    (void) err;

    GtkMl_S *str = gtk_ml_pop(ctx);
    if (str->kind != GTKML_S_ARRAY) {
        *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
                gtk_ml_new_keyword(ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(ctx, NULL, 0, "string", strlen("string")),
                gtk_ml_new_keyword(ctx, NULL, 0, "got", strlen("got")), str);
        return NULL;
    }
    char *c_str = gtk_ml_to_c_str(str);

    GtkMl_S *string_to_symbol = gtk_ml_pop(ctx);
    (void) string_to_symbol;

    return gtk_ml_new_symbol(ctx, &expr->span, 1, c_str, strlen(c_str));
}

GtkMl_S *vm_std_compile_expr(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr) {
    (void) expr;
    GtkMl_S *arg = gtk_ml_pop(ctx);
    GtkMl_S *bb_var = gtk_ml_pop(ctx);
    if (bb_var->kind != GTKML_S_VAR) {
        *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 1, gtk_ml_new_keyword(ctx, NULL, 0, "got", strlen("got")), bb_var);
        return NULL;
    }
    if (bb_var->value.s_var.expr->kind != GTKML_S_LIGHTDATA) {
        *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 1, gtk_ml_new_keyword(ctx, NULL, 0, "got", strlen("got")), bb_var);
        return NULL;
    }
    GtkMl_BasicBlock *arg_basic_block = bb_var->value.s_var.expr->value.s_lightdata.userdata;
    GtkMl_Builder *arg_b = gtk_ml_pop(ctx)->value.s_lightdata.userdata;
    GtkMl_Context *arg_ctx = gtk_ml_pop(ctx)->value.s_lightdata.userdata;
    GtkMl_S *compile_expr = gtk_ml_pop(ctx);
    (void) compile_expr;

    if (!gtk_ml_compile_expression(arg_ctx, arg_b, &arg_basic_block, err, &arg, 0, 0, 1, 1)) {
        return NULL;
    }

    if (arg_basic_block != bb_var->value.s_var.expr->value.s_lightdata.userdata) {
        bb_var->value.s_var.expr = gtk_ml_new_lightdata(ctx, NULL, arg_basic_block);
    }

    return gtk_ml_new_true(ctx, NULL);
}

GtkMl_S *vm_std_emit_bytecode(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr) {
    GtkMl_S *imm = NULL;
    if (expr->value.s_int.value == 7) {
        imm = gtk_ml_pop(ctx);
    }

    GtkMl_S *bc = gtk_ml_pop(ctx);
    unsigned int arg_cond = gtk_ml_pop(ctx)->value.s_int.value;
    GtkMl_BasicBlock *arg_basic_block = gtk_ml_pop(ctx)->value.s_lightdata.userdata;
    GtkMl_Builder *arg_b = gtk_ml_pop(ctx)->value.s_lightdata.userdata;
    GtkMl_Context *arg_ctx = gtk_ml_pop(ctx)->value.s_lightdata.userdata;
    const char *bc_ptr = bc->value.s_keyword.ptr;
    size_t bc_len = bc->value.s_keyword.len;
    GtkMl_S *emit_bytecode = gtk_ml_pop(ctx);
    (void) emit_bytecode;

    const char *bc_halt = "halt";
    const char *bc_push_imm = "push-imm";
    const char *bc_push_addr = "push-addr";
    const char *bc_pop = "pop";
    const char *bc_setf_imm = "setf-imm";
    const char *bc_popf = "popf";
    const char *bc_define = "define";
    const char *bc_car = "car";
    const char *bc_cdr = "cdr";
    const char *bc_bind = "bind";
    const char *bc_bind_args = "bind-args";
    const char *bc_list = "list";
    const char *bc_enter = "enter";
    const char *bc_leave = "leave";
    const char *bc_typeof = "typeof";
    const char *bc_get_imm = "get-imm";
    const char *bc_list_imm = "list-imm";
    const char *bc_map_imm = "map-imm";
    const char *bc_set_imm = "set-imm";
    const char *bc_array_imm = "array-imm";
    const char *bc_setmm_imm = "setmm-imm";
    const char *bc_getmm_imm = "getmm-imm";
    const char *bc_var_imm = "var-imm";
    const char *bc_getvar_imm = "getvar-imm";
    const char *bc_assignvar_imm = "assignvar-imm";
    const char *bc_length = "len";
    const char *bc_array_index = "array-index";
    const char *bc_array_push = "array-push";
    const char *bc_array_pop = "array-pop";
    const char *bc_array_concat = "array-concat";
    const char *bc_map_get = "map-get";
    const char *bc_map_insert = "map-insert";
    const char *bc_map_delete = "map-gtk_ml_delete";
    const char *bc_set_contains = "set-contains";
    const char *bc_set_insert = "set-insert";
    const char *bc_set_delete = "set-gtk_ml_delete";
    const char *bc_call_std = "call-std";
    const char *bc_call = "call";
    const char *bc_branch_absolute = "branch-absolute";
    const char *bc_ret = "ret";
    const char *bc_add = "add";
    const char *bc_sub = "sub";
    const char *bc_mul = "mul";
    const char *bc_div = "div";
    const char *bc_mod = "mod";
    const char *bc_bitand = "bitand";
    const char *bc_bitor = "bitor";
    const char *bc_bitxor = "bitxor";
    const char *bc_cmp = "cmp";

    gtk_ml_builder_set_cond(arg_b, arg_cond);
    if (strlen(bc_halt) == bc_len && strncmp(bc_ptr, bc_halt, bc_len) == 0) {
        return gtk_ml_build_halt(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_push_imm) == bc_len && strncmp(bc_ptr, bc_push_imm, bc_len) == 0) {
        if (!imm) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return 0;
        }
        return gtk_ml_build_push_extended_imm(arg_ctx, arg_b, arg_basic_block, err, gtk_ml_append_static(arg_b, imm))? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_push_addr) == bc_len && strncmp(bc_ptr, bc_push_addr, bc_len) == 0) {
        if (!imm) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return 0;
        }
        return gtk_ml_build_push_extended_addr(arg_ctx, arg_b, arg_basic_block, err, gtk_ml_append_static(arg_b, imm))? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_pop) == bc_len && strncmp(bc_ptr, bc_pop, bc_len) == 0) {
        return gtk_ml_build_pop(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_setf_imm) == bc_len && strncmp(bc_ptr, bc_setf_imm, bc_len) == 0) {
        if (!imm) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return 0;
        }
        return gtk_ml_build_setf_extended_imm(arg_ctx, arg_b, arg_basic_block, err, gtk_ml_append_static(arg_b, imm))? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_popf) == bc_len && strncmp(bc_ptr, bc_popf, bc_len) == 0) {
        return gtk_ml_build_popf(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_define) == bc_len && strncmp(bc_ptr, bc_define, bc_len) == 0) {
        return gtk_ml_build_define(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_car) == bc_len && strncmp(bc_ptr, bc_car, bc_len) == 0) {
        return gtk_ml_build_car(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_cdr) == bc_len && strncmp(bc_ptr, bc_cdr, bc_len) == 0) {
        return gtk_ml_build_cdr(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_bind) == bc_len && strncmp(bc_ptr, bc_bind, bc_len) == 0) {
        return gtk_ml_build_bind(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_bind_args) == bc_len && strncmp(bc_ptr, bc_bind_args, bc_len) == 0) {
        return gtk_ml_build_bind_args(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_list) == bc_len && strncmp(bc_ptr, bc_list, bc_len) == 0) {
        return gtk_ml_build_list(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_enter) == bc_len && strncmp(bc_ptr, bc_enter, bc_len) == 0) {
        return gtk_ml_build_enter(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_leave) == bc_len && strncmp(bc_ptr, bc_leave, bc_len) == 0) {
        return gtk_ml_build_leave(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_typeof) == bc_len && strncmp(bc_ptr, bc_typeof, bc_len) == 0) {
        return gtk_ml_build_typeof(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_get_imm) == bc_len && strncmp(bc_ptr, bc_get_imm, bc_len) == 0) {
        if (!imm) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return 0;
        }
        return gtk_ml_build_get_extended_imm(arg_ctx, arg_b, arg_basic_block, err, gtk_ml_append_static(arg_b, imm))? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_list_imm) == bc_len && strncmp(bc_ptr, bc_list_imm, bc_len) == 0) {
        if (!imm) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return 0;
        }
        return gtk_ml_build_list_extended_imm(arg_ctx, arg_b, arg_basic_block, err, gtk_ml_append_static(arg_b, imm))? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_map_imm) == bc_len && strncmp(bc_ptr, bc_map_imm, bc_len) == 0) {
        if (!imm) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return 0;
        }
        return gtk_ml_build_map_extended_imm(arg_ctx, arg_b, arg_basic_block, err, gtk_ml_append_static(arg_b, imm))? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_set_imm) == bc_len && strncmp(bc_ptr, bc_set_imm, bc_len) == 0) {
        if (!imm) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return 0;
        }
        return gtk_ml_build_set_extended_imm(arg_ctx, arg_b, arg_basic_block, err, gtk_ml_append_static(arg_b, imm))? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_array_imm) == bc_len && strncmp(bc_ptr, bc_array_imm, bc_len) == 0) {
        if (!imm) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return 0;
        }
        return gtk_ml_build_array_extended_imm(arg_ctx, arg_b, arg_basic_block, err, gtk_ml_append_static(arg_b, imm))? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_setmm_imm) == bc_len && strncmp(bc_ptr, bc_setmm_imm, bc_len) == 0) {
        return gtk_ml_build_setmm_imm(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_getmm_imm) == bc_len && strncmp(bc_ptr, bc_getmm_imm, bc_len) == 0) {
        return gtk_ml_build_getmm_imm(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_var_imm) == bc_len && strncmp(bc_ptr, bc_var_imm, bc_len) == 0) {
        return gtk_ml_build_var_imm(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_getvar_imm) == bc_len && strncmp(bc_ptr, bc_getvar_imm, bc_len) == 0) {
        return gtk_ml_build_getvar_imm(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_assignvar_imm) == bc_len && strncmp(bc_ptr, bc_assignvar_imm, bc_len) == 0) {
        return gtk_ml_build_assignvar_imm(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_length) == bc_len && strncmp(bc_ptr, bc_length, bc_len) == 0) {
        return gtk_ml_build_len(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_array_index) == bc_len && strncmp(bc_ptr, bc_array_index, bc_len) == 0) {
        return gtk_ml_build_array_index(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_array_push) == bc_len && strncmp(bc_ptr, bc_array_push, bc_len) == 0) {
        return gtk_ml_build_array_push(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_array_pop) == bc_len && strncmp(bc_ptr, bc_array_pop, bc_len) == 0) {
        return gtk_ml_build_array_pop(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_array_concat) == bc_len && strncmp(bc_ptr, bc_array_concat, bc_len) == 0) {
        return gtk_ml_build_array_concat(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_map_get) == bc_len && strncmp(bc_ptr, bc_map_get, bc_len) == 0) {
        return gtk_ml_build_map_get(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_map_insert) == bc_len && strncmp(bc_ptr, bc_map_insert, bc_len) == 0) {
        return gtk_ml_build_map_insert(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_map_delete) == bc_len && strncmp(bc_ptr, bc_map_delete, bc_len) == 0) {
        return gtk_ml_build_map_delete(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_set_contains) == bc_len && strncmp(bc_ptr, bc_set_contains, bc_len) == 0) {
        return gtk_ml_build_set_contains(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_set_insert) == bc_len && strncmp(bc_ptr, bc_set_insert, bc_len) == 0) {
        return gtk_ml_build_set_insert(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_set_delete) == bc_len && strncmp(bc_ptr, bc_set_delete, bc_len) == 0) {
        return gtk_ml_build_set_delete(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_call_std) == bc_len && strncmp(bc_ptr, bc_call_std, bc_len) == 0) {
        if (!imm) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return 0;
        }
        return gtk_ml_build_call_extended_std(arg_ctx, arg_b, arg_basic_block, err, gtk_ml_append_static(arg_b, imm))? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_call) == bc_len && strncmp(bc_ptr, bc_call, bc_len) == 0) {
        return gtk_ml_build_call(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_branch_absolute) == bc_len && strncmp(bc_ptr, bc_branch_absolute, bc_len) == 0) {
        if (!imm) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return 0;
        }
        return gtk_ml_build_branch_absolute_extended(arg_ctx, arg_b, arg_basic_block, err, gtk_ml_append_static(arg_b, imm))? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_ret) == bc_len && strncmp(bc_ptr, bc_ret, bc_len) == 0) {
        return gtk_ml_build_ret(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_add) == bc_len && strncmp(bc_ptr, bc_add, bc_len) == 0) {
        return gtk_ml_build_add(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_sub) == bc_len && strncmp(bc_ptr, bc_sub, bc_len) == 0) {
        return gtk_ml_build_sub(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_mul) == bc_len && strncmp(bc_ptr, bc_mul, bc_len) == 0) {
        return gtk_ml_build_mul(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_div) == bc_len && strncmp(bc_ptr, bc_div, bc_len) == 0) {
        return gtk_ml_build_div(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_mod) == bc_len && strncmp(bc_ptr, bc_mod, bc_len) == 0) {
        return gtk_ml_build_mod(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_bitand) == bc_len && strncmp(bc_ptr, bc_bitand, bc_len) == 0) {
        return gtk_ml_build_bitand(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_bitor) == bc_len && strncmp(bc_ptr, bc_bitor, bc_len) == 0) {
        return gtk_ml_build_bitor(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_bitxor) == bc_len && strncmp(bc_ptr, bc_bitxor, bc_len) == 0) {
        return gtk_ml_build_bitxor(arg_ctx, arg_b, arg_basic_block, err)? gtk_ml_new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_cmp) == bc_len && strncmp(bc_ptr, bc_cmp, bc_len) == 0) {
        if (!imm) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return 0;
        }
        return gtk_ml_build_cmp(arg_ctx, arg_b, arg_basic_block, err, gtk_ml_append_static(arg_b, imm))? gtk_ml_new_true(ctx, NULL) : NULL;
    } else {
        *err = gtk_ml_error(ctx, "bytecode-error", GTKML_ERR_BYTECODE_ERROR, bc->span.ptr != NULL, bc->span.line, bc->span.col, 0);
        return NULL;
    }
}

GtkMl_S *vm_std_export_symbol(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr) {
    (void) err;

    GtkMl_BasicBlock *arg_basic_block = gtk_ml_pop(ctx)->value.s_lightdata.userdata;
    GtkMl_Builder *arg_b = gtk_ml_pop(ctx)->value.s_lightdata.userdata;
    GtkMl_Context *arg_ctx = gtk_ml_pop(ctx)->value.s_lightdata.userdata;
    GtkMl_S *export_symbol = gtk_ml_pop(ctx);
    (void) export_symbol;

    if (arg_basic_block->len_exec == arg_basic_block->cap_exec) {
        arg_basic_block->cap_exec *= 2;
        arg_basic_block->exec = realloc(arg_basic_block->exec, sizeof(GtkMl_Instruction) * arg_basic_block->cap_exec);
    }

    arg_basic_block->exec[arg_basic_block->len_exec].instr = 0;
    arg_basic_block->exec[arg_basic_block->len_exec].imm.cond = 0;
    arg_basic_block->exec[arg_basic_block->len_exec].imm.category = GTKML_EI_EXPORT;
    arg_basic_block->exec[arg_basic_block->len_exec].imm.imm = GTKML_EI_EXPORT_FLAG_NONE;
    ++arg_basic_block->len_exec;

    if (arg_basic_block->len_exec == arg_basic_block->cap_exec) {
        arg_basic_block->cap_exec *= 2;
        arg_basic_block->exec = realloc(arg_basic_block->exec, sizeof(GtkMl_Instruction) * arg_basic_block->cap_exec);
    }

    GtkMl_S *address = gtk_ml_new_address(arg_ctx, &expr->span, arg_basic_block->name, 0);
    arg_basic_block->exec[arg_basic_block->len_exec].imm64 = gtk_ml_append_static(arg_b, address);
    ++arg_basic_block->len_exec;

    return address;
}

GtkMl_S *vm_std_append_basic_block(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr) {
    (void) expr;
    (void) err;

    GtkMl_S *arg_name = gtk_ml_pop(ctx);
    GtkMl_Builder *arg_b = gtk_ml_pop(ctx)->value.s_lightdata.userdata;
    char *name = gtk_ml_to_c_str(arg_name);
    GtkMl_S *append_basic_block = gtk_ml_pop(ctx);
    (void) append_basic_block;

    GtkMl_BasicBlock *bb = gtk_ml_append_basic_block(arg_b, name);

    return gtk_ml_new_lightdata(ctx, NULL, bb);
}

GtkMl_S *vm_std_global_counter(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr) {
    (void) expr;
    (void) err;

    GtkMl_Builder *arg_b = gtk_ml_pop(ctx)->value.s_lightdata.userdata;

    GtkMl_S *global_counter = gtk_ml_pop(ctx);
    (void) global_counter;

    return arg_b->counter;
}

GtkMl_S *vm_std_basic_block_name(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr) {
    (void) expr;
    (void) err;

    GtkMl_BasicBlock *arg_bb = gtk_ml_pop(ctx)->value.s_lightdata.userdata;

    GtkMl_S *basic_block_name = gtk_ml_pop(ctx);
    (void) basic_block_name;

    return gtk_ml_new_string(ctx, &expr->span, arg_bb->name, strlen(arg_bb->name));
}

#ifdef GTKML_ENABLE_POSIX
GtkMl_S *vm_std_dbg_run(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr) {
    if (!ctx->is_debugger) {
        *err = gtk_ml_error(ctx, "debugger-error", GTKML_ERR_DEBUGGER_ERROR, 0, 0, 0, 0);
        return 0;
    }

    gtk_ml_dbg_write_boolean(ctx, err, &ctx->dbg_ctx->enable_breakpoint, 0);
    if (*err) {
        return 0;
    }

    ptrace(PTRACE_CONT, ctx->dbg_process, NULL, NULL);

    int wait_status;
    waitpid(ctx->dbg_process, &wait_status, 0);

    return gtk_ml_new_nil(ctx, &expr->span);
}

GtkMl_S *vm_std_dbg_cont(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr) {
    if (!ctx->is_debugger) {
        *err = gtk_ml_error(ctx, "debugger-error", GTKML_ERR_DEBUGGER_ERROR, 0, 0, 0, 0);
        return 0;
    }

    gtk_ml_dbg_write_boolean(ctx, err, &ctx->dbg_ctx->enable_breakpoint, 0);
    if (*err) {
        return 0;
    }

    ptrace(PTRACE_CONT, ctx->dbg_process, NULL, NULL);

    int wait_status;
    waitpid(ctx->dbg_process, &wait_status, 0);

    return gtk_ml_new_nil(ctx, &expr->span);
}

GtkMl_S *vm_std_dbg_step(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr) {
    if (!ctx->is_debugger) {
        *err = gtk_ml_error(ctx, "debugger-error", GTKML_ERR_DEBUGGER_ERROR, 0, 0, 0, 0);
        return 0;
    }

    gtk_ml_dbg_write_boolean(ctx, err, &ctx->dbg_ctx->enable_breakpoint, 1);
    if (*err) {
        return 0;
    }

    ptrace(PTRACE_CONT, ctx->dbg_process, NULL, NULL);

    int wait_status;
    waitpid(ctx->dbg_process, &wait_status, 0);

    GtkMl_S *_err;
    if (!gtk_ml_dumpf_line_debug(ctx, stderr, &_err)) {
        if (!gtk_ml_dumpf(ctx, stderr, err, _err)) {
            return NULL;
        }
        fprintf(stderr, "\n");
    }

    return gtk_ml_new_nil(ctx, &expr->span);
}

GtkMl_S *vm_std_dbg_disasm(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr) {
    if (!ctx->is_debugger) {
        *err = gtk_ml_error(ctx, "debugger-error", GTKML_ERR_DEBUGGER_ERROR, 0, 0, 0, 0);
        return 0;
    }

    GtkMl_S *_err;
    if (!gtk_ml_dumpf_program_debug(ctx, stderr, &_err)) {
        if (!gtk_ml_dumpf(ctx, stderr, err, _err)) {
            return NULL;
        }
        fprintf(stderr, "\n");
    }

    return gtk_ml_new_nil(ctx, &expr->span);
}

GtkMl_S *vm_std_dbg_stack(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr) {
    if (!ctx->is_debugger) {
        *err = gtk_ml_error(ctx, "debugger-error", GTKML_ERR_DEBUGGER_ERROR, 0, 0, 0, 0);
        return 0;
    }

    GtkMl_S *_err;
    if (!gtk_ml_dumpf_stack_debug(ctx, stderr, &_err)) {
        if (!gtk_ml_dumpf(ctx, stderr, err, _err)) {
            return NULL;
        }
        fprintf(stderr, "\n");
    }

    return gtk_ml_new_nil(ctx, &expr->span);
}

GtkMl_S *vm_std_dbg_backtrace(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr) {
    if (!ctx->is_debugger) {
        *err = gtk_ml_error(ctx, "debugger-error", GTKML_ERR_DEBUGGER_ERROR, 0, 0, 0, 0);
        return 0;
    }

    GtkMl_S *_err;
    if (!gtk_ml_backtracef_debug(ctx, stderr, &_err)) {
        if (!gtk_ml_dumpf(ctx, stderr, err, _err)) {
            return NULL;
        }
        fprintf(stderr, "\n");
    }

    return gtk_ml_new_nil(ctx, &expr->span);
}
#endif /* GTKML_ENABLE_POSIX */

GtkMl_Vm *gtk_ml_new_vm(GtkMl_Context *ctx) {
    GtkMl_Vm *vm = malloc(sizeof(GtkMl_Vm));

    vm->call_stack = malloc(sizeof(GtkMl_Register) * GTKML_VM_CALL_STACK);
    vm->call_stack_ptr = 0;
    vm->call_stack_cap = GTKML_VM_CALL_STACK;

    vm->program = NULL;

    vm->reg[GTKML_R_ZERO].value = 0;
    vm->reg[GTKML_R_FLAGS].flags = GTKML_F_NONE;
    vm->reg[GTKML_R_BP].sp = 0;

    vm->std = STD;

    vm->ctx = ctx;

    return vm;
}

void gtk_ml_del_vm(GtkMl_Vm *vm) {
    free(vm);
}

GTKML_PRIVATE gboolean gtk_ml_vm_step(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction *instr) {
#ifdef GTKML_ENABLE_ASM
    gtk_ml_breakpoint(vm->ctx);
#endif /* GTKML_ENABLE_ASM */

    if (!instr->gen.cond || (instr->gen.cond && (vm->reg[GTKML_R_FLAGS].flags & instr->gen.cond))) {
        if (instr->gen.cond) {
            vm->reg[GTKML_R_FLAGS].flags &= GTKML_F_TOPCALL;
        }
        gboolean (*category)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction *) = CATEGORY[instr->gen.category];
        if (category) {
            return category(vm, err, instr);
        } else {
            *err = gtk_ml_error(vm->ctx, "category-error", GTKML_ERR_CATEGORY_ERROR, 0, 0, 0, 1, gtk_ml_new_keyword(vm->ctx, NULL, 0, "pc", strlen("pc")), gtk_ml_new_int(vm->ctx, NULL, pc));
            return 0;
        }
    } else {
        if (instr->gen.category & GTKML_I_EXTENDED) {
            vm->reg[GTKML_R_PC].pc += 16;
        } else {
            vm->reg[GTKML_R_PC].pc += 8;
        }
        return 1;
    }
}

gboolean gtk_ml_vm_run(GtkMl_Vm *vm, GtkMl_S **err, gboolean brk) {
#ifdef GTKML_ENABLE_ASM
    if (brk && getenv("GTKML_ENABLE_DEBUG") && strcmp(getenv("GTKML_ENABLE_DEBUG"), "0") != 0) {
        gtk_ml_breakpoint_internal(vm->ctx, !vm->ctx->dbg_done);
        vm->ctx->dbg_done = 1;
    }
#else
    (void) brk;
#endif /* GTKML_ENABLE_ASM */

    vm->reg[GTKML_R_FLAGS].flags |= GTKML_F_TOPCALL;
    vm->reg[GTKML_R_FLAGS].flags &= ~GTKML_F_HALT;
    size_t gc_counter = 0;
    while (!(vm->reg[GTKML_R_FLAGS].flags & GTKML_F_HALT)) {
        if ((vm->reg[GTKML_R_PC].pc >> 3) >= vm->program->n_exec) {
            *err = gtk_ml_error(vm->ctx, "index-out-of-bounds", GTKML_ERR_INDEX_ERROR, 0, 0, 0, 1, gtk_ml_new_keyword(vm->ctx, NULL, 0, "pc", strlen("pc")), gtk_ml_new_int(vm->ctx, NULL, vm->reg[GTKML_R_PC].pc));
            return 0;
        }
        if (!gtk_ml_vm_step(vm, err, vm->reg[GTKML_R_PC].pc, vm->program->exec + (vm->reg[GTKML_R_PC].pc >> 3))) {
            return 0;
        }
        if (gc_counter++ == GTKML_GC_STEP_THRESHOLD) {
            if (gtk_ml_collect(vm->ctx)) {
                gc_counter = 0;
            }
        }
        vm->reg[GTKML_R_ZERO].value = 0;
    }
    return 1;
}
