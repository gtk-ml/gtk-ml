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

#define GTKML_VM_STACK 16 * 1024 * 1024

#define GTKML_F_HALT 0x10
#define GTKML_F_ZERO 0x8
#define GTKML_F_SIGN 0x4
#define GTKML_F_OVERFLOW 0x2
#define GTKML_F_CARRY 0x1
#define GTKML_F_NONE 0x0

#define GTKML_I_ARITH 0x1
#define GTKML_I_IMM 0x2
#define GTKML_I_BR 0x3
#define GTKML_EI_IMM 0xa
#define GTKML_EI_BR 0xb
#define GTKML_I_EXTENDED 0x8

#define GTKML_IA_NOP 0x0
#define GTKML_IA_HALT 0x1
#define GTKML_IA_MOVE 0x2
#define GTKML_IA_INTEGER_ADD 0x4
#define GTKML_IA_INTEGER_SUBTRACT 0x5
#define GTKML_IA_INTEGER_SIGNED_MULTIPLY 0x6
#define GTKML_IA_INTEGER_UNSIGNED_MULTIPLY 0x7
#define GTKML_IA_INTEGER_SIGNED_DIVIDE 0x8
#define GTKML_IA_INTEGER_UNSIGNED_DIVIDE 0x9
#define GTKML_IA_INTEGER_SIGNED_MODULO 0xa
#define GTKML_IA_INTEGER_UNSIGNED_MODULO 0xb
#define GTKML_IA_FLOAT_ADD 0x14
#define GTKML_IA_FLOAT_SUBTRACT 0x15
#define GTKML_IA_FLOAT_MULTIPLY 0x16
#define GTKML_IA_FLOAT_DIVIDE 0x18
#define GTKML_IA_FLOAT_MODULO 0x19
#define GTKML_IA_BIT_AND 0x20
#define GTKML_IA_BIT_OR 0x21
#define GTKML_IA_BIT_XOR 0x22
#define GTKML_IA_BIT_NAND 0x24
#define GTKML_IA_BIT_NOR 0x25
#define GTKML_IA_BIT_XNOR 0x26
#define GTKML_IA_BRANCH_REGISTER_ABSOLUTE 0x40
#define GTKML_IA_BRANCH_REGISTER_RELATIVE 0x41

#define GTKML_EII_PUSH_EXT_IMM 0x0
#define GTKML_EII_POP 0x1

#define GTKML_EIBR_CALL_FFI 0x0

#define GTKML_SIA_NOP "NOP"
#define GTKML_SIA_HALT "HALT"
#define GTKML_SIA_MOVE "MOVE"
#define GTKML_SIA_INTEGER_ADD "INTEGER_ADD"
#define GTKML_SIA_INTEGER_SUBTRACT "INTEGER_SUBTRACT"
#define GTKML_SIA_INTEGER_SIGNED_MULTIPLY "INTEGER_SIGNED_MULTIPLY"
#define GTKML_SIA_INTEGER_UNSIGNED_MULTIPLY "INTEGER_UNSIGNED_MULTIPLY"
#define GTKML_SIA_INTEGER_SIGNED_DIVIDE "INTEGER_SIGNED_DIVIDE"
#define GTKML_SIA_INTEGER_UNSIGNED_DIVIDE "INTEGER_UNSIGNED_DIVIDE"
#define GTKML_SIA_INTEGER_SIGNED_MODULO "INTEGER_SIGNED_MODULO"
#define GTKML_SIA_INTEGER_UNSIGNED_MODULO "INTEGER_UNSIGNED_MODULO"
#define GTKML_SIA_FLOAT_ADD "FLOAT_ADD"
#define GTKML_SIA_FLOAT_SUBTRACT "FLOAT_SUBTRACT"
#define GTKML_SIA_FLOAT_MULTIPLY "FLOAT_MULTIPLY"
#define GTKML_SIA_FLOAT_DIVIDE "FLOAT_DIVIDE"
#define GTKML_SIA_FLOAT_MODULO "FLOAT_MODULO"
#define GTKML_SIA_BIT_AND "BIT_AND"
#define GTKML_SIA_BIT_OR "BIT_OR"
#define GTKML_SIA_BIT_XOR "BIT_XOR"
#define GTKML_SIA_BIT_NAND "BIT_NAND"
#define GTKML_SIA_BIT_NOR "BIT_NOR"
#define GTKML_SIA_BIT_XNOR "BIT_XNOR"
#define GTKML_SIA_BRANCH_REGISTER_ABSOLUTE "BRANCH_REGISTER_ABSOLUTE"
#define GTKML_SIA_BRANCH_REGISTER_RELATIVE "BRANCH_REGISTER_RELATIVE"

#define GTKML_SEII_PUSH_EXT_IMM "PUSH_EXT_IMM"
#define GTKML_SEII_POP "POP"

#define GTKML_SEIBR_CALL_FFI "CALL_FFI"

#define GTKML_R_ZERO 0
#define GTKML_R_FLAGS 251
#define GTKML_R_SP 252
#define GTKML_R_BP 253
#define GTKML_R_LR 254
#define GTKML_R_PC 255
#define GTKML_REGISTER_COUNT 256

#define GTKML_ERR_IO_ERROR ":error \"unknown io error, check errno\""
#define GTKML_ERR_CHARACTER_ERROR ":error \"unexpected character\""
#define GTKML_ERR_TOKEN_ERROR ":error \"unexpected token\""
#define GTKML_ERR_EOF_ERROR ":error \"unexpected eof\""
#define GTKML_ERR_NULL_ERROR ":error \"null dereference\""
#define GTKML_ERR_INVALID_SEXPR ":error \"invalid s-expression\""
#define GTKML_ERR_ARGUMENT_ERROR ":error \"invalid arguments\""
#define GTKML_ERR_TYPE_ERROR ":error \"invalid type for expression\""
#define GTKML_ERR_ARITY_ERROR ":error \"invalid argument count\""
#define GTKML_ERR_BINDING_ERROR ":error \"binding not found\""
#define GTKML_ERR_VARIADIC_ERROR ":error \"free-standing variadic expression\""
#define GTKML_ERR_CATEGORY_ERROR ":error \"invalid category\""
#define GTKML_ERR_OPCODE_ERROR ":error \"invalid opcode\""
#define GTKML_ERR_PROGRAM_ERROR ":error \"not a program\""
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
typedef struct GtkMl_Vm GtkMl_Vm;
typedef union GtkMl_Register GtkMl_Register;

// a lexical analysis level token tag
typedef enum GtkMl_TokenKind {
    GTKML_TOK_DOT,
    GTKML_TOK_ELLIPSIS,
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
    GTKML_S_VARIADIC,
    GTKML_S_LAMBDA,
    GTKML_S_MACRO,
    GTKML_S_FFI,
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

typedef struct GtkMl_SVariadic {
    GtkMl_S *expr;
} GtkMl_SVariadic;

// a closure that evaluates its arguments
typedef struct GtkMl_SLambda {
    GtkMl_S *args;
    GtkMl_S *body;
    GtkMl_S *capture;
} GtkMl_SLambda;

// a closure that doesn't evaluate its arguments
typedef struct GtkMl_SMacro {
    GtkMl_S *args;
    GtkMl_S *body;
    GtkMl_S *capture;
} GtkMl_SMacro;

typedef struct GtkMl_SFfi {
    GtkMl_S *(*function)(GtkMl_Context *, const char **, GtkMl_S *);
} GtkMl_SFfi;

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
    GtkMl_SVariadic s_var;
    GtkMl_SLambda s_lambda;
    GtkMl_SMacro s_macro;
    GtkMl_SFfi s_ffi;
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

typedef struct GtkMl_InstrGen {
    unsigned long cond : 4;
    unsigned long category : 4;
    unsigned long _pad : 56;
} GtkMl_InstrGen;

typedef struct GtkMl_InstrArith {
    unsigned long cond : 4;
    unsigned long category : 4; // must be 0001
    unsigned long opcode : 8;
    unsigned long rd : 8;
    unsigned long rs : 8;
    unsigned long ra : 8;
    unsigned long _pad : 24;
} GtkMl_InstrArith;

typedef struct GtkMl_InstrImm {
    unsigned long cond : 4;
    unsigned long category : 4; // must be 0010 or 1010
    unsigned long opcode : 8;
    unsigned long rd : 8;
    unsigned long rs : 8;
    unsigned long imm : 32;
} GtkMl_InstrImm;

typedef struct GtkMl_InstrBr {
    unsigned long cond : 4;
    unsigned long category : 4; // must be 0011 or 1011
    unsigned long opcode : 8;
    unsigned long imm : 48;
} GtkMl_InstrBr;

typedef union GtkMl_Instruction {
    GtkMl_InstrGen gen;
    GtkMl_InstrArith arith;
    GtkMl_InstrImm imm;
    GtkMl_InstrBr br;
    uint64_t imm64;
} GtkMl_Instruction;

union GtkMl_Register {
    GtkMl_S *s;
    GtkMl_Register *p;
    GtkMl_Instruction *b;
    uint64_t u;
    int64_t i;
    double f;
};

// creates a new context on the heap
// must be deleted with `gtk_ml_del_context`
GTKML_PUBLIC GtkMl_Context *gtk_ml_new_context();
// deletes a context created with `gtk_ml_new_context`
GTKML_PUBLIC void gtk_ml_del_context(GtkMl_Context *ctx);
// loads an executable program into the context
GTKML_PUBLIC void gtk_ml_load_program(GtkMl_Context *ctx, GtkMl_Instruction *exec, size_t n_exec);
// runs a program previously loaded with `gtk_ml_load_program`
GTKML_PUBLIC gboolean gtk_ml_run_program(GtkMl_Context *ctx, const char **err);

// creates a new virtual machine on the heap
// must be deleted with `gtk_ml_del_vm`
GTKML_PUBLIC GtkMl_Vm *gtk_ml_new_vm(GtkMl_Context *ctx);
// deletes a virtual machine created with `gtk_ml_new_vm`
GTKML_PUBLIC void gtk_ml_del_vm(GtkMl_Vm *vm);

// loads an expression from a path
GTKML_PUBLIC GtkMl_S *gtk_ml_load(GtkMl_Context *ctx, char **src, const char **err, const char *file);
// loads an expression from a file
GTKML_PUBLIC GtkMl_S *gtk_ml_loadf(GtkMl_Context *ctx, char **src, const char **err, FILE *stream);
// loads an expression from a string
GTKML_PUBLIC GtkMl_S *gtk_ml_loads(GtkMl_Context *ctx, const char **err, const char *src);

// compile a lambda expression to bytecode
GTKML_PUBLIC GtkMl_Instruction *gtk_ml_compile(GtkMl_Context *ctx, const char **err, size_t *n_exec, GtkMl_S *lambda);

// pushes an expression to the stack
GTKML_PUBLIC void gtk_ml_push(GtkMl_Context *ctx, GtkMl_S *value);
// pops an expression from the stack
GTKML_PUBLIC GtkMl_S *gtk_ml_pop(GtkMl_Context *ctx);
// peeks an expression at the top of the stack
GTKML_PUBLIC GtkMl_S *gtk_ml_peek(GtkMl_Context *ctx);
// enters a bindings context
GTKML_PUBLIC void gtk_ml_enter(GtkMl_Context *ctx);
// leaves the most recent bindings context
GTKML_PUBLIC void gtk_ml_leave(GtkMl_Context *ctx);
// binds a value to a key in the top bindings context
GTKML_PUBLIC void gtk_ml_define(GtkMl_Context *ctx, GtkMl_S *key, GtkMl_S *value);
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
// calls a lambda, a macro or an ffi expression with arguments
GTKML_PUBLIC GtkMl_S *gtk_ml_call(GtkMl_Context *ctx, const char **err, GtkMl_S *function, GtkMl_S *expr);
// compares two values for equality
GTKML_PUBLIC gboolean gtk_ml_equal(GtkMl_S *lhs, GtkMl_S *rhs);

GTKML_PUBLIC GtkMl_S *gtk_ml_nil(GtkMl_Context *ctx);

GTKML_PUBLIC void gtk_ml_delete_reference(GtkMl_Context *ctx, void *);
GTKML_PUBLIC void gtk_ml_delete_value(GtkMl_Context *ctx, void *);

#endif /* ifndef GTK_ML_H */

// vim: set ft=c :
