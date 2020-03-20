#ifndef GTK_ML_INTERNAL_H
#define GTK_ML_INTERNAL_H 1

#ifndef GTKML_INCLUDE_INTERNAL
#error "did you really mean to include \"gtk-ml-internal.h\"?"
#endif /* GTKML_INCLUDE_INTERNAL */

#include "gtk-ml.h"

struct GtkMl_Gc {
    int rc;

    gboolean gc_enabled;
    size_t n_values;
    size_t m_values;
    GtkMl_SObj first;

    GtkMl_SObj *stack;
    size_t stack_len;
    size_t stack_cap;

    GtkMl_SObj *local;
    int64_t local_len;
    int64_t local_cap;
    int64_t local_base;

    int64_t *base_stack;
    size_t base_stack_ptr;
    size_t base_stack_cap;

    GtkMl_Program **programs;
    size_t program_len;
    size_t program_cap;

    GtkMl_SObj static_stack;
    GtkMl_Builder *builder;

    GtkMl_SObj *free_all;
    size_t free_len;
    size_t free_cap;
};

struct GtkMl_Context {
    GtkMl_Index *global_index;
    GtkMl_Builder *default_builder;
    GtkMl_Program *program;
    GtkMl_SObj bindings;

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
    uint32_t pc;
    uint32_t flags;

    GtkMl_TaggedValue *stack;
    size_t stack_len;
    size_t stack_cap;

    GtkMl_TaggedValue *local;
    int64_t local_len;
    int64_t local_cap;
    int64_t local_base;

    int64_t *base_stack;
    size_t base_stack_ptr;
    size_t base_stack_cap;

    uint64_t *call_stack;
    size_t call_stack_ptr;
    size_t call_stack_cap;

    GtkMl_Program *program;
    GtkMl_TaggedValue (**core)(GtkMl_Context *, GtkMl_SObj *, GtkMl_TaggedValue);

    GtkMl_Context *ctx;
};

#ifdef GTKML_ENABLE_ASM
GTKML_PUBLIC void gtk_ml_breakpoint(GtkMl_Context *ctx);
GTKML_PUBLIC void gtk_ml_breakpoint_internal(GtkMl_Context *ctx, gboolean enable);
#endif /* GTKML_ENABLE_ASM */
#ifdef GTKML_ENABLE_POSIX
GTKML_PUBLIC uint64_t gtk_ml_dbg_read_memory(GtkMl_Context *ctx, GtkMl_SObj *err, const void *addr) GTKML_MUST_USE;
GTKML_PUBLIC void gtk_ml_dbg_write_memory(GtkMl_Context *ctx, GtkMl_SObj *err, void *addr, uint64_t value);
GTKML_PUBLIC gboolean gtk_ml_dbg_read_boolean(GtkMl_Context *ctx, GtkMl_SObj *err, const void *addr) GTKML_MUST_USE;
GTKML_PUBLIC void gtk_ml_dbg_write_boolean(GtkMl_Context *ctx, GtkMl_SObj *err, void *addr, gboolean value);
GTKML_PUBLIC uint8_t gtk_ml_dbg_read_u8(GtkMl_Context *ctx, GtkMl_SObj *err, const void *addr) GTKML_MUST_USE;
GTKML_PUBLIC void gtk_ml_dbg_write_u8(GtkMl_Context *ctx, GtkMl_SObj *err, void *addr, uint8_t value);
GTKML_PUBLIC uint32_t gtk_ml_dbg_read_u32(GtkMl_Context *ctx, GtkMl_SObj *err, const void *addr) GTKML_MUST_USE;
GTKML_PUBLIC void gtk_ml_dbg_write_u32(GtkMl_Context *ctx, GtkMl_SObj *err, void *addr, uint32_t value);
GTKML_PUBLIC uint64_t gtk_ml_dbg_read_u64(GtkMl_Context *ctx, GtkMl_SObj *err, const void *addr) GTKML_MUST_USE;
GTKML_PUBLIC void gtk_ml_dbg_write_u64(GtkMl_Context *ctx, GtkMl_SObj *err, void *addr, uint64_t value);
GTKML_PUBLIC void *gtk_ml_dbg_read_ptr(GtkMl_Context *ctx, GtkMl_SObj *err, const void *addr) GTKML_MUST_USE;
GTKML_PUBLIC void gtk_ml_dbg_write_ptr(GtkMl_Context *ctx, GtkMl_SObj *err, void *addr, void *value);
GTKML_PUBLIC void gtk_ml_dbg_read_sobject(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_SObj out, const GtkMl_SObj addr);
GTKML_PUBLIC void gtk_ml_dbg_write_sobject(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_SObj addr, GtkMl_SObj value);
GTKML_PUBLIC GtkMl_TaggedValue gtk_ml_dbg_read_value(GtkMl_Context *ctx, GtkMl_SObj *err, const GtkMl_TaggedValue *addr) GTKML_MUST_USE;
GTKML_PUBLIC void gtk_ml_dbg_write_value(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue *addr, GtkMl_TaggedValue value);
#endif /* GTKML_ENABLE_POSIX */

// dumps a value to a string and reallocates if necessary
GTKML_PUBLIC char *gtk_ml_dumpsnr_internal(GtkMl_Context *ctx, char *ptr, size_t *offset, size_t n, GtkMl_SObj *err, GtkMl_SObj expr) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_dumpf_program_internal(GtkMl_Context *ctx, FILE *stream, GtkMl_SObj *err, gboolean line, gboolean debug);

// dumps a value to a file in debug mode
GTKML_PUBLIC gboolean gtk_ml_dumpf_debug(GtkMl_Context *ctx, FILE *stream, GtkMl_SObj *err, GtkMl_SObj expr);
// dumps the current line to a file in debug mode
GTKML_PUBLIC gboolean gtk_ml_dumpf_line_debug(GtkMl_Context *ctx, FILE *stream, GtkMl_SObj *err);
// dumps the current program to a file in debug mode
GTKML_PUBLIC gboolean gtk_ml_dumpf_program_debug(GtkMl_Context *ctx, FILE *stream, GtkMl_SObj *err);
// dumps the stack to a file in debug mode
GTKML_PUBLIC gboolean gtk_ml_dumpf_stack_debug(GtkMl_Context *ctx, FILE *stream, GtkMl_SObj *err);
// dumps the call backtrace to a file in debug mode
GTKML_PUBLIC gboolean gtk_ml_backtracef_debug(GtkMl_Context *ctx, FILE *stream, GtkMl_SObj *err);

// creates a new context on the heap, with an existing gc
// must be deleted with `gtk_ml_del_context`
GTKML_PUBLIC GtkMl_Context *gtk_ml_new_context_with_gc(GtkMl_Index *index, GtkMl_Gc *gc) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_Context *gtk_ml_new_context_with_gc_builder(GtkMl_Index *index, GtkMl_Gc *gc, GtkMl_Builder *builder) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_Gc *gtk_ml_new_gc() GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_Gc *gtk_ml_gc_copy(GtkMl_Gc *gc) GTKML_MUST_USE;
GTKML_PUBLIC void gtk_ml_del_gc(GtkMl_Context *ctx, GtkMl_Gc *gc);

// pushes an expression to the stack
GTKML_PUBLIC void gtk_ml_gc_push(GtkMl_Gc *gc, GtkMl_SObj value);
// pops an expression from the stack
GTKML_PUBLIC GtkMl_SObj gtk_ml_gc_pop(GtkMl_Gc *gc);
// peeks an expression at the top of the stack
GTKML_PUBLIC GtkMl_SObj gtk_ml_gc_peek(GtkMl_Context *gc);

// early-builds the program's intrinsics
GTKML_PUBLIC GtkMl_Program *gtk_ml_build_intr_apply(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Builder *b, GtkMl_Program **additional_programs, size_t n_programs) GTKML_MUST_USE;
// builds the program's intrinsics
GTKML_PUBLIC GtkMl_Program *gtk_ml_build_intrinsics(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Builder *b, GtkMl_Program **additional_programs, size_t n_programs) GTKML_MUST_USE;
// builds the program's macros
GTKML_PUBLIC GtkMl_Program *gtk_ml_build_macros(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Builder *b, GtkMl_Program **additional_programs, size_t n_programs) GTKML_MUST_USE;

// lexical analysis
GTKML_PUBLIC gboolean gtk_ml_lex(GtkMl_Context *ctx, GtkMl_Token **tokenv, size_t *tokenc, GtkMl_SObj *err, const char *src) GTKML_MUST_USE;
// parsing
GTKML_PUBLIC GtkMl_SObj gtk_ml_parse(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Token **tokenv, size_t *tokenc) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_SObj gtk_ml_parse_vararg(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Token **tokenv, size_t *tokenc) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_SObj gtk_ml_parse_quote(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Token **tokenv, size_t *tokenc) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_SObj gtk_ml_parse_quasiquote(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Token **tokenv, size_t *tokenc) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_SObj gtk_ml_parse_unquote(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Token **tokenv, size_t *tokenc) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_SObj gtk_ml_parse_alternative(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Token **tokenv, size_t *tokenc) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_SObj gtk_ml_parse_get(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Token **tokenv, size_t *tokenc) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_SObj gtk_ml_parse_dot(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Token **tokenv, size_t *tokenc) GTKML_MUST_USE;

GTKML_PUBLIC gboolean gtk_ml_builder_load(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
#ifdef GTKML_ENABLE_GTK
GTKML_PUBLIC gboolean gtk_ml_builder_application(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_new_window(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
#endif /* GTKML_ENABLE_GTK */
#ifdef GTKML_ENABLE_POSIX
GTKML_PUBLIC gboolean gtk_ml_builder_dbg_run(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_dbg_cont(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_dbg_step(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_dbg_disasm(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_dbg_stack(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_dbg_backtrace(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
#endif /* GTKML_ENABLE_POSIX */
GTKML_PUBLIC gboolean gtk_ml_builder_setmetamap(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_getmetamap(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_getvar(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_assignvar(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_error(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_dbg(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_define(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_define_macro(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_define_intrinsic(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_intr_apply(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_compile_expr(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_emit_bytecode(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_bind_symbol(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_export_symbol(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_append_basic_block(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_global_counter(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_basic_block_name(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_string_to_symbol(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_do(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_let(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_let_star(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_lambda(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_macro(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_cond(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_while(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_len(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_car(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_cdr(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_cons(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_map(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_set(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_array(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_typeof(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_to_sobject(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_to_prim(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_index(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_push(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_pop(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_concat(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_map_get(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_map_insert(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_map_rawget(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_map_rawinsert(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_map_delete(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_map_concat(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_set_contains(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_set_insert(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_set_delete(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_set_concat(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_add(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_sub(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_mul(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_div(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_mod(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_bitnot(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_bitand(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_bitor(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_bitxor(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_cmp(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_var(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_vararg(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_quote(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_quasiquote(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_unquote(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_allocate(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_to_cstr(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_to_buffer(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_to_string(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_builder_to_array(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
#ifdef GTKML_ENABLE_WEB
#include "libs/em_gles3/code-gen.h"
gboolean gtk_ml_builder_web_log(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion);
#endif /* GTKML_ENABLE_WEB */

GTKML_PUBLIC void gtk_ml_delete(GtkMl_Context *ctx, GtkMl_SObj s);
GTKML_PUBLIC void gtk_ml_del(GtkMl_Context *ctx, GtkMl_SObj s);
#ifdef GTKML_ENABLE_GTK
GTKML_PUBLIC void gtk_ml_object_unref(GtkMl_Context *ctx, void *obj);
#endif /* GTKML_ENABLE_GTK */

GTKML_PUBLIC gboolean gtk_ml_execute_internal(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue value, GtkMl_TaggedValue *args, size_t n_args, GtkMl_Stage stage);
GTKML_PUBLIC gboolean gtk_ml_vm_run(GtkMl_Vm *vm, GtkMl_SObj *err, gboolean brk) GTKML_MUST_USE;
GTKML_PUBLIC void gtk_ml_vm_push(GtkMl_Vm *vm, GtkMl_TaggedValue value);
GTKML_PUBLIC GtkMl_TaggedValue gtk_ml_vm_pop(GtkMl_Vm *vm);

GTKML_PUBLIC gboolean gtk_ml_i_nop(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_signed_add(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_signed_subtract(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_signed_multiply(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_unsigned_multiply(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_signed_divide(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_unsigned_divide(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_signed_modulo(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_unsigned_modulo(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_bit_and(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_bit_or(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_bit_xor(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_bit_nand(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_bit_nor(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_bit_xnor(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_cmp_imm(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_car(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_cdr(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_cons(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_map(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_set(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_array(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_bind(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_enter_bind_args(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_define(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_list(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_enter(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_leave(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_unwrap(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_typeof(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_to_sobj(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_to_prim(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;

GTKML_PUBLIC gboolean gtk_ml_i_push_imm(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_pop(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_setf_imm(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_popf(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_get_imm(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_local_imm(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_list_imm(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_map_imm(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_set_imm(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_array_imm(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_setmm_imm(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_getmm_imm(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_var(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_getvar(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_assignvar(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_len(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_index(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_array_push(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_array_pop(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_array_concat(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_map_get(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_map_insert(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_map_rawget(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_map_rawinsert(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_map_delete(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_set_contains(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_set_insert(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_set_delete(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;

GTKML_PUBLIC gboolean gtk_ml_i_call(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_leave_ret(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_call_core(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_branch_absolute(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_i_branch_relative(GtkMl_Vm *vm, GtkMl_SObj *err, uint64_t data) GTKML_MUST_USE;

GTKML_PUBLIC void gtk_ml_set_local_internal(GtkMl_Vm *vm, GtkMl_Gc *gc, GtkMl_TaggedValue value);
GTKML_PUBLIC GtkMl_TaggedValue gtk_ml_get_local_internal(GtkMl_Vm *vm, GtkMl_Gc *gc, int64_t offset) GTKML_MUST_USE;

// creates a new virtual machine on the heap
// must be deleted with `gtk_ml_del_vm`
GTKML_PUBLIC GtkMl_Vm *gtk_ml_new_vm(GtkMl_Context *ctx) GTKML_MUST_USE;
// deletes a virtual machine created with `gtk_ml_new_vm`
GTKML_PUBLIC void gtk_ml_del_vm(GtkMl_Vm *vm);

GTKML_PUBLIC gboolean gtk_ml_compile_cond_expression(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_compile_quasi_expression(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion, gboolean *unwrapped) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_compile_expression(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj *stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_compile_body(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, GtkMl_SObj stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_compile_program_internal(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_BasicBlock **basic_block, GtkMl_SObj *err, const char *linkage_name, GtkMl_SObj stmt, gboolean allow_intr, gboolean allow_macro, gboolean allow_runtime, gboolean allow_macro_expansion, GtkMl_ProgramKind kind) GTKML_MUST_USE;

// compile a lambda expression to bytecode
GTKML_PUBLIC gboolean gtk_ml_compile_intrinsics(GtkMl_Index *index, GtkMl_Builder *b, GtkMl_BasicBlock **start, GtkMl_SObj *err, GtkMl_SObj intrinsic) GTKML_MUST_USE;
// compile a lambda expression to bytecode
GTKML_PUBLIC gboolean gtk_ml_compile_macros(GtkMl_Index *index, GtkMl_Builder *b, GtkMl_BasicBlock **start, GtkMl_SObj *err, GtkMl_SObj macro) GTKML_MUST_USE;
// compile a lambda expression to bytecode
GTKML_PUBLIC gboolean gtk_ml_compile(GtkMl_Index *index, GtkMl_Context *ctx, GtkMl_Builder *b, GtkMl_SObj *err, GtkMl_SObj lambda) GTKML_MUST_USE;

#ifdef GTKML_ENABLE_POSIX
/* debug versions of container and other operations */

// calculates a hash of a value if possible, but it's debug mode
GTKML_PUBLIC gboolean gtk_ml_hash_debug(GtkMl_Context *ctx, GtkMl_Hasher *hasher, GtkMl_Hash *hash, GtkMl_TaggedValue value);

typedef GtkMl_VisitResult (*GtkMl_HashTrieDebugFn)(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_HashTrie *ht, GtkMl_TaggedValue key, GtkMl_TaggedValue value, GtkMl_TaggedValue data);
typedef GtkMl_VisitResult (*GtkMl_HashSetDebugFn)(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_HashSet *hs, GtkMl_TaggedValue value, GtkMl_TaggedValue data);
typedef GtkMl_VisitResult (*GtkMl_ArrayDebugFn)(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Array *array, size_t index, GtkMl_TaggedValue value, GtkMl_TaggedValue data);

GTKML_PUBLIC void gtk_ml_hash_trie_copy_debug(GtkMl_Context *ctx, GtkMl_HashTrie *out, GtkMl_HashTrie *hs);
GTKML_PUBLIC size_t gtk_ml_hash_trie_len_debug(GtkMl_Context *ctx, GtkMl_HashTrie *hs) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_hash_trie_concat_debug(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_HashTrie *out, GtkMl_HashTrie *lhs, GtkMl_HashTrie *rhs) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_TaggedValue gtk_ml_hash_trie_insert_debug(GtkMl_Context *ctx, GtkMl_HashTrie *out, GtkMl_HashTrie *hs, GtkMl_TaggedValue key, GtkMl_TaggedValue value);
GTKML_PUBLIC GtkMl_TaggedValue gtk_ml_hash_trie_get_debug(GtkMl_Context *ctx, GtkMl_HashTrie *hs, GtkMl_TaggedValue value) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_hash_trie_contains_debug(GtkMl_Context *ctx, GtkMl_HashTrie *hs, GtkMl_TaggedValue value) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_TaggedValue gtk_ml_hash_trie_delete_debug(GtkMl_Context *ctx, GtkMl_HashTrie *out, GtkMl_HashTrie *hs, GtkMl_TaggedValue value);
GTKML_PUBLIC gboolean gtk_ml_hash_trie_foreach_debug(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_HashTrie *ht, GtkMl_HashTrieDebugFn fn, GtkMl_TaggedValue data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_hash_trie_equal_debug(GtkMl_Context *ctx, GtkMl_HashTrie *lhs, GtkMl_HashTrie *rhs) GTKML_MUST_USE;

GTKML_PUBLIC void gtk_ml_hash_set_copy_debug(GtkMl_Context *ctx, GtkMl_HashSet *out, GtkMl_HashSet *hs);
GTKML_PUBLIC size_t gtk_ml_hash_set_len_debug(GtkMl_Context *ctx, GtkMl_HashSet *hs) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_hash_set_concat_debug(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_HashSet *out, GtkMl_HashSet *lhs, GtkMl_HashSet *rhs) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_TaggedValue gtk_ml_hash_set_insert_debug(GtkMl_Context *ctx, GtkMl_HashSet *out, GtkMl_HashSet *hs, GtkMl_TaggedValue value);
GTKML_PUBLIC GtkMl_TaggedValue gtk_ml_hash_set_get_debug(GtkMl_Context *ctx, GtkMl_HashSet *hs, GtkMl_TaggedValue value) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_hash_set_contains_debug(GtkMl_Context *ctx, GtkMl_HashSet *hs, GtkMl_TaggedValue value) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_TaggedValue gtk_ml_hash_set_delete_debug(GtkMl_Context *ctx, GtkMl_HashSet *out, GtkMl_HashSet *hs, GtkMl_TaggedValue value);
GTKML_PUBLIC gboolean gtk_ml_hash_set_foreach_debug(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_HashSet *ht, GtkMl_HashSetDebugFn fn, GtkMl_TaggedValue data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_hash_set_equal_debug(GtkMl_Context *ctx, GtkMl_HashSet *lhs, GtkMl_HashSet *rhs) GTKML_MUST_USE;

GTKML_PUBLIC void gtk_ml_array_trie_copy_debug(GtkMl_Context *ctx, GtkMl_Array *out, GtkMl_Array *array);
GTKML_PUBLIC gboolean gtk_ml_array_trie_is_string_debug(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Array *array) GTKML_MUST_USE;
GTKML_PUBLIC size_t gtk_ml_array_trie_len_debug(GtkMl_Context *ctx, GtkMl_Array *array) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_array_trie_concat_debug(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Array *out, GtkMl_Array *lhs, GtkMl_Array *rhs) GTKML_MUST_USE;
GTKML_PUBLIC void gtk_ml_array_trie_push_debug(GtkMl_Context *ctx, GtkMl_Array *out, GtkMl_Array *array, GtkMl_TaggedValue value);
GTKML_PUBLIC GtkMl_TaggedValue gtk_ml_array_trie_pop_debug(GtkMl_Context *ctx, GtkMl_Array *out, GtkMl_Array *array);
GTKML_PUBLIC GtkMl_TaggedValue gtk_ml_array_trie_get_debug(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Array *array, size_t index) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_array_trie_contains_debug(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Array *array, size_t *index, GtkMl_TaggedValue value) GTKML_MUST_USE;
GTKML_PUBLIC GtkMl_TaggedValue gtk_ml_array_trie_delete_debug(GtkMl_Context *ctx, GtkMl_Array *out, GtkMl_Array *array, size_t index);
GTKML_PUBLIC gboolean gtk_ml_array_trie_foreach_debug(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Array *ht, GtkMl_ArrayDebugFn fn, GtkMl_TaggedValue data) GTKML_MUST_USE;
GTKML_PUBLIC gboolean gtk_ml_array_trie_equal_debug(GtkMl_Context *ctx, GtkMl_Array *lhs, GtkMl_Array *rhs) GTKML_MUST_USE;
#endif /* GTKML_ENABLE_POSIX */

#endif /* ifndef GTK_ML_INTERNAL_H */

// vim: set ft=c :
