#ifndef GTK_ML_INTERNAL_H
#define GTK_ML_INTERNAL_H 1

#ifndef GTKML_INCLUDE_INTERNAL
#error "did you really mean to include \"gtk-ml-internal.h\"?"
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

struct GtkMl_Gc {
    int rc;

    gboolean gc_enabled;
    size_t n_values;
    size_t m_values;
    GtkMl_S *first;

    GtkMl_S **stack;
    size_t stack_len;
    size_t stack_cap;

    GtkMl_Program **programs;
    size_t program_len;
    size_t program_cap;

    GtkMl_Builder *builder;

    GtkMl_S **free_all;
    size_t free_len;
    size_t free_cap;
};

struct GtkMl_Context {
    GtkMl_S *bindings;
    GtkMl_S **top_scope;

    GtkMl_Gc *gc;
    GtkMl_Vm *vm;
    GtkMl_Parser parser;

    gboolean is_debugger;
    gboolean enable_breakpoint;
    gboolean dbg_done;
#ifdef GTKML_ENABLE_POSIX
    pid_t dbg_process;
    GtkMl_Context *dbg_ctx;
#endif /* GTKML_ENABLE_POSIX */
};

struct GtkMl_Vm {
    GtkMl_Register reg[GTKML_REGISTER_COUNT];
    uint64_t *call_stack;
    size_t call_stack_ptr;
    size_t call_stack_cap;
    GtkMl_Program *program;
    GtkMl_S *(**std)(GtkMl_Context *, GtkMl_S **, GtkMl_S *);

    GtkMl_Context *ctx;
};

#ifdef GTKML_ENABLE_ASM
GTKML_PUBLIC void gtk_ml_breakpoint(GtkMl_Context *ctx);
GTKML_PUBLIC void gtk_ml_breakpoint_internal(GtkMl_Context *ctx, gboolean enable);
#endif /* GTKML_ENABLE_ASM */
#ifdef GTKML_ENABLE_POSIX
GTKML_PUBLIC uint64_t gtk_ml_dbg_read_memory(GtkMl_Context *ctx, GtkMl_S **err, const void *addr);
GTKML_PUBLIC void gtk_ml_dbg_write_memory(GtkMl_Context *ctx, GtkMl_S **err, void *addr, uint64_t value);
GTKML_PUBLIC gboolean gtk_ml_dbg_read_boolean(GtkMl_Context *ctx, GtkMl_S **err, const void *addr);
GTKML_PUBLIC void gtk_ml_dbg_write_boolean(GtkMl_Context *ctx, GtkMl_S **err, void *addr, gboolean value);
GTKML_PUBLIC uint8_t gtk_ml_dbg_read_u8(GtkMl_Context *ctx, GtkMl_S **err, const void *addr);
GTKML_PUBLIC void gtk_ml_dbg_write_u8(GtkMl_Context *ctx, GtkMl_S **err, void *addr, uint8_t value);
GTKML_PUBLIC uint32_t gtk_ml_dbg_read_u32(GtkMl_Context *ctx, GtkMl_S **err, const void *addr);
GTKML_PUBLIC void gtk_ml_dbg_write_u32(GtkMl_Context *ctx, GtkMl_S **err, void *addr, uint32_t value);
GTKML_PUBLIC uint64_t gtk_ml_dbg_read_u64(GtkMl_Context *ctx, GtkMl_S **err, const void *addr);
GTKML_PUBLIC void gtk_ml_dbg_write_u64(GtkMl_Context *ctx, GtkMl_S **err, void *addr, uint64_t value);
GTKML_PUBLIC void *gtk_ml_dbg_read_ptr(GtkMl_Context *ctx, GtkMl_S **err, const void *addr);
GTKML_PUBLIC void gtk_ml_dbg_write_ptr(GtkMl_Context *ctx, GtkMl_S **err, void *addr, void *value);
GTKML_PUBLIC void gtk_ml_dbg_read_value(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *out, const GtkMl_S *addr);
GTKML_PUBLIC void gtk_ml_dbg_write_value(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *addr, GtkMl_S *value);
#endif /* GTKML_ENABLE_POSIX */

// dumps a value to a string and reallocates if necessary
GTKML_PUBLIC char *gtk_ml_dumpsnr_internal(GtkMl_Context *ctx, char *ptr, size_t *offset, size_t n, GtkMl_S **err, GtkMl_S *expr);

// dumps a value to a file in debug mode
GTKML_PUBLIC gboolean gtk_ml_dumpf_debug(GtkMl_Context *ctx, FILE *stream, GtkMl_S **err, GtkMl_S *expr);
// dumps the current line to a file in debug mode
GTKML_PUBLIC gboolean gtk_ml_dumpf_line_debug(GtkMl_Context *ctx, FILE *stream, GtkMl_S **err);
// dumps the current program to a file in debug mode
GTKML_PUBLIC gboolean gtk_ml_dumpf_program_debug(GtkMl_Context *ctx, FILE *stream, GtkMl_S **err);
// dumps the stack to a file in debug mode
GTKML_PUBLIC gboolean gtk_ml_dumpf_stack_debug(GtkMl_Context *ctx, FILE *stream, GtkMl_S **err);
// dumps the call backtrace to a file in debug mode
GTKML_PUBLIC gboolean gtk_ml_backtracef_debug(GtkMl_Context *ctx, FILE *stream, GtkMl_S **err);

// creates a new context on the heap, with an existing gc
// must be deleted with `gtk_ml_del_context`
GTKML_PUBLIC GtkMl_Context *gtk_ml_new_context_with_gc(GtkMl_Gc *gc);
GTKML_PUBLIC GtkMl_Gc *gtk_ml_new_gc();
GTKML_PUBLIC GtkMl_Gc *gtk_ml_gc_copy(GtkMl_Gc *gc);
GTKML_PUBLIC void gtk_ml_del_gc(GtkMl_Context *ctx, GtkMl_Gc *gc);

// early-builds the program's intrinsics
GTKML_PUBLIC GtkMl_Program *gtk_ml_build_intr_apply(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Builder *b);
// builds the program's intrinsics
GTKML_PUBLIC GtkMl_Program *gtk_ml_build_intrinsics(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Builder *b);
// builds the program's macros
GTKML_PUBLIC GtkMl_Program *gtk_ml_build_macros(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Builder *b);

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

#ifdef GTKML_ENABLE_GTK
GTKML_PUBLIC gboolean gtk_ml_builder_application(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_new_window(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
#endif /* GTKML_ENABLE_GTK */
#ifdef GTKML_ENABLE_POSIX
GTKML_PUBLIC gboolean gtk_ml_builder_dbg_run(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_dbg_cont(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_dbg_step(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_dbg_disasm(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_dbg_stack(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_dbg_backtrace(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
#endif /* GTKML_ENABLE_POSIX */
GTKML_PUBLIC gboolean gtk_ml_builder_setmetamap(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_getmetamap(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_getvar(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_assignvar(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_error(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_dbg(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_define(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_define_macro(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_define_intrinsic(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_intr_apply(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_compile_expr(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_emit_bytecode(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_export_symbol(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_append_basic_block(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_global_counter(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_basic_block_name(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_string_to_symbol(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_do(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_let(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
GTKML_PUBLIC gboolean gtk_ml_builder_let_star(GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_S **err, GtkMl_S **stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
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

// runs a program previously loaded with `gtk_ml_load_program`
GTKML_PUBLIC gboolean gtk_ml_run_program_internal(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_S *program, GtkMl_S *args, gboolean brk);

GTKML_PUBLIC gboolean gtk_ml_vm_run(GtkMl_Vm *vm, GtkMl_S **err, gboolean brk);

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

#ifdef GTKML_ENABLE_POSIX
/* debug versions of container and other operations */

// calculates a hash of a value if possible, but it's debug mode
GTKML_PUBLIC gboolean gtk_ml_hash_debug(GtkMl_Context *ctx, GtkMl_Hasher *hasher, GtkMl_Hash *hash, GtkMl_S *value);

typedef GtkMl_VisitResult (*GtkMl_HashTrieDebugFn)(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_HashTrie *ht, void *key, void *value, void *data);
typedef GtkMl_VisitResult (*GtkMl_HashSetDebugFn)(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_HashSet *hs, void *value, void *data);
typedef GtkMl_VisitResult (*GtkMl_ArrayDebugFn)(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Array *array, size_t index, GtkMl_S *value, void *data);

GTKML_PUBLIC void gtk_ml_hash_trie_copy_debug(GtkMl_Context *ctx, GtkMl_HashTrie *out, GtkMl_HashTrie *ht);
GTKML_PUBLIC size_t gtk_ml_hash_trie_len_debug(GtkMl_Context *ctx, GtkMl_HashTrie *ht);
GTKML_PUBLIC gboolean gtk_ml_hash_trie_concat_debug(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_HashTrie *out, GtkMl_HashTrie *lhs, GtkMl_HashTrie *rhs);
GTKML_PUBLIC void *gtk_ml_hash_trie_insert_debug(GtkMl_Context *ctx, GtkMl_HashTrie *out, GtkMl_HashTrie *ht, void *key, void *value);
GTKML_PUBLIC void *gtk_ml_hash_trie_get_debug(GtkMl_Context *ctx, GtkMl_HashTrie *ht, void *key);
GTKML_PUBLIC gboolean gtk_ml_hash_trie_contains_debug(GtkMl_Context *ctx, GtkMl_HashTrie *ht, void *key);
GTKML_PUBLIC void *gtk_ml_hash_trie_delete_debug(GtkMl_Context *ctx, GtkMl_HashTrie *out, GtkMl_HashTrie *ht, void *key);
GTKML_PUBLIC gboolean gtk_ml_hash_trie_foreach_debug(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_HashTrie *ht, GtkMl_HashTrieDebugFn fn, void *data);
GTKML_PUBLIC gboolean gtk_ml_hash_trie_equal_debug(GtkMl_Context *ctx, GtkMl_HashTrie *lhs, GtkMl_HashTrie *rhs);

GTKML_PUBLIC void gtk_ml_hash_set_copy_debug(GtkMl_Context *ctx, GtkMl_HashSet *out, GtkMl_HashSet *hs);
GTKML_PUBLIC size_t gtk_ml_hash_set_len_debug(GtkMl_Context *ctx, GtkMl_HashSet *hs);
GTKML_PUBLIC gboolean gtk_ml_hash_set_concat_debug(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_HashSet *out, GtkMl_HashSet *lhs, GtkMl_HashSet *rhs);
GTKML_PUBLIC void *gtk_ml_hash_set_insert_debug(GtkMl_Context *ctx, GtkMl_HashSet *out, GtkMl_HashSet *hs, void *value);
GTKML_PUBLIC void *gtk_ml_hash_set_get_debug(GtkMl_Context *ctx, GtkMl_HashSet *hs, void *value);
GTKML_PUBLIC gboolean gtk_ml_hash_set_contains_debug(GtkMl_Context *ctx, GtkMl_HashSet *hs, void *value);
GTKML_PUBLIC void *gtk_ml_hash_set_delete_debug(GtkMl_Context *ctx, GtkMl_HashSet *out, GtkMl_HashSet *hs, void *value);
GTKML_PUBLIC gboolean gtk_ml_hash_set_foreach_debug(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_HashSet *ht, GtkMl_HashSetDebugFn fn, void *data);
GTKML_PUBLIC gboolean gtk_ml_hash_set_equal_debug(GtkMl_Context *ctx, GtkMl_HashSet *lhs, GtkMl_HashSet *rhs);

GTKML_PUBLIC void gtk_ml_array_trie_copy_debug(GtkMl_Context *ctx, GtkMl_Array *out, GtkMl_Array *array);
GTKML_PUBLIC gboolean gtk_ml_array_trie_is_string_debug(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Array *array);
GTKML_PUBLIC size_t gtk_ml_array_trie_len_debug(GtkMl_Context *ctx, GtkMl_Array *array);
GTKML_PUBLIC gboolean gtk_ml_array_trie_concat_debug(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Array *out, GtkMl_Array *lhs, GtkMl_Array *rhs);
GTKML_PUBLIC void gtk_ml_array_trie_push_debug(GtkMl_Context *ctx, GtkMl_Array *out, GtkMl_Array *array, GtkMl_S *value);
GTKML_PUBLIC GtkMl_S *gtk_ml_array_trie_pop_debug(GtkMl_Context *ctx, GtkMl_Array *out, GtkMl_Array *array);
GTKML_PUBLIC GtkMl_S *gtk_ml_array_trie_get_debug(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Array *array, size_t index);
GTKML_PUBLIC gboolean gtk_ml_array_trie_contains_debug(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Array *array, size_t *index, GtkMl_S *value);
GTKML_PUBLIC GtkMl_S *gtk_ml_array_trie_delete_debug(GtkMl_Context *ctx, GtkMl_Array *out, GtkMl_Array *array, size_t index);
GTKML_PUBLIC gboolean gtk_ml_array_trie_foreach_debug(GtkMl_Context *ctx, GtkMl_S **err, GtkMl_Array *ht, GtkMl_ArrayDebugFn fn, void *data);
GTKML_PUBLIC gboolean gtk_ml_array_trie_equal_debug(GtkMl_Context *ctx, GtkMl_Array *lhs, GtkMl_Array *rhs);
#endif /* GTKML_ENABLE_POSIX */

#endif /* ifndef GTK_ML_INTERNAL_H */

// vim: set ft=c :
