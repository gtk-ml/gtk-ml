#include <stdlib.h>
#include <ctype.h>
#include <gtk/gtk.h>
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
    GtkMl_Context *ctx = malloc(sizeof(GtkMl_Context));
    ctx->gc_enabled = 1;
    ctx->n_values = 0;
    ctx->m_values = GTKML_GC_COUNT_THRESHOLD;
    ctx->first = NULL;
    ctx->vm = gtk_ml_new_vm(ctx);

    // ({'flags-none G_APPLICATION_FLAGS_NONE})
    ctx->bindings = gtk_ml_new_list(ctx, NULL, gtk_ml_new_map(ctx, NULL, NULL), gtk_ml_new_nil(ctx, NULL));
    ctx->top_scope = &gtk_ml_car(ctx->bindings);

    gtk_ml_define(ctx, gtk_ml_new_symbol(ctx, NULL, 0, "flags-none", 10), gtk_ml_new_int(ctx, NULL, G_APPLICATION_FLAGS_NONE));
    gtk_ml_define(ctx, gtk_ml_new_symbol(ctx, NULL, 0, "cond-none", 9), gtk_ml_new_int(ctx, NULL, GTKML_F_NONE));

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

void gtk_ml_del_context(GtkMl_Context *ctx) {
    gtk_ml_del_vm(ctx->vm);

    GtkMl_S *value = ctx->first;
    while (value) {
        GtkMl_S *next = value->next;
        gtk_ml_del(ctx, value);
        value = next;
    }

    free(ctx->parser.readers);

    free(ctx);
}

void gtk_ml_load_program(GtkMl_Context *ctx, GtkMl_Program* program) {
    char *start = NULL;
    if (program->start) {
        start = malloc(strlen(program->start) + 1);
        strcpy(start, program->start);
    }
    ctx->vm->program.start = start;

    ctx->vm->program.exec = malloc(sizeof(GtkMl_Instruction) * program->n_exec);
    ctx->vm->program.n_exec = program->n_exec;
    memcpy(ctx->vm->program.exec, program->exec, sizeof(GtkMl_Instruction) * program->n_exec);

    ctx->vm->program.statics = malloc(sizeof(GtkMl_S *) * program->n_static);
    ctx->vm->program.n_static = program->n_static;
    memcpy(ctx->vm->program.statics, program->statics, sizeof(GtkMl_S *) * program->n_static);
}

gboolean gtk_ml_run_program(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *program, GtkMl_S *args) {
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
    gboolean result = gtk_ml_vm_run(ctx->vm, err);

    gtk_ml_leave(ctx);

    return result;
}

GtkMl_S *gtk_ml_get_export(GtkMl_Context *ctx, GtkMl_S **err, const char *linkage_name) {
    GtkMl_S *ext = gtk_ml_new_string(ctx, NULL, linkage_name, strlen(linkage_name));
    for (size_t i = 0; i < ctx->vm->program.n_exec;) {
        GtkMl_Instruction instr = ctx->vm->program.exec[i];
        if (instr.gen.category == GTKML_EI_EXPORT) {
            GtkMl_S *program = ctx->vm->program.statics[ctx->vm->program.exec[i + 1].imm64];
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
}

GTKML_PRIVATE GtkMl_VisitResult array_to_c_str(GtkMl_Array *array, size_t idx, GtkMl_S *value, void *data) {
    size_t len = gtk_ml_array_trie_len(array);
    char *c_str = data;
    // for some reason we have to reverse it?
    c_str[len - idx - 1] = value->value.s_char.value;
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
    jenkins_update(hash, &ptr, sizeof(void *));
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
    case GTKML_S_ADDRESS:
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
    case GTKML_S_PROGRAM:
        mark_value(s->value.s_program.args);
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
    for (GtkMl_Static i = 0; i < program->n_static;) {
        mark_value(program->statics[i]);
    }
}

GTKML_PRIVATE void mark(GtkMl_Context *ctx) {
    for (size_t sp = 0; sp < ctx->vm->reg[GTKML_R_SP].sp; sp++) {
        mark_value(ctx->vm->stack[sp].s_expr);
    }
    mark_value(ctx->bindings);
    mark_program(&ctx->vm->program);
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
    free(s);
    --ctx->n_values;
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
    free(s);
    --ctx->n_values;
}

GTKML_PRIVATE void sweep(GtkMl_Context *ctx) {
    GtkMl_S **value = &ctx->first;
    while (*value) {
        if ((*value)->flags & GTKML_FLAG_REACHABLE) {
            (*value)->flags &= ~GTKML_FLAG_REACHABLE;
            value = &(*value)->next;
        } else {
            GtkMl_S *unreachable = *value;
            *value = (*value)->next;
            gtk_ml_delete(ctx, unreachable);
        }
    }
}

// simple mark & sweep gc
gboolean gtk_ml_collect(GtkMl_Context *ctx) {
    if (!ctx->gc_enabled) {
        return 0;
    }

    if (ctx->n_values < ctx->m_values) {
        return 0;
    }

    size_t n_values = ctx->n_values;
    mark(ctx);
    sweep(ctx);
    ctx->m_values = 2 * n_values;

    return 1;
}

gboolean gtk_ml_disable_gc(GtkMl_Context *ctx) {
    gboolean enabled = ctx->gc_enabled;
    ctx->gc_enabled = 0;
    return enabled;
}

void gtk_ml_enable_gc(GtkMl_Context *ctx, gboolean enabled) {
    ctx->gc_enabled = enabled;
}

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
        fprintf(stream, "%ld", expr->value.s_int.value);
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
        fprintf(stream, "[");
        struct DumpfData data = { ctx, stream, err, 0 }; 
        gtk_ml_array_trie_foreach(&expr->value.s_array.array, dumpf_array, &data);
        fprintf(stream, "]");
        return 1;
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
            fprintf(stream, " 0x%lx ", expr->value.s_program.addr);
            break;
        case GTKML_PROG_MACRO:
            fprintf(stream, "(program-macro ");
            gtk_ml_dumpf(ctx, stream, err, expr->value.s_program.linkage_name);
            fprintf(stream, " 0x%lx ", expr->value.s_program.addr);
            break;
        case GTKML_PROG_RUNTIME:
            fprintf(stream, "(program ");
            gtk_ml_dumpf(ctx, stream, err, expr->value.s_program.linkage_name);
            fprintf(stream, " 0x%lx ", expr->value.s_program.addr);
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
        fprintf(stream, "(address 0x%lx)", expr->value.s_address.addr);
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
        *err = gtk_ml_error(ctx, "invalid-sexpr", GTKML_ERR_INVALID_SEXPR, expr->span.ptr != NULL, expr->span.line, expr->span.col, 0);
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

char *gtk_ml_dumpsnr(GtkMl_Context *ctx, char *ptr, size_t n, GtkMl_S **err, GtkMl_S *expr) {
    (void) ctx;
    (void) ptr;
    (void) n;
    (void) err;
    (void) expr;
    return NULL;
}

gboolean gtk_ml_dumpf_program(GtkMl_Context *ctx, FILE *stream, GtkMl_S **err) {
    if (!ctx->vm->program.exec) {
        return 0;
    }

    fprintf(stream, "section TEXT\n\n");
    for (size_t pc = 0; pc < ctx->vm->program.n_exec;) {
        GtkMl_Instruction instr = ctx->vm->program.exec[pc];
        fprintf(stream, "%zx ", 8 * pc);
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
                fprintf(stream, "%lu", instr.br.imm);
                break;
            case GTKML_EI_IMM:
            case GTKML_EI_IMM | GTKML_EI_IMM_EXTERN:
                fprintf(stream, "%u, %u, ", instr.imm.rd, instr.imm.rs);
                break;
            case GTKML_EI_BR:
                break;
            }
            if (instr.gen.category & GTKML_I_EXTENDED) {
                fprintf(stream, "%lu", ctx->vm->program.exec[pc + 1].imm64);
            }
            fprintf(stream, "\n");
        } else if (instr.gen.category == GTKML_EI_EXPORT) {
            if (instr.imm.imm & GTKML_EI_EXPORT_FLAG_LOCAL) {
                fprintf(stream, "EXPORT LOCAL %lu\n", ctx->vm->program.exec[pc + 1].imm64);
            } else {
                fprintf(stream, "EXPORT %lu\n", ctx->vm->program.exec[pc + 1].imm64);
            }
        } else if (instr.gen.category & GTKML_I_EXTENDED) {
            fprintf(stream, "INVALID %lx %lu\n", instr.instr, ctx->vm->program.exec[pc + 1].imm64);
        } else {
            fprintf(stream, "INVALID %lx\n", instr.instr);
        }
        
        if (instr.gen.category & GTKML_I_EXTENDED) {
            pc += 2;
        } else {
            ++pc;
        }
    }

    fprintf(stream, "\n");
    fprintf(stream, "section STATIC\n\n");
    for (size_t i = 1; i < ctx->vm->program.n_static; i++) {
        GtkMl_S *s = ctx->vm->program.statics[i];
        fprintf(stream, "%zu ", i);
        if (!gtk_ml_dumpf(ctx, stream, err, s)) {
            return 0;
        }
        fprintf(stream, "\n");
    }

    return 1;
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
