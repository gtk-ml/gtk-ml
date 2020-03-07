#include <stdlib.h>
#include <gtk/gtk.h>
#include <math.h>
#include "gtk-ml.h"

typedef GtkMl_S *(*GtkMl_ReaderFn)(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Token **tokenv, size_t *tokenc);

typedef struct GtkMl_Reader {
    GtkMl_TokenKind token;
    GtkMl_ReaderFn fn;
} GtkMl_Reader;

typedef struct GtkMl_Parser {
    GtkMl_Reader *readers;
    size_t len_reader;
    size_t cap_reader;
} GtkMl_Parser;

struct GtkMl_Context {
    gboolean gc_enabled;
    size_t n_values;
    size_t m_values;
    GtkMl_S *first;

    GtkMl_S *bindings;
    GtkMl_S **top_scope;

    GtkMl_Vm *vm;
    GtkMl_Parser parser;
};

struct GtkMl_Vm {
    GtkMl_Register reg[GTKML_REGISTER_COUNT];
    GtkMl_Register *stack;
    size_t stack_len;
    uint64_t *call_stack;
    size_t call_stack_ptr;
    size_t call_stack_cap;
    GtkMl_Program program;
    GtkMl_S *(**std)(GtkMl_Context *, GtkMl_S **, GtkMl_S *);

    GtkMl_Context *ctx;
};


GTKML_PRIVATE GtkMl_S *parse(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Token **tokenv, size_t *tokenc);
GTKML_PRIVATE GtkMl_S *parse_vararg(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Token **tokenv, size_t *tokenc);
GTKML_PRIVATE GtkMl_S *parse_quote(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Token **tokenv, size_t *tokenc);
GTKML_PRIVATE GtkMl_S *parse_quasiquote(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Token **tokenv, size_t *tokenc);
GTKML_PRIVATE GtkMl_S *parse_unquote(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Token **tokenv, size_t *tokenc);
GTKML_PRIVATE GtkMl_S *parse_alternative(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Token **tokenv, size_t *tokenc);
GTKML_PRIVATE GtkMl_S *parse_get(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Token **tokenv, size_t *tokenc);

GTKML_PRIVATE GtkMl_S *new_value(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_SKind kind);
GTKML_PRIVATE GtkMl_S *new_nil(GtkMl_Context *ctx, GtkMl_Span *span);
GTKML_PRIVATE GtkMl_S *new_true(GtkMl_Context *ctx, GtkMl_Span *span);
GTKML_PRIVATE GtkMl_S *new_false(GtkMl_Context *ctx, GtkMl_Span *span);
GTKML_PRIVATE GtkMl_S *new_int(GtkMl_Context *ctx, GtkMl_Span *span, int64_t value);
GTKML_PRIVATE GtkMl_S *new_float(GtkMl_Context *ctx, GtkMl_Span *span, float value);
GTKML_PRIVATE GtkMl_S *new_string(GtkMl_Context *ctx, GtkMl_Span *span, const char *ptr, size_t len);
GTKML_PRIVATE GtkMl_S *new_symbol(GtkMl_Context *ctx, GtkMl_Span *span, gboolean owned, const char *ptr, size_t len);
GTKML_PRIVATE GtkMl_S *new_keyword(GtkMl_Context *ctx, GtkMl_Span *span, gboolean owned, const char *ptr, size_t len);
GTKML_PRIVATE GtkMl_S *new_list(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *car, GtkMl_S *cdr);
GTKML_PRIVATE GtkMl_S *new_map(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *metamap);
GTKML_PRIVATE GtkMl_S *new_set(GtkMl_Context *ctx, GtkMl_Span *span);
GTKML_PRIVATE GtkMl_S *new_array(GtkMl_Context *ctx, GtkMl_Span *span);
GTKML_PRIVATE GtkMl_S *new_var(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *expr);
GTKML_PRIVATE GtkMl_S *new_vararg(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *expr);
GTKML_PRIVATE GtkMl_S *new_quote(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *expr);
GTKML_PRIVATE GtkMl_S *new_quasiquote(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *expr);
GTKML_PRIVATE GtkMl_S *new_unquote(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *expr);
GTKML_PRIVATE GtkMl_S *new_lambda(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *args, GtkMl_S *body, GtkMl_S *capture);
GTKML_PRIVATE GtkMl_S *new_program(GtkMl_Context *ctx, GtkMl_Span *span, const char *linkage_name, uint64_t addr, GtkMl_S *args, GtkMl_S *body, GtkMl_S *capture);
GTKML_PRIVATE GtkMl_S *new_address(GtkMl_Context *ctx, GtkMl_Span *span, const char *linkage_name, uint64_t addr);
GTKML_PRIVATE GtkMl_S *new_macro(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *args, GtkMl_S *body, GtkMl_S *capture);
GTKML_PRIVATE GtkMl_S *new_lightdata(GtkMl_Context *ctx, GtkMl_Span *span, void *data);
GTKML_PRIVATE GtkMl_S *new_userdata(GtkMl_Context *ctx, GtkMl_Span *span, void *data, void (*del)(GtkMl_Context *ctx, void *));
GTKML_PRIVATE void delete(GtkMl_Context *ctx, GtkMl_S *s);
GTKML_PRIVATE void del(GtkMl_Context *ctx, GtkMl_S *s);
GTKML_PRIVATE void gtk_ml_object_unref(GtkMl_Context *ctx, void *obj);

GTKML_PRIVATE gboolean builder_compile_expr(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PRIVATE gboolean builder_emit_bytecode(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PRIVATE gboolean builder_do(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PRIVATE gboolean builder_lambda(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PRIVATE gboolean builder_macro(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PRIVATE gboolean builder_cond(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PRIVATE gboolean builder_while(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PRIVATE gboolean builder_add(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PRIVATE gboolean builder_sub(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PRIVATE gboolean builder_mul(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PRIVATE gboolean builder_div(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PRIVATE gboolean builder_mod(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PRIVATE gboolean builder_bitnot(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PRIVATE gboolean builder_bitand(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PRIVATE gboolean builder_bitor(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PRIVATE gboolean builder_bitxor(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PRIVATE gboolean builder_cmp(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PRIVATE gboolean builder_var(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PRIVATE gboolean builder_vararg(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PRIVATE gboolean builder_quote(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PRIVATE gboolean builder_quasiquote(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PRIVATE gboolean builder_unquote(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PRIVATE gboolean builder_define(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PRIVATE gboolean builder_define_macro(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PRIVATE gboolean builder_define_intrinsic(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PRIVATE gboolean builder_application(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PRIVATE gboolean builder_new_window(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PRIVATE gboolean builder_setmetamap(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PRIVATE gboolean builder_getmetamap(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PRIVATE gboolean builder_getvar(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PRIVATE gboolean builder_assignvar(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PRIVATE gboolean builder_error(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);

GTKML_PRIVATE GtkMl_S *vm_std_application(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr);
GTKML_PRIVATE GtkMl_S *vm_std_new_window(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr);
GTKML_PRIVATE GtkMl_S *vm_std_error(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr);
GTKML_PRIVATE GtkMl_S *vm_std_compile_expr(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr);
GTKML_PRIVATE GtkMl_S *vm_std_emit_bytecode(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr);

GTKML_PRIVATE gboolean gtk_ml_vm_run(GtkMl_Vm *vm, GtkMl_S **err);

GTKML_PRIVATE gboolean gtk_ml_ia(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction *instr);
GTKML_PRIVATE gboolean gtk_ml_ii(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction *instr);
GTKML_PRIVATE gboolean gtk_ml_ibr(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction *instr);
GTKML_PRIVATE gboolean gtk_ml_eii(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction *instr);
GTKML_PRIVATE gboolean gtk_ml_eibr(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction *instr);
GTKML_PRIVATE gboolean gtk_ml_enop(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction *instr);

GTKML_PRIVATE gboolean gtk_ml_ia_nop(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_halt(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_add(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_subtract(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_signed_multiply(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_unsigned_multiply(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_signed_divide(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_unsigned_divide(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_signed_modulo(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_unsigned_modulo(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_bit_and(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_bit_or(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_bit_xor(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_bit_nand(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_bit_nor(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_bit_xnor(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_bind(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_define(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ia_bind_args(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);

GTKML_PRIVATE gboolean gtk_ml_ii_push_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ii_pop(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ii_setf_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ii_popf(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ii_get_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ii_list_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ii_map_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ii_set_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ii_array_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ii_setmm_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ii_getmm_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ii_var_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ii_getvar_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ii_assignvar_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);

GTKML_PRIVATE gboolean gtk_ml_ibr_call(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ibr_ret(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ibr_call_std(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ibr_branch_absolute(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PRIVATE gboolean gtk_ml_ibr_branch_relative(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);

GTKML_PRIVATE gboolean gtk_ml_eii_push_ext_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64);
GTKML_PRIVATE gboolean gtk_ml_eii_pop_ext(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64);
GTKML_PRIVATE gboolean gtk_ml_eii_setf_ext_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64);
GTKML_PRIVATE gboolean gtk_ml_eii_popf_ext(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64);
GTKML_PRIVATE gboolean gtk_ml_eii_get_ext_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64);
GTKML_PRIVATE gboolean gtk_ml_eii_list_ext_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64);
GTKML_PRIVATE gboolean gtk_ml_eii_map_ext_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64);
GTKML_PRIVATE gboolean gtk_ml_eii_set_ext_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64);
GTKML_PRIVATE gboolean gtk_ml_eii_array_ext_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64);
GTKML_PRIVATE gboolean gtk_ml_eii_cmp_ext_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64);

GTKML_PRIVATE gboolean gtk_ml_eibr_call_ext_std(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64);
GTKML_PRIVATE gboolean gtk_ml_eibr_branch_absolute_ext(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64);
GTKML_PRIVATE gboolean gtk_ml_eibr_branch_relative_ext(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64);

GTKML_PRIVATE void default_hash_start(GtkMl_Hash *hash);
GTKML_PRIVATE gboolean default_hash_update(GtkMl_Hash *hash, void *ptr);
GTKML_PRIVATE void default_hash_finish(GtkMl_Hash *hash);
GTKML_PRIVATE gboolean default_equal(void *lhs, void *rhs);

GTKML_PRIVATE void ptr_hash_start(GtkMl_Hash *hash);
GTKML_PRIVATE gboolean ptr_hash_update(GtkMl_Hash *hash, void *ptr);
GTKML_PRIVATE void ptr_hash_finish(GtkMl_Hash *hash);
GTKML_PRIVATE gboolean ptr_equal(void *lhs, void *rhs);

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
    [GTKML_IA_BIND] = GTKML_SIA_BIND,
    [GTKML_IA_DEFINE] = GTKML_SIA_DEFINE,
    [GTKML_IA_BIND_ARGS] = GTKML_SIA_BIND_ARGS,
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

GTKML_PRIVATE gboolean (*CATEGORY[])(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction *) = {
    [0] = (gboolean (*)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction *)) NULL,
    [GTKML_I_ARITH] = gtk_ml_ia,
    [GTKML_I_IMM] = gtk_ml_ii,
    [GTKML_I_BR] = gtk_ml_ibr,
    [4] = (gboolean (*)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction *)) NULL,
    [5] = (gboolean (*)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction *)) NULL,
    [6] = (gboolean (*)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction *)) NULL,
    [7] = (gboolean (*)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction *)) NULL,
    [8] = (gboolean (*)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction *)) NULL,
    [9] = (gboolean (*)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction *)) NULL,
    [GTKML_EI_IMM] = gtk_ml_eii,
    [GTKML_EI_BR] = gtk_ml_eibr,
    [GTKML_EI_EXPORT] = gtk_ml_enop,
    [13] = (gboolean (*)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction *)) NULL,
    [14] = (gboolean (*)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction *)) NULL,
    [15] = (gboolean (*)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction *)) NULL,
};

GTKML_PRIVATE gboolean (*I_ARITH[])(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction) = {
    [GTKML_IA_NOP] = gtk_ml_ia_nop,
    [GTKML_IA_HALT] = gtk_ml_ia_halt,
    [GTKML_IA_ADD] = gtk_ml_ia_add,
    [GTKML_IA_SUBTRACT] = gtk_ml_ia_subtract,
    [GTKML_IA_SIGNED_MULTIPLY] = gtk_ml_ia_signed_multiply,
    [GTKML_IA_UNSIGNED_MULTIPLY] = gtk_ml_ia_unsigned_multiply,
    [GTKML_IA_SIGNED_DIVIDE] = gtk_ml_ia_signed_divide,
    [GTKML_IA_UNSIGNED_DIVIDE] = gtk_ml_ia_unsigned_divide,
    [GTKML_IA_SIGNED_MODULO] = gtk_ml_ia_signed_modulo,
    [GTKML_IA_UNSIGNED_MODULO] = gtk_ml_ia_unsigned_modulo,
    [GTKML_IA_BIT_AND] = gtk_ml_ia_bit_and,
    [GTKML_IA_BIT_OR] = gtk_ml_ia_bit_or,
    [GTKML_IA_BIT_XOR] = gtk_ml_ia_bit_xor,
    [GTKML_IA_BIT_NAND] = gtk_ml_ia_bit_nand,
    [GTKML_IA_BIT_NOR] = gtk_ml_ia_bit_nor,
    [GTKML_IA_BIT_XNOR] = gtk_ml_ia_bit_xnor,
    [GTKML_IA_BIND] = gtk_ml_ia_bind,
    [GTKML_IA_DEFINE] = gtk_ml_ia_define,
    [GTKML_IA_BIND_ARGS] = gtk_ml_ia_bind_args,
    [255] = (gboolean (*)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction)) NULL,
};

GTKML_PRIVATE gboolean (*I_IMM[])(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction) = {
    [GTKML_II_PUSH_IMM] = gtk_ml_ii_push_imm,
    [GTKML_II_POP] = gtk_ml_ii_pop,
    [GTKML_II_SETF_IMM] = gtk_ml_ii_setf_imm,
    [GTKML_II_POPF] = gtk_ml_ii_popf,
    [GTKML_II_GET_IMM] = gtk_ml_ii_get_imm,
    [GTKML_II_LIST_IMM] = gtk_ml_ii_list_imm,
    [GTKML_II_MAP_IMM] = gtk_ml_ii_map_imm,
    [GTKML_II_SET_IMM] = gtk_ml_ii_set_imm,
    [GTKML_II_ARRAY_IMM] = gtk_ml_ii_array_imm,
    [GTKML_II_SETMM_IMM] = gtk_ml_ii_setmm_imm,
    [GTKML_II_GETMM_IMM] = gtk_ml_ii_getmm_imm,
    [GTKML_II_VAR_IMM] = gtk_ml_ii_var_imm,
    [GTKML_II_GETVAR_IMM] = gtk_ml_ii_getvar_imm,
    [GTKML_II_ASSIGNVAR_IMM] = gtk_ml_ii_assignvar_imm,
    [255] = (gboolean (*)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction)) NULL,
};

GTKML_PRIVATE gboolean (*I_BR[])(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction) = {
    [GTKML_IBR_CALL] = gtk_ml_ibr_call,
    [GTKML_IBR_RET] = gtk_ml_ibr_ret,
    [GTKML_IBR_CALL_STD] = gtk_ml_ibr_call_std,
    [GTKML_IBR_BRANCH_ABSOLUTE] = gtk_ml_ibr_branch_absolute,
    [GTKML_IBR_BRANCH_RELATIVE] = gtk_ml_ibr_branch_relative,
    [255] = (gboolean (*)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction)) NULL,
};

GTKML_PRIVATE gboolean (*EI_IMM[])(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction, GtkMl_S *) = {
    [GTKML_EII_PUSH_EXT_IMM] = gtk_ml_eii_push_ext_imm,
    [GTKML_EII_POP_EXT] = gtk_ml_eii_pop_ext,
    [GTKML_EII_SETF_EXT_IMM] = gtk_ml_eii_setf_ext_imm,
    [GTKML_EII_POPF_EXT] = gtk_ml_eii_popf_ext,
    [GTKML_EII_GET_EXT_IMM] = gtk_ml_eii_get_ext_imm,
    [GTKML_EII_LIST_EXT_IMM] = gtk_ml_eii_list_ext_imm,
    [GTKML_EII_MAP_EXT_IMM] = gtk_ml_eii_map_ext_imm,
    [GTKML_EII_SET_EXT_IMM] = gtk_ml_eii_set_ext_imm,
    [GTKML_EII_ARRAY_EXT_IMM] = gtk_ml_eii_array_ext_imm,
    [GTKML_EII_CMP_EXT_IMM] = gtk_ml_eii_cmp_ext_imm,
    [255] = (gboolean (*)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction, GtkMl_S *)) NULL,
};

GTKML_PRIVATE gboolean (*EI_BR[])(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction, GtkMl_S *) = {
    [GTKML_EIBR_CALL_EXT_STD] = gtk_ml_eibr_call_ext_std,
    [GTKML_EIBR_BRANCH_ABSOLUTE_EXT] = gtk_ml_eibr_branch_absolute_ext,
    [GTKML_EIBR_BRANCH_RELATIVE_EXT] = gtk_ml_eibr_branch_relative_ext,
    [255] = (gboolean (*)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction, GtkMl_S *)) NULL,
};

GTKML_PRIVATE GtkMl_S *(*STD[])(GtkMl_Context *, GtkMl_S **, GtkMl_S *) = {
    [GTKML_STD_APPLICATION] = vm_std_application,
    [GTKML_STD_NEW_WINDOW] = vm_std_new_window,
    [GTKML_STD_ERROR] = vm_std_error,
    [GTKML_STD_COMPILE_EXPR] = vm_std_compile_expr,
    [GTKML_STD_EMIT_BYTECODE] = vm_std_emit_bytecode,
};

GTKML_PRIVATE GtkMl_Hasher DEFAULT_HASHER = {
    default_hash_start,
    default_hash_update,
    default_hash_finish,
    default_equal
};

GTKML_PRIVATE GtkMl_Hasher PTR_HASHER = {
    ptr_hash_start,
    ptr_hash_update,
    ptr_hash_finish,
    ptr_equal
};

GtkMl_Context *gtk_ml_new_context() {
    GtkMl_Context *ctx = malloc(sizeof(GtkMl_Context));
    ctx->gc_enabled = 1;
    ctx->n_values = 0;
    ctx->m_values = GTKML_GC_COUNT_THRESHOLD;
    ctx->first = NULL;
    ctx->vm = gtk_ml_new_vm(ctx);

    // ({'flags-none G_APPLICATION_FLAGS_NONE})
    ctx->bindings = new_list(ctx, NULL, new_map(ctx, NULL, NULL), new_nil(ctx, NULL));
    ctx->top_scope = &gtk_ml_car(ctx->bindings);

    gtk_ml_define(ctx, new_symbol(ctx, NULL, 0, "flags-none", 10), new_int(ctx, NULL, G_APPLICATION_FLAGS_NONE));
    gtk_ml_define(ctx, new_symbol(ctx, NULL, 0, "cond-none", 9), new_int(ctx, NULL, GTKML_F_NONE));

    ctx->parser.readers = malloc(sizeof(GtkMl_Reader) * 64);
    ctx->parser.len_reader = 0;
    ctx->parser.cap_reader = 64;

    ctx->parser.readers[ctx->parser.len_reader].token = GTKML_TOK_ELLIPSIS;
    ctx->parser.readers[ctx->parser.len_reader].fn = parse_vararg;
    ++ctx->parser.len_reader;

    ctx->parser.readers[ctx->parser.len_reader].token = GTKML_TOK_TICK;
    ctx->parser.readers[ctx->parser.len_reader].fn = parse_quote;
    ++ctx->parser.len_reader;

    ctx->parser.readers[ctx->parser.len_reader].token = GTKML_TOK_BACKTICK;
    ctx->parser.readers[ctx->parser.len_reader].fn = parse_quasiquote;
    ++ctx->parser.len_reader;

    ctx->parser.readers[ctx->parser.len_reader].token = GTKML_TOK_COMMA;
    ctx->parser.readers[ctx->parser.len_reader].fn = parse_unquote;
    ++ctx->parser.len_reader;

    ctx->parser.readers[ctx->parser.len_reader].token = GTKML_TOK_POUND;
    ctx->parser.readers[ctx->parser.len_reader].fn = parse_alternative;
    ++ctx->parser.len_reader;

    ctx->parser.readers[ctx->parser.len_reader].token = GTKML_TOK_AT;
    ctx->parser.readers[ctx->parser.len_reader].fn = parse_get;
    ++ctx->parser.len_reader;

    return ctx;
}

void gtk_ml_del_context(GtkMl_Context *ctx) {
    gtk_ml_del_vm(ctx->vm);

    GtkMl_S *value = ctx->first;
    while (value) {
        GtkMl_S *next = value->next;
        del(ctx, value);
        value = next;
    }

    free(ctx->parser.readers);

    free(ctx);
}

GTKML_PRIVATE GtkMl_S *local_scope(GtkMl_Context *ctx) {
    GtkMl_S *local = new_map(ctx, NULL, NULL);

    GtkMl_S *bindings = ctx->bindings;
    while (bindings->kind != GTKML_S_NIL && gtk_ml_cdr(bindings)->kind != GTKML_S_NIL) {
        GtkMl_S *scope = gtk_ml_car(bindings);
        GtkMl_HashTrie new;
        gtk_ml_hash_trie_concat(&new, &scope->value.s_map.map, &local->value.s_map.map);
        gtk_ml_del_hash_trie(ctx, &local->value.s_map.map, gtk_ml_delete_void_reference);
        local->value.s_map.map = new;
        bindings = gtk_ml_cdr(bindings);
    }

    return local;
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

    while (params->kind != GTKML_S_NIL && args && args->kind != GTKML_S_NIL) {
        GtkMl_S *arg = gtk_ml_car(args);
        gtk_ml_push(ctx, arg);
        params = gtk_ml_cdr(params);
        args = gtk_ml_cdr(args);
        ++n_args;
    }

    if (params->kind == GTKML_S_LIST) {
        GtkMl_S *param = gtk_ml_car(params);
        if (param->kind == GTKML_S_VARARG) {
            *err = gtk_ml_error(ctx, "unimplemented", GTKML_ERR_UNIMPLEMENTED, 0, 0, 0, 0);
            return 0;
        } else {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return 0;
        }
    }

    gtk_ml_push(ctx, new_int(ctx, NULL, n_args));

    ctx->vm->reg[GTKML_R_PC].pc = program->value.s_program.addr;
    gboolean result = gtk_ml_vm_run(ctx->vm, err);

    gtk_ml_leave(ctx);

    return result;
}

GtkMl_S *gtk_ml_get_export(GtkMl_Context *ctx, GtkMl_S **err, const char *linkage_name) {
    for (size_t i = 0; i < ctx->vm->program.n_exec;) {
        GtkMl_Instruction instr = ctx->vm->program.exec[i];
        if (instr.gen.category == GTKML_EI_EXPORT) {
            GtkMl_S *program = ctx->vm->program.statics[ctx->vm->program.exec[i + 1].imm64];
            const char *export = program->value.s_program.linkage_name;
            if (strcmp(linkage_name, export) == 0) {
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
    GtkMl_S *new = new_map(ctx, NULL, NULL);
    gtk_ml_del_hash_trie(ctx, &new->value.s_map.map, gtk_ml_delete_void_reference);
    char *name = malloc(strlen(linkage_name) + 1);
    strcpy(name, linkage_name);
    gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, new_keyword(ctx, NULL, 0, "binding", strlen("binding")), new_symbol(ctx, NULL, 1, name, strlen(name)));
    error = new;
    *err = error;
    return NULL;
}

GtkMl_Builder *gtk_ml_new_builder() {
    GtkMl_Builder *b = malloc(sizeof(GtkMl_Builder));

    b->basic_blocks = malloc(sizeof(GtkMl_BasicBlock) * 64);
    b->len_bb = 0;
    b->cap_bb = 64;

    b->statics = malloc(sizeof(GtkMl_S *) * 64);
    b->statics[0] = NULL;
    b->len_static = 1;
    b->cap_static = 64;

    b->counter = 0;
    b->flags = 0;

    b->intr_ctx = gtk_ml_new_context();
    b->intr_vm = b->intr_ctx->vm;

    b->macro_ctx = gtk_ml_new_context();
    b->macro_vm = b->macro_ctx->vm;

    b->builders = malloc(sizeof(GtkMl_BuilderMacro) * 64);
    b->len_builder = 0;
    b->cap_builder = 64;

    char *name_compile_expr = malloc(strlen("compile-expr") + 1);
    strcpy(name_compile_expr, "compile-expr");
    b->builders[b->len_builder].name = name_compile_expr;
    b->builders[b->len_builder].fn = builder_compile_expr;
    b->builders[b->len_builder].require_intrinsic = 1;
    b->builders[b->len_builder].require_macro = 0;
    b->builders[b->len_builder].require_runtime = 0;
    ++b->len_builder;

    char *name_emit_bytecode = malloc(strlen("emit-bytecode") + 1);
    strcpy(name_emit_bytecode, "emit-bytecode");
    b->builders[b->len_builder].name = name_emit_bytecode;
    b->builders[b->len_builder].fn = builder_emit_bytecode;
    b->builders[b->len_builder].require_intrinsic = 1;
    b->builders[b->len_builder].require_macro = 0;
    b->builders[b->len_builder].require_runtime = 0;
    ++b->len_builder;

    char *name_do = malloc(strlen("do") + 1);
    strcpy(name_do, "do");
    b->builders[b->len_builder].name = name_do;
    b->builders[b->len_builder].fn = builder_do;
    b->builders[b->len_builder].require_intrinsic = 0;
    b->builders[b->len_builder].require_macro = 0;
    b->builders[b->len_builder].require_runtime = 0;
    ++b->len_builder;

    char *name_lambda = malloc(strlen("lambda") + 1);
    strcpy(name_lambda, "lambda");
    b->builders[b->len_builder].name = name_lambda;
    b->builders[b->len_builder].fn = builder_lambda;
    b->builders[b->len_builder].require_intrinsic = 0;
    b->builders[b->len_builder].require_macro = 0;
    b->builders[b->len_builder].require_runtime = 0;
    ++b->len_builder;

    char *name_macro = malloc(strlen("macro") + 1);
    strcpy(name_macro, "macro");
    b->builders[b->len_builder].name = name_macro;
    b->builders[b->len_builder].fn = builder_macro;
    b->builders[b->len_builder].require_intrinsic = 0;
    b->builders[b->len_builder].require_macro = 1;
    b->builders[b->len_builder].require_runtime = 0;
    ++b->len_builder;

    char *name_cond = malloc(strlen("cond") + 1);
    strcpy(name_cond, "cond");
    b->builders[b->len_builder].name = name_cond;
    b->builders[b->len_builder].fn = builder_cond;
    b->builders[b->len_builder].require_intrinsic = 0;
    b->builders[b->len_builder].require_macro = 0;
    b->builders[b->len_builder].require_runtime = 0;
    ++b->len_builder;

    char *name_while = malloc(strlen("while") + 1);
    strcpy(name_while, "while");
    b->builders[b->len_builder].name = name_while;
    b->builders[b->len_builder].fn = builder_while;
    b->builders[b->len_builder].require_intrinsic = 0;
    b->builders[b->len_builder].require_macro = 0;
    b->builders[b->len_builder].require_runtime = 0;
    ++b->len_builder;

    char *name_add = malloc(strlen("+") + 1);
    strcpy(name_add, "+");
    b->builders[b->len_builder].name = name_add;
    b->builders[b->len_builder].fn = builder_add;
    b->builders[b->len_builder].require_intrinsic = 0;
    b->builders[b->len_builder].require_macro = 0;
    b->builders[b->len_builder].require_runtime = 0;
    ++b->len_builder;

    char *name_sub = malloc(strlen("-") + 1);
    strcpy(name_sub, "-");
    b->builders[b->len_builder].name = name_sub;
    b->builders[b->len_builder].fn = builder_sub;
    b->builders[b->len_builder].require_intrinsic = 0;
    b->builders[b->len_builder].require_macro = 0;
    b->builders[b->len_builder].require_runtime = 0;
    ++b->len_builder;

    char *name_mul = malloc(strlen("*") + 1);
    strcpy(name_mul, "*");
    b->builders[b->len_builder].name = name_mul;
    b->builders[b->len_builder].fn = builder_mul;
    b->builders[b->len_builder].require_intrinsic = 0;
    b->builders[b->len_builder].require_macro = 0;
    b->builders[b->len_builder].require_runtime = 0;
    ++b->len_builder;

    char *name_div = malloc(strlen("/") + 1);
    strcpy(name_div, "/");
    b->builders[b->len_builder].name = name_div;
    b->builders[b->len_builder].fn = builder_div;
    b->builders[b->len_builder].require_intrinsic = 0;
    b->builders[b->len_builder].require_macro = 0;
    b->builders[b->len_builder].require_runtime = 0;
    ++b->len_builder;

    char *name_mod = malloc(strlen("%") + 1);
    strcpy(name_mod, "%");
    b->builders[b->len_builder].name = name_mod;
    b->builders[b->len_builder].fn = builder_mod;
    b->builders[b->len_builder].require_intrinsic = 0;
    b->builders[b->len_builder].require_macro = 0;
    b->builders[b->len_builder].require_runtime = 0;
    ++b->len_builder;

    char *name_bitnot = malloc(strlen("bit-not") + 1);
    strcpy(name_bitnot, "bit-not");
    b->builders[b->len_builder].name = name_bitnot;
    b->builders[b->len_builder].fn = builder_bitnot;
    b->builders[b->len_builder].require_intrinsic = 0;
    b->builders[b->len_builder].require_macro = 0;
    b->builders[b->len_builder].require_runtime = 0;
    ++b->len_builder;

    char *name_bitand = malloc(strlen("bit-and") + 1);
    strcpy(name_bitand, "bit-and");
    b->builders[b->len_builder].name = name_bitand;
    b->builders[b->len_builder].fn = builder_bitand;
    b->builders[b->len_builder].require_intrinsic = 0;
    b->builders[b->len_builder].require_macro = 0;
    b->builders[b->len_builder].require_runtime = 0;
    ++b->len_builder;

    char *name_bitor = malloc(strlen("bit-or") + 1);
    strcpy(name_bitor, "bit-or");
    b->builders[b->len_builder].name = name_bitor;
    b->builders[b->len_builder].fn = builder_bitor;
    b->builders[b->len_builder].require_intrinsic = 0;
    b->builders[b->len_builder].require_macro = 0;
    b->builders[b->len_builder].require_runtime = 0;
    ++b->len_builder;

    char *name_bitxor = malloc(strlen("bit-xor") + 1);
    strcpy(name_bitxor, "bit-xor");
    b->builders[b->len_builder].name = name_bitxor;
    b->builders[b->len_builder].fn = builder_bitxor;
    b->builders[b->len_builder].require_intrinsic = 0;
    b->builders[b->len_builder].require_macro = 0;
    b->builders[b->len_builder].require_runtime = 0;
    ++b->len_builder;

    char *name_cmp = malloc(strlen("cmp") + 1);
    strcpy(name_cmp, "cmp");
    b->builders[b->len_builder].name = name_cmp;
    b->builders[b->len_builder].fn = builder_cmp;
    b->builders[b->len_builder].require_intrinsic = 0;
    b->builders[b->len_builder].require_macro = 0;
    b->builders[b->len_builder].require_runtime = 0;
    ++b->len_builder;

    char *name_var = malloc(strlen("var") + 1);
    strcpy(name_var, "var");
    b->builders[b->len_builder].name = name_var;
    b->builders[b->len_builder].fn = builder_var;
    b->builders[b->len_builder].require_intrinsic = 0;
    b->builders[b->len_builder].require_macro = 0;
    b->builders[b->len_builder].require_runtime = 0;
    ++b->len_builder;

    char *name_vararg = malloc(strlen("vararg") + 1);
    strcpy(name_vararg, "vararg");
    b->builders[b->len_builder].name = name_vararg;
    b->builders[b->len_builder].fn = builder_vararg;
    b->builders[b->len_builder].require_intrinsic = 0;
    b->builders[b->len_builder].require_macro = 0;
    b->builders[b->len_builder].require_runtime = 0;
    ++b->len_builder;

    char *name_quote = malloc(strlen("quote") + 1);
    strcpy(name_quote, "quote");
    b->builders[b->len_builder].name = name_quote;
    b->builders[b->len_builder].fn = builder_quote;
    b->builders[b->len_builder].require_intrinsic = 0;
    b->builders[b->len_builder].require_macro = 0;
    b->builders[b->len_builder].require_runtime = 0;
    ++b->len_builder;

    char *name_quasiquote = malloc(strlen("quasiquote") + 1);
    strcpy(name_quasiquote, "quasiquote");
    b->builders[b->len_builder].name = name_quasiquote;
    b->builders[b->len_builder].fn = builder_quasiquote;
    b->builders[b->len_builder].require_intrinsic = 0;
    b->builders[b->len_builder].require_macro = 0;
    b->builders[b->len_builder].require_runtime = 0;
    ++b->len_builder;

    char *name_unquote = malloc(strlen("unquote") + 1);
    strcpy(name_unquote, "unquote");
    b->builders[b->len_builder].name = name_unquote;
    b->builders[b->len_builder].fn = builder_unquote;
    b->builders[b->len_builder].require_intrinsic = 0;
    b->builders[b->len_builder].require_macro = 0;
    b->builders[b->len_builder].require_runtime = 0;
    ++b->len_builder;

    char *name_define = malloc(strlen("define") + 1);
    strcpy(name_define, "define");
    b->builders[b->len_builder].name = name_define;
    b->builders[b->len_builder].fn = builder_define;
    b->builders[b->len_builder].require_intrinsic = 0;
    b->builders[b->len_builder].require_macro = 0;
    b->builders[b->len_builder].require_runtime = 0;
    ++b->len_builder;

    char *name_define_macro = malloc(strlen("define-macro") + 1);
    strcpy(name_define_macro, "define-macro");
    b->builders[b->len_builder].name = name_define_macro;
    b->builders[b->len_builder].fn = builder_define_macro;
    b->builders[b->len_builder].require_intrinsic = 0;
    b->builders[b->len_builder].require_macro = 0;
    b->builders[b->len_builder].require_runtime = 0;
    ++b->len_builder;

    char *name_define_intrinsic = malloc(strlen("define-intrinsic") + 1);
    strcpy(name_define_intrinsic, "define-intrinsic");
    b->builders[b->len_builder].name = name_define_intrinsic;
    b->builders[b->len_builder].fn = builder_define_intrinsic;
    b->builders[b->len_builder].require_intrinsic = 0;
    b->builders[b->len_builder].require_macro = 0;
    b->builders[b->len_builder].require_runtime = 0;
    ++b->len_builder;

    char *name_application = malloc(strlen("Application") + 1);
    strcpy(name_application, "Application");
    b->builders[b->len_builder].name = name_application;
    b->builders[b->len_builder].fn = builder_application;
    b->builders[b->len_builder].require_intrinsic = 0;
    b->builders[b->len_builder].require_macro = 0;
    b->builders[b->len_builder].require_runtime = 1;
    ++b->len_builder;

    char *name_new_window = malloc(strlen("new-window") + 1);
    strcpy(name_new_window, "new-window");
    b->builders[b->len_builder].name = name_new_window;
    b->builders[b->len_builder].fn = builder_new_window;
    b->builders[b->len_builder].require_intrinsic = 0;
    b->builders[b->len_builder].require_macro = 0;
    b->builders[b->len_builder].require_runtime = 1;
    ++b->len_builder;

    char *name_setmetamap = malloc(strlen("setmetamap") + 1);
    strcpy(name_setmetamap, "setmetamap");
    b->builders[b->len_builder].name = name_setmetamap;
    b->builders[b->len_builder].fn = builder_setmetamap;
    b->builders[b->len_builder].require_intrinsic = 0;
    b->builders[b->len_builder].require_macro = 0;
    b->builders[b->len_builder].require_runtime = 0;
    ++b->len_builder;

    char *name_getmetamap = malloc(strlen("getmetamap") + 1);
    strcpy(name_getmetamap, "getmetamap");
    b->builders[b->len_builder].name = name_getmetamap;
    b->builders[b->len_builder].fn = builder_getmetamap;
    b->builders[b->len_builder].require_intrinsic = 0;
    b->builders[b->len_builder].require_macro = 0;
    b->builders[b->len_builder].require_runtime = 0;
    ++b->len_builder;

    char *name_getvar = malloc(strlen("get") + 1);
    strcpy(name_getvar, "get");
    b->builders[b->len_builder].name = name_getvar;
    b->builders[b->len_builder].fn = builder_getvar;
    b->builders[b->len_builder].require_intrinsic = 0;
    b->builders[b->len_builder].require_macro = 0;
    b->builders[b->len_builder].require_runtime = 0;
    ++b->len_builder;

    char *name_assignvar = malloc(strlen("assign") + 1);
    strcpy(name_assignvar, "assign");
    b->builders[b->len_builder].name = name_assignvar;
    b->builders[b->len_builder].fn = builder_assignvar;
    b->builders[b->len_builder].require_intrinsic = 0;
    b->builders[b->len_builder].require_macro = 0;
    b->builders[b->len_builder].require_runtime = 0;
    ++b->len_builder;

    char *name_error = malloc(strlen("error") + 1);
    strcpy(name_error, "error");
    b->builders[b->len_builder].name = name_error;
    b->builders[b->len_builder].fn = builder_error;
    b->builders[b->len_builder].require_intrinsic = 0;
    b->builders[b->len_builder].require_macro = 0;
    b->builders[b->len_builder].require_runtime = 0;
    ++b->len_builder;

    return b;
}

gboolean build(GtkMl_Context *ctx, GtkMl_Program *out, GtkMl_S **err, GtkMl_Builder *b, GtkMl_Stage stage) {
    size_t n = 0;
    size_t n_static = b->len_static;
    for (size_t i = 0; i < b->len_bb; i++) {
        n += b->basic_blocks[i].len_exec;
    }

    GtkMl_Instruction *result = malloc(sizeof(GtkMl_Instruction) * n);
    GtkMl_S **statics = malloc(sizeof(GtkMl_S *) * n_static);
    memcpy(statics, b->statics, sizeof(GtkMl_S *) * n_static);
    uint64_t pc = 0;

    for (size_t i = 0; i < b->len_bb; i++) {
        size_t n = b->basic_blocks[i].len_exec;
        memcpy(result + pc, b->basic_blocks[i].exec, sizeof(GtkMl_Instruction) * n);
        pc += n;
    }

    for (size_t i = 0; i < n;) {
        GtkMl_Instruction instr = result[i];
        if (instr.gen.category == GTKML_EI_EXPORT) {
            GtkMl_S *addr = statics[result[i + 1].imm64];
            if (addr->kind == GTKML_S_PROGRAM) {
                addr->value.s_program.addr = 8 * i;
            } else if (addr->kind == GTKML_S_ADDRESS) {
                addr->value.s_address.addr = 8 * i;
            } else {
                *err = gtk_ml_error(ctx, "linkage-error", GTKML_ERR_LINKAGE_ERROR, 0, 0, 0, 0);
                return 0;
            }
        }
        if (instr.gen.category & GTKML_I_EXTENDED) {
            i += 2;
        } else {
            ++i;
        }
    }

    for (size_t i = 0; i < n;) {
        GtkMl_Instruction instr = result[i];
        if (instr.gen.category != GTKML_EI_EXPORT && (instr.gen.category & GTKML_EI_IMM_EXTERN)) {
            GtkMl_S *ext;
            if (instr.gen.category & GTKML_I_EXTENDED) {
                ext = statics[result[i + 1].imm64];
            } else if ((instr.gen.category & ~GTKML_EI_IMM_EXTERN) == GTKML_I_IMM) {
                ext = statics[instr.imm.imm];
            } else if ((instr.gen.category & ~GTKML_EI_BR_EXTERN) == GTKML_I_BR) {
                ext = statics[instr.br.imm];
            } else {
                *err = gtk_ml_error(ctx, "category-error", GTKML_ERR_CATEGORY_ERROR, 0, 0, 0, 0);
                return 0;
            }
            const char *bb1 = ext->value.s_string.ptr;
            gboolean found = 0;

            for (size_t l = 0; l < n;) {
                GtkMl_Instruction instr = result[l];
                if (instr.gen.category == GTKML_EI_EXPORT) {
                    GtkMl_S *addr = statics[result[l + 1].imm64];
                    const char *bb2;
                    if (addr->kind == GTKML_S_PROGRAM) {
                        bb2 = addr->value.s_program.linkage_name;
                    } else if (addr->kind == GTKML_S_ADDRESS) {
                        bb2 = addr->value.s_address.linkage_name;
                    } else {
                        *err = gtk_ml_error(ctx, "linkage-error", GTKML_ERR_LINKAGE_ERROR, 0, 0, 0, 0);
                        return 0;
                    }
                    if (strcmp(bb1, bb2) == 0) {
                        result[i].gen.category &= ~GTKML_EI_IMM_EXTERN;
                        if (result[i].gen.category & GTKML_I_EXTENDED) {
                            result[i + 1].imm64 = result[l + 1].imm64;
                        } else if (result[i].gen.category == GTKML_I_IMM) {
                            result[i].imm.imm = result[l + 1].imm64;
                        } else if (result[i].gen.category == GTKML_I_BR) {
                            result[i].br.imm = result[l + 1].imm64;
                        } else {
                            *err = gtk_ml_error(ctx, "category-error", GTKML_ERR_CATEGORY_ERROR, 0, 0, 0, 0);
                            return 0;
                        }
                        found = 1;
                        break;
                    }
                }
                if (instr.gen.category & GTKML_I_EXTENDED) {
                    l += 2;
                } else {
                    ++l;
                }
            }

            if (!found) {
                GtkMl_S *error = gtk_ml_error(ctx, "linkage-error", GTKML_ERR_LINKAGE_ERROR, 0, 0, 0, 0);
                GtkMl_S *new = new_map(ctx, NULL, NULL);
                gtk_ml_del_hash_trie(ctx, &new->value.s_map.map, gtk_ml_delete_void_reference);
                gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, new_keyword(ctx, NULL, 0, "linkage-name", strlen("linkage-name")), ext);
                error = new;
                *err = error;
                return 0;
            }
        }
        if (instr.gen.category & GTKML_I_EXTENDED) {
            i += 2;
        } else {
            ++i;
        }
    }

    switch (stage) {
    case GTKML_STAGE_INTR:
        for (size_t i = 0; i < b->len_bb; i++) {
            free(b->basic_blocks[i].exec);
        }
        b->len_bb = 0;
        b->len_static = 1;

        out->start = NULL;
        out->exec = result;
        out->n_exec = n;
        out->statics = statics;
        out->n_static = n_static;
        break;
    case GTKML_STAGE_MACRO: {
        // first we transfer the values from the intr context into the macro context
        GtkMl_S *intr_values = b->intr_ctx->first;
        b->intr_ctx->first = NULL;
        GtkMl_S **prev = &b->macro_ctx->first;
        while (*prev) {
            prev = &(*prev)->next;
        }
        *prev = intr_values;
        // then it's safe to delete the intr context and vm

        for (size_t i = 0; i < b->len_bb; i++) {
            free(b->basic_blocks[i].exec);
        }
        b->len_bb = 0;
        b->len_static = 1;

        out->start = NULL;
        out->exec = result;
        out->n_exec = n;
        out->statics = statics;
        out->n_static = n_static;
    } break;
    case GTKML_STAGE_RUNTIME: {
        // first we transfer the values from the macro context into the real context
        GtkMl_S *macro_values = b->macro_ctx->first;
        b->macro_ctx->first = NULL;
        GtkMl_S **prev = &ctx->first;
        while (*prev) {
            prev = &(*prev)->next;
        }
        *prev = macro_values;
        // then it's safe to delete the macro context and vm
        gtk_ml_del_context(b->macro_ctx);
        // and the intr context
        gtk_ml_del_context(b->intr_ctx);

        for (size_t i = 0; i < b->len_bb; i++) {
            free(b->basic_blocks[i].exec);
        }
        for (size_t i = 0; i < b->len_builder; i++) {
            free((void *) b->builders[i].name);
        }
        free(b->builders);
        free(b->statics);
        free(b->basic_blocks);
        free(b);

        char *start = malloc(strlen("_start") + 1);
        strcpy(start, "_start");
        out->start = start;
        out->exec = result;
        out->n_exec = n;
        out->statics = statics;
        out->n_static = n_static;
    } break;
    }

    return 1;
}

gboolean gtk_ml_build_intrinsics(GtkMl_Program *out, GtkMl_S **err, GtkMl_Builder *b) {
    return build(NULL, out, err, b, GTKML_STAGE_INTR);
}

gboolean gtk_ml_build_macros(GtkMl_Program *out, GtkMl_S **err, GtkMl_Builder *b) {
    return build(NULL, out, err, b, GTKML_STAGE_MACRO);
}

gboolean gtk_ml_build(GtkMl_Context *ctx, GtkMl_Program *out, GtkMl_S **err, GtkMl_Builder *b) {
    return build(ctx, out, err, b, GTKML_STAGE_RUNTIME);
}

GTKML_PUBLIC void gtk_ml_del_program(GtkMl_Program* program) {
    free((void *) program->start);
    free(program->exec);
    free(program->statics);
}

GtkMl_BasicBlock *gtk_ml_append_basic_block(GtkMl_Builder *b, const char *name) {
    if (b->len_bb == b->cap_bb) {
        b->cap_bb *= 2;
        b->basic_blocks = realloc(b->basic_blocks, sizeof(GtkMl_BasicBlock) * b->cap_bb);
    }

    GtkMl_BasicBlock *basic_block = b->basic_blocks + b->len_bb;
    ++b->len_bb;

    basic_block->name = name;
    basic_block->exec = malloc(sizeof(GtkMl_Instruction) * 256);
    basic_block->len_exec = 0;
    basic_block->cap_exec = 256;

    return basic_block;
}

GtkMl_Static gtk_ml_append_static(GtkMl_Builder *b, GtkMl_S *value) {
    for (GtkMl_Static i = 0; i < b->len_static; i++) {
        if (b->statics[i] == value) {
            return i;
        }
    }

    if (b->len_static == b->cap_static) {
        b->cap_static *= 2;
        b->statics = realloc(b->statics, sizeof(GtkMl_S *) * b->cap_static);
    }

    GtkMl_Static handle = b->len_static;
    b->statics[handle] = value;
    ++b->len_static;

    return handle;
}

GTKML_PRIVATE gboolean IDENT_BEGIN[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
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
    0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
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

GTKML_PRIVATE gboolean lex(GtkMl_Context *ctx, GtkMl_Token **tokenv, size_t *tokenc, GtkMl_S **err, const char *src) {
    *tokenv = malloc(sizeof(GtkMl_Token) * 64);
    *tokenc = 0;
    size_t cap = 64;

    int line = 1;
    int col = 1;

    while (*src) {
        if (*tokenc == cap) {
            cap *= 2;
            *tokenv = realloc(*tokenv, sizeof(GtkMl_Token) * cap);
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
        case ';':
            while (*src && *src != '\n') {
                ++col;
                ++src;
            }
            continue;
        case '@':
            (*tokenv)[*tokenc].kind = GTKML_TOK_AT;
            (*tokenv)[*tokenc].span.ptr = src;
            (*tokenv)[*tokenc].span.len = 1;
            (*tokenv)[*tokenc].span.line = line;
            (*tokenv)[*tokenc].span.col = col;
            ++*tokenc;
            break;
        case '#':
            (*tokenv)[*tokenc].kind = GTKML_TOK_POUND;
            (*tokenv)[*tokenc].span.ptr = src;
            (*tokenv)[*tokenc].span.len = 1;
            (*tokenv)[*tokenc].span.line = line;
            (*tokenv)[*tokenc].span.col = col;
            ++*tokenc;
            break;
        case '.':
            if (*(src + 1) == '.' && *(src + 2) == '.') {
                (*tokenv)[*tokenc].kind = GTKML_TOK_ELLIPSIS;
                (*tokenv)[*tokenc].span.ptr = src;
                (*tokenv)[*tokenc].span.len = 3;
                (*tokenv)[*tokenc].span.line = line;
                (*tokenv)[*tokenc].span.col = col;
                ++*tokenc;
                col += 3;
                src += 3;
                continue;
            }
            (*tokenv)[*tokenc].kind = GTKML_TOK_DOT;
            (*tokenv)[*tokenc].span.ptr = src;
            (*tokenv)[*tokenc].span.len = 1;
            (*tokenv)[*tokenc].span.line = line;
            (*tokenv)[*tokenc].span.col = col;
            ++*tokenc;
            break;
        case '\'':
            (*tokenv)[*tokenc].kind = GTKML_TOK_TICK;
            (*tokenv)[*tokenc].span.ptr = src;
            (*tokenv)[*tokenc].span.len = 1;
            (*tokenv)[*tokenc].span.line = line;
            (*tokenv)[*tokenc].span.col = col;
            ++*tokenc;
            break;
        case '`':
            (*tokenv)[*tokenc].kind = GTKML_TOK_BACKTICK;
            (*tokenv)[*tokenc].span.ptr = src;
            (*tokenv)[*tokenc].span.len = 1;
            (*tokenv)[*tokenc].span.line = line;
            (*tokenv)[*tokenc].span.col = col;
            ++*tokenc;
            break;
        case ',':
            (*tokenv)[*tokenc].kind = GTKML_TOK_COMMA;
            (*tokenv)[*tokenc].span.ptr = src;
            (*tokenv)[*tokenc].span.len = 1;
            (*tokenv)[*tokenc].span.line = line;
            (*tokenv)[*tokenc].span.col = col;
            ++*tokenc;
            break;
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
        case '[':
            (*tokenv)[*tokenc].kind = GTKML_TOK_SQUAREL;
            (*tokenv)[*tokenc].span.ptr = src;
            (*tokenv)[*tokenc].span.len = 1;
            (*tokenv)[*tokenc].span.line = line;
            (*tokenv)[*tokenc].span.col = col;
            ++*tokenc;
            break;
        case ']':
            (*tokenv)[*tokenc].kind = GTKML_TOK_SQUARER;
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
                    *err = gtk_ml_error(ctx, "character-error", GTKML_ERR_CHARACTER_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
                    return 0;
                }
            } else {
                *err = gtk_ml_error(ctx, "character-error", GTKML_ERR_CHARACTER_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
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
                *err = gtk_ml_error(ctx, "character-error", GTKML_ERR_CHARACTER_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
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
                *err = gtk_ml_error(ctx, "character-error", GTKML_ERR_CHARACTER_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
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
    ++ctx->n_values;

    GtkMl_S *s = malloc(sizeof(GtkMl_S));
    s->next = ctx->first;
    ctx->first = s;

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

GTKML_PRIVATE GtkMl_S *new_symbol(GtkMl_Context *ctx, GtkMl_Span *span, gboolean owned, const char *ptr, size_t len) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_SYMBOL);
    s->value.s_symbol.owned = owned;
    s->value.s_symbol.ptr = ptr;
    s->value.s_symbol.len = len;
    return s;
}

GTKML_PRIVATE GtkMl_S *new_keyword(GtkMl_Context *ctx, GtkMl_Span *span, gboolean owned, const char *ptr, size_t len) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_KEYWORD);
    s->value.s_keyword.owned = owned;
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

GTKML_PRIVATE GtkMl_S *new_map(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *metamap) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_MAP);
    gtk_ml_new_hash_trie(&s->value.s_map.map, &DEFAULT_HASHER);
    gtk_ml_setmetamap(s, metamap);
    return s;
}

GTKML_PRIVATE GtkMl_S *new_set(GtkMl_Context *ctx, GtkMl_Span *span) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_SET);
    gtk_ml_new_hash_set(&s->value.s_set.set, &DEFAULT_HASHER);
    return s;
}

GTKML_PRIVATE GtkMl_S *new_array(GtkMl_Context *ctx, GtkMl_Span *span) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_ARRAY);
    gtk_ml_new_array(&s->value.s_array.array);
    return s;
}

GTKML_PRIVATE GtkMl_S *new_var(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *expr) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_VAR);
    s->value.s_var.expr = expr;
    return s;
}

GTKML_PRIVATE GtkMl_S *new_vararg(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *expr) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_VARARG);
    s->value.s_vararg.expr = expr;
    return s;
}

GTKML_PRIVATE GtkMl_S *new_quote(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *expr) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_QUOTE);
    s->value.s_vararg.expr = expr;
    return s;
}

GTKML_PRIVATE GtkMl_S *new_quasiquote(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *expr) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_QUASIQUOTE);
    s->value.s_vararg.expr = expr;
    return s;
}

GTKML_PRIVATE GtkMl_S *new_unquote(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *expr) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_UNQUOTE);
    s->value.s_vararg.expr = expr;
    return s;
}

GTKML_PRIVATE GtkMl_S *new_lambda(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *args, GtkMl_S *body, GtkMl_S *capture) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_LAMBDA);
    s->value.s_lambda.args = args;
    s->value.s_lambda.body = body;
    s->value.s_lambda.capture = capture;
    return s;
}

GTKML_PRIVATE GtkMl_S *new_program(GtkMl_Context *ctx, GtkMl_Span *span, const char *linkage_name, uint64_t addr, GtkMl_S *args, GtkMl_S *body, GtkMl_S *capture) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_PROGRAM);
    s->value.s_program.linkage_name = linkage_name;
    s->value.s_program.addr = addr;
    s->value.s_program.args = args;
    s->value.s_program.body = body;
    s->value.s_program.capture = capture;
    return s;
}

GTKML_PRIVATE GtkMl_S *new_address(GtkMl_Context *ctx, GtkMl_Span *span, const char *linkage_name, uint64_t addr) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_ADDRESS);
    s->value.s_address.linkage_name = linkage_name;
    s->value.s_address.addr = addr;
    return s;
}

GTKML_PRIVATE GtkMl_S *new_macro(GtkMl_Context *ctx, GtkMl_Span *span, GtkMl_S *args, GtkMl_S *body, GtkMl_S *capture) {
    GtkMl_S *s = new_value(ctx, span, GTKML_S_MACRO);
    s->value.s_macro.args = args;
    s->value.s_macro.body = body;
    s->value.s_macro.capture = capture;
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

GtkMl_S *parse_vararg(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_ELLIPSIS) {
        *err = gtk_ml_error(ctx, "token-error", GTKML_ERR_TOKEN_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
        return NULL;
    }

    GtkMl_Span span = (*tokenv)[0].span;

    ++*tokenv;
    --*tokenc;

    GtkMl_S *expr = parse(ctx, err, tokenv, tokenc);
    if (!expr) {
        return NULL;
    }

    span_add(&span, &span, &expr->span);

    return new_vararg(ctx, &span, expr);
}

GtkMl_S *parse_quote(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_TICK) {
        *err = gtk_ml_error(ctx, "token-error", GTKML_ERR_TOKEN_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
        return NULL;
    }

    GtkMl_Span span = (*tokenv)[0].span;

    ++*tokenv;
    --*tokenc;

    GtkMl_S *expr = parse(ctx, err, tokenv, tokenc);
    if (!expr) {
        return NULL;
    }

    span_add(&span, &span, &expr->span);

    return new_quote(ctx, &span, expr);
}

GtkMl_S *parse_quasiquote(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_BACKTICK) {
        *err = gtk_ml_error(ctx, "token-error", GTKML_ERR_TOKEN_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
        return NULL;
    }

    GtkMl_Span span = (*tokenv)[0].span;

    ++*tokenv;
    --*tokenc;

    GtkMl_S *expr = parse(ctx, err, tokenv, tokenc);
    if (!expr) {
        return NULL;
    }

    span_add(&span, &span, &expr->span);

    return new_quasiquote(ctx, &span, expr);
}

GtkMl_S *parse_unquote(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_COMMA) {
        *err = gtk_ml_error(ctx, "token-error", GTKML_ERR_TOKEN_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
        return NULL;
    }

    GtkMl_Span span = (*tokenv)[0].span;

    ++*tokenv;
    --*tokenc;

    GtkMl_S *expr = parse(ctx, err, tokenv, tokenc);
    if (!expr) {
        return NULL;
    }

    span_add(&span, &span, &expr->span);

    return new_unquote(ctx, &span, expr);
}

GTKML_PRIVATE GtkMl_S *parse_set(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Token **tokenv, size_t *tokenc) {
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

    GtkMl_S *result = new_set(ctx, &span);

    while ((*tokenv)[0].kind != GTKML_TOK_CURLYR) {
        GtkMl_S *key = parse(ctx, err, tokenv, tokenc);
        if (!key) {
            return NULL;
        }

        if (*tokenc == 0) {
            *err = gtk_ml_error(ctx, "eof-error", GTKML_ERR_EOF_ERROR, span.ptr != NULL, span.line, span.col, 0);
            return NULL;
        }

        span_add(&span, &span, &(*tokenv)[0].span);

        GtkMl_S *set = result;
        result = new_set(ctx, &span);
        gtk_ml_hash_set_insert(&result->value.s_set.set, &set->value.s_set.set, key);
    }

    result->span = span;

    ++*tokenv;
    --*tokenc;

    return result;
}

GTKML_PRIVATE GtkMl_S *parse_constant(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_IDENT) {
        *err = gtk_ml_error(ctx, "token-error", GTKML_ERR_TOKEN_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
        return NULL;
    }
    GtkMl_S *result = NULL;
    if (strncmp((*tokenv)[0].span.ptr, "nil", (*tokenv)[0].span.len) == 0) {
        result = new_nil(ctx, &(*tokenv)[0].span);
    } else if (strncmp((*tokenv)[0].span.ptr, "t", (*tokenv)[0].span.len) == 0) {
        result = new_true(ctx, &(*tokenv)[0].span);
    } else if (strncmp((*tokenv)[0].span.ptr, "f", (*tokenv)[0].span.len) == 0) {
        result = new_false(ctx, &(*tokenv)[0].span);
    } else {
        *err = gtk_ml_error(ctx, "constant-error", GTKML_ERR_CONSTANT_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
        return NULL;
    }
    ++*tokenv;
    --*tokenc;
    return result;
}

GTKML_PRIVATE GtkMl_S *parse_alternative(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_POUND) {
        *err = gtk_ml_error(ctx, "token-error", GTKML_ERR_TOKEN_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
        return NULL;
    }

    GtkMl_Span span = (*tokenv)[0].span;

    ++*tokenv;
    --*tokenc;

    GtkMl_S *expr;
    switch ((*tokenv)[0].kind) {
    case GTKML_TOK_CURLYL:
        expr = parse_set(ctx, err, tokenv, tokenc);
        break;
    case GTKML_TOK_IDENT:
        expr = parse_constant(ctx, err, tokenv, tokenc);
        break;
    default:
        *err = gtk_ml_error(ctx, "token-error", GTKML_ERR_TOKEN_ERROR, span.ptr != NULL, span.line, span.col, 0);
        return NULL;
    }

    span_add(&span, &span, &expr->span);
    if (expr) {
        expr->span = span;
    }

    return expr;
}

GtkMl_S *parse_get(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_AT) {
        *err = gtk_ml_error(ctx, "token-error", GTKML_ERR_TOKEN_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
        return NULL;
    }

    GtkMl_Span span = (*tokenv)[0].span;

    GtkMl_S *get = new_symbol(ctx, &span, 0, "get", strlen("get"));

    ++*tokenv;
    --*tokenc;

    GtkMl_S *expr = parse(ctx, err, tokenv, tokenc);
    if (!expr) {
        return NULL;
    }

    span_add(&span, &span, &expr->span);

    return new_list(ctx, &span, get, new_list(ctx, &span, expr, new_nil(ctx, &span)));
}

GTKML_PRIVATE GtkMl_S *parse_list_rest(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Token **tokenv, size_t *tokenc) {
    if (*tokenc == 0) {
        *err = gtk_ml_error(ctx, "eof-error", GTKML_ERR_EOF_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
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

GTKML_PRIVATE GtkMl_S *parse_list(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_PARENL) {
        *err = gtk_ml_error(ctx, "token-error", GTKML_ERR_TOKEN_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
        return NULL;
    }

    ++*tokenv;
    --*tokenc;

    return parse_list_rest(ctx, err, tokenv, tokenc);
}

GTKML_PRIVATE GtkMl_S *parse_array(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Token **tokenv, size_t *tokenc) {
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

    GtkMl_S *result = new_array(ctx, &span);

    while ((*tokenv)[0].kind != GTKML_TOK_SQUARER) {
        GtkMl_S *elem = parse(ctx, err, tokenv, tokenc);
        if (!elem) {
            return NULL;
        }

        if (*tokenc == 0) {
            *err = gtk_ml_error(ctx, "eof-error", GTKML_ERR_EOF_ERROR, span.ptr != NULL, span.line, span.col, 0);
            return NULL;
        }

        span_add(&span, &span, &(*tokenv)[0].span);

        GtkMl_S *array = result;
        result = new_array(ctx, &span);
        gtk_ml_array_push(&result->value.s_array.array, &array->value.s_array.array, elem);
    }

    result->span = span;

    ++*tokenv;
    --*tokenc;

    return result;
}

GTKML_PRIVATE GtkMl_S *parse_map(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Token **tokenv, size_t *tokenc) {
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

    GtkMl_S *result = new_map(ctx, &span, NULL);

    while ((*tokenv)[0].kind != GTKML_TOK_CURLYR) {
        GtkMl_S *key = parse(ctx, err, tokenv, tokenc);
        if (!key) {
            return NULL;
        }

        if (*tokenc == 0) {
            *err = gtk_ml_error(ctx, "eof-error", GTKML_ERR_EOF_ERROR, span.ptr != NULL, span.line, span.col, 0);
            return NULL;
        }

        GtkMl_S *value = parse(ctx, err, tokenv, tokenc);
        if (!value) {
            return NULL;
        }

        if (*tokenc == 0) {
            *err = gtk_ml_error(ctx, "eof-error", GTKML_ERR_EOF_ERROR, span.ptr != NULL, span.line, span.col, 0);
            return NULL;
        }

        span_add(&span, &span, &(*tokenv)[0].span);

        GtkMl_S *map = result;
        result = new_map(ctx, &span, NULL);
        gtk_ml_hash_trie_insert(&result->value.s_map.map, &map->value.s_map.map, key, value);
    }

    result->span = span;

    ++*tokenv;
    --*tokenc;

    return result;
}

GTKML_PRIVATE GtkMl_S *parse_int(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_INT) {
        *err = gtk_ml_error(ctx, "token-error", GTKML_ERR_TOKEN_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
        return NULL;
    }
    GtkMl_S *result = new_int(ctx, &(*tokenv)[0].span, (*tokenv)[0].value.intval);
    ++*tokenv;
    --*tokenc;
    return result;
}

GTKML_PRIVATE GtkMl_S *parse_float(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_FLOAT) {
        *err = gtk_ml_error(ctx, "token-error", GTKML_ERR_TOKEN_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
        return NULL;
    }
    GtkMl_S *result = new_float(ctx, &(*tokenv)[0].span, (*tokenv)[0].value.floatval);
    ++*tokenv;
    --*tokenc;
    return result;
}

GTKML_PRIVATE GtkMl_S *parse_string(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_STRING) {
        *err = gtk_ml_error(ctx, "token-error", GTKML_ERR_TOKEN_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
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

GTKML_PRIVATE GtkMl_S *parse_symbol(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_IDENT) {
        *err = gtk_ml_error(ctx, "token-error", GTKML_ERR_TOKEN_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
        return NULL;
    }
    GtkMl_S *result = new_symbol(ctx, &(*tokenv)[0].span, 0, (*tokenv)[0].span.ptr, (*tokenv)[0].span.len);
    ++*tokenv;
    --*tokenc;
    return result;
}

GTKML_PRIVATE GtkMl_S *parse_keyword(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Token **tokenv, size_t *tokenc) {
    if ((*tokenv)[0].kind != GTKML_TOK_KEYWORD) {
        *err = gtk_ml_error(ctx, "token-error", GTKML_ERR_TOKEN_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
        return NULL;
    }
    GtkMl_S *result = new_keyword(ctx, &(*tokenv)[0].span, 0, (*tokenv)[0].span.ptr + 1, (*tokenv)[0].span.len - 1);
    ++*tokenv;
    --*tokenc;
    return result;
}

GtkMl_S *parse(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Token **tokenv, size_t *tokenc) {
    switch ((*tokenv)[0].kind) {
    case GTKML_TOK_PARENL:
        return parse_list(ctx, err, tokenv, tokenc);
    case GTKML_TOK_SQUAREL:
        return parse_array(ctx, err, tokenv, tokenc);
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
        for (size_t i = 0; i < ctx->parser.len_reader; i++) {
            if (ctx->parser.readers[i].token == (*tokenv)[0].kind) {
                return ctx->parser.readers[i].fn(ctx, err, tokenv, tokenc);
            }
        }
        *err = gtk_ml_error(ctx, "token-error", GTKML_ERR_TOKEN_ERROR, (*tokenv)[*tokenc].span.ptr != NULL, (*tokenv)[*tokenc].span.line, (*tokenv)[*tokenc].span.col, 0);
        return NULL;
    }
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
    if (!lex(ctx, &tokenv, &tokenc, err, src)) {
        return NULL;
    }
    GtkMl_Token *_tokenv = tokenv;

    GtkMl_S *body = new_nil(ctx, NULL);
    GtkMl_S **last = &body;

    while (tokenc) {
        GtkMl_S *line = parse(ctx, err, &_tokenv, &tokenc);
        if (!line) {
            return NULL;
        }
        GtkMl_S *new = new_list(ctx, NULL, line, *last);
        *last = new;
        last = &gtk_ml_cdr(new);
    }

    GtkMl_S *result = new_lambda(ctx, NULL, new_nil(ctx, NULL), body, new_nil(ctx, NULL));

    free(tokenv);
    return result;
}

GTKML_PRIVATE gboolean compile_std_call(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, uint64_t function, GtkMl_S *args, gboolean compile_first);
GTKML_PRIVATE gboolean compile_runtime_expression(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt);
GTKML_PRIVATE gboolean compile_runtime_program(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, const char *linkage_name, GtkMl_S *stmt, gboolean ret);

GTKML_PRIVATE gboolean compile_cond_expression(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PRIVATE gboolean compile_quasi_expression(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PRIVATE gboolean compile_expression(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PRIVATE gboolean compile_body(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PRIVATE gboolean compile_program(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, const char *linkage_name, GtkMl_S *stmt, gboolean ret, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);

gboolean builder_application(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    (void) allow_intr;
    (void) allow_macro;
    (void) allow_runtime;
    (void) allow_macro_expansion;

    GtkMl_S *args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL
            || gtk_ml_cddr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_std_call(ctx, b, basic_block, err, GTKML_STD_APPLICATION, *stmt, 0);
}

gboolean builder_new_window(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    (void) allow_intr;
    (void) allow_macro;
    (void) allow_runtime;
    (void) allow_macro_expansion;

    GtkMl_S *args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL
            || gtk_ml_cddr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_std_call(ctx, b, basic_block, err, GTKML_STD_NEW_WINDOW, *stmt, 0);
}

gboolean builder_setmetamap(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL || gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S *map = gtk_ml_car(args);
    GtkMl_S *metamap = gtk_ml_cdar(args);

    if (!compile_expression(ctx, b, basic_block, err, &map, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!compile_expression(ctx, b, basic_block, err, &metamap, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_setmm_imm(ctx, b, *basic_block, err);
}

gboolean builder_getmetamap(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S *map = gtk_ml_car(args);

    if (!compile_expression(ctx, b, basic_block, err, &map, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_getmm_imm(ctx, b, *basic_block, err);
}

gboolean builder_getvar(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S *map = gtk_ml_car(args);

    if (!compile_expression(ctx, b, basic_block, err, &map, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_getvar_imm(ctx, b, *basic_block, err);
}

gboolean builder_assignvar(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL || gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S *var = gtk_ml_car(args);
    GtkMl_S *newvalue = gtk_ml_cdar(args);

    if (!compile_expression(ctx, b, basic_block, err, &var, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!compile_expression(ctx, b, basic_block, err, &newvalue, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_assignvar_imm(ctx, b, *basic_block, err);
}

gboolean builder_error(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    (void) allow_intr;
    (void) allow_macro;
    (void) allow_runtime;
    (void) allow_macro_expansion;

    GtkMl_S *args = gtk_ml_cdr(*stmt);
    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_std_call(ctx, b, basic_block, err, GTKML_STD_ERROR, *stmt, 0);
}

gboolean builder_define(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S *definition = gtk_ml_car(args);
    if (definition->kind == GTKML_S_SYMBOL) {
        GtkMl_S *name = definition;
        GtkMl_S **value = &gtk_ml_cdar(args);
        if (!compile_expression(ctx, b, basic_block, err, value, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
            return 0;
        }
        if (!gtk_ml_build_push_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, name))) {
            return 0;
        }
        return gtk_ml_build_define(ctx, b, *basic_block, err);
    } else if (definition->kind == GTKML_S_LIST) {
        GtkMl_S *name = gtk_ml_car(definition);

        if (name->kind != GTKML_S_SYMBOL) {
            *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, definition->span.ptr != NULL, definition->span.line, definition->span.col, 0);
            return 0;
        }

        GtkMl_S *params = gtk_ml_cdr(definition);
        GtkMl_S *body = gtk_ml_cdr(args);

        GtkMl_S *lambda = new_lambda(ctx, &(*stmt)->span, params, body, local_scope(ctx));

        size_t len = name->value.s_symbol.len;
        char *linkage_name = malloc(len + 1);
        memcpy(linkage_name, name->value.s_symbol.ptr, len);
        linkage_name[len] = 0;
        GtkMl_BasicBlock *bb = gtk_ml_append_basic_block(b, linkage_name);
        if (!compile_runtime_program(ctx, b, &bb, err, linkage_name, lambda, 1)) {
            return 0;
        }
        if (!gtk_ml_build_push_extended_addr(ctx, b, *basic_block, err, gtk_ml_append_static(b, new_string(ctx, NULL, linkage_name, strlen(linkage_name))))) {
            return 0;
        }
        if (!gtk_ml_build_push_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, name))) {
            return 0;
        }
        return gtk_ml_build_define(ctx, b, *basic_block, err);
    } else {
        *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, definition->span.ptr != NULL, definition->span.line, definition->span.col, 0);
        return 0;
    }
}


gboolean builder_define_macro(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    (void) ctx;
    (void) b;
    (void) basic_block;
    (void) err;
    (void) stmt;
    (void) allow_intr;
    (void) allow_macro;
    (void) allow_runtime;
    (void) allow_macro_expansion;
    return 1;
}

gboolean builder_define_intrinsic(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    (void) ctx;
    (void) b;
    (void) basic_block;
    (void) err;
    (void) stmt;
    (void) allow_intr;
    (void) allow_macro;
    (void) allow_runtime;
    (void) allow_macro_expansion;
    return 1;
}

gboolean builder_compile_expr(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    (void) b;
    (void) basic_block;
    (void) allow_intr;
    (void) allow_macro;
    (void) allow_runtime;
    (void) allow_macro_expansion;
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL
            || gtk_ml_cddr(args)->kind == GTKML_S_NIL
            || gtk_ml_cdddr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_std_call(ctx, b, basic_block, err, GTKML_STD_COMPILE_EXPR, *stmt, 0);
}

gboolean builder_emit_bytecode(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    (void) b;
    (void) basic_block;
    (void) allow_intr;
    (void) allow_macro;
    (void) allow_runtime;
    (void) allow_macro_expansion;
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL
            || gtk_ml_cddr(args)->kind == GTKML_S_NIL
            || gtk_ml_cdddr(args)->kind == GTKML_S_NIL
            || gtk_ml_cddddr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    return compile_std_call(ctx, b, basic_block, err, GTKML_STD_EMIT_BYTECODE, *stmt, 0);
}

gboolean builder_do(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        GtkMl_S *nil = new_nil(ctx, &(*stmt)->span);
        return compile_expression(ctx, b, basic_block, err, &nil, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
    }

    while (args->kind != GTKML_S_NIL) {
        if (!compile_expression(ctx, b, basic_block, err, &gtk_ml_car(args), allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
            return 0;
        }
        args = gtk_ml_cdr(args);
        if (args->kind != GTKML_S_NIL) {
            if (!gtk_ml_build_pop(ctx, b, *basic_block, err)) {
                return 0;
            }
        }
    }

    return 1;
}

gboolean builder_lambda(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S *lambda_args = gtk_ml_car(args);
    GtkMl_S *lambda_body = gtk_ml_cdr(args);

    GtkMl_S *lambda = new_lambda(ctx, &(*stmt)->span, lambda_args, lambda_body, local_scope(ctx));
    if (!lambda) {
        return 0;
    }

    return compile_expression(ctx, b, basic_block, err, &lambda, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean builder_macro(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S *macro_args = gtk_ml_car(args);
    GtkMl_S *macro_body = gtk_ml_cdr(args);

    GtkMl_S *macro = new_macro(ctx, &(*stmt)->span, macro_args, macro_body, local_scope(ctx));
    if (!macro) {
        return 0;
    }

    return compile_expression(ctx, b, basic_block, err, &macro, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean builder_cond(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);
    GtkMl_S *_args = args;

    if (args->kind == GTKML_S_NIL) {
        return gtk_ml_build_push_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, new_nil(ctx, &(*stmt)->span)));
    }

    size_t n = 0;
    while (args->kind != GTKML_S_NIL) {
        if (gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
            return 0;
        }

        ++n;

        GtkMl_S *cond = gtk_ml_car(args);
        if (cond->kind == GTKML_S_KEYWORD
                && cond->value.s_keyword.len == 4
                && strncmp(cond->value.s_keyword.ptr, "else", 4) == 0) {
            break;
        }

        args = gtk_ml_cddr(args);
    }

    GtkMl_BasicBlock **branches = malloc(sizeof(GtkMl_BasicBlock *) * (n + 1));
    uint32_t cond_number = b->counter++;

    char *linkage_name = malloc(strlen("cond$$end") + 16);
    snprintf(linkage_name, strlen("cond$$end") + 16, "cond$%u$end", cond_number);
    GtkMl_BasicBlock *bb = gtk_ml_append_basic_block(b, linkage_name);

    if (bb->len_exec == bb->cap_exec) {
        bb->cap_exec *= 2;
        bb->exec = realloc(bb->exec, sizeof(GtkMl_Instruction) * bb->cap_exec);
    }

    bb->exec[bb->len_exec].instr = 0;
    bb->exec[bb->len_exec].imm.cond = 0;
    bb->exec[bb->len_exec].imm.category = GTKML_EI_EXPORT;
    bb->exec[bb->len_exec].imm.imm = GTKML_EI_EXPORT_FLAG_LOCAL;
    ++bb->len_exec;

    if (bb->len_exec == bb->cap_exec) {
        bb->cap_exec *= 2;
        bb->exec = realloc(bb->exec, sizeof(GtkMl_Instruction) * bb->cap_exec);
    }

    char *name = malloc(strlen(linkage_name) + 1);
    strcpy(name, linkage_name);
    GtkMl_S *addr = new_address(ctx, NULL, name, 0);
    bb->exec[bb->len_exec].imm64 = gtk_ml_append_static(b, addr);
    ++bb->len_exec;

    branches[n] = bb;

    uint32_t i = 0;
    args = _args;
    while (args->kind != GTKML_S_NIL) {
        char *linkage_name = malloc(strlen("cond$$") + 32);
        snprintf(linkage_name, strlen("cond$$") + 32, "cond$%u$%u", cond_number, i);
        GtkMl_BasicBlock *bb = gtk_ml_append_basic_block(b, linkage_name);

        if (bb->len_exec == bb->cap_exec) {
            bb->cap_exec *= 2;
            bb->exec = realloc(bb->exec, sizeof(GtkMl_Instruction) * bb->cap_exec);
        }

        bb->exec[bb->len_exec].instr = 0;
        bb->exec[bb->len_exec].imm.cond = 0;
        bb->exec[bb->len_exec].imm.category = GTKML_EI_EXPORT;
        bb->exec[bb->len_exec].imm.imm = GTKML_EI_EXPORT_FLAG_LOCAL;
        ++bb->len_exec;

        if (bb->len_exec == bb->cap_exec) {
            bb->cap_exec *= 2;
            bb->exec = realloc(bb->exec, sizeof(GtkMl_Instruction) * bb->cap_exec);
        }

        char *name = malloc(strlen(linkage_name) + 1);
        strcpy(name, linkage_name);
        GtkMl_S *addr = new_address(ctx, NULL, name, 0);
        bb->exec[bb->len_exec].imm64 = gtk_ml_append_static(b, addr);
        ++bb->len_exec;

        branches[i] = bb;

        GtkMl_S *cond = gtk_ml_car(args);
        if (cond->kind == GTKML_S_KEYWORD
                && cond->value.s_keyword.len == 4
                && strncmp(cond->value.s_keyword.ptr, "else", 4) == 0) {
            break;
        }

        ++i;

        args = gtk_ml_cddr(args);
    }

    gboolean has_else = 0;
    i = 0;
    args = _args;
    while (args->kind != GTKML_S_NIL) {
        GtkMl_S **cond = &gtk_ml_car(args);

        char *branch_name = malloc(strlen("cond$$") + 32);
        snprintf(branch_name, strlen("cond$$") + 32, "cond$%u$%u", cond_number, i);

        if ((*cond)->kind == GTKML_S_KEYWORD
                && (*cond)->value.s_keyword.len == 4
                && strncmp((*cond)->value.s_keyword.ptr, "else", 4) == 0) {
            gtk_ml_builder_clear_cond(b);
            gtk_ml_build_branch_absolute_extended(ctx, b, *basic_block, err, gtk_ml_append_static(b, new_string(ctx, NULL, branch_name, strlen(branch_name))));
            has_else = 1;
            break;
        }

        if (!compile_cond_expression(ctx, b, basic_block, err, cond, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
            return 0;
        }
        gtk_ml_builder_set_cond(b, GTKML_F_ZERO);
        gtk_ml_build_branch_absolute_extended(ctx, b, *basic_block, err, gtk_ml_append_static(b, new_string(ctx, NULL, branch_name, strlen(branch_name))));

        ++i;

        args = gtk_ml_cddr(args);
    }

    if (!has_else) {
        char *end_name = malloc(strlen("cond$$end") + 16);
        snprintf(end_name, strlen("cond$$end") + 16, "cond$%u$end", cond_number);
        gtk_ml_builder_clear_cond(b);
        gtk_ml_build_branch_absolute_extended(ctx, b, *basic_block, err, gtk_ml_append_static(b, new_string(ctx, NULL, end_name, strlen(end_name))));
    }

    i = 0;
    args = _args;
    while (args->kind != GTKML_S_NIL) {
        GtkMl_S *cond = gtk_ml_car(args);
        GtkMl_S **body = &gtk_ml_cdar(args);

        if (!compile_expression(ctx, b, &branches[i], err, body, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
            return 0;
        }

        char *end_name = malloc(strlen("cond$$end") + 16);
        snprintf(end_name, strlen("cond$$end") + 16, "cond$%u$end", cond_number);
        gtk_ml_builder_clear_cond(b);
        gtk_ml_build_branch_absolute_extended(ctx, b, branches[i], err, gtk_ml_append_static(b, new_string(ctx, NULL, end_name, strlen(end_name))));

        if (cond->kind == GTKML_S_KEYWORD
                && cond->value.s_keyword.len == 4
                && strncmp(cond->value.s_keyword.ptr, "else", 4) == 0) {
            break;
        }

        i++;

        args = gtk_ml_cddr(args);
    }

    *basic_block = branches[n];
    free(branches);

    return 1;
}

gboolean builder_while(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    uint32_t while_number = b->counter++;

    char *linkage_name = malloc(strlen("while$$cond") + 16);
    snprintf(linkage_name, strlen("while$$cond") + 16, "while$%u$cond", while_number);
    GtkMl_BasicBlock *cond = gtk_ml_append_basic_block(b, linkage_name);

    if (cond->len_exec == cond->cap_exec) {
        cond->cap_exec *= 2;
        cond->exec = realloc(cond->exec, sizeof(GtkMl_Instruction) * cond->cap_exec);
    }

    cond->exec[cond->len_exec].instr = 0;
    cond->exec[cond->len_exec].imm.cond = 0;
    cond->exec[cond->len_exec].imm.category = GTKML_EI_EXPORT;
    cond->exec[cond->len_exec].imm.imm = GTKML_EI_EXPORT_FLAG_LOCAL;
    ++cond->len_exec;

    if (cond->len_exec == cond->cap_exec) {
        cond->cap_exec *= 2;
        cond->exec = realloc(cond->exec, sizeof(GtkMl_Instruction) * cond->cap_exec);
    }

    char *name = malloc(strlen(linkage_name) + 1);
    strcpy(name, linkage_name);
    GtkMl_S *cond_addr = new_address(ctx, NULL, name, 0);
    cond->exec[cond->len_exec].imm64 = gtk_ml_append_static(b, cond_addr);
    ++cond->len_exec;

    linkage_name = malloc(strlen("while$$end") + 16);
    snprintf(linkage_name, strlen("while$$end") + 16, "while$%u$end", while_number);
    GtkMl_BasicBlock *end = gtk_ml_append_basic_block(b, linkage_name);

    if (end->len_exec == end->cap_exec) {
        end->cap_exec *= 2;
        end->exec = realloc(end->exec, sizeof(GtkMl_Instruction) * end->cap_exec);
    }

    end->exec[end->len_exec].instr = 0;
    end->exec[end->len_exec].imm.cond = 0;
    end->exec[end->len_exec].imm.category = GTKML_EI_EXPORT;
    end->exec[end->len_exec].imm.imm = GTKML_EI_EXPORT_FLAG_LOCAL;
    ++end->len_exec;

    if (end->len_exec == end->cap_exec) {
        end->cap_exec *= 2;
        end->exec = realloc(end->exec, sizeof(GtkMl_Instruction) * end->cap_exec);
    }

    name = malloc(strlen(linkage_name) + 1);
    strcpy(name, linkage_name);
    GtkMl_S *end_addr = new_address(ctx, NULL, name, 0);
    end->exec[end->len_exec].imm64 = gtk_ml_append_static(b, end_addr);
    ++end->len_exec;

    linkage_name = malloc(strlen("while$$") + 32);
    snprintf(linkage_name, strlen("while$$") + 32, "while$%u$body", while_number);
    GtkMl_BasicBlock *body = gtk_ml_append_basic_block(b, linkage_name);

    if (body->len_exec == body->cap_exec) {
        body->cap_exec *= 2;
        body->exec = realloc(body->exec, sizeof(GtkMl_Instruction) * body->cap_exec);
    }

    body->exec[body->len_exec].instr = 0;
    body->exec[body->len_exec].imm.cond = 0;
    body->exec[body->len_exec].imm.category = GTKML_EI_EXPORT;
    body->exec[body->len_exec].imm.imm = GTKML_EI_EXPORT_FLAG_LOCAL;
    ++body->len_exec;

    if (body->len_exec == body->cap_exec) {
        body->cap_exec *= 2;
        body->exec = realloc(body->exec, sizeof(GtkMl_Instruction) * body->cap_exec);
    }

    name = malloc(strlen(linkage_name) + 1);
    strcpy(name, linkage_name);
    GtkMl_S *body_addr = new_address(ctx, NULL, name, 0);
    body->exec[body->len_exec].imm64 = gtk_ml_append_static(b, body_addr);
    ++body->len_exec;

    char *body_name = malloc(strlen("while$$body") + 16);
    snprintf(body_name, strlen("while$$body") + 16, "while$%u$body", while_number);
    char *end_name = malloc(strlen("while$$end") + 16);
    snprintf(end_name, strlen("while$$end") + 16, "while$%u$end", while_number);

    compile_cond_expression(ctx, b, &cond, err, &gtk_ml_car(args), allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
    // if true: jump to body
    gtk_ml_builder_set_cond(b, GTKML_F_ZERO);
    gtk_ml_build_branch_absolute_extended(ctx, b, cond, err, gtk_ml_append_static(b, new_string(ctx, NULL, body_name, strlen(body_name))));
    // else: jump to end
    gtk_ml_build_branch_absolute_extended(ctx, b, cond, err, gtk_ml_append_static(b, new_string(ctx, NULL, end_name, strlen(end_name))));

    args = gtk_ml_cdr(args);
    while (args->kind != GTKML_S_NIL) {
        char *cond_name = malloc(strlen("while$$cond") + 16);
        snprintf(cond_name, strlen("while$$cond") + 16, "while$%u$cond", while_number);

        compile_expression(ctx, b, &body, err, &gtk_ml_car(args), allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
        gtk_ml_build_branch_absolute_extended(ctx, b, body, err, gtk_ml_append_static(b, new_string(ctx, NULL, cond_name, strlen(cond_name))));
        args = gtk_ml_cdr(args);
    }

    *basic_block = end;

    return 1;
}

gboolean builder_add(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S **lhs = &gtk_ml_car(args);
    GtkMl_S **rhs = &gtk_ml_cdar(args);

    if (!compile_expression(ctx, b, basic_block, err, rhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!compile_expression(ctx, b, basic_block, err, lhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_add(ctx, b, *basic_block, err);
}

gboolean builder_sub(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind != GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        GtkMl_S **value = &gtk_ml_car(args);
        GtkMl_S *zero = new_int(ctx, &(*stmt)->span, 0);

        if (!compile_expression(ctx, b, basic_block, err, value, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
            return 0;
        }
        if (!compile_expression(ctx, b, basic_block, err, &zero, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
            return 0;
        }
        return gtk_ml_build_sub(ctx, b, *basic_block, err);
    }

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S **lhs = &gtk_ml_car(args);
    GtkMl_S **rhs = &gtk_ml_cdar(args);

    if (!compile_expression(ctx, b, basic_block, err, rhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!compile_expression(ctx, b, basic_block, err, lhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_sub(ctx, b, *basic_block, err);
}

gboolean builder_mul(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S **lhs = &gtk_ml_car(args);
    GtkMl_S **rhs = &gtk_ml_cdar(args);

    if (!compile_expression(ctx, b, basic_block, err, rhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!compile_expression(ctx, b, basic_block, err, lhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_mul(ctx, b, *basic_block, err);
}

gboolean builder_div(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S **lhs = &gtk_ml_car(args);
    GtkMl_S **rhs = &gtk_ml_cdar(args);

    if (!compile_expression(ctx, b, basic_block, err, rhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!compile_expression(ctx, b, basic_block, err, lhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_div(ctx, b, *basic_block, err);
}

gboolean builder_mod(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S **lhs = &gtk_ml_car(args);
    GtkMl_S **rhs = &gtk_ml_cdar(args);

    if (!compile_expression(ctx, b, basic_block, err, rhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!compile_expression(ctx, b, basic_block, err, lhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_mod(ctx, b, *basic_block, err);
}

gboolean builder_bitnot(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S **value = &gtk_ml_car(args);
    GtkMl_S *ff = new_int(ctx, &(*stmt)->span, 0xfffffffffffffffful);

    if (!compile_expression(ctx, b, basic_block, err, &ff, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!compile_expression(ctx, b, basic_block, err, value, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_bitxor(ctx, b, *basic_block, err);
}

gboolean builder_bitand(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S **lhs = &gtk_ml_car(args);
    GtkMl_S **rhs = &gtk_ml_cdar(args);

    if (!compile_expression(ctx, b, basic_block, err, rhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!compile_expression(ctx, b, basic_block, err, lhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_bitand(ctx, b, *basic_block, err);
}

gboolean builder_bitor(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S **lhs = &gtk_ml_car(args);
    GtkMl_S **rhs = &gtk_ml_cdar(args);

    if (!compile_expression(ctx, b, basic_block, err, rhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!compile_expression(ctx, b, basic_block, err, lhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_bitor(ctx, b, *basic_block, err);
}

gboolean builder_bitxor(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL && gtk_ml_cdr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S **lhs = &gtk_ml_car(args);
    GtkMl_S **rhs = &gtk_ml_cdar(args);

    if (!compile_expression(ctx, b, basic_block, err, rhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!compile_expression(ctx, b, basic_block, err, lhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_bitxor(ctx, b, *basic_block, err);
}

gboolean builder_cmp(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL
            || gtk_ml_cdr(args)->kind == GTKML_S_NIL
            || gtk_ml_cddr(args)->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S *cmp = gtk_ml_car(args);
    GtkMl_S **lhs = &gtk_ml_cdar(args);
    GtkMl_S **rhs = &gtk_ml_cddar(args);

    if (cmp->kind != GTKML_S_INT) {
        *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }
    switch ((GtkMl_Cmp) cmp->value.s_int.value) {
    case GTKML_CMP_EQUAL:
    case GTKML_CMP_NOT_EQUAL:
    case GTKML_CMP_LESS:
    case GTKML_CMP_GREATER:
    case GTKML_CMP_LESS_EQUAL:
    case GTKML_CMP_GREATER_EQUAL:
        break;
    default:
        *err = gtk_ml_error(ctx, "cmp-error", GTKML_ERR_CMP_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    if (!compile_expression(ctx, b, basic_block, err, rhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    if (!compile_expression(ctx, b, basic_block, err, lhs, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }
    return gtk_ml_build_cmp(ctx, b, *basic_block, err, gtk_ml_append_static(b, cmp));
}

gboolean builder_var(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S *var = new_var(ctx, &(*stmt)->span, gtk_ml_car(args));
    if (!var) {
        return 0;
    }

    return compile_expression(ctx, b, basic_block, err, &var, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean builder_vararg(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S *vararg = new_vararg(ctx, &(*stmt)->span, gtk_ml_car(args));
    if (!vararg) {
        return 0;
    }

    return compile_expression(ctx, b, basic_block, err, &vararg, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean builder_quote(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S *quote = new_quote(ctx, &(*stmt)->span, gtk_ml_car(args));
    if (!quote) {
        return 0;
    }

    return compile_expression(ctx, b, basic_block, err, &quote, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean builder_quasiquote(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S *quasiquote = new_quasiquote(ctx, &(*stmt)->span, gtk_ml_car(args));
    if (!quasiquote) {
        return 0;
    }

    return compile_expression(ctx, b, basic_block, err, &quasiquote, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean builder_unquote(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    GtkMl_S *args = gtk_ml_cdr(*stmt);

    if (args->kind == GTKML_S_NIL) {
        *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }

    GtkMl_S *unquote = new_unquote(ctx, &(*stmt)->span, gtk_ml_car(args));
    if (!unquote) {
        return 0;
    }

    return compile_expression(ctx, b, basic_block, err, &unquote, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
}

gboolean compile_cond_expression(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    switch ((*stmt)->kind) {
    case GTKML_S_FALSE:
    case GTKML_S_TRUE:
        return gtk_ml_build_setf_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, *stmt));
    case GTKML_S_NIL:
        return gtk_ml_build_setf_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, new_false(ctx, &(*stmt)->span)));
    case GTKML_S_INT:
    case GTKML_S_FLOAT:
    case GTKML_S_STRING:
    case GTKML_S_KEYWORD:
    case GTKML_S_PROGRAM:
    case GTKML_S_ADDRESS:
    case GTKML_S_LIGHTDATA:
    case GTKML_S_USERDATA:
    case GTKML_S_LAMBDA: 
    case GTKML_S_MACRO:
    case GTKML_S_MAP:
    case GTKML_S_SET:
    case GTKML_S_ARRAY: 
    case GTKML_S_VAR:
        return gtk_ml_build_setf_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, new_true(ctx, &(*stmt)->span)));
    case GTKML_S_LIST:
    case GTKML_S_SYMBOL:
        if (!compile_expression(ctx, b, basic_block, err, stmt, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
            return 0;
        }
        return gtk_ml_build_popf(ctx, b, *basic_block, err);
    case GTKML_S_VARARG:
        *err = gtk_ml_error(ctx, "unimplemented", GTKML_ERR_UNIMPLEMENTED, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    case GTKML_S_QUOTE:
        if ((*stmt)->value.s_quote.expr->kind == GTKML_S_SYMBOL || (*stmt)->value.s_quote.expr->kind == GTKML_S_LIST) {
            return gtk_ml_build_setf_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, new_true(ctx, &(*stmt)->span)));
        } else {
            return compile_cond_expression(ctx, b, basic_block, err, &(*stmt)->value.s_quote.expr, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
        }
    case GTKML_S_QUASIQUOTE:
        if ((*stmt)->value.s_quote.expr->kind == GTKML_S_SYMBOL || (*stmt)->value.s_quote.expr->kind == GTKML_S_LIST) {
            return gtk_ml_build_setf_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, new_true(ctx, &(*stmt)->span)));
        } else {
            return compile_cond_expression(ctx, b, basic_block, err, &(*stmt)->value.s_quote.expr, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
        }
    case GTKML_S_UNQUOTE:
        *err = gtk_ml_error(ctx, "unquote-error", GTKML_ERR_UNQUOTE_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    }
}

gboolean compile_std_call(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, uint64_t function, GtkMl_S *args, gboolean compile_first) {
    int64_t n = 0;
    if (!compile_first) {
        if (!gtk_ml_build_push_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, gtk_ml_car(args)))) {
            return 0;
        }
        args = gtk_ml_cdr(args);
        ++n;
    }
    while (args->kind != GTKML_S_NIL) {
        if (!compile_runtime_expression(ctx, b, basic_block, err, &gtk_ml_car(args))) {
            return 0;
        }
        args = gtk_ml_cdr(args);
        ++n;
    }

    if (!gtk_ml_build_push_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, new_int(ctx, NULL, n)))) {
        return 0;
    }
    if (!gtk_ml_build_call_extended_std(ctx, b, *basic_block, err, gtk_ml_append_static(b, new_int(ctx, NULL, function)))) {
        return 0;
    }

    return 1;
}

struct CompileData {
    GtkMl_Context *ctx;
    GtkMl_Builder *b;
    GtkMl_BasicBlock **basic_block;
    GtkMl_S **err;
    GtkMl_S *stmt;
    gboolean allow_intr;
    gboolean allow_macro;
    gboolean allow_runtime;
    gboolean allow_macro_expansion;
    gboolean result;
};

GTKML_PRIVATE GtkMl_VisitResult compile_quasi_map(GtkMl_HashTrie *ht, void *key, void *value, void *_data) {
    (void) ht;

    struct CompileData *data = _data;
    if (!compile_quasi_expression(data->ctx, data->b, data->basic_block, data->err, key, data->allow_intr, data->allow_macro, data->allow_runtime, data->allow_macro_expansion)) {
        data->result = 0;
        return GTKML_VISIT_BREAK;
    }
    if (!compile_quasi_expression(data->ctx, data->b, data->basic_block, data->err, value, data->allow_intr, data->allow_macro, data->allow_runtime, data->allow_macro_expansion)) {
        data->result = 0;
        return GTKML_VISIT_BREAK;
    }
    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult compile_quasi_set(GtkMl_HashSet *hs, void *key, void *_data) {
    (void) hs;

    struct CompileData *data = _data;
    if (!compile_quasi_expression(data->ctx, data->b, data->basic_block, data->err, key, data->allow_intr, data->allow_macro, data->allow_runtime, data->allow_macro_expansion)) {
        data->result = 0;
        return GTKML_VISIT_BREAK;
    }
    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult compile_quasi_array(GtkMl_Array *array, size_t idx, GtkMl_S *value, void *_data) {
    (void) array;
    (void) idx;

    struct CompileData *data = _data;
    if (!compile_quasi_expression(data->ctx, data->b, data->basic_block, data->err, value, data->allow_intr, data->allow_macro, data->allow_runtime, data->allow_macro_expansion)) {
        data->result = 0;
        return GTKML_VISIT_BREAK;
    }
    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult compile_map(GtkMl_HashTrie *ht, void *key_ptr, void *value_ptr, void *_data) {
    (void) ht;
    GtkMl_S *key = key_ptr;
    GtkMl_S *value = value_ptr;

    struct CompileData *data = _data;
    if (!compile_expression(data->ctx, data->b, data->basic_block, data->err, &key, data->allow_intr, data->allow_macro, data->allow_runtime, data->allow_macro_expansion)) {
        data->result = 0;
        return GTKML_VISIT_BREAK;
    }
    if (!compile_expression(data->ctx, data->b, data->basic_block, data->err, &value, data->allow_intr, data->allow_macro, data->allow_runtime, data->allow_macro_expansion)) {
        data->result = 0;
        return GTKML_VISIT_BREAK;
    }
    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult compile_set(GtkMl_HashSet *hs, void *key_ptr, void *_data) {
    (void) hs;
    GtkMl_S *key = key_ptr;

    struct CompileData *data = _data;
    if (!compile_expression(data->ctx, data->b, data->basic_block, data->err, &key, data->allow_intr, data->allow_macro, data->allow_runtime, data->allow_macro_expansion)) {
        data->result = 0;
        return GTKML_VISIT_BREAK;
    }
    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult compile_array(GtkMl_Array *array, size_t idx, GtkMl_S *value, void *_data) {
    (void) array;
    (void) idx;

    struct CompileData *data = _data;
    if (!compile_expression(data->ctx, data->b, data->basic_block, data->err, &value, data->allow_intr, data->allow_macro, data->allow_runtime, data->allow_macro_expansion)) {
        data->result = 0;
        return GTKML_VISIT_BREAK;
    }
    return GTKML_VISIT_RECURSE;
}

gboolean compile_runtime_expression(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt) {
    return compile_expression(ctx, b, basic_block, err, stmt, 0, 0, 1, 1);
}

gboolean compile_runtime_program(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, const char *linkage_name, GtkMl_S *lambda, gboolean ret) {
    return compile_program(ctx, b, basic_block, err, linkage_name, lambda, ret, 0, 0, 1, 1);
}

gboolean compile_quasi_expression(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    switch (stmt->kind) {
    case GTKML_S_NIL:
    case GTKML_S_TRUE:
    case GTKML_S_FALSE:
    case GTKML_S_INT:
    case GTKML_S_FLOAT:
    case GTKML_S_STRING:
    case GTKML_S_KEYWORD:
    case GTKML_S_PROGRAM:
    case GTKML_S_ADDRESS:
    case GTKML_S_LIGHTDATA:
    case GTKML_S_USERDATA:
    case GTKML_S_SYMBOL:
    case GTKML_S_LAMBDA:
    case GTKML_S_MACRO:
    case GTKML_S_VAR:
    case GTKML_S_VARARG:
    case GTKML_S_QUOTE:
    case GTKML_S_QUASIQUOTE:
        return gtk_ml_build_push_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, stmt));
    case GTKML_S_MAP: {
        int64_t n = gtk_ml_hash_trie_len(&stmt->value.s_map.map);
        struct CompileData data = { ctx, b, basic_block, err, stmt, allow_intr, allow_macro, allow_runtime, allow_macro_expansion, 1 }; 
        gtk_ml_hash_trie_foreach(&stmt->value.s_map.map, compile_quasi_map, &data);
        return gtk_ml_build_map_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, new_int(ctx, NULL, n)));
    }
    case GTKML_S_SET: {
        int64_t n = gtk_ml_hash_set_len(&stmt->value.s_set.set);
        struct CompileData data = { ctx, b, basic_block, err, stmt, allow_intr, allow_macro, allow_runtime, allow_macro_expansion, 1 }; 
        gtk_ml_hash_set_foreach(&stmt->value.s_set.set, compile_quasi_set, &data);
        return gtk_ml_build_set_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, new_int(ctx, NULL, n)));
    }
    case GTKML_S_ARRAY: {
        int64_t n = gtk_ml_array_len(&stmt->value.s_array.array);
        struct CompileData data = { ctx, b, basic_block, err, stmt, allow_intr, allow_macro, allow_runtime, allow_macro_expansion, 1 }; 
        gtk_ml_array_foreach(&stmt->value.s_array.array, compile_quasi_array, &data);
        return gtk_ml_build_array_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, new_int(ctx, NULL, n)));
    }
    case GTKML_S_UNQUOTE:
        return compile_expression(ctx, b, basic_block, err, &stmt->value.s_unquote.expr, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
    case GTKML_S_LIST: {
        GtkMl_S *function = gtk_ml_car(stmt);

        GtkMl_S *args = gtk_ml_cdr(stmt);

        if (function->kind == GTKML_S_SYMBOL) {
            const char *symbol_unquote = "unquote";

            if (function->value.s_symbol.len == strlen(symbol_unquote)
                    && memcmp(function->value.s_symbol.ptr, symbol_unquote, function->value.s_symbol.len) == 0) {
                if (args->kind == GTKML_S_NIL) {
                    *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, stmt->span.ptr != NULL, stmt->span.line, stmt->span.col, 0);
                    return 0;
                }

                return compile_expression(ctx, b, basic_block, err, &gtk_ml_car(args), allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
            }
        }

        int64_t n = 0;
        while (stmt->kind != GTKML_S_NIL) {
            if (!compile_quasi_expression(ctx, b, basic_block, err, gtk_ml_car(stmt), allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
                return 0;
            }
            stmt = gtk_ml_cdr(stmt);
            ++n;
        }
        return gtk_ml_build_list_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, new_int(ctx, NULL, n)));
    }
    }
}

gboolean compile_expression(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    switch ((*stmt)->kind) {
    case GTKML_S_NIL:
    case GTKML_S_TRUE:
    case GTKML_S_FALSE:
    case GTKML_S_INT:
    case GTKML_S_FLOAT:
    case GTKML_S_STRING:
    case GTKML_S_KEYWORD:
    case GTKML_S_PROGRAM:
    case GTKML_S_ADDRESS:
    case GTKML_S_LIGHTDATA:
    case GTKML_S_USERDATA:
        return gtk_ml_build_push_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, *stmt));
    case GTKML_S_SYMBOL:
        return gtk_ml_build_get_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, *stmt));
    case GTKML_S_LAMBDA: {
        char *linkage_name = malloc(strlen("lambda$") + 16);
        snprintf(linkage_name, strlen("lambda$") + 16, "lambda$%u", b->counter++);
        GtkMl_BasicBlock *bb = gtk_ml_append_basic_block(b, linkage_name);
        if (!compile_program(ctx, b, &bb, err, linkage_name, *stmt, 1, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
            return 0;
        }
        return gtk_ml_build_push_extended_addr(ctx, b, *basic_block, err, gtk_ml_append_static(b, new_string(ctx, NULL, linkage_name, strlen(linkage_name))));
    }
    case GTKML_S_MACRO: if (allow_macro) {
        char *linkage_name = malloc(strlen("macro$") + 16);
        snprintf(linkage_name, strlen("macro$") + 16, "macro$%u", b->counter++);
        GtkMl_BasicBlock *bb = gtk_ml_append_basic_block(b, linkage_name);
        if (!compile_program(ctx, b, &bb, err, linkage_name, *stmt, 1, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
            return 0;
        }
        return gtk_ml_build_push_extended_addr(ctx, b, *basic_block, err, gtk_ml_append_static(b, new_string(ctx, NULL, linkage_name, strlen(linkage_name))));
    }
    case GTKML_S_MAP: {
        int64_t n = gtk_ml_hash_trie_len(&(*stmt)->value.s_map.map);
        struct CompileData data = { ctx, b, basic_block, err, (*stmt), allow_intr, allow_macro, allow_runtime, allow_macro_expansion, 1 }; 
        gtk_ml_hash_trie_foreach(&(*stmt)->value.s_map.map, compile_map, &data);
        return gtk_ml_build_map_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, new_int(ctx, NULL, n)));
    }
    case GTKML_S_SET: {
        int64_t n = gtk_ml_hash_set_len(&(*stmt)->value.s_set.set);
        struct CompileData data = { ctx, b, basic_block, err, (*stmt), allow_intr, allow_macro, allow_runtime, allow_macro_expansion, 1 }; 
        gtk_ml_hash_set_foreach(&(*stmt)->value.s_set.set, compile_set, &data);
        return gtk_ml_build_set_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, new_int(ctx, NULL, n)));
    }
    case GTKML_S_ARRAY: {
        int64_t n = gtk_ml_array_len(&(*stmt)->value.s_array.array);
        struct CompileData data = { ctx, b, basic_block, err, (*stmt), allow_intr, allow_macro, allow_runtime, allow_macro_expansion, 1 }; 
        gtk_ml_array_foreach(&(*stmt)->value.s_array.array, compile_array, &data);
        return gtk_ml_build_array_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, new_int(ctx, NULL, n)));
    }
    case GTKML_S_VAR:
        compile_expression(ctx, b, basic_block, err, &(*stmt)->value.s_var.expr, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
        return gtk_ml_build_var_imm(ctx, b, *basic_block, err);
    case GTKML_S_VARARG:
        *err = gtk_ml_error(ctx, "unimplemented", GTKML_ERR_UNIMPLEMENTED, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    case GTKML_S_QUOTE:
        return gtk_ml_build_push_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, (*stmt)->value.s_quote.expr));
    case GTKML_S_QUASIQUOTE:
        return compile_quasi_expression(ctx, b, basic_block, err, (*stmt)->value.s_quasiquote.expr, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
    case GTKML_S_UNQUOTE:
        *err = gtk_ml_error(ctx, "unquote-error", GTKML_ERR_UNQUOTE_ERROR, (*stmt)->span.ptr != NULL, (*stmt)->span.line, (*stmt)->span.col, 0);
        return 0;
    case GTKML_S_LIST: {
        GtkMl_S **function = &gtk_ml_car(*stmt);
        GtkMl_S *args = gtk_ml_cdr(*stmt);

        if ((*function)->kind == GTKML_S_SYMBOL) {
            size_t len = (*function)->value.s_symbol.len;
            const char *ptr = (*function)->value.s_symbol.ptr;

            for (size_t i = 0; i < b->len_builder; i++) {
                GtkMl_BuilderMacro *bm = b->builders + i;
                if (strlen(bm->name) == len && strncmp(bm->name, ptr, len) == 0) {
                    if (bm->require_intrinsic) {
                        if (allow_intr) {
                            return bm->fn(ctx, b, basic_block, err, stmt, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
                        }
                    } else if (bm->require_macro) {
                        if (allow_macro) {
                            return bm->fn(ctx, b, basic_block, err, stmt, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
                        }
                    } else if (bm->require_runtime) {
                        if (allow_runtime) {
                            return bm->fn(ctx, b, basic_block, err, stmt, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
                        }
                    } else {
                        return bm->fn(ctx, b, basic_block, err, stmt, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
                    }
                }
            }
        }

        if (!allow_intr && (*function)->kind == GTKML_S_SYMBOL) {
            size_t len = (*function)->value.s_symbol.len;
            char *linkage_name = malloc(len + 1);
            memcpy(linkage_name, (*function)->value.s_symbol.ptr, len);
            linkage_name[len] = 0;

            GtkMl_S *_err = NULL;
            GtkMl_S *program = gtk_ml_get_export(b->intr_ctx, &_err, linkage_name);
            free(linkage_name);
            if (program) {
                gtk_ml_define(b->intr_ctx, new_symbol(ctx, NULL, 0, "CTX", strlen("CTX")), new_lightdata(b->intr_ctx, NULL, ctx));
                gtk_ml_define(b->intr_ctx, new_symbol(ctx, NULL, 0, "CODE-BUILDER", strlen("CODE-BUILDER")), new_lightdata(b->intr_ctx, NULL, b));
                gtk_ml_define(b->intr_ctx, new_symbol(ctx, NULL, 0, "BASIC-BLOCK", strlen("BASIC-BLOCK")), new_lightdata(b->intr_ctx, NULL, basic_block));
                return gtk_ml_run_program(b->intr_ctx, err, program, args);
            }
        }

        if (allow_macro_expansion && (*function)->kind == GTKML_S_SYMBOL) {
            size_t len = (*function)->value.s_symbol.len;
            char *linkage_name = malloc(len + 1);
            memcpy(linkage_name, (*function)->value.s_symbol.ptr, len);
            linkage_name[len] = 0;

            GtkMl_S *_err = NULL;
            GtkMl_S *program = gtk_ml_get_export(b->macro_ctx, &_err, linkage_name);
            free(linkage_name);
            if (program) {
                if (!gtk_ml_run_program(b->macro_ctx, err, program, args)) {
                    return 0;
                }

                GtkMl_S *result = gtk_ml_peek(b->macro_ctx);

                if (!result) {
                    return 0;
                }

                *stmt = result;

                return compile_expression(ctx, b, basic_block, err, stmt, allow_intr, allow_macro, allow_runtime, allow_macro_expansion);
            }
        }

        int64_t n_args = 0;
        while (args->kind != GTKML_S_NIL) {
            GtkMl_S **arg = &gtk_ml_car(args);
            if (!compile_expression(ctx, b, basic_block, err, arg, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
                return 0;
            }
            ++n_args;
            args = gtk_ml_cdr(args);
        }

        if (!gtk_ml_build_push_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, new_int(ctx, NULL, n_args)))) {
            return 0;
        }

        if (!compile_expression(ctx, b, basic_block, err, function, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
            return 0;
        }

        return gtk_ml_build_call(ctx, b, *basic_block, err);
    }
    }
}

gboolean compile_body(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S *body, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    while (body->kind != GTKML_S_NIL) {
        GtkMl_S **stmt = &gtk_ml_car(body);
        if (!compile_expression(ctx, b, basic_block, err, stmt, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
            return 0;
        }
        body = gtk_ml_cdr(body);
    }

    return 1;
}

gboolean compile_program(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, const char *linkage_name, GtkMl_S *lambda, gboolean ret, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) {
    if ((!allow_macro || lambda->kind != GTKML_S_MACRO) && lambda->kind != GTKML_S_LAMBDA) {
        *err = gtk_ml_error(ctx, "program-error", GTKML_ERR_PROGRAM_ERROR, lambda->span.ptr != NULL, lambda->span.line, lambda->span.col, 0);
        return 0;
    }

    if ((*basic_block)->len_exec == (*basic_block)->cap_exec) {
        (*basic_block)->cap_exec *= 2;
        (*basic_block)->exec = realloc((*basic_block)->exec, sizeof(GtkMl_Instruction) * (*basic_block)->cap_exec);
    }

    (*basic_block)->exec[(*basic_block)->len_exec].instr = 0;
    (*basic_block)->exec[(*basic_block)->len_exec].imm.cond = 0;
    (*basic_block)->exec[(*basic_block)->len_exec].imm.category = GTKML_EI_EXPORT;
    (*basic_block)->exec[(*basic_block)->len_exec].imm.imm = GTKML_EI_EXPORT_FLAG_NONE;
    ++(*basic_block)->len_exec;

    if ((*basic_block)->len_exec == (*basic_block)->cap_exec) {
        (*basic_block)->cap_exec *= 2;
        (*basic_block)->exec = realloc((*basic_block)->exec, sizeof(GtkMl_Instruction) * (*basic_block)->cap_exec);
    }

    char *name = malloc(strlen(linkage_name) + 1);
    strcpy(name, linkage_name);
    GtkMl_S *program = new_program(ctx, &lambda->span, name, 0, lambda->value.s_lambda.args, lambda->value.s_lambda.body, lambda->value.s_lambda.capture);
    (*basic_block)->exec[(*basic_block)->len_exec].imm64 = gtk_ml_append_static(b, program);
    ++(*basic_block)->len_exec;

    GtkMl_S *params = lambda->value.s_lambda.args;
    GtkMl_S *revparams = new_nil(ctx, NULL);
    while (params->kind != GTKML_S_NIL) {
        revparams = new_list(ctx, NULL, gtk_ml_car(params), revparams);
        params = gtk_ml_cdr(params);
    }
    if (!gtk_ml_build_push_extended_imm(ctx, b, *basic_block, err, gtk_ml_append_static(b, revparams))) {
        return 0;
    }

    if (!gtk_ml_build_bind_args(ctx, b, *basic_block, err)) {
        return 0;
    }

    if (!compile_body(ctx, b, basic_block, err, lambda->value.s_lambda.body, allow_intr, allow_macro, allow_runtime, allow_macro_expansion)) {
        return 0;
    }

    if (ret) {
        if (!gtk_ml_build_ret(ctx, b, *basic_block, err)) {
            return 0;
        }
    }

    return 1;
}

void gtk_ml_builder_set_cond(GtkMl_Builder *b, unsigned int flags) {
    b->flags = flags;
}

unsigned int gtk_ml_builder_clear_cond(GtkMl_Builder *b) {
    unsigned int flags = b->flags;
    b->flags = 0;
    return flags;
}

gboolean gtk_ml_build_halt(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].arith.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].arith.category = GTKML_I_ARITH;
    basic_block->exec[basic_block->len_exec].arith.opcode = GTKML_IA_HALT;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_push_extended_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64) {
    if (imm64 < ((GtkMl_Static) 1 << 32)) {
        return gtk_ml_build_push_imm(ctx, b, basic_block, err, imm64);
    }

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_EI_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_EII_PUSH_EXT_IMM;
    ++basic_block->len_exec;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].imm64 = imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_push_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_PUSH_IMM;
    basic_block->exec[basic_block->len_exec].imm.imm = imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_setf_extended_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64) {
    if (imm64 < ((GtkMl_Static) 1 << 32)) {
        return gtk_ml_build_setf_imm(ctx, b, basic_block, err, imm64);
    }

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_EI_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_EII_SETF_EXT_IMM;
    ++basic_block->len_exec;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].imm64 = imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_setf_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_SETF_IMM;
    basic_block->exec[basic_block->len_exec].imm.imm = imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_popf(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_POPF;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_push_extended_addr(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static linkage_name) {
    if (linkage_name < ((GtkMl_Static) 1 << 32)) {
        return gtk_ml_build_push_addr(ctx, b, basic_block, err, linkage_name);
    }

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_EI_IMM | GTKML_EI_IMM_EXTERN;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_EII_PUSH_EXT_IMM;
    ++basic_block->len_exec;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].imm64 = linkage_name;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_push_addr(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static linkage_name) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM | GTKML_I_IMM_EXTERN;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_PUSH_IMM;

    basic_block->exec[basic_block->len_exec].imm.imm = linkage_name;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_pop(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_POP;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_define(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].arith.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].arith.category = GTKML_I_ARITH;
    basic_block->exec[basic_block->len_exec].arith.opcode = GTKML_IA_DEFINE;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_bind(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].arith.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].arith.category = GTKML_I_ARITH;
    basic_block->exec[basic_block->len_exec].arith.opcode = GTKML_IA_BIND;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_bind_args(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].arith.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].arith.category = GTKML_I_ARITH;
    basic_block->exec[basic_block->len_exec].arith.opcode = GTKML_IA_BIND_ARGS;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_get_extended_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64) {
    if (imm64 < ((GtkMl_Static) 1 << 32)) {
        return gtk_ml_build_get_imm(ctx, b, basic_block, err, imm64);
    }

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_EI_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_EII_GET_EXT_IMM;
    ++basic_block->len_exec;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].imm64 = imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_get_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_GET_IMM;
    basic_block->exec[basic_block->len_exec].imm.imm = imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_list_extended_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64) {
    if (imm64 < ((GtkMl_Static) 1 << 32)) {
        return gtk_ml_build_list_imm(ctx, b, basic_block, err, imm64);
    }

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_EI_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_EII_LIST_EXT_IMM;
    ++basic_block->len_exec;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].imm64 = imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_list_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_LIST_IMM;
    basic_block->exec[basic_block->len_exec].imm.imm = imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_map_extended_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64) {
    if (imm64 < ((GtkMl_Static) 1 << 32)) {
        return gtk_ml_build_map_imm(ctx, b, basic_block, err, imm64);
    }

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_EI_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_EII_MAP_EXT_IMM;
    ++basic_block->len_exec;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].imm64 = imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_map_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_MAP_IMM;
    basic_block->exec[basic_block->len_exec].imm.imm = imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_set_extended_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64) {
    if (imm64 < ((GtkMl_Static) 1 << 32)) {
        return gtk_ml_build_set_imm(ctx, b, basic_block, err, imm64);
    }

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_EI_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_EII_SET_EXT_IMM;
    ++basic_block->len_exec;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].imm64 = imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_set_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_SET_IMM;
    basic_block->exec[basic_block->len_exec].imm.imm = imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_array_extended_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64) {
    if (imm64 < ((GtkMl_Static) 1 << 32)) {
        return gtk_ml_build_array_imm(ctx, b, basic_block, err, imm64);
    }

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_EI_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_EII_ARRAY_EXT_IMM;
    ++basic_block->len_exec;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].imm64 = imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_array_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_ARRAY_IMM;
    basic_block->exec[basic_block->len_exec].imm.imm = imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_setmm_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_SETMM_IMM;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_getmm_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_GETMM_IMM;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_var_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_VAR_IMM;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_getvar_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_GETVAR_IMM;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_assignvar_imm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_I_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_II_ASSIGNVAR_IMM;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_call_extended_std(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64) {
    if (imm64 < ((GtkMl_Static) 1 << 48)) {
        return gtk_ml_build_call_std(ctx, b, basic_block, err, imm64);
    }

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].br.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].br.category = GTKML_EI_BR;
    basic_block->exec[basic_block->len_exec].br.opcode = GTKML_EIBR_CALL_EXT_STD;
    ++basic_block->len_exec;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].imm64 = imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_call_std(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].br.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].br.category = GTKML_I_BR;
    basic_block->exec[basic_block->len_exec].br.opcode = GTKML_IBR_CALL_STD;
    basic_block->exec[basic_block->len_exec].br.imm = imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_call(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].imm64 = 0;
    basic_block->exec[basic_block->len_exec].br.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].br.category = GTKML_I_BR;
    basic_block->exec[basic_block->len_exec].br.opcode = GTKML_IBR_CALL;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_branch_absolute_extended(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static linkage_name) {
    if (linkage_name < ((GtkMl_Static) 1 << 32)) {
        return gtk_ml_build_branch_absolute(ctx, b, basic_block, err, linkage_name);
    }

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].br.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].br.category = GTKML_EI_BR | GTKML_EI_BR_EXTERN;
    basic_block->exec[basic_block->len_exec].br.opcode = GTKML_EIBR_BRANCH_ABSOLUTE_EXT;
    ++basic_block->len_exec;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].imm64 = linkage_name;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_branch_absolute(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static linkage_name) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].br.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].br.category = GTKML_I_BR | GTKML_I_BR_EXTERN;
    basic_block->exec[basic_block->len_exec].br.opcode = GTKML_IBR_BRANCH_ABSOLUTE;
    basic_block->exec[basic_block->len_exec].br.imm = linkage_name;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_ret(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].br.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].br.category = GTKML_I_BR;
    basic_block->exec[basic_block->len_exec].br.opcode = GTKML_IBR_RET;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_add(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].br.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].br.category = GTKML_I_ARITH;
    basic_block->exec[basic_block->len_exec].br.opcode = GTKML_IA_ADD;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_sub(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].br.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].br.category = GTKML_I_ARITH;
    basic_block->exec[basic_block->len_exec].br.opcode = GTKML_IA_SUBTRACT;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_mul(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].br.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].br.category = GTKML_I_ARITH;
    basic_block->exec[basic_block->len_exec].br.opcode = GTKML_IA_SIGNED_MULTIPLY;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_div(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].br.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].br.category = GTKML_I_ARITH;
    basic_block->exec[basic_block->len_exec].br.opcode = GTKML_IA_SIGNED_DIVIDE;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_mod(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].br.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].br.category = GTKML_I_ARITH;
    basic_block->exec[basic_block->len_exec].br.opcode = GTKML_IA_SIGNED_MODULO;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_cmp(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err, GtkMl_Static imm64) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].imm.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].imm.category = GTKML_EI_IMM;
    basic_block->exec[basic_block->len_exec].imm.opcode = GTKML_EII_CMP_EXT_IMM;
    ++basic_block->len_exec;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].imm64 = imm64;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_bitand(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].br.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].br.category = GTKML_I_ARITH;
    basic_block->exec[basic_block->len_exec].br.opcode = GTKML_IA_BIT_AND;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_bitor(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].br.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].br.category = GTKML_I_ARITH;
    basic_block->exec[basic_block->len_exec].br.opcode = GTKML_IA_BIT_AND;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_build_bitxor(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock *basic_block, GtkMl_S **err) {
    (void) ctx;
    (void) err;

    if (basic_block->len_exec == basic_block->cap_exec) {
        basic_block->cap_exec *= 2;
        basic_block->exec = realloc(basic_block->exec, sizeof(GtkMl_Instruction) * basic_block->cap_exec);
    }

    basic_block->exec[basic_block->len_exec].instr = 0;
    basic_block->exec[basic_block->len_exec].br.cond = gtk_ml_builder_clear_cond(b);
    basic_block->exec[basic_block->len_exec].br.category = GTKML_I_ARITH;
    basic_block->exec[basic_block->len_exec].br.opcode = GTKML_IA_BIT_XOR;
    ++basic_block->len_exec;

    return 1;
}

gboolean gtk_ml_compile_intrinsics(GtkMl_Builder *b, GtkMl_BasicBlock **start, GtkMl_S **err, GtkMl_S *lambda) {
    GtkMl_S *prev = lambda->value.s_lambda.body;
    GtkMl_S *next = prev;
    while (next->kind != GTKML_S_NIL) {
        GtkMl_S *stmt = gtk_ml_car(next);

        GtkMl_S *function = gtk_ml_car(stmt);

        GtkMl_S *args = gtk_ml_cdr(stmt);

        if (function->kind == GTKML_S_SYMBOL) {
            const char *symbol_define_intrinsic = "define-intrinsic";

            if (function->value.s_symbol.len == strlen(symbol_define_intrinsic)
                    && memcmp(function->value.s_symbol.ptr, symbol_define_intrinsic, function->value.s_symbol.len) == 0) {
                GtkMl_S *intrinsic_definition = gtk_ml_car(args);
                GtkMl_S *intrinsic_name = gtk_ml_car(intrinsic_definition);
                GtkMl_S *intrinsic_args = gtk_ml_cdr(intrinsic_definition);
                GtkMl_S *intrinsic_body = gtk_ml_cdr(args);

                GtkMl_S *intrinsic = new_lambda(b->intr_ctx, NULL, intrinsic_args, intrinsic_body, local_scope(b->intr_ctx));
                if (!intrinsic) {
                    return 0;
                }

                size_t len = intrinsic_name->value.s_symbol.len;
                char *linkage_name = malloc(len + 1);
                memcpy(linkage_name, intrinsic_name->value.s_symbol.ptr, len);
                linkage_name[len] = 0;

                GtkMl_BasicBlock *basic_block = gtk_ml_append_basic_block(b, linkage_name);

                if (!compile_program(b->intr_ctx, b, &basic_block, err, linkage_name, intrinsic, 1, 1, 0, 0, 0)) {
                    free(linkage_name);
                    return 0;
                }

                if (!gtk_ml_build_push_extended_addr(b->intr_ctx, b, *start, err, gtk_ml_append_static(b, new_string(b->intr_ctx, NULL, linkage_name, strlen(linkage_name))))) {
                    return 0;
                }
                if (!gtk_ml_build_push_extended_imm(b->intr_ctx, b, *start, err, gtk_ml_append_static(b, intrinsic_name))) {
                    return 0;
                }
                if (!gtk_ml_build_define(b->intr_ctx, b, *start, err)) {
                    return 0;
                }
            }
        }
        next = gtk_ml_cdr(next);
    }
    
    return 1;
}

gboolean gtk_ml_compile_macros(GtkMl_Builder *b, GtkMl_BasicBlock **start, GtkMl_S **err, GtkMl_S *lambda) {
    GtkMl_S *prev = lambda->value.s_lambda.body;
    GtkMl_S *next = prev;
    while (next->kind != GTKML_S_NIL) {
        GtkMl_S *stmt = gtk_ml_car(next);

        GtkMl_S *function = gtk_ml_car(stmt);

        GtkMl_S *args = gtk_ml_cdr(stmt);

        if (function->kind == GTKML_S_SYMBOL) {
            const char *symbol_define_macro = "define-macro";

            if (function->value.s_symbol.len == strlen(symbol_define_macro)
                    && memcmp(function->value.s_symbol.ptr, symbol_define_macro, function->value.s_symbol.len) == 0) {
                GtkMl_S *macro_definition = gtk_ml_car(args);
                GtkMl_S *macro_name = gtk_ml_car(macro_definition);
                GtkMl_S *macro_args = gtk_ml_cdr(macro_definition);
                GtkMl_S *macro_body = gtk_ml_cdr(args);

                GtkMl_S *macro = new_macro(b->macro_ctx, NULL, macro_args, macro_body, local_scope(b->macro_ctx));
                if (!macro) {
                    return 0;
                }

                size_t len = macro_name->value.s_symbol.len;
                char *linkage_name = malloc(len + 1);
                memcpy(linkage_name, macro_name->value.s_symbol.ptr, len);
                linkage_name[len] = 0;

                GtkMl_BasicBlock *basic_block = gtk_ml_append_basic_block(b, linkage_name);

                if (!compile_program(b->macro_ctx, b, &basic_block, err, linkage_name, macro, 1, 0, 1, 0, 0)) {
                    free(linkage_name);
                    return 0;
                }

                if (!gtk_ml_build_push_extended_addr(b->macro_ctx, b, *start, err, gtk_ml_append_static(b, new_string(b->macro_ctx, NULL, linkage_name, strlen(linkage_name))))) {
                    return 0;
                }
                if (!gtk_ml_build_push_extended_imm(b->macro_ctx, b, *start, err, gtk_ml_append_static(b, macro_name))) {
                    return 0;
                }
                if (!gtk_ml_build_define(b->macro_ctx, b, *start, err)) {
                    return 0;
                }
            }
        }
        next = gtk_ml_cdr(next);
    }
    
    return 1;
}

gboolean gtk_ml_compile(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_S **err, GtkMl_S *lambda) {
    const char *linkage_name = "_start";
    GtkMl_BasicBlock *basic_block = gtk_ml_append_basic_block(b, linkage_name);

    if (!compile_runtime_program(ctx, b, &basic_block, err, linkage_name, lambda, 0)) {
        return 0;
    }

    if (!gtk_ml_build_halt(ctx, b, basic_block, err)) {
        return 0;
    }
    
    return 1;
}

gboolean gtk_ml_compile_program(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_S **err, GtkMl_S *lambda) {
    GtkMl_BasicBlock *intr_start = gtk_ml_append_basic_block(b, "_start");

    if (intr_start->len_exec == intr_start->cap_exec) {
        intr_start->cap_exec *= 2;
        intr_start->exec = realloc(intr_start->exec, sizeof(GtkMl_Instruction) * intr_start->cap_exec);
    }

    intr_start->exec[intr_start->len_exec].instr = 0;
    intr_start->exec[intr_start->len_exec].imm.cond = 0;
    intr_start->exec[intr_start->len_exec].imm.category = GTKML_EI_EXPORT;
    intr_start->exec[intr_start->len_exec].imm.imm = GTKML_EI_EXPORT_FLAG_NONE;
    ++intr_start->len_exec;

    if (intr_start->len_exec == intr_start->cap_exec) {
        intr_start->cap_exec *= 2;
        intr_start->exec = realloc(intr_start->exec, sizeof(GtkMl_Instruction) * intr_start->cap_exec);
    }

    char *name = malloc(strlen("_start") + 1);
    strcpy(name, "_start");
    GtkMl_S *program = new_program(b->intr_ctx, &lambda->span, name, 0, lambda->value.s_lambda.args, lambda->value.s_lambda.body, lambda->value.s_lambda.capture);
    intr_start->exec[intr_start->len_exec].imm64 = gtk_ml_append_static(b, program);
    ++intr_start->len_exec;

    if (!gtk_ml_compile_intrinsics(b, &intr_start, err, lambda)) {
        return 0;
    }

    if (!gtk_ml_build_halt(ctx, b, intr_start, err)) {
        return 0;
    }

    GtkMl_Program intrinsics;
    if (!gtk_ml_build_intrinsics(&intrinsics, err, b)) {
        return 0;
    }
    gtk_ml_load_program(b->intr_ctx, &intrinsics);
    gtk_ml_del_program(&intrinsics);
    GtkMl_S *intr_program = gtk_ml_get_export(b->intr_ctx, err, "_start");
    if (!intr_program) {
        return 0;
    }
    if (!gtk_ml_run_program(b->intr_ctx, err, intr_program, new_nil(b->intr_ctx, NULL))) {
        return 0;
    }

    GtkMl_BasicBlock *macro_start = gtk_ml_append_basic_block(b, "_start");

    if (macro_start->len_exec == macro_start->cap_exec) {
        macro_start->cap_exec *= 2;
        macro_start->exec = realloc(macro_start->exec, sizeof(GtkMl_Instruction) * macro_start->cap_exec);
    }

    macro_start->exec[macro_start->len_exec].instr = 0;
    macro_start->exec[macro_start->len_exec].imm.cond = 0;
    macro_start->exec[macro_start->len_exec].imm.category = GTKML_EI_EXPORT;
    macro_start->exec[macro_start->len_exec].imm.imm = GTKML_EI_EXPORT_FLAG_NONE;
    ++macro_start->len_exec;

    if (macro_start->len_exec == macro_start->cap_exec) {
        macro_start->cap_exec *= 2;
        macro_start->exec = realloc(macro_start->exec, sizeof(GtkMl_Instruction) * macro_start->cap_exec);
    }

    name = malloc(strlen("_start") + 1);
    strcpy(name, "_start");
    program = new_program(b->macro_ctx, &lambda->span, name, 0, lambda->value.s_lambda.args, lambda->value.s_lambda.body, lambda->value.s_lambda.capture);
    macro_start->exec[macro_start->len_exec].imm64 = gtk_ml_append_static(b, program);
    ++macro_start->len_exec;

    if (!gtk_ml_compile_macros(b, &macro_start, err, lambda)) {
        return 0;
    }

    if (!gtk_ml_build_halt(ctx, b, macro_start, err)) {
        return 0;
    }

    GtkMl_Program macros;
    if (!gtk_ml_build_macros(&macros, err, b)) {
        return 0;
    }
    gtk_ml_load_program(b->macro_ctx, &macros);
    gtk_ml_del_program(&macros);
    GtkMl_S *macro_program = gtk_ml_get_export(b->macro_ctx, err, "_start");
    if (!macro_program) {
        return 0;
    }
    if (!gtk_ml_run_program(b->macro_ctx, err, macro_program, new_nil(b->macro_ctx, NULL))) {
        return 0;
    }

    return gtk_ml_compile(ctx, b, err, lambda);
}

void gtk_ml_push(GtkMl_Context *ctx, GtkMl_S *value) {
    ctx->vm->stack[ctx->vm->reg[GTKML_R_SP].sp].s_expr = value;
    ctx->vm->reg[GTKML_R_SP].sp++;
}

GtkMl_S *gtk_ml_pop(GtkMl_Context *ctx) {
    --ctx->vm->reg[GTKML_R_SP].sp;
    return ctx->vm->stack[ctx->vm->reg[GTKML_R_SP].sp].s_expr;
}

GtkMl_S *gtk_ml_peek(GtkMl_Context *ctx) {
    return ctx->vm->stack[ctx->vm->reg[GTKML_R_SP].sp - 1].s_expr;
}

void gtk_ml_enter(GtkMl_Context *ctx) {
    ctx->bindings = new_list(ctx, NULL, new_map(ctx, NULL, NULL), ctx->bindings);
}

void gtk_ml_leave(GtkMl_Context *ctx) {
    ctx->bindings = gtk_ml_cdr(ctx->bindings);
}

void gtk_ml_define(GtkMl_Context *ctx, GtkMl_S *key, GtkMl_S *value) {
    GtkMl_S *new_context = new_map(ctx, NULL, NULL);
    gtk_ml_hash_trie_insert(&new_context->value.s_map.map, &(*ctx->top_scope)->value.s_map.map, key, value);
    *ctx->top_scope = new_context;
}

void gtk_ml_bind(GtkMl_Context *ctx, GtkMl_S *key, GtkMl_S *value) {
    GtkMl_S *new_context = new_map(ctx, NULL, NULL);
    gtk_ml_hash_trie_insert(&new_context->value.s_map.map, &gtk_ml_car(ctx->bindings)->value.s_map.map, key, value);
    gtk_ml_car(ctx->bindings) = new_context;
}

GTKML_PRIVATE GtkMl_S *map_find(GtkMl_S *map, GtkMl_S *key) {
    return gtk_ml_hash_trie_get(&map->value.s_map.map, key);
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
    case GTKML_S_STRING:
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
        gtk_ml_array_foreach(&s->value.s_array.array, mark_array, NULL);
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
    case GTKML_S_ADDRESS:
    case GTKML_S_LIGHTDATA:
        break;
    case GTKML_S_STRING:
        // const cast required
        free((void *) s->value.s_string.ptr);
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
        delete(ctx, gtk_ml_cdr(s));
        delete(ctx, gtk_ml_car(s));
        break;
    case GTKML_S_MAP:
        gtk_ml_del_hash_trie(ctx, &s->value.s_map.map, gtk_ml_delete_value);
        break;
    case GTKML_S_SET:
        gtk_ml_del_hash_set(ctx, &s->value.s_set.set, gtk_ml_delete_value);
        break;
    case GTKML_S_ARRAY:
        gtk_ml_del_array(ctx, &s->value.s_array.array, delete);
        break;
    case GTKML_S_VAR:
        delete(ctx, s->value.s_var.expr);
        break;
    case GTKML_S_VARARG:
        delete(ctx, s->value.s_vararg.expr);
        break;
    case GTKML_S_QUOTE:
        delete(ctx, s->value.s_quote.expr);
        break;
    case GTKML_S_QUASIQUOTE:
        delete(ctx, s->value.s_quasiquote.expr);
        break;
    case GTKML_S_UNQUOTE:
        delete(ctx, s->value.s_unquote.expr);
        break;
    case GTKML_S_PROGRAM:
        free((void *) s->value.s_program.linkage_name);
        delete(ctx, s->value.s_program.args);
        delete(ctx, s->value.s_program.capture);
        break;
    case GTKML_S_LAMBDA:
        delete(ctx, s->value.s_lambda.args);
        delete(ctx, s->value.s_lambda.body);
        delete(ctx, s->value.s_lambda.capture);
        break;
    case GTKML_S_MACRO:
        delete(ctx, s->value.s_macro.args);
        delete(ctx, s->value.s_macro.body);
        delete(ctx, s->value.s_macro.capture);
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
    case GTKML_S_LIGHTDATA:
    case GTKML_S_LIST:
    case GTKML_S_VAR:
    case GTKML_S_VARARG:
    case GTKML_S_QUOTE:
    case GTKML_S_QUASIQUOTE:
    case GTKML_S_UNQUOTE:
    case GTKML_S_LAMBDA:
    case GTKML_S_ADDRESS:
    case GTKML_S_MACRO:
        break;
    case GTKML_S_MAP:
        gtk_ml_del_hash_trie(ctx, &s->value.s_map.map, gtk_ml_delete_void_reference);
        break;
    case GTKML_S_SET:
        gtk_ml_del_hash_set(ctx, &s->value.s_set.set, gtk_ml_delete_void_reference);
        break;
    case GTKML_S_ARRAY:
        gtk_ml_del_array(ctx, &s->value.s_array.array, gtk_ml_delete_value_reference);
        break;
    case GTKML_S_STRING:
        // const cast required
        free((void *) s->value.s_string.ptr);
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
    case GTKML_S_PROGRAM:
        free((void *) s->value.s_program.linkage_name);
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
            delete(ctx, unreachable);
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
    if (data->n < gtk_ml_array_len(array)) {
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
        gtk_ml_array_foreach(&expr->value.s_array.array, dumpf_array, &data);
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
        fprintf(stream, "(program \"%s\" 0x%lx ", expr->value.s_program.linkage_name, expr->value.s_program.addr);
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

GTKML_PRIVATE void activate_program(GtkApplication* app, gpointer userdata) {
    (void) app;

    GtkMl_S *args = userdata;
    GtkMl_S *ctx_expr = gtk_ml_car(args);
    GtkMl_S *app_expr = gtk_ml_cdar(args);
    GtkMl_S *program_expr = gtk_ml_cddar(args);

    GtkMl_Context *ctx = ctx_expr->value.s_lightdata.userdata;

    GtkMl_S *err;
    uint64_t pc = ctx->vm->reg[GTKML_R_PC].pc;
    uint64_t flags = ctx->vm->reg[GTKML_R_FLAGS].flags & GTKML_F_TOPCALL;
    if (gtk_ml_run_program(ctx, &err, program_expr, new_list(ctx, NULL, app_expr, new_nil(ctx, NULL)))) {
        GtkMl_S *result = gtk_ml_pop(ctx);
        app_expr->value.s_userdata.keep = new_list(ctx, NULL, result, app_expr->value.s_userdata.keep);
    } else {
        gtk_ml_dumpf(ctx, stderr, NULL, err);
    }
    ctx->vm->reg[GTKML_R_FLAGS].flags &= ~GTKML_F_TOPCALL;
    ctx->vm->reg[GTKML_R_FLAGS].flags |= flags;
    ctx->vm->reg[GTKML_R_PC].pc = pc;
}

GTKML_PRIVATE GtkMl_S *vm_std_application(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr) {
    (void) err;

    GtkMl_S *map_expr = gtk_ml_pop(ctx);
    GtkMl_S *flags_expr = gtk_ml_pop(ctx);
    GtkMl_S *id_expr = gtk_ml_pop(ctx);
    GtkMl_S *application = gtk_ml_pop(ctx);
    (void) application;

    GtkApplication *app = gtk_application_new(id_expr->value.s_string.ptr, flags_expr->value.s_int.value);
    GtkMl_S *app_expr = new_userdata(ctx, &expr->span, app, gtk_ml_object_unref);

    GtkMl_S *activate = map_find(map_expr, new_keyword(ctx, NULL, 0, "activate", strlen("activate")));
    if (activate) {
        GtkMl_S *ctx_expr = new_lightdata(ctx, NULL, ctx);
        GtkMl_S *userdata = new_list(ctx, NULL, ctx_expr, new_list(ctx, NULL, app_expr, new_list(ctx, NULL, activate, new_nil(ctx, NULL))));
        app_expr->value.s_userdata.keep = new_list(ctx, &app_expr->span, userdata, app_expr->value.s_userdata.keep);
        g_signal_connect(app, "activate", G_CALLBACK(activate_program), userdata);
    }

    return app_expr;
}

GTKML_PRIVATE GtkMl_S *vm_std_new_window(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr) {
    (void) err;

    GtkMl_S *height_expr = gtk_ml_pop(ctx);
    GtkMl_S *width_expr = gtk_ml_pop(ctx);
    GtkMl_S *title_expr = gtk_ml_pop(ctx);
    GtkMl_S *app_expr = gtk_ml_pop(ctx);
    GtkMl_S *new_window = gtk_ml_pop(ctx);
    (void) new_window;

    GtkWidget *window = gtk_application_window_new(app_expr->value.s_userdata.userdata);
    gtk_window_set_title(GTK_WINDOW(window), title_expr->value.s_string.ptr);
    gtk_window_set_default_size(GTK_WINDOW(window), width_expr->value.s_int.value, height_expr->value.s_int.value);
    gtk_widget_show_all(window);

    return new_lightdata(ctx, &expr->span, window);
}

GTKML_PRIVATE GtkMl_S *vm_std_error(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr) {
    GtkMl_S *error_expr = gtk_ml_pop(ctx);
    GtkMl_S *error = gtk_ml_pop(ctx);
    (void) error;

    gboolean has_loc = expr->span.ptr != NULL;
    GtkMl_S *s_line = has_loc? new_int(ctx, NULL, expr->span.line) : NULL;
    GtkMl_S *s_col = has_loc? new_int(ctx, NULL, expr->span.col) : NULL;

    GtkMl_S *s_has_loc = has_loc? new_true(ctx, NULL) : new_false(ctx, NULL);
    GtkMl_S *s_loc = new_array(ctx, NULL);
    GtkMl_S *new_loc = new_array(ctx, NULL);
    gtk_ml_array_push(&new_loc->value.s_array.array, &s_loc->value.s_array.array, s_has_loc);
    s_loc = new_loc;
    if (has_loc) {
        new_loc = new_array(ctx, NULL);

        gtk_ml_array_push(&new_loc->value.s_array.array, &s_loc->value.s_array.array, s_line);
        s_loc = new_loc;

        new_loc = new_array(ctx, NULL);
        gtk_ml_array_push(&new_loc->value.s_array.array, &s_loc->value.s_array.array, s_col);
        s_loc = new_loc;
    }

    GtkMl_S *new = new_map(ctx, NULL, NULL);
    gtk_ml_hash_trie_insert(&new->value.s_map.map, &error_expr->value.s_map.map, new_keyword(ctx, NULL, 0, "loc", 3), s_loc);
    error_expr = new;

    *err = error_expr;

    return NULL;
}

GTKML_PRIVATE GtkMl_S *vm_std_compile_expr(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr) {
    (void) expr;
    GtkMl_S *arg = gtk_ml_pop(ctx);
    GtkMl_BasicBlock **arg_basic_block = gtk_ml_pop(ctx)->value.s_lightdata.userdata;
    GtkMl_Builder *arg_b = gtk_ml_pop(ctx)->value.s_lightdata.userdata;
    GtkMl_Context *arg_ctx = gtk_ml_pop(ctx)->value.s_lightdata.userdata;

    return compile_expression(arg_ctx, arg_b, arg_basic_block, err, &arg, 0, 0, 1, 1)? new_true(ctx, NULL) : NULL;
}

GTKML_PRIVATE GtkMl_S *vm_std_emit_bytecode(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *expr) {
    (void) expr;

    GtkMl_S *imm = NULL;
    if (expr->value.s_int.value == 7) {
        imm = gtk_ml_pop(ctx);
    }

    GtkMl_S *bc = gtk_ml_pop(ctx);
    unsigned int arg_cond = gtk_ml_pop(ctx)->value.s_int.value;
    GtkMl_BasicBlock **arg_basic_block = gtk_ml_pop(ctx)->value.s_lightdata.userdata;
    GtkMl_Builder *arg_b = gtk_ml_pop(ctx)->value.s_lightdata.userdata;
    GtkMl_Context *arg_ctx = gtk_ml_pop(ctx)->value.s_lightdata.userdata;
    const char *bc_ptr = bc->value.s_keyword.ptr;
    size_t bc_len = bc->value.s_keyword.len;

    const char *bc_halt = "halt";
    const char *bc_push_imm = "push-imm";
    const char *bc_push_addr = "push-addr";
    const char *bc_pop = "pop";
    const char *bc_setf_imm = "setf-imm";
    const char *bc_popf = "popf";
    const char *bc_define = "define";
    const char *bc_bind = "bind";
    const char *bc_bind_args = "bind-args";
    const char *bc_get_imm = "get-imm";
    const char *bc_list_imm = "list-imm";
    const char *bc_map_imm = "map-imm";
    const char *bc_set_imm = "set-imm";
    const char *bc_array_imm = "array-imm";
    const char *bc_setmm_imm = "setmm-imm";
    const char *bc_getmm_imm = "getmm-imm";
    const char *bc_var_imm = "var-imm";
    const char *bc_getvar_imm = "getvar-imm";
    const char *bc_assignvar_imm = "assignvar-imm";
    const char *bc_call_std = "call-std";
    const char *bc_call = "call";
    const char *bc_branch_absolute = "branch-absolute";
    const char *bc_ret = "ret";
    const char *bc_add = "add";
    const char *bc_sub = "sub";
    const char *bc_mul = "mul";
    const char *bc_div = "div";
    const char *bc_mod = "mod";
    const char *bc_bitand = "bitand";
    const char *bc_bitor = "bitor";
    const char *bc_bitxor = "bitxor";
    const char *bc_cmp = "cmp";

    gtk_ml_builder_set_cond(arg_b, arg_cond);
    if (strlen(bc_halt) == bc_len && strncmp(bc_ptr, bc_halt, bc_len) == 0) {
        return gtk_ml_build_halt(arg_ctx, arg_b, *arg_basic_block, err)? new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_push_imm) == bc_len && strncmp(bc_ptr, bc_push_imm, bc_len) == 0) {
        if (!imm) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return 0;
        }
        return gtk_ml_build_push_extended_imm(arg_ctx, arg_b, *arg_basic_block, err, gtk_ml_append_static(arg_b, imm))? new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_push_addr) == bc_len && strncmp(bc_ptr, bc_push_addr, bc_len) == 0) {
        if (!imm) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return 0;
        }
        return gtk_ml_build_push_extended_addr(arg_ctx, arg_b, *arg_basic_block, err, gtk_ml_append_static(arg_b, imm))? new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_pop) == bc_len && strncmp(bc_ptr, bc_pop, bc_len) == 0) {
        return gtk_ml_build_pop(arg_ctx, arg_b, *arg_basic_block, err)? new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_setf_imm) == bc_len && strncmp(bc_ptr, bc_setf_imm, bc_len) == 0) {
        if (!imm) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return 0;
        }
        return gtk_ml_build_setf_extended_imm(arg_ctx, arg_b, *arg_basic_block, err, gtk_ml_append_static(arg_b, imm))? new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_popf) == bc_len && strncmp(bc_ptr, bc_popf, bc_len) == 0) {
        return gtk_ml_build_popf(arg_ctx, arg_b, *arg_basic_block, err)? new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_define) == bc_len && strncmp(bc_ptr, bc_define, bc_len) == 0) {
        return gtk_ml_build_define(arg_ctx, arg_b, *arg_basic_block, err)? new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_bind) == bc_len && strncmp(bc_ptr, bc_bind, bc_len) == 0) {
        return gtk_ml_build_bind(arg_ctx, arg_b, *arg_basic_block, err)? new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_bind_args) == bc_len && strncmp(bc_ptr, bc_bind_args, bc_len) == 0) {
        return gtk_ml_build_bind_args(arg_ctx, arg_b, *arg_basic_block, err)? new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_get_imm) == bc_len && strncmp(bc_ptr, bc_get_imm, bc_len) == 0) {
        if (!imm) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return 0;
        }
        return gtk_ml_build_get_extended_imm(arg_ctx, arg_b, *arg_basic_block, err, gtk_ml_append_static(arg_b, imm))? new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_list_imm) == bc_len && strncmp(bc_ptr, bc_list_imm, bc_len) == 0) {
        if (!imm) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return 0;
        }
        return gtk_ml_build_list_extended_imm(arg_ctx, arg_b, *arg_basic_block, err, gtk_ml_append_static(arg_b, imm))? new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_map_imm) == bc_len && strncmp(bc_ptr, bc_map_imm, bc_len) == 0) {
        if (!imm) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return 0;
        }
        return gtk_ml_build_map_extended_imm(arg_ctx, arg_b, *arg_basic_block, err, gtk_ml_append_static(arg_b, imm))? new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_set_imm) == bc_len && strncmp(bc_ptr, bc_set_imm, bc_len) == 0) {
        if (!imm) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return 0;
        }
        return gtk_ml_build_set_extended_imm(arg_ctx, arg_b, *arg_basic_block, err, gtk_ml_append_static(arg_b, imm))? new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_array_imm) == bc_len && strncmp(bc_ptr, bc_array_imm, bc_len) == 0) {
        if (!imm) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return 0;
        }
        return gtk_ml_build_array_extended_imm(arg_ctx, arg_b, *arg_basic_block, err, gtk_ml_append_static(arg_b, imm))? new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_setmm_imm) == bc_len && strncmp(bc_ptr, bc_setmm_imm, bc_len) == 0) {
        return gtk_ml_build_setmm_imm(arg_ctx, arg_b, *arg_basic_block, err)? new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_getmm_imm) == bc_len && strncmp(bc_ptr, bc_getmm_imm, bc_len) == 0) {
        return gtk_ml_build_getmm_imm(arg_ctx, arg_b, *arg_basic_block, err)? new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_var_imm) == bc_len && strncmp(bc_ptr, bc_var_imm, bc_len) == 0) {
        return gtk_ml_build_var_imm(arg_ctx, arg_b, *arg_basic_block, err)? new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_getvar_imm) == bc_len && strncmp(bc_ptr, bc_getvar_imm, bc_len) == 0) {
        return gtk_ml_build_getvar_imm(arg_ctx, arg_b, *arg_basic_block, err)? new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_assignvar_imm) == bc_len && strncmp(bc_ptr, bc_assignvar_imm, bc_len) == 0) {
        return gtk_ml_build_assignvar_imm(arg_ctx, arg_b, *arg_basic_block, err)? new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_call_std) == bc_len && strncmp(bc_ptr, bc_call_std, bc_len) == 0) {
        if (!imm) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return 0;
        }
        return gtk_ml_build_call_extended_std(arg_ctx, arg_b, *arg_basic_block, err, gtk_ml_append_static(arg_b, imm))? new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_call) == bc_len && strncmp(bc_ptr, bc_call, bc_len) == 0) {
        return gtk_ml_build_call(arg_ctx, arg_b, *arg_basic_block, err)? new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_branch_absolute) == bc_len && strncmp(bc_ptr, bc_branch_absolute, bc_len) == 0) {
        if (!imm) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return 0;
        }
        return gtk_ml_build_branch_absolute_extended(arg_ctx, arg_b, *arg_basic_block, err, gtk_ml_append_static(arg_b, imm))? new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_ret) == bc_len && strncmp(bc_ptr, bc_ret, bc_len) == 0) {
        return gtk_ml_build_ret(arg_ctx, arg_b, *arg_basic_block, err)? new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_add) == bc_len && strncmp(bc_ptr, bc_add, bc_len) == 0) {
        return gtk_ml_build_add(arg_ctx, arg_b, *arg_basic_block, err)? new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_sub) == bc_len && strncmp(bc_ptr, bc_sub, bc_len) == 0) {
        return gtk_ml_build_sub(arg_ctx, arg_b, *arg_basic_block, err)? new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_mul) == bc_len && strncmp(bc_ptr, bc_mul, bc_len) == 0) {
        return gtk_ml_build_mul(arg_ctx, arg_b, *arg_basic_block, err)? new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_div) == bc_len && strncmp(bc_ptr, bc_div, bc_len) == 0) {
        return gtk_ml_build_div(arg_ctx, arg_b, *arg_basic_block, err)? new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_mod) == bc_len && strncmp(bc_ptr, bc_mod, bc_len) == 0) {
        return gtk_ml_build_mod(arg_ctx, arg_b, *arg_basic_block, err)? new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_bitand) == bc_len && strncmp(bc_ptr, bc_bitand, bc_len) == 0) {
        return gtk_ml_build_bitand(arg_ctx, arg_b, *arg_basic_block, err)? new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_bitor) == bc_len && strncmp(bc_ptr, bc_bitor, bc_len) == 0) {
        return gtk_ml_build_bitor(arg_ctx, arg_b, *arg_basic_block, err)? new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_bitxor) == bc_len && strncmp(bc_ptr, bc_bitxor, bc_len) == 0) {
        return gtk_ml_build_bitxor(arg_ctx, arg_b, *arg_basic_block, err)? new_true(ctx, NULL) : NULL;
    } else if (strlen(bc_cmp) == bc_len && strncmp(bc_ptr, bc_cmp, bc_len) == 0) {
        if (!imm) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return 0;
        }
        return gtk_ml_build_cmp(arg_ctx, arg_b, *arg_basic_block, err, gtk_ml_append_static(arg_b, imm))? new_true(ctx, NULL) : NULL;
    } else {
        *err = gtk_ml_error(ctx, "bytecode-error", GTKML_ERR_BYTECODE_ERROR, bc->span.ptr != NULL, bc->span.line, bc->span.col, 0);
        return NULL;
    }
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
        return gtk_ml_array_equal(&lhs->value.s_array.array, &rhs->value.s_array.array);
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
    case GTKML_S_STRING:
        jenkins_update(hash, value->value.s_string.ptr, value->value.s_string.len);
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
        gtk_ml_array_foreach(&value->value.s_array.array, array_update, &data);
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
        jenkins_update(hash, value->value.s_program.linkage_name, strlen(value->value.s_program.linkage_name));
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

gboolean gtk_ml_hash(GtkMl_Hasher *hasher, GtkMl_Hash *hash, GtkMl_S *value) {
    hasher->start(hash);
    if (hasher->update(hash, value)) {
        hasher->finish(hash);
        return 1;
    }
    return 0;
}

void gtk_ml_setmetamap(GtkMl_S *value, GtkMl_S *mm) {
    if (value->kind == GTKML_S_MAP) {
        value->value.s_map.metamap = mm;
    }
}

GTKML_PUBLIC GtkMl_S *gtk_ml_getmetamap(GtkMl_S *value) {
    if (value->kind == GTKML_S_MAP) {
        return value->value.s_map.metamap;
    } else {
        return NULL;
    }
}

GtkMl_S *gtk_ml_error(GtkMl_Context *ctx, const char *err, const char *description, gboolean has_loc, int64_t line, int64_t col, size_t n, ...) {
    char *desc = malloc(strlen(description) + 1);
    strcpy(desc, description);

    GtkMl_S *s_err = new_symbol(ctx, NULL, 0, err, strlen(err));
    GtkMl_S *s_desc = new_string(ctx, NULL, desc, strlen(desc));
    GtkMl_S *s_has_loc = has_loc? new_true(ctx, NULL) : new_false(ctx, NULL);
    GtkMl_S *s_line = has_loc? new_int(ctx, NULL, line) : NULL;
    GtkMl_S *s_col = has_loc? new_int(ctx, NULL, col) : NULL;

    GtkMl_S *s_loc = new_array(ctx, NULL);
    GtkMl_S *new_loc = new_array(ctx, NULL);
    gtk_ml_array_push(&new_loc->value.s_array.array, &s_loc->value.s_array.array, s_has_loc);
    s_loc = new_loc;
    if (has_loc) {
        new_loc = new_array(ctx, NULL);

        gtk_ml_array_push(&new_loc->value.s_array.array, &s_loc->value.s_array.array, s_line);
        s_loc = new_loc;

        new_loc = new_array(ctx, NULL);
        gtk_ml_array_push(&new_loc->value.s_array.array, &s_loc->value.s_array.array, s_col);
        s_loc = new_loc;
    }

    GtkMl_S *error = new_map(ctx, NULL, NULL);

    GtkMl_S *new = new_map(ctx, NULL, NULL);
    gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, new_keyword(ctx, NULL, 0, "err", 3), s_err);
    error = new;

    new = new_map(ctx, NULL, NULL);
    gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, new_keyword(ctx, NULL, 0, "desc", 4), s_desc);
    error = new;

    new = new_map(ctx, NULL, NULL);
    gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, new_keyword(ctx, NULL, 0, "loc", 3), s_loc);
    error = new;

    va_list args;
    va_start(args, n);

    for (size_t i = 0; i < n; i++) {
        GtkMl_S *key = va_arg(args, GtkMl_S *);
        GtkMl_S *value = va_arg(args, GtkMl_S *);

        new = new_map(ctx, NULL, NULL);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, key, value);
        error = new;
    }

    va_end(args);

    return error;
}

GtkMl_S *gtk_ml_nil(GtkMl_Context *ctx) {
    return new_nil(ctx, NULL);
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
    delete(ctx, s);
}

void gtk_ml_object_unref(GtkMl_Context *ctx, void *obj) {
    (void) ctx;
    g_object_unref(obj);
}

GtkMl_Vm *gtk_ml_new_vm(GtkMl_Context *ctx) {
    GtkMl_Vm *vm = malloc(sizeof(GtkMl_Vm));

    vm->stack = malloc(sizeof(GtkMl_Register) * GTKML_VM_STACK);
    vm->stack_len = GTKML_VM_STACK;

    vm->call_stack = malloc(sizeof(GtkMl_Register) * GTKML_VM_CALL_STACK);
    vm->call_stack_ptr = 0;
    vm->call_stack_cap = GTKML_VM_CALL_STACK;

    vm->program.start = NULL;
    vm->program.exec = NULL;
    vm->program.n_exec = 0;
    vm->program.statics = NULL;
    vm->program.n_static = 0;

    vm->reg[GTKML_R_ZERO].value = 0;
    vm->reg[GTKML_R_FLAGS].flags = GTKML_F_NONE;
    vm->reg[GTKML_R_SP].sp = 0;
    vm->reg[GTKML_R_BP].sp = 0;

    vm->std = STD;

    vm->ctx = ctx;

    return vm;
}

void gtk_ml_del_vm(GtkMl_Vm *vm) {
    free(vm->stack);
    gtk_ml_del_program(&vm->program);
    free(vm);
}

gboolean gtk_ml_vm_step(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction *instr) {
    if (!instr->gen.cond || (instr->gen.cond && (vm->reg[GTKML_R_FLAGS].flags & instr->gen.cond))) {
        if (instr->gen.cond) {
            vm->reg[GTKML_R_FLAGS].flags &= GTKML_F_TOPCALL;
        }
        gboolean (*category)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction *) = CATEGORY[instr->gen.category];
        if (category) {
            return category(vm, err, instr);
        } else {
            *err = gtk_ml_error(vm->ctx, "category-error", GTKML_ERR_CATEGORY_ERROR, 0, 0, 0, 0);
            return 0;
        }
    } else {
        if (instr->gen.category & GTKML_I_EXTENDED) {
            vm->reg[GTKML_R_PC].pc += 16;
        } else {
            vm->reg[GTKML_R_PC].pc += 8;
        }
        return 1;
    }
}

gboolean gtk_ml_vm_run(GtkMl_Vm *vm, GtkMl_S **err) {
    vm->reg[GTKML_R_FLAGS].flags |= GTKML_F_TOPCALL;
    vm->reg[GTKML_R_FLAGS].flags &= ~GTKML_F_HALT;
    size_t gc_counter = 0;
    while (!(vm->reg[GTKML_R_FLAGS].flags & GTKML_F_HALT)) {
        if (!gtk_ml_vm_step(vm, err, vm->program.exec + (vm->reg[GTKML_R_PC].pc >> 3))) {
            return 0;
        }
        if (gc_counter++ == GTKML_GC_STEP_THRESHOLD) {
            if (gtk_ml_collect(vm->ctx)) {
                gc_counter = 0;
            }
        }
        vm->reg[GTKML_R_ZERO].value = 0;
    }
    return 1;
}

GTKML_PRIVATE gboolean gtk_ml_ia(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction *instr) {
    gboolean (*opcode)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction) = I_ARITH[instr->arith.opcode];
    if (opcode) {
        return opcode(vm, err, *instr);
    } else {
        *err = gtk_ml_error(vm->ctx, "opcode-error", GTKML_ERR_OPCODE_ERROR, 0, 0, 0, 0);
        return 0;
    }
}

GTKML_PRIVATE gboolean gtk_ml_ii(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction *instr) {
    gboolean (*opcode)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction) = I_IMM[instr->imm.opcode];
    if (opcode) {
        return opcode(vm, err, *instr);
    } else {
        *err = gtk_ml_error(vm->ctx, "opcode-error", GTKML_ERR_OPCODE_ERROR, 0, 0, 0, 0);
        return 0;
    }
}

GTKML_PRIVATE gboolean gtk_ml_ibr(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction *instr) {
    gboolean (*opcode)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction) = I_BR[instr->br.opcode];
    if (opcode) {
        return opcode(vm, err, *instr);
    } else {
        *err = gtk_ml_error(vm->ctx, "opcode-error", GTKML_ERR_OPCODE_ERROR, 0, 0, 0, 0);
        return 0;
    }
}

GTKML_PRIVATE gboolean gtk_ml_eii(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction *instr) {
    gboolean (*opcode)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction, GtkMl_S *) = EI_IMM[instr->imm.opcode];
    if (opcode) {
        return opcode(vm, err, instr[0], vm->program.statics[instr[1].imm64]);
    } else {
        *err = gtk_ml_error(vm->ctx, "opcode-error", GTKML_ERR_OPCODE_ERROR, 0, 0, 0, 0);
        return 0;
    }
}

GTKML_PRIVATE gboolean gtk_ml_eibr(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction *instr) {
    gboolean (*opcode)(GtkMl_Vm *, GtkMl_S **, GtkMl_Instruction, GtkMl_S *) = EI_BR[instr->br.opcode];
    if (opcode) {
        return opcode(vm, err, instr[0], vm->program.statics[instr[1].imm64]);
    } else {
        *err = gtk_ml_error(vm->ctx, "opcode-error", GTKML_ERR_OPCODE_ERROR, 0, 0, 0, 0);
        return 0;
    }
}

GTKML_PRIVATE gboolean gtk_ml_enop(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction *instr) {
    (void) err;
    (void) instr;

    vm->reg[GTKML_R_PC].pc += 16;

    return 1;
}

#define PC_INCREMENT vm->reg[GTKML_R_PC].pc += 8
#define PC_INCREMENT_EXTENDED vm->reg[GTKML_R_PC].pc += 16

gboolean gtk_ml_ia_nop(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_halt(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    vm->reg[GTKML_R_FLAGS].flags |= GTKML_F_HALT;
    PC_INCREMENT;
    return 1;
}

GTKML_PRIVATE void set_flags(GtkMl_Vm *vm, GtkMl_S *lhs, GtkMl_S *rhs, GtkMl_S *result) {
    (void) vm;
    (void) lhs;
    (void) rhs;

    // TODO(walterpi): overflow, carry
    if (result->kind == GTKML_S_INT) {
        if (result->value.s_int.value == 0) {
            vm->reg[GTKML_R_FLAGS].flags |= GTKML_F_ZERO;
        } else if (result->value.s_int.value < 0) {
            vm->reg[GTKML_R_FLAGS].flags |= GTKML_F_SIGN;
        }
    } else if (result->kind == GTKML_S_FLOAT) {
        if (result->value.s_float.value == 0.0) {
            vm->reg[GTKML_R_FLAGS].flags |= GTKML_F_ZERO;
        } else if (result->value.s_float.value < 0.0) {
            vm->reg[GTKML_R_FLAGS].flags |= GTKML_F_SIGN;
        }
    }
}

gboolean gtk_ml_ia_add(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, new_int(vm->ctx, NULL, lhs->value.s_int.value + rhs->value.s_int.value));
    } else if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_FLOAT) {
        gtk_ml_push(vm->ctx, new_float(vm->ctx, NULL, (double) lhs->value.s_int.value + rhs->value.s_float.value));
    } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, new_float(vm->ctx, NULL, lhs->value.s_float.value + (double) rhs->value.s_int.value));
    } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_FLOAT) {
        gtk_ml_push(vm->ctx, new_float(vm->ctx, NULL, lhs->value.s_float.value + rhs->value.s_float.value));
    } else {
        *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
        return 0;
    }
    set_flags(vm, lhs, rhs, gtk_ml_peek(vm->ctx));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_subtract(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, new_int(vm->ctx, NULL, lhs->value.s_int.value - rhs->value.s_int.value));
    } else if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_FLOAT) {
        gtk_ml_push(vm->ctx, new_float(vm->ctx, NULL, (double) lhs->value.s_int.value - rhs->value.s_float.value));
    } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, new_float(vm->ctx, NULL, lhs->value.s_float.value - (double) rhs->value.s_int.value));
    } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_FLOAT) {
        gtk_ml_push(vm->ctx, new_float(vm->ctx, NULL, lhs->value.s_float.value - rhs->value.s_float.value));
    } else {
        *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
        return 0;
    }
    set_flags(vm, lhs, rhs, gtk_ml_peek(vm->ctx));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_signed_multiply(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, new_int(vm->ctx, NULL, lhs->value.s_int.value * rhs->value.s_int.value));
    } else if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_FLOAT) {
        gtk_ml_push(vm->ctx, new_float(vm->ctx, NULL, (double) lhs->value.s_int.value * rhs->value.s_float.value));
    } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, new_float(vm->ctx, NULL, lhs->value.s_float.value * (double) rhs->value.s_int.value));
    } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_FLOAT) {
        gtk_ml_push(vm->ctx, new_float(vm->ctx, NULL, lhs->value.s_float.value * rhs->value.s_float.value));
    } else {
        *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
        return 0;
    }
    set_flags(vm, lhs, rhs, gtk_ml_peek(vm->ctx));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_unsigned_multiply(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, new_int(vm->ctx, NULL, (uint64_t) lhs->value.s_int.value * (uint64_t) rhs->value.s_int.value));
    } else if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_FLOAT) {
        gtk_ml_push(vm->ctx, new_float(vm->ctx, NULL, (double) (uint64_t) lhs->value.s_int.value * rhs->value.s_float.value));
    } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, new_float(vm->ctx, NULL, lhs->value.s_float.value * (double) (uint64_t) rhs->value.s_int.value));
    } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_FLOAT) {
        gtk_ml_push(vm->ctx, new_float(vm->ctx, NULL, lhs->value.s_float.value * rhs->value.s_float.value));
    } else {
        *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
        return 0;
    }
    set_flags(vm, lhs, rhs, gtk_ml_peek(vm->ctx));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_signed_divide(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, new_int(vm->ctx, NULL, lhs->value.s_int.value / rhs->value.s_int.value));
    } else if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_FLOAT) {
        gtk_ml_push(vm->ctx, new_float(vm->ctx, NULL, (double) lhs->value.s_int.value / rhs->value.s_float.value));
    } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, new_float(vm->ctx, NULL, lhs->value.s_float.value / (double) rhs->value.s_int.value));
    } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_FLOAT) {
        gtk_ml_push(vm->ctx, new_float(vm->ctx, NULL, lhs->value.s_float.value / rhs->value.s_float.value));
    } else {
        *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
        return 0;
    }
    set_flags(vm, lhs, rhs, gtk_ml_peek(vm->ctx));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_unsigned_divide(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, new_int(vm->ctx, NULL, (uint64_t) lhs->value.s_int.value / (uint64_t) rhs->value.s_int.value));
    } else if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_FLOAT) {
        gtk_ml_push(vm->ctx, new_float(vm->ctx, NULL, (double) (uint64_t) lhs->value.s_int.value / rhs->value.s_float.value));
    } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, new_float(vm->ctx, NULL, lhs->value.s_float.value / (double) (uint64_t) rhs->value.s_int.value));
    } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_FLOAT) {
        gtk_ml_push(vm->ctx, new_float(vm->ctx, NULL, lhs->value.s_float.value / rhs->value.s_float.value));
    } else {
        *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
        return 0;
    }
    set_flags(vm, lhs, rhs, gtk_ml_peek(vm->ctx));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_signed_modulo(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, new_int(vm->ctx, NULL, lhs->value.s_int.value % rhs->value.s_int.value));
    } else if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_FLOAT) {
        gtk_ml_push(vm->ctx, new_float(vm->ctx, NULL, fmod((double) lhs->value.s_int.value, rhs->value.s_float.value)));
    } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, new_float(vm->ctx, NULL, fmod(lhs->value.s_float.value, (double) rhs->value.s_int.value)));
    } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_FLOAT) {
        gtk_ml_push(vm->ctx, new_float(vm->ctx, NULL, fmod(lhs->value.s_float.value, rhs->value.s_float.value)));
    } else {
        *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
        return 0;
    }
    set_flags(vm, lhs, rhs, gtk_ml_peek(vm->ctx));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_unsigned_modulo(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, new_int(vm->ctx, NULL, (uint64_t) lhs->value.s_int.value % (uint64_t) rhs->value.s_int.value));
    } else if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_FLOAT) {
        gtk_ml_push(vm->ctx, new_float(vm->ctx, NULL, fmod((double) (uint64_t) lhs->value.s_int.value, rhs->value.s_float.value)));
    } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, new_float(vm->ctx, NULL, fmod(lhs->value.s_float.value, (double) (uint64_t) rhs->value.s_int.value)));
    } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_FLOAT) {
        gtk_ml_push(vm->ctx, new_float(vm->ctx, NULL, fmod(lhs->value.s_float.value, rhs->value.s_float.value)));
    } else {
        *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
        return 0;
    }
    set_flags(vm, lhs, rhs, gtk_ml_peek(vm->ctx));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_bit_and(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, new_int(vm->ctx, NULL, lhs->value.s_int.value & rhs->value.s_int.value));
    } else {
        *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
        return 0;
    }
    set_flags(vm, lhs, rhs, gtk_ml_peek(vm->ctx));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_bit_or(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, new_int(vm->ctx, NULL, lhs->value.s_int.value | rhs->value.s_int.value));
    } else {
        *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
        return 0;
    }
    set_flags(vm, lhs, rhs, gtk_ml_peek(vm->ctx));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_bit_xor(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, new_int(vm->ctx, NULL, lhs->value.s_int.value ^ rhs->value.s_int.value));
    } else {
        *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
        return 0;
    }
    set_flags(vm, lhs, rhs, gtk_ml_peek(vm->ctx));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_bit_nand(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, new_int(vm->ctx, NULL, ~(lhs->value.s_int.value & rhs->value.s_int.value)));
    } else {
        *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
        return 0;
    }
    set_flags(vm, lhs, rhs, gtk_ml_peek(vm->ctx));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_bit_nor(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, new_int(vm->ctx, NULL, ~(lhs->value.s_int.value | rhs->value.s_int.value)));
    } else {
        *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
        return 0;
    }
    set_flags(vm, lhs, rhs, gtk_ml_peek(vm->ctx));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_bit_xnor(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);
    if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
        gtk_ml_push(vm->ctx, new_int(vm->ctx, NULL, ~(lhs->value.s_int.value ^ rhs->value.s_int.value)));
    } else {
        *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
        return 0;
    }
    set_flags(vm, lhs, rhs, gtk_ml_peek(vm->ctx));
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_bind(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *key = gtk_ml_pop(vm->ctx);
    GtkMl_S *value = gtk_ml_pop(vm->ctx);
    gtk_ml_bind(vm->ctx, key, value);
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_define(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *key = gtk_ml_pop(vm->ctx);
    GtkMl_S *value = gtk_ml_pop(vm->ctx);
    gtk_ml_define(vm->ctx, key, value);
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ia_bind_args(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_S *revkeys = gtk_ml_pop(vm->ctx);
    size_t n = gtk_ml_pop(vm->ctx)->value.s_int.value;

    for (size_t i = 0; i < n; i++) {
        GtkMl_S *value = gtk_ml_pop(vm->ctx);
        gtk_ml_bind(vm->ctx, gtk_ml_car(revkeys), value);
        revkeys = gtk_ml_cdr(revkeys);
    }

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_push_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    gtk_ml_push(vm->ctx, vm->program.statics[instr.imm.imm]);
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_setf_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    switch (vm->program.statics[instr.imm.imm]->kind) {
    case GTKML_S_TRUE:
        vm->reg[GTKML_R_FLAGS].flags |= GTKML_F_ZERO;
        break;
    case GTKML_S_FALSE:
        vm->reg[GTKML_R_FLAGS].flags &= ~GTKML_F_ZERO;
        break;
    default:
        *err = gtk_ml_error(vm->ctx, "boolean-error", GTKML_ERR_BOOLEAN_ERROR, 0, 0, 0, 0);
        return 0;
    }
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_popf(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    switch (gtk_ml_pop(vm->ctx)->kind) {
    case GTKML_S_TRUE:
        vm->reg[GTKML_R_FLAGS].flags |= GTKML_F_ZERO;
        break;
    case GTKML_S_FALSE:
        vm->reg[GTKML_R_FLAGS].flags &= ~GTKML_F_ZERO;
        break;
    default:
        *err = gtk_ml_error(vm->ctx, "boolean-error", GTKML_ERR_BOOLEAN_ERROR, 0, 0, 0, 0);
        return 0;
    }
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_pop(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    gtk_ml_pop(vm->ctx);
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_get_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    GtkMl_S *value = gtk_ml_get(vm->ctx, vm->program.statics[instr.imm.imm]);
    if (value) {
        gtk_ml_push(vm->ctx, value);
    } else {
        GtkMl_S *error = gtk_ml_error(vm->ctx, "binding-error", GTKML_ERR_BINDING_ERROR, 0, 0, 0, 0);
        GtkMl_S *new = new_map(vm->ctx, NULL, NULL);
        gtk_ml_del_hash_trie(vm->ctx, &new->value.s_map.map, gtk_ml_delete_void_reference);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, new_keyword(vm->ctx, NULL, 0, "binding", strlen("binding")), vm->program.statics[instr.imm.imm]);
        error = new;
        *err = error;
        return 0;
    }
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_list_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;

    int64_t n = vm->program.statics[instr.imm.imm]->value.s_int.value;

    GtkMl_S *map = new_nil(vm->ctx, NULL);

    while (n--) {
        GtkMl_S *expr = gtk_ml_pop(vm->ctx);
        map = new_list(vm->ctx, NULL, expr, map);
    }

    gtk_ml_push(vm->ctx, map);

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_map_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;

    int64_t n = vm->program.statics[instr.imm.imm]->value.s_int.value;

    GtkMl_S *map = new_map(vm->ctx, NULL, NULL);

    while (n--) {
        GtkMl_S *value = gtk_ml_pop(vm->ctx);
        GtkMl_S *key = gtk_ml_pop(vm->ctx);
        GtkMl_S *new = new_map(vm->ctx, NULL, NULL);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &map->value.s_map.map, key, value);
        map = new;
    }

    gtk_ml_push(vm->ctx, map);

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_set_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;

    int64_t n = vm->program.statics[instr.imm.imm]->value.s_int.value;

    GtkMl_S *set = new_set(vm->ctx, NULL);

    while (n--) {
        GtkMl_S *key = gtk_ml_pop(vm->ctx);
        GtkMl_S *new = new_set(vm->ctx, NULL);
        gtk_ml_hash_set_insert(&new->value.s_set.set, &set->value.s_set.set, key);
        set = new;
    }

    gtk_ml_push(vm->ctx, set);

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_array_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;

    int64_t n = vm->program.statics[instr.imm.imm]->value.s_int.value;

    GtkMl_S *array = new_array(vm->ctx, NULL);

    while (n--) {
        GtkMl_S *value = gtk_ml_pop(vm->ctx);
        GtkMl_S *new = new_array(vm->ctx, NULL);
        gtk_ml_array_push(&new->value.s_array.array, &array->value.s_array.array, value);
        array = new;
    }

    gtk_ml_push(vm->ctx, array);

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_setmm_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_S *metamap = gtk_ml_pop(vm->ctx);
    GtkMl_S *map = gtk_ml_pop(vm->ctx);

    GtkMl_S *result = new_map(vm->ctx, NULL, metamap);
    gtk_ml_del_hash_trie(vm->ctx, &result->value.s_map.map, gtk_ml_delete_void_reference);
    gtk_ml_hash_trie_copy(&result->value.s_map.map, &map->value.s_map.map);

    gtk_ml_push(vm->ctx, result);

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_getmm_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_S *map = gtk_ml_pop(vm->ctx);

    GtkMl_S *result = gtk_ml_getmetamap(map);
    if (!result) {
        *err = gtk_ml_error(vm->ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 0);
        return 0;
    }
    gtk_ml_push(vm->ctx, result);

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_var_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_S *value = gtk_ml_pop(vm->ctx);
    GtkMl_S *result = new_var(vm->ctx, NULL, value);
    gtk_ml_push(vm->ctx, result);

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_getvar_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_S *var = gtk_ml_pop(vm->ctx);
    gtk_ml_push(vm->ctx, var->value.s_var.expr);

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ii_assignvar_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_S *newvalue = gtk_ml_pop(vm->ctx);
    GtkMl_S *var = gtk_ml_pop(vm->ctx);
    var->value.s_var.expr = newvalue;
    gtk_ml_push(vm->ctx, var);

    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ibr_call(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    GtkMl_S *program = gtk_ml_pop(vm->ctx);

    uint64_t flags = vm->reg[GTKML_R_FLAGS].flags & GTKML_F_TOPCALL;
    vm->call_stack[vm->call_stack_ptr++] = flags;
    vm->reg[GTKML_R_FLAGS].flags &= ~GTKML_F_TOPCALL;

    PC_INCREMENT;
    uint64_t pc = vm->reg[GTKML_R_PC].pc;
    vm->call_stack[vm->call_stack_ptr++] = pc;
    vm->reg[GTKML_R_PC].pc = program->value.s_program.addr;

    gtk_ml_enter(vm->ctx);

    return 1;
}

gboolean gtk_ml_ibr_ret(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;

    gtk_ml_leave(vm->ctx);

    if (vm->reg[GTKML_R_FLAGS].flags & GTKML_F_TOPCALL) {
        vm->reg[GTKML_R_FLAGS].flags |= GTKML_F_HALT;
        PC_INCREMENT;
    } else {
        uint64_t pc = vm->call_stack[--vm->call_stack_ptr];
        uint64_t flags = vm->call_stack[--vm->call_stack_ptr];
        vm->reg[GTKML_R_PC].pc = pc;
        vm->reg[GTKML_R_FLAGS].flags |= flags;
    }

    return 1;
}

gboolean gtk_ml_ibr_call_std(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) instr;
    GtkMl_S *(*function)(GtkMl_Context *, GtkMl_S **, GtkMl_S *) = vm->std[vm->program.statics[instr.br.imm]->value.s_int.value];
    GtkMl_S *expr = gtk_ml_pop(vm->ctx);
    GtkMl_S *value = function(vm->ctx, err, expr);
    if (value) {
        gtk_ml_push(vm->ctx, value);
    } else {
        return 0;
    }
    PC_INCREMENT;
    return 1;
}

gboolean gtk_ml_ibr_branch_absolute(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    vm->reg[GTKML_R_FLAGS].flags &= ~GTKML_F_GENERIC;
    GtkMl_S *addr = vm->program.statics[instr.br.imm];
    vm->reg[GTKML_R_PC].pc = addr->value.s_address.addr;
    return 1;
}

gboolean gtk_ml_ibr_branch_relative(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr) {
    (void) err;
    (void) instr;
    vm->reg[GTKML_R_FLAGS].flags &= ~GTKML_F_GENERIC;
    GtkMl_S *addr = vm->program.statics[instr.br.imm];
    vm->reg[GTKML_R_PC].pc += 8 + addr->value.s_address.addr;
    return 1;
}

gboolean gtk_ml_eii_push_ext_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64) {
    (void) err;
    (void) instr;
    gtk_ml_push(vm->ctx, imm64);
    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eii_setf_ext_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64) {
    (void) instr;
    switch (imm64->kind) {
    case GTKML_S_TRUE:
        vm->reg[GTKML_R_FLAGS].flags |= GTKML_F_ZERO;
        break;
    case GTKML_S_FALSE:
        vm->reg[GTKML_R_FLAGS].flags &= ~GTKML_F_ZERO;
        break;
    default:
        *err = gtk_ml_error(vm->ctx, "boolean-error", GTKML_ERR_BOOLEAN_ERROR, 0, 0, 0, 0);
        return 0;
    }
    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eii_popf_ext(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64) {
    (void) err;
    (void) instr;
    (void) imm64;
    switch (gtk_ml_pop(vm->ctx)->kind) {
    case GTKML_S_TRUE:
        vm->reg[GTKML_R_FLAGS].flags |= GTKML_F_ZERO;
        break;
    case GTKML_S_FALSE:
        vm->reg[GTKML_R_FLAGS].flags &= ~GTKML_F_ZERO;
        break;
    default:
        *err = gtk_ml_error(vm->ctx, "boolean-error", GTKML_ERR_BOOLEAN_ERROR, 0, 0, 0, 0);
        return 0;
    }
    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eii_pop_ext(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64) {
    (void) err;
    (void) instr;
    (void) imm64;
    gtk_ml_pop(vm->ctx);
    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eii_get_ext_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64) {
    (void) err;
    (void) instr;
    GtkMl_S *value = gtk_ml_get(vm->ctx, imm64);
    if (value) {
        gtk_ml_push(vm->ctx, value);
    } else {
        GtkMl_S *error = gtk_ml_error(vm->ctx, "binding-error", GTKML_ERR_BINDING_ERROR, 0, 0, 0, 0);
        GtkMl_S *new = new_map(vm->ctx, NULL, NULL);
        gtk_ml_del_hash_trie(vm->ctx, &new->value.s_map.map, gtk_ml_delete_void_reference);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, new_keyword(vm->ctx, NULL, 0, "binding", strlen("binding")), imm64);
        error = new;
        *err = error;
        return 0;
    }
    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eii_list_ext_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64) {
    (void) err;
    (void) instr;

    int64_t n = imm64->value.s_int.value;

    GtkMl_S *map = new_nil(vm->ctx, NULL);

    while (n--) {
        GtkMl_S *expr = gtk_ml_pop(vm->ctx);
        map = new_list(vm->ctx, NULL, expr, map);
    }

    gtk_ml_push(vm->ctx, map);

    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eii_map_ext_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64) {
    (void) err;
    (void) instr;

    int64_t n = imm64->value.s_int.value;

    GtkMl_S *map = new_map(vm->ctx, NULL, NULL);

    while (n--) {
        GtkMl_S *key = gtk_ml_pop(vm->ctx);
        GtkMl_S *value = gtk_ml_pop(vm->ctx);
        GtkMl_S *new = new_map(vm->ctx, NULL, NULL);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &map->value.s_map.map, key, value);
        map = new;
    }

    gtk_ml_push(vm->ctx, map);

    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eii_set_ext_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64) {
    (void) err;
    (void) instr;

    int64_t n = imm64->value.s_int.value;

    GtkMl_S *set = new_set(vm->ctx, NULL);

    while (n--) {
        GtkMl_S *key = gtk_ml_pop(vm->ctx);
        GtkMl_S *new = new_set(vm->ctx, NULL);
        gtk_ml_hash_set_insert(&new->value.s_set.set, &set->value.s_set.set, key);
        set = new;
    }

    gtk_ml_push(vm->ctx, set);

    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eii_array_ext_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64) {
    (void) err;
    (void) instr;

    int64_t n = imm64->value.s_int.value;

    GtkMl_S *array = new_array(vm->ctx, NULL);

    while (n--) {
        GtkMl_S *value = gtk_ml_pop(vm->ctx);
        GtkMl_S *new = new_array(vm->ctx, NULL);
        gtk_ml_array_push(&new->value.s_array.array, &array->value.s_array.array, value);
        array = new;
    }

    gtk_ml_push(vm->ctx, array);

    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eii_cmp_ext_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64) {
    (void) err;
    (void) instr;

    GtkMl_Cmp cmp = imm64->value.s_int.value;

    GtkMl_S *lhs = gtk_ml_pop(vm->ctx);
    GtkMl_S *rhs = gtk_ml_pop(vm->ctx);

    switch (cmp) {
    case GTKML_CMP_EQUAL:
        gtk_ml_push(vm->ctx, gtk_ml_equal(lhs, rhs)? new_true(vm->ctx, NULL) : new_false(vm->ctx, NULL));
        break;
    case GTKML_CMP_NOT_EQUAL:
        gtk_ml_push(vm->ctx, (!gtk_ml_equal(lhs, rhs))? new_true(vm->ctx, NULL) : new_false(vm->ctx, NULL));
        break;
    case GTKML_CMP_LESS:
        if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
            gtk_ml_push(vm->ctx, (lhs->value.s_int.value < rhs->value.s_int.value)? new_true(vm->ctx, NULL) : new_false(vm->ctx, NULL));
        } else if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_FLOAT) {
            gtk_ml_push(vm->ctx, ((double) lhs->value.s_int.value < rhs->value.s_float.value)? new_true(vm->ctx, NULL) : new_false(vm->ctx, NULL));
        } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_INT) {
            gtk_ml_push(vm->ctx, (lhs->value.s_float.value < (double) rhs->value.s_int.value)? new_true(vm->ctx, NULL) : new_false(vm->ctx, NULL));
        } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_FLOAT) {
            gtk_ml_push(vm->ctx, (lhs->value.s_float.value < rhs->value.s_float.value)? new_true(vm->ctx, NULL) : new_false(vm->ctx, NULL));
        } else {
            *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
            return 0;
        }
        break;
    case GTKML_CMP_GREATER:
        if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
            gtk_ml_push(vm->ctx, (lhs->value.s_int.value > rhs->value.s_int.value)? new_true(vm->ctx, NULL) : new_false(vm->ctx, NULL));
        } else if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_FLOAT) {
            gtk_ml_push(vm->ctx, ((double) lhs->value.s_int.value > rhs->value.s_float.value)? new_true(vm->ctx, NULL) : new_false(vm->ctx, NULL));
        } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_INT) {
            gtk_ml_push(vm->ctx, (lhs->value.s_float.value > (double) rhs->value.s_int.value)? new_true(vm->ctx, NULL) : new_false(vm->ctx, NULL));
        } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_FLOAT) {
            gtk_ml_push(vm->ctx, (lhs->value.s_float.value > rhs->value.s_float.value)? new_true(vm->ctx, NULL) : new_false(vm->ctx, NULL));
        } else {
            *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
            return 0;
        }
        break;
    case GTKML_CMP_LESS_EQUAL:
        if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
            gtk_ml_push(vm->ctx, (lhs->value.s_int.value <= rhs->value.s_int.value)? new_true(vm->ctx, NULL) : new_false(vm->ctx, NULL));
        } else if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_FLOAT) {
            gtk_ml_push(vm->ctx, ((double) lhs->value.s_int.value <= rhs->value.s_float.value)? new_true(vm->ctx, NULL) : new_false(vm->ctx, NULL));
        } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_INT) {
            gtk_ml_push(vm->ctx, (lhs->value.s_float.value <= (double) rhs->value.s_int.value)? new_true(vm->ctx, NULL) : new_false(vm->ctx, NULL));
        } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_FLOAT) {
            gtk_ml_push(vm->ctx, (lhs->value.s_float.value <= rhs->value.s_float.value)? new_true(vm->ctx, NULL) : new_false(vm->ctx, NULL));
        } else {
            *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
            return 0;
        }
        break;
    case GTKML_CMP_GREATER_EQUAL:
        if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_INT) {
            gtk_ml_push(vm->ctx, (lhs->value.s_int.value >= rhs->value.s_int.value)? new_true(vm->ctx, NULL) : new_false(vm->ctx, NULL));
        } else if (lhs->kind == GTKML_S_INT && rhs->kind == GTKML_S_FLOAT) {
            gtk_ml_push(vm->ctx, ((double) lhs->value.s_int.value >= rhs->value.s_float.value)? new_true(vm->ctx, NULL) : new_false(vm->ctx, NULL));
        } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_INT) {
            gtk_ml_push(vm->ctx, (lhs->value.s_float.value >= (double) rhs->value.s_int.value)? new_true(vm->ctx, NULL) : new_false(vm->ctx, NULL));
        } else if (lhs->kind == GTKML_S_FLOAT && rhs->kind == GTKML_S_FLOAT) {
            gtk_ml_push(vm->ctx, (lhs->value.s_float.value >= rhs->value.s_float.value)? new_true(vm->ctx, NULL) : new_false(vm->ctx, NULL));
        } else {
            *err = gtk_ml_error(vm->ctx, "arithmetic-error", GTKML_ERR_ARITH_ERROR, 0, 0, 0, 0);
            return 0;
        }
        break;
    default:
        *err = gtk_ml_error(vm->ctx, "cmp-error", GTKML_ERR_CMP_ERROR, 0, 0, 0, 0);
        return 0;
    }

    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eibr_call_ext_std(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64) {
    (void) instr;
    GtkMl_S *(*function)(GtkMl_Context *, GtkMl_S **, GtkMl_S *) = vm->std[imm64->value.s_int.value];
    GtkMl_S *expr = gtk_ml_pop(vm->ctx);
    GtkMl_S *value = function(vm->ctx, err, expr);
    if (value) {
        gtk_ml_push(vm->ctx, value);
    } else {
        return 0;
    }
    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eibr_branch_absolute_ext(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64) {
    (void) err;
    (void) instr;
    vm->reg[GTKML_R_FLAGS].flags = 0;
    GtkMl_S *addr = imm64;
    vm->reg[GTKML_R_PC].pc = addr->value.s_address.addr;
    PC_INCREMENT_EXTENDED;
    return 1;
}

gboolean gtk_ml_eibr_branch_relative_ext(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64) {
    (void) err;
    (void) instr;
    vm->reg[GTKML_R_FLAGS].flags = 0;
    GtkMl_S *addr = imm64;
    vm->reg[GTKML_R_PC].pc += 8 + addr->value.s_address.addr;
    PC_INCREMENT_EXTENDED;
    return 1;
}

GTKML_PUBLIC void gtk_ml_new_serializer(GtkMl_Serializer *serf) {
    gtk_ml_new_hash_trie(&serf->ptr_map, &PTR_HASHER);
}

GTKML_PUBLIC void gtk_ml_new_deserializer(GtkMl_Deserializer *deserf) {
    gtk_ml_new_hash_trie(&deserf->ptr_map, &PTR_HASHER);
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
    if (data->n < gtk_ml_array_len(array)) {
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
    gtk_ml_new_hash_trie(&ptr_map, &PTR_HASHER);
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
    case GTKML_S_STRING: {
        uint64_t len = value->value.s_string.len;
        fwrite(&len, sizeof(uint64_t), 1, stream);
        fwrite(value->value.s_string.ptr, 1, len + 1, stream);
        break;
    }
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
        uint64_t len = strlen(value->value.s_program.linkage_name);
        fwrite(&len, sizeof(uint64_t), 1, stream);
        fwrite(value->value.s_program.linkage_name, 1, len + 1, stream);
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
        gtk_ml_array_foreach(&value->value.s_array.array, serf_array, &data);
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

    GtkMl_S *result = new_value(ctx, NULL, kind);

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
    case GTKML_S_STRING: {
        uint64_t len;
        fread(&len, sizeof(uint64_t), 1, stream);
        char *ptr = malloc(len + 1);
        fread(ptr, 1, len + 1, stream);
        ptr[len] = 0;
        result->value.s_string.ptr = ptr;
        result->value.s_string.len = len;
        break;
    }
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
            *tail = new_list(ctx, NULL, value, new_nil(ctx, NULL));
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
        gtk_ml_new_hash_trie(&result->value.s_map.map, &DEFAULT_HASHER);
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
            GtkMl_S *new = new_map(ctx, NULL, NULL);
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
        gtk_ml_new_hash_set(&result->value.s_set.set, &DEFAULT_HASHER);
        while (next != ')') {
            GtkMl_S *key = gtk_ml_deserf_value(deserf, ctx, stream, err);
            if (!key) {
                return NULL;
            }
            GtkMl_S *new = new_set(ctx, NULL);
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
        gtk_ml_new_array(&result->value.s_array.array);
        while (next != ')') {
            GtkMl_S *value = gtk_ml_deserf_value(deserf, ctx, stream, err);
            if (!value) {
                return NULL;
            }
            GtkMl_S *new = new_array(ctx, NULL);
            gtk_ml_array_push(&new->value.s_array.array, &result->value.s_array.array, value);
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
        uint64_t len;
        fread(&len, sizeof(uint64_t), 1, stream);
        char *ptr = malloc(len + 1);
        fread(ptr, 1, len + 1, stream);
        result->value.s_program.linkage_name = ptr;
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
    gtk_ml_new_hash_trie(&ptr_map, &PTR_HASHER);
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
