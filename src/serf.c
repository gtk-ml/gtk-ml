#include <stdlib.h>
#include <string.h>
#ifdef GTKML_ENABLE_GTK
#include <gtk/gtk.h>
#endif /* GTKML_ENABLE_GTK */
#include <math.h>
#define GTKML_INCLUDE_INTERNAL
#include "gtk-ml.h"
#include "gtk-ml-internal.h"

void gtk_ml_new_serializer(GtkMl_Serializer *serf) {
    gtk_ml_new_hash_trie(&serf->ptr_map, &GTKML_PTR_HASHER);
}

void gtk_ml_new_deserializer(GtkMl_Deserializer *deserf) {
    gtk_ml_new_hash_trie(&deserf->offset_map, &GTKML_PTR_HASHER);
}

struct SerfData {
    GtkMl_Context *ctx;
    GtkMl_Serializer *serf;
    FILE *stream;
    GtkMl_SObj *err;
    size_t n;
    gboolean result;
};

GTKML_PRIVATE GtkMl_VisitResult serf_hash_trie(GtkMl_HashTrie *ht, GtkMl_TaggedValue key, GtkMl_TaggedValue value, GtkMl_TaggedValue _data) {
    struct SerfData *data = _data.value.userdata;
    if (!gtk_ml_serf_sobject(data->serf, data->ctx, data->stream, data->err, key.value.sobj)) {
        data->result = 0;
        return GTKML_VISIT_BREAK;
    }
    fprintf(data->stream, ":");
    if (!gtk_ml_serf_sobject(data->serf, data->ctx, data->stream, data->err, value.value.sobj)) {
        data->result = 0;
        return GTKML_VISIT_BREAK;
    }
    ++data->n;
    if (data->n < gtk_ml_hash_trie_len(ht)) {
        fprintf(data->stream, ",");
    }

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult serf_hash_set(GtkMl_HashSet *hs, GtkMl_TaggedValue key, GtkMl_TaggedValue _data) {
    struct SerfData *data = _data.value.userdata;
    if (!gtk_ml_serf_sobject(data->serf, data->ctx, data->stream, data->err, key.value.sobj)) {
        data->result = 0;
        return GTKML_VISIT_BREAK;
    }
    ++data->n;
    if (data->n < gtk_ml_hash_set_len(hs)) {
        fprintf(data->stream, ",");
    }

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult serf_array(GtkMl_Array *array, size_t idx, GtkMl_TaggedValue value, GtkMl_TaggedValue _data) {
    (void) idx;
    struct SerfData *data = _data.value.userdata;
    if (!gtk_ml_serf_sobject(data->serf, data->ctx, data->stream, data->err, value.value.sobj)) {
        data->result = 0;
        return GTKML_VISIT_BREAK;
    }
    ++data->n;
    if (data->n < gtk_ml_array_trie_len(array)) {
        fprintf(data->stream, ",");
    }

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult serf_string(GtkMl_Array *array, size_t idx, GtkMl_TaggedValue value, GtkMl_TaggedValue _data) {
    (void) array;
    (void) idx;
    struct SerfData *data = _data.value.userdata;
    fwrite(&value.value.unicode, sizeof(uint32_t), 1, data->stream);
    ++data->n;

    return GTKML_VISIT_RECURSE;
}

gboolean gtk_ml_serf_sobject(GtkMl_Serializer *serf, GtkMl_Context *ctx, FILE *stream, GtkMl_SObj *err, GtkMl_SObj value) {
    GtkMl_TaggedValue opt_offset = gtk_ml_hash_trie_get(&serf->ptr_map, gtk_ml_value_sobject(value));
    
    if (gtk_ml_has_value(opt_offset)) {
        uint64_t offset = opt_offset.value.u64;
        fprintf(stream, "GTKML-R(");
        fwrite(&offset, sizeof(offset), 1, stream);
        fprintf(stream, ")");
        return 1;
    }

    uint64_t offset = ftell(stream);
    GtkMl_HashTrie ptr_map;
    gtk_ml_new_hash_trie(&ptr_map, &GTKML_PTR_HASHER);
    gtk_ml_hash_trie_insert(&ptr_map, &serf->ptr_map, gtk_ml_value_sobject(value), gtk_ml_value_uint(offset));
    serf->ptr_map = ptr_map;

    fprintf(stream, "GTKML-S(");
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
        fwrite(&value->value.s_program.kind, sizeof(uint32_t), 1, stream);
        if (!gtk_ml_serf_sobject(serf, ctx, stream, err, value->value.s_program.linkage_name)) {
            return 0;
        }
        fwrite(&value->value.s_program.addr, sizeof(uint64_t), 1, stream);
        if (!gtk_ml_serf_sobject(serf, ctx, stream, err, value->value.s_program.args)) {
            return 0;
        }
        if (!gtk_ml_serf_sobject(serf, ctx, stream, err, value->value.s_program.body)) {
            return 0;
        }
        if (!gtk_ml_serf_sobject(serf, ctx, stream, err, value->value.s_program.capture)) {
            return 0;
        }
        break;
    }
    case GTKML_S_ADDRESS:
        if (!gtk_ml_serf_sobject(serf, ctx, stream, err, value->value.s_address.linkage_name)) {
            return 0;
        }
        fwrite(&value->value.s_address.addr, sizeof(uint64_t), 1, stream);
        break;
    case GTKML_S_LIGHTDATA:
    case GTKML_S_USERDATA:
    case GTKML_S_FFI:
        *err = gtk_ml_error(ctx, "ser-error", GTKML_ERR_SER_ERROR, 0, 0, 0, 0);
        return 0;
    case GTKML_S_LAMBDA:
        if (!gtk_ml_serf_sobject(serf, ctx, stream, err, value->value.s_lambda.args)) {
            return 0;
        }
        if (!gtk_ml_serf_sobject(serf, ctx, stream, err, value->value.s_lambda.body)) {
            return 0;
        }
        if (!gtk_ml_serf_sobject(serf, ctx, stream, err, value->value.s_lambda.capture)) {
            return 0;
        }
        break;
    case GTKML_S_MACRO:
        if (!gtk_ml_serf_sobject(serf, ctx, stream, err, value->value.s_macro.args)) {
            return 0;
        }
        if (!gtk_ml_serf_sobject(serf, ctx, stream, err, value->value.s_macro.body)) {
            return 0;
        }
        if (!gtk_ml_serf_sobject(serf, ctx, stream, err, value->value.s_macro.capture)) {
            return 0;
        }
        break;
    case GTKML_S_MAP: {
        struct SerfData data = { ctx, serf, stream, err, 0, 1 }; 
        gtk_ml_hash_trie_foreach(&value->value.s_map.map, serf_hash_trie, gtk_ml_value_userdata(&data));
        if (!data.result) {
            return 0;
        }
        fwrite(";", 1, 1, stream);
        uint32_t has_metamap = value->value.s_map.metamap != NULL;
        fwrite(&has_metamap, sizeof(uint32_t), 1, stream);
        if (has_metamap) {
            if (!gtk_ml_serf_sobject(serf, ctx, stream, err, value->value.s_map.metamap)) {
                return 0;
            }
        }
    } break;
    case GTKML_S_SET: {
        struct SerfData data = { ctx, serf, stream, err, 0, 1 }; 
        gtk_ml_hash_set_foreach(&value->value.s_set.set, serf_hash_set, gtk_ml_value_userdata(&data));
        result = data.result;
    } break;
    case GTKML_S_ARRAY: {
        uint32_t is_string = gtk_ml_array_trie_is_string(&value->value.s_array.array);
        fwrite(&is_string, sizeof(uint32_t), 1, stream);
        if (is_string) {
            uint64_t len = gtk_ml_array_trie_len(&value->value.s_array.array);
            fwrite(&len, sizeof(uint64_t), 1, stream);
            struct SerfData data = { ctx, serf, stream, err, 0, 1 }; 
            gtk_ml_array_trie_foreach(&value->value.s_array.array, serf_string, gtk_ml_value_userdata(&data));
            result = data.result;
        } else {
            struct SerfData data = { ctx, serf, stream, err, 0, 1 }; 
            gtk_ml_array_trie_foreach(&value->value.s_array.array, serf_array, gtk_ml_value_userdata(&data));
            result = data.result;
        }
    } break;
    case GTKML_S_VAR:
        if (!gtk_ml_serf_sobject(serf, ctx, stream, err, value->value.s_var.expr)) {
            return 0;
        }
        break;
    case GTKML_S_VARARG:
        if (!gtk_ml_serf_sobject(serf, ctx, stream, err, value->value.s_vararg.expr)) {
            return 0;
        }
        break;
    case GTKML_S_QUOTE:
        if (!gtk_ml_serf_sobject(serf, ctx, stream, err, value->value.s_quote.expr)) {
            return 0;
        }
        break;
    case GTKML_S_QUASIQUOTE:
        if (!gtk_ml_serf_sobject(serf, ctx, stream, err, value->value.s_quasiquote.expr)) {
            return 0;
        }
        break;
    case GTKML_S_UNQUOTE:
        if (!gtk_ml_serf_sobject(serf, ctx, stream, err, value->value.s_unquote.expr)) {
            return 0;
        }
        break;
    case GTKML_S_LIST:
        while (value->kind != GTKML_S_NIL) {
            if (!gtk_ml_serf_sobject(serf, ctx, stream, err, gtk_ml_car(value))) {
                return 0;
            }
            value = gtk_ml_cdr(value);
            if (value->kind != GTKML_S_NIL) {
                fprintf(stream, ",");
            }
        }
        break;
    default:
        *err = gtk_ml_error(ctx, "invalid-sexpr", GTKML_ERR_INVALID_SEXPR, 0, 0, 0, 0);
        return 0;
    }
    fprintf(stream, ")");
    return result;
}

GtkMl_SObj gtk_ml_deserf_sobject(GtkMl_Deserializer *deserf, GtkMl_Context *ctx, FILE *stream, GtkMl_SObj *err) {
    uint64_t offset = ftell(stream);

    GtkMl_TaggedValue opt_ref = gtk_ml_hash_trie_get(&deserf->offset_map, gtk_ml_value_uint(offset));
    
    if (gtk_ml_has_value(opt_ref)) {
        return opt_ref.value.sobj;
    }

    char *gtkml_v = malloc(strlen("GTKML-S(") + 1);
    fread(gtkml_v, 1, strlen("GTKML-S("), stream);
    gtkml_v[strlen("GTKML-S(")] = 0;

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
        GtkMl_SObj result = gtk_ml_deserf_sobject(deserf, ctx, stream, err);
        fseek(stream, cont_at, SEEK_SET);

        return result;
    }

    if (strcmp(gtkml_v, "GTKML-S(") != 0) {
        free(gtkml_v);
        *err = gtk_ml_error(ctx, "deser-error", GTKML_ERR_DESER_ERROR, 0, 0, 0, 0);
        return NULL;
    }
    free(gtkml_v);

    uint32_t kind;
    fread(&kind, sizeof(uint32_t), 1, stream);

    GtkMl_SObj result = gtk_ml_new_sobject(ctx, 0, kind);

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
        result->kind = GTKML_S_NIL;
        char next = 0;
        fread(&next, 1, 1, stream);
        if (next != ')') {
            fseek(stream, -1, SEEK_CUR);
        }
        GtkMl_SObj *tail = &result;
        while (next != ')') {
            GtkMl_SObj value = gtk_ml_deserf_sobject(deserf, ctx, stream, err);
            if (!value) {
                return NULL;
            }
            *tail = gtk_ml_new_list(ctx, 0, value, gtk_ml_new_nil(ctx, 0));
            tail = &gtk_ml_cdr(*tail);
            fread(&next, 1, 1, stream);
        }
        fseek(stream, -1, SEEK_CUR);
        break;
    }
    case GTKML_S_MAP: {
        char next = 0;
        fread(&next, 1, 1, stream);
        if (next != ';') {
            fseek(stream, -1, SEEK_CUR);
        }
        gtk_ml_new_hash_trie(&result->value.s_map.map, &GTKML_DEFAULT_HASHER);
        while (next != ';') {
            GtkMl_SObj key = gtk_ml_deserf_sobject(deserf, ctx, stream, err);
            if (!key) {
                return NULL;
            }
            char colon;
            fread(&colon, 1, 1, stream);
            if (colon != ':') {
                *err = gtk_ml_error(ctx, "deser-error", GTKML_ERR_DESER_ERROR, 0, 0, 0, 0);
                return NULL;
            }
            GtkMl_SObj value = gtk_ml_deserf_sobject(deserf, ctx, stream, err);
            if (!value) {
                return NULL;
            }
            GtkMl_SObj new = gtk_ml_new_map(ctx, 0, NULL);
            gtk_ml_hash_trie_insert(&new->value.s_map.map, &result->value.s_map.map, gtk_ml_value_sobject(key), gtk_ml_value_sobject(value));
            result = new;
            fread(&next, 1, 1, stream);
        }
        uint32_t has_metamap;
        fread(&has_metamap, sizeof(uint32_t), 1, stream);
        GtkMl_SObj metamap = NULL;
        if (has_metamap) {
            metamap = gtk_ml_deserf_sobject(deserf, ctx, stream, err);
            if (!metamap) {
                return 0;
            }
        }
        result->value.s_map.metamap = metamap;
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
            GtkMl_SObj key = gtk_ml_deserf_sobject(deserf, ctx, stream, err);
            if (!key) {
                return NULL;
            }
            GtkMl_SObj new = gtk_ml_new_set(ctx, 0);
            gtk_ml_hash_set_insert(&new->value.s_set.set, &result->value.s_set.set, gtk_ml_value_sobject(key));
            result = new;
            fread(&next, 1, 1, stream);
        }
        fseek(stream, -1, SEEK_CUR);
        break;
    }
    case GTKML_S_ARRAY: {
        uint32_t is_string;
        fread(&is_string, sizeof(uint32_t), 1, stream);
        if (is_string) {
            uint64_t len;
            fread(&len, sizeof(uint64_t), 1, stream);
            gtk_ml_new_array_trie(&result->value.s_array.array);
            for (size_t i = 0; i < len; i++) {
                uint32_t unicode;
                fread(&unicode, sizeof(uint32_t), 1, stream);
                GtkMl_SObj new = gtk_ml_new_array(ctx, 0);
                gtk_ml_array_trie_push(&new->value.s_array.array, &result->value.s_array.array, gtk_ml_value_char(unicode));
                result = new;
            }
        } else {
            char next = 0;
            fread(&next, 1, 1, stream);
            if (next != ')') {
                fseek(stream, -1, SEEK_CUR);
            }
            gtk_ml_new_array_trie(&result->value.s_array.array);
            while (next != ')') {
                GtkMl_SObj value = gtk_ml_deserf_sobject(deserf, ctx, stream, err);
                if (!value) {
                    return NULL;
                }
                GtkMl_SObj new = gtk_ml_new_array(ctx, 0);
                gtk_ml_array_trie_push(&new->value.s_array.array, &result->value.s_array.array, gtk_ml_value_sobject(value));
                result = new;
                fread(&next, 1, 1, stream);
            }
            fseek(stream, -1, SEEK_CUR);
        }
        result->value.s_array.array.string = is_string;
        break;
    }
    case GTKML_S_VAR: {
        GtkMl_SObj expr = gtk_ml_deserf_sobject(deserf, ctx, stream, err);
        if (!expr) {
            return NULL;
        }
        result->value.s_var.expr = expr;
    } break;
    case GTKML_S_VARARG: {
        GtkMl_SObj expr = gtk_ml_deserf_sobject(deserf, ctx, stream, err);
        if (!expr) {
            return NULL;
        }
        result->value.s_vararg.expr = expr;
    } break;
    case GTKML_S_QUOTE: {
        GtkMl_SObj expr = gtk_ml_deserf_sobject(deserf, ctx, stream, err);
        if (!expr) {
            return NULL;
        }
        result->value.s_quote.expr = expr;
    } break;
    case GTKML_S_QUASIQUOTE: {
        GtkMl_SObj expr = gtk_ml_deserf_sobject(deserf, ctx, stream, err);
        if (!expr) {
            return NULL;
        }
        result->value.s_quasiquote.expr = expr;
    } break;
    case GTKML_S_UNQUOTE: {
        GtkMl_SObj expr = gtk_ml_deserf_sobject(deserf, ctx, stream, err);
        if (!expr) {
            return NULL;
        }
        result->value.s_unquote.expr = expr;
    } break;
    case GTKML_S_LAMBDA: {
        GtkMl_SObj args = gtk_ml_deserf_sobject(deserf, ctx, stream, err);
        if (!args) {
            return NULL;
        }
        GtkMl_SObj body = gtk_ml_deserf_sobject(deserf, ctx, stream, err);
        if (!body) {
            return NULL;
        }
        GtkMl_SObj capture = gtk_ml_deserf_sobject(deserf, ctx, stream, err);
        if (!capture) {
            return NULL;
        }
        result->value.s_lambda.args = args;
        result->value.s_lambda.body = body;
        result->value.s_lambda.capture = capture;
        break;
    }
    case GTKML_S_PROGRAM: {
        GtkMl_ProgramKind kind;
        fread(&kind, sizeof(uint32_t), 1, stream);
        GtkMl_SObj linkage_name = gtk_ml_deserf_sobject(deserf, ctx, stream, err);
        if (!linkage_name) {
            return NULL;
        }
        result->value.s_program.linkage_name = linkage_name;
        uint64_t addr;
        fread(&addr, sizeof(uint64_t), 1, stream);
        result->value.s_program.addr = addr;
        GtkMl_SObj args = gtk_ml_deserf_sobject(deserf, ctx, stream, err);
        if (!args) {
            return NULL;
        }
        GtkMl_SObj body = gtk_ml_deserf_sobject(deserf, ctx, stream, err);
        if (!body) {
            return NULL;
        }
        GtkMl_SObj capture = gtk_ml_deserf_sobject(deserf, ctx, stream, err);
        if (!capture) {
            return NULL;
        }
        result->value.s_program.kind = kind;
        result->value.s_program.args = args;
        result->value.s_program.body = body;
        result->value.s_program.capture = capture;
        break;
    }
    case GTKML_S_ADDRESS: {
        GtkMl_SObj linkage_name = gtk_ml_deserf_sobject(deserf, ctx, stream, err);
        if (!linkage_name) {
            return NULL;
        }
        result->value.s_address.linkage_name = linkage_name;
        uint64_t addr;
        fread(&addr, sizeof(uint64_t), 1, stream);
        result->value.s_address.addr = addr;
        break;
    }
    case GTKML_S_MACRO: {
        GtkMl_SObj args = gtk_ml_deserf_sobject(deserf, ctx, stream, err);
        if (!args) {
            return NULL;
        }
        GtkMl_SObj body = gtk_ml_deserf_sobject(deserf, ctx, stream, err);
        if (!body) {
            return NULL;
        }
        GtkMl_SObj capture = gtk_ml_deserf_sobject(deserf, ctx, stream, err);
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
    case GTKML_S_FFI:
        *err = gtk_ml_error(ctx, "deser-error", GTKML_ERR_DESER_ERROR, 0, 0, 0, 0);
        return 0;
    default:
        *err = gtk_ml_error(ctx, "invalid-sexpr", GTKML_ERR_INVALID_SEXPR, 0, 0, 0, 0);
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

    GtkMl_HashTrie offset_map;
    gtk_ml_new_hash_trie(&offset_map, &GTKML_PTR_HASHER);
    gtk_ml_hash_trie_insert(&offset_map, &deserf->offset_map, gtk_ml_value_uint(offset), gtk_ml_value_sobject(result));
    deserf->offset_map = offset_map;

    return result;
}

gboolean gtk_ml_serf_program(GtkMl_Serializer *serf, GtkMl_Context *ctx, FILE *stream, GtkMl_SObj *err, const GtkMl_Program *program) {
    fprintf(stream, "GTKML-P(");

    uint64_t n_start = strlen(program->start);
    fwrite(&n_start, sizeof(uint64_t), 1, stream);
    fwrite(program->start, 1, n_start + 1, stream);

    uint64_t n_text = program->n_text;
    fwrite(&n_text, sizeof(uint64_t), 1, stream);
    fwrite(program->text, sizeof(GtkMl_Instruction), program->n_text, stream);

    uint64_t n_debug = program->n_loc;
    fwrite(&n_debug, sizeof(uint64_t), 1, stream);
    fwrite(program->debug_loc, sizeof(GtkMl_RleLoc), program->n_loc, stream);

    uint64_t n_data = program->n_data;
    fwrite(&n_data, sizeof(uint64_t), 1, stream);
    fwrite(program->data, sizeof(GtkMl_TaggedValue), program->n_data, stream);

    uint64_t n_static = program->n_static;
    fwrite(&n_static, sizeof(uint64_t), 1, stream);

    for (size_t i = 1; i < n_static; i++) {
        if (!gtk_ml_serf_sobject(serf, ctx, stream, err, program->statics[i])) {
            return 0;
        }
    }
    fprintf(stream, ")");

    gtk_ml_del_hash_trie(ctx, &serf->ptr_map, gtk_ml_delete_value);

    return 1;
}

GtkMl_Program *gtk_ml_deserf_program(GtkMl_Deserializer *deserf, GtkMl_Context *ctx, FILE *stream, GtkMl_SObj *err) {
    if (ctx->gc->program_len == ctx->gc->program_cap) {
        ctx->gc->program_cap *= 2;
        ctx->gc->programs = realloc(ctx->gc->programs, sizeof(GtkMl_Program *) * ctx->gc->program_cap);
    }
    ctx->gc->programs[ctx->gc->program_len] = malloc(sizeof(GtkMl_Program));
    GtkMl_Program *program = ctx->gc->programs[ctx->gc->program_len++];

    char *gtkml_p = malloc(strlen("GTKML-P(") + 1);
    fread(gtkml_p, 1, strlen("GTKML-P("), stream);
    gtkml_p[strlen("GTKML-P(")] = 0;
    if (strcmp(gtkml_p, "GTKML-P(") != 0) {
        free(gtkml_p);
        *err = gtk_ml_error(ctx, "deser-error", GTKML_ERR_DESER_ERROR, 0, 0, 0, 0);
        return NULL;
    }
    free(gtkml_p);

    uint64_t n_start;
    fread(&n_start, sizeof(uint64_t), 1, stream);
    program->start = malloc(n_start + 1);
    fread((void *) program->start, 1, n_start + 1, stream);

    uint64_t n_text;
    fread(&n_text, sizeof(uint64_t), 1, stream);
    program->n_text = n_text;
    program->text = malloc(sizeof(GtkMl_Instruction) * program->n_text);
    fread(program->text, sizeof(GtkMl_Instruction), program->n_text, stream);

    uint64_t n_debug;
    fread(&n_debug, sizeof(uint64_t), 1, stream);
    program->n_loc = n_debug;
    program->debug_loc = malloc(sizeof(GtkMl_RleLoc) * program->n_loc);
    fread(program->debug_loc, sizeof(GtkMl_RleLoc), program->n_loc, stream);

    uint64_t n_data;
    fread(&n_data, sizeof(uint64_t), 1, stream);
    program->n_data = n_data;
    program->data = malloc(sizeof(GtkMl_TaggedValue) * program->n_data);
    fread(program->data, sizeof(GtkMl_TaggedValue), program->n_data, stream);

    uint64_t n_static;
    fread(&n_static, sizeof(uint64_t), 1, stream);
    program->n_static = n_static;
    program->statics = malloc(sizeof(GtkMl_SObj) * program->n_static);

    for (size_t i = 1; i < program->n_static; i++) {
        GtkMl_SObj value = gtk_ml_deserf_sobject(deserf, ctx, stream, err);
        if (!value) {
            return NULL;
        }
        program->statics[i] = value;
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

    gtk_ml_del_hash_trie(ctx, &deserf->offset_map, gtk_ml_delete_value);

    return program;
}
