#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>
#ifdef GTKML_ENABLE_POSIX
#include <sys/ptrace.h>
#endif /* GTKML_ENABLE_POSIX */
#ifdef GTKML_ENABLE_GTK
#include <gtk/gtk.h>
#endif /* GTKML_ENABLE_GTK */
#include <math.h>
#define GTKML_INCLUDE_INTERNAL
#include "gtk-ml.h"
#include "gtk-ml-internal.h"

GTKML_PRIVATE const char *S_I_ARITH[] = {
    [GTKML_IA_NOP] = GTKML_SIA_NOP,
    [GTKML_IA_HALT] = GTKML_SIA_HALT,
    [GTKML_IA_ADD] = GTKML_SIA_ADD,
    [GTKML_IA_SUBTRACT] = GTKML_SIA_SUBTRACT,
    [GTKML_IA_SIGNED_MULTIPLY] = GTKML_SIA_SIGNED_MULTIPLY,
    [GTKML_IA_UNSIGNED_MULTIPLY] = GTKML_SIA_UNSIGNED_MULTIPLY,
    [GTKML_IA_SIGNED_DIVIDE] = GTKML_SIA_SIGNED_DIVIDE,
    [GTKML_IA_UNSIGNED_DIVIDE] = GTKML_SIA_UNSIGNED_DIVIDE,
    [GTKML_IA_SIGNED_MODULO] = GTKML_SIA_SIGNED_MODULO,
    [GTKML_IA_UNSIGNED_MODULO] = GTKML_SIA_UNSIGNED_MODULO,
    [GTKML_IA_BIT_AND] = GTKML_SIA_BIT_AND,
    [GTKML_IA_BIT_OR] = GTKML_SIA_BIT_OR,
    [GTKML_IA_BIT_XOR] = GTKML_SIA_BIT_XOR,
    [GTKML_IA_BIT_NAND] = GTKML_SIA_BIT_NAND,
    [GTKML_IA_BIT_NOR] = GTKML_SIA_BIT_NOR,
    [GTKML_IA_BIT_XNOR] = GTKML_SIA_BIT_XNOR,
    [GTKML_IA_CAR] = GTKML_SIA_CAR,
    [GTKML_IA_CDR] = GTKML_SIA_CDR,
    [GTKML_IA_BIND] = GTKML_SIA_BIND,
    [GTKML_IA_DEFINE] = GTKML_SIA_DEFINE,
    [GTKML_IA_BIND_ARGS] = GTKML_SIA_BIND_ARGS,
    [GTKML_IA_LIST] = GTKML_SIA_LIST,
    [GTKML_IA_ENTER] = GTKML_SIA_ENTER,
    [GTKML_IA_LEAVE] = GTKML_SIA_LEAVE,
    [GTKML_IA_UNWRAP] = GTKML_SIA_UNWRAP,
    [GTKML_IA_TYPEOF] = GTKML_SIA_TYPEOF,
    [255] = NULL,
};

GTKML_PRIVATE const char *S_I_IMM[] = {
    [GTKML_II_PUSH_IMM] = GTKML_SII_PUSH_IMM,
    [GTKML_II_POP] = GTKML_SII_POP,
    [GTKML_II_SETF_IMM] = GTKML_SII_SETF_IMM,
    [GTKML_II_POPF] = GTKML_SII_POPF,
    [GTKML_II_GET_IMM] = GTKML_SII_GET_IMM,
    [GTKML_II_LIST_IMM] = GTKML_SII_LIST_IMM,
    [GTKML_II_MAP_IMM] = GTKML_SII_MAP_IMM,
    [GTKML_II_SET_IMM] = GTKML_SII_SET_IMM,
    [GTKML_II_ARRAY_IMM] = GTKML_SII_ARRAY_IMM,
    [GTKML_II_SETMM_IMM] = GTKML_SII_SETMM_IMM,
    [GTKML_II_GETMM_IMM] = GTKML_SII_GETMM_IMM,
    [GTKML_II_VAR_IMM] = GTKML_SII_VAR_IMM,
    [GTKML_II_GETVAR_IMM] = GTKML_SII_GETVAR_IMM,
    [GTKML_II_ASSIGNVAR_IMM] = GTKML_SII_ASSIGNVAR_IMM,
    [GTKML_II_LEN] = GTKML_SII_LEN,
    [GTKML_II_ARRAY_INDEX] = GTKML_SII_ARRAY_INDEX,
    [GTKML_II_ARRAY_PUSH] = GTKML_SII_ARRAY_PUSH,
    [GTKML_II_ARRAY_CONCAT] = GTKML_SII_ARRAY_CONCAT,
    [GTKML_II_ARRAY_POP] = GTKML_SII_ARRAY_POP,
    [GTKML_II_MAP_GET] = GTKML_SII_MAP_GET,
    [GTKML_II_MAP_INSERT] = GTKML_SII_MAP_INSERT,
    [GTKML_II_MAP_DELETE] = GTKML_SII_MAP_DELETE,
    [GTKML_II_MAP_CONCAT] = GTKML_SII_MAP_CONCAT,
    [GTKML_II_SET_CONTAINS] = GTKML_SII_SET_CONTAINS,
    [GTKML_II_SET_INSERT] = GTKML_SII_SET_INSERT,
    [GTKML_II_SET_DELETE] = GTKML_SII_SET_DELETE,
    [GTKML_II_SET_CONCAT] = GTKML_SII_SET_CONCAT,
    [255] = NULL,
};

GTKML_PRIVATE const char *S_I_IMM_EXTERN[] = {
    [GTKML_II_PUSH_IMM] = GTKML_SII_PUSH_IMM_EXTERN,
    [GTKML_II_POP] = GTKML_SII_POP,
    [GTKML_II_SETF_IMM] = GTKML_SII_SETF_IMM,
    [GTKML_II_POPF] = GTKML_SII_POPF,
    [GTKML_II_GET_IMM] = GTKML_SII_GET_IMM,
    [GTKML_II_LIST_IMM] = GTKML_SII_LIST_IMM,
    [GTKML_II_MAP_IMM] = GTKML_SII_MAP_IMM,
    [GTKML_II_SET_IMM] = GTKML_SII_SET_IMM,
    [GTKML_II_ARRAY_IMM] = GTKML_SII_ARRAY_IMM,
    [GTKML_II_SETMM_IMM] = GTKML_SII_SETMM_IMM,
    [GTKML_II_GETMM_IMM] = GTKML_SII_GETMM_IMM,
    [GTKML_II_VAR_IMM] = GTKML_SII_VAR_IMM,
    [GTKML_II_GETVAR_IMM] = GTKML_SII_GETVAR_IMM,
    [GTKML_II_ASSIGNVAR_IMM] = GTKML_SII_ASSIGNVAR_IMM,
    [GTKML_II_LEN] = GTKML_SII_LEN,
    [GTKML_II_ARRAY_INDEX] = GTKML_SII_ARRAY_INDEX,
    [GTKML_II_ARRAY_PUSH] = GTKML_SII_ARRAY_PUSH,
    [GTKML_II_ARRAY_CONCAT] = GTKML_SII_ARRAY_CONCAT,
    [GTKML_II_ARRAY_POP] = GTKML_SII_ARRAY_POP,
    [GTKML_II_MAP_GET] = GTKML_SII_MAP_GET,
    [GTKML_II_MAP_INSERT] = GTKML_SII_MAP_INSERT,
    [GTKML_II_MAP_DELETE] = GTKML_SII_MAP_DELETE,
    [GTKML_II_MAP_CONCAT] = GTKML_SII_MAP_CONCAT,
    [GTKML_II_SET_CONTAINS] = GTKML_SII_SET_CONTAINS,
    [GTKML_II_SET_INSERT] = GTKML_SII_SET_INSERT,
    [GTKML_II_SET_DELETE] = GTKML_SII_SET_DELETE,
    [GTKML_II_SET_CONCAT] = GTKML_SII_SET_CONCAT,
    [255] = NULL,
};

GTKML_PRIVATE const char *S_I_BR[] = {
    [GTKML_IBR_CALL] = GTKML_SIBR_CALL,
    [GTKML_IBR_RET] = GTKML_SIBR_RET,
    [GTKML_IBR_CALL_STD] = GTKML_SIBR_CALL_STD,
    [GTKML_IBR_BRANCH_ABSOLUTE] = GTKML_SIBR_BRANCH_ABSOLUTE,
    [GTKML_IBR_BRANCH_RELATIVE] = GTKML_SIBR_BRANCH_RELATIVE,
    [255] = NULL,
};

GTKML_PRIVATE const char *S_EI_IMM[] = {
    [GTKML_EII_PUSH_EXT_IMM] = GTKML_SEII_PUSH_EXT_IMM,
    [GTKML_EII_POP_EXT] = GTKML_SEII_POP_EXT,
    [GTKML_EII_SETF_EXT_IMM] = GTKML_SEII_SETF_EXT_IMM,
    [GTKML_EII_POPF_EXT] = GTKML_SEII_POPF_EXT,
    [GTKML_EII_GET_EXT_IMM] = GTKML_SEII_GET_EXT_IMM,
    [GTKML_EII_LIST_EXT_IMM] = GTKML_SEII_LIST_EXT_IMM,
    [GTKML_EII_MAP_EXT_IMM] = GTKML_SEII_MAP_EXT_IMM,
    [GTKML_EII_SET_EXT_IMM] = GTKML_SEII_SET_EXT_IMM,
    [GTKML_EII_ARRAY_EXT_IMM] = GTKML_SEII_ARRAY_EXT_IMM,
    [GTKML_EII_CMP_EXT_IMM] = GTKML_SEII_CMP_EXT_IMM,
    [255] = NULL,
};

GTKML_PRIVATE const char *S_EI_IMM_EXTERN[] = {
    [GTKML_EII_PUSH_EXT_IMM] = GTKML_SEII_PUSH_EXT_IMM_EXTERN,
    [GTKML_EII_POP_EXT] = GTKML_SEII_POP_EXT,
    [GTKML_EII_SETF_EXT_IMM] = GTKML_SEII_SETF_EXT_IMM,
    [GTKML_EII_POPF_EXT] = GTKML_SEII_POPF_EXT,
    [GTKML_EII_GET_EXT_IMM] = GTKML_SEII_GET_EXT_IMM,
    [GTKML_EII_LIST_EXT_IMM] = GTKML_SEII_LIST_EXT_IMM,
    [GTKML_EII_MAP_EXT_IMM] = GTKML_SEII_MAP_EXT_IMM,
    [GTKML_EII_SET_EXT_IMM] = GTKML_SEII_SET_EXT_IMM,
    [GTKML_EII_ARRAY_EXT_IMM] = GTKML_SEII_ARRAY_EXT_IMM,
    [GTKML_EII_CMP_EXT_IMM] = GTKML_SEII_CMP_EXT_IMM,
    [255] = NULL,
};

GTKML_PRIVATE const char *S_EI_BR[] = {
    [GTKML_EIBR_CALL_EXT_STD] = GTKML_SEIBR_CALL_EXT_STD,
    [GTKML_EIBR_BRANCH_ABSOLUTE_EXT] = GTKML_SEIBR_BRANCH_ABSOLUTE_EXT,
    [GTKML_EIBR_BRANCH_RELATIVE_EXT] = GTKML_SEIBR_BRANCH_RELATIVE_EXT,
    [255] = NULL,
};

GTKML_PRIVATE const char **S_CATEGORY[] = {
    [0] = NULL,
    [GTKML_I_ARITH] = S_I_ARITH,
    [GTKML_I_IMM] = S_I_IMM,
    [GTKML_I_BR] = S_I_BR,
    [4] = NULL,
    [5] = NULL,
    [GTKML_I_IMM | GTKML_I_IMM_EXTERN] = S_I_IMM_EXTERN,
    [7] = NULL,
    [8] = NULL,
    [9] = NULL,
    [GTKML_EI_IMM] = S_EI_IMM,
    [GTKML_EI_BR] = S_EI_BR,
    [12] = NULL,
    [13] = NULL,
    [GTKML_EI_IMM | GTKML_EI_IMM_EXTERN] = S_EI_IMM_EXTERN,
    [15] = NULL,
};

GTKML_PRIVATE void default_hash_start(GtkMl_Hash *hash);
GTKML_PRIVATE gboolean default_hash_update(GtkMl_Hash *hash, void *ptr);
GTKML_PRIVATE void default_hash_finish(GtkMl_Hash *hash);
GTKML_PRIVATE gboolean default_equal(void *lhs, void *rhs);

GTKML_PRIVATE void ptr_hash_start(GtkMl_Hash *hash);
GTKML_PRIVATE gboolean ptr_hash_update(GtkMl_Hash *hash, void *ptr);
GTKML_PRIVATE void ptr_hash_finish(GtkMl_Hash *hash);
GTKML_PRIVATE gboolean ptr_equal(void *lhs, void *rhs);

GtkMl_Hasher GTKML_DEFAULT_HASHER = {
    default_hash_start,
    default_hash_update,
    default_hash_finish,
    default_equal
};

GtkMl_Hasher GTKML_PTR_HASHER = {
    ptr_hash_start,
    ptr_hash_update,
    ptr_hash_finish,
    ptr_equal
};

#define gtk_ml_add_reader(ctx, _token, reader) \
    do { \
        ctx->parser.readers[ctx->parser.len_reader].token = _token; \
        ctx->parser.readers[ctx->parser.len_reader].fn = reader; \
        ++ctx->parser.len_reader; \
    } while (0);

GtkMl_Context *gtk_ml_new_context() {
    return gtk_ml_new_context_with_gc(gtk_ml_new_gc());
}

GtkMl_Context *gtk_ml_new_context_with_gc(GtkMl_Gc *gc) {
    GtkMl_Context *ctx = malloc(sizeof(GtkMl_Context));
    ctx->is_debugger = 0;
    ctx->vm = gtk_ml_new_vm(ctx);
    ctx->gc = gc;

    // ({'flags-none G_APPLICATION_FLAGS_NONE})
    GtkMl_S *bindings = gtk_ml_new_list(ctx, NULL, gtk_ml_new_map(ctx, NULL, NULL), gtk_ml_new_nil(ctx, NULL));
    ctx->top_scope = &gtk_ml_car(bindings);
    gtk_ml_push(ctx, gtk_ml_new_var(ctx, NULL, bindings));
    ctx->bindings = ctx->gc->stack[ctx->gc->stack_len - 1];

#ifdef GTKML_ENABLE_GTK
    gtk_ml_define(ctx, gtk_ml_new_symbol(ctx, NULL, 0, "flags-none", 10), gtk_ml_new_int(ctx, NULL, G_APPLICATION_FLAGS_NONE));
#endif /* GTKML_ENABLE_GTK */
    gtk_ml_define(ctx, gtk_ml_new_symbol(ctx, NULL, 0, "cond-none", 9), gtk_ml_new_int(ctx, NULL, GTKML_F_NONE));
    gtk_ml_define(ctx, gtk_ml_new_symbol(ctx, NULL, 0, "cond-eq", 7), gtk_ml_new_int(ctx, NULL, GTKML_F_ZERO));

    ctx->parser.readers = malloc(sizeof(GtkMl_Reader) * 64);
    ctx->parser.len_reader = 0;
    ctx->parser.cap_reader = 64;

    gtk_ml_add_reader(ctx, GTKML_TOK_ELLIPSIS, gtk_ml_parse_vararg);
    gtk_ml_add_reader(ctx, GTKML_TOK_TICK, gtk_ml_parse_quote);
    gtk_ml_add_reader(ctx, GTKML_TOK_BACKTICK, gtk_ml_parse_quasiquote);
    gtk_ml_add_reader(ctx, GTKML_TOK_COMMA, gtk_ml_parse_unquote);
    gtk_ml_add_reader(ctx, GTKML_TOK_POUND, gtk_ml_parse_alternative);
    gtk_ml_add_reader(ctx, GTKML_TOK_AT, gtk_ml_parse_get);

    return ctx;
}

#ifdef GTKML_ENABLE_POSIX
GtkMl_Context *gtk_ml_new_debugger(pid_t dbg_process) {
    GtkMl_Context *ctx = gtk_ml_new_context();
    ctx->is_debugger = 1;
    ctx->dbg_process = dbg_process;
    return ctx;
}

void gtk_ml_set_debug(GtkMl_Context *ctx, pid_t dbg_process, GtkMl_Context *debugee) {
    ctx->dbg_process = dbg_process;
    ctx->dbg_ctx = debugee;
}
#endif /* GTKML_ENABLE_POSIX */

void gtk_ml_del_context(GtkMl_Context *ctx) {
    gtk_ml_del_gc(ctx, ctx->gc);
    gtk_ml_del_vm(ctx->vm);

    free(ctx->parser.readers);

    free(ctx);
}

GtkMl_Gc *gtk_ml_new_gc() {
    GtkMl_Gc *gc = malloc(sizeof(GtkMl_Gc));
    gc->rc = 1;
    gc->gc_enabled = 1;
    gc->n_values = 0;
    gc->m_values = GTKML_GC_COUNT_THRESHOLD;
    gc->first = NULL;

    gc->stack_len = 0;
    gc->stack_cap = 256;
    gc->stack = malloc(sizeof(GtkMl_S *) * gc->stack_cap);

    gc->program_len = 0;
    gc->program_cap = 16;
    gc->programs = malloc(sizeof(GtkMl_Program *) * gc->program_cap);

    gc->free_len = 0;
    gc->free_cap = 64;
    gc->free_all = malloc(sizeof(GtkMl_S *) * gc->free_cap);

    gc->builder = NULL;

    return gc;
}

GtkMl_Gc *gtk_ml_gc_copy(GtkMl_Gc *gc) {
    ++gc->rc;
    return gc;
}

GTKML_PRIVATE void free_all(GtkMl_Gc *gc) {
    for (size_t i = 0; i < gc->free_len; i++) {
        free(gc->free_all[i]);
    }
    gc->free_len = 0;
}

void gtk_ml_del_gc(GtkMl_Context *ctx, GtkMl_Gc *gc) {
    --gc->rc;
    if (!gc->rc) {
        GtkMl_S *value = gc->first;
        while (value) {
            GtkMl_S *next = value->next;
            gtk_ml_del(ctx, value);
            value = next;
        }
        free_all(gc);
        free(gc->free_all);

        for (size_t i = 0; i < gc->program_len; i++) {
            gtk_ml_del_program(gc->programs[i]);
        }
        free(gc->programs);
        free(gc->stack);
        free(gc);
    }
}

void gtk_ml_push(GtkMl_Context *ctx, GtkMl_S *value) {
    if (ctx->gc->stack_len == ctx->gc->stack_cap) {
        ctx->gc->stack_cap *= 2;
        ctx->gc->stack = realloc(ctx->gc->stack, sizeof(GtkMl_S *) * ctx->gc->stack_cap);
    }
    ctx->gc->stack[ctx->gc->stack_len++] = value;
}

GtkMl_S *gtk_ml_pop(GtkMl_Context *ctx) {
    GtkMl_S *result = ctx->gc->stack[--ctx->gc->stack_len];
    ctx->gc->stack[ctx->gc->stack_len] = NULL;
    return result;
}

GtkMl_S *gtk_ml_peek(GtkMl_Context *ctx) {
    return ctx->gc->stack[ctx->gc->stack_len - 1];
}

void gtk_ml_del_context(GtkMl_Context *ctx) {
    gtk_ml_del_gc(ctx, ctx->gc);
    gtk_ml_del_vm(ctx->vm);

    free(ctx->parser.readers);

    free(ctx);
}

void gtk_ml_load_program(GtkMl_Context *ctx, GtkMl_Program* program) {
    ctx->vm->program = program;
}

gboolean gtk_ml_run_program_internal(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *program, GtkMl_S *args, gboolean brk) {
    gtk_ml_enter(ctx);

    GtkMl_S *params = program->value.s_program.args;

    int64_t n_args = 0;
    int64_t n_params = 0;

    if (args) {
        while (args->kind != GTKML_S_NIL) {
            GtkMl_S *arg = gtk_ml_car(args);
            gtk_ml_push(ctx, arg);
            args = gtk_ml_cdr(args);
            ++n_args;
        }
    }

    GtkMl_S *last_param = NULL;
    while (params->kind != GTKML_S_NIL) {
        last_param = gtk_ml_car(params);
        params = gtk_ml_cdr(params);
        ++n_params;
    }

    if (n_params < n_args) {
        if (!last_param || last_param->kind != GTKML_S_VARARG) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return 0;
        }
    }

    gtk_ml_push(ctx, gtk_ml_new_int(ctx, NULL, n_args));

    ctx->vm->reg[GTKML_R_PC].pc = program->value.s_program.addr;
    gboolean result = gtk_ml_vm_run(ctx->vm, err, brk);

    if (ctx->bindings->value.s_var.expr->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "scope-error", GTKML_ERR_SCOPE_ERROR, 0, 0, 0, 0);
        return 0;
    }
    gtk_ml_leave(ctx);

    return result;
}

gboolean gtk_ml_run_program(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *program, GtkMl_S *args) {
    return gtk_ml_run_program_internal(ctx, err, program, args, 1);
}

GtkMl_S *gtk_ml_get_export(GtkMl_Context *ctx, GtkMl_S **err, const char *linkage_name) {
    GtkMl_S *ext = gtk_ml_new_string(ctx, NULL, linkage_name, strlen(linkage_name));
    for (size_t i = 0; i < ctx->vm->program->n_exec;) {
        GtkMl_Instruction instr = ctx->vm->program->exec[i];
        if (instr.gen.category == GTKML_EI_EXPORT) {
            GtkMl_S *program = ctx->vm->program->statics[ctx->vm->program->exec[i + 1].imm64];
            if (gtk_ml_equal(ext, program->value.s_program.linkage_name)) {
                return program;
            }
        }
        if (instr.gen.category & GTKML_I_EXTENDED) {
            i += 2;
        } else {
            ++i;
        }
    }

    GtkMl_S *error = gtk_ml_error(ctx, "binding-error", GTKML_ERR_BINDING_ERROR, 0, 0, 0, 0);
    GtkMl_S *new = gtk_ml_new_map(ctx, NULL, NULL);
    gtk_ml_del_hash_trie(ctx, &new->value.s_map.map, gtk_ml_delete_void_reference);
    char *name = malloc(strlen(linkage_name) + 1);
    strcpy(name, linkage_name);
    gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_new_keyword(ctx, NULL, 0, "binding", strlen("binding")), gtk_ml_new_symbol(ctx, NULL, 1, name, strlen(name)));
    error = new;
    *err = error;
    return NULL;
}

void gtk_ml_del_program(GtkMl_Program* program) {
    free((void *) program->start);
    free(program->exec);
    free(program->statics);
    free(program);
}

GTKML_PRIVATE GtkMl_VisitResult array_to_c_str(GtkMl_Array *array, size_t idx, GtkMl_S *value, void *data) {
    (void) array;
    char *c_str = data;
    c_str[idx] = value->value.s_char.value;
    return GTKML_VISIT_RECURSE;
}

char *gtk_ml_to_c_str(GtkMl_S *string) {
    GtkMl_Array *array = &string->value.s_array.array;
    size_t len = gtk_ml_array_trie_len(array);
    char *c_str = malloc(len + 1);
    gtk_ml_array_trie_foreach(array, array_to_c_str, c_str);
    c_str[len] = 0;
    return c_str;
}

GtkMl_S *gtk_ml_load(GtkMl_Context *ctx, char **src, GtkMl_S **err, const char *file) {
    FILE *stream = fopen(file, "r");
    if (!stream) {
        *err = gtk_ml_error(ctx, "io-error", GTKML_ERR_IO_ERROR, 0, 0, 0, 0);
        return NULL;
    }
    GtkMl_S *result = gtk_ml_loadf(ctx, src, err, stream);
    fclose(stream);
    return result;
}

GtkMl_S *gtk_ml_loadf(GtkMl_Context *ctx, char **src, GtkMl_S **err, FILE *stream) {
    fseek(stream, 0l, SEEK_END);
    size_t size = ftell(stream);
    fseek(stream, 0l, SEEK_SET);
    *src = malloc(size + 1);
    size_t read = fread(*src, 1, size, stream);
    if (read != size) {
        *err = gtk_ml_error(ctx, "io-error", GTKML_ERR_IO_ERROR, 0, 0, 0, 0);
        return NULL;
    }
    (*src)[size] = 0;
    return gtk_ml_loads(ctx, err, *src);
}

GtkMl_S *gtk_ml_loads(GtkMl_Context *ctx, GtkMl_S **err, const char *src) {
    GtkMl_Token *tokenv;
    size_t tokenc;
    if (!gtk_ml_lex(ctx, &tokenv, &tokenc, err, src)) {
        return NULL;
    }
    GtkMl_Token *_tokenv = tokenv;

    GtkMl_S *body = gtk_ml_new_nil(ctx, NULL);
    GtkMl_S **last = &body;

    while (tokenc) {
        GtkMl_S *line = gtk_ml_parse(ctx, err, &_tokenv, &tokenc);
        if (!line) {
            return NULL;
        }
        GtkMl_S *new = gtk_ml_new_list(ctx, NULL, line, *last);
        *last = new;
        last = &gtk_ml_cdr(new);
    }

    GtkMl_S *result = gtk_ml_new_lambda(ctx, NULL, gtk_ml_new_nil(ctx, NULL), body, gtk_ml_new_nil(ctx, NULL));

    free(tokenv);
    return result;
}

GTKML_PRIVATE void jenkins_start(GtkMl_Hash *hash) {
    *hash = 0;
}

GTKML_PRIVATE void jenkins_update(GtkMl_Hash *hash, const void *_ptr, size_t len) {
    GtkMl_Hash h = *hash;
    const char *ptr = _ptr;
    for (size_t i = 0; i < len; i++) {
        h += ptr[i];
        h += h << 10;
        h ^= h >> 6;
    }
    *hash = h;
}

GTKML_PRIVATE void jenkins_finish(GtkMl_Hash *hash) {
    GtkMl_Hash h = *hash;
    h += h << 3;
    h ^= h >> 11;
    h += h << 15;
    *hash = h;
}

struct HashData {
    GtkMl_Hash *hash;
    gboolean (*hasher)(GtkMl_Hash *hash, void *value);
};

GTKML_PRIVATE GtkMl_VisitResult hash_trie_update(GtkMl_HashTrie *ht, void *key, void *value, void *_data) {
    (void) ht;

    struct HashData *data = _data;
    data->hasher(data->hash, key);
    data->hasher(data->hash, value);

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult hash_set_update(GtkMl_HashSet *hs, void *value, void *_data) {
    (void) hs;

    struct HashData *data = _data;
    data->hasher(data->hash, value);

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult array_update(GtkMl_Array *array, size_t index, GtkMl_S *value, void *_data) {
    (void) array;
    (void) index;

    struct HashData *data = _data;
    data->hasher(data->hash, value);

    return GTKML_VISIT_RECURSE;
}

void default_hash_start(GtkMl_Hash *hash) {
    jenkins_start(hash);
}

gboolean default_hash_update(GtkMl_Hash *hash, void *ptr) {
    GtkMl_S *value = ptr;

    jenkins_update(hash, &value->kind, sizeof(GtkMl_SKind));
    switch (value->kind) {
    case GTKML_S_NIL:
    case GTKML_S_FALSE:
    case GTKML_S_TRUE:
        break;
    case GTKML_S_INT:
        jenkins_update(hash, &value->value.s_int.value, sizeof(int64_t));
        break;
    case GTKML_S_FLOAT:
        return 0;
    case GTKML_S_CHAR:
        jenkins_update(hash, &value->value.s_char.value, sizeof(uint32_t));
        break;
    case GTKML_S_SYMBOL:
        jenkins_update(hash, value->value.s_symbol.ptr, value->value.s_symbol.len);
        break;
    case GTKML_S_KEYWORD:
        jenkins_update(hash, value->value.s_keyword.ptr, value->value.s_keyword.len);
        break;
    case GTKML_S_LIST:
        do {
            default_hash_update(hash, gtk_ml_car(value));
            value = gtk_ml_cdr(value);
        } while (value->kind != GTKML_S_NIL);
        break;
    case GTKML_S_MAP: {
        struct HashData data = { hash, default_hash_update };
        gtk_ml_hash_trie_foreach(&value->value.s_map.map, hash_trie_update, &data);
    } break;
    case GTKML_S_SET: {
        struct HashData data = { hash, default_hash_update };
        gtk_ml_hash_set_foreach(&value->value.s_set.set, hash_set_update, &data);
    } break;
    case GTKML_S_ARRAY: {
        struct HashData data = { hash, default_hash_update };
        gtk_ml_array_trie_foreach(&value->value.s_array.array, array_update, &data);
    } break;
    case GTKML_S_VAR:
        default_hash_update(hash, value->value.s_var.expr);
        break;
    case GTKML_S_VARARG:
        default_hash_update(hash, value->value.s_vararg.expr);
        break;
    case GTKML_S_QUOTE:
        default_hash_update(hash, value->value.s_quote.expr);
        break;
    case GTKML_S_QUASIQUOTE:
        default_hash_update(hash, value->value.s_quasiquote.expr);
        break;
    case GTKML_S_UNQUOTE:
        default_hash_update(hash, value->value.s_unquote.expr);
        break;
    case GTKML_S_LAMBDA:
        default_hash_update(hash, value->value.s_lambda.args);
        default_hash_update(hash, value->value.s_lambda.body);
        default_hash_update(hash, value->value.s_lambda.capture);
        break;
    case GTKML_S_PROGRAM:
        default_hash_update(hash, value->value.s_program.linkage_name);
        jenkins_update(hash, &value->value.s_program.addr, sizeof(uint64_t));
        default_hash_update(hash, value->value.s_program.args);
        default_hash_update(hash, value->value.s_program.body);
        default_hash_update(hash, value->value.s_program.capture);
        break;
    case GTKML_S_ADDRESS:
        jenkins_update(hash, &value->value.s_address.addr, sizeof(uint64_t));
        break;
    case GTKML_S_MACRO:
        default_hash_update(hash, value->value.s_macro.args);
        default_hash_update(hash, value->value.s_macro.body);
        default_hash_update(hash, value->value.s_macro.capture);
        break;
    case GTKML_S_LIGHTDATA:
        jenkins_update(hash, &value->value.s_lightdata.userdata, sizeof(void *));
        break;
    case GTKML_S_USERDATA:
        jenkins_update(hash, &value->value.s_userdata.userdata, sizeof(void *));
        break;
    }
    return 1;
}

void default_hash_finish(GtkMl_Hash *hash) {
    jenkins_finish(hash);
}

gboolean default_equal(void *lhs, void *rhs) {
    return gtk_ml_equal(lhs, rhs);
}

void ptr_hash_start(GtkMl_Hash *hash) {
    jenkins_start(hash);
}

gboolean ptr_hash_update(GtkMl_Hash *hash, void *ptr) {
    *hash = (size_t) ptr >> 3;
    // jenkins_update(hash, &ptr, sizeof(void *));
    return 1;
}

void ptr_hash_finish(GtkMl_Hash *hash) {
    jenkins_finish(hash);
}

gboolean ptr_equal(void *lhs, void *rhs) {
    return lhs == rhs;
}

GTKML_PRIVATE void mark_value(GtkMl_S *s);

GTKML_PRIVATE GtkMl_VisitResult mark_hash_trie(GtkMl_HashTrie *ht, void *key, void *value, void *data) {
    (void) ht;
    (void) data;

    mark_value(key);
    mark_value(value);

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult mark_hash_set(GtkMl_HashSet *hs, void *key, void *data) {
    (void) hs;
    (void) data;

    mark_value(key);

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult mark_array(GtkMl_Array *array, size_t idx, GtkMl_S *value, void *data) {
    (void) array;
    (void) idx;
    (void) data;

    mark_value(value);

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE void mark_value(GtkMl_S *s) {
    if (s->flags & GTKML_FLAG_REACHABLE) {
        return;
    }

    s->flags |= GTKML_FLAG_REACHABLE;

    switch (s->kind) {
    case GTKML_S_NIL:
    case GTKML_S_TRUE:
    case GTKML_S_FALSE:
    case GTKML_S_INT:
    case GTKML_S_FLOAT:
    case GTKML_S_CHAR:
    case GTKML_S_KEYWORD:
    case GTKML_S_SYMBOL:
    case GTKML_S_LIGHTDATA:
        break;
    case GTKML_S_USERDATA:
        mark_value(s->value.s_userdata.keep);
        break;
    case GTKML_S_LIST:
        mark_value(gtk_ml_car(s));
        mark_value(gtk_ml_cdr(s));
        break;
    case GTKML_S_MAP:
        gtk_ml_hash_trie_foreach(&s->value.s_map.map, mark_hash_trie, NULL);
        if (s->value.s_map.metamap) {
            mark_value(s->value.s_map.metamap);
        }
        break;
    case GTKML_S_SET:
        gtk_ml_hash_set_foreach(&s->value.s_set.set, mark_hash_set, NULL);
        break;
    case GTKML_S_ARRAY:
        gtk_ml_array_trie_foreach(&s->value.s_array.array, mark_array, NULL);
        break;
    case GTKML_S_VAR:
        mark_value(s->value.s_var.expr);
        break;
    case GTKML_S_VARARG:
        mark_value(s->value.s_vararg.expr);
        break;
    case GTKML_S_QUOTE:
        mark_value(s->value.s_quote.expr);
        break;
    case GTKML_S_QUASIQUOTE:
        mark_value(s->value.s_quasiquote.expr);
        break;
    case GTKML_S_UNQUOTE:
        mark_value(s->value.s_unquote.expr);
        break;
    case GTKML_S_ADDRESS:
        mark_value(s->value.s_address.linkage_name);
        break;
    case GTKML_S_PROGRAM:
        mark_value(s->value.s_program.linkage_name);
        mark_value(s->value.s_program.args);
        mark_value(s->value.s_program.body);
        mark_value(s->value.s_program.capture);
        break;
    case GTKML_S_LAMBDA:
        mark_value(s->value.s_lambda.args);
        mark_value(s->value.s_lambda.body);
        mark_value(s->value.s_lambda.capture);
        break;
    case GTKML_S_MACRO:
        mark_value(s->value.s_macro.args);
        mark_value(s->value.s_macro.body);
        mark_value(s->value.s_macro.capture);
        break;
    }
}

GTKML_PRIVATE void mark_program(GtkMl_Program *program) {
    for (GtkMl_Static i = 1; i < program->n_static; i++) {
        mark_value(program->statics[i]);
    }
}

GTKML_PRIVATE void mark_builder(GtkMl_Builder *b) {
    for (GtkMl_Static i = 1; i < b->len_static; i++) {
        mark_value(b->statics[i]);
    }
}

GTKML_PRIVATE void mark(GtkMl_Context *ctx) {
    for (size_t sp = 0; sp < ctx->gc->stack_len; sp++) {
        mark_value(ctx->gc->stack[sp]);
    }
    for (size_t i = 0; i < ctx->gc->program_len; i++) {
        mark_program(ctx->gc->programs[i]);
    }
    if (ctx->gc->builder) {
        mark_builder(ctx->gc->builder);
    }
}

void gtk_ml_delete(GtkMl_Context *ctx, GtkMl_S *s) {
    if ((s->flags & GTKML_FLAG_REACHABLE) || (s->flags & GTKML_FLAG_DELETE)) {
        return;
    }

    s->flags |= GTKML_FLAG_DELETE;

    switch (s->kind) {
    case GTKML_S_NIL:
    case GTKML_S_TRUE:
    case GTKML_S_FALSE:
    case GTKML_S_INT:
    case GTKML_S_FLOAT:
    case GTKML_S_CHAR:
    case GTKML_S_LIGHTDATA:
        break;
    case GTKML_S_KEYWORD:
        if (s->value.s_keyword.owned) {
            free((void *) s->value.s_keyword.ptr);
        }
        break;
    case GTKML_S_SYMBOL:
        if (s->value.s_symbol.owned) {
            free((void *) s->value.s_symbol.ptr);
        }
        break;
    case GTKML_S_USERDATA:
        s->value.s_userdata.del(ctx, s->value.s_userdata.userdata);
        break;
    case GTKML_S_LIST:
        gtk_ml_delete(ctx, gtk_ml_cdr(s));
        gtk_ml_delete(ctx, gtk_ml_car(s));
        break;
    case GTKML_S_MAP:
        gtk_ml_del_hash_trie(ctx, &s->value.s_map.map, gtk_ml_delete_value);
        break;
    case GTKML_S_SET:
        gtk_ml_del_hash_set(ctx, &s->value.s_set.set, gtk_ml_delete_value);
        break;
    case GTKML_S_ARRAY:
        gtk_ml_del_array_trie(ctx, &s->value.s_array.array, gtk_ml_delete);
        break;
    case GTKML_S_VAR:
        gtk_ml_delete(ctx, s->value.s_var.expr);
        break;
    case GTKML_S_VARARG:
        gtk_ml_delete(ctx, s->value.s_vararg.expr);
        break;
    case GTKML_S_QUOTE:
        gtk_ml_delete(ctx, s->value.s_quote.expr);
        break;
    case GTKML_S_QUASIQUOTE:
        gtk_ml_delete(ctx, s->value.s_quasiquote.expr);
        break;
    case GTKML_S_UNQUOTE:
        gtk_ml_delete(ctx, s->value.s_unquote.expr);
        break;
    case GTKML_S_ADDRESS:
        gtk_ml_delete(ctx, s->value.s_address.linkage_name);
        break;
    case GTKML_S_PROGRAM:
        gtk_ml_delete(ctx, s->value.s_program.linkage_name);
        gtk_ml_delete(ctx, s->value.s_program.args);
        gtk_ml_delete(ctx, s->value.s_program.capture);
        break;
    case GTKML_S_LAMBDA:
        gtk_ml_delete(ctx, s->value.s_lambda.args);
        gtk_ml_delete(ctx, s->value.s_lambda.body);
        gtk_ml_delete(ctx, s->value.s_lambda.capture);
        break;
    case GTKML_S_MACRO:
        gtk_ml_delete(ctx, s->value.s_macro.args);
        gtk_ml_delete(ctx, s->value.s_macro.body);
        gtk_ml_delete(ctx, s->value.s_macro.capture);
        break;
    }
    if (ctx->gc->free_len == ctx->gc->free_cap) {
        ctx->gc->free_cap *= 2;
        ctx->gc->free_all = realloc(ctx->gc->free_all, sizeof(GtkMl_S *) * ctx->gc->free_cap);
    }
    ctx->gc->free_all[ctx->gc->free_len++] = s;
    --ctx->gc->n_values;
}

void gtk_ml_del(GtkMl_Context *ctx, GtkMl_S *s) {
    switch (s->kind) {
    case GTKML_S_NIL:
    case GTKML_S_TRUE:
    case GTKML_S_FALSE:
    case GTKML_S_INT:
    case GTKML_S_FLOAT:
    case GTKML_S_CHAR:
    case GTKML_S_LIGHTDATA:
    case GTKML_S_LIST:
    case GTKML_S_VAR:
    case GTKML_S_VARARG:
    case GTKML_S_QUOTE:
    case GTKML_S_QUASIQUOTE:
    case GTKML_S_UNQUOTE:
    case GTKML_S_LAMBDA:
    case GTKML_S_ADDRESS:
    case GTKML_S_PROGRAM:
    case GTKML_S_MACRO:
        break;
    case GTKML_S_MAP:
        gtk_ml_del_hash_trie(ctx, &s->value.s_map.map, gtk_ml_delete_void_reference);
        break;
    case GTKML_S_SET:
        gtk_ml_del_hash_set(ctx, &s->value.s_set.set, gtk_ml_delete_void_reference);
        break;
    case GTKML_S_ARRAY:
        gtk_ml_del_array_trie(ctx, &s->value.s_array.array, gtk_ml_delete_value_reference);
        break;
    case GTKML_S_KEYWORD:
        if (s->value.s_keyword.owned) {
            free((void *) s->value.s_keyword.ptr);
        }
        break;
    case GTKML_S_SYMBOL:
        if (s->value.s_symbol.owned) {
            free((void *) s->value.s_symbol.ptr);
        }
        break;
    case GTKML_S_USERDATA:
        s->value.s_userdata.del(ctx, s->value.s_userdata.userdata);
        break;
    }
    if (ctx->gc->free_len == ctx->gc->free_cap) {
        ctx->gc->free_cap *= 2;
        ctx->gc->free_all = realloc(ctx->gc->free_all, sizeof(GtkMl_S *) * ctx->gc->free_cap);
    }
    ctx->gc->free_all[ctx->gc->free_len++] = s;
    --ctx->gc->n_values;
}

GTKML_PRIVATE void sweep(GtkMl_Context *ctx) {
    GtkMl_S **value = &ctx->gc->first;
    while (*value) {
        if ((*value)->flags & GTKML_FLAG_REACHABLE) {
            (*value)->flags &= ~GTKML_FLAG_REACHABLE;
            value = &(*value)->next;
        } else {
            GtkMl_S *unreachable = *value;
            *value = (*value)->next;
            gtk_ml_del(ctx, unreachable);
        }
    }
}

// simple mark & sweep gc
gboolean gtk_ml_collect(GtkMl_Context *ctx) {
    if (!ctx->gc->gc_enabled) {
        return 0;
    }

    if (ctx->gc->n_values < ctx->gc->m_values) {
        return 0;
    }

    size_t n_values = ctx->gc->n_values;
    mark(ctx);
    sweep(ctx);
    free_all(ctx->gc);
    ctx->gc->m_values = 2 * n_values;

    return 1;
}

gboolean gtk_ml_disable_gc(GtkMl_Context *ctx) {
    gboolean enabled = ctx->gc->gc_enabled;
    ctx->gc->gc_enabled = 0;
    return enabled;
}

void gtk_ml_enable_gc(GtkMl_Context *ctx, gboolean enabled) {
    ctx->gc->gc_enabled = enabled;
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

gboolean gtk_ml_hash_debug(GtkMl_Context *ctx, GtkMl_Hasher *hasher, GtkMl_Hash *hash, GtkMl_S *value) {
    (void) ctx;
    (void) hasher;
    (void) hash;
    (void) value;
    fprintf(stderr, "warning: hashing in debug mode is currently unavailable\n");
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

#ifdef GTKML_ENABLE_GTK
void gtk_ml_object_unref(GtkMl_Context *ctx, void *obj) {
    (void) ctx;
    g_object_unref(obj);
}
#endif /* GTKML_ENABLE_GTK */

#ifdef GTKML_ENABLE_POSIX
struct DumpfDebugData {
    FILE *stream;
    size_t n;
};

GTKML_PRIVATE GtkMl_VisitResult dumpf_debug_hash_trie(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_HashTrie *ht, void *key, void *value, void *_data) {
    struct DumpfDebugData *data = _data;

    gtk_ml_dumpf_debug(ctx, data->stream, err, key);
    if (*err) {
        return GTKML_VISIT_BREAK;
    }
    fprintf(data->stream, " ");
    gtk_ml_dumpf_debug(ctx, data->stream, err, value);
    if (*err) {
        return GTKML_VISIT_BREAK;
    }
    ++data->n;
    if (data->n < gtk_ml_hash_trie_len_debug(ctx, ht)) {
        fprintf(data->stream, " ");
    }

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult dumpf_debug_hash_set(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_HashSet *hs, void *key, void *_data) {
    struct DumpfDebugData *data = _data;

    gtk_ml_dumpf_debug(ctx, data->stream, err, key);
    if (*err) {
        return GTKML_VISIT_BREAK;
    }
    ++data->n;
    if (data->n < gtk_ml_hash_set_len_debug(ctx, hs)) {
        fprintf(data->stream, " ");
    }

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult dumpf_debug_array(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Array *array, size_t idx, GtkMl_S *value, void *_data) {
    (void) idx;

    struct DumpfDebugData *data = _data;

    gtk_ml_dumpf_debug(ctx, data->stream, err, value);
    if (*err) {
        return GTKML_VISIT_BREAK;
    }
    ++data->n;
    if (data->n < gtk_ml_array_trie_len(array)) {
        fprintf(data->stream, " ");
    }

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult dumpf_debug_string(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Array *array, size_t idx, GtkMl_S *value, void *data) {
    (void) ctx;
    (void) err;
    (void) array;
    (void) idx;
    (void) data;

    GtkMl_S expr_value;
    expr_value.next = NULL;
    gtk_ml_dbg_read_value(ctx, err, &expr_value, value);
    if (*err) {
        return 0;
    }
    fprintf(data, "%c", expr_value.value.s_char.value);

    return GTKML_VISIT_RECURSE;
}

gboolean gtk_ml_dumpf_debug(GtkMl_Context *ctx, FILE *stream, GtkMl_S **err, GtkMl_S *expr_ptr) {
    GtkMl_S expr_value;
    expr_value.next = NULL;
    gtk_ml_dbg_read_value(ctx, err, &expr_value, expr_ptr);
    if (*err) {
        return 0;
    }
    GtkMl_S *expr = &expr_value;
    switch (expr->kind) {
    case GTKML_S_NIL:
        fprintf(stream, "#nil");
        return 1;
    case GTKML_S_TRUE:
        fprintf(stream, "#t");
        return 1;
    case GTKML_S_FALSE:
        fprintf(stream, "#f");
        return 1;
    case GTKML_S_INT:
        fprintf(stream, "%"GTKML_FMT_64"d", expr->value.s_int.value);
        return 1;
    case GTKML_S_FLOAT:
        fprintf(stream, "%f", expr->value.s_float.value);
        return 1;
    case GTKML_S_CHAR:
        switch (expr->value.s_char.value) {
        case '\n':
            fprintf(stream, "\\newline");
            break;
        case ' ':
            fprintf(stream, "\\space");
            break;
        case '\t':
            fprintf(stream, "\\tab");
            break;
        case '\x1b':
            fprintf(stream, "\\escape");
            break;
        default:
            if (isgraph(expr->value.s_char.value)) {
                fprintf(stream, "\\%c", expr->value.s_char.value);
            } else if (expr->value.s_char.value < 0x100) {
                fprintf(stream, "\\x%x", expr->value.s_char.value);
            } else if (expr->value.s_char.value < 0x10000) {
                fprintf(stream, "\\u%x", expr->value.s_char.value);
            } else {
                fprintf(stream, "\\U%x", expr->value.s_char.value);
            }
            break;
        }
        return 1;
    case GTKML_S_KEYWORD:
        fprintf(stream, ":");
        for (size_t i = 0; i < expr->value.s_keyword.len; i++) {
            fprintf(stream, "%c", gtk_ml_dbg_read_u8(ctx, err, &expr->value.s_keyword.ptr[i]));
            if (*err) {
                return 0;
            }
        }
        return 1;
    case GTKML_S_SYMBOL:
        fprintf(stream, "'");
        for (size_t i = 0; i < expr->value.s_symbol.len; i++) {
            fprintf(stream, "%c", gtk_ml_dbg_read_u8(ctx, err, &expr->value.s_symbol.ptr[i]));
            if (*err) {
                return 0;
            }
        }
        return 1;
    case GTKML_S_LIST:
        fprintf(stream, "(");
        while (expr->kind != GTKML_S_NIL) {
            gtk_ml_dumpf_debug(ctx, stream, err, gtk_ml_car(expr));
            gtk_ml_dbg_read_value(ctx, err, expr, gtk_ml_cdr(expr));
            if (*err) {
                return 0;
            }
            if (expr->kind != GTKML_S_NIL) {
                fprintf(stream, " ");
            }
        }
        fprintf(stream, ")");
        return 1;
    case GTKML_S_MAP: {
        fprintf(stream, "{");
        struct DumpfDebugData data = { stream, 0 };
        gtk_ml_hash_trie_foreach_debug(ctx, err, &expr->value.s_map.map, dumpf_debug_hash_trie, &data);
        fprintf(stream, "}");
        return 1;
    }
    case GTKML_S_SET: {
        fprintf(stream, "#{");
        struct DumpfDebugData data = { stream, 0 };
        gtk_ml_hash_set_foreach_debug(ctx, err, &expr->value.s_set.set, dumpf_debug_hash_set, &data);
        fprintf(stream, "}");
        return 1;
    }
    case GTKML_S_ARRAY: {
        if (gtk_ml_array_trie_len_debug(ctx, &expr->value.s_array.array) > 0 && gtk_ml_array_trie_is_string_debug(ctx, err, &expr->value.s_array.array)) {
            fprintf(stream, "\"");
            gtk_ml_array_trie_foreach_debug(ctx, err, &expr->value.s_array.array, dumpf_debug_string, stream);
            fprintf(stream, "\"");
            return 1;
        } else {
            fprintf(stream, "[");
            struct DumpfDebugData data = { stream, 0 };
            gtk_ml_array_trie_foreach_debug(ctx, err, &expr->value.s_array.array, dumpf_debug_array, &data);
            fprintf(stream, "]");
            return 1;
        }
    }
    case GTKML_S_VAR:
        fprintf(stream, "(var ");
        if (!gtk_ml_dumpf_debug(ctx, stream, err, expr->value.s_vararg.expr)) {
            return 0;
        }
        fprintf(stream, ")");
        return 1;
    case GTKML_S_VARARG:
        fprintf(stream, "...");
        return gtk_ml_dumpf_debug(ctx, stream, err, expr->value.s_vararg.expr);
    case GTKML_S_QUOTE:
        fprintf(stream, "'");
        return gtk_ml_dumpf_debug(ctx, stream, err, expr->value.s_vararg.expr);
    case GTKML_S_QUASIQUOTE:
        fprintf(stream, "`");
        return gtk_ml_dumpf_debug(ctx, stream, err, expr->value.s_vararg.expr);
    case GTKML_S_UNQUOTE:
        fprintf(stream, ",");
        return gtk_ml_dumpf_debug(ctx, stream, err, expr->value.s_vararg.expr);
    case GTKML_S_LAMBDA: {
        fprintf(stream, "(lambda ");
        if (!gtk_ml_dumpf_debug(ctx, stream, err, expr->value.s_lambda.args)) {
            return 0;
        }
        fprintf(stream, " ");
        GtkMl_S body_value;
        body_value.next = NULL;
        gtk_ml_dbg_read_value(ctx, err, &body_value, expr->value.s_lambda.body);
        if (*err) {
            return 0;
        }
        GtkMl_S *body = &body_value;
        while (body->kind != GTKML_S_NIL) {
            if (!gtk_ml_dumpf_debug(ctx, stream, err, gtk_ml_car(body))) {
                return 0;
            }
            gtk_ml_dbg_read_value(ctx, err, body, gtk_ml_cdr(body));
            if (*err) {
                return 0;
            }
            if (body->kind != GTKML_S_NIL) {
                fprintf(stream, " ");
            }
        }
        fprintf(stream, ")");
        return 1;
    }
    case GTKML_S_PROGRAM: {
        switch (expr->value.s_program.kind) {
        case GTKML_PROG_INTRINSIC:
            fprintf(stream, "(program-intrinsic ");
            gtk_ml_dumpf_debug(ctx, stream, err, expr->value.s_program.linkage_name);
            fprintf(stream, " 0x%"GTKML_FMT_64"x ", expr->value.s_program.addr);
            break;
        case GTKML_PROG_MACRO:
            fprintf(stream, "(program-macro ");
            gtk_ml_dumpf_debug(ctx, stream, err, expr->value.s_program.linkage_name);
            fprintf(stream, " 0x%"GTKML_FMT_64"x ", expr->value.s_program.addr);
            break;
        case GTKML_PROG_RUNTIME:
            fprintf(stream, "(program ");
            gtk_ml_dumpf_debug(ctx, stream, err, expr->value.s_program.linkage_name);
            fprintf(stream, " 0x%"GTKML_FMT_64"x ", expr->value.s_program.addr);
            break;
        }
        if (!gtk_ml_dumpf_debug(ctx, stream, err, expr->value.s_program.args)) {
            return 0;
        }
        fprintf(stream, " ");
        GtkMl_S body_value;
        gtk_ml_dbg_read_value(ctx, err, &body_value, expr->value.s_program.body);
        if (*err) {
            return 0;
        }
        GtkMl_S *body = &body_value;
        while (body->kind != GTKML_S_NIL) {
            if (!gtk_ml_dumpf_debug(ctx, stream, err, gtk_ml_car(body))) {
                return 0;
            }
            gtk_ml_dbg_read_value(ctx, err, body, gtk_ml_cdr(body));
            if (*err) {
                return 0;
            }
            if (body->kind != GTKML_S_NIL) {
                fprintf(stream, " ");
            }
        }
        fprintf(stream, ")");
        return 1;
    }
    case GTKML_S_ADDRESS:
        fprintf(stream, "(address ");
        gtk_ml_dumpf(ctx, stream, err, expr->value.s_address.linkage_name);
        fprintf(stream, " 0x%"GTKML_FMT_64"x)", expr->value.s_address.addr);
        return 1;
    case GTKML_S_MACRO: {
        fprintf(stream, "(macro ");
        gtk_ml_dumpf_debug(ctx, stream, err, expr->value.s_macro.args);
        fprintf(stream, " ");
        GtkMl_S body_value;
        gtk_ml_dbg_read_value(ctx, err, &body_value, expr->value.s_macro.body);
        if (*err) {
            return 0;
        }
        GtkMl_S *body = &body_value;
        while (body->kind != GTKML_S_NIL) {
            gtk_ml_dumpf_debug(ctx, stream, err, gtk_ml_car(body));
            gtk_ml_dbg_read_value(ctx, err, body, gtk_ml_cdr(body));
            if (*err) {
                return 0;
            }
            if (body->kind != GTKML_S_NIL) {
                fprintf(stream, " ");
            }
        }
        fprintf(stream, ")");
        return 1;
    }
    case GTKML_S_LIGHTDATA:
        fprintf(stream, "%p", expr->value.s_lightdata.userdata);
        return 1;
    case GTKML_S_USERDATA:
        fprintf(stream, "%p", expr->value.s_lightdata.userdata);
        return 1;
    default:
        *err = gtk_ml_error(ctx, "invalid-sexpr", GTKML_ERR_INVALID_SEXPR, expr->span.ptr != NULL, expr->span.line, expr->span.col, 1, gtk_ml_new_keyword(ctx, NULL, 0, "kind", strlen("kind")), gtk_ml_new_int(ctx, NULL, expr->kind));
        return 0;
    }
}
#endif /* GTKML_ENABLE_POSIX */

struct DumpfData {
    GtkMl_Context *ctx;
    FILE *stream;
    GtkMl_S **err;
    size_t n;
};

GTKML_PRIVATE GtkMl_VisitResult dumpf_hash_trie(GtkMl_HashTrie *ht, void *key, void *value, void *_data) {
    struct DumpfData *data = _data;

    gtk_ml_dumpf(data->ctx, data->stream, data->err, key);
    fprintf(data->stream, " ");
    gtk_ml_dumpf(data->ctx, data->stream, data->err, value);
    ++data->n;
    if (data->n < gtk_ml_hash_trie_len(ht)) {
        fprintf(data->stream, " ");
    }

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult dumpf_hash_set(GtkMl_HashSet *hs, void *key, void *_data) {
    struct DumpfData *data = _data;

    gtk_ml_dumpf(data->ctx, data->stream, data->err, key);
    ++data->n;
    if (data->n < gtk_ml_hash_set_len(hs)) {
        fprintf(data->stream, " ");
    }

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult dumpf_array(GtkMl_Array *array, size_t idx, GtkMl_S *key, void *_data) {
    (void) idx;

    struct DumpfData *data = _data;

    gtk_ml_dumpf(data->ctx, data->stream, data->err, key);
    ++data->n;
    if (data->n < gtk_ml_array_trie_len(array)) {
        fprintf(data->stream, " ");
    }

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult dumpf_string(GtkMl_Array *array, size_t idx, GtkMl_S *key, void *data) {
    (void) array;
    (void) idx;
    (void) data;

    fprintf(data, "%c", key->value.s_char.value);

    return GTKML_VISIT_RECURSE;
}

gboolean gtk_ml_dumpf(GtkMl_Context *ctx, FILE *stream, GtkMl_S **err, GtkMl_S *expr) {
    switch (expr->kind) {
    case GTKML_S_NIL:
        fprintf(stream, "#nil");
        return 1;
    case GTKML_S_TRUE:
        fprintf(stream, "#t");
        return 1;
    case GTKML_S_FALSE:
        fprintf(stream, "#f");
        return 1;
    case GTKML_S_INT:
        fprintf(stream, "%"GTKML_FMT_64"d", expr->value.s_int.value);
        return 1;
    case GTKML_S_FLOAT:
        fprintf(stream, "%f", expr->value.s_float.value);
        return 1;
    case GTKML_S_CHAR:
        fprintf(stream, "\\%c", expr->value.s_char.value);
        return 1;
    case GTKML_S_KEYWORD:
        fprintf(stream, ":%.*s", (int) expr->value.s_keyword.len, expr->value.s_keyword.ptr);
        return 1;
    case GTKML_S_SYMBOL:
        fprintf(stream, "'%.*s", (int) expr->value.s_symbol.len, expr->value.s_symbol.ptr);
        return 1;
    case GTKML_S_LIST:
        fprintf(stream, "(");
        while (expr->kind != GTKML_S_NIL) {
            gtk_ml_dumpf(ctx, stream, err, gtk_ml_car(expr));
            expr = gtk_ml_cdr(expr);
            if (expr->kind != GTKML_S_NIL) {
                fprintf(stream, " ");
            }
        }
        fprintf(stream, ")");
        return 1;
    case GTKML_S_MAP: {
        fprintf(stream, "{");
        struct DumpfData data = { ctx, stream, err, 0 };
        gtk_ml_hash_trie_foreach(&expr->value.s_map.map, dumpf_hash_trie, &data);
        fprintf(stream, "}");
        return 1;
    }
    case GTKML_S_SET: {
        fprintf(stream, "#{");
        struct DumpfData data = { ctx, stream, err, 0 };
        gtk_ml_hash_set_foreach(&expr->value.s_set.set, dumpf_hash_set, &data);
        fprintf(stream, "}");
        return 1;
    }
    case GTKML_S_ARRAY: {
        if (gtk_ml_array_trie_len(&expr->value.s_array.array) > 0 && gtk_ml_array_trie_is_string(&expr->value.s_array.array)) {
            fprintf(stream, "\"");
            gtk_ml_array_trie_foreach(&expr->value.s_array.array, dumpf_string, stream);
            fprintf(stream, "\"");
            return 1;
        } else {
            fprintf(stream, "[");
            struct DumpfData data = { ctx, stream, err, 0 };
            gtk_ml_array_trie_foreach(&expr->value.s_array.array, dumpf_array, &data);
            fprintf(stream, "]");
            return 1;
        }
    }
    case GTKML_S_VAR:
        fprintf(stream, "(var ");
        if (!gtk_ml_dumpf(ctx, stream, err, expr->value.s_vararg.expr)) {
            return 0;
        }
        fprintf(stream, ")");
        return 1;
    case GTKML_S_VARARG:
        fprintf(stream, "...");
        return gtk_ml_dumpf(ctx, stream, err, expr->value.s_vararg.expr);
    case GTKML_S_QUOTE:
        fprintf(stream, "'");
        return gtk_ml_dumpf(ctx, stream, err, expr->value.s_vararg.expr);
    case GTKML_S_QUASIQUOTE:
        fprintf(stream, "`");
        return gtk_ml_dumpf(ctx, stream, err, expr->value.s_vararg.expr);
    case GTKML_S_UNQUOTE:
        fprintf(stream, ",");
        return gtk_ml_dumpf(ctx, stream, err, expr->value.s_vararg.expr);
    case GTKML_S_LAMBDA: {
        fprintf(stream, "(lambda ");
        if (!gtk_ml_dumpf(ctx, stream, err, expr->value.s_lambda.args)) {
            return 0;
        }
        fprintf(stream, " ");
        GtkMl_S *body = expr->value.s_lambda.body;
        while (body->kind != GTKML_S_NIL) {
            if (!gtk_ml_dumpf(ctx, stream, err, gtk_ml_car(body))) {
                return 0;
            }
            body = gtk_ml_cdr(body);
            if (body->kind != GTKML_S_NIL) {
                fprintf(stream, " ");
            }
        }
        fprintf(stream, ")");
        return 1;
    }
    case GTKML_S_PROGRAM:
        switch (expr->value.s_program.kind) {
        case GTKML_PROG_INTRINSIC:
            fprintf(stream, "(program-intrinsic ");
            gtk_ml_dumpf(ctx, stream, err, expr->value.s_program.linkage_name);
            fprintf(stream, " 0x%"GTKML_FMT_64"x ", expr->value.s_program.addr);
            break;
        case GTKML_PROG_MACRO:
            fprintf(stream, "(program-macro ");
            gtk_ml_dumpf(ctx, stream, err, expr->value.s_program.linkage_name);
            fprintf(stream, " 0x%"GTKML_FMT_64"x ", expr->value.s_program.addr);
            break;
        case GTKML_PROG_RUNTIME:
            fprintf(stream, "(program ");
            gtk_ml_dumpf(ctx, stream, err, expr->value.s_program.linkage_name);
            fprintf(stream, " 0x%"GTKML_FMT_64"x ", expr->value.s_program.addr);
            break;
        }
        if (!gtk_ml_dumpf(ctx, stream, err, expr->value.s_program.args)) {
            return 0;
        }
        fprintf(stream, " ");
        GtkMl_S *body = expr->value.s_program.body;
        while (body->kind != GTKML_S_NIL) {
            if (!gtk_ml_dumpf(ctx, stream, err, gtk_ml_car(body))) {
                return 0;
            }
            body = gtk_ml_cdr(body);
            if (body->kind != GTKML_S_NIL) {
                fprintf(stream, " ");
            }
        }
        fprintf(stream, ")");
        return 1;
    case GTKML_S_ADDRESS:
        fprintf(stream, "(address ");
        gtk_ml_dumpf(ctx, stream, err, expr->value.s_address.linkage_name);
        fprintf(stream, " 0x%"GTKML_FMT_64"x)", expr->value.s_address.addr);
        return 1;
    case GTKML_S_MACRO: {
        fprintf(stream, "(macro ");
        gtk_ml_dumpf(ctx, stream, err, expr->value.s_macro.args);
        fprintf(stream, " ");
        GtkMl_S *body = expr->value.s_macro.body;
        while (body->kind != GTKML_S_NIL) {
            gtk_ml_dumpf(ctx, stream, err, gtk_ml_car(body));
            body = gtk_ml_cdr(body);
            if (body->kind != GTKML_S_NIL) {
                fprintf(stream, " ");
            }
        }
        fprintf(stream, ")");
        return 1;
    }
    case GTKML_S_LIGHTDATA:
        fprintf(stream, "%p", expr->value.s_lightdata.userdata);
        return 1;
    case GTKML_S_USERDATA:
        fprintf(stream, "%p", expr->value.s_lightdata.userdata);
        return 1;
    default:
        *err = gtk_ml_error(ctx, "invalid-sexpr", GTKML_ERR_INVALID_SEXPR, expr->span.ptr != NULL, expr->span.line, expr->span.col, 1, gtk_ml_new_keyword(ctx, NULL, 0, "kind", strlen("kind")), gtk_ml_new_int(ctx, NULL, expr->kind));
        return 0;
    }
}

char *gtk_ml_dumpsn(GtkMl_Context *ctx, char *ptr, size_t n, GtkMl_S **err, GtkMl_S *expr) {
    (void) ctx;
    (void) ptr;
    (void) n;
    (void) err;
    (void) expr;
    return NULL;
}

#define rprintf(buffer, fmt, ...) \
    do { \
        size_t cap = 1024; \
        char *_buf; \
        while (1) { \
            _buf = malloc(cap); \
            size_t n = snprintf(_buf, cap, fmt, __VA_ARGS__); \
            if (n == cap - 1) { \
                cap *= 2; \
                free(_buf); \
            } \
            break; \
        } \
        buffer = _buf; \
    } while (0)

#define rprint(buffer, fmt) \
    do { \
        size_t cap = 1024; \
        char *_buf; \
        while (1) { \
            _buf = malloc(cap); \
            size_t n = snprintf(_buf, cap, fmt); \
            if (n == cap - 1) { \
                cap *= 2; \
                free(_buf); \
            } \
            break; \
        } \
        buffer = _buf; \
    } while (0)

#define snrprintf_at(buffer, at, size, fmt, ...) \
    do { \
        char *buf; \
        rprintf(buf, fmt, __VA_ARGS__);\
        if (strlen(buf) >= size - at) { \
            size += strlen(buf) + 1; \
            buffer = realloc(buffer, size); \
        } \
        strcpy(buffer + at, buf); \
        at += strlen(buf); \
        free(buf); \
    } while (0)

#define snrprint_at(buffer, at, size, fmt) \
    do { \
        char *buf; \
        rprint(buf, fmt);\
        if (strlen(buf) >= size - at) { \
            size += strlen(buf) + 1; \
            buffer = realloc(buffer, size); \
        } \
        strcpy(buffer + at, buf); \
        at += strlen(buf); \
        free(buf); \
    } while (0)

struct DumpsnrData {
    GtkMl_Context *ctx;
    char *buffer;
    size_t *offset;
    size_t size;
    GtkMl_S **err;
    size_t n;
};

GTKML_PRIVATE GtkMl_VisitResult dumpsnr_hash_trie(GtkMl_HashTrie *ht, void *key, void *value, void *_data) {
    struct DumpsnrData *data = _data;

    gtk_ml_dumpsnr_internal(data->ctx, data->buffer, data->offset, data->size, data->err, key);
    snrprint_at(data->buffer, *data->offset, data->size, " ");
    gtk_ml_dumpsnr_internal(data->ctx, data->buffer, data->offset, data->size, data->err, value);
    ++data->n;
    if (data->n < gtk_ml_hash_trie_len(ht)) {
        snrprint_at(data->buffer, *data->offset, data->size, " ");
    }

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult dumpsnr_hash_set(GtkMl_HashSet *hs, void *key, void *_data) {
    struct DumpsnrData *data = _data;

    gtk_ml_dumpsnr_internal(data->ctx, data->buffer, data->offset, data->size, data->err, key);
    ++data->n;
    if (data->n < gtk_ml_hash_set_len(hs)) {
        snrprint_at(data->buffer, *data->offset, data->size, " ");
    }

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult dumpsnr_array(GtkMl_Array *array, size_t idx, GtkMl_S *key, void *_data) {
    (void) idx;

    struct DumpsnrData *data = _data;

    gtk_ml_dumpsnr_internal(data->ctx, data->buffer, data->offset, data->size, data->err, key);
    ++data->n;
    if (data->n < gtk_ml_array_trie_len(array)) {
        snrprint_at(data->buffer, *data->offset, data->size, " ");
    }

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult dumpsnr_string(GtkMl_Array *array, size_t idx, GtkMl_S *key, void *_data) {
    (void) array;
    (void) idx;

    struct DumpsnrData *data = _data;
    snrprintf_at(data->buffer, *data->offset, data->size, "%c", key->value.s_char.value);

    return GTKML_VISIT_RECURSE;
}

char *gtk_ml_dumpsnr_internal(GtkMl_Context *ctx, char *buffer, size_t *offset, size_t size, GtkMl_S **err, GtkMl_S *expr) {
    switch (expr->kind) {
    case GTKML_S_NIL:
        snrprint_at(buffer, *offset, size, "#nil");
        return buffer;
    case GTKML_S_TRUE:
        snrprint_at(buffer, *offset, size, "#t");
        return buffer;
    case GTKML_S_FALSE:
        snrprint_at(buffer, *offset, size, "#f");
        return buffer;
    case GTKML_S_INT:
        snrprintf_at(buffer, *offset, size, "%"GTKML_FMT_64"d", expr->value.s_int.value);
        return buffer;
    case GTKML_S_FLOAT:
        snrprintf_at(buffer, *offset, size, "%f", expr->value.s_float.value);
        return buffer;
    case GTKML_S_CHAR:
        snrprintf_at(buffer, *offset, size, "\\%c", expr->value.s_char.value);
        return buffer;
    case GTKML_S_KEYWORD:
        snrprintf_at(buffer, *offset, size, ":%.*s", (int) expr->value.s_keyword.len, expr->value.s_keyword.ptr);
        return buffer;
    case GTKML_S_SYMBOL:
        snrprintf_at(buffer, *offset, size, "'%.*s", (int) expr->value.s_symbol.len, expr->value.s_symbol.ptr);
        return buffer;
    case GTKML_S_LIST:
        snrprint_at(buffer, *offset, size, "(");
        while (expr->kind != GTKML_S_NIL) {
            gtk_ml_dumpsnr_internal(ctx, buffer, offset, size, err, gtk_ml_car(expr));
            expr = gtk_ml_cdr(expr);
            if (expr->kind != GTKML_S_NIL) {
                snrprint_at(buffer, *offset, size, " ");
            }
        }
        snrprint_at(buffer, *offset, size, ")");
        return buffer;
    case GTKML_S_MAP: {
        snrprint_at(buffer, *offset, size, "{");
        struct DumpsnrData data = { ctx, buffer, offset, size, err, 0 };
        gtk_ml_hash_trie_foreach(&expr->value.s_map.map, dumpsnr_hash_trie, &data);
        snrprint_at(buffer, *offset, size, "}");
        return buffer;
    }
    case GTKML_S_SET: {
        snrprint_at(buffer, *offset, size, "#{");
        struct DumpsnrData data = { ctx, buffer, offset, size, err, 0 };
        gtk_ml_hash_set_foreach(&expr->value.s_set.set, dumpsnr_hash_set, &data);
        snrprint_at(buffer, *offset, size, "}");
        return buffer;
    }
    case GTKML_S_ARRAY: {
        if (gtk_ml_array_trie_len(&expr->value.s_array.array) > 0 && gtk_ml_array_trie_is_string(&expr->value.s_array.array)) {
            snrprint_at(buffer, *offset, size, "\"");
            struct DumpsnrData data = { ctx, buffer, offset, size, err, 0 };
            gtk_ml_array_trie_foreach(&expr->value.s_array.array, dumpsnr_string, &data);
            snrprint_at(buffer, *offset, size, "\"");
            return buffer;
        } else {
            snrprint_at(buffer, *offset, size, "[");
            struct DumpsnrData data = { ctx, buffer, offset, size, err, 0 };
            gtk_ml_array_trie_foreach(&expr->value.s_array.array, dumpsnr_array, &data);
            snrprint_at(buffer, *offset, size, "]");
            return buffer;
        }
    }
    case GTKML_S_VAR:
        snrprint_at(buffer, *offset, size, "(var ");
        if (!gtk_ml_dumpsnr_internal(ctx, buffer, offset, size, err, expr->value.s_vararg.expr)) {
            return NULL;
        }
        snrprint_at(buffer, *offset, size, ")");
        return buffer;
    case GTKML_S_VARARG:
        snrprint_at(buffer, *offset, size, "...");
        return gtk_ml_dumpsnr_internal(ctx, buffer, offset, size, err, expr->value.s_vararg.expr);
    case GTKML_S_QUOTE:
        snrprint_at(buffer, *offset, size, "'");
        return gtk_ml_dumpsnr_internal(ctx, buffer, offset, size, err, expr->value.s_vararg.expr);
    case GTKML_S_QUASIQUOTE:
        snrprint_at(buffer, *offset, size, "`");
        return gtk_ml_dumpsnr_internal(ctx, buffer, offset, size, err, expr->value.s_vararg.expr);
    case GTKML_S_UNQUOTE:
        snrprint_at(buffer, *offset, size, ",");
        return gtk_ml_dumpsnr_internal(ctx, buffer, offset, size, err, expr->value.s_vararg.expr);
    case GTKML_S_LAMBDA: {
        snrprint_at(buffer, *offset, size, "(lambda ");
        if (!gtk_ml_dumpsnr_internal(ctx, buffer, offset, size, err, expr->value.s_lambda.args)) {
            return NULL;
        }
        snrprint_at(buffer, *offset, size, " ");
        GtkMl_S *body = expr->value.s_lambda.body;
        while (body->kind != GTKML_S_NIL) {
            if (!gtk_ml_dumpsnr_internal(ctx, buffer, offset, size, err, gtk_ml_car(body))) {
                return NULL;
            }
            body = gtk_ml_cdr(body);
            if (body->kind != GTKML_S_NIL) {
                snrprint_at(buffer, *offset, size, " ");
            }
        }
        snrprint_at(buffer, *offset, size, ")");
        return buffer;
    }
    case GTKML_S_PROGRAM:
        switch (expr->value.s_program.kind) {
        case GTKML_PROG_INTRINSIC:
            snrprint_at(buffer, *offset, size, "(program-intrinsic ");
            gtk_ml_dumpsnr_internal(ctx, buffer, offset, size, err, expr->value.s_program.linkage_name);
            snrprintf_at(buffer, *offset, size, " 0x%"GTKML_FMT_64"x ", expr->value.s_program.addr);
            break;
        case GTKML_PROG_MACRO:
            snrprint_at(buffer, *offset, size, "(program-macro ");
            gtk_ml_dumpsnr_internal(ctx, buffer, offset, size, err, expr->value.s_program.linkage_name);
            snrprintf_at(buffer, *offset, size, " 0x%"GTKML_FMT_64"x ", expr->value.s_program.addr);
            break;
        case GTKML_PROG_RUNTIME:
            snrprint_at(buffer, *offset, size, "(program ");
            gtk_ml_dumpsnr_internal(ctx, buffer, offset, size, err, expr->value.s_program.linkage_name);
            snrprintf_at(buffer, *offset, size, " 0x%"GTKML_FMT_64"x ", expr->value.s_program.addr);
            break;
        }
        if (!gtk_ml_dumpsnr_internal(ctx, buffer, offset, size, err, expr->value.s_program.args)) {
            return NULL;
        }
        snrprint_at(buffer, *offset, size, " ");
        GtkMl_S *body = expr->value.s_program.body;
        while (body->kind != GTKML_S_NIL) {
            if (!gtk_ml_dumpsnr_internal(ctx, buffer, offset, size, err, gtk_ml_car(body))) {
                return NULL;
            }
            body = gtk_ml_cdr(body);
            if (body->kind != GTKML_S_NIL) {
                snrprint_at(buffer, *offset, size, " ");
            }
        }
        snrprint_at(buffer, *offset, size, ")");
        return buffer;
    case GTKML_S_ADDRESS:
        snrprintf_at(buffer, *offset, size, "(address 0x%"GTKML_FMT_64"x)", expr->value.s_address.addr);
        return buffer;
    case GTKML_S_MACRO: {
        snrprint_at(buffer, *offset, size, "(macro ");
        gtk_ml_dumpsnr_internal(ctx, buffer, offset, size, err, expr->value.s_macro.args);
        snrprint_at(buffer, *offset, size, " ");
        GtkMl_S *body = expr->value.s_macro.body;
        while (body->kind != GTKML_S_NIL) {
            gtk_ml_dumpsnr_internal(ctx, buffer, offset, size, err, gtk_ml_car(body));
            body = gtk_ml_cdr(body);
            if (body->kind != GTKML_S_NIL) {
                snrprint_at(buffer, *offset, size, " ");
            }
        }
        snrprint_at(buffer, *offset, size, ")");
        return buffer;
    }
    case GTKML_S_LIGHTDATA:
        snrprintf_at(buffer, *offset, size, "%p", expr->value.s_lightdata.userdata);
        return buffer;
    case GTKML_S_USERDATA:
        snrprintf_at(buffer, *offset, size, "%p", expr->value.s_lightdata.userdata);
        return buffer;
    default:
        *err = gtk_ml_error(ctx, "invalid-sexpr", GTKML_ERR_INVALID_SEXPR, expr->span.ptr != NULL, expr->span.line, expr->span.col, 1, gtk_ml_new_keyword(ctx, NULL, 0, "kind", strlen("kind")), gtk_ml_new_int(ctx, NULL, expr->kind));
        return NULL;
    }
}

char *gtk_ml_dumpsnr(GtkMl_Context *ctx, char *buffer, size_t size, GtkMl_S **err, GtkMl_S *expr) {
    size_t offset = 0;
    return gtk_ml_dumpsnr_internal(ctx, buffer, &offset, size, err, expr);
}

GTKML_PRIVATE gboolean gtk_ml_dumpf_program_internal(GtkMl_Context *ctx, FILE *stream, GtkMl_S **err, gboolean line, gboolean debug) {
    if (!ctx->vm->program || !ctx->vm->program->exec) {
        return 0;
    }

    size_t start;
    size_t end;
    if (line) {
        start = ctx->vm->reg[GTKML_R_PC].pc >> 3;
        end = (ctx->vm->reg[GTKML_R_PC].pc >> 3) + 1;
    } else {
        start = 0;
        end = ctx->vm->program->n_exec;
        fprintf(stream, "section TEXT\n\n");
    }
    for (size_t pc = start; pc < end;) {
        GtkMl_Instruction instr = ctx->vm->program->exec[pc];
        if (debug) {
            char current_line = (pc == ctx->vm->reg[GTKML_R_PC].pc)? '*' : ' ';
            fprintf(stream, "%c %zx ", current_line, 8 * pc);
        } else {
            fprintf(stream, "%zx ", 8 * pc);
        }
        if (S_CATEGORY[instr.gen.category]) {
            fprintf(stream, "%s ", S_CATEGORY[instr.gen.category][instr.arith.opcode]);
            switch (instr.gen.category) {
            case GTKML_I_ARITH:
                fprintf(stream, "%u, %u, %u", instr.arith.rd, instr.arith.rs, instr.arith.ra);
                break;
            case GTKML_I_IMM:
            case GTKML_I_IMM | GTKML_I_IMM_EXTERN:
                fprintf(stream, "%u, %u, %u", instr.imm.rd, instr.imm.rs, instr.imm.imm);
                break;
            case GTKML_I_BR:
                fprintf(stream, "%"GTKML_FMT_64"u", instr.br.imm);
                break;
            case GTKML_EI_IMM:
            case GTKML_EI_IMM | GTKML_EI_IMM_EXTERN:
                fprintf(stream, "%u, %u, ", instr.imm.rd, instr.imm.rs);
                break;
            case GTKML_EI_BR:
                break;
            }
            if (instr.gen.category & GTKML_I_EXTENDED) {
                fprintf(stream, "%"GTKML_FMT_64"u", ctx->vm->program.exec[pc + 1].imm64);
            }
            fprintf(stream, "\n");
        } else if (instr.gen.category == GTKML_EI_EXPORT) {
            if (instr.imm.imm & GTKML_EI_EXPORT_FLAG_LOCAL) {
                fprintf(stream, "EXPORT LOCAL %"GTKML_FMT_64"u\n", ctx->vm->program.exec[pc + 1].imm64);
            } else {
                fprintf(stream, "EXPORT %"GTKML_FMT_64"u\n", ctx->vm->program.exec[pc + 1].imm64);
            }
        } else if (instr.gen.category & GTKML_I_EXTENDED) {
            fprintf(stream, "INVALID %"GTKML_FMT_64"x %"GTKML_FMT_64"u\n", instr.instr, ctx->vm->program.exec[pc + 1].imm64);
        } else {
            fprintf(stream, "INVALID %"GTKML_FMT_64"x\n", instr.instr);
        }

        if (instr.gen.category & GTKML_I_EXTENDED) {
            pc += 2;
        } else {
            ++pc;
        }
    }

    if (!line) {
        fprintf(stream, "\n");
        fprintf(stream, "section STATIC\n\n");
        for (size_t i = 1; i < ctx->vm->program->n_static; i++) {
            GtkMl_S *s = ctx->vm->program->statics[i];
            fprintf(stream, "%zu ", i);
            if (!gtk_ml_dumpf(ctx, stream, err, s)) {
                return 0;
            }
            fprintf(stream, "\n");
        }
    }

    return 1;
}

#ifdef GTKML_ENABLE_POSIX
GTKML_PRIVATE gboolean gtk_ml_dumpf_program_debug_internal(GtkMl_Context *ctx, FILE *stream, GtkMl_S **err, gboolean line, gboolean debug) {
    GtkMl_Vm *vm = gtk_ml_dbg_read_ptr(ctx, err, &ctx->dbg_ctx->vm);
    if (*err) {
        return 0;
    }
    GtkMl_Instruction *text = gtk_ml_dbg_read_ptr(ctx, err, &vm->program.exec);
    if (*err) {
        return 0;
    }
    GtkMl_S **stat = gtk_ml_dbg_read_ptr(ctx, err, &vm->program.statics);
    if (!text) {
        return 0;
    }

    uint64_t dbg_pc = gtk_ml_dbg_read_u64(ctx, err, &vm->reg[GTKML_R_PC].pc);
    if (*err) {
        return 0;
    }
    size_t start;
    size_t end;
    if (line) {
        start = dbg_pc >> 3;
        end = (dbg_pc >> 3) + 1;
    } else {
        start = 0;
        end = gtk_ml_dbg_read_u64(ctx, err, &vm->program.n_exec);
        if (*err) {
            return 0;
        }
        fprintf(stream, "section TEXT\n\n");
    }
    for (size_t pc = start; pc < end;) {
        uint64_t word = gtk_ml_dbg_read_u64(ctx, err, &text[pc]);
        if (*err) {
            return 0;
        }
        GtkMl_Instruction instr = *(GtkMl_Instruction *) &word;
        if (debug) {
            char current_line = (pc == dbg_pc)? '*' : ' ';
            fprintf(stream, "%c %zx ", current_line, 8 * pc);
        } else {
            fprintf(stream, "%zx ", 8 * pc);
        }
        if (S_CATEGORY[instr.gen.category]) {
            fprintf(stream, "%s ", S_CATEGORY[instr.gen.category][instr.arith.opcode]);
            switch (instr.gen.category) {
            case GTKML_I_ARITH:
                fprintf(stream, "%u, %u, %u", instr.arith.rd, instr.arith.rs, instr.arith.ra);
                break;
            case GTKML_I_IMM:
            case GTKML_I_IMM | GTKML_I_IMM_EXTERN:
                fprintf(stream, "%u, %u, %u", instr.imm.rd, instr.imm.rs, instr.imm.imm);
                break;
            case GTKML_I_BR:
                fprintf(stream, "%"GTKML_FMT_64"u", instr.br.imm);
                break;
            case GTKML_EI_IMM:
            case GTKML_EI_IMM | GTKML_EI_IMM_EXTERN:
                fprintf(stream, "%u, %u, ", instr.imm.rd, instr.imm.rs);
                break;
            case GTKML_EI_BR:
                break;
            }
            if (instr.gen.category & GTKML_I_EXTENDED) {
                uint64_t imm64 = gtk_ml_dbg_read_u64(ctx, err, &text[pc + 1]);
                if (*err) {
                    return 0;
                }
                fprintf(stream, "%"GTKML_FMT_64"u", imm64);
            }
            fprintf(stream, "\n");
        } else if (instr.gen.category == GTKML_EI_EXPORT) {
            if (instr.imm.imm & GTKML_EI_EXPORT_FLAG_LOCAL) {
                uint64_t imm64 = gtk_ml_dbg_read_u64(ctx, err, &text[pc + 1]);
                if (*err) {
                    return 0;
                }
                fprintf(stream, "EXPORT LOCAL %"GTKML_FMT_64"u\n", imm64);
            } else {
                uint64_t imm64 = gtk_ml_dbg_read_u64(ctx, err, &text[pc + 1]);
                if (*err) {
                    return 0;
                }
                fprintf(stream, "EXPORT %"GTKML_FMT_64"u\n", imm64);
            }
        } else if (instr.gen.category & GTKML_I_EXTENDED) {
            uint64_t imm64 = gtk_ml_dbg_read_u64(ctx, err, &text[pc + 1]);
            if (*err) {
                return 0;
            }
            fprintf(stream, "INVALID %"GTKML_FMT_64"x %"GTKML_FMT_64"u\n", instr.instr, imm64);
        } else {
            fprintf(stream, "INVALID %"GTKML_FMT_64"x\n", instr.instr);
        }

        if (instr.gen.category & GTKML_I_EXTENDED) {
            pc += 2;
        } else {
            ++pc;
        }
    }

    if (!line) {
        size_t start = 1;
        size_t end;
        end = gtk_ml_dbg_read_u64(ctx, err, &vm->program.n_static);
        if (*err) {
            return 0;
        }

        fprintf(stream, "\n");
        fprintf(stream, "section STATIC\n\n");
        for (size_t i = start; i < end; i++) {
            GtkMl_S *s = gtk_ml_dbg_read_ptr(ctx, err, &stat[i]);
            if (*err) {
                return 0;
            }
            fprintf(stream, "%zu ", i);
            if (!gtk_ml_dumpf_debug(ctx, stream, err, s)) {
                return 0;
            }
            fprintf(stream, "\n");
        }
    }

    return 1;
}

gboolean gtk_ml_dumpf_program_debug(GtkMl_Context *ctx, FILE *stream, GtkMl_S **err) {
    return gtk_ml_dumpf_program_debug_internal(ctx, stream, err, 0, 1);
}

gboolean gtk_ml_dumpf_line_debug(GtkMl_Context *ctx, FILE *stream, GtkMl_S **err) {
    return gtk_ml_dumpf_program_debug_internal(ctx, stream, err, 1, 1);
}

gboolean gtk_ml_dumpf_stack_debug(GtkMl_Context *ctx, FILE *stream, GtkMl_S **err) {
    GtkMl_Vm *vm = gtk_ml_dbg_read_ptr(ctx, err, &ctx->dbg_ctx->vm);
    if (*err) {
        return 0;
    }
    GtkMl_Register *stack = gtk_ml_dbg_read_ptr(ctx, err, &vm->stack);
    if (*err) {
        return 0;
    }
    size_t start = 0;
    size_t end = gtk_ml_dbg_read_u64(ctx, err, &vm->stack_len);
    if (*err) {
        return 0;
    }

    fprintf(stream, "vm stack\n\n");
    for (size_t i = start; i < end; i++) {
        GtkMl_S *s = gtk_ml_dbg_read_ptr(ctx, err, &stack[i].s_expr);
        if (*err) {
            return 0;
        }
        fprintf(stream, "%zu ", i);
        if (!gtk_ml_dumpf_debug(ctx, stream, err, s)) {
            return 0;
        }
        fprintf(stream, "\n");
    }

    return 1;
}

gboolean gtk_ml_backtracef_debug(GtkMl_Context *ctx, FILE *stream, GtkMl_S **err) {
    GtkMl_Vm *vm = gtk_ml_dbg_read_ptr(ctx, err, &ctx->dbg_ctx->vm);
    if (*err) {
        return 0;
    }
    GtkMl_Instruction *text = gtk_ml_dbg_read_ptr(ctx, err, &vm->program.exec);
    if (*err) {
        return 0;
    }
    GtkMl_S **statics = gtk_ml_dbg_read_ptr(ctx, err, &vm->program.statics);
    if (*err) {
        return 0;
    }
    fprintf(stream, "callstack backtrace\n\n");
    gboolean first = 1;
    fprintf(stream, "[");
    if (text) {
        size_t call_stack_ptr = gtk_ml_dbg_read_u64(ctx, err, &vm->call_stack_ptr);
        if (*err) {
            return 0;
        }
        uint64_t *call_stack = gtk_ml_dbg_read_ptr(ctx, err, &vm->call_stack);
        if (*err) {
            return 0;
        }
        for (size_t i = 0; i < call_stack_ptr; i += 2) {
            size_t call_at = gtk_ml_dbg_read_u64(ctx, err, &call_stack[i + 1]);
            if (*err) {
                return 0;
            }
            GtkMl_S *export = NULL;
            for (size_t ptr = 0; ptr <= call_at;) {
                uint64_t word = gtk_ml_dbg_read_u64(ctx, err, &text[ptr >> 3]);
                if (*err) {
                    return 0;
                }
                GtkMl_Instruction instr = *(GtkMl_Instruction *) &word;
                if (instr.gen.category == GTKML_EI_EXPORT) {
                    uint64_t imm64 = gtk_ml_dbg_read_u64(ctx, err, &text[(ptr >> 3) + 1]);
                    if (*err) {
                        return 0;
                    }
                    GtkMl_S *_export = gtk_ml_dbg_read_ptr(ctx, err, &statics[imm64]);
                    if (*err) {
                        return 0;
                    }
                    GtkMl_SKind kind = (GtkMl_SKind) gtk_ml_dbg_read_u32(ctx, err, &_export->kind);
                    if (kind == GTKML_S_PROGRAM || kind == GTKML_S_ADDRESS) {
                        export = _export;
                    }
                }
                if (instr.gen.category & GTKML_I_EXTENDED) {
                    ptr += 16;
                } else {
                    ptr += 8;
                }
            }

            if (export) {
                GtkMl_S export_value;
                export_value.next = NULL;
                gtk_ml_dbg_read_value(ctx, err, &export_value, export);
                GtkMl_S *export = &export_value;

                if (!first) {
                    fprintf(stream, " ");
                } else {
                    first = 0;
                }
                if (export->kind == GTKML_S_PROGRAM) {
                    gtk_ml_dumpf_debug(ctx, stream, err, export->value.s_program.linkage_name);
                    fprintf(stream, " 0x%08"GTKML_FMT_64"x 0x%08"GTKML_FMT_64"x\n", export->value.s_program.addr, call_at);
                } else if (export->kind == GTKML_S_ADDRESS) {
                    gtk_ml_dumpf_debug(ctx, stream, err, export->value.s_address.linkage_name);
                    fprintf(stream, " 0x%08"GTKML_FMT_64"x 0x%08"GTKML_FMT_64"x\n", export->value.s_address.addr, call_at);
                }
            } else {
                fprintf(stream, " #nil\n");
            }
        }

        size_t call_at = gtk_ml_dbg_read_u64(ctx, err, &vm->reg[GTKML_R_PC].pc);
        if (*err) {
            return 0;
        }
        GtkMl_S *export = NULL;
        for (size_t ptr = 0; ptr <= call_at;) {
            uint64_t word = gtk_ml_dbg_read_u64(ctx, err, &text[ptr >> 3]);
            if (*err) {
                return 0;
            }
            GtkMl_Instruction instr = *(GtkMl_Instruction *) &word;
            if (instr.gen.category == GTKML_EI_EXPORT) {
                uint64_t imm64 = gtk_ml_dbg_read_u64(ctx, err, &text[(ptr >> 3) + 1]);
                if (*err) {
                    return 0;
                }
                GtkMl_S *_export = gtk_ml_dbg_read_ptr(ctx, err, &statics[imm64]);
                if (*err) {
                    return 0;
                }
                GtkMl_SKind kind = (GtkMl_SKind) gtk_ml_dbg_read_u32(ctx, err, &_export->kind);
                if (kind == GTKML_S_PROGRAM || kind == GTKML_S_ADDRESS) {
                    export = _export;
                }
            }
            if (instr.gen.category & GTKML_I_EXTENDED) {
                ptr += 16;
            } else {
                ptr += 8;
            }
        }

        if (export) {
            GtkMl_S export_value;
            export_value.next = NULL;
            gtk_ml_dbg_read_value(ctx, err, &export_value, export);
            GtkMl_S *export = &export_value;

            if (!first) {
                fprintf(stream, " ");
            } else {
                first = 0;
            }
            if (export->kind == GTKML_S_PROGRAM) {
                gtk_ml_dumpf_debug(ctx, stream, err, export->value.s_program.linkage_name);
                fprintf(stream, " 0x%08"GTKML_FMT_64"x 0x%08"GTKML_FMT_64"x", export->value.s_program.addr, call_at);
            } else if (export->kind == GTKML_S_ADDRESS) {
                gtk_ml_dumpf_debug(ctx, stream, err, export->value.s_address.linkage_name);
                fprintf(stream, " 0x%08"GTKML_FMT_64"x 0x%08"GTKML_FMT_64"x", export->value.s_address.addr, call_at);
            }
        } else {
            fprintf(stream, " #nil");
        }
    }
    fprintf(stream, "]\n");
    return 1;
}
#endif /* GTKML_ENABLE_POSIX */

gboolean gtk_ml_dumpf_program(GtkMl_Context *ctx, FILE *stream, GtkMl_S **err) {
    return gtk_ml_dumpf_program_internal(ctx, stream, err, 0, 0);
}

char *gtk_ml_dumpsn_program(GtkMl_Context *ctx, char *ptr, size_t n, GtkMl_S **err) {
    (void) ctx;
    (void) ptr;
    (void) n;
    (void) err;
    return NULL;
}

char *gtk_ml_dumpsnr_program(GtkMl_Context *ctx, char *ptr, size_t n, GtkMl_S **err) {
    (void) ctx;
    (void) ptr;
    (void) n;
    (void) err;
    return NULL;
}

#ifdef GTKML_ENABLE_ASM
void gtk_ml_breakpoint(GtkMl_Context *ctx) {
    gtk_ml_breakpoint_internal(ctx, ctx->enable_breakpoint);
}

void gtk_ml_breakpoint_internal(GtkMl_Context *ctx, gboolean enable) {
    if (enable) {
        __asm__ volatile ( "int $3" : : "a" (ctx) :);
    }
}
#endif /* GTKML_ENABLE_ASM */

#ifdef GTKML_ENABLE_POSIX
uint64_t gtk_ml_dbg_read_memory(GtkMl_Context *ctx, GtkMl_S **err, const void *addr) {
    *err = NULL;
    errno = 0;
    uint64_t result = ptrace(PTRACE_PEEKDATA, ctx->dbg_process, addr, NULL);
    if (errno) {
        int errnum = errno;
        *err = gtk_ml_error(ctx, "debugger-error", strerror(errnum), 0, 0, 0, 1, gtk_ml_new_keyword(ctx, NULL, 0, "errno", strlen("errno")), gtk_ml_new_int(ctx, NULL, errnum));
    }
    return result;
}

void gtk_ml_dbg_write_memory(GtkMl_Context *ctx, GtkMl_S **err, void *addr, uint64_t value) {
    *err = NULL;
    errno = 0;
    ptrace(PTRACE_POKEDATA, ctx->dbg_process, addr, value);
    if (errno) {
        int errnum = errno;
        *err = gtk_ml_error(ctx, "debugger-error", strerror(errnum), 0, 0, 0, 1, gtk_ml_new_keyword(ctx, NULL, 0, "errno", strlen("errno")), gtk_ml_new_int(ctx, NULL, errnum));
    }
}

gboolean gtk_ml_dbg_read_boolean(GtkMl_Context *ctx, GtkMl_S **err, const void *addr) {
    return gtk_ml_dbg_read_memory(ctx, err, addr) & 0xffffffff;
}

void gtk_ml_dbg_write_boolean(GtkMl_Context *ctx, GtkMl_S **err, void *addr, gboolean value) {
    uint64_t word = gtk_ml_dbg_read_memory(ctx, err, addr);
    if (*err) {
        return;
    }
    word &= ~0xffffffff;
    word |= value;
    gtk_ml_dbg_write_memory(ctx, err, addr, word);
}

uint8_t gtk_ml_dbg_read_u8(GtkMl_Context *ctx, GtkMl_S **err, const void *addr) {
    return gtk_ml_dbg_read_memory(ctx, err, addr) & 0xff;
}

void gtk_ml_dbg_write_u8(GtkMl_Context *ctx, GtkMl_S **err, void *addr, uint8_t value) {
    uint64_t word = gtk_ml_dbg_read_memory(ctx, err, addr);
    if (*err) {
        return;
    }
    word &= ~0xff;
    word |= value;
    gtk_ml_dbg_write_memory(ctx, err, addr, word);
}

uint32_t gtk_ml_dbg_read_u32(GtkMl_Context *ctx, GtkMl_S **err, const void *addr) {
    return gtk_ml_dbg_read_memory(ctx, err, addr) & 0xffffffff;
}

void gtk_ml_dbg_write_u32(GtkMl_Context *ctx, GtkMl_S **err, void *addr, uint32_t value) {
    uint64_t word = gtk_ml_dbg_read_memory(ctx, err, addr);
    if (*err) {
        return;
    }
    word &= ~0xffffffff;
    word |= value;
    gtk_ml_dbg_write_memory(ctx, err, addr, word);
}

uint64_t gtk_ml_dbg_read_u64(GtkMl_Context *ctx, GtkMl_S **err, const void *addr) {
    return gtk_ml_dbg_read_memory(ctx, err, addr);
}

void gtk_ml_dbg_write_u64(GtkMl_Context *ctx, GtkMl_S **err, void *addr, uint64_t value) {
    gtk_ml_dbg_write_memory(ctx, err, addr, value);
}

void *gtk_ml_dbg_read_ptr(GtkMl_Context *ctx, GtkMl_S **err, const void *addr) {
    return (void *) gtk_ml_dbg_read_memory(ctx, err, addr);
}

void gtk_ml_dbg_write_ptr(GtkMl_Context *ctx, GtkMl_S **err, void *addr, void *value) {
    gtk_ml_dbg_write_memory(ctx, err, addr, (uint64_t) value);
}

void gtk_ml_dbg_read_value(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *out, const GtkMl_S *addr) {
    GtkMl_S *next = out->next;
    void *ptr = out;
    for (size_t i = 0; i < sizeof(GtkMl_S); i += 8) {
        *((uint64_t *) ((uint8_t *) ptr + i)) = gtk_ml_dbg_read_u64(ctx, err, (uint8_t *) addr + i);
        if (*err) {
            return;
        }
    }
    out->next = next;
}

void gtk_ml_dbg_write_value(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *addr, GtkMl_S *value) {
    // read the original value at address
    GtkMl_S at_addr;
    gtk_ml_dbg_read_value(ctx, err, &at_addr, addr);
    if (*err) {
        return;
    }

    void *ptr = value;
    for (size_t i = 0; i < sizeof(GtkMl_S); i += 8) {
        gtk_ml_dbg_write_u64(ctx, err, (uint8_t *) addr + i, *((uint64_t *) ((uint8_t *) ptr + i)));
        if (*err) {
            return;
        }
    }

    // write the original `next` pointer
    gtk_ml_dbg_write_ptr(ctx, err, addr, at_addr.next);
}
#endif /* GTKML_ENABLE_POSIX */
