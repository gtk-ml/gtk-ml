#include <stdlib.h>
#include <gtk/gtk.h>
#include <math.h>
#define GTKML_INCLUDE_INTERNAL
#include "gtk-ml.h"
#include "gtk-ml-internal.h"

void gtk_ml_new_serializer(GtkMl_Serializer *serf) {
    gtk_ml_new_hash_trie(&serf->ptr_map, &GTKML_PTR_HASHER);
}

void gtk_ml_new_deserializer(GtkMl_Deserializer *deserf) {
    gtk_ml_new_hash_trie(&deserf->ptr_map, &GTKML_PTR_HASHER);
}

struct SerfData {
    GtkMl_Context *ctx;
    GtkMl_Serializer *serf;
    FILE *stream;
    GtkMl_S **err;
    size_t n;
    gboolean result;
};

GTKML_PRIVATE GtkMl_VisitResult serf_hash_trie(GtkMl_HashTrie *ht, void *key, void *value, void *_data) {
    struct SerfData *data = _data;
    if (!gtk_ml_serf_value(data->serf, data->ctx, data->stream, data->err, key)) {
        data->result = 0;
        return GTKML_VISIT_BREAK;
    }
    fprintf(data->stream, ":");
    if (!gtk_ml_serf_value(data->serf, data->ctx, data->stream, data->err, value)) {
        data->result = 0;
        return GTKML_VISIT_BREAK;
    }
    ++data->n;
    if (data->n < gtk_ml_hash_trie_len(ht)) {
        fprintf(data->stream, ",");
    }

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult serf_hash_set(GtkMl_HashSet *hs, void *key, void *_data) {
    struct SerfData *data = _data;
    if (!gtk_ml_serf_value(data->serf, data->ctx, data->stream, data->err, key)) {
        data->result = 0;
        return GTKML_VISIT_BREAK;
    }
    ++data->n;
    if (data->n < gtk_ml_hash_set_len(hs)) {
        fprintf(data->stream, ",");
    }

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult serf_array(GtkMl_Array *array, size_t idx, GtkMl_S *value, void *_data) {
    (void) idx;
    struct SerfData *data = _data;
    if (!gtk_ml_serf_value(data->serf, data->ctx, data->stream, data->err, value)) {
        data->result = 0;
        return GTKML_VISIT_BREAK;
    }
    ++data->n;
    if (data->n < gtk_ml_array_trie_len(array)) {
        fprintf(data->stream, ",");
    }

    return GTKML_VISIT_RECURSE;
}

gboolean gtk_ml_serf_value(GtkMl_Serializer *serf, GtkMl_Context *ctx, FILE *stream, GtkMl_S **err, GtkMl_S *value) {
    size_t offset = (size_t) gtk_ml_hash_trie_get(&serf->ptr_map, value);
    
    if (offset) {
        fprintf(stream, "GTKML-R(");
        fwrite(&offset, sizeof(offset), 1, stream);
        fprintf(stream, ")");
        return 1;
    }

    offset = ftell(stream);
    GtkMl_HashTrie ptr_map;
    gtk_ml_new_hash_trie(&ptr_map, &GTKML_PTR_HASHER);
    gtk_ml_hash_trie_insert(&ptr_map, &serf->ptr_map, value, (void *) offset); // converting a size_t to void * is a hack, but it works
    serf->ptr_map = ptr_map;

    fprintf(stream, "GTKML-V(");
    uint32_t kind = value->kind;
    fwrite(&kind, sizeof(uint32_t), 1, stream);

    gboolean result = 1;
    switch (value->kind) {
    case GTKML_S_NIL:
    case GTKML_S_TRUE:
    case GTKML_S_FALSE:
        break;
    case GTKML_S_INT:
        fwrite(&value->value.s_int.value, sizeof(int64_t), 1, stream);
        break;
    case GTKML_S_FLOAT:
        fwrite(&value->value.s_float.value, sizeof(double), 1, stream);
        break;
    case GTKML_S_CHAR:
        fwrite(&value->value.s_char.value, sizeof(uint32_t), 1, stream);
        break;
    case GTKML_S_KEYWORD: {
        uint64_t len = value->value.s_keyword.len;
        fwrite(&len, sizeof(uint64_t), 1, stream);
        fwrite(value->value.s_keyword.ptr, 1, len, stream);
        break;
    }
    case GTKML_S_SYMBOL: {
        uint64_t len = value->value.s_symbol.len;
        fwrite(&len, sizeof(uint64_t), 1, stream);
        fwrite(value->value.s_symbol.ptr, 1, len, stream);
        break;
    }
    case GTKML_S_PROGRAM: {
        if (!gtk_ml_serf_value(serf, ctx, stream, err, value->value.s_program.linkage_name)) {
            return 0;
        }
        fwrite(&value->value.s_program.addr, sizeof(uint64_t), 1, stream);
        if (!gtk_ml_serf_value(serf, ctx, stream, err, value->value.s_program.args)) {
            return 0;
        }
        if (!gtk_ml_serf_value(serf, ctx, stream, err, value->value.s_program.body)) {
            return 0;
        }
        if (!gtk_ml_serf_value(serf, ctx, stream, err, value->value.s_program.capture)) {
            return 0;
        }
        break;
    }
    case GTKML_S_ADDRESS:
        fwrite(&value->value.s_address.addr, sizeof(uint64_t), 1, stream);
        break;
    case GTKML_S_LIGHTDATA:
    case GTKML_S_USERDATA:
        *err = gtk_ml_error(ctx, "ser-error", GTKML_ERR_SER_ERROR, value->span.ptr != NULL, value->span.line, value->span.col, 0);
        return 0;
    case GTKML_S_LAMBDA:
        if (!gtk_ml_serf_value(serf, ctx, stream, err, value->value.s_lambda.args)) {
            return 0;
        }
        if (!gtk_ml_serf_value(serf, ctx, stream, err, value->value.s_lambda.body)) {
            return 0;
        }
        if (!gtk_ml_serf_value(serf, ctx, stream, err, value->value.s_lambda.capture)) {
            return 0;
        }
        break;
    case GTKML_S_MACRO:
        if (!gtk_ml_serf_value(serf, ctx, stream, err, value->value.s_macro.args)) {
            return 0;
        }
        if (!gtk_ml_serf_value(serf, ctx, stream, err, value->value.s_macro.body)) {
            return 0;
        }
        if (!gtk_ml_serf_value(serf, ctx, stream, err, value->value.s_macro.capture)) {
            return 0;
        }
        break;
    case GTKML_S_MAP: {
        struct SerfData data = { ctx, serf, stream, err, 0, 1 }; 
        gtk_ml_hash_trie_foreach(&value->value.s_map.map, serf_hash_trie, &data);
        result = data.result;
    } break;
    case GTKML_S_SET: {
        struct SerfData data = { ctx, serf, stream, err, 0, 1 }; 
        gtk_ml_hash_set_foreach(&value->value.s_set.set, serf_hash_set, &data);
        result = data.result;
    } break;
    case GTKML_S_ARRAY: {
        struct SerfData data = { ctx, serf, stream, err, 0, 1 }; 
        gtk_ml_array_trie_foreach(&value->value.s_array.array, serf_array, &data);
        result = data.result;
    } break;
    case GTKML_S_VAR:
        if (!gtk_ml_serf_value(serf, ctx, stream, err, value->value.s_var.expr)) {
            return 0;
        }
        break;
    case GTKML_S_VARARG:
        if (!gtk_ml_serf_value(serf, ctx, stream, err, value->value.s_vararg.expr)) {
            return 0;
        }
        break;
    case GTKML_S_QUOTE:
        if (!gtk_ml_serf_value(serf, ctx, stream, err, value->value.s_quote.expr)) {
            return 0;
        }
        break;
    case GTKML_S_QUASIQUOTE:
        if (!gtk_ml_serf_value(serf, ctx, stream, err, value->value.s_quasiquote.expr)) {
            return 0;
        }
        break;
    case GTKML_S_UNQUOTE:
        if (!gtk_ml_serf_value(serf, ctx, stream, err, value->value.s_unquote.expr)) {
            return 0;
        }
        break;
    case GTKML_S_LIST:
        while (value->kind != GTKML_S_NIL) {
            if (!gtk_ml_serf_value(serf, ctx, stream, err, gtk_ml_car(value))) {
                return 0;
            }
            value = gtk_ml_cdr(value);
            if (value->kind != GTKML_S_NIL) {
                fprintf(stream, ",");
            }
        }
        break;
    }
    fprintf(stream, ")");
    return result;
}

GtkMl_S *gtk_ml_deserf_value(GtkMl_Deserializer *deserf, GtkMl_Context *ctx, FILE *stream, GtkMl_S **err) {
    size_t offset = ftell(stream);

    GtkMl_S *ref = gtk_ml_hash_trie_get(&deserf->ptr_map, (void *) offset);
    
    if (ref) {
        return ref;
    }

    char *gtkml_v = malloc(strlen("GTKML-V(") + 1);
    fread(gtkml_v, 1, strlen("GTKML-V("), stream);
    gtkml_v[strlen("GTKML-V(")] = 0;

    if (strcmp(gtkml_v, "GTKML-R(") == 0) {
        free(gtkml_v);

        size_t offset;
        fread(&offset, sizeof(size_t), 1, stream);

        char *end = malloc(2);
        fread(end, 1, 1, stream);
        end[1] = 0;
        if (strcmp(end, ")") != 0) {
            free(end);
            *err = gtk_ml_error(ctx, "deser-error", GTKML_ERR_DESER_ERROR, 0, 0, 0, 0);
            return NULL;
        }
        free(end);

        size_t cont_at = ftell(stream);

        fseek(stream, offset, SEEK_SET);
        GtkMl_S *result = gtk_ml_deserf_value(deserf, ctx, stream, err);
        fseek(stream, cont_at, SEEK_SET);

        return result;
    }

    if (strcmp(gtkml_v, "GTKML-V(") != 0) {
        free(gtkml_v);
        *err = gtk_ml_error(ctx, "deser-error", GTKML_ERR_DESER_ERROR, 0, 0, 0, 0);
        return NULL;
    }
    free(gtkml_v);

    uint32_t kind;
    fread(&kind, sizeof(uint32_t), 1, stream);

    GtkMl_S *result = gtk_ml_new_value(ctx, NULL, kind);

    switch (result->kind) {
    case GTKML_S_NIL:
    case GTKML_S_FALSE:
    case GTKML_S_TRUE:
        break;
    case GTKML_S_INT:
        fread(&result->value.s_int.value, sizeof(int64_t), 1, stream);
        break;
    case GTKML_S_FLOAT:
        fread(&result->value.s_float.value, sizeof(double), 1, stream);
        break;
    case GTKML_S_CHAR:
        fread(&result->value.s_char.value, sizeof(uint32_t), 1, stream);
        break;
    case GTKML_S_SYMBOL: {
        uint64_t len;
        fread(&len, sizeof(uint64_t), 1, stream);
        char *ptr = malloc(len);
        fread(ptr, 1, len, stream);
        result->value.s_symbol.owned = 1; 
        result->value.s_symbol.ptr = ptr;
        result->value.s_symbol.len = len;
        break;
    }
    case GTKML_S_KEYWORD: {
        uint64_t len;
        fread(&len, sizeof(uint64_t), 1, stream);
        char *ptr = malloc(len);
        fread(ptr, 1, len, stream);
        result->value.s_keyword.owned = 1; 
        result->value.s_keyword.ptr = ptr;
        result->value.s_keyword.len = len;
        break;
    }
    case GTKML_S_LIST: {
        char next = 0;
        fread(&next, 1, 1, stream);
        if (next != ')') {
            fseek(stream, -1, SEEK_CUR);
        }
        GtkMl_S **tail = &result;
        while (next != ')') {
            GtkMl_S *value = gtk_ml_deserf_value(deserf, ctx, stream, err);
            if (!value) {
                return NULL;
            }
            *tail = gtk_ml_new_list(ctx, NULL, value, gtk_ml_new_nil(ctx, NULL));
            tail = &gtk_ml_cdr(*tail);
            fread(&next, 1, 1, stream);
        }
        fseek(stream, -1, SEEK_CUR);
        break;
    }
    case GTKML_S_MAP: {
        char next = 0;
        fread(&next, 1, 1, stream);
        if (next != ')') {
            fseek(stream, -1, SEEK_CUR);
        }
        gtk_ml_new_hash_trie(&result->value.s_map.map, &GTKML_DEFAULT_HASHER);
        while (next != ')') {
            GtkMl_S *key = gtk_ml_deserf_value(deserf, ctx, stream, err);
            if (!key) {
                return NULL;
            }
            char colon;
            fread(&colon, 1, 1, stream);
            if (colon != ':') {
                *err = gtk_ml_error(ctx, "deser-error", GTKML_ERR_DESER_ERROR, 0, 0, 0, 0);
                return NULL;
            }
            GtkMl_S *value = gtk_ml_deserf_value(deserf, ctx, stream, err);
            if (!value) {
                return NULL;
            }
            GtkMl_S *new = gtk_ml_new_map(ctx, NULL, NULL);
            gtk_ml_hash_trie_insert(&new->value.s_map.map, &result->value.s_map.map, key, value);
            result = new;
            fread(&next, 1, 1, stream);
        }
        fseek(stream, -1, SEEK_CUR);
        break;
    }
    case GTKML_S_SET: {
        char next = 0;
        fread(&next, 1, 1, stream);
        if (next != ')') {
            fseek(stream, -1, SEEK_CUR);
        }
        gtk_ml_new_hash_set(&result->value.s_set.set, &GTKML_DEFAULT_HASHER);
        while (next != ')') {
            GtkMl_S *key = gtk_ml_deserf_value(deserf, ctx, stream, err);
            if (!key) {
                return NULL;
            }
            GtkMl_S *new = gtk_ml_new_set(ctx, NULL);
            gtk_ml_hash_set_insert(&new->value.s_set.set, &result->value.s_set.set, key);
            result = new;
            fread(&next, 1, 1, stream);
        }
        fseek(stream, -1, SEEK_CUR);
        break;
    }
    case GTKML_S_ARRAY: {
        char next = 0;
        fread(&next, 1, 1, stream);
        if (next != ')') {
            fseek(stream, -1, SEEK_CUR);
        }
        gtk_ml_new_array_trie(&result->value.s_array.array);
        while (next != ')') {
            GtkMl_S *value = gtk_ml_deserf_value(deserf, ctx, stream, err);
            if (!value) {
                return NULL;
            }
            GtkMl_S *new = gtk_ml_new_array(ctx, NULL);
            gtk_ml_array_trie_push(&new->value.s_array.array, &result->value.s_array.array, value);
            result = new;
            fread(&next, 1, 1, stream);
        }
        fseek(stream, -1, SEEK_CUR);
        break;
    }
    case GTKML_S_VAR: {
        GtkMl_S *expr = gtk_ml_deserf_value(deserf, ctx, stream, err);
        if (!expr) {
            return NULL;
        }
        result->value.s_var.expr = expr;
    } break;
    case GTKML_S_VARARG: {
        GtkMl_S *expr = gtk_ml_deserf_value(deserf, ctx, stream, err);
        if (!expr) {
            return NULL;
        }
        result->value.s_vararg.expr = expr;
    } break;
    case GTKML_S_QUOTE: {
        GtkMl_S *expr = gtk_ml_deserf_value(deserf, ctx, stream, err);
        if (!expr) {
            return NULL;
        }
        result->value.s_quote.expr = expr;
    } break;
    case GTKML_S_QUASIQUOTE: {
        GtkMl_S *expr = gtk_ml_deserf_value(deserf, ctx, stream, err);
        if (!expr) {
            return NULL;
        }
        result->value.s_quasiquote.expr = expr;
    } break;
    case GTKML_S_UNQUOTE: {
        GtkMl_S *expr = gtk_ml_deserf_value(deserf, ctx, stream, err);
        if (!expr) {
            return NULL;
        }
        result->value.s_unquote.expr = expr;
    } break;
    case GTKML_S_LAMBDA: {
        GtkMl_S *args = gtk_ml_deserf_value(deserf, ctx, stream, err);
        if (!args) {
            return NULL;
        }
        GtkMl_S *body = gtk_ml_deserf_value(deserf, ctx, stream, err);
        if (!body) {
            return NULL;
        }
        GtkMl_S *capture = gtk_ml_deserf_value(deserf, ctx, stream, err);
        if (!capture) {
            return NULL;
        }
        result->value.s_lambda.args = args;
        result->value.s_lambda.body = body;
        result->value.s_lambda.capture = capture;
        break;
    }
    case GTKML_S_PROGRAM: {
        GtkMl_S *linkage_name = gtk_ml_deserf_value(deserf, ctx, stream, err);
        if (!linkage_name) {
            return NULL;
        }
        result->value.s_program.linkage_name = linkage_name;
        uint64_t addr;
        fread(&addr, sizeof(uint64_t), 1, stream);
        result->value.s_program.addr = addr;
        GtkMl_S *args = gtk_ml_deserf_value(deserf, ctx, stream, err);
        if (!args) {
            return NULL;
        }
        GtkMl_S *body = gtk_ml_deserf_value(deserf, ctx, stream, err);
        if (!body) {
            return NULL;
        }
        GtkMl_S *capture = gtk_ml_deserf_value(deserf, ctx, stream, err);
        if (!capture) {
            return NULL;
        }
        result->value.s_program.args = args;
        result->value.s_program.body = body;
        result->value.s_program.capture = capture;
        break;
    }
    case GTKML_S_ADDRESS: {
        uint64_t addr;
        fread(&addr, sizeof(uint64_t), 1, stream);
        result->value.s_address.addr = addr;
        break;
    }
    case GTKML_S_MACRO: {
        GtkMl_S *args = gtk_ml_deserf_value(deserf, ctx, stream, err);
        if (!args) {
            return NULL;
        }
        GtkMl_S *body = gtk_ml_deserf_value(deserf, ctx, stream, err);
        if (!body) {
            return NULL;
        }
        GtkMl_S *capture = gtk_ml_deserf_value(deserf, ctx, stream, err);
        if (!capture) {
            return NULL;
        }
        result->value.s_macro.args = args;
        result->value.s_macro.body = body;
        result->value.s_macro.capture = capture;
        break;
    }
    case GTKML_S_LIGHTDATA:
    case GTKML_S_USERDATA:
        *err = gtk_ml_error(ctx, "deser-error", GTKML_ERR_DESER_ERROR, 0, 0, 0, 0);
        return 0;
    }

    char *end = malloc(2);
    fread(end, 1, 1, stream);
    end[1] = 0;
    if (strcmp(end, ")") != 0) {
        free(end);
        *err = gtk_ml_error(ctx, "deser-error", GTKML_ERR_DESER_ERROR, 0, 0, 0, 0);
        return NULL;
    }
    free(end);

    GtkMl_HashTrie ptr_map;
    gtk_ml_new_hash_trie(&ptr_map, &GTKML_PTR_HASHER);
    gtk_ml_hash_trie_insert(&ptr_map, &deserf->ptr_map, (void *) offset, result); // converting a size_t to void * is a hack, but it works
    deserf->ptr_map = ptr_map;

    return result;
}

gboolean gtk_ml_serf_program(GtkMl_Serializer *serf, GtkMl_Context *ctx, FILE *stream, GtkMl_S **err, const GtkMl_Program *program) {
    fprintf(stream, "GTKML-P(");

    uint64_t n_start = strlen(program->start);
    fwrite(&n_start, sizeof(uint64_t), 1, stream);
    fwrite(program->start, 1, n_start + 1, stream);

    uint64_t n_exec = program->n_exec;
    fwrite(&n_exec, sizeof(uint64_t), 1, stream);
    fwrite(program->exec, sizeof(GtkMl_Instruction), program->n_exec, stream);

    uint64_t n_static = program->n_static;
    fwrite(&n_static, sizeof(uint64_t), 1, stream);

    for (size_t i = 1; i < n_static; i++) {
        if (!gtk_ml_serf_value(serf, ctx, stream, err, program->statics[i])) {
            return 0;
        }
    }
    fprintf(stream, ")");

    gtk_ml_del_hash_trie(ctx, &serf->ptr_map, gtk_ml_delete_void_reference);

    return 1;
}

gboolean gtk_ml_deserf_program(GtkMl_Deserializer *deserf, GtkMl_Context *ctx, GtkMl_Program *program, FILE *stream, GtkMl_S **err) {
    char *gtkml_p = malloc(strlen("GTKML-P(") + 1);
    fread(gtkml_p, 1, strlen("GTKML-P("), stream);
    gtkml_p[strlen("GTKML-P(")] = 0;
    if (strcmp(gtkml_p, "GTKML-P(") != 0) {
        free(gtkml_p);
        *err = gtk_ml_error(ctx, "deser-error", GTKML_ERR_DESER_ERROR, 0, 0, 0, 0);
        return 0;
    }
    free(gtkml_p);

    uint64_t n_start;
    fread(&n_start, sizeof(uint64_t), 1, stream);
    program->start = malloc(n_start + 1);
    fread((void *) program->start, 1, n_start + 1, stream);

    uint64_t n_exec;
    fread(&n_exec, sizeof(uint64_t), 1, stream);
    program->n_exec = n_exec;
    program->exec = malloc(sizeof(GtkMl_Instruction) * program->n_exec);
    fread(program->exec, sizeof(GtkMl_Instruction), program->n_exec, stream);

    uint64_t n_static;
    fread(&n_static, sizeof(uint64_t), 1, stream);
    program->n_static = n_static;
    program->statics = malloc(sizeof(GtkMl_S *) * program->n_static);

    for (size_t i = 1; i < program->n_static; i++) {
        GtkMl_S *value = gtk_ml_deserf_value(deserf, ctx, stream, err);
        if (!value) {
            return 0;
        }
        program->statics[i] = value;
    }

    char *end = malloc(2);
    fread(end, 1, 1, stream);
    end[1] = 0;
    if (strcmp(end, ")") != 0) {
        free(end);
        *err = gtk_ml_error(ctx, "deser-error", GTKML_ERR_DESER_ERROR, 0, 0, 0, 0);
        return 0;
    }
    free(end);

    gtk_ml_del_hash_trie(ctx, &deserf->ptr_map, gtk_ml_delete_void_reference);

    return 1;
}
