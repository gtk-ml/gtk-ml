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

#define GTKML_GC_COUNT_THRESHOLD 1024
#define GTKML_GC_STEP_THRESHOLD 256

#define GTKML_VM_STACK 16 * 1024 * 1024
#define GTKML_VM_CALL_STACK 16 * 1024 * 1024

#define GTKML_F_TOPCALL 0x20
#define GTKML_F_HALT 0x10
#define GTKML_F_ZERO 0x8
#define GTKML_F_SIGN 0x4
#define GTKML_F_OVERFLOW 0x2
#define GTKML_F_CARRY 0x1
#define GTKML_F_NONE 0x0
#define GTKML_F_GENERIC (GTKML_F_ZERO | GTKML_F_SIGN | GTKML_F_OVERFLOW | GTKML_F_CARRY)

#define GTKML_STD_APPLICATION 0x0
#define GTKML_STD_NEW_WINDOW 0x1
#define GTKML_STD_ERROR 0x2
#define GTKML_STD_COMPILE_EXPR 0x100
#define GTKML_STD_EMIT_BYTECODE 0x101

#define GTKML_I_ARITH 0x1
#define GTKML_I_IMM 0x2
#define GTKML_I_IMM_EXTERN 0x4
#define GTKML_I_BR 0x3
#define GTKML_I_BR_EXTERN 0x4
#define GTKML_EI_IMM 0xa
#define GTKML_EI_IMM_EXTERN 0x4
#define GTKML_EI_BR 0xb
#define GTKML_EI_BR_EXTERN 0x4
#define GTKML_EI_EXPORT 0xc
#define GTKML_I_EXTENDED 0x8

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

#define GTKML_IA_NOP 0x0
#define GTKML_IA_HALT 0x1
#define GTKML_IA_ADD 0x4
#define GTKML_IA_SUBTRACT 0x5
#define GTKML_IA_SIGNED_MULTIPLY 0x6
#define GTKML_IA_UNSIGNED_MULTIPLY 0x7
#define GTKML_IA_SIGNED_DIVIDE 0x8
#define GTKML_IA_UNSIGNED_DIVIDE 0x9
#define GTKML_IA_SIGNED_MODULO 0xa
#define GTKML_IA_UNSIGNED_MODULO 0xb
#define GTKML_IA_BIT_AND 0x20
#define GTKML_IA_BIT_OR 0x21
#define GTKML_IA_BIT_XOR 0x22
#define GTKML_IA_BIT_NAND 0x24
#define GTKML_IA_BIT_NOR 0x25
#define GTKML_IA_BIT_XNOR 0x26
#define GTKML_IA_BIND 0x50
#define GTKML_IA_DEFINE 0x51
#define GTKML_IA_BIND_ARGS 0x52

#define GTKML_II_PUSH_IMM 0x0
#define GTKML_II_POP 0x1
#define GTKML_II_SETF_IMM 0x2
#define GTKML_II_POPF 0x3
#define GTKML_II_GET_IMM 0x4
#define GTKML_II_LIST_IMM 0x10
#define GTKML_II_MAP_IMM 0x11
#define GTKML_II_SET_IMM 0x12
#define GTKML_II_ARRAY_IMM 0x13
#define GTKML_II_SETMM_IMM 0x14
#define GTKML_II_GETMM_IMM 0x15
#define GTKML_II_VAR_IMM 0x16
#define GTKML_II_GETVAR_IMM 0x17
#define GTKML_II_ASSIGNVAR_IMM 0x18
#define GTKML_II_LEN 0x19
#define GTKML_II_ARRAY_INDEX 0x1a
#define GTKML_II_ARRAY_PUSH 0x1b
#define GTKML_II_ARRAY_POP 0x1c
#define GTKML_II_MAP_GET 0x1d
#define GTKML_II_MAP_INSERT 0x1f
#define GTKML_II_MAP_DELETE 0x20
#define GTKML_II_SET_CONTAINS 0x21
#define GTKML_II_SET_INSERT 0x22
#define GTKML_II_SET_DELETE 0x23

#define GTKML_IBR_CALL 0x1
#define GTKML_IBR_RET 0x2
#define GTKML_IBR_CALL_STD 0x3
#define GTKML_IBR_BRANCH_ABSOLUTE 0x4
#define GTKML_IBR_BRANCH_RELATIVE 0x5

#define GTKML_EII_PUSH_EXT_IMM 0x0
#define GTKML_EII_POP_EXT 0x1
#define GTKML_EII_SETF_EXT_IMM 0x2
#define GTKML_EII_POPF_EXT 0x3
#define GTKML_EII_GET_EXT_IMM 0x4
#define GTKML_EII_LIST_EXT_IMM 0x10
#define GTKML_EII_MAP_EXT_IMM 0x11
#define GTKML_EII_SET_EXT_IMM 0x12
#define GTKML_EII_ARRAY_EXT_IMM 0x13
#define GTKML_EII_CMP_EXT_IMM 0xf0

#define GTKML_EIBR_CALL_EXT_STD 0x3
#define GTKML_EIBR_BRANCH_ABSOLUTE_EXT 0x4
#define GTKML_EIBR_BRANCH_RELATIVE_EXT 0x5

#define GTKML_SIA_NOP "NOP"
#define GTKML_SIA_HALT "HALT"
#define GTKML_SIA_ADD "ADD"
#define GTKML_SIA_SUBTRACT "SUBTRACT"
#define GTKML_SIA_SIGNED_MULTIPLY "SIGNED_MULTIPLY"
#define GTKML_SIA_UNSIGNED_MULTIPLY "UNSIGNED_MULTIPLY"
#define GTKML_SIA_SIGNED_DIVIDE "SIGNED_DIVIDE"
#define GTKML_SIA_UNSIGNED_DIVIDE "UNSIGNED_DIVIDE"
#define GTKML_SIA_SIGNED_MODULO "SIGNED_MODULO"
#define GTKML_SIA_UNSIGNED_MODULO "UNSIGNED_MODULO"
#define GTKML_SIA_BIT_AND "BIT_AND"
#define GTKML_SIA_BIT_OR "BIT_OR"
#define GTKML_SIA_BIT_XOR "BIT_XOR"
#define GTKML_SIA_BIT_NAND "BIT_NAND"
#define GTKML_SIA_BIT_NOR "BIT_NOR"
#define GTKML_SIA_BIT_XNOR "BIT_XNOR"
#define GTKML_SIA_BIND "BIND"
#define GTKML_SIA_DEFINE "DEFINE"
#define GTKML_SIA_BIND_ARGS "BIND_ARGS"

#define GTKML_SII_PUSH_IMM_EXTERN "PUSH_IMM EXTERN"
#define GTKML_SII_PUSH_IMM "PUSH_IMM"
#define GTKML_SII_POP "POP"
#define GTKML_SII_SETF_IMM "SETF_IMM"
#define GTKML_SII_POPF "POPF"
#define GTKML_SII_GET_IMM "GET_IMM"
#define GTKML_SII_LIST_IMM "LIST_IMM"
#define GTKML_SII_MAP_IMM "MAP_IMM"
#define GTKML_SII_SET_IMM "SET_IMM"
#define GTKML_SII_ARRAY_IMM "ARRAY_IMM"
#define GTKML_SII_SETMM_IMM "SETMM_IMM"
#define GTKML_SII_GETMM_IMM "GETMM_IMM"
#define GTKML_SII_VAR_IMM "VAR_IMM"
#define GTKML_SII_GETVAR_IMM "GETVAR_IMM"
#define GTKML_SII_ASSIGNVAR_IMM "ASSIGNVAR_IMM"
#define GTKML_SII_LEN "LEN"
#define GTKML_SII_ARRAY_INDEX "ARRAY_INDEX"
#define GTKML_SII_ARRAY_PUSH "ARRAY_PUSH"
#define GTKML_SII_ARRAY_POP "ARRAY_POP"
#define GTKML_SII_MAP_GET "MAP_GET"
#define GTKML_SII_MAP_INSERT "MAP_INSERT"
#define GTKML_SII_MAP_DELETE "MAP_DELETE"
#define GTKML_SII_SET_CONTAINS "SET_CONTAINS"
#define GTKML_SII_SET_INSERT "SET_INSERT"
#define GTKML_SII_SET_DELETE "SET_DELETE"

#define GTKML_SIBR_CALL_STD "CALL_STD"
#define GTKML_SIBR_CALL "CALL"
#define GTKML_SIBR_RET "RET"
#define GTKML_SIBR_BRANCH_ABSOLUTE "BRANCH_ABSOLUTE"
#define GTKML_SIBR_BRANCH_RELATIVE "BRANCH_RELATIVE"

#define GTKML_SEII_PUSH_EXT_IMM_EXTERN "PUSH_EXT_IMM EXTERN"
#define GTKML_SEII_PUSH_EXT_IMM "PUSH_EXT_IMM"
#define GTKML_SEII_POP_EXT "POP_EXT"
#define GTKML_SEII_SETF_EXT_IMM "SETF_EXT_IMM"
#define GTKML_SEII_POPF_EXT "POPF_EXT"
#define GTKML_SEII_GET_EXT_IMM "GET_EXT_IMM"
#define GTKML_SEII_LIST_EXT_IMM "LIST_EXT_IMM"
#define GTKML_SEII_MAP_EXT_IMM "MAP_EXT_IMM"
#define GTKML_SEII_SET_EXT_IMM "SET_EXT_IMM"
#define GTKML_SEII_ARRAY_EXT_IMM "ARRAY_EXT_IMM"
#define GTKML_SEII_CMP_EXT_IMM "CMP_EXT_IMM"

#define GTKML_SEIBR_CALL_EXT_STD "CALL_EXT_STD"
#define GTKML_SEIBR_CALL_EXT "CALL_EXT"
#define GTKML_SEIBR_RET_EXT "RET_EXT"
#define GTKML_SEIBR_BRANCH_ABSOLUTE_EXT "BRANCH_ABSOLUTE_EXT"
#define GTKML_SEIBR_BRANCH_RELATIVE_EXT "BRANCH_RELATIVE_EXT"

#define GTKML_R_ZERO 0
#define GTKML_R_FLAGS 1
#define GTKML_R_SP 2
#define GTKML_R_BP 3
#define GTKML_R_LR 4
#define GTKML_R_PC 5
#define GTKML_REGISTER_COUNT 6

#define GTKML_ERR_IO_ERROR "unknown io error, check errno"
#define GTKML_ERR_CHARACTER_ERROR "unexpected character"
#define GTKML_ERR_TOKEN_ERROR "unexpected token"
#define GTKML_ERR_EOF_ERROR "unexpected eof"
#define GTKML_ERR_NULL_ERROR "null dereference"
#define GTKML_ERR_INVALID_SEXPR "invalid s-expression"
#define GTKML_ERR_ARGUMENT_ERROR "invalid arguments"
#define GTKML_ERR_TYPE_ERROR "invalid type for expression"
#define GTKML_ERR_CONTAINER_ERROR "not a container"
#define GTKML_ERR_CMP_ERROR "invalid comparison enum"
#define GTKML_ERR_BYTECODE_ERROR "unrecognized bytecode keyword"
#define GTKML_ERR_BOOLEAN_ERROR "expected a boolean expression"
#define GTKML_ERR_ARITY_ERROR "invalid argument count"
#define GTKML_ERR_BINDING_ERROR "binding not found"
#define GTKML_ERR_CONSTANT_ERROR "constant not found"
#define GTKML_ERR_VARARG_ERROR "free-standing vararg expression"
#define GTKML_ERR_UNQUOTE_ERROR "free-standing unquote expression"
#define GTKML_ERR_CATEGORY_ERROR "invalid category"
#define GTKML_ERR_OPCODE_ERROR "invalid opcode"
#define GTKML_ERR_ARITH_ERROR "attempt to perform arithmetic on something not a number"
#define GTKML_ERR_PROGRAM_ERROR "not a program"
#define GTKML_ERR_LINKAGE_ERROR "symbol not found while linking"
#define GTKML_ERR_SER_ERROR "serialization error"
#define GTKML_ERR_DESER_ERROR "deserialization error"
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

typedef struct GtkMl_S GtkMl_S;
typedef struct GtkMl_Context GtkMl_Context;
typedef struct GtkMl_Vm GtkMl_Vm;
typedef struct GtkMl_Builder GtkMl_Builder;
typedef union GtkMl_Register GtkMl_Register;
typedef uint64_t GtkMl_Static;
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
    gboolean (*update)(GtkMl_Hash *, void *);
    void (*finish)(GtkMl_Hash *);
    gboolean (*equal)(void *, void *);
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
} GtkMl_Array;

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

// a string like "x", "42", "hello world!"
typedef struct GtkMl_SString {
    const char *ptr; // heap allocated
    size_t len;
} GtkMl_SString;

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
    GtkMl_S *car;
    GtkMl_S *cdr;
} GtkMl_SList;

// a map like {:width 640 :height 480}
typedef struct GtkMl_SMap {
    GtkMl_HashTrie map;
    GtkMl_S *metamap;
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
    GtkMl_S *expr;
} GtkMl_SVar;

typedef struct GtkMl_SVararg {
    GtkMl_S *expr;
} GtkMl_SVararg;

typedef struct GtkMl_SQuote {
    GtkMl_S *expr;
} GtkMl_SQuote;

typedef struct GtkMl_SQuasiquote {
    GtkMl_S *expr;
} GtkMl_SQuasiquote;

typedef struct GtkMl_SUnquote {
    GtkMl_S *expr;
} GtkMl_SUnquote;

// a closure that evaluates its arguments
typedef struct GtkMl_SLambda {
    GtkMl_S *args;
    GtkMl_S *body;
    GtkMl_S *capture;
} GtkMl_SLambda;

// a compiled closure
typedef struct GtkMl_SProgram {
    const char *linkage_name;
    uint64_t addr;
    GtkMl_S *args;
    GtkMl_S *body;
    GtkMl_S *capture;
} GtkMl_SProgram;

// a compiled closure
typedef struct GtkMl_SAddress {
    const char *linkage_name;
    uint64_t addr;
} GtkMl_SAddress;

// a closure that doesn't evaluate its arguments
typedef struct GtkMl_SMacro {
    GtkMl_S *args;
    GtkMl_S *body;
    GtkMl_S *capture;
} GtkMl_SMacro;

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
    uint64_t instr;
    GtkMl_Static imm64;
} GtkMl_Instruction;

typedef struct GtkMl_BasicBlock {
    const char *name;
    GtkMl_Instruction *exec;
    size_t len_exec;
    size_t cap_exec;
} GtkMl_BasicBlock;

typedef gboolean (*GtkMl_BuilderFn)(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);

typedef struct GtkMl_BuilderMacro {
    const char *name;
    GtkMl_BuilderFn fn;
    gboolean require_intrinsic;
    gboolean require_macro;
    gboolean require_runtime;
} GtkMl_BuilderMacro;

struct GtkMl_Builder {
    GtkMl_BasicBlock *basic_blocks;
    size_t len_bb;
    size_t cap_bb;

    GtkMl_S **statics;
    size_t len_static;
    size_t cap_static;

    unsigned int counter;
    unsigned int flags;

    GtkMl_Context *intr_ctx;
    GtkMl_Vm *intr_vm;

    GtkMl_Context *macro_ctx;
    GtkMl_Vm *macro_vm;

    GtkMl_BuilderMacro *builders;
    size_t len_builder;
    size_t cap_builder;

    GtkMl_HashSet intr_fns;
    GtkMl_HashSet macro_fns;
};

union GtkMl_Register {
    GtkMl_S *s_expr;
    uint64_t sp;
    uint64_t pc;
    uint64_t flags;
    uint64_t value;
};

typedef struct GtkMl_Program {
    const char *start;
    GtkMl_Instruction *exec;
    size_t n_exec;
    GtkMl_S **statics;
    size_t n_static;
} GtkMl_Program;

typedef struct GtkMl_Serializer {
    // ptr to offset map
    // uses a hack to convert `void *` into size_t
    GtkMl_HashTrie ptr_map;
} GtkMl_Serializer;

typedef struct GtkMl_Deserializer {
    // offset to ptr map
    // uses a hack to convert `void *` into size_t
    GtkMl_HashTrie ptr_map;
} GtkMl_Deserializer;

// creates a new context on the heap
// must be deleted with `gtk_ml_del_context`
GTKML_PUBLIC GtkMl_Context *gtk_ml_new_context();
// deletes a context created with `gtk_ml_new_context`
GTKML_PUBLIC void gtk_ml_del_context(GtkMl_Context *ctx);
// loads an executable program into the context
GTKML_PUBLIC void gtk_ml_load_program(GtkMl_Context *ctx, GtkMl_Program* program);
// runs a program previously loaded with `gtk_ml_load_program`
GTKML_PUBLIC gboolean gtk_ml_run_program(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *program, GtkMl_S *args);
// gets an export address from a program previously loaded with `gtk_ml_load_program`
GTKML_PUBLIC GtkMl_S *gtk_ml_get_export(GtkMl_Context *ctx, GtkMl_S **err, const char *linkage_name);

// creates a new builder on the heap
GTKML_PUBLIC GtkMl_Builder *gtk_ml_new_builder();
// builds the program's macros
GTKML_PUBLIC gboolean gtk_ml_build_macros(GtkMl_Program *out, GtkMl_S **err, GtkMl_Builder *b);
// builds the program
GTKML_PUBLIC gboolean gtk_ml_build(GtkMl_Context *ctx, GtkMl_Program *out, GtkMl_S **err, GtkMl_Builder *b);
// deletes a program returned by `gtk_ml_build`
GTKML_PUBLIC void gtk_ml_del_program(GtkMl_Program* program);
// appends and returns a basic block to builder
GTKML_PUBLIC GtkMl_BasicBlock *gtk_ml_append_basic_block(GtkMl_Builder *b, const char *name);
// appends a static value and returns a handle to it
GTKML_PUBLIC GtkMl_Static gtk_ml_append_static(GtkMl_Builder *b, GtkMl_S *value);

// sets the conditional flags of the next instruction
GTKML_PUBLIC void gtk_ml_builder_set_cond(GtkMl_Builder *b, unsigned int flags);
// clears and returns the currently set conditional flags
GTKML_PUBLIC unsigned int gtk_ml_builder_clear_cond(GtkMl_Builder *b);
// builds a halt instruction in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_halt(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_push_extended_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_push_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_push_extended_addr(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_push_addr(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static linkage_name);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_pop(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_setf_extended_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_setf_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_popf(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_define(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_bind(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_bind_args(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_get_extended_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_get_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_list_extended_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_list_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_map_extended_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_map_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_set_extended_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_set_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_array_extended_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_array_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_setmm_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_getmm_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_var_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_getvar_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_assignvar_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_len(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_array_index(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_array_push(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_array_pop(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_map_get(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_map_insert(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_map_delete(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_set_contains(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_set_insert(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err);
// builds a push in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_set_delete(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err);
// builds a call to C in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_call_extended_std(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64);
// builds a call to C in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_call_std(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64);
// builds a call instruction in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_call(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err);
// builds a call instruction in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_branch_absolute_extended(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static address);
// builds a call instruction in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_branch_absolute(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static address);
// builds a ret instruction in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_ret(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err);
// builds an add instruction in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_add(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err);
// builds a sub instruction in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_sub(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err);
// builds a mul instruction in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_mul(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err);
// builds a div instruction in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_div(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err);
// builds a mod instruction in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_mod(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err);
// builds a bitand instruction in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_bitand(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err);
// builds a bitor instruction in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_bitor(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err);
// builds a bitxor instruction in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_bitxor(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err);
// builds a cmp instruction in the chosen basic_block
GTKML_PUBLIC gboolean gtk_ml_build_cmp(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64);

// creates a new virtual machine on the heap
// must be deleted with `gtk_ml_del_vm`
GTKML_PUBLIC GtkMl_Vm *gtk_ml_new_vm(GtkMl_Context *ctx);
// deletes a virtual machine created with `gtk_ml_new_vm`
GTKML_PUBLIC void gtk_ml_del_vm(GtkMl_Vm *vm);

// loads an expression from a path
GTKML_PUBLIC GtkMl_S *gtk_ml_load(GtkMl_Context *ctx, char **src, GtkMl_S **err, const char *file);
// loads an expression from a file
GTKML_PUBLIC GtkMl_S *gtk_ml_loadf(GtkMl_Context *ctx, char **src, GtkMl_S **err, FILE *stream);
// loads an expression from a string
GTKML_PUBLIC GtkMl_S *gtk_ml_loads(GtkMl_Context *ctx, GtkMl_S **err, const char *src);

// compile a lambda expression to bytecode
GTKML_PUBLIC gboolean gtk_ml_compile_intrinsics(GtkMl_Builder *b, GtkMl_BasicBlock **start, GtkMl_S **err, GtkMl_S *intrinsic);
// compile a lambda expression to bytecode
GTKML_PUBLIC gboolean gtk_ml_compile_macros(GtkMl_Builder *b, GtkMl_BasicBlock **start, GtkMl_S **err, GtkMl_S *macro);
// compile a lambda expression to bytecode
GTKML_PUBLIC gboolean gtk_ml_compile(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_S **err, GtkMl_S *lambda);
// compile a lambda expression to bytecode with expanding macros
GTKML_PUBLIC gboolean gtk_ml_compile_program(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_S **err, GtkMl_S *lambda);

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
GTKML_PUBLIC gboolean gtk_ml_collect(GtkMl_Context *ctx);
// temporarily disables gc
GTKML_PUBLIC gboolean gtk_ml_disable_gc(GtkMl_Context *ctx);
// reenables gc
GTKML_PUBLIC void gtk_ml_enable_gc(GtkMl_Context *ctx, gboolean enabled);
// dumps a value to a file
GTKML_PUBLIC gboolean gtk_ml_dumpf(GtkMl_Context *ctx, FILE *stream, GtkMl_S **err, GtkMl_S *expr);
// dumps a value to a string
GTKML_PUBLIC char *gtk_ml_dumpsn(GtkMl_Context *ctx, char *ptr, size_t n, GtkMl_S **err, GtkMl_S *expr);
// dumps a value to a string and reallocates if necessary
GTKML_PUBLIC char *gtk_ml_dumpsnr(GtkMl_Context *ctx, char *ptr, size_t n, GtkMl_S **err, GtkMl_S *expr);
// dumps a program to a file
GTKML_PUBLIC gboolean gtk_ml_dumpf_program(GtkMl_Context *ctx, FILE *stream, GtkMl_S **err);
// dumps a program to a string
GTKML_PUBLIC char *gtk_ml_dumpsn_program(GtkMl_Context *ctx, char *ptr, size_t n, GtkMl_S **err);
// dumps a program to a string and reallocates if necessary
GTKML_PUBLIC char *gtk_ml_dumpsnr_program(GtkMl_Context *ctx, char *ptr, size_t n, GtkMl_S **err);
// compares two values for equality
GTKML_PUBLIC gboolean gtk_ml_equal(GtkMl_S *lhs, GtkMl_S *rhs);
// calculates a hash of a value if possible
GTKML_PUBLIC gboolean gtk_ml_hash(GtkMl_Hasher *hasher, GtkMl_Hash *hash, GtkMl_S *value);

// will set the metamap of a value, if that value is a table
GTKML_PUBLIC void gtk_ml_setmetamap(GtkMl_S *value, GtkMl_S *mm);
// will get the metamap of a value, or NULL if that value is not a table
GTKML_PUBLIC GtkMl_S *gtk_ml_getmetamap(GtkMl_S *value);

/* miscelaneous */

GTKML_PUBLIC GtkMl_S *gtk_ml_error(GtkMl_Context *ctx, const char *err, const char *desc, gboolean has_loc, int64_t line, int64_t col, size_t n, ...);

GTKML_PUBLIC GtkMl_S *gtk_ml_nil(GtkMl_Context *ctx);

GTKML_PUBLIC void gtk_ml_delete_value_reference(GtkMl_Context *ctx, GtkMl_S *value);
GTKML_PUBLIC void gtk_ml_delete_void_reference(GtkMl_Context *ctx, void *);
GTKML_PUBLIC void gtk_ml_delete_value(GtkMl_Context *ctx, void *);

/* serialization and deserialization */

GTKML_PUBLIC void gtk_ml_new_serializer(GtkMl_Serializer *serf);
GTKML_PUBLIC void gtk_ml_new_deserializer(GtkMl_Deserializer *deserf);

// serializes a value and returns a heap-allocated pointer to it
GTKML_PUBLIC gboolean gtk_ml_serf_value(GtkMl_Serializer *serf, GtkMl_Context *ctx, FILE *stream, GtkMl_S **err, GtkMl_S *value);
// deserializes a value from a sequence of bytes
GTKML_PUBLIC GtkMl_S *gtk_ml_deserf_value(GtkMl_Deserializer *deserf, GtkMl_Context *ctx, FILE *stream, GtkMl_S **err);

// serializes a program and returns a heap-allocated pointer to it
GTKML_PUBLIC gboolean gtk_ml_serf_program(GtkMl_Serializer *serf, GtkMl_Context *ctx, FILE *stream, GtkMl_S **err, const GtkMl_Program *program);
// deserializes a program from a sequence of bytes
GTKML_PUBLIC gboolean gtk_ml_deserf_program(GtkMl_Deserializer *deserf, GtkMl_Context *ctx, GtkMl_Program *program, FILE *stream, GtkMl_S **err);

/* data structures */

typedef enum GtkMl_VisitResult {
    GTKML_VISIT_RECURSE,
    GTKML_VISIT_CONTINUE,
    GTKML_VISIT_BREAK,
} GtkMl_VisitResult;

typedef GtkMl_VisitResult (*GtkMl_HashTrieFn)(GtkMl_HashTrie *ht, void *key, void *value, void *data);
typedef GtkMl_VisitResult (*GtkMl_HashSetFn)(GtkMl_HashSet *hs, void *value, void *data);
typedef GtkMl_VisitResult (*GtkMl_ArrayFn)(GtkMl_Array *array, size_t index, GtkMl_S *value, void *data);

GTKML_PUBLIC void gtk_ml_new_hash_trie(GtkMl_HashTrie *ht, GtkMl_Hasher *hasher);
GTKML_PUBLIC void gtk_ml_del_hash_trie(GtkMl_Context *ctx, GtkMl_HashTrie *ht, void (*deleter)(GtkMl_Context *, void *));
GTKML_PUBLIC void gtk_ml_hash_trie_copy(GtkMl_HashTrie *out, GtkMl_HashTrie *ht);
GTKML_PUBLIC size_t gtk_ml_hash_trie_len(GtkMl_HashTrie *ht);
GTKML_PUBLIC void gtk_ml_hash_trie_concat(GtkMl_HashTrie *out, GtkMl_HashTrie *lhs, GtkMl_HashTrie *rhs);
GTKML_PUBLIC void *gtk_ml_hash_trie_insert(GtkMl_HashTrie *out, GtkMl_HashTrie *ht, void *key, void *value);
GTKML_PUBLIC void *gtk_ml_hash_trie_get(GtkMl_HashTrie *ht, void *key);
GTKML_PUBLIC gboolean gtk_ml_hash_trie_contains(GtkMl_HashTrie *ht, void *key);
GTKML_PUBLIC void *gtk_ml_hash_trie_delete(GtkMl_HashTrie *out, GtkMl_HashTrie *ht, void *key);
GTKML_PUBLIC void gtk_ml_hash_trie_foreach(GtkMl_HashTrie *ht, GtkMl_HashTrieFn fn, void *data);
GTKML_PUBLIC gboolean gtk_ml_hash_trie_equal(GtkMl_HashTrie *lhs, GtkMl_HashTrie *rhs);

GTKML_PUBLIC void gtk_ml_new_hash_set(GtkMl_HashSet *hs, GtkMl_Hasher *hasher);
GTKML_PUBLIC void gtk_ml_del_hash_set(GtkMl_Context *ctx, GtkMl_HashSet *hs, void (*deleter)(GtkMl_Context *, void *));
GTKML_PUBLIC void gtk_ml_hash_set_copy(GtkMl_HashSet *out, GtkMl_HashSet *hs);
GTKML_PUBLIC size_t gtk_ml_hash_set_len(GtkMl_HashSet *hs);
GTKML_PUBLIC void gtk_ml_hash_set_concat(GtkMl_HashSet *out, GtkMl_HashSet *lhs, GtkMl_HashSet *rhs);
GTKML_PUBLIC void *gtk_ml_hash_set_insert(GtkMl_HashSet *out, GtkMl_HashSet *hs, void *value);
GTKML_PUBLIC void *gtk_ml_hash_set_get(GtkMl_HashSet *hs, void *value);
GTKML_PUBLIC gboolean gtk_ml_hash_set_contains(GtkMl_HashSet *hs, void *value);
GTKML_PUBLIC void *gtk_ml_hash_set_delete(GtkMl_HashSet *out, GtkMl_HashSet *hs, void *value);
GTKML_PUBLIC void gtk_ml_hash_set_foreach(GtkMl_HashSet *ht, GtkMl_HashSetFn fn, void *data);
GTKML_PUBLIC gboolean gtk_ml_hash_set_equal(GtkMl_HashSet *lhs, GtkMl_HashSet *rhs);

GTKML_PUBLIC void gtk_ml_new_array(GtkMl_Array *array);
GTKML_PUBLIC void gtk_ml_del_array(GtkMl_Context *ctx, GtkMl_Array *array, void (*deleter)(GtkMl_Context *, GtkMl_S *));
GTKML_PUBLIC void gtk_ml_array_copy(GtkMl_Array *out, GtkMl_Array *array);
GTKML_PUBLIC size_t gtk_ml_array_len(GtkMl_Array *array);
GTKML_PUBLIC void gtk_ml_array_concat(GtkMl_Array *out, GtkMl_Array *lhs, GtkMl_Array *rhs);
GTKML_PUBLIC void gtk_ml_array_push(GtkMl_Array *out, GtkMl_Array *array, GtkMl_S *value);
GTKML_PUBLIC GtkMl_S *gtk_ml_array_pop(GtkMl_Array *out, GtkMl_Array *array);
GTKML_PUBLIC GtkMl_S *gtk_ml_array_get(GtkMl_Array *array, size_t index);
GTKML_PUBLIC gboolean gtk_ml_array_contains(GtkMl_Array *array, size_t *index, GtkMl_S *value);
GTKML_PUBLIC GtkMl_S *gtk_ml_array_delete(GtkMl_Array *out, GtkMl_Array *array, size_t index);
GTKML_PUBLIC void gtk_ml_array_foreach(GtkMl_Array *ht, GtkMl_ArrayFn fn, void *data);
GTKML_PUBLIC gboolean gtk_ml_array_equal(GtkMl_Array *lhs, GtkMl_Array *rhs);

#endif /* ifndef GTK_ML_H */

// vim: set ft=c :
