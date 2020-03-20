#include <stdlib.h>
#include <string.h>
#define GTKML_INCLUDE_INTERNAL
#include "gtk-ml.h"
#include "gtk-ml-internal.h"

GTKML_PRIVATE void span_add(GtkMl_Span *dest, GtkMl_Span *lhs, GtkMl_Span *rhs) {
    size_t len = rhs->ptr - lhs->ptr + rhs->len;
    const char *ptr = lhs->ptr;
    int line = lhs->line;
    int col = lhs->col;
    dest->ptr = ptr;
    dest->len = len;
    dest->line = line;
    dest->col = col;
}

GtkMl_SObj gtk_ml_parse_vararg(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_ELLIPSIS) {
        *err = gtk_ml_error(ctx, "token-error", GTKML_ERR_TOKEN_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
        return NULL;
    }

    GtkMl_Span span = (*tokenv)[0].span;

    ++*tokenv;
    --*tokenc;

    GtkMl_SObj expr = gtk_ml_parse(index, ctx, err, tokenv, tokenc);
    if (!expr) {
        return NULL;
    }

    span_add(&span, &span, gtk_ml_get_span(&index->debug_data, expr->loc));

    return gtk_ml_new_vararg(ctx, gtk_ml_add_location(&index->debug_data, &span), expr);
}

GtkMl_SObj gtk_ml_parse_quote(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_TICK) {
        *err = gtk_ml_error(ctx, "token-error", GTKML_ERR_TOKEN_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
        return NULL;
    }

    GtkMl_Span span = (*tokenv)[0].span;

    ++*tokenv;
    --*tokenc;

    GtkMl_SObj expr = gtk_ml_parse(index, ctx, err, tokenv, tokenc);
    if (!expr) {
        return NULL;
    }

    span_add(&span, &span, gtk_ml_get_span(&index->debug_data, expr->loc));

    return gtk_ml_new_quote(ctx, gtk_ml_add_location(&index->debug_data, &span), expr);
}

GtkMl_SObj gtk_ml_parse_quasiquote(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_BACKTICK) {
        *err = gtk_ml_error(ctx, "token-error", GTKML_ERR_TOKEN_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
        return NULL;
    }

    GtkMl_Span span = (*tokenv)[0].span;

    ++*tokenv;
    --*tokenc;

    GtkMl_SObj expr = gtk_ml_parse(index, ctx, err, tokenv, tokenc);
    if (!expr) {
        return NULL;
    }

    span_add(&span, &span, gtk_ml_get_span(&index->debug_data, expr->loc));

    return gtk_ml_new_quasiquote(ctx, gtk_ml_add_location(&index->debug_data, &span), expr);
}

GtkMl_SObj gtk_ml_parse_unquote(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_COMMA) {
        *err = gtk_ml_error(ctx, "token-error", GTKML_ERR_TOKEN_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
        return NULL;
    }

    GtkMl_Span span = (*tokenv)[0].span;

    ++*tokenv;
    --*tokenc;

    GtkMl_SObj expr = gtk_ml_parse(index, ctx, err, tokenv, tokenc);
    if (!expr) {
        return NULL;
    }

    span_add(&span, &span, gtk_ml_get_span(&index->debug_data, expr->loc));

    return gtk_ml_new_unquote(ctx, gtk_ml_add_location(&index->debug_data, &span), expr);
}

GtkMl_SObj gtk_ml_parse_set(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_CURLYL) {
        *err = gtk_ml_error(ctx, "token-error", GTKML_ERR_TOKEN_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
        return NULL;
    }

    GtkMl_Span span = (*tokenv)[0].span;

    ++*tokenv;
    --*tokenc;

    if (*tokenc == 0) {
        *err = gtk_ml_error(ctx, "eof-error", GTKML_ERR_EOF_ERROR, span.ptr != NULL, span.line, span.col, 0);
        return NULL;
    }

    GtkMl_SObj result = gtk_ml_new_set(ctx, gtk_ml_add_location(&index->debug_data, &span));

    while ((*tokenv)[0].kind != GTKML_TOK_CURLYR) {
        GtkMl_SObj key = gtk_ml_parse(index, ctx, err, tokenv, tokenc);
        if (!key) {
            return NULL;
        }

        if (*tokenc == 0) {
            *err = gtk_ml_error(ctx, "eof-error", GTKML_ERR_EOF_ERROR, span.ptr != NULL, span.line, span.col, 0);
            return NULL;
        }

        span_add(&span, &span, &(*tokenv)[0].span);

        GtkMl_SObj set = result;
        result = gtk_ml_new_set(ctx, gtk_ml_add_location(&index->debug_data, &span));
        gtk_ml_hash_set_insert(&result->value.s_set.set, &set->value.s_set.set, gtk_ml_value_sobject(key));
    }

    result->loc = gtk_ml_add_location(&index->debug_data, &span);

    ++*tokenv;
    --*tokenc;

    return result;
}

GtkMl_SObj gtk_ml_parse_constant(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_IDENT) {
        *err = gtk_ml_error(ctx, "token-error", GTKML_ERR_TOKEN_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
        return NULL;
    }
    GtkMl_SObj result = NULL;
    GtkMl_Location loc = gtk_ml_add_location(&index->debug_data, &(*tokenv)[0].span);
    if (strncmp((*tokenv)[0].span.ptr, "nil", (*tokenv)[0].span.len) == 0) {
        result = gtk_ml_new_nil(ctx, loc);
    } else if (strncmp((*tokenv)[0].span.ptr, "t", (*tokenv)[0].span.len) == 0) {
        result = gtk_ml_new_true(ctx, loc);
    } else if (strncmp((*tokenv)[0].span.ptr, "f", (*tokenv)[0].span.len) == 0) {
        result = gtk_ml_new_false(ctx, loc);
    } else {
        *err = gtk_ml_error(ctx, "constant-error", GTKML_ERR_CONSTANT_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
        return NULL;
    }
    ++*tokenv;
    --*tokenc;
    return result;
}

GtkMl_SObj gtk_ml_parse_alternative(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_POUND) {
        *err = gtk_ml_error(ctx, "token-error", GTKML_ERR_TOKEN_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
        return NULL;
    }

    GtkMl_Span span = (*tokenv)[0].span;

    ++*tokenv;
    --*tokenc;

    GtkMl_SObj expr;
    switch ((*tokenv)[0].kind) {
    case GTKML_TOK_CURLYL:
        expr = gtk_ml_parse_set(index, ctx, err, tokenv, tokenc);
        break;
    case GTKML_TOK_IDENT:
        expr = gtk_ml_parse_constant(index, ctx, err, tokenv, tokenc);
        break;
    default:
        *err = gtk_ml_error(ctx, "token-error", GTKML_ERR_TOKEN_ERROR, span.ptr != NULL, span.line, span.col, 0);
        return NULL;
    }

    if (!expr) {
        return NULL;
    }

    span_add(&span, &span, gtk_ml_get_span(&index->debug_data, expr->loc));
    expr->loc = gtk_ml_add_location(&index->debug_data, &span);

    return expr;
}

GtkMl_SObj gtk_ml_parse_get(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_AT) {
        *err = gtk_ml_error(ctx, "token-error", GTKML_ERR_TOKEN_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
        return NULL;
    }

    GtkMl_Span span = (*tokenv)[0].span;

    GtkMl_SObj get = gtk_ml_new_symbol(ctx, gtk_ml_add_location(&index->debug_data, &span), 0, "get", strlen("get"));

    ++*tokenv;
    --*tokenc;

    GtkMl_SObj expr = gtk_ml_parse(index, ctx, err, tokenv, tokenc);
    if (!expr) {
        return NULL;
    }

    span_add(&span, &span, gtk_ml_get_span(&index->debug_data, expr->loc));
    GtkMl_Location loc = gtk_ml_add_location(&index->debug_data, &span);
    return gtk_ml_new_list(ctx, gtk_ml_add_location(&index->debug_data, &span), get, gtk_ml_new_list(ctx, loc, expr, gtk_ml_new_nil(ctx, loc)));
}

GtkMl_SObj gtk_ml_parse_dot(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_DOT) {
        *err = gtk_ml_error(ctx, "token-error", GTKML_ERR_TOKEN_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
        return NULL;
    }

    GtkMl_Span span = (*tokenv)[0].span;

    GtkMl_SObj get = gtk_ml_new_symbol(ctx, gtk_ml_add_location(&index->debug_data, &span), 0, "map-get", strlen("map-get"));

    ++*tokenv;
    --*tokenc;

    GtkMl_SObj key = gtk_ml_parse(index, ctx, err, tokenv, tokenc);
    if (!key) {
        return NULL;
    }

    key = gtk_ml_new_quasiquote(ctx, key->loc, key);

    GtkMl_SObj map = gtk_ml_parse(index, ctx, err, tokenv, tokenc);
    if (!map) {
        return NULL;
    }

    span_add(&span, &span, gtk_ml_get_span(&index->debug_data, map->loc)); 
    GtkMl_Location loc = gtk_ml_add_location(&index->debug_data, &span);
    return gtk_ml_new_list(ctx, gtk_ml_add_location(&index->debug_data, &span), get, gtk_ml_new_list(ctx, loc, map, gtk_ml_new_list(ctx, loc, key, gtk_ml_new_nil(ctx, loc))));
}

GtkMl_SObj gtk_ml_parse_list_rest(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Token **tokenv, size_t *tokenc) {
    if (*tokenc == 0) {
        *err = gtk_ml_error(ctx, "eof-error", GTKML_ERR_EOF_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
        return NULL;
    }

    GtkMl_Span span = (*tokenv)[0].span;

    if ((*tokenv)[0].kind == GTKML_TOK_PARENR) {
        span_add(&span, &span, &(*tokenv)[0].span);

        ++*tokenv;
        --*tokenc;

        return gtk_ml_new_nil(ctx, gtk_ml_add_location(&index->debug_data, &span));
    }

    GtkMl_SObj car = gtk_ml_parse(index, ctx, err, tokenv, tokenc);
    if (!car) {
        return NULL;
    }
    GtkMl_SObj cdr = gtk_ml_parse_list_rest(index, ctx, err, tokenv, tokenc);
    if (!cdr) {
        return NULL;
    }

    span_add(&span, &span, gtk_ml_get_span(&index->debug_data, cdr->loc));

    return gtk_ml_new_list(ctx, gtk_ml_add_location(&index->debug_data, &span), car, cdr);
}

GtkMl_SObj gtk_ml_parse_list(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_PARENL) {
        *err = gtk_ml_error(ctx, "token-error", GTKML_ERR_TOKEN_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
        return NULL;
    }

    ++*tokenv;
    --*tokenc;

    return gtk_ml_parse_list_rest(index, ctx, err, tokenv, tokenc);
}

GtkMl_SObj gtk_ml_parse_array(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_SQUAREL) {
        *err = gtk_ml_error(ctx, "token-error", GTKML_ERR_TOKEN_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
        return NULL;
    }

    GtkMl_Span span = (*tokenv)[0].span;

    ++*tokenv;
    --*tokenc;

    if (*tokenc == 0) {
        *err = gtk_ml_error(ctx, "eof-error", GTKML_ERR_EOF_ERROR, span.ptr != NULL, span.line, span.col, 0);
        return NULL;
    }

    GtkMl_SObj result = gtk_ml_new_array(ctx, gtk_ml_add_location(&index->debug_data, &span));

    while ((*tokenv)[0].kind != GTKML_TOK_SQUARER) {
        GtkMl_SObj elem = gtk_ml_parse(index, ctx, err, tokenv, tokenc);
        if (!elem) {
            return NULL;
        }

        if (*tokenc == 0) {
            *err = gtk_ml_error(ctx, "eof-error", GTKML_ERR_EOF_ERROR, span.ptr != NULL, span.line, span.col, 0);
            return NULL;
        }

        span_add(&span, &span, &(*tokenv)[0].span);

        GtkMl_SObj array = result;
        result = gtk_ml_new_array(ctx, gtk_ml_add_location(&index->debug_data, &span));
        gtk_ml_array_trie_push(&result->value.s_array.array, &array->value.s_array.array, gtk_ml_value_sobject(elem));
    }

    result->loc = gtk_ml_add_location(&index->debug_data, &span);

    ++*tokenv;
    --*tokenc;

    return result;
}

GtkMl_SObj gtk_ml_parse_map(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_CURLYL) {
        *err = gtk_ml_error(ctx, "token-error", GTKML_ERR_TOKEN_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
        return NULL;
    }

    GtkMl_Span span = (*tokenv)[0].span;

    ++*tokenv;
    --*tokenc;

    if (*tokenc == 0) {
        *err = gtk_ml_error(ctx, "eof-error", GTKML_ERR_EOF_ERROR, span.ptr != NULL, span.line, span.col, 0);
        return NULL;
    }

    GtkMl_SObj result = gtk_ml_new_map(ctx, gtk_ml_add_location(&index->debug_data, &span), NULL);

    while ((*tokenv)[0].kind != GTKML_TOK_CURLYR) {
        GtkMl_SObj key = gtk_ml_parse(index, ctx, err, tokenv, tokenc);
        if (!key) {
            return NULL;
        }

        if (*tokenc == 0) {
            *err = gtk_ml_error(ctx, "eof-error", GTKML_ERR_EOF_ERROR, span.ptr != NULL, span.line, span.col, 0);
            return NULL;
        }

        GtkMl_SObj value = gtk_ml_parse(index, ctx, err, tokenv, tokenc);
        if (!value) {
            return NULL;
        }

        if (*tokenc == 0) {
            *err = gtk_ml_error(ctx, "eof-error", GTKML_ERR_EOF_ERROR, span.ptr != NULL, span.line, span.col, 0);
            return NULL;
        }

        span_add(&span, &span, &(*tokenv)[0].span);

        GtkMl_SObj map = result;
        result = gtk_ml_new_map(ctx, gtk_ml_add_location(&index->debug_data, &span), NULL);
        gtk_ml_hash_trie_insert(&result->value.s_map.map, &map->value.s_map.map, gtk_ml_value_sobject(key), gtk_ml_value_sobject(value));
    }

    result->loc = gtk_ml_add_location(&index->debug_data, &span);

    ++*tokenv;
    --*tokenc;

    return result;
}

GtkMl_SObj gtk_ml_parse_int(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_INT) {
        *err = gtk_ml_error(ctx, "token-error", GTKML_ERR_TOKEN_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
        return NULL;
    }
    GtkMl_SObj result = gtk_ml_new_int(ctx, gtk_ml_add_location(&index->debug_data, &(*tokenv)[0].span), (*tokenv)[0].value.intval);
    ++*tokenv;
    --*tokenc;
    return result;
}

GtkMl_SObj gtk_ml_parse_float(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_FLOAT) {
        *err = gtk_ml_error(ctx, "token-error", GTKML_ERR_TOKEN_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
        return NULL;
    }
    GtkMl_SObj result = gtk_ml_new_float(ctx, gtk_ml_add_location(&index->debug_data, &(*tokenv)[0].span), (*tokenv)[0].value.floatval);
    ++*tokenv;
    --*tokenc;
    return result;
}

GtkMl_SObj gtk_ml_parse_char(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_CHAR) {
        *err = gtk_ml_error(ctx, "token-error", GTKML_ERR_TOKEN_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
        return NULL;
    }
    size_t len = (*tokenv)[0].span.len;
    GtkMl_SObj result;
    if (len == 2) {
        result = gtk_ml_new_char(ctx, gtk_ml_add_location(&index->debug_data, &(*tokenv)[0].span), (*tokenv)[0].span.ptr[1]);
    } else {
        const char *ptr = (*tokenv)[0].span.ptr;
        if (strncmp(ptr, "\\newline", len) == 0) {
            result = gtk_ml_new_char(ctx, gtk_ml_add_location(&index->debug_data, &(*tokenv)[0].span), '\n');
        } else if (strncmp(ptr, "\\space", len) == 0) {
            result = gtk_ml_new_char(ctx, gtk_ml_add_location(&index->debug_data, &(*tokenv)[0].span), ' ');
        } else if (strncmp(ptr, "\\tab", len) == 0) {
            result = gtk_ml_new_char(ctx, gtk_ml_add_location(&index->debug_data, &(*tokenv)[0].span), '\t');
        } else if (strncmp(ptr, "\\escape", len) == 0) {
            result = gtk_ml_new_char(ctx, gtk_ml_add_location(&index->debug_data, &(*tokenv)[0].span), '\x1b');
        } else if (len == 4 && strncmp(ptr, "\\x", 2) == 0) {
            char *endptr;
            uint8_t ascii = strtoul(ptr + 2, &endptr, 16);
            if (gtk_ml_is_ident_cont(*endptr)) {
                *err = gtk_ml_error(ctx, "escape-error", GTKML_ERR_ESCAPE_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
                return NULL;
            }
            result = gtk_ml_new_char(ctx, gtk_ml_add_location(&index->debug_data, &(*tokenv)[0].span), ascii);
        } else if (len == 6 && strncmp(ptr, "\\u", 2) == 0) {
            char *endptr;
            uint16_t unicode = strtoul(ptr + 2, &endptr, 16);
            if (gtk_ml_is_ident_cont(*endptr)) {
                *err = gtk_ml_error(ctx, "escape-error", GTKML_ERR_ESCAPE_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
                return NULL;
            }
            result = gtk_ml_new_char(ctx, gtk_ml_add_location(&index->debug_data, &(*tokenv)[0].span), unicode);
        } else if (len == 10 && strncmp(ptr, "\\U", 2) == 0) {
            char *endptr;
            uint32_t unicode = strtoul(ptr + 2, &endptr, 16);
            if (gtk_ml_is_ident_cont(*endptr)) {
                *err = gtk_ml_error(ctx, "escape-error", GTKML_ERR_ESCAPE_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
                return NULL;
            }
            result = gtk_ml_new_char(ctx, gtk_ml_add_location(&index->debug_data, &(*tokenv)[0].span), unicode);
        } else {
            *err = gtk_ml_error(ctx, "escape-error", GTKML_ERR_ESCAPE_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
            return NULL;
        }
    }
    ++*tokenv;
    --*tokenc;
    return result;
}

GtkMl_SObj gtk_ml_parse_string(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_STRING) {
        *err = gtk_ml_error(ctx, "token-error", GTKML_ERR_TOKEN_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
        return NULL;
    }
    size_t len = (*tokenv)[0].span.len - 2;
    GtkMl_SObj result = gtk_ml_new_string(ctx, gtk_ml_add_location(&index->debug_data, &(*tokenv)[0].span), (*tokenv)[0].span.ptr + 1, len);
    ++*tokenv;
    --*tokenc;
    return result;
}

GtkMl_SObj gtk_ml_parse_symbol(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_IDENT) {
        *err = gtk_ml_error(ctx, "token-error", GTKML_ERR_TOKEN_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
        return NULL;
    }
    GtkMl_SObj result = gtk_ml_new_symbol(ctx, gtk_ml_add_location(&index->debug_data, &(*tokenv)[0].span), 0, (*tokenv)[0].span.ptr, (*tokenv)[0].span.len);
    ++*tokenv;
    --*tokenc;
    return result;
}

GtkMl_SObj gtk_ml_parse_keyword(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_KEYWORD) {
        *err = gtk_ml_error(ctx, "token-error", GTKML_ERR_TOKEN_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
        return NULL;
    }
    GtkMl_SObj result = gtk_ml_new_keyword(ctx, gtk_ml_add_location(&index->debug_data, &(*tokenv)[0].span), 0, (*tokenv)[0].span.ptr + 1, (*tokenv)[0].span.len - 1);
    ++*tokenv;
    --*tokenc;
    return result;
}

GtkMl_SObj gtk_ml_parse(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Token **tokenv, size_t *tokenc) {
    switch ((*tokenv)[0].kind) {
    case GTKML_TOK_PARENL:
        return gtk_ml_parse_list(index, ctx, err, tokenv, tokenc);
    case GTKML_TOK_SQUAREL:
        return gtk_ml_parse_array(index, ctx, err, tokenv, tokenc);
    case GTKML_TOK_CURLYL:
        return gtk_ml_parse_map(index, ctx, err, tokenv, tokenc);
    case GTKML_TOK_INT:
        return gtk_ml_parse_int(index, ctx, err, tokenv, tokenc);
    case GTKML_TOK_FLOAT:
        return gtk_ml_parse_float(index, ctx, err, tokenv, tokenc);
    case GTKML_TOK_CHAR:
        return gtk_ml_parse_char(index, ctx, err, tokenv, tokenc);
    case GTKML_TOK_STRING:
        return gtk_ml_parse_string(index, ctx, err, tokenv, tokenc);
    case GTKML_TOK_IDENT:
        return gtk_ml_parse_symbol(index, ctx, err, tokenv, tokenc);
    case GTKML_TOK_KEYWORD:
        return gtk_ml_parse_keyword(index, ctx, err, tokenv, tokenc);
    default:
        for (size_t i = 0; i < ctx->parser.len_reader; i++) {
            if (ctx->parser.readers[i].token == (*tokenv)[0].kind) {
                return ctx->parser.readers[i].fn(index, ctx, err, tokenv, tokenc);
            }
        }
        *err = gtk_ml_error(ctx, "token-error", GTKML_ERR_TOKEN_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
        return NULL;
    }
}
