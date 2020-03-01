#ifndef GTK_ML_H
#define GTK_ML_H 1

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <gtk/gtk.h>

#ifdef __cplusplus
#define GTKML_PUBLIC extern "C"
#else
#define GTKML_PUBLIC extern
#endif

#define GTKML_PRIVATE static

#define GTKML_FLAG_NONE 0x0
#define GTKML_FLAG_REACHABLE 0x1
#define GTKML_FLAG_DELETE 0x2

#define GTKML_GC_THRESHOLD 256

#define GTKML_ERR_IO_ERROR ":error \"unknown io error, check errno\""
#define GTKML_ERR_CHARACTER_ERROR ":error \"unexpected character\""
#define GTKML_ERR_TOKEN_ERROR ":error \"unexpected token\""
#define GTKML_ERR_EOF_ERROR ":error \"unexpected eof\""
#define GTKML_ERR_NULL_ERROR ":error \"null dereference\""
#define GTKML_ERR_INVALID_SEXPR ":error \"invalid s-expression\""
#define GTKML_ERR_ARGUMENT_ERROR ":error \"invalid arguments\""
#define GTKML_ERR_ARITY_ERROR ":error \"invalid argument count\""
#define GTKML_ERR_BINDING_ERROR ":error \"binding not found\""
#define GTKML_ERR_UNIMPLEMENTED ":error \"unimplemented\""

#define gtk_ml_car(x) (x->value.s_list.car)
#define gtk_ml_cdr(x) (x->value.s_list.cdr)
#define gtk_ml_cddr(x) (x->value.s_list.cdr->value.s_list.cdr)
#define gtk_ml_cdddr(x) (x->value.s_list.cdr->value.s_list.cdr->value.s_list.cdr)
#define gtk_ml_cddddr(x) (x->value.s_list.cdr->value.s_list.cdr->value.s_list.cdr->value.s_list.cdr)
#define gtk_ml_cdddddr(x) (x->value.s_list.cdr->value.s_list.cdr->value.s_list.cdr->value.s_list.cdr->value.s_list.cdr)
#define gtk_ml_cdar(x) (x->value.s_list.cdr->value.s_list.car)
#define gtk_ml_cddar(x) (x->value.s_list.cdr->value.s_list.cdr->value.s_list.car)
#define gtk_ml_cdddar(x) (x->value.s_list.cdr->value.s_list.cdr->value.s_list.cdr->value.s_list.car)
#define gtk_ml_cddddar(x) (x->value.s_list.cdr->value.s_list.cdr->value.s_list.cdr->value.s_list.cdr->value.s_list.car)
#define gtk_ml_cdddddar(x) (x->value.s_list.cdr->value.s_list.cdr->value.s_list.cdr->value.s_list.cdr->value.s_list.cdr->value.s_list.car)

typedef struct GtkMl_S GtkMl_S;
typedef struct GtkMl_Context GtkMl_Context;

// a lexical analysis level token tag
typedef enum GtkMl_TokenKind {
    GTKML_TOK_PARENL,
    GTKML_TOK_PARENR,
    GTKML_TOK_CURLYL,
    GTKML_TOK_CURLYR,
    GTKML_TOK_INT,
    GTKML_TOK_FLOAT,
    GTKML_TOK_STRING,
    GTKML_TOK_IDENT,
    GTKML_TOK_KEYWORD,
} GtkMl_TokenKind;

// a source code location
typedef struct GtkMl_Span {
    const char *ptr; // reference
    size_t len;
    int line;
    int col;
} GtkMl_Span;

typedef union GtkMl_TokenValue {
    int64_t intval;
    double floatval;
} GtkMl_TokenValue;

// a lexical analysis level token
typedef struct GtkMl_Token {
    GtkMl_TokenKind kind;
    GtkMl_Span span;
    GtkMl_TokenValue value;
} GtkMl_Token;

// a grammar level s expression tag
typedef enum GtkMl_SKind {
    GTKML_S_NIL,
    GTKML_S_FALSE,
    GTKML_S_TRUE,
    GTKML_S_INT,
    GTKML_S_FLOAT,
    GTKML_S_STRING,
    GTKML_S_SYMBOL,
    GTKML_S_KEYWORD,
    GTKML_S_LIST,
    GTKML_S_MAP,
    GTKML_S_LAMBDA,
    GTKML_S_LIGHTDATA,
    GTKML_S_USERDATA,
} GtkMl_SKind;

// a 64-bit signed integer
typedef struct GtkMl_SInt {
    int64_t value;
} GtkMl_SInt;

// a 64-bit IEEE-754 floating point number
typedef struct GtkMl_SFloat {
    double value;
} GtkMl_SFloat;

// a string like "x", "42", "hello world!"
typedef struct GtkMl_SString {
    const char *ptr; // heap allocated
    size_t len;
} GtkMl_SString;

// a regular symbol like x, forty-two, +, if
typedef struct GtkMl_SSymbol {
    const char *ptr; // reference
    size_t len;
} GtkMl_SSymbol;

// a keyword like :x, :width, :height
typedef struct GtkMl_SKeyword {
    const char *ptr; // reference
    size_t len;
} GtkMl_SKeyword;

// a list like (1 2), (1 (2 3)), (1 "2" 3.0)
typedef struct GtkMl_SList {
    GtkMl_S *car;
    GtkMl_S *cdr;
} GtkMl_SList;

// a map like {:width 640 :height 480}
// TODO(walterpi):
// map currently implement as an assoc list
// will be implemented as a hashmap in the future
typedef struct GtkMl_SMap {
    GtkMl_S *car;
    GtkMl_S *cdr;
} GtkMl_SMap;

typedef struct GtkMl_SLambda {
    GtkMl_S *args;
    GtkMl_S *body;
} GtkMl_SLambda;

typedef struct GtkMl_SLightdata {
    void *userdata; // reference
} GtkMl_SLightdata;

typedef struct GtkMl_SUserdata {
    void *userdata; // reference or heap allocated
    void (*del)(GtkMl_Context *, void *); // userdata deleter
    GtkMl_S *keep;
} GtkMl_SUserdata;

typedef union GtkMl_SUnion {
    GtkMl_SInt s_int;
    GtkMl_SFloat s_float;
    GtkMl_SString s_string;
    GtkMl_SSymbol s_symbol;
    GtkMl_SKeyword s_keyword;
    GtkMl_SList s_list;
    GtkMl_SMap s_map;
    GtkMl_SLambda s_lambda;
    GtkMl_SLightdata s_lightdata;
    GtkMl_SUserdata s_userdata;
} GtkMl_SUnion;

// a grammar level s expression
typedef struct GtkMl_S {
    GtkMl_S *next;
    unsigned int flags;
    GtkMl_SKind kind;
    GtkMl_Span span;
    GtkMl_SUnion value;
} GtkMl_S;

// creates a new context on the heap
// must be deleted with `gtk_ml_del_context`
GTKML_PUBLIC GtkMl_Context *gtk_ml_new_context();
// deletes a context created with `gtk_ml_new_context`
GTKML_PUBLIC void gtk_ml_del_context(GtkMl_Context *ctx);
// loads an expression from a path
GTKML_PUBLIC GtkMl_S *gtk_ml_load(GtkMl_Context *ctx, char **src, const char **err, const char *file);
// loads an expression from a file
GTKML_PUBLIC GtkMl_S *gtk_ml_loadf(GtkMl_Context *ctx, char **src, const char **err, FILE *stream);
// loads an expression from a string
GTKML_PUBLIC GtkMl_S *gtk_ml_loads(GtkMl_Context *ctx, const char **err, const char *src);
// pushes an expression to the stack
GTKML_PUBLIC void gtk_ml_push(GtkMl_Context *ctx, GtkMl_S *value);
// pops an expression from the stack
GTKML_PUBLIC GtkMl_S *gtk_ml_pop(GtkMl_Context *ctx);
// enters a bindings context
GTKML_PUBLIC void gtk_ml_enter(GtkMl_Context *ctx);
// leaves the most recent bindings context
GTKML_PUBLIC void gtk_ml_leave(GtkMl_Context *ctx);
// binds a value to a key in the most recent bindings context
GTKML_PUBLIC void gtk_ml_bind(GtkMl_Context *ctx, GtkMl_S *key, GtkMl_S *value);
// gets a value bound to a key
GTKML_PUBLIC GtkMl_S *gtk_ml_get(GtkMl_Context *ctx, GtkMl_S *key);
// collects garbage
GTKML_PUBLIC void gtk_ml_collect(GtkMl_Context *ctx);
// temporarily disables gc
GTKML_PUBLIC gboolean gtk_ml_disable_gc(GtkMl_Context *ctx);
// reenables gc
GTKML_PUBLIC void gtk_ml_enable_gc(GtkMl_Context *ctx, gboolean enabled);
// dumps a value to a file
GTKML_PUBLIC gboolean gtk_ml_dumpf(FILE *stream, const char **err, GtkMl_S *expr);
// dumps a value to a string
GTKML_PUBLIC char *gtk_ml_dumpsn(char *ptr, size_t n, const char **err, GtkMl_S *expr);
// dumps a value to a string and reallocates if necessary
GTKML_PUBLIC char *gtk_ml_dumpsnr(char *ptr, size_t n, const char **err, GtkMl_S *expr);
// evaluates an expression
GTKML_PUBLIC GtkMl_S *gtk_ml_exec(GtkMl_Context *ctx, const char **err, GtkMl_S *expr);
// calls a lambda expression with arguments
GTKML_PUBLIC GtkMl_S *gtk_ml_call(GtkMl_Context *ctx, const char **err, GtkMl_S *lambda, GtkMl_S *args);
// compares two values for equality
GTKML_PUBLIC gboolean gtk_ml_equal(GtkMl_S *lhs, GtkMl_S *rhs);
GTKML_PUBLIC void gtk_ml_delete_reference(GtkMl_Context *ctx, void *);
GTKML_PUBLIC void gtk_ml_delete_value(GtkMl_Context *ctx, void *);

#endif /* ifndef GTK_ML_H */

// vim: set ft=c :
