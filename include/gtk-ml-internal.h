#ifndef GTK_ML_INTERNAL_H
#define GTK_ML_INTERNAL_H 1

#ifndef GTKML_INCLUDE_INTERNAL
#error "did you really mean to include \"gtk-ml-internal.h\""
#endif /* GTKML_INCLUDE_INTERNAL */

#include "gtk-ml.h"

typedef union GtkMl_Register GtkMl_Register;

union GtkMl_Register {
    GtkMl_S *s_expr;
    uint64_t sp;
    uint64_t pc;
    uint64_t flags;
    uint64_t value;
};

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

// builds the program's macros
GTKML_PUBLIC gboolean gtk_ml_build_macros(GtkMl_Program *out, GtkMl_S **err, GtkMl_Builder *b);
// builds the program
GTKML_PUBLIC gboolean gtk_ml_build_intr_apply(GtkMl_Program *out, GtkMl_S **err, GtkMl_Builder *b);
// builds the program
GTKML_PUBLIC gboolean gtk_ml_build_intrinsics(GtkMl_Program *out, GtkMl_S **err, GtkMl_Builder *b);
// builds the program
GTKML_PUBLIC gboolean gtk_ml_build_macros(GtkMl_Program *out, GtkMl_S **err, GtkMl_Builder *b);

// lexical analysis
GTKML_PUBLIC gboolean gtk_ml_lex(GtkMl_Context *ctx, GtkMl_Token **tokenv, size_t *tokenc, GtkMl_S **err, const char *src);
// parsing
GTKML_PUBLIC GtkMl_S *gtk_ml_parse(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Token **tokenv, size_t *tokenc);
GTKML_PUBLIC GtkMl_S *gtk_ml_parse(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Token **tokenv, size_t *tokenc);
GTKML_PUBLIC GtkMl_S *gtk_ml_parse_vararg(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Token **tokenv, size_t *tokenc);
GTKML_PUBLIC GtkMl_S *gtk_ml_parse_quote(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Token **tokenv, size_t *tokenc);
GTKML_PUBLIC GtkMl_S *gtk_ml_parse_quasiquote(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Token **tokenv, size_t *tokenc);
GTKML_PUBLIC GtkMl_S *gtk_ml_parse_unquote(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Token **tokenv, size_t *tokenc);
GTKML_PUBLIC GtkMl_S *gtk_ml_parse_alternative(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Token **tokenv, size_t *tokenc);
GTKML_PUBLIC GtkMl_S *gtk_ml_parse_get(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Token **tokenv, size_t *tokenc);

GTKML_PUBLIC gboolean gtk_ml_builder_application(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_new_window(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_setmetamap(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_getmetamap(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_getvar(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_assignvar(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_error(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_define(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_define_macro(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_define_intrinsic(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_intr_apply(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_compile_expr(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_emit_bytecode(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_append_basic_block(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_global_counter(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_do(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_lambda(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_macro(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_cond(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_while(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_len(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_car(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_cdr(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_typeof(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_index(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_push(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_pop(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_concat(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_add(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_sub(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_mul(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_div(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_mod(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_bitnot(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_bitand(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_bitor(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_bitxor(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_cmp(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_var(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_vararg(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_quote(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_quasiquote(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_unquote(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);

GTKML_PUBLIC void gtk_ml_delete(GtkMl_Context *ctx, GtkMl_S *s);
GTKML_PUBLIC void gtk_ml_del(GtkMl_Context *ctx, GtkMl_S *s);
GTKML_PUBLIC void gtk_ml_object_unref(GtkMl_Context *ctx, void *obj);

GTKML_PUBLIC gboolean gtk_ml_vm_run(GtkMl_Vm *vm, GtkMl_S **err);

GTKML_PUBLIC gboolean gtk_ml_ia(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction *instr);
GTKML_PUBLIC gboolean gtk_ml_ii(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction *instr);
GTKML_PUBLIC gboolean gtk_ml_ibr(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction *instr);
GTKML_PUBLIC gboolean gtk_ml_eii(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction *instr);
GTKML_PUBLIC gboolean gtk_ml_eibr(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction *instr);
GTKML_PUBLIC gboolean gtk_ml_enop(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction *instr);

GTKML_PUBLIC gboolean gtk_ml_ia_nop(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ia_halt(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ia_add(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ia_subtract(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ia_signed_multiply(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ia_unsigned_multiply(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ia_signed_divide(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ia_unsigned_divide(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ia_signed_modulo(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ia_unsigned_modulo(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ia_bit_and(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ia_bit_or(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ia_bit_xor(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ia_bit_nand(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ia_bit_nor(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ia_bit_xnor(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ia_car(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ia_cdr(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ia_bind(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ia_define(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ia_bind_args(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ia_list(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ia_enter(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ia_leave(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ia_unwrap(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ia_typeof(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);

GTKML_PUBLIC gboolean gtk_ml_ii_push_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ii_pop(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ii_setf_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ii_popf(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ii_get_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ii_list_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ii_map_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ii_set_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ii_array_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ii_setmm_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ii_getmm_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ii_var_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ii_getvar_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ii_assignvar_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ii_len(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ii_array_index(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ii_array_push(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ii_array_pop(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ii_array_concat(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ii_map_get(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ii_map_insert(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ii_map_delete(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ii_set_contains(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ii_set_insert(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ii_set_delete(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);

GTKML_PUBLIC gboolean gtk_ml_ibr_call(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ibr_ret(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ibr_call_std(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ibr_branch_absolute(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);
GTKML_PUBLIC gboolean gtk_ml_ibr_branch_relative(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr);

GTKML_PUBLIC gboolean gtk_ml_eii_push_ext_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64);
GTKML_PUBLIC gboolean gtk_ml_eii_pop_ext(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64);
GTKML_PUBLIC gboolean gtk_ml_eii_setf_ext_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64);
GTKML_PUBLIC gboolean gtk_ml_eii_popf_ext(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64);
GTKML_PUBLIC gboolean gtk_ml_eii_get_ext_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64);
GTKML_PUBLIC gboolean gtk_ml_eii_list_ext_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64);
GTKML_PUBLIC gboolean gtk_ml_eii_map_ext_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64);
GTKML_PUBLIC gboolean gtk_ml_eii_set_ext_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64);
GTKML_PUBLIC gboolean gtk_ml_eii_array_ext_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64);
GTKML_PUBLIC gboolean gtk_ml_eii_cmp_ext_imm(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64);

GTKML_PUBLIC gboolean gtk_ml_eibr_call_ext_std(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64);
GTKML_PUBLIC gboolean gtk_ml_eibr_branch_absolute_ext(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64);
GTKML_PUBLIC gboolean gtk_ml_eibr_branch_relative_ext(GtkMl_Vm *vm, GtkMl_S **err, GtkMl_Instruction instr, GtkMl_S *imm64);

// creates a new virtual machine on the heap
// must be deleted with `gtk_ml_del_vm`
GTKML_PUBLIC GtkMl_Vm *gtk_ml_new_vm(GtkMl_Context *ctx);
// deletes a virtual machine created with `gtk_ml_new_vm`
GTKML_PUBLIC void gtk_ml_del_vm(GtkMl_Vm *vm);

GTKML_PUBLIC gboolean gtk_ml_compile_cond_expression(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_compile_quasi_expression(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion, gboolean *unwrapped);
GTKML_PUBLIC gboolean gtk_ml_compile_expression(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_compile_body(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_compile_program_internal(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, const char *linkage_name, GtkMl_S *stmt, gboolean ret, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion, GtkMl_ProgramKind kind);

// compile a lambda expression to bytecode
GTKML_PUBLIC gboolean gtk_ml_compile_intrinsics(GtkMl_Builder *b, GtkMl_BasicBlock **start, GtkMl_S **err, GtkMl_S *intrinsic);
// compile a lambda expression to bytecode
GTKML_PUBLIC gboolean gtk_ml_compile_macros(GtkMl_Builder *b, GtkMl_BasicBlock **start, GtkMl_S **err, GtkMl_S *macro);
// compile a lambda expression to bytecode
GTKML_PUBLIC gboolean gtk_ml_compile(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_S **err, GtkMl_S *lambda);

/* miscelaneous */

GTKML_PUBLIC void gtk_ml_delete_value_reference(GtkMl_Context *ctx, GtkMl_S *value);
GTKML_PUBLIC void gtk_ml_delete_void_reference(GtkMl_Context *ctx, void *);
GTKML_PUBLIC void gtk_ml_delete_value(GtkMl_Context *ctx, void *);

#endif /* ifndef GTK_ML_INTERNAL_H */

// vim: set ft=c :
