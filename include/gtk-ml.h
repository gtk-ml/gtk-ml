#ifndef GTK_ML_H
#define GTK_ML_H 1

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#ifdef GTKML_ENABLE_GTK
#include <gtk/gtk.h>
#else
typedef int gboolean;
#endif /* GTKML_ENABLE_GTK */

#ifdef __cplusplus
#define GTKML_PUBLIC extern "C"
#else
#define GTKML_PUBLIC extern
#endif

#if defined(__GNUC__) || defined(__clang__)
#define GTKML_MUST_USE __attribute__((warn_unused_result))
#else
#define GTKML_MUST_USE
#endif

#ifndef GTKML_INTWIDTH_DEFINED

#if defined(__LONG_WIDTH__)
#define GTKML_LONG_WIDTH __LONG_WIDTH__
#elif defined(LONG_WIDTH)
#define GTKML_LONG_WIDTH LONG_WIDTH
#else
#define GTKML_LONG_WIDTH 32
#endif /* defined(__LONG_WIDTH__) */

#if defined(__LONG_LONG_WIDTH__)
#define GTKML_LLONG_WIDTH __LONG_LONG_WIDTH__
#elif defined(LLONG_WIDTH)
#define GTKML_LLONG_WIDTH LLONG_WIDTH
#else
#define GTKML_LLONG_WIDTH 64
#endif /* defined(__LONG_LONG_WIDTH__) */

#endif /* GTKML_INTWIDTH_DEFINED */

#if GTKML_LONG_WIDTH == 64
#define GTKML_FMT_64 "l"
#elif GTKML_LLONG_WIDTH == 64
#define GTKML_FMT_64 "ll"
#else
#error "gtk-ml: i don't know a type that's 64-bit long"
#endif /* GTKML_LONG_WIDTH == 64 */

#define GTKML_PRIVATE static

#define GTKML_VERSION "libgtkml ver. 0.0.0"

#define GTKML_FLAG_NONE 0x0
#define GTKML_FLAG_REACHABLE 0x1
#define GTKML_FLAG_DELETE 0x2

#define GTKML_GC_COUNT_THRESHOLD 1024
#define GTKML_GC_STEP_THRESHOLD 256

#define GTKML_GC_STACK (GTKML_STACK_SIZE)
#define GTKML_VM_STACK (GTKML_STACK_SIZE)
#define GTKML_VM_CALL_STACK (GTKML_STACK_SIZE)

#define GTKML_F_TOPCALL 0x20
#define GTKML_F_HALT 0x10
#define GTKML_F_EQUAL 0x8
#define GTKML_F_NEQUAL 0x4
#define GTKML_F_OVERFLOW 0x2
#define GTKML_F_CARRY 0x1
#define GTKML_F_NONE 0x0
#define GTKML_F_GENERIC (GTKML_F_EQUAL | GTKML_F_NEQUAL | GTKML_F_OVERFLOW | GTKML_F_CARRY)

#define GTKML_CORE_LOAD 0x0
#ifdef GTKML_ENABLE_GTK
#define GTKML_CORE_APPLICATION 0x1
#define GTKML_CORE_NEW_WINDOW 0x2
#endif /* GTKML_ENABLE_GTK */
#define GTKML_CORE_ERROR 0x3
#define GTKML_CORE_DBG 0x4
#define GTKML_CORE_STRING_TO_SYMBOL 0x5
#define GTKML_CORE_COMPILE_EXPR 0x100
#define GTKML_CORE_EMIT_BYTECODE 0x101
#define GTKML_CORE_BIND_SYMBOL 0x102
#define GTKML_CORE_EXPORT_SYMBOL 0x103
#define GTKML_CORE_APPEND_BASIC_BLOCK 0x104
#define GTKML_CORE_GLOBAL_COUNTER 0x105
#define GTKML_CORE_BASIC_BLOCK_NAME 0x106
#ifdef GTKML_ENABLE_POSIX
#define GTKML_CORE_DBG_RUN 0x200
#define GTKML_CORE_DBG_CONT 0x201
#define GTKML_CORE_DBG_STEP 0x202
#define GTKML_CORE_DBG_DISASM 0x203
#define GTKML_CORE_DBG_STACK 0x204
#define GTKML_CORE_DBG_BACKTRACE 0x205
#endif /* GTKML_ENABLE_POSIX */
#ifdef GTKML_ENABLE_WEB
#define GTKML_CORE_WEB_LOG 0x301
#define GTKML_CORE_WEB_NEW_FLOAT32_ARRAY 0x302
#define GTKML_CORE_WEBGL_CREATE_SHADER 0x303
#define GTKML_CORE_WEBGL_SHADER_SOURCE 0x304
#define GTKML_CORE_WEBGL_COMPILE_SHADER 0x305
#define GTKML_CORE_WEBGL_GET_SHADER_PARAMETER 0x306
#define GTKML_CORE_WEBGL_GET_SHADER_INFO_LOG 0x307
#define GTKML_CORE_WEBGL_DELETE_SHADER 0x308
#define GTKML_CORE_WEBGL_CREATE_PROGRAM 0x309
#define GTKML_CORE_WEBGL_ATTACH_SHADER 0x30a
#define GTKML_CORE_WEBGL_LINK_PROGRAM 0x30b
#define GTKML_CORE_WEBGL_GET_PROGRAM_PARAMETER 0x30c
#define GTKML_CORE_WEBGL_GET_PROGRAM_INFO_LOG 0x30d
#define GTKML_CORE_WEBGL_DELETE_PROGRAM 0x30e
#define GTKML_CORE_WEBGL_GET_UNIFORM_LOCATION 0x30f
#define GTKML_CORE_WEBGL_GET_ATTRIB_LOCATION 0x310
#define GTKML_CORE_WEBGL_CREATE_BUFFER 0x311
#define GTKML_CORE_WEBGL_BIND_BUFFER 0x312
#define GTKML_CORE_WEBGL_BUFFER_DATA 0x313
#define GTKML_CORE_WEBGL_CREATE_VERTEX_ARRAY 0x314
#define GTKML_CORE_WEBGL_BIND_VERTEX_ARRAY 0x315
#define GTKML_CORE_WEBGL_ENABLE_VERTEX_ATTRIB_ARRAY 0x316
#define GTKML_CORE_WEBGL_VERTEX_ATTRIB_POINTER 0x317
#define GTKML_CORE_WEBGL_VIEWPORT 0x318
#define GTKML_CORE_WEBGL_CLEAR_COLOR 0x319
#define GTKML_CORE_WEBGL_CLEAR 0x31a
#define GTKML_CORE_WEBGL_USE_PROGRAM 0x31b
#define GTKML_CORE_WEBGL_UNIFORM_4F 0x31c
#define GTKML_CORE_WEBGL_DRAW_ARRAYS 0x31d
#define GTKML_CORE_WEBGL_CANVAS_WIDTH 0x31e
#define GTKML_CORE_WEBGL_CANVAS_HEIGHT 0x31f
#endif /* GTKML_ENABLE_WEB */

#define GTKML_I_GENERIC 0x0
#define GTKML_I_RESERVED 0x1
#define GTKML_I_EXTERN 0x2
#define GTKML_I_EXPORT 0x4
#define GTKML_I_LOCAL 0x8
#define GTKML_I_EXTERN_LOCAL (GTKML_I_EXTERN | GTKML_I_LOCAL)
#define GTKML_I_EXPORT_LOCAL (GTKML_I_EXPORT | GTKML_I_LOCAL)

#define GTKML_EI_EXPORT_FLAG_NONE 0x0
#define GTKML_EI_EXPORT_FLAG_LOCAL 0x1

typedef enum GtkMl_Cmp {
    GTKML_CMP_EQUAL,
    GTKML_CMP_NOT_EQUAL,
    GTKML_CMP_LESS,
    GTKML_CMP_GREATER,
    GTKML_CMP_LESS_EQUAL,
    GTKML_CMP_GREATER_EQUAL,
} GtkMl_Cmp;

typedef enum GtkMl_Opcode {
    GTKML_I_NOP,
    GTKML_I_ADD,
    GTKML_I_SUBTRACT,
    GTKML_I_SIGNED_MULTIPLY,
    GTKML_I_UNSIGNED_MULTIPLY,
    GTKML_I_SIGNED_DIVIDE,
    GTKML_I_UNSIGNED_DIVIDE,
    GTKML_I_SIGNED_MODULO,
    GTKML_I_UNSIGNED_MODULO,
    GTKML_I_BIT_AND,
    GTKML_I_BIT_OR,
    GTKML_I_BIT_XOR,
    GTKML_I_BIT_NAND,
    GTKML_I_BIT_NOR,
    GTKML_I_BIT_XNOR,
    GTKML_I_CMP_IMM,
    GTKML_I_CAR,
    GTKML_I_CDR,
    GTKML_I_BIND,
    GTKML_I_ENTER_BIND_ARGS,
    GTKML_I_DEFINE,
    GTKML_I_LIST,
    GTKML_I_ENTER,
    GTKML_I_LEAVE,
    GTKML_I_UNWRAP,
    GTKML_I_TYPEOF,
    GTKML_I_TO_SOBJ,
    GTKML_I_TO_PRIM,

    GTKML_I_PUSH_IMM,
    GTKML_I_POP,
    GTKML_I_SETF_IMM,
    GTKML_I_POPF,
    GTKML_I_GET_IMM,
    GTKML_I_LOCAL_IMM,
    GTKML_I_LIST_IMM,
    GTKML_I_MAP_IMM,
    GTKML_I_SET_IMM,
    GTKML_I_ARRAY_IMM,
    GTKML_I_SETMM_IMM,
    GTKML_I_GETMM_IMM,
    GTKML_I_VAR,
    GTKML_I_GETVAR,
    GTKML_I_ASSIGNVAR,
    GTKML_I_LEN,
    GTKML_I_ARRAY_INDEX,
    GTKML_I_ARRAY_PUSH,
    GTKML_I_ARRAY_POP,
    GTKML_I_ARRAY_CONCAT,
    GTKML_I_MAP_GET,
    GTKML_I_MAP_INSERT,
    GTKML_I_MAP_DELETE,
    GTKML_I_MAP_CONCAT,
    GTKML_I_SET_CONTAINS,
    GTKML_I_SET_INSERT,
    GTKML_I_SET_DELETE,
    GTKML_I_SET_CONCAT,

    GTKML_I_CALL,
    GTKML_I_LEAVE_RET,
    GTKML_I_CALL_CORE,
    GTKML_I_BRANCH_ABSOLUTE,
    GTKML_I_BRANCH_RELATIVE,
} GtkMl_Opcode;

#define GTKML_SI_NOP "nop"
#define GTKML_SI_ADD "add"
#define GTKML_SI_SUBTRACT "subtract"
#define GTKML_SI_SIGNED_MULTIPLY "signed-multiply"
#define GTKML_SI_UNSIGNED_MULTIPLY "unsigned-multiply"
#define GTKML_SI_SIGNED_DIVIDE "signed-divide"
#define GTKML_SI_UNSIGNED_DIVIDE "unsigned-divide"
#define GTKML_SI_SIGNED_MODULO "signed-modulo"
#define GTKML_SI_UNSIGNED_MODULO "unsigned-modulo"
#define GTKML_SI_BIT_AND "bit-and"
#define GTKML_SI_BIT_OR "bit-or"
#define GTKML_SI_BIT_XOR "bit-xor"
#define GTKML_SI_BIT_NAND "bit-nand"
#define GTKML_SI_BIT_NOR "bit-nor"
#define GTKML_SI_BIT_XNOR "bit-xnor"
#define GTKML_SI_CMP_IMM "cmp-imm"
#define GTKML_SI_CAR "car"
#define GTKML_SI_CDR "cdr"
#define GTKML_SI_BIND "bind"
#define GTKML_SI_ENTER_BIND_ARGS "enter-bind-args"
#define GTKML_SI_DEFINE "define"
#define GTKML_SI_LIST "list"
#define GTKML_SI_ENTER "enter"
#define GTKML_SI_LEAVE "leave"
#define GTKML_SI_UNWRAP "unwrap"
#define GTKML_SI_TYPEOF "typeof"
#define GTKML_SI_TO_SOBJ "->sobj"
#define GTKML_SI_TO_PRIM "->prim"

#define GTKML_SI_PUSH_IMM_EXTERN "push-imm extern"
#define GTKML_SI_PUSH_IMM "push-imm"
#define GTKML_SI_POP "pop"
#define GTKML_SI_SETF_IMM "setf-imm"
#define GTKML_SI_POPF "popf"
#define GTKML_SI_GET_IMM "get-imm"
#define GTKML_SI_LOCAL_IMM "local-imm"
#define GTKML_SI_LIST_IMM "list-imm"
#define GTKML_SI_MAP_IMM "map-imm"
#define GTKML_SI_SET_IMM "set-imm"
#define GTKML_SI_ARRAY_IMM "array-imm"
#define GTKML_SI_SETMM_IMM "setmm-imm"
#define GTKML_SI_GETMM_IMM "getmm-imm"
#define GTKML_SI_VAR "var"
#define GTKML_SI_GETVAR "getvar"
#define GTKML_SI_ASSIGNVAR "assignvar"
#define GTKML_SI_LEN "len"
#define GTKML_SI_ARRAY_INDEX "array-index"
#define GTKML_SI_ARRAY_PUSH "array-push"
#define GTKML_SI_ARRAY_CONCAT "array-concat"
#define GTKML_SI_ARRAY_POP "array-pop"
#define GTKML_SI_MAP_GET "map-get"
#define GTKML_SI_MAP_INSERT "map-insert"
#define GTKML_SI_MAP_DELETE "map-delete"
#define GTKML_SI_MAP_CONCAT "map-concat"
#define GTKML_SI_SET_CONTAINS "set-contains"
#define GTKML_SI_SET_INSERT "set-insert"
#define GTKML_SI_SET_DELETE "set-delete"
#define GTKML_SI_SET_CONCAT "set-concat"

#define GTKML_SI_CALL_CORE "call-core"
#define GTKML_SI_CALL "call"
#define GTKML_SI_LEAVE_RET "leave-ret"
#define GTKML_SI_BRANCH_ABSOLUTE "branch-absolute"
#define GTKML_SI_BRANCH_RELATIVE "branch-relative"

#define GTKML_R_ZERO 0
#define GTKML_R_FLAGS 1
#define GTKML_R_BP 3
#define GTKML_R_LR 4
#define GTKML_R_PC 5
#define GTKML_REGISTER_COUNT 6

#define GTKML_ERR_IO_ERROR "unknown io error, check errno"
#define GTKML_ERR_CHARACTER_ERROR "unexpected character"
#define GTKML_ERR_TOKEN_ERROR "unexpected token"
#define GTKML_ERR_ESCAPE_ERROR "invalid escape sequence"
#define GTKML_ERR_EOF_ERROR "unexpected eof"
#define GTKML_ERR_NULL_ERROR "null dereference"
#define GTKML_ERR_SCOPE_ERROR "cannot leave an empty scope"
#define GTKML_ERR_INVALID_SEXPR "invalid s-expression"
#define GTKML_ERR_ARGUMENT_ERROR "invalid arguments"
#define GTKML_ERR_TYPE_ERROR "invalid type for expression"
#define GTKML_ERR_VALUE_ERROR "entity doesn't have a value"
#define GTKML_ERR_TAG_ERROR "unrecognized value tag"
#define GTKML_ERR_INDEX_ERROR "index out of bounds"
#define GTKML_ERR_CONTAINER_ERROR "not a container"
#define GTKML_ERR_STACK_ERROR "stack overflow"
#define GTKML_ERR_CMP_ERROR "invalid comparison enum"
#define GTKML_ERR_BYTECODE_ERROR "unrecognized bytecode keyword"
#define GTKML_ERR_BOOLEAN_ERROR "expected a boolean expression"
#define GTKML_ERR_ARITY_ERROR "invalid argument count"
#define GTKML_ERR_LET_ERROR "let requires and even number of key-value pairs"
#define GTKML_ERR_BINDING_ERROR "binding not found"
#define GTKML_ERR_CONSTANT_ERROR "constant not found"
#define GTKML_ERR_VARARG_ERROR "free-standing vararg expression"
#define GTKML_ERR_UNWRAP_ERROR "unwrap must be the last expression in an application"
#define GTKML_ERR_UNQUOTE_ERROR "free-standing unquote expression"
#define GTKML_ERR_CATEGORY_ERROR "invalid category"
#define GTKML_ERR_OPCODE_ERROR "invalid opcode"
#define GTKML_ERR_ARITH_ERROR "attempt to perform arithmetic on something not a number"
#define GTKML_ERR_PROGRAM_ERROR "not a program"
#define GTKML_ERR_EXPORT_ERROR "export is not a program or address"
#define GTKML_ERR_LINKAGE_ERROR "symbol not found while linking"
#define GTKML_ERR_SER_ERROR "serialization error"
#define GTKML_ERR_DESER_ERROR "deserialization error"
#define GTKML_ERR_DEBUGGER_ERROR "process is not a debugger"
#define GTKML_ERR_UNIMPLEMENTED "unimplemented"

#define gtk_ml_car(x) ((x)->value.s_list.car)
#define gtk_ml_cdr(x) ((x)->value.s_list.cdr)
#define gtk_ml_cddr(x) ((x)->value.s_list.cdr->value.s_list.cdr)
#define gtk_ml_cdddr(x) ((x)->value.s_list.cdr->value.s_list.cdr->value.s_list.cdr)
#define gtk_ml_cddddr(x) ((x)->value.s_list.cdr->value.s_list.cdr->value.s_list.cdr->value.s_list.cdr)
#define gtk_ml_cdddddr(x) ((x)->value.s_list.cdr->value.s_list.cdr->value.s_list.cdr->value.s_list.cdr->value.s_list.cdr)
#define gtk_ml_cdar(x) ((x)->value.s_list.cdr->value.s_list.car)
#define gtk_ml_cddar(x) ((x)->value.s_list.cdr->value.s_list.cdr->value.s_list.car)
#define gtk_ml_cdddar(x) ((x)->value.s_list.cdr->value.s_list.cdr->value.s_list.cdr->value.s_list.car)
#define gtk_ml_cddddar(x) ((x)->value.s_list.cdr->value.s_list.cdr->value.s_list.cdr->value.s_list.cdr->value.s_list.car)
#define gtk_ml_cdddddar(x) ((x)->value.s_list.cdr->value.s_list.cdr->value.s_list.cdr->value.s_list.cdr->value.s_list.cdr->value.s_list.car)

typedef union GtkMl_Value GtkMl_Value;
typedef struct GtkMl_TaggedValue GtkMl_TaggedValue;
typedef struct GtkMl_S GtkMl_S;
typedef struct GtkMl_S *GtkMl_SObj;
typedef struct GtkMl_Context GtkMl_Context;
typedef struct GtkMl_Gc GtkMl_Gc;
typedef struct GtkMl_Vm GtkMl_Vm;
typedef struct GtkMl_Builder GtkMl_Builder;
typedef struct GtkMl_Program GtkMl_Program;
typedef uint64_t uint48_t;
typedef uint48_t GtkMl_Data;
typedef uint48_t GtkMl_Static;
typedef uint32_t GtkMl_Hash;

typedef enum GtkMl_Stage {
    GTKML_STAGE_INTR,
    GTKML_STAGE_MACRO,
    GTKML_STAGE_RUNTIME,
} GtkMl_Stage;

// a lexical analysis level token tag
typedef enum GtkMl_TokenKind {
    GTKML_TOK_DOT,
    GTKML_TOK_ELLIPSIS,
    GTKML_TOK_TICK,
    GTKML_TOK_BACKTICK,
    GTKML_TOK_COMMA,
    GTKML_TOK_POUND,
    GTKML_TOK_AT,
    GTKML_TOK_PARENL,
    GTKML_TOK_PARENR,
    GTKML_TOK_SQUAREL,
    GTKML_TOK_SQUARER,
    GTKML_TOK_CURLYL,
    GTKML_TOK_CURLYR,
    GTKML_TOK_INT,
    GTKML_TOK_FLOAT,
    GTKML_TOK_CHAR,
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

typedef struct GtkMl_Hasher {
    void (*start)(GtkMl_Hash *);
    gboolean (*update)(GtkMl_Hash *, GtkMl_TaggedValue);
    void (*finish)(GtkMl_Hash *);
    gboolean (*equal)(GtkMl_TaggedValue, GtkMl_TaggedValue);
} GtkMl_Hasher;

typedef struct GtkMl_HashTrieNode GtkMl_HashTrieNode;

typedef struct GtkMl_HashTrie {
    GtkMl_Hasher *hasher;
    GtkMl_HashTrieNode *root;
    size_t len;
} GtkMl_HashTrie;

typedef struct GtkMl_HashSetNode GtkMl_HashSetNode;

typedef struct GtkMl_HashSet {
    GtkMl_Hasher *hasher;
    GtkMl_HashSetNode *root;
    size_t len;
} GtkMl_HashSet;

typedef struct GtkMl_ArrayNode GtkMl_ArrayNode;

typedef struct GtkMl_Array {
    GtkMl_ArrayNode *root;
    size_t len;
    int string;
} GtkMl_Array;

// a grammar level s expression tag
typedef enum GtkMl_SKind {
    GTKML_S_NIL,
    GTKML_S_FALSE,
    GTKML_S_TRUE,
    GTKML_S_INT,
    GTKML_S_FLOAT,
    GTKML_S_CHAR,
    GTKML_S_SYMBOL,
    GTKML_S_KEYWORD,
    GTKML_S_LIST,
    GTKML_S_MAP,
    GTKML_S_SET,
    GTKML_S_ARRAY,
    GTKML_S_VAR,
    GTKML_S_VARARG,
    GTKML_S_QUOTE,
    GTKML_S_QUASIQUOTE,
    GTKML_S_UNQUOTE,
    GTKML_S_LAMBDA,
    GTKML_S_PROGRAM,
    GTKML_S_ADDRESS,
    GTKML_S_MACRO,
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

// a character like \x, \4, \newline, \space, \x30, \u0030
typedef struct GtkMl_SChar {
    uint32_t value;
} GtkMl_SChar;

// a regular symbol like x, forty-two, +, if
typedef struct GtkMl_SSymbol {
    gboolean owned;
    const char *ptr; // reference or heap allocated
    size_t len;
} GtkMl_SSymbol;

// a keyword like :x, :width, :height
typedef struct GtkMl_SKeyword {
    gboolean owned;
    const char *ptr; // reference or heap allocated
    size_t len;
} GtkMl_SKeyword;

// a list like (1 2), (1 (2 3)), (1 "2" 3.0)
typedef struct GtkMl_SList {
    GtkMl_SObj car;
    GtkMl_SObj cdr;
} GtkMl_SList;

// a map like {:width 640 :height 480}
typedef struct GtkMl_SMap {
    GtkMl_HashTrie map;
    GtkMl_SObj metamap;
} GtkMl_SMap;

// a set like #{:a :b :c}
typedef struct GtkMl_SSet {
    GtkMl_HashSet set;
} GtkMl_SSet;

// an array like [1 2 3]
typedef struct GtkMl_SArray {
    GtkMl_Array array;
} GtkMl_SArray;

typedef struct GtkMl_SVar {
    GtkMl_SObj expr;
} GtkMl_SVar;

typedef struct GtkMl_SVararg {
    GtkMl_SObj expr;
} GtkMl_SVararg;

typedef struct GtkMl_SQuote {
    GtkMl_SObj expr;
} GtkMl_SQuote;

typedef struct GtkMl_SQuasiquote {
    GtkMl_SObj expr;
} GtkMl_SQuasiquote;

typedef struct GtkMl_SUnquote {
    GtkMl_SObj expr;
} GtkMl_SUnquote;

// a closure that evaluates its arguments
typedef struct GtkMl_SLambda {
    GtkMl_SObj args;
    GtkMl_SObj body;
    GtkMl_SObj capture;
} GtkMl_SLambda;

typedef enum GtkMl_ProgramKind {
    GTKML_PROG_INTRINSIC,
    GTKML_PROG_MACRO,
    GTKML_PROG_RUNTIME,
} GtkMl_ProgramKind;

// a compiled closure
typedef struct GtkMl_SProgram {
    GtkMl_SObj linkage_name;
    uint64_t addr;
    GtkMl_SObj args;
    GtkMl_SObj body;
    GtkMl_SObj capture;
    GtkMl_ProgramKind kind;
} GtkMl_SProgram;

// a compiled closure
typedef struct GtkMl_SAddress {
    GtkMl_SObj linkage_name;
    uint64_t addr;
} GtkMl_SAddress;

// a closure that doesn't evaluate its arguments
typedef struct GtkMl_SMacro {
    GtkMl_SObj args;
    GtkMl_SObj body;
    GtkMl_SObj capture;
} GtkMl_SMacro;

typedef struct GtkMl_SLightdata {
    void *userdata; // reference
} GtkMl_SLightdata;

typedef struct GtkMl_SUserdata {
    void *userdata; // reference or heap allocated
    void (*del)(GtkMl_Context *, void *); // userdata deleter
    GtkMl_SObj keep;
} GtkMl_SUserdata;

typedef union GtkMl_SUnion {
    GtkMl_SInt s_int;
    GtkMl_SFloat s_float;
    GtkMl_SChar s_char;
    GtkMl_SSymbol s_symbol;
    GtkMl_SKeyword s_keyword;
    GtkMl_SList s_list;
    GtkMl_SMap s_map;
    GtkMl_SSet s_set;
    GtkMl_SArray s_array;
    GtkMl_SVar s_var;
    GtkMl_SVararg s_vararg;
    GtkMl_SQuote s_quote;
    GtkMl_SQuasiquote s_quasiquote;
    GtkMl_SUnquote s_unquote;
    GtkMl_SLambda s_lambda;
    GtkMl_SProgram s_program;
    GtkMl_SAddress s_address;
    GtkMl_SMacro s_macro;
    GtkMl_SLightdata s_lightdata;
    GtkMl_SUserdata s_userdata;
} GtkMl_SUnion;

// a grammar level s expression
typedef struct GtkMl_S {
    GtkMl_SObj next;
    unsigned int flags;
    GtkMl_SKind kind;
    GtkMl_Span span;
    GtkMl_SUnion value;
} GtkMl_S;

#define GTKML_TAG_HAS 0x1
#define GTKML_TAG_PRIM 0x2
#define GTKML_TAG_PRIMITIVE (GTKML_TAG_HAS | GTKML_TAG_PRIM)
#define GTKML_TAG_NIL (GTKML_TAG_PRIMITIVE | 0x4)
#define GTKML_TAG_BOOL (GTKML_TAG_PRIMITIVE | 0x8)
#define GTKML_TAG_CHAR (GTKML_TAG_PRIMITIVE | 0x10)
#define GTKML_TAG_INT (GTKML_TAG_PRIMITIVE | 0x20)
#define GTKML_TAG_INT64 (GTKML_TAG_INT | 0x40)
#define GTKML_TAG_UINT64 (GTKML_TAG_INT | 0x80)
#define GTKML_TAG_FLOAT (GTKML_TAG_PRIMITIVE | 0x100)
#define GTKML_TAG_USERDATA (GTKML_TAG_PRIMITIVE | 0x200)

#define gtk_ml_has_value(val) (val.tag & GTKML_TAG_HAS)
#define gtk_ml_is_primitive(val) (val.tag & GTKML_TAG_PRIM)
#define gtk_ml_is_sobject(val) (!gtk_ml_is_primitive(val))
#define gtk_ml_prim_to_float(val) ((val.tag == GTKML_TAG_FLOAT)? val.value.flt : (float) val.value.s64)
#define gtk_ml_prim_to_int(val) (((val.tag & GTKML_TAG_INT) == GTKML_TAG_INT)? val.value.s64 : (int64_t) val.value.flt)

union GtkMl_Value {
    GtkMl_SObj sobj; // a gc object
    void *userdata; // light userdata, never heavy
    int boolean; // a boolean value
    uint32_t unicode; // a unicode character
    int64_t s64; // a signed 64-bit integer
    uint64_t u64; // an unsigned 64-bit integer
    double flt; // an IEEE-754 floating point number
};

struct GtkMl_TaggedValue {
    GtkMl_Value value;
    uintptr_t tag;
};

typedef struct GkMl_Instruction {
    uint64_t cond : 4;
    uint64_t category : 4;
    uint64_t opcode : 8;
    uint64_t data : 48;
} GtkMl_Instruction;

typedef struct GtkMl_BasicBlock {
    const char *name;
    GtkMl_Instruction *text;
    size_t len_text;
    size_t cap_text;
} GtkMl_BasicBlock;

typedef gboolean (*GtkMl_BuilderFn)(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);

typedef struct GtkMl_BuilderMacro {
    const char *name;
    GtkMl_BuilderFn fn;
    gboolean require_intrinsic;
    gboolean require_macro;
    gboolean require_runtime;
} GtkMl_BuilderMacro;

struct GtkMl_Builder {
    GtkMl_BasicBlock **basic_blocks;
    size_t len_bb;
    size_t cap_bb;
    size_t bb_offset_intr;
    size_t bb_offset_macro;
    size_t bb_offset_runtime;

    GtkMl_TaggedValue *data;
    size_t len_data;
    size_t cap_data;
    size_t data_offset_intr;
    size_t data_offset_macro;
    size_t data_offset_runtime;

    GtkMl_SObj *statics;
    size_t len_static;
    size_t cap_static;
    size_t static_offset_intr;
    size_t static_offset_macro;
    size_t static_offset_runtime;

    GtkMl_SObj counter; // (var 0)
    unsigned int flags;

    GtkMl_Context *intr_ctx;
    GtkMl_Context *macro_ctx;

    GtkMl_Program **intrinsics;
    size_t idx_prev_intr;
    size_t len_intrinsics;
    size_t cap_intrinsics;
    GtkMl_Program **macros;
    size_t idx_prev_macro;
    size_t len_macros;
    size_t cap_macros;

    GtkMl_BuilderMacro *builders;
    size_t len_builder;
    size_t cap_builder;

    GtkMl_HashSet intr_fns;
    GtkMl_HashSet macro_fns;

    int64_t *base;
    size_t len_base;
    size_t cap_base;

    GtkMl_SObj bindings;
};

struct GtkMl_Program {
    const char *start;

    GtkMl_Instruction *text;
    size_t n_text;

    GtkMl_TaggedValue *data;
    size_t n_data;

    GtkMl_SObj *statics;
    size_t n_static;
};

typedef GtkMl_SObj (*GtkMl_ReaderFn)(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Token **tokenv, size_t *tokenc);

typedef struct GtkMl_Reader {
    GtkMl_TokenKind token;
    GtkMl_ReaderFn fn;
} GtkMl_Reader;

typedef struct GtkMl_Parser {
    GtkMl_Reader *readers;
    size_t len_reader;
    size_t cap_reader;
} GtkMl_Parser;

typedef struct GtkMl_Serializer {
    // ptr to offset map
    GtkMl_HashTrie ptr_map;
} GtkMl_Serializer;

typedef struct GtkMl_Deserializer {
    // offset to ptr map
    GtkMl_HashTrie offset_map;
} GtkMl_Deserializer;

GTKML_PUBLIC GtkMl_Hasher GTKML_DEFAULT_HASHER;
GTKML_PUBLIC GtkMl_Hasher GTKML_VALUE_HASHER;
GTKML_PUBLIC GtkMl_Hasher GTKML_PTR_HASHER;

// creates a new context on the heap
// must be deleted with `gtk_ml_del_context`
GTKML_PUBLIC GtkMl_Context *gtk_ml_new_context() GTKML_MUST_USE;
#ifdef GTKML_ENABLE_POSIX
// creates a new debugger context on the heap
// must be deleted with `gtk_ml_del_context`
GTKML_PUBLIC GtkMl_Context *gtk_ml_new_debugger(pid_t dbg_process) GTKML_MUST_USE;
// sets the debug process of a debugger context
GTKML_PUBLIC void gtk_ml_set_debug(GtkMl_Context *ctx, pid_t dbg_process, GtkMl_Context *debugee);
#endif /* GTKML_ENABLE_POSIX */
// deletes a context created with `gtk_ml_new_context`
GTKML_PUBLIC void gtk_ml_del_context(GtkMl_Context *ctx);
// loads an executable program into the context
GTKML_PUBLIC void gtk_ml_load_program(GtkMl_Context *ctx, GtkMl_Program* program);
// runs a program previously loaded with `gtk_ml_load_program`
GTKML_PUBLIC gboolean gtk_ml_run_program(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_SObj program, GtkMl_SObj args) GTKML_MUST_USE;
// gets an export address from a program previously loaded with `gtk_ml_load_program`
GTKML_PUBLIC GtkMl_SObj gtk_ml_get_export(GtkMl_Context *ctx, GtkMl_SObj *err, const char *linkage_name) GTKML_MUST_USE;
// compile a lambda expression to bytecode with expanding macros
GTKML_PUBLIC gboolean gtk_ml_compile_program(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_SObj *err, GtkMl_SObj lambda) GTKML_MUST_USE;

// creates a new builder on the heap
GTKML_PUBLIC GtkMl_Builder *gtk_ml_new_builder(GtkMl_Context *ctx) GTKML_MUST_USE;
// builds the program
GTKML_PUBLIC GtkMl_Program *gtk_ml_build(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Builder *b, GtkMl_Program **additional_programs, size_t n_programs) GTKML_MUST_USE;
// deletes a program returned by `gtk_ml_build`
GTKML_PUBLIC void gtk_ml_del_program(GtkMl_Program *program);
// appends and returns a basic block to builder
GTKML_PUBLIC GtkMl_BasicBlock *gtk_ml_append_basic_block(GtkMl_Builder *b, const char *name) GTKML_MUST_USE;
// appends a data and returns a handle to it
GTKML_PUBLIC GtkMl_Static gtk_ml_append_data(GtkMl_Builder *b, GtkMl_TaggedValue value) GTKML_MUST_USE;
// appends a static value and returns a handle to it
GTKML_PUBLIC GtkMl_Static gtk_ml_append_static(GtkMl_Builder *b, GtkMl_SObj value) GTKML_MUST_USE;
// apends a static sobject to the STATIC section and appends its handle to the DATA section
#define gtk_ml_append_static_data(b, sobj) gtk_ml_append_data(b, gtk_ml_value_sobject((GtkMl_SObj) gtk_ml_append_static(b, sobj)))

GTKML_PUBLIC void gtk_ml_delete(GtkMl_Context *ctx, GtkMl_SObj s);
GTKML_PUBLIC void gtk_ml_del(GtkMl_Context *ctx, GtkMl_SObj s);

// sets the conditional flags of the next instruction
GTKML_PUBLIC void gtk_ml_builder_set_cond(GtkMl_Builder *b, unsigned int flags);
// clears and returns the currently set conditional flags
GTKML_PUBLIC unsigned int gtk_ml_builder_clear_cond(GtkMl_Builder *b);
// returns the current counter value and post-increments it
GTKML_PUBLIC GtkMl_SObj gtk_ml_builder_get_and_inc(GtkMl_Context *ctx, GtkMl_Builder *b) GTKML_MUST_USE;
// binds a symbol in a lexical scope
GTKML_PUBLIC int64_t gtk_ml_builder_alloca(GtkMl_Context *ctx, GtkMl_Builder *b) GTKML_MUST_USE;
// binds a symbol in a lexical scope
GTKML_PUBLIC void gtk_ml_builder_bind(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_SObj key, int64_t offset);
// get a symbol from a lexical scope
GTKML_PUBLIC GtkMl_TaggedValue gtk_ml_builder_get(GtkMl_Builder *b, GtkMl_SObj key);
// enters a new scope
GTKML_PUBLIC void gtk_ml_builder_enter(GtkMl_Context *ctx, GtkMl_Builder *b, gboolean inherit);
// leaves a new scope
GTKML_PUBLIC void gtk_ml_builder_leave(GtkMl_Context *ctx, GtkMl_Builder *b);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_push_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err, GtkMl_Data data) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_push_addr(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err, GtkMl_Static linkage_name) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_pop(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_setf_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err, GtkMl_Data data) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_popf(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_define(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_car(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_cdr(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_bind(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_bind_args(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_list(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_enter(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_leave(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_unwrap(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_typeof(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_to_sobject(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_to_prim(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_get_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err, GtkMl_Data data) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_local_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err, GtkMl_Data data) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_list_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err, GtkMl_Data data) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_map_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err, GtkMl_Data data) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_set_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err, GtkMl_Data data) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_array_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err, GtkMl_Data data) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_setmm_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_getmm_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_var(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_getvar(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_assignvar(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_len(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_array_index(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_array_push(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_array_pop(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_array_concat(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_map_get(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_map_insert(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_map_delete(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_map_concat(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_set_contains(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_set_insert(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_set_delete(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_set_concat(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a call to C in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_call_core(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err, GtkMl_Data data) GTKML_MUST_USE;
// builds a call instruction in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_call(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a call instruction in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_branch_absolute(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err, GtkMl_Static address) GTKML_MUST_USE;
// builds a ret instruction in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_ret(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds an add instruction in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_add(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a sub instruction in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_sub(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a mul instruction in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_mul(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a div instruction in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_div(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a mod instruction in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_mod(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a bitand instruction in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_bitand(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a bitor instruction in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_bitor(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a bitxor instruction in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_bitxor(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err) GTKML_MUST_USE;
// builds a cmp instruction in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_cmp(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_SObj *err, GtkMl_Data data) GTKML_MUST_USE;

// loads an expression from a path
GTKML_PUBLIC GtkMl_SObj gtk_ml_load(GtkMl_Context *ctx, char **src, GtkMl_SObj *err, const char *file) GTKML_MUST_USE;
// loads an expression from a file
GTKML_PUBLIC GtkMl_SObj gtk_ml_loadf(GtkMl_Context *ctx, char **src, GtkMl_SObj *err, FILE *stream) GTKML_MUST_USE;
// loads an expression from a string
GTKML_PUBLIC GtkMl_SObj gtk_ml_loads(GtkMl_Context *ctx, GtkMl_SObj *err, const char *src) GTKML_MUST_USE;

GTKML_PUBLIC gboolean gtk_ml_is_ident_begin(unsigned char c) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_is_ident_cont(unsigned char c) GTKML_MUST_USE;

// pushes an expression to the stack
GTKML_PUBLIC void gtk_ml_push(GtkMl_Context *ctx, GtkMl_TaggedValue value);
// pops an expression from the stack
GTKML_PUBLIC GtkMl_TaggedValue gtk_ml_pop(GtkMl_Context *ctx);
// peeks an expression at the top of the stack
GTKML_PUBLIC GtkMl_TaggedValue gtk_ml_peek(GtkMl_Context *ctx) GTKML_MUST_USE;
// binds a value to a key in the most recent bindings context
GTKML_PUBLIC void gtk_ml_bind(GtkMl_Context *ctx, GtkMl_SObj key, GtkMl_TaggedValue value);
// gets a value bound to a key
GTKML_PUBLIC GtkMl_TaggedValue gtk_ml_get(GtkMl_Context *ctx, GtkMl_SObj key) GTKML_MUST_USE;
// sets a local value
GTKML_PUBLIC void gtk_ml_set_local(GtkMl_Context *ctx, GtkMl_TaggedValue value);
// gets a value bound to a key
GTKML_PUBLIC GtkMl_TaggedValue gtk_ml_get_local(GtkMl_Context *ctx, int64_t offset) GTKML_MUST_USE;
// collects garbage
GTKML_PUBLIC gboolean gtk_ml_collect(GtkMl_Context *ctx) GTKML_MUST_USE;
// temporarily disables gc
GTKML_PUBLIC gboolean gtk_ml_disable_gc(GtkMl_Context *ctx);
// reenables gc
GTKML_PUBLIC void gtk_ml_enable_gc(GtkMl_Context *ctx, gboolean enabled);
// dumps a value to a file
GTKML_PUBLIC gboolean gtk_ml_dumpf_value(GtkMl_Context *ctx, FILE *stream, GtkMl_SObj *err, GtkMl_TaggedValue expr) GTKML_MUST_USE;
// dumps an sobject to a file
GTKML_PUBLIC gboolean gtk_ml_dumpf(GtkMl_Context *ctx, FILE *stream, GtkMl_SObj *err, GtkMl_SObj expr) GTKML_MUST_USE;
// dumps a value to a string
GTKML_PUBLIC char *gtk_ml_dumpsn(GtkMl_Context *ctx, char *ptr, size_t n, GtkMl_SObj *err, GtkMl_SObj expr) GTKML_MUST_USE;
// dumps a value to a string and reallocates if necessary
GTKML_PUBLIC char *gtk_ml_dumpsnr(GtkMl_Context *ctx, char *ptr, size_t n, GtkMl_SObj *err, GtkMl_SObj expr) GTKML_MUST_USE;
// dumps a value to a string and reallocates if necessary
GTKML_PUBLIC char *gtk_ml_dumpsnr_value(GtkMl_Context *ctx, char *ptr, size_t n, GtkMl_SObj *err, GtkMl_TaggedValue expr) GTKML_MUST_USE;
// dumps a program to a file
GTKML_PUBLIC gboolean gtk_ml_dumpf_program(GtkMl_Context *ctx, FILE *stream, GtkMl_SObj *err) GTKML_MUST_USE;
// dumps a program to a string
GTKML_PUBLIC char *gtk_ml_dumpsn_program(GtkMl_Context *ctx, char *ptr, size_t n, GtkMl_SObj *err) GTKML_MUST_USE;
// dumps a program to a string and reallocates if necessary
GTKML_PUBLIC char *gtk_ml_dumpsnr_program(GtkMl_Context *ctx, char *ptr, size_t n, GtkMl_SObj *err) GTKML_MUST_USE;
// dumps the stack to a file
GTKML_PUBLIC gboolean gtk_ml_dumpf_stack(GtkMl_Context *ctx, FILE *stream, GtkMl_SObj *err) GTKML_MUST_USE;
// compares two sobjects for equality
GTKML_PUBLIC gboolean gtk_ml_equal(GtkMl_SObj lhs, GtkMl_SObj rhs) GTKML_MUST_USE;
// compares two values for equality
GTKML_PUBLIC gboolean gtk_ml_equal_value(GtkMl_TaggedValue lhs, GtkMl_TaggedValue rhs) GTKML_MUST_USE;
// calculates a hash of a value if possible
GTKML_PUBLIC gboolean gtk_ml_hash(GtkMl_Hasher *hasher, GtkMl_Hash *hash, GtkMl_TaggedValue value) GTKML_MUST_USE;

// will set the metamap of a value, if that value is a table
GTKML_PUBLIC void gtk_ml_setmetamap(GtkMl_SObj value, GtkMl_SObj mm);
// will get the metamap of a value, or NULL if that value is not a table
GTKML_PUBLIC GtkMl_SObj gtk_ml_getmetamap(GtkMl_SObj value) GTKML_MUST_USE;

/* values */

GTKML_PUBLIC char *gtk_ml_to_c_str(GtkMl_SObj string) GTKML_MUST_USE;

GTKML_PUBLIC GtkMl_SObj gtk_ml_new_sobject(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_SKind kind) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_SObj gtk_ml_new_nil(GtkMl_Context *ctx, GtkMl_Span *span) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_SObj gtk_ml_new_true(GtkMl_Context *ctx, GtkMl_Span *span) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_SObj gtk_ml_new_false(GtkMl_Context *ctx, GtkMl_Span *span) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_SObj gtk_ml_new_int(GtkMl_Context *ctx, GtkMl_Span *span, int64_t value) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_SObj gtk_ml_new_float(GtkMl_Context *ctx, GtkMl_Span *span, float value) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_SObj gtk_ml_new_char(GtkMl_Context *ctx, GtkMl_Span *span, uint32_t value) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_SObj gtk_ml_new_string(GtkMl_Context *ctx, GtkMl_Span *span, const char *ptr, size_t len) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_SObj gtk_ml_new_symbol(GtkMl_Context *ctx, GtkMl_Span *span, gboolean owned, const char *ptr, size_t len) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_SObj gtk_ml_new_keyword(GtkMl_Context *ctx, GtkMl_Span *span, gboolean owned, const char *ptr, size_t len) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_SObj gtk_ml_new_list(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_SObj car, GtkMl_SObj cdr) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_SObj gtk_ml_new_map(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_SObj metamap) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_SObj gtk_ml_new_set(GtkMl_Context *ctx, GtkMl_Span *span) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_SObj gtk_ml_new_array(GtkMl_Context *ctx, GtkMl_Span *span) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_SObj gtk_ml_new_var(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_SObj expr) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_SObj gtk_ml_new_vararg(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_SObj expr) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_SObj gtk_ml_new_quote(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_SObj expr) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_SObj gtk_ml_new_quasiquote(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_SObj expr) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_SObj gtk_ml_new_unquote(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_SObj expr) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_SObj gtk_ml_new_lambda(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_SObj args, GtkMl_SObj body, GtkMl_SObj capture) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_SObj gtk_ml_new_program(GtkMl_Context *ctx, GtkMl_Span *span, const char *linkage_name, uint64_t addr, GtkMl_SObj args, GtkMl_SObj body, GtkMl_SObj capture, GtkMl_ProgramKind kind) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_SObj gtk_ml_new_address(GtkMl_Context *ctx, GtkMl_Span *span, const char *linkage_name, uint64_t addr) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_SObj gtk_ml_new_macro(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_SObj args, GtkMl_SObj body, GtkMl_SObj capture) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_SObj gtk_ml_new_lightdata(GtkMl_Context *ctx, GtkMl_Span *span, void *data) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_SObj gtk_ml_new_userdata(GtkMl_Context *ctx, GtkMl_Span *span, void *data, void (*del)(GtkMl_Context *ctx, void *)) GTKML_MUST_USE;

GTKML_PUBLIC GtkMl_TaggedValue gtk_ml_value_none() GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_TaggedValue gtk_ml_value_sobject(GtkMl_SObj obj) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_TaggedValue gtk_ml_value_nil() GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_TaggedValue gtk_ml_value_true() GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_TaggedValue gtk_ml_value_false() GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_TaggedValue gtk_ml_value_int(int64_t value) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_TaggedValue gtk_ml_value_uint(uint64_t value) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_TaggedValue gtk_ml_value_float(float value) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_TaggedValue gtk_ml_value_char(uint32_t value) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_TaggedValue gtk_ml_value_userdata(void *data) GTKML_MUST_USE;

/* miscelaneous */

GTKML_PUBLIC GtkMl_SObj gtk_ml_error(GtkMl_Context *ctx, const char *err, const char *desc, gboolean has_loc, int64_t line, int64_t col, size_t n, ...) GTKML_MUST_USE;

/* serialization and deserialization */

GTKML_PUBLIC void gtk_ml_new_serializer(GtkMl_Serializer *serf);
GTKML_PUBLIC void gtk_ml_new_deserializer(GtkMl_Deserializer *deserf);

// serializes an sobject and returns a heap-allocated pointer to it
GTKML_PUBLIC gboolean gtk_ml_serf_sobject(GtkMl_Serializer *serf, GtkMl_Context *ctx, FILE *stream, GtkMl_SObj *err, GtkMl_SObj value) GTKML_MUST_USE;
// deserializes an sobject from a sequence of bytes
GTKML_PUBLIC GtkMl_SObj gtk_ml_deserf_sobject(GtkMl_Deserializer *deserf, GtkMl_Context *ctx, FILE *stream, GtkMl_SObj *err) GTKML_MUST_USE;

// serializes a program and returns a heap-allocated pointer to it
GTKML_PUBLIC gboolean gtk_ml_serf_program(GtkMl_Serializer *serf, GtkMl_Context *ctx, FILE *stream, GtkMl_SObj *err, const GtkMl_Program *program) GTKML_MUST_USE;
// deserializes a program from a sequence of bytes
GTKML_PUBLIC GtkMl_Program *gtk_ml_deserf_program(GtkMl_Deserializer *deserf, GtkMl_Context *ctx, FILE *stream, GtkMl_SObj *err) GTKML_MUST_USE;

/* data structures */

typedef enum GtkMl_VisitResult {
    GTKML_VISIT_RECURSE,
    GTKML_VISIT_CONTINUE,
    GTKML_VISIT_BREAK,
} GtkMl_VisitResult;

typedef GtkMl_VisitResult (*GtkMl_HashTrieFn)(GtkMl_HashTrie *ht, GtkMl_TaggedValue key, GtkMl_TaggedValue value, GtkMl_TaggedValue data);
typedef GtkMl_VisitResult (*GtkMl_HashSetFn)(GtkMl_HashSet *hs, GtkMl_TaggedValue value, GtkMl_TaggedValue data);
typedef GtkMl_VisitResult (*GtkMl_ArrayFn)(GtkMl_Array *array, size_t index, GtkMl_TaggedValue value, GtkMl_TaggedValue data);

GTKML_PUBLIC void gtk_ml_new_hash_trie(GtkMl_HashTrie *ht, GtkMl_Hasher *hasher);
GTKML_PUBLIC void gtk_ml_del_hash_trie(GtkMl_Context *ctx, GtkMl_HashTrie *ht, void (*deleter)(GtkMl_Context *, GtkMl_TaggedValue));
GTKML_PUBLIC void gtk_ml_hash_trie_copy(GtkMl_HashTrie *out, GtkMl_HashTrie *ht);
GTKML_PUBLIC size_t gtk_ml_hash_trie_len(GtkMl_HashTrie *ht) GTKML_MUST_USE;
GTKML_PUBLIC void gtk_ml_hash_trie_concat(GtkMl_HashTrie *out, GtkMl_HashTrie *lhs, GtkMl_HashTrie *rhs);
GTKML_PUBLIC GtkMl_TaggedValue gtk_ml_hash_trie_insert(GtkMl_HashTrie *out, GtkMl_HashTrie *ht, GtkMl_TaggedValue key, GtkMl_TaggedValue value);
GTKML_PUBLIC GtkMl_TaggedValue gtk_ml_hash_trie_get(GtkMl_HashTrie *ht, GtkMl_TaggedValue key) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_hash_trie_contains(GtkMl_HashTrie *ht, GtkMl_TaggedValue key) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_TaggedValue gtk_ml_hash_trie_delete(GtkMl_HashTrie *out, GtkMl_HashTrie *ht, GtkMl_TaggedValue key);
GTKML_PUBLIC void gtk_ml_hash_trie_foreach(GtkMl_HashTrie *ht, GtkMl_HashTrieFn fn, GtkMl_TaggedValue data);
GTKML_PUBLIC gboolean gtk_ml_hash_trie_equal(GtkMl_HashTrie *lhs, GtkMl_HashTrie *rhs) GTKML_MUST_USE;

GTKML_PUBLIC void gtk_ml_new_hash_set(GtkMl_HashSet *hs, GtkMl_Hasher *hasher);
GTKML_PUBLIC void gtk_ml_del_hash_set(GtkMl_Context *ctx, GtkMl_HashSet *hs, void (*deleter)(GtkMl_Context *, GtkMl_TaggedValue));
GTKML_PUBLIC void gtk_ml_hash_set_copy(GtkMl_HashSet *out, GtkMl_HashSet *hs);
GTKML_PUBLIC size_t gtk_ml_hash_set_len(GtkMl_HashSet *hs) GTKML_MUST_USE;
GTKML_PUBLIC void gtk_ml_hash_set_concat(GtkMl_HashSet *out, GtkMl_HashSet *lhs, GtkMl_HashSet *rhs);
GTKML_PUBLIC GtkMl_TaggedValue gtk_ml_hash_set_insert(GtkMl_HashSet *out, GtkMl_HashSet *hs, GtkMl_TaggedValue value);
GTKML_PUBLIC GtkMl_TaggedValue gtk_ml_hash_set_get(GtkMl_HashSet *hs, GtkMl_TaggedValue value) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_hash_set_contains(GtkMl_HashSet *hs, GtkMl_TaggedValue value) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_TaggedValue gtk_ml_hash_set_delete(GtkMl_HashSet *out, GtkMl_HashSet *hs, GtkMl_TaggedValue value);
GTKML_PUBLIC void gtk_ml_hash_set_foreach(GtkMl_HashSet *ht, GtkMl_HashSetFn fn, GtkMl_TaggedValue data);
GTKML_PUBLIC gboolean gtk_ml_hash_set_equal(GtkMl_HashSet *lhs, GtkMl_HashSet *rhs) GTKML_MUST_USE;

GTKML_PUBLIC void gtk_ml_new_array_trie(GtkMl_Array *array);
GTKML_PUBLIC void gtk_ml_new_string_trie(GtkMl_Array *array);
GTKML_PUBLIC void gtk_ml_del_array_trie(GtkMl_Context *ctx, GtkMl_Array *array, void (*deleter)(GtkMl_Context *, GtkMl_TaggedValue));
GTKML_PUBLIC void gtk_ml_array_trie_copy(GtkMl_Array *out, GtkMl_Array *array);
GTKML_PUBLIC gboolean gtk_ml_array_trie_is_string(GtkMl_Array *array) GTKML_MUST_USE;
GTKML_PUBLIC size_t gtk_ml_array_trie_len(GtkMl_Array *array) GTKML_MUST_USE;
GTKML_PUBLIC void gtk_ml_array_trie_concat(GtkMl_Array *out, GtkMl_Array *lhs, GtkMl_Array *rhs);
GTKML_PUBLIC void gtk_ml_array_trie_push(GtkMl_Array *out, GtkMl_Array *array, GtkMl_TaggedValue value);
GTKML_PUBLIC GtkMl_TaggedValue gtk_ml_array_trie_pop(GtkMl_Array *out, GtkMl_Array *array);
GTKML_PUBLIC GtkMl_TaggedValue gtk_ml_array_trie_get(GtkMl_Array *array, size_t index) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_array_trie_contains(GtkMl_Array *array, size_t *index, GtkMl_TaggedValue value) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_TaggedValue gtk_ml_array_trie_delete(GtkMl_Array *out, GtkMl_Array *array, size_t index);
GTKML_PUBLIC void gtk_ml_array_trie_foreach(GtkMl_Array *ht, GtkMl_ArrayFn fn, GtkMl_TaggedValue data);
GTKML_PUBLIC void gtk_ml_array_trie_foreach_rev(GtkMl_Array *ht, GtkMl_ArrayFn fn, GtkMl_TaggedValue data);
GTKML_PUBLIC gboolean gtk_ml_array_trie_equal(GtkMl_Array *lhs, GtkMl_Array *rhs) GTKML_MUST_USE;

/* miscelaneous */

GTKML_PUBLIC void gtk_ml_delete_sobject_reference(GtkMl_Context *ctx, GtkMl_TaggedValue sobject);
GTKML_PUBLIC void gtk_ml_delete_sobject(GtkMl_Context *ctx, GtkMl_TaggedValue sobject);
GTKML_PUBLIC void gtk_ml_delete_value(GtkMl_Context *ctx, GtkMl_TaggedValue value);
GTKML_PUBLIC void gtk_ml_free(GtkMl_Context *ctx, void *value);

#endif /* ifndef GTK_ML_H */

// vim: set ft=c :
