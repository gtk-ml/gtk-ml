#include <stdlib.h>
#include <gtk/gtk.h>
#include "gtk-ml.h"

struct GtkMl_Context {
    gboolean gc_enabled;
    size_t n_values;
    size_t m_values;
    GtkMl_S *first;
    GtkMl_S *last;

    GtkMl_S **stack;
    size_t stack_len;
    size_t stack_cap;

    GtkMl_S *bindings;
    GtkMl_S **top_scope;
};

GTKML_PRIVATE GtkMl_S *new_value(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_SKind kind);
GTKML_PRIVATE GtkMl_S *new_nil(GtkMl_Context *ctx, GtkMl_Span *span);
GTKML_PRIVATE GtkMl_S *new_true(GtkMl_Context *ctx, GtkMl_Span *span);
GTKML_PRIVATE GtkMl_S *new_false(GtkMl_Context *ctx, GtkMl_Span *span);
GTKML_PRIVATE GtkMl_S *new_int(GtkMl_Context *ctx, GtkMl_Span *span, int64_t value);
GTKML_PRIVATE GtkMl_S *new_float(GtkMl_Context *ctx, GtkMl_Span *span, float value);
GTKML_PRIVATE GtkMl_S *new_string(GtkMl_Context *ctx, GtkMl_Span *span, const char *ptr, size_t len);
GTKML_PRIVATE GtkMl_S *new_symbol(GtkMl_Context *ctx, GtkMl_Span *span, const char *ptr, size_t len);
GTKML_PRIVATE GtkMl_S *new_keyword(GtkMl_Context *ctx, GtkMl_Span *span, const char *ptr, size_t len);
GTKML_PRIVATE GtkMl_S *new_list(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *car, GtkMl_S *cdr);
GTKML_PRIVATE GtkMl_S *new_map(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *car, GtkMl_S *cdr);
GTKML_PRIVATE GtkMl_S *new_lambda(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *args, GtkMl_S *body);
GTKML_PRIVATE GtkMl_S *new_lightdata(GtkMl_Context *ctx, GtkMl_Span *span, void *data);
GTKML_PRIVATE GtkMl_S *new_userdata(GtkMl_Context *ctx, GtkMl_Span *span, void *data, void (*del)(GtkMl_Context *ctx, void *));
GTKML_PRIVATE void delete(GtkMl_Context *ctx, GtkMl_S *s);
GTKML_PRIVATE void del(GtkMl_Context *ctx, GtkMl_S *s);
GTKML_PRIVATE void gtk_ml_object_unref(GtkMl_Context *ctx, void *obj);

GtkMl_Context *gtk_ml_new_context() {
    GtkMl_Context *ctx = malloc(sizeof(GtkMl_Context));
    ctx->gc_enabled = 1;
    ctx->n_values = 0;
    ctx->m_values = GTKML_GC_THRESHOLD;
    ctx->first = NULL;
    ctx->last = NULL;
    ctx->stack = malloc(sizeof(GtkMl_S *) * 64);
    ctx->stack_len = 0;
    ctx->stack_cap = 64;
    gtk_ml_disable_gc(ctx);
    // root value
    // the root doesn't get sweeped by gc, so it doesn't need to be pushed to stack
    new_nil(ctx, NULL);
    // ({'#nil #nil '#t #t '#f #f 'flags-none G_APPLICATION_FLAGS_NONE})
    ctx->bindings = new_list(ctx, NULL, new_nil(ctx, NULL), new_nil(ctx, NULL));
    ctx->top_scope = &gtk_ml_car(ctx->bindings);
    gtk_ml_define(ctx, new_symbol(ctx, NULL, "#nil", 4), new_nil(ctx, NULL));
    gtk_ml_define(ctx, new_symbol(ctx, NULL, "#t", 2), new_true(ctx, NULL));
    gtk_ml_define(ctx, new_symbol(ctx, NULL, "#f", 2), new_false(ctx, NULL));
    gtk_ml_define(ctx, new_symbol(ctx, NULL, "flags-none", 10), new_int(ctx, NULL, G_APPLICATION_FLAGS_NONE));
    gtk_ml_enable_gc(ctx, 1);
    return ctx;
}

void gtk_ml_del_context(GtkMl_Context *ctx) {
    ctx->stack_len = 0;
    free(ctx->stack);

    GtkMl_S *value = ctx->first;
    while (value) {
        GtkMl_S *next = value->next;
        del(ctx, value);
        value = next;
    }

    free(ctx);
}

GTKML_PRIVATE gboolean IDENT_BEGIN[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

GTKML_PRIVATE gboolean IDENT_CONT[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

GTKML_PRIVATE gboolean is_ident_begin(unsigned char c) {
    return IDENT_BEGIN[c];
}

GTKML_PRIVATE gboolean is_ident_cont(unsigned char c) {
    return IDENT_CONT[c];
}

GTKML_PRIVATE gboolean lex(GtkMl_Token **tokenv, size_t *tokenc, const char **err, const char *src) {
    *tokenv = malloc(sizeof(GtkMl_Token) * 64);
    *tokenc = 0;
    size_t cap = 64;

    int line = 1;
    int col = 1;

    while (*src) {
        if (*tokenc == cap) {
            cap *= 2;
            *tokenv = realloc(*tokenv, cap);
        }

        switch (*src) {
        case '\n':
            col = 1;
            ++line;
            ++src;
            continue;
        case '\t':
            ++col;
            ++src;
            continue;
        case ' ':
            ++col;
            ++src;
            continue;
        case '(':
            (*tokenv)[*tokenc].kind = GTKML_TOK_PARENL;
            (*tokenv)[*tokenc].span.ptr = src;
            (*tokenv)[*tokenc].span.len = 1;
            (*tokenv)[*tokenc].span.line = line;
            (*tokenv)[*tokenc].span.col = col;
            ++*tokenc;
            break;
        case ')':
            (*tokenv)[*tokenc].kind = GTKML_TOK_PARENR;
            (*tokenv)[*tokenc].span.ptr = src;
            (*tokenv)[*tokenc].span.len = 1;
            (*tokenv)[*tokenc].span.line = line;
            (*tokenv)[*tokenc].span.col = col;
            ++*tokenc;
            break;
        case '{':
            (*tokenv)[*tokenc].kind = GTKML_TOK_CURLYL;
            (*tokenv)[*tokenc].span.ptr = src;
            (*tokenv)[*tokenc].span.len = 1;
            (*tokenv)[*tokenc].span.line = line;
            (*tokenv)[*tokenc].span.col = col;
            ++*tokenc;
            break;
        case '}':
            (*tokenv)[*tokenc].kind = GTKML_TOK_CURLYR;
            (*tokenv)[*tokenc].span.ptr = src;
            (*tokenv)[*tokenc].span.len = 1;
            (*tokenv)[*tokenc].span.line = line;
            (*tokenv)[*tokenc].span.col = col;
            ++*tokenc;
            break;
        case '"': {
            ++col;
            ++src;
            // TODO(walterpi): escape codes
            const char *str_ptr = src - 1;
            int str_line = line;
            int str_col = col - 1;
            size_t str_len = 1;
            do {
                ++str_len;
                ++col;
                ++src;
            } while (*src != '"');
            (*tokenv)[*tokenc].kind = GTKML_TOK_STRING;
            (*tokenv)[*tokenc].span.ptr = str_ptr;
            (*tokenv)[*tokenc].span.len = str_len + 1;
            (*tokenv)[*tokenc].span.line = str_line;
            (*tokenv)[*tokenc].span.col = str_col;
            ++*tokenc;
        } break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': {
            char *endptr;
            int64_t intval = strtoll(src, &endptr, 10);
            if (*endptr == ' ' || *endptr == '\n' || *endptr == '\t'
                    || *endptr == '(' || *endptr == ')'
                    || *endptr == '{' || *endptr == '}') {
                (*tokenv)[*tokenc].kind = GTKML_TOK_INT;
                (*tokenv)[*tokenc].value.intval = intval;
                (*tokenv)[*tokenc].span.ptr = src;
                (*tokenv)[*tokenc].span.len = 1;
                (*tokenv)[*tokenc].span.line = line;
                (*tokenv)[*tokenc].span.col = col;
                ++*tokenc;
                src = endptr;
                continue;
            } else if (*endptr == '.') {
                double floatval = strtod(src, &endptr);
                if (*endptr == ' ' || *endptr == '\n' || *endptr == '\t'
                        || *endptr == '(' || *endptr == ')'
                        || *endptr == '{' || *endptr == '}') {
                    (*tokenv)[*tokenc].kind = GTKML_TOK_FLOAT;
                    (*tokenv)[*tokenc].value.floatval = floatval;
                    (*tokenv)[*tokenc].span.ptr = src;
                    (*tokenv)[*tokenc].span.len = 1;
                    (*tokenv)[*tokenc].span.line = line;
                    (*tokenv)[*tokenc].span.col = col;
                    ++*tokenc;
                    src = endptr;
                    continue;
                } else {
                    *err = GTKML_ERR_CHARACTER_ERROR;
                    return 0;
                }
            } else {
                *err = GTKML_ERR_CHARACTER_ERROR;
                return 0;
            }
        } break;
        case ':':
            ++col;
            ++src;
            if (is_ident_begin(*src)) {
                const char *kw_ptr = src - 1;
                int kw_line = line;
                int kw_col = col - 1;
                size_t kw_len = 1;
                while (is_ident_cont(*src)) {
                    ++kw_len;
                    ++col;
                    ++src;
                }
                (*tokenv)[*tokenc].kind = GTKML_TOK_KEYWORD;
                (*tokenv)[*tokenc].span.ptr = kw_ptr;
                (*tokenv)[*tokenc].span.len = kw_len;
                (*tokenv)[*tokenc].span.line = kw_line;
                (*tokenv)[*tokenc].span.col = kw_col;
                ++*tokenc;
                continue;
            } else {
                *err = GTKML_ERR_CHARACTER_ERROR;
                return 0;
            }
            break;
        default:
            if (is_ident_begin(*src)) {
                const char *ident_ptr = src;
                int ident_line = line;
                int ident_col = col;
                size_t ident_len = 0;
                while (is_ident_cont(*src)) {
                    ++ident_len;
                    ++col;
                    ++src;
                }
                (*tokenv)[*tokenc].kind = GTKML_TOK_IDENT;
                (*tokenv)[*tokenc].span.ptr = ident_ptr;
                (*tokenv)[*tokenc].span.len = ident_len;
                (*tokenv)[*tokenc].span.line = ident_line;
                (*tokenv)[*tokenc].span.col = ident_col;
                ++*tokenc;
                continue;
            } else {
                *err = GTKML_ERR_CHARACTER_ERROR;
                return 0;
            }
            break;
        }

        ++col;
        ++src;
    }

    return 1;
}

GTKML_PRIVATE GtkMl_S *new_value(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_SKind kind) {
    if (ctx->n_values >= ctx->m_values) {
        gtk_ml_collect(ctx);
    }
    ++ctx->n_values;

    GtkMl_S *s = malloc(sizeof(GtkMl_S));
    if (!ctx->last) {
        ctx->first = s;
        ctx->last = s;
    } else {
        ctx->last->next = s;
        ctx->last = s;
    }

    s->next = NULL;
    s->flags = GTKML_FLAG_NONE;
    s->kind = kind;
    if (span) {
        s->span = *span;
    } else {
        s->span = (GtkMl_Span) {0};
    }

    return s;
}

GTKML_PRIVATE GtkMl_S *new_nil(GtkMl_Context *ctx, GtkMl_Span *span) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_NIL);
    return s;
}

GTKML_PRIVATE GtkMl_S *new_true(GtkMl_Context *ctx, GtkMl_Span *span) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_TRUE);
    return s;
}

GTKML_PRIVATE GtkMl_S *new_false(GtkMl_Context *ctx, GtkMl_Span *span) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_FALSE);
    return s;
}

GTKML_PRIVATE GtkMl_S *new_int(GtkMl_Context *ctx, GtkMl_Span *span, int64_t value) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_INT);
    s->value.s_int.value = value;
    return s;
}

GTKML_PRIVATE GtkMl_S *new_float(GtkMl_Context *ctx, GtkMl_Span *span, float value) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_FLOAT);
    s->value.s_float.value = value;
    return s;
}

GTKML_PRIVATE GtkMl_S *new_string(GtkMl_Context *ctx, GtkMl_Span *span, const char *ptr, size_t len) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_STRING);
    s->value.s_string.ptr = ptr;
    s->value.s_string.len = len;
    return s;
}

GTKML_PRIVATE GtkMl_S *new_symbol(GtkMl_Context *ctx, GtkMl_Span *span, const char *ptr, size_t len) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_SYMBOL);
    s->value.s_symbol.ptr = ptr;
    s->value.s_symbol.len = len;
    return s;
}

GTKML_PRIVATE GtkMl_S *new_keyword(GtkMl_Context *ctx, GtkMl_Span *span, const char *ptr, size_t len) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_KEYWORD);
    s->value.s_keyword.ptr = ptr;
    s->value.s_keyword.len = len;
    return s;
}

GTKML_PRIVATE GtkMl_S *new_list(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *car, GtkMl_S *cdr) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_LIST);
    s->value.s_list.car = car;
    s->value.s_list.cdr = cdr;
    return s;
}

GTKML_PRIVATE GtkMl_S *new_map(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *car, GtkMl_S *cdr) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_MAP);
    s->value.s_map.car = car;
    s->value.s_map.cdr = cdr;
    return s;
}

GTKML_PRIVATE GtkMl_S *new_lambda(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *args, GtkMl_S *body) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_LAMBDA);
    s->value.s_lambda.args = args;
    s->value.s_lambda.body = body;
    return s;
}

GTKML_PRIVATE GtkMl_S *new_lightdata(GtkMl_Context *ctx, GtkMl_Span *span, void *data) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_LIGHTDATA);
    s->value.s_lightdata.userdata = data;
    return s;
}

GTKML_PRIVATE GtkMl_S *new_userdata(GtkMl_Context *ctx, GtkMl_Span *span, void *data, void (*del)(GtkMl_Context *ctx, void *)) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_USERDATA);
    s->value.s_userdata.userdata = data;
    s->value.s_userdata.del = del;
    s->value.s_userdata.keep = new_nil(ctx, span);
    return s;
}

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

GTKML_PRIVATE GtkMl_S *parse(GtkMl_Context *ctx, const char **err, GtkMl_Token **tokenv, size_t *tokenc);

GTKML_PRIVATE GtkMl_S *parse_list_rest(GtkMl_Context *ctx, const char **err, GtkMl_Token **tokenv, size_t *tokenc) {
    if (*tokenc == 0) {
        *err = GTKML_ERR_EOF_ERROR;
        return NULL;
    }

    GtkMl_Span span = (*tokenv)[0].span;

    if ((*tokenv)[0].kind == GTKML_TOK_PARENR) {
        span_add(&span, &span, &(*tokenv)[0].span);

        ++*tokenv;
        --*tokenc;

        return new_nil(ctx, &span);
    }

    GtkMl_S *car = parse(ctx, err, tokenv, tokenc);
    GtkMl_S *cdr = parse_list_rest(ctx, err, tokenv, tokenc);
    if (!cdr) {
        return NULL;
    }

    span_add(&span, &span, &cdr->span);

    return new_list(ctx, &span, car, cdr);
}

GTKML_PRIVATE GtkMl_S *parse_list(GtkMl_Context *ctx, const char **err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_PARENL) {
        *err = GTKML_ERR_TOKEN_ERROR;
        return NULL;
    }

    ++*tokenv;
    --*tokenc;

    return parse_list_rest(ctx, err, tokenv, tokenc);
}

GTKML_PRIVATE GtkMl_S *parse_map_rest(GtkMl_Context *ctx, const char **err, GtkMl_Token **tokenv, size_t *tokenc) {
    if (*tokenc == 0) {
        *err = GTKML_ERR_EOF_ERROR;
        return NULL;
    }

    GtkMl_Span span = (*tokenv)[0].span;

    if ((*tokenv)[0].kind == GTKML_TOK_CURLYR) {
        span_add(&span, &span, &(*tokenv)[0].span);

        ++*tokenv;
        --*tokenc;

        return new_nil(ctx, &span);
    }

    GtkMl_S *car = parse(ctx, err, tokenv, tokenc);
    GtkMl_S *cdr = parse_map_rest(ctx, err, tokenv, tokenc);
    if (!cdr) {
        return NULL;
    }

    span_add(&span, &span, &cdr->span);

    return new_map(ctx, &span, car, cdr);
}

GTKML_PRIVATE GtkMl_S *parse_map(GtkMl_Context *ctx, const char **err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_CURLYL) {
        *err = GTKML_ERR_TOKEN_ERROR;
        return NULL;
    }

    ++*tokenv;
    --*tokenc;

    return parse_map_rest(ctx, err, tokenv, tokenc);
}

GTKML_PRIVATE GtkMl_S *parse_int(GtkMl_Context *ctx, const char **err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_INT) {
        *err = GTKML_ERR_TOKEN_ERROR;
        return NULL;
    }
    GtkMl_S *result = new_int(ctx, &(*tokenv)[0].span, (*tokenv)[0].value.intval);
    ++*tokenv;
    --*tokenc;
    return result;
}

GTKML_PRIVATE GtkMl_S *parse_float(GtkMl_Context *ctx, const char **err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_FLOAT) {
        *err = GTKML_ERR_TOKEN_ERROR;
        return NULL;
    }
    GtkMl_S *result = new_float(ctx, &(*tokenv)[0].span, (*tokenv)[0].value.floatval);
    ++*tokenv;
    --*tokenc;
    return result;
}

GTKML_PRIVATE GtkMl_S *parse_string(GtkMl_Context *ctx, const char **err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_STRING) {
        *err = GTKML_ERR_TOKEN_ERROR;
        return NULL;
    }
    size_t len = (*tokenv)[0].span.len - 2;
    char *ptr = malloc(len + 1);
    memcpy(ptr, (*tokenv)[0].span.ptr + 1, len);
    ptr[len] = 0;
    GtkMl_S *result = new_string(ctx, &(*tokenv)[0].span, ptr, len);
    ++*tokenv;
    --*tokenc;
    return result;
}

GTKML_PRIVATE GtkMl_S *parse_symbol(GtkMl_Context *ctx, const char **err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_IDENT) {
        *err = GTKML_ERR_TOKEN_ERROR;
        return NULL;
    }
    GtkMl_S *result = new_symbol(ctx, &(*tokenv)[0].span, (*tokenv)[0].span.ptr, (*tokenv)[0].span.len);
    ++*tokenv;
    --*tokenc;
    return result;
}

GTKML_PRIVATE GtkMl_S *parse_keyword(GtkMl_Context *ctx, const char **err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_KEYWORD) {
        *err = GTKML_ERR_TOKEN_ERROR;
        return NULL;
    }
    GtkMl_S *result = new_keyword(ctx, &(*tokenv)[0].span, (*tokenv)[0].span.ptr + 1, (*tokenv)[0].span.len - 1);
    ++*tokenv;
    --*tokenc;
    return result;
}

GTKML_PRIVATE GtkMl_S *parse(GtkMl_Context *ctx, const char **err, GtkMl_Token **tokenv, size_t *tokenc) {
    switch ((*tokenv)[0].kind) {
    case GTKML_TOK_PARENL:
        return parse_list(ctx, err, tokenv, tokenc);
    case GTKML_TOK_CURLYL:
        return parse_map(ctx, err, tokenv, tokenc);
    case GTKML_TOK_INT:
        return parse_int(ctx, err, tokenv, tokenc);
    case GTKML_TOK_FLOAT:
        return parse_float(ctx, err, tokenv, tokenc);
    case GTKML_TOK_STRING:
        return parse_string(ctx, err, tokenv, tokenc);
    case GTKML_TOK_IDENT:
        return parse_symbol(ctx, err, tokenv, tokenc);
    case GTKML_TOK_KEYWORD:
        return parse_keyword(ctx, err, tokenv, tokenc);
    default:
        *err = GTKML_ERR_TOKEN_ERROR;
        return NULL;
    }
}

GtkMl_S *gtk_ml_load(GtkMl_Context *ctx, char **src, const char **err, const char *file) {
    FILE *stream = fopen(file, "r");
    if (!stream) {
        *err = GTKML_ERR_IO_ERROR;
        return NULL;
    }
    GtkMl_S *result = gtk_ml_loadf(ctx, src, err, stream);
    fclose(stream);
    return result;
}

GtkMl_S *gtk_ml_loadf(GtkMl_Context *ctx, char **src, const char **err, FILE *stream) {
    fseek(stream, SEEK_END, 0l);
    size_t size = ftell(stream);
    fseek(stream, SEEK_SET, 0l);
    *src = malloc(size + 1);
    size_t read = fread(*src, 1, size, stream);
    if (read != size) {
        *err = GTKML_ERR_IO_ERROR;
        return NULL;
    }
    (*src)[size] = 0;
    return gtk_ml_loads(ctx, err, *src);
}

GtkMl_S *gtk_ml_loads(GtkMl_Context *ctx, const char **err, const char *src) {
    GtkMl_Token *tokenv;
    size_t tokenc;
    if (!lex(&tokenv, &tokenc, err, src)) {
        return NULL;
    }
    GtkMl_Token *_tokenv = tokenv;
    gboolean enabled = gtk_ml_disable_gc(ctx);
    GtkMl_S *result = parse(ctx, err, &_tokenv, &tokenc);
    gtk_ml_enable_gc(ctx, enabled);
    free(tokenv);
    return result;
}

void gtk_ml_push(GtkMl_Context *ctx, GtkMl_S *value) {
    if (ctx->stack_len == ctx->stack_cap) {
        ctx->stack_cap *= 2;
        ctx->stack = realloc(ctx->stack, sizeof(GtkMl_S *) * ctx->stack_cap);
    }
    ctx->stack[ctx->stack_len] = value;
    ++ctx->stack_len;
}

GtkMl_S *gtk_ml_pop(GtkMl_Context *ctx) {
    --ctx->stack_len;
    return ctx->stack[ctx->stack_len];
}

void gtk_ml_enter(GtkMl_Context *ctx) {
    gboolean enabled = gtk_ml_disable_gc(ctx);
    ctx->bindings = new_list(ctx, NULL, new_nil(ctx, NULL), ctx->bindings);
    gtk_ml_enable_gc(ctx, enabled);
}

void gtk_ml_leave(GtkMl_Context *ctx) {
    ctx->bindings = gtk_ml_cdr(ctx->bindings);
}

void gtk_ml_define(GtkMl_Context *ctx, GtkMl_S *key, GtkMl_S *value) {
    gboolean enabled = gtk_ml_disable_gc(ctx);
    GtkMl_S *new_context = new_map(ctx, NULL, key, new_map(ctx, NULL, value,
        *ctx->top_scope));
    *ctx->top_scope = new_context;
    gtk_ml_enable_gc(ctx, enabled);
}

void gtk_ml_bind(GtkMl_Context *ctx, GtkMl_S *key, GtkMl_S *value) {
    gboolean enabled = gtk_ml_disable_gc(ctx);
    GtkMl_S *new_context = new_map(ctx, NULL, key, new_map(ctx, NULL, value,
        ctx->bindings->value.s_map.car));
    ctx->bindings->value.s_map.car = new_context;
    gtk_ml_enable_gc(ctx, enabled);
}

GTKML_PRIVATE GtkMl_S *map_find(GtkMl_S *map, GtkMl_S *key) {
    if (map->kind == GTKML_S_NIL) {
        return NULL;
    }

    if (gtk_ml_equal(gtk_ml_car(map), key)) {
        return gtk_ml_cdar(map);
    }

    return map_find(gtk_ml_cddr(map), key);
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
    case GTKML_S_STRING:
    case GTKML_S_KEYWORD:
    case GTKML_S_SYMBOL:
    case GTKML_S_LIGHTDATA:
        break;
    case GTKML_S_USERDATA:
        mark_value(s->value.s_userdata.keep);
        break;
    case GTKML_S_LIST:
    case GTKML_S_MAP:
        mark_value(gtk_ml_car(s));
        mark_value(gtk_ml_cdr(s));
        break;
    case GTKML_S_LAMBDA:
        mark_value(s->value.s_lambda.args);
        mark_value(s->value.s_lambda.body);
        break;
    }
}

GTKML_PRIVATE void mark(GtkMl_Context *ctx) {
    for (size_t i = 0; i < ctx->stack_len; i++) {
        mark_value(ctx->stack[i]);
    }
    mark_value(ctx->bindings);
}

GTKML_PRIVATE void delete(GtkMl_Context *ctx, GtkMl_S *s) {
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
    case GTKML_S_KEYWORD:
    case GTKML_S_SYMBOL:
    case GTKML_S_LIGHTDATA:
        break;
    case GTKML_S_STRING:
        // const cast required
        free((void *) s->value.s_string.ptr);
        break;
    case GTKML_S_USERDATA:
        s->value.s_userdata.del(ctx, s->value.s_userdata.userdata);
        break;
    case GTKML_S_LIST:
    case GTKML_S_MAP:
        delete(ctx, gtk_ml_cdr(s));
        delete(ctx, gtk_ml_car(s));
        break;
    case GTKML_S_LAMBDA:
        delete(ctx, s->value.s_lambda.args);
        delete(ctx, s->value.s_lambda.body);
        break;
    }
    free(s);
    --ctx->n_values;
}

GTKML_PRIVATE void del(GtkMl_Context *ctx, GtkMl_S *s) {
    switch (s->kind) {
    case GTKML_S_NIL:
    case GTKML_S_TRUE:
    case GTKML_S_FALSE:
    case GTKML_S_INT:
    case GTKML_S_FLOAT:
    case GTKML_S_KEYWORD:
    case GTKML_S_SYMBOL:
    case GTKML_S_LIGHTDATA:
    case GTKML_S_LIST:
    case GTKML_S_MAP:
    case GTKML_S_LAMBDA:
        break;
    case GTKML_S_STRING:
        // const cast required
        free((void *) s->value.s_string.ptr);
        break;
    case GTKML_S_USERDATA:
        s->value.s_userdata.del(ctx, s->value.s_userdata.userdata);
        break;
    }
    free(s);
    --ctx->n_values;
}

GTKML_PRIVATE void sweep(GtkMl_Context *ctx) {
    GtkMl_S *prev = ctx->first;
    GtkMl_S *value = prev->next;
    while (value) {
        if (value->flags & GTKML_FLAG_REACHABLE) {
            value->flags &= ~GTKML_FLAG_REACHABLE;
            prev = value;
            value = value->next;
        } else {
            prev->next = value->next;
            delete(ctx, value);
            value = prev->next;
        }
    }
}

// simple mark & sweep gc
void gtk_ml_collect(GtkMl_Context *ctx) {
    if (!ctx->gc_enabled) {
        return;
    }

    size_t n_values = ctx->n_values;
    mark(ctx);
    sweep(ctx);
    ctx->m_values = 2 * n_values;
}

gboolean gtk_ml_disable_gc(GtkMl_Context *ctx) {
    gboolean enabled = ctx->gc_enabled;
    ctx->gc_enabled = 0;
    return enabled;
}

void gtk_ml_enable_gc(GtkMl_Context *ctx, gboolean enabled) {
    ctx->gc_enabled = enabled;
}

gboolean gtk_ml_dumpf(FILE *stream, const char **err, GtkMl_S *expr) {
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
    case GTKML_S_STRING:
        fprintf(stream, "\"%.*s\"", (int) expr->value.s_string.len, expr->value.s_string.ptr);
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
            gtk_ml_dumpf(stream, err, gtk_ml_car(expr));
            expr = gtk_ml_cdr(expr);
            if (expr->kind != GTKML_S_NIL) {
                fprintf(stream, " ");
            }
        }
        fprintf(stream, ")");
        return 1;
    case GTKML_S_MAP:
        fprintf(stream, "{");
        while (expr->kind != GTKML_S_NIL) {
            gtk_ml_dumpf(stream, err, gtk_ml_car(expr));
            expr = gtk_ml_cdr(expr);
            if (expr->kind != GTKML_S_NIL) {
                fprintf(stream, " ");
            }
        }
        fprintf(stream, "}");
        return 1;
    case GTKML_S_LAMBDA:
        fprintf(stream, "(lambda ");
        gtk_ml_dumpf(stream, err, expr->value.s_lambda.args);
        fprintf(stream, " ");
        GtkMl_S *body = expr->value.s_lambda.body;
        while (body->kind != GTKML_S_NIL) {
            gtk_ml_dumpf(stream, err, gtk_ml_car(body));
            body = gtk_ml_cdr(body);
            if (body->kind != GTKML_S_NIL) {
                fprintf(stream, " ");
            }
        }
        fprintf(stream, ")");
        return 1;
    case GTKML_S_LIGHTDATA:
        fprintf(stream, "%p", expr->value.s_lightdata.userdata);
        return 1;
    case GTKML_S_USERDATA:
        fprintf(stream, "%p", expr->value.s_lightdata.userdata);
        return 1;
    default:
        *err = GTKML_ERR_INVALID_SEXPR;
        return 0;
    }
}

char *gtk_ml_dumpsn(char *ptr, size_t n, const char **err, GtkMl_S *expr) {
    (void) ptr;
    (void) n;
    (void) err;
    (void) expr;
    return NULL;
}

char *gtk_ml_dumpsnr(char *ptr, size_t n, const char **err, GtkMl_S *expr) {
    (void) ptr;
    (void) n;
    (void) err;
    (void) expr;
    return NULL;
}

GTKML_PRIVATE GtkMl_S *map_finds(GtkMl_S *map, const char *keyword) {
    if (map->kind == GTKML_S_NIL) {
        return NULL;
    }

    if (gtk_ml_car(map)->kind == GTKML_S_KEYWORD) {
        int cmp = strncmp(
            gtk_ml_car(map)->value.s_keyword.ptr,
            keyword,
            gtk_ml_car(map)->value.s_keyword.len
        );
        if (cmp == 0) {
            return gtk_ml_cdar(map);
        }
    }

    return map_finds(gtk_ml_cddr(map), keyword);
}

GTKML_PRIVATE void activate_lambda(GtkApplication* app, gpointer userdata) {
    (void) app;

    GtkMl_S *args = userdata;
    GtkMl_S *ctx_expr = gtk_ml_car(args);
    GtkMl_S *app_expr = gtk_ml_cdar(args);
    GtkMl_S *lambda_expr = gtk_ml_cddar(args);

    GtkMl_Context *ctx = ctx_expr->value.s_lightdata.userdata;

    gboolean enabled = gtk_ml_disable_gc(ctx);
    const char *err;
    GtkMl_S *result = gtk_ml_call(ctx, &err, lambda_expr, new_list(ctx, NULL, app_expr, new_nil(ctx, NULL)));
    if (result) {
        gtk_ml_push(ctx, result);
    }
    gtk_ml_enable_gc(ctx, enabled);
}

GTKML_PRIVATE GtkMl_S *builtin_lambda(GtkMl_Context *ctx, const char **err, GtkMl_S *expr) {
    GtkMl_S *args = gtk_ml_cdr(expr);

    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = GTKML_ERR_ARITY_ERROR;
        return NULL;
    }

    GtkMl_S *lambda_args = gtk_ml_car(args);
    GtkMl_S *lambda_body = gtk_ml_cdr(args);

    return new_lambda(ctx, &expr->span, lambda_args, lambda_body);
}

GTKML_PRIVATE GtkMl_S *builtin_define(GtkMl_Context *ctx, const char **err, GtkMl_S *expr) {
    GtkMl_S *args = gtk_ml_cdr(expr);

    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = GTKML_ERR_ARITY_ERROR;
        return NULL;
    }

    GtkMl_S *key = gtk_ml_car(args);
    GtkMl_S *value = gtk_ml_cdar(args);

    gtk_ml_define(ctx, key, value);
    return new_nil(ctx, &expr->span);
}

GTKML_PRIVATE GtkMl_S *builtin_application(GtkMl_Context *ctx, const char **err, GtkMl_S *expr) {
    GtkMl_S *args = gtk_ml_cdr(expr);

    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL
            || gtk_ml_cddr(args)->kind == GTKML_S_NIL) {
        *err = GTKML_ERR_ARITY_ERROR;
        return NULL;
    }

    GtkMl_S *id_expr = gtk_ml_exec(ctx, err, gtk_ml_car(args));
    if (!id_expr) {
        return NULL;
    }
    GtkMl_S *flags_expr = gtk_ml_exec(ctx, err, gtk_ml_cdar(args));
    if (!flags_expr) {
        return NULL;
    }

    gboolean enabled = gtk_ml_disable_gc(ctx);
    GtkApplication *app = gtk_application_new(id_expr->value.s_string.ptr, flags_expr->value.s_int.value);
    GtkMl_S *app_expr = new_userdata(ctx, &expr->span, app, gtk_ml_object_unref);

    GtkMl_S *map_expr = gtk_ml_exec(ctx, err, gtk_ml_cddar(args));
    if (!map_expr) {
        return NULL;
    }
    GtkMl_S *activate = map_finds(map_expr, "activate");
    if (activate) {
        GtkMl_S *ctx_expr = new_lightdata(ctx, NULL, ctx);
        GtkMl_S *userdata = new_list(ctx, NULL, ctx_expr, new_list(ctx, NULL, app_expr, new_list(ctx, NULL, activate, new_nil(ctx, NULL))));
        app_expr->value.s_userdata.keep = new_list(ctx, &app_expr->span, userdata, app_expr->value.s_userdata.keep);
        g_signal_connect(app, "activate", G_CALLBACK(activate_lambda), userdata);
    }
    gtk_ml_enable_gc(ctx, enabled);

    return app_expr;
}

GTKML_PRIVATE GtkMl_S *builtin_new_window(GtkMl_Context *ctx, const char **err, GtkMl_S *expr) {
    (void) ctx;

    GtkMl_S *args = gtk_ml_cdr(expr);

    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL
            || gtk_ml_cddr(args)->kind == GTKML_S_NIL
            || gtk_ml_cdddr(args)->kind == GTKML_S_NIL) {
        *err = GTKML_ERR_ARITY_ERROR;
        return NULL;
    }

    GtkMl_S *app_expr = gtk_ml_exec(ctx, err, gtk_ml_car(args));
    if (!app_expr) {
        return NULL;
    }
    GtkMl_S *title_expr = gtk_ml_exec(ctx, err, gtk_ml_cdar(args));
    if (!title_expr) {
        return NULL;
    }
    GtkMl_S *width_expr = gtk_ml_exec(ctx, err, gtk_ml_cddar(args));
    if (!width_expr) {
        return NULL;
    }
    GtkMl_S *height_expr = gtk_ml_exec(ctx, err, gtk_ml_cdddar(args));
    if (!height_expr) {
        return NULL;
    }

    GtkWidget *window = gtk_application_window_new(app_expr->value.s_userdata.userdata);
    gtk_window_set_title(GTK_WINDOW(window), title_expr->value.s_string.ptr);
    gtk_window_set_default_size(GTK_WINDOW(window), width_expr->value.s_int.value, height_expr->value.s_int.value);
    gtk_widget_show_all(window);

    return new_lightdata(ctx, &expr->span, window);
}

GtkMl_S *gtk_ml_exec(GtkMl_Context *ctx, const char **err, GtkMl_S *expr) {
    if (!expr) {
        *err = GTKML_ERR_NULL_ERROR;
        return NULL;
    }

    switch (expr->kind) {
    case GTKML_S_NIL:
    case GTKML_S_TRUE:
    case GTKML_S_FALSE:
    case GTKML_S_INT:
    case GTKML_S_FLOAT:
    case GTKML_S_STRING:
    case GTKML_S_KEYWORD:
    case GTKML_S_LIGHTDATA:
    case GTKML_S_USERDATA:
    case GTKML_S_LAMBDA:
        return expr;
    case GTKML_S_SYMBOL: {
        GtkMl_S *result = gtk_ml_get(ctx, expr);
        if (!result) {
            *err = GTKML_ERR_BINDING_ERROR;
            return NULL;
        }
        return result;
    }
    case GTKML_S_LIST: {
        GtkMl_S *car = gtk_ml_car(expr);
        if (car->kind == GTKML_S_SYMBOL) {
            if (strncmp(car->value.s_symbol.ptr, "lambda", car->value.s_symbol.len) == 0) {
                return builtin_lambda(ctx, err, expr);
            } else if (strncmp(car->value.s_symbol.ptr, "define", car->value.s_symbol.len) == 0) {
                return builtin_define(ctx, err, expr);
            } else if (strncmp(car->value.s_symbol.ptr, "Application", car->value.s_symbol.len) == 0) {
                return builtin_application(ctx, err, expr);
            } else if (strncmp(car->value.s_symbol.ptr, "new-window", car->value.s_symbol.len) == 0) {
                return builtin_new_window(ctx, err, expr);
            }
        }
        *err = GTKML_ERR_UNIMPLEMENTED;
        return NULL;
    }
    case GTKML_S_MAP: {
        GtkMl_S *car = gtk_ml_exec(ctx, err, gtk_ml_car(expr));
        if (!car) {
            return NULL;
        }
        GtkMl_S *cdr = gtk_ml_exec(ctx, err, gtk_ml_cdr(expr));
        if (!cdr) {
            return NULL;
        }
        return new_map(ctx, &expr->span, car, cdr);
    }
    default:
        *err = GTKML_ERR_INVALID_SEXPR;
        return NULL;
    }
}

GtkMl_S *gtk_ml_call(GtkMl_Context *ctx, const char **err, GtkMl_S *lambda, GtkMl_S *args) {
    gtk_ml_enter(ctx);

    GtkMl_S *params = lambda->value.s_lambda.args;

    while (params->kind != GTKML_S_NIL && args->kind != GTKML_S_NIL) {
        gtk_ml_bind(ctx, gtk_ml_car(params), gtk_ml_car(args));
        params = gtk_ml_cdr(params);
        args = gtk_ml_cdr(args);
    }

    if (params->kind != args->kind) {
        *err = GTKML_ERR_ARITY_ERROR;
        return NULL;
    }

    GtkMl_S *result = new_nil(ctx, NULL);
    GtkMl_S *body = lambda->value.s_lambda.body;
    while (body->kind != GTKML_S_NIL) {
        result = gtk_ml_exec(ctx, err, gtk_ml_car(body));
        if (!result) {
            gtk_ml_leave(ctx);
            return NULL;
        }
        body = gtk_ml_cdr(body);
    }

    gtk_ml_leave(ctx);

    return result;
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
    case GTKML_S_STRING:
        return lhs->value.s_string.len == rhs->value.s_string.len && memcmp(lhs->value.s_string.ptr, rhs->value.s_string.ptr, lhs->value.s_string.len) == 0;
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
    case GTKML_S_LIST:
    case GTKML_S_MAP:
        if (gtk_ml_equal(gtk_ml_car(lhs), gtk_ml_car(rhs))) {
            return gtk_ml_equal(gtk_ml_cdr(lhs), gtk_ml_cdr(rhs));
        }
    }

    return 0;
}

void gtk_ml_delete_reference(GtkMl_Context *ctx, void *reference) {
    (void) ctx;
    (void) reference;
}

void gtk_ml_delete_value(GtkMl_Context *ctx, void *s) {
    delete(ctx, s);
}

void gtk_ml_object_unref(GtkMl_Context *ctx, void *obj) {
    (void) ctx;
    g_object_unref(obj);
}
