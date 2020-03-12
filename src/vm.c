#include <stdlib.h>
#include <gtk/gtk.h>
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

GTKML_PRIVATE GtkMl_S *vm_std_application(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr);
GTKML_PRIVATE GtkMl_S *vm_std_new_window(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr);
GTKML_PRIVATE GtkMl_S *vm_std_error(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr);
GTKML_PRIVATE GtkMl_S *vm_std_compile_expr(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr);
GTKML_PRIVATE GtkMl_S *vm_std_emit_bytecode(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr);

GTKML_PRIVATE GtkMl_S *(*STD[])(GtkMl_Context *, GtkMl_S **, GtkMl_S *) = {
    [GTKML_STD_APPLICATION] = vm_std_application,
    [GTKML_STD_NEW_WINDOW] = vm_std_new_window,
    [GTKML_STD_ERROR] = vm_std_error,
    [GTKML_STD_COMPILE_EXPR] = vm_std_compile_expr,
    [GTKML_STD_EMIT_BYTECODE] = vm_std_emit_bytecode,
};

void gtk_ml_push(GtkMl_Context *ctx, GtkMl_S *value) {
    ctx->vm->stack[ctx->vm->reg[GTKML_R_SP].sp].s_expr = value;
    ctx->vm->reg[GTKML_R_SP].sp++;
}

GtkMl_S *gtk_ml_pop(GtkMl_Context *ctx) {
    --ctx->vm->reg[GTKML_R_SP].sp;
    return ctx->vm->stack[ctx->vm->reg[GTKML_R_SP].sp].s_expr;
}

GtkMl_S *gtk_ml_peek(GtkMl_Context *ctx) {
    return ctx->vm->stack[ctx->vm->reg[GTKML_R_SP].sp - 1].s_expr;
}

void gtk_ml_enter(GtkMl_Context *ctx) {
    ctx->bindings = gtk_ml_new_list(ctx, NULL, gtk_ml_new_map(ctx, NULL, NULL), ctx->bindings);
}

void gtk_ml_leave(GtkMl_Context *ctx) {
    ctx->bindings = gtk_ml_cdr(ctx->bindings);
}

void gtk_ml_define(GtkMl_Context *ctx, GtkMl_S *key, GtkMl_S *value) {
    GtkMl_S *new_context = gtk_ml_new_map(ctx, NULL, NULL);
    gtk_ml_hash_trie_insert(&new_context->value.s_map.map, &(*ctx->top_scope)->value.s_map.map, key, value);
    *ctx->top_scope = new_context;
}

void gtk_ml_bind(GtkMl_Context *ctx, GtkMl_S *key, GtkMl_S *value) {
    GtkMl_S *new_context = gtk_ml_new_map(ctx, NULL, NULL);
    gtk_ml_hash_trie_insert(&new_context->value.s_map.map, &gtk_ml_car(ctx->bindings)->value.s_map.map, key, value);
    gtk_ml_car(ctx->bindings) = new_context;
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
    return get_inner(ctx->bindings, key);
}

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
    if (gtk_ml_run_program(ctx, &err, program_expr, gtk_ml_new_list(ctx, NULL, app_expr, gtk_ml_new_nil(ctx, NULL)))) {
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

    GtkWidget *window = gtk_application_window_new(app_expr->value.s_userdata.userdata);
    char *title = gtk_ml_to_c_str(title_expr);
    gtk_window_set_title(GTK_WINDOW(window), title);
    free(title);
    gtk_window_set_default_size(GTK_WINDOW(window), width_expr->value.s_int.value, height_expr->value.s_int.value);
    gtk_widget_show_all(window);

    return gtk_ml_new_lightdata(ctx, &expr->span, window);
}

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

GtkMl_S *vm_std_compile_expr(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr) {
    (void) expr;
    GtkMl_S *arg = gtk_ml_pop(ctx);
    GtkMl_S *bb_var = gtk_ml_pop(ctx);
    GtkMl_BasicBlock *arg_basic_block = bb_var->value.s_var.expr->value.s_lightdata.userdata;
    GtkMl_Builder *arg_b = gtk_ml_pop(ctx)->value.s_lightdata.userdata;
    GtkMl_Context *arg_ctx = gtk_ml_pop(ctx)->value.s_lightdata.userdata;

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

gboolean gtk_ml_equal(GtkMl_S *lhs, GtkMl_S *rhs) {
    if (lhs == rhs) {
        return 1;
    }

    if (lhs->kind != rhs->kind) {
        return 0;
    }

    switch (lhs->kind) {
    case GTKML_S_NIL:
        return 1;
    case GTKML_S_TRUE:
        return 1;
    case GTKML_S_FALSE:
        return 1;
    case GTKML_S_INT:
        return lhs->value.s_int.value == rhs->value.s_int.value;
    case GTKML_S_FLOAT:
        return lhs->value.s_float.value == rhs->value.s_float.value;
    case GTKML_S_CHAR:
        return lhs->value.s_char.value == rhs->value.s_char.value;
    case GTKML_S_KEYWORD:
        return lhs->value.s_keyword.len == rhs->value.s_keyword.len && memcmp(lhs->value.s_keyword.ptr, rhs->value.s_keyword.ptr, lhs->value.s_keyword.len) == 0;
    case GTKML_S_SYMBOL:
        return lhs->value.s_symbol.len == rhs->value.s_symbol.len && memcmp(lhs->value.s_symbol.ptr, rhs->value.s_symbol.ptr, lhs->value.s_symbol.len) == 0;
    case GTKML_S_LIGHTDATA:
        return lhs->value.s_lightdata.userdata == rhs->value.s_lightdata.userdata;
    case GTKML_S_USERDATA:
        return lhs->value.s_userdata.userdata == rhs->value.s_userdata.userdata;
    case GTKML_S_LAMBDA:
        if (gtk_ml_equal(lhs->value.s_lambda.args, rhs->value.s_lambda.args)) {
            return gtk_ml_equal(lhs->value.s_lambda.body, rhs->value.s_lambda.body);
        }
        break;
    case GTKML_S_PROGRAM:
        if (gtk_ml_equal(lhs->value.s_program.args, rhs->value.s_program.args)) {
            return lhs->value.s_program.addr == rhs->value.s_program.addr;
        }
        break;
    case GTKML_S_ADDRESS:
        return lhs->value.s_address.addr == rhs->value.s_address.addr;
    case GTKML_S_MACRO:
        if (gtk_ml_equal(lhs->value.s_macro.args, rhs->value.s_macro.args)) {
            return gtk_ml_equal(lhs->value.s_macro.body, rhs->value.s_macro.body);
        }
        break;
    case GTKML_S_LIST:
        if (gtk_ml_equal(gtk_ml_car(lhs), gtk_ml_car(rhs))) {
            return gtk_ml_equal(gtk_ml_cdr(lhs), gtk_ml_cdr(rhs));
        }
        break;
    case GTKML_S_MAP:
        return gtk_ml_hash_trie_equal(&lhs->value.s_map.map, &rhs->value.s_map.map);
    case GTKML_S_SET:
        return gtk_ml_hash_set_equal(&lhs->value.s_set.set, &rhs->value.s_set.set);
    case GTKML_S_ARRAY:
        return gtk_ml_array_trie_equal(&lhs->value.s_array.array, &rhs->value.s_array.array);
    case GTKML_S_VAR:
        return gtk_ml_equal(lhs->value.s_var.expr, rhs->value.s_var.expr);
    case GTKML_S_VARARG:
        return gtk_ml_equal(lhs->value.s_vararg.expr, rhs->value.s_vararg.expr);
    case GTKML_S_QUOTE:
        return gtk_ml_equal(lhs->value.s_quote.expr, rhs->value.s_quote.expr);
    case GTKML_S_QUASIQUOTE:
        return gtk_ml_equal(lhs->value.s_quasiquote.expr, rhs->value.s_quasiquote.expr);
    case GTKML_S_UNQUOTE:
        return gtk_ml_equal(lhs->value.s_unquote.expr, rhs->value.s_unquote.expr);
    }

    return 0;
}

gboolean gtk_ml_hash(GtkMl_Hasher *hasher, GtkMl_Hash *hash, GtkMl_S *value) {
    hasher->start(hash);
    if (hasher->update(hash, value)) {
        hasher->finish(hash);
        return 1;
    }
    return 0;
}

void gtk_ml_setmetamap(GtkMl_S *value, GtkMl_S *mm) {
    if (value->kind == GTKML_S_MAP) {
        value->value.s_map.metamap = mm;
    }
}

GtkMl_S *gtk_ml_getmetamap(GtkMl_S *value) {
    if (value->kind == GTKML_S_MAP) {
        return value->value.s_map.metamap;
    } else {
        return NULL;
    }
}

GtkMl_S *gtk_ml_error(GtkMl_Context *ctx, const char *err, const char *description, gboolean has_loc, int64_t line, int64_t col, size_t n, ...) {
    char *desc = malloc(strlen(description) + 1);
    strcpy(desc, description);

    GtkMl_S *s_err = gtk_ml_new_symbol(ctx, NULL, 0, err, strlen(err));
    GtkMl_S *s_desc = gtk_ml_new_string(ctx, NULL, desc, strlen(desc));
    GtkMl_S *s_has_loc = has_loc? gtk_ml_new_true(ctx, NULL) : gtk_ml_new_false(ctx, NULL);
    GtkMl_S *s_line = has_loc? gtk_ml_new_int(ctx, NULL, line) : NULL;
    GtkMl_S *s_col = has_loc? gtk_ml_new_int(ctx, NULL, col) : NULL;

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

    GtkMl_S *s_stacktrace = NULL;
    if (ctx->vm->program.exec) {
        s_stacktrace = gtk_ml_new_array(ctx, NULL);
        for (size_t i = 0; i < ctx->vm->call_stack_ptr; i += 2) {
            GtkMl_S *program = gtk_ml_new_nil(ctx, NULL);
            size_t call_at = ctx->vm->call_stack[i + 1];
            GtkMl_S *export = NULL;
            for (size_t ptr = 0; ptr <= call_at;) {
                GtkMl_Instruction instr = ctx->vm->program.exec[ptr >> 3];
                if (instr.gen.category == GTKML_EI_EXPORT) {
                    if (ctx->vm->program.statics[ctx->vm->program.exec[(ptr >> 3) + 1].imm64]->kind == GTKML_S_PROGRAM
                            || ctx->vm->program.statics[ctx->vm->program.exec[(ptr >> 3) + 1].imm64]->kind == GTKML_S_ADDRESS) {
                        export = ctx->vm->program.statics[ctx->vm->program.exec[(ptr >> 3) + 1].imm64];
                    }
                }
                if (instr.gen.category & GTKML_I_EXTENDED) {
                    ptr += 16;
                } else {
                    ptr += 8;
                }
            }

            if (export) {
                program = gtk_ml_new_array(ctx, NULL);

                if (export->kind == GTKML_S_PROGRAM) {
                    GtkMl_S *new = gtk_ml_new_array(ctx, NULL);
                    gtk_ml_array_trie_push(&new->value.s_array.array, &program->value.s_array.array, export->value.s_program.linkage_name);
                    program = new;

                    new = gtk_ml_new_array(ctx, NULL);
                    gtk_ml_array_trie_push(&new->value.s_array.array, &program->value.s_array.array, gtk_ml_new_int(ctx, NULL, export->value.s_program.addr));
                    program = new;
                } else if (export->kind == GTKML_S_ADDRESS) {
                    GtkMl_S *new = gtk_ml_new_array(ctx, NULL);
                    gtk_ml_array_trie_push(&new->value.s_array.array, &program->value.s_array.array, export->value.s_address.linkage_name);
                    program = new;

                    new = gtk_ml_new_array(ctx, NULL);
                    gtk_ml_array_trie_push(&new->value.s_array.array, &program->value.s_array.array, gtk_ml_new_int(ctx, NULL, export->value.s_address.addr));
                    program = new;
                }

                GtkMl_S *new = gtk_ml_new_array(ctx, NULL);
                gtk_ml_array_trie_push(&new->value.s_array.array, &program->value.s_array.array, gtk_ml_new_int(ctx, NULL, call_at));
                program = new;
            }
            GtkMl_S *new = gtk_ml_new_array(ctx, NULL);
            gtk_ml_array_trie_push(&new->value.s_array.array, &s_stacktrace->value.s_array.array, program);
            s_stacktrace = new;
        }

        GtkMl_S *program = gtk_ml_new_nil(ctx, NULL);
        size_t call_at = ctx->vm->reg[GTKML_R_PC].pc;
        if (call_at < ctx->vm->program.n_exec) {
            GtkMl_S *export = NULL;
            for (size_t ptr = 0; ptr <= call_at;) {
                GtkMl_Instruction instr = ctx->vm->program.exec[ptr >> 3];
                if (instr.gen.category == GTKML_EI_EXPORT) {
                    if (ctx->vm->program.statics[ctx->vm->program.exec[(ptr >> 3) + 1].imm64]->kind == GTKML_S_PROGRAM
                            || ctx->vm->program.statics[ctx->vm->program.exec[(ptr >> 3) + 1].imm64]->kind == GTKML_S_ADDRESS) {
                        export = ctx->vm->program.statics[ctx->vm->program.exec[(ptr >> 3) + 1].imm64];
                    }
                }
                if (instr.gen.category & GTKML_I_EXTENDED) {
                    ptr += 16;
                } else {
                    ptr += 8;
                }
            }

            if (export) {
                program = gtk_ml_new_array(ctx, NULL);

                if (export->kind == GTKML_S_PROGRAM) {
                    GtkMl_S *new = gtk_ml_new_array(ctx, NULL);
                    gtk_ml_array_trie_push(&new->value.s_array.array, &program->value.s_array.array, export->value.s_program.linkage_name);
                    program = new;

                    new = gtk_ml_new_array(ctx, NULL);
                    gtk_ml_array_trie_push(&new->value.s_array.array, &program->value.s_array.array, gtk_ml_new_int(ctx, NULL, export->value.s_program.addr));
                    program = new;
                } else if (export->kind == GTKML_S_ADDRESS) {
                    GtkMl_S *new = gtk_ml_new_array(ctx, NULL);
                    gtk_ml_array_trie_push(&new->value.s_array.array, &program->value.s_array.array, export->value.s_address.linkage_name);
                    program = new;

                    new = gtk_ml_new_array(ctx, NULL);
                    gtk_ml_array_trie_push(&new->value.s_array.array, &program->value.s_array.array, gtk_ml_new_int(ctx, NULL, export->value.s_address.addr));
                    program = new;
                }

                GtkMl_S *new = gtk_ml_new_array(ctx, NULL);
                gtk_ml_array_trie_push(&new->value.s_array.array, &program->value.s_array.array, gtk_ml_new_int(ctx, NULL, call_at));
                program = new;
            }
            GtkMl_S *new = gtk_ml_new_array(ctx, NULL);
            gtk_ml_array_trie_push(&new->value.s_array.array, &s_stacktrace->value.s_array.array, program);
            s_stacktrace = new;
        }
    }

    GtkMl_S *error = gtk_ml_new_map(ctx, NULL, NULL);

    GtkMl_S *new = gtk_ml_new_map(ctx, NULL, NULL);
    gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_new_keyword(ctx, NULL, 0, "err", 3), s_err);
    error = new;

    new = gtk_ml_new_map(ctx, NULL, NULL);
    gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_new_keyword(ctx, NULL, 0, "desc", 4), s_desc);
    error = new;

    new = gtk_ml_new_map(ctx, NULL, NULL);
    gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_new_keyword(ctx, NULL, 0, "loc", 3), s_loc);
    error = new;

    if (s_stacktrace) {
        new = gtk_ml_new_map(ctx, NULL, NULL);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_new_keyword(ctx, NULL, 0, "stacktrace", 10), s_stacktrace);
        error = new;
    }

    va_list args;
    va_start(args, n);

    for (size_t i = 0; i < n; i++) {
        GtkMl_S *key = va_arg(args, GtkMl_S *);
        GtkMl_S *value = va_arg(args, GtkMl_S *);

        new = gtk_ml_new_map(ctx, NULL, NULL);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, key, value);
        error = new;
    }

    va_end(args);

    return error;
}

void gtk_ml_delete_value_reference(GtkMl_Context *ctx, GtkMl_S *value) {
    (void) ctx;
    (void) value;
}

void gtk_ml_delete_void_reference(GtkMl_Context *ctx, void *reference) {
    (void) ctx;
    (void) reference;
}

void gtk_ml_delete_value(GtkMl_Context *ctx, void *s) {
    gtk_ml_delete(ctx, s);
}

void gtk_ml_object_unref(GtkMl_Context *ctx, void *obj) {
    (void) ctx;
    g_object_unref(obj);
}

GtkMl_Vm *gtk_ml_new_vm(GtkMl_Context *ctx) {
    GtkMl_Vm *vm = malloc(sizeof(GtkMl_Vm));

    vm->stack = malloc(sizeof(GtkMl_Register) * GTKML_VM_STACK);
    vm->stack_len = GTKML_VM_STACK;

    vm->call_stack = malloc(sizeof(GtkMl_Register) * GTKML_VM_CALL_STACK);
    vm->call_stack_ptr = 0;
    vm->call_stack_cap = GTKML_VM_CALL_STACK;

    vm->program.start = NULL;
    vm->program.exec = NULL;
    vm->program.n_exec = 0;
    vm->program.statics = NULL;
    vm->program.n_static = 0;

    vm->reg[GTKML_R_ZERO].value = 0;
    vm->reg[GTKML_R_FLAGS].flags = GTKML_F_NONE;
    vm->reg[GTKML_R_SP].sp = 0;
    vm->reg[GTKML_R_BP].sp = 0;

    vm->std = STD;

    vm->ctx = ctx;

    return vm;
}

void gtk_ml_del_vm(GtkMl_Vm *vm) {
    free(vm->stack);
    gtk_ml_del_program(&vm->program);
    free(vm);
}

gboolean gtk_ml_vm_step(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction *instr) {
    if (!instr->gen.cond || (instr->gen.cond && (vm->reg[GTKML_R_FLAGS].flags & instr->gen.cond))) {
        if (instr->gen.cond) {
            vm->reg[GTKML_R_FLAGS].flags &= GTKML_F_TOPCALL;
        }
        gboolean (*category)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction *) = CATEGORY[instr->gen.category];
        if (category) {
            return category(vm, err, instr);
        } else {
            *err = gtk_ml_error(vm->ctx, "category-error", GTKML_ERR_CATEGORY_ERROR, 0, 0, 0, 0);
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

gboolean gtk_ml_vm_run(GtkMl_Vm *vm, GtkMl_S **err) {
    vm->reg[GTKML_R_FLAGS].flags |= GTKML_F_TOPCALL;
    vm->reg[GTKML_R_FLAGS].flags &= ~GTKML_F_HALT;
    size_t gc_counter = 0;
    while (!(vm->reg[GTKML_R_FLAGS].flags & GTKML_F_HALT)) {
        if (!gtk_ml_vm_step(vm, err, vm->program.exec + (vm->reg[GTKML_R_PC].pc >> 3))) {
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
