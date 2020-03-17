#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>
#ifdef GTKML_ENABLE_POSIX
#include <sys/ptrace.h>
#endif /* GTKML_ENABLE_POSIX */
#ifdef GTKML_ENABLE_GTK
#include <gtk/gtk.h>
#endif /* GTKML_ENABLE_GTK */
#include <math.h>
#define GTKML_INCLUDE_INTERNAL
#include "gtk-ml.h"
#include "gtk-ml-internal.h"

GTKML_PRIVATE const char *S_OPCODES[] = {
    [GTKML_I_NOP] = GTKML_SI_NOP,
    [GTKML_I_ADD] = GTKML_SI_ADD,
    [GTKML_I_SUBTRACT] = GTKML_SI_SUBTRACT,
    [GTKML_I_SIGNED_MULTIPLY] = GTKML_SI_SIGNED_MULTIPLY,
    [GTKML_I_UNSIGNED_MULTIPLY] = GTKML_SI_UNSIGNED_MULTIPLY,
    [GTKML_I_SIGNED_DIVIDE] = GTKML_SI_SIGNED_DIVIDE,
    [GTKML_I_UNSIGNED_DIVIDE] = GTKML_SI_UNSIGNED_DIVIDE,
    [GTKML_I_SIGNED_MODULO] = GTKML_SI_SIGNED_MODULO,
    [GTKML_I_UNSIGNED_MODULO] = GTKML_SI_UNSIGNED_MODULO,
    [GTKML_I_BIT_AND] = GTKML_SI_BIT_AND,
    [GTKML_I_BIT_OR] = GTKML_SI_BIT_OR,
    [GTKML_I_BIT_XOR] = GTKML_SI_BIT_XOR,
    [GTKML_I_BIT_NAND] = GTKML_SI_BIT_NAND,
    [GTKML_I_BIT_NOR] = GTKML_SI_BIT_NOR,
    [GTKML_I_BIT_XNOR] = GTKML_SI_BIT_XNOR,
    [GTKML_I_CMP_IMM] = GTKML_SI_CMP_IMM,
    [GTKML_I_CAR] = GTKML_SI_CAR,
    [GTKML_I_CDR] = GTKML_SI_CDR,
    [GTKML_I_BIND] = GTKML_SI_BIND,
    [GTKML_I_ENTER_BIND_ARGS] = GTKML_SI_ENTER_BIND_ARGS,
    [GTKML_I_DEFINE] = GTKML_SI_DEFINE,
    [GTKML_I_LIST] = GTKML_SI_LIST,
    [GTKML_I_ENTER] = GTKML_SI_ENTER,
    [GTKML_I_LEAVE] = GTKML_SI_LEAVE,
    [GTKML_I_UNWRAP] = GTKML_SI_UNWRAP,
    [GTKML_I_TYPEOF] = GTKML_SI_TYPEOF,
    [GTKML_I_TO_SOBJ] = GTKML_SI_TO_SOBJ,
    [GTKML_I_TO_PRIM] = GTKML_SI_TO_PRIM,
    [GTKML_I_PUSH_IMM] = GTKML_SI_PUSH_IMM,
    [GTKML_I_POP] = GTKML_SI_POP,
    [GTKML_I_SETF_IMM] = GTKML_SI_SETF_IMM,
    [GTKML_I_POPF] = GTKML_SI_POPF,
    [GTKML_I_GET_IMM] = GTKML_SI_GET_IMM,
    [GTKML_I_LOCAL_IMM] = GTKML_SI_LOCAL_IMM,
    [GTKML_I_LIST_IMM] = GTKML_SI_LIST_IMM,
    [GTKML_I_MAP_IMM] = GTKML_SI_MAP_IMM,
    [GTKML_I_SET_IMM] = GTKML_SI_SET_IMM,
    [GTKML_I_ARRAY_IMM] = GTKML_SI_ARRAY_IMM,
    [GTKML_I_SETMM_IMM] = GTKML_SI_SETMM_IMM,
    [GTKML_I_GETMM_IMM] = GTKML_SI_GETMM_IMM,
    [GTKML_I_VAR] = GTKML_SI_VAR,
    [GTKML_I_GETVAR] = GTKML_SI_GETVAR,
    [GTKML_I_ASSIGNVAR] = GTKML_SI_ASSIGNVAR,
    [GTKML_I_LEN] = GTKML_SI_LEN,
    [GTKML_I_ARRAY_INDEX] = GTKML_SI_ARRAY_INDEX,
    [GTKML_I_ARRAY_PUSH] = GTKML_SI_ARRAY_PUSH,
    [GTKML_I_ARRAY_CONCAT] = GTKML_SI_ARRAY_CONCAT,
    [GTKML_I_ARRAY_POP] = GTKML_SI_ARRAY_POP,
    [GTKML_I_MAP_GET] = GTKML_SI_MAP_GET,
    [GTKML_I_MAP_INSERT] = GTKML_SI_MAP_INSERT,
    [GTKML_I_MAP_DELETE] = GTKML_SI_MAP_DELETE,
    [GTKML_I_MAP_CONCAT] = GTKML_SI_MAP_CONCAT,
    [GTKML_I_SET_CONTAINS] = GTKML_SI_SET_CONTAINS,
    [GTKML_I_SET_INSERT] = GTKML_SI_SET_INSERT,
    [GTKML_I_SET_DELETE] = GTKML_SI_SET_DELETE,
    [GTKML_I_SET_CONCAT] = GTKML_SI_SET_CONCAT,
    [GTKML_I_CALL] = GTKML_SI_CALL,
    [GTKML_I_LEAVE_RET] = GTKML_SI_LEAVE_RET,
    [GTKML_I_CALL_CORE] = GTKML_SI_CALL_CORE,
    [GTKML_I_BRANCH_ABSOLUTE] = GTKML_SI_BRANCH_ABSOLUTE,
    [GTKML_I_BRANCH_RELATIVE] = GTKML_SI_BRANCH_RELATIVE,
    [255] = NULL,
};

GTKML_PRIVATE void default_hash_start(GtkMl_Hash *hash);
GTKML_PRIVATE gboolean default_hash_update(GtkMl_Hash *hash, GtkMl_TaggedValue ptr);
GTKML_PRIVATE void default_hash_finish(GtkMl_Hash *hash);
GTKML_PRIVATE gboolean default_equal(GtkMl_TaggedValue lhs, GtkMl_TaggedValue rhs);

GTKML_PRIVATE void value_hash_start(GtkMl_Hash *hash);
GTKML_PRIVATE gboolean value_hash_update(GtkMl_Hash *hash, GtkMl_TaggedValue ptr);
GTKML_PRIVATE void value_hash_finish(GtkMl_Hash *hash);
GTKML_PRIVATE gboolean value_equal(GtkMl_TaggedValue lhs, GtkMl_TaggedValue rhs);

GTKML_PRIVATE void ptr_hash_start(GtkMl_Hash *hash);
GTKML_PRIVATE gboolean ptr_hash_update(GtkMl_Hash *hash, GtkMl_TaggedValue ptr);
GTKML_PRIVATE void ptr_hash_finish(GtkMl_Hash *hash);
GTKML_PRIVATE gboolean ptr_equal(GtkMl_TaggedValue lhs, GtkMl_TaggedValue rhs);

GTKML_PRIVATE gboolean gtk_ml_dumpf_value_internal(GtkMl_Context *ctx, FILE *stream, GtkMl_SObj *err, GtkMl_TaggedValue value, gboolean dump_ptr, gboolean dump_sobj);

GtkMl_Hasher GTKML_DEFAULT_HASHER = {
    default_hash_start,
    default_hash_update,
    default_hash_finish,
    default_equal
};

GtkMl_Hasher GTKML_VALUE_HASHER = {
    value_hash_start,
    value_hash_update,
    value_hash_finish,
    value_equal
};

GtkMl_Hasher GTKML_PTR_HASHER = {
    ptr_hash_start,
    ptr_hash_update,
    ptr_hash_finish,
    ptr_equal
};

#define gtk_ml_add_reader(ctx, _token, reader) \
    do { \
        ctx->parser.readers[ctx->parser.len_reader].token = _token; \
        ctx->parser.readers[ctx->parser.len_reader].fn = reader; \
        ++ctx->parser.len_reader; \
    } while (0);

GtkMl_Context *gtk_ml_new_context() {
    GtkMl_Context *ctx = gtk_ml_new_context_with_gc(gtk_ml_new_gc());
    return ctx;
}

GtkMl_Context *gtk_ml_new_context_with_gc(GtkMl_Gc *gc) {
    GtkMl_Context *ctx = gtk_ml_new_context_with_gc_builder(gc, NULL);
    ctx->default_builder = gtk_ml_new_builder(ctx);
    return ctx;
}

GtkMl_Context *gtk_ml_new_context_with_gc_builder(GtkMl_Gc *gc, GtkMl_Builder *builder) {
    GtkMl_Context *ctx = malloc(sizeof(GtkMl_Context));
    ctx->is_debugger = 0;
    ctx->enable_breakpoint = 0;
    ctx->dbg_done = 0;
    ctx->vm = gtk_ml_new_vm(ctx);
    ctx->gc = gc;
    ctx->default_builder = builder;
    ctx->program = NULL;

    // ({'flags-none G_APPLICATION_FLAGS_NONE})
    GtkMl_SObj bindings = gtk_ml_new_var(ctx, NULL, gtk_ml_new_map(ctx, NULL, NULL));
    ctx->bindings = bindings;

    if (!ctx->gc->static_stack) {
        ctx->gc->static_stack = gtk_ml_new_nil(ctx, NULL);
    }
    ctx->gc->static_stack = gtk_ml_new_list(ctx, NULL, bindings, ctx->gc->static_stack);


    gtk_ml_bind(ctx, gtk_ml_new_symbol(ctx, NULL, 0, "nullptr", 7), gtk_ml_value_userdata(NULL));
#ifdef GTKML_ENABLE_GTK
    gtk_ml_bind(ctx, gtk_ml_new_symbol(ctx, NULL, 0, "flags-none", 10), gtk_ml_value_sobject(gtk_ml_new_int(ctx, NULL, G_APPLICATION_FLAGS_NONE)));
#endif /* GTKML_ENABLE_GTK */
    gtk_ml_bind(ctx, gtk_ml_new_symbol(ctx, NULL, 0, "cond-none", 9), gtk_ml_value_sobject(gtk_ml_new_int(ctx, NULL, GTKML_F_NONE)));
    gtk_ml_bind(ctx, gtk_ml_new_symbol(ctx, NULL, 0, "cond-eq", 7), gtk_ml_value_sobject(gtk_ml_new_int(ctx, NULL, GTKML_F_EQUAL)));
    gtk_ml_bind(ctx, gtk_ml_new_symbol(ctx, NULL, 0, "cond-ne", 7), gtk_ml_value_sobject(gtk_ml_new_int(ctx, NULL, GTKML_F_NEQUAL)));
#ifdef GTKML_ENABLE_WEB
#include "libs/em_gles3/const-bindings.h"
    gtk_ml_bind(ctx, gtk_ml_new_symbol(ctx, NULL, 0, "webgl/VERSION", strlen("webgl/VERSION")), gtk_ml_value_int(GL_VERSION));
    gtk_ml_bind(ctx, gtk_ml_new_symbol(ctx, NULL, 0, "webgl/SHADING-LANGUAGE-VERSION", strlen("webgl/SHADING-LANGUAGE-VERSION")), gtk_ml_value_int(GL_SHADING_LANGUAGE_VERSION));
    gtk_ml_bind(ctx, gtk_ml_new_symbol(ctx, NULL, 0, "webgl/COMPILE-STATUS", strlen("webgl/COMPILE-STATUS")), gtk_ml_value_int(GL_COMPILE_STATUS));
    gtk_ml_bind(ctx, gtk_ml_new_symbol(ctx, NULL, 0, "webgl/LINK-STATUS", strlen("webgl/LINK-STATUS")), gtk_ml_value_int(GL_LINK_STATUS));
    gtk_ml_bind(ctx, gtk_ml_new_symbol(ctx, NULL, 0, "webgl/VERTEX-SHADER", strlen("webgl/VERTEX-SHADER")), gtk_ml_value_int(GL_VERTEX_SHADER));
    gtk_ml_bind(ctx, gtk_ml_new_symbol(ctx, NULL, 0, "webgl/FRAGMENT-SHADER", strlen("webgl/FRAGMENT-SHADER")), gtk_ml_value_int(GL_FRAGMENT_SHADER));
    gtk_ml_bind(ctx, gtk_ml_new_symbol(ctx, NULL, 0, "webgl/ARRAY-BUFFER", strlen("webgl/ARRAY-BUFFER")), gtk_ml_value_int(GL_ARRAY_BUFFER));
    gtk_ml_bind(ctx, gtk_ml_new_symbol(ctx, NULL, 0, "webgl/STATIC-DRAW", strlen("webgl/STATIC-DRAW")), gtk_ml_value_int(GL_STATIC_DRAW));
    gtk_ml_bind(ctx, gtk_ml_new_symbol(ctx, NULL, 0, "webgl/FLOAT", strlen("webgl/FLOAT")), gtk_ml_value_int(GL_FLOAT));
    gtk_ml_bind(ctx, gtk_ml_new_symbol(ctx, NULL, 0, "webgl/COLOR-BUFFER-BIT", strlen("webgl/COLOR-BUFFER-BIT")), gtk_ml_value_int(GL_COLOR_BUFFER_BIT));
    gtk_ml_bind(ctx, gtk_ml_new_symbol(ctx, NULL, 0, "webgl/TRIANGLES", strlen("webgl/TRIANGLES")), gtk_ml_value_int(GL_TRIANGLES));
#endif /* GTKML_ENABLE_WEB */

    ctx->parser.readers = malloc(sizeof(GtkMl_Reader) * 64);
    ctx->parser.len_reader = 0;
    ctx->parser.cap_reader = 64;

    gtk_ml_add_reader(ctx, GTKML_TOK_ELLIPSIS, gtk_ml_parse_vararg);
    gtk_ml_add_reader(ctx, GTKML_TOK_TICK, gtk_ml_parse_quote);
    gtk_ml_add_reader(ctx, GTKML_TOK_BACKTICK, gtk_ml_parse_quasiquote);
    gtk_ml_add_reader(ctx, GTKML_TOK_COMMA, gtk_ml_parse_unquote);
    gtk_ml_add_reader(ctx, GTKML_TOK_POUND, gtk_ml_parse_alternative);
    gtk_ml_add_reader(ctx, GTKML_TOK_AT, gtk_ml_parse_get);
    gtk_ml_add_reader(ctx, GTKML_TOK_DOT, gtk_ml_parse_dot);

    return ctx;
}

void gtk_ml_del_context(GtkMl_Context *ctx) {
    gtk_ml_del_gc(ctx, ctx->gc);
    gtk_ml_del_vm(ctx->vm);

    free(ctx->parser.readers);

    free(ctx);
}

#ifdef GTKML_ENABLE_POSIX
GtkMl_Context *gtk_ml_new_debugger(pid_t dbg_process) {
    GtkMl_Context *ctx = gtk_ml_new_context();
    ctx->is_debugger = 1;
    ctx->dbg_process = dbg_process;
    return ctx;
}

void gtk_ml_set_debug(GtkMl_Context *ctx, pid_t dbg_process, GtkMl_Context *debugee) {
    ctx->dbg_process = dbg_process;
    ctx->dbg_ctx = debugee;
}
#endif /* GTKML_ENABLE_POSIX */

GtkMl_Gc *gtk_ml_new_gc() {
    GtkMl_Gc *gc = malloc(sizeof(GtkMl_Gc));
    gc->rc = 1;
    gc->gc_enabled = 1;
    gc->n_values = 0;
    gc->m_values = GTKML_GC_COUNT_THRESHOLD;
    gc->first = NULL;

    gc->stack_len = 0;
    gc->stack_cap = GTKML_GC_STACK;
    gc->stack = malloc(sizeof(GtkMl_SObj) * gc->stack_cap);

    gc->local_base = 0;
    gc->local_len = 0;
    gc->local_cap = GTKML_GC_STACK;
    gc->local = malloc(sizeof(GtkMl_SObj) * gc->local_cap);

    gc->base_stack_ptr = 0;
    gc->base_stack_cap = GTKML_GC_STACK;
    gc->base_stack = malloc(sizeof(uint64_t) * gc->base_stack_cap);

    gc->program_len = 0;
    gc->program_cap = 16;
    gc->programs = malloc(sizeof(GtkMl_Program *) * gc->program_cap);

    gc->free_len = 0;
    gc->free_cap = 64;
    gc->free_all = malloc(sizeof(GtkMl_SObj) * gc->free_cap);

    gc->static_stack = NULL;
    gc->builder = NULL;

    return gc;
}

GtkMl_Gc *gtk_ml_gc_copy(GtkMl_Gc *gc) {
    ++gc->rc;
    return gc;
}

GTKML_PRIVATE void free_all(GtkMl_Gc *gc) {
    for (size_t i = 0; i < gc->free_len; i++) {
        free(gc->free_all[i]);
    }
    gc->free_len = 0;
}

void gtk_ml_del_gc(GtkMl_Context *ctx, GtkMl_Gc *gc) {
    --gc->rc;
    if (!gc->rc) {
        GtkMl_SObj value = gc->first;
        while (value) {
            GtkMl_SObj next = value->next;
            gtk_ml_del(ctx, value);
            value = next;
        }
        free_all(gc);
        free(gc->free_all);

        for (size_t i = 0; i < gc->program_len; i++) {
            gtk_ml_del_program(gc->programs[i]);
        }
        free(gc->programs);
        free(gc->stack);
        free(gc->local);
        free(gc);
    }
}

void gtk_ml_gc_push(GtkMl_Gc *gc, GtkMl_SObj sobj) {
    if (gc->stack_len == gc->stack_cap) {
        gc->stack_cap *= 2;
        gc->stack = realloc(gc->stack, sizeof(GtkMl_SObj) * gc->stack_cap);
    }
    gc->stack[gc->stack_len++] = sobj;
}

GtkMl_SObj gtk_ml_gc_pop(GtkMl_Gc *gc) {
    return gc->stack[--gc->stack_len];
}

void gtk_ml_push(GtkMl_Context *ctx, GtkMl_TaggedValue value) {
    gtk_ml_vm_push(ctx->vm, value);
    if (gtk_ml_is_sobject(value)) {
        gtk_ml_gc_push(ctx->gc, value.value.sobj);
    }
}

void gtk_ml_set_local(GtkMl_Context *ctx, GtkMl_TaggedValue value) {
    gtk_ml_set_local_internal(ctx->vm, ctx->gc, value);
}

GtkMl_TaggedValue gtk_ml_get_local(GtkMl_Context *ctx, int64_t offset) {
    return gtk_ml_get_local_internal(ctx->vm, ctx->gc, offset);
}

void gtk_ml_set_local_internal(GtkMl_Vm *vm, GtkMl_Gc *gc, GtkMl_TaggedValue value) {
    if (vm->local_len == vm->local_cap) {
        vm->local_cap *= 2;
        vm->local = realloc(vm->local, sizeof(GtkMl_TaggedValue) * vm->local_cap);
    }
    vm->local[vm->local_len++] = value;
    if (gtk_ml_is_sobject(value)) {
        if (gc->local_len == gc->local_cap) {
            gc->local_cap *= 2;
            gc->local = realloc(gc->local, sizeof(GtkMl_TaggedValue) * gc->local_cap);
        }
        gc->local[gc->local_len++] = value.value.sobj;
    }
}

GtkMl_TaggedValue gtk_ml_get_local_internal(GtkMl_Vm *vm, GtkMl_Gc *gc, int64_t offset) {
    (void) gc;
    int64_t ptr = vm->local_base + offset;
    if (ptr < 0 || ptr >= (int64_t) vm->local_len) {
        return gtk_ml_value_none();
    }
    GtkMl_TaggedValue value = vm->local[ptr];
    return value;
}

GtkMl_TaggedValue gtk_ml_pop(GtkMl_Context *ctx) {
    GtkMl_TaggedValue result = gtk_ml_vm_pop(ctx->vm);
    if (gtk_ml_is_sobject(result)) {
        GtkMl_SObj gc_result = gtk_ml_gc_pop(ctx->gc);
        if (result.value.sobj != gc_result) {
            size_t offset = 0;
            fprintf(stderr, "fatal error: vm stack and gc stack got desynchronized %p, %p\n\n vm value: ", (void *) result.value.sobj, (void *) gc_result);
            (void) gtk_ml_dumpf(ctx, stderr, NULL, result.value.sobj);
            fprintf(stderr, "\n gc value: ");
            (void) gtk_ml_dumpf(ctx, stderr, NULL, gc_result);
            fprintf(stderr, "\n\n popping from vm stack until equal\n");
            while (result.value.sobj != gc_result) {
                result = gtk_ml_vm_pop(ctx->vm);
                ++offset;
            }
            fprintf(stderr, " offset is %zu\n", offset);
            exit(1);
        }
    }
    return result;
}

GtkMl_TaggedValue gtk_ml_peek(GtkMl_Context *ctx) {
    return ctx->vm->stack[ctx->vm->stack_len - 1];
}

void gtk_ml_load_program(GtkMl_Context *ctx, GtkMl_Program* program) {
    ctx->program = program;
    ctx->vm->program = program;
}

gboolean gtk_ml_run_program_internal(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_SObj program, GtkMl_SObj args, gboolean brk) {
    if (program->kind != GTKML_S_PROGRAM) {
        *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 2,
            gtk_ml_new_keyword(ctx, NULL, 0, "expected", strlen("expected")), gtk_ml_new_keyword(ctx, NULL, 0, "list", strlen("list")),
            gtk_ml_new_keyword(ctx, NULL, 0, "got-value", strlen("got-value")), program);
        return 0;
    }

    GtkMl_SObj params = program->value.s_program.args;

    int64_t n_args = 0;
    int64_t n_params = 0;

    if (args) {
        while (args->kind != GTKML_S_NIL) {
            GtkMl_SObj arg = gtk_ml_car(args);
            gtk_ml_push(ctx, gtk_ml_value_sobject(arg));
            args = gtk_ml_cdr(args);
            ++n_args;
        }
    }

    GtkMl_SObj last_param = NULL;
    while (params->kind != GTKML_S_NIL) {
        last_param = gtk_ml_car(params);
        params = gtk_ml_cdr(params);
        ++n_params;
    }

    if (n_params < n_args) {
        if (!last_param || last_param->kind != GTKML_S_VARARG) {
            *err = gtk_ml_error(ctx, "arity-error", GTKML_ERR_ARITY_ERROR, 0, 0, 0, 0);
            return 0;
        }
    }

    gtk_ml_push(ctx, gtk_ml_value_int(n_args));

    uint32_t pc = ctx->vm->pc;
    uint32_t flags = ctx->vm->flags & ~GTKML_F_GENERIC;
    ctx->vm->pc = program->value.s_program.addr;
    gboolean result = gtk_ml_vm_run(ctx->vm, err, brk);
    ctx->vm->pc = pc;
    ctx->vm->flags = (ctx->vm->flags & GTKML_F_GENERIC) | flags;

    return result;
}

gboolean gtk_ml_run_program(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_SObj program, GtkMl_SObj args) {
    return gtk_ml_run_program_internal(ctx, err, program, args, 1);
}

GtkMl_SObj gtk_ml_get_export(GtkMl_Context *ctx, GtkMl_SObj *err, const char *linkage_name) {
    GtkMl_SObj ext = gtk_ml_new_string(ctx, NULL, linkage_name, strlen(linkage_name));
    for (size_t i = 0; i < ctx->vm->program->n_text; i++) {
        GtkMl_Instruction instr = ctx->vm->program->text[i];
        if (instr.category == GTKML_I_EXPORT) {
            GtkMl_SObj program = ctx->vm->program->statics[ctx->vm->program->data[instr.data].value.u64];
            if (gtk_ml_equal(ext, program->value.s_program.linkage_name)) {
                return program;
            }
        }
    }

    char *name = malloc(strlen(linkage_name) + 1);
    strcpy(name, linkage_name);
    GtkMl_SObj error = gtk_ml_error(ctx, "binding-error", GTKML_ERR_BINDING_ERROR, 0, 0, 0, 1, gtk_ml_new_keyword(ctx, NULL, 0, "binding", strlen("binding")), gtk_ml_new_symbol(ctx, NULL, 1, name, strlen(name)));
    *err = error;
    return NULL;
}

void gtk_ml_del_program(GtkMl_Program* program) {
    free((void *) program->start);
    free(program->text);
    free(program->data);
    free(program->statics);
    free(program);
}

GTKML_PRIVATE GtkMl_VisitResult array_to_c_str(GtkMl_Array *array, size_t idx, GtkMl_TaggedValue value, GtkMl_TaggedValue data) {
    (void) array;
    char *c_str = data.value.userdata;
    c_str[idx] = value.value.unicode;
    return GTKML_VISIT_RECURSE;
}

char *gtk_ml_to_c_str(GtkMl_SObj string) {
    GtkMl_Array *array = &string->value.s_array.array;
    size_t len = gtk_ml_array_trie_len(array);
    char *c_str = malloc(len + 1);
    gtk_ml_array_trie_foreach(array, array_to_c_str, gtk_ml_value_userdata(c_str));
    c_str[len] = 0;
    return c_str;
}

GtkMl_SObj gtk_ml_load(GtkMl_Context *ctx, char **src, GtkMl_SObj *err, const char *file) {
    FILE *stream = fopen(file, "r");
    if (!stream) {
        *err = gtk_ml_error(ctx, "io-error", GTKML_ERR_IO_ERROR, 0, 0, 0, 0);
        return NULL;
    }
    GtkMl_SObj result = gtk_ml_loadf(ctx, src, err, stream);
    fclose(stream);
    return result;
}

GtkMl_SObj gtk_ml_loadf(GtkMl_Context *ctx, char **src, GtkMl_SObj *err, FILE *stream) {
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

GtkMl_SObj gtk_ml_loads(GtkMl_Context *ctx, GtkMl_SObj *err, const char *src) {
    GtkMl_Token *tokenv;
    size_t tokenc;
    if (!gtk_ml_lex(ctx, &tokenv, &tokenc, err, src)) {
        return NULL;
    }
    GtkMl_Token *_tokenv = tokenv;

    GtkMl_SObj body = gtk_ml_new_nil(ctx, NULL);
    GtkMl_SObj *last = &body;

    while (tokenc) {
        GtkMl_SObj line = gtk_ml_parse(ctx, err, &_tokenv, &tokenc);
        if (!line) {
            return NULL;
        }
        GtkMl_SObj new = gtk_ml_new_list(ctx, NULL, line, *last);
        *last = new;
        last = &gtk_ml_cdr(new);
    }

    GtkMl_SObj result = gtk_ml_new_lambda(ctx, NULL, gtk_ml_new_nil(ctx, NULL), body, gtk_ml_new_nil(ctx, NULL));

    free(tokenv);
    return result;
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
    gboolean (*hasher)(GtkMl_Hash *hash, GtkMl_TaggedValue value);
};

GTKML_PRIVATE GtkMl_VisitResult hash_trie_update(GtkMl_HashTrie *ht, GtkMl_TaggedValue key, GtkMl_TaggedValue value, GtkMl_TaggedValue _data) {
    (void) ht;

    struct HashData *data = _data.value.userdata;
    data->hasher(data->hash, key);
    data->hasher(data->hash, value);

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult hash_set_update(GtkMl_HashSet *hs, GtkMl_TaggedValue value, GtkMl_TaggedValue _data) {
    (void) hs;

    struct HashData *data = _data.value.userdata;
    data->hasher(data->hash, value);

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult array_update(GtkMl_Array *array, size_t index, GtkMl_TaggedValue value, GtkMl_TaggedValue _data) {
    (void) array;
    (void) index;

    struct HashData *data = _data.value.userdata;
    data->hasher(data->hash, value);

    return GTKML_VISIT_RECURSE;
}

void default_hash_start(GtkMl_Hash *hash) {
    jenkins_start(hash);
}

gboolean default_hash_update(GtkMl_Hash *hash, GtkMl_TaggedValue ptr) {
    GtkMl_SObj value = ptr.value.sobj;

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
    case GTKML_S_CHAR:
        jenkins_update(hash, &value->value.s_char.value, sizeof(uint32_t));
        break;
    case GTKML_S_SYMBOL:
        jenkins_update(hash, value->value.s_symbol.ptr, value->value.s_symbol.len);
        break;
    case GTKML_S_KEYWORD:
        jenkins_update(hash, value->value.s_keyword.ptr, value->value.s_keyword.len);
        break;
    case GTKML_S_LIST:
        do {
            default_hash_update(hash, gtk_ml_value_sobject(gtk_ml_car(value)));
            value = gtk_ml_cdr(value);
        } while (value->kind != GTKML_S_NIL);
        break;
    case GTKML_S_MAP: {
        struct HashData data = { hash, default_hash_update };
        gtk_ml_hash_trie_foreach(&value->value.s_map.map, hash_trie_update, gtk_ml_value_userdata(&data));
    } break;
    case GTKML_S_SET: {
        struct HashData data = { hash, default_hash_update };
        gtk_ml_hash_set_foreach(&value->value.s_set.set, hash_set_update, gtk_ml_value_userdata(&data));
    } break;
    case GTKML_S_ARRAY: {
        struct HashData data;
        if (gtk_ml_array_trie_is_string(&value->value.s_array.array)) {
            data = (struct HashData) { hash, value_hash_update };
        } else {
            data = (struct HashData) { hash, default_hash_update };
        }
        gtk_ml_array_trie_foreach(&value->value.s_array.array, array_update, gtk_ml_value_userdata(&data));
    } break;
    case GTKML_S_VAR:
        default_hash_update(hash, gtk_ml_value_sobject(value->value.s_var.expr));
        break;
    case GTKML_S_VARARG:
        default_hash_update(hash, gtk_ml_value_sobject(value->value.s_vararg.expr));
        break;
    case GTKML_S_QUOTE:
        default_hash_update(hash, gtk_ml_value_sobject(value->value.s_quote.expr));
        break;
    case GTKML_S_QUASIQUOTE:
        default_hash_update(hash, gtk_ml_value_sobject(value->value.s_quasiquote.expr));
        break;
    case GTKML_S_UNQUOTE:
        default_hash_update(hash, gtk_ml_value_sobject(value->value.s_unquote.expr));
        break;
    case GTKML_S_LAMBDA:
        default_hash_update(hash, gtk_ml_value_sobject(value->value.s_lambda.args));
        default_hash_update(hash, gtk_ml_value_sobject(value->value.s_lambda.body));
        default_hash_update(hash, gtk_ml_value_sobject(value->value.s_lambda.capture));
        break;
    case GTKML_S_PROGRAM:
        default_hash_update(hash, gtk_ml_value_sobject(value->value.s_program.linkage_name));
        jenkins_update(hash, &value->value.s_program.addr, sizeof(uint64_t));
        default_hash_update(hash, gtk_ml_value_sobject(value->value.s_program.args));
        default_hash_update(hash, gtk_ml_value_sobject(value->value.s_program.body));
        default_hash_update(hash, gtk_ml_value_sobject(value->value.s_program.capture));
        break;
    case GTKML_S_ADDRESS:
        default_hash_update(hash, gtk_ml_value_sobject(value->value.s_address.linkage_name));
        jenkins_update(hash, &value->value.s_address.addr, sizeof(uint64_t));
        break;
    case GTKML_S_MACRO:
        default_hash_update(hash, gtk_ml_value_sobject(value->value.s_macro.args));
        default_hash_update(hash, gtk_ml_value_sobject(value->value.s_macro.body));
        default_hash_update(hash, gtk_ml_value_sobject(value->value.s_macro.capture));
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

gboolean default_equal(GtkMl_TaggedValue lhs, GtkMl_TaggedValue rhs) {
    return gtk_ml_equal_value(lhs, rhs);
}

void value_hash_start(GtkMl_Hash *hash) {
    jenkins_start(hash);
}

gboolean value_hash_update(GtkMl_Hash *hash, GtkMl_TaggedValue value) {
    jenkins_update(hash, &value, sizeof(value));
    return 1;
}

void value_hash_finish(GtkMl_Hash *hash) {
    jenkins_finish(hash);
}

gboolean value_equal(GtkMl_TaggedValue lhs, GtkMl_TaggedValue rhs) {
    return gtk_ml_equal_value(lhs, rhs);
}

void ptr_hash_start(GtkMl_Hash *hash) {
    jenkins_start(hash);
}

gboolean ptr_hash_update(GtkMl_Hash *hash, GtkMl_TaggedValue ptr) {
    *hash = ptr.value.u64 >> 3;
    return 1;
}

void ptr_hash_finish(GtkMl_Hash *hash) {
    jenkins_finish(hash);
}

gboolean ptr_equal(GtkMl_TaggedValue lhs, GtkMl_TaggedValue rhs) {
    return lhs.value.u64 == rhs.value.u64;
}

GTKML_PRIVATE void mark_sobject(GtkMl_SObj s);

GTKML_PRIVATE GtkMl_VisitResult mark_hash_trie(GtkMl_HashTrie *ht, GtkMl_TaggedValue key, GtkMl_TaggedValue value, GtkMl_TaggedValue data) {
    (void) ht;
    (void) data;

    if (gtk_ml_is_sobject(key)) {
        mark_sobject(key.value.sobj);
    }
    if (gtk_ml_is_sobject(value)) {
        mark_sobject(value.value.sobj);
    }

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult mark_hash_set(GtkMl_HashSet *hs, GtkMl_TaggedValue key, GtkMl_TaggedValue data) {
    (void) hs;
    (void) data;

    if (gtk_ml_is_sobject(key)) {
        mark_sobject(key.value.sobj);
    }

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult mark_array(GtkMl_Array *array, size_t idx, GtkMl_TaggedValue value, GtkMl_TaggedValue data) {
    (void) array;
    (void) idx;
    (void) data;

    if (gtk_ml_is_sobject(value)) {
        mark_sobject(value.value.sobj);
    }

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE void mark_sobject(GtkMl_SObj s) {
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
    case GTKML_S_CHAR:
    case GTKML_S_KEYWORD:
    case GTKML_S_SYMBOL:
    case GTKML_S_LIGHTDATA:
        break;
    case GTKML_S_USERDATA:
        mark_sobject(s->value.s_userdata.keep);
        break;
    case GTKML_S_LIST:
        mark_sobject(gtk_ml_car(s));
        mark_sobject(gtk_ml_cdr(s));
        break;
    case GTKML_S_MAP:
        gtk_ml_hash_trie_foreach(&s->value.s_map.map, mark_hash_trie, gtk_ml_value_none());
        if (s->value.s_map.metamap) {
            mark_sobject(s->value.s_map.metamap);
        }
        break;
    case GTKML_S_SET:
        gtk_ml_hash_set_foreach(&s->value.s_set.set, mark_hash_set, gtk_ml_value_none());
        break;
    case GTKML_S_ARRAY:
        if (!gtk_ml_array_trie_is_string(&s->value.s_array.array)) {
            gtk_ml_array_trie_foreach(&s->value.s_array.array, mark_array, gtk_ml_value_none());
        }
        break;
    case GTKML_S_VAR:
        mark_sobject(s->value.s_var.expr);
        break;
    case GTKML_S_VARARG:
        mark_sobject(s->value.s_vararg.expr);
        break;
    case GTKML_S_QUOTE:
        mark_sobject(s->value.s_quote.expr);
        break;
    case GTKML_S_QUASIQUOTE:
        mark_sobject(s->value.s_quasiquote.expr);
        break;
    case GTKML_S_UNQUOTE:
        mark_sobject(s->value.s_unquote.expr);
        break;
    case GTKML_S_ADDRESS:
        mark_sobject(s->value.s_address.linkage_name);
        break;
    case GTKML_S_PROGRAM:
        mark_sobject(s->value.s_program.linkage_name);
        mark_sobject(s->value.s_program.args);
        mark_sobject(s->value.s_program.body);
        mark_sobject(s->value.s_program.capture);
        break;
    case GTKML_S_LAMBDA:
        mark_sobject(s->value.s_lambda.args);
        mark_sobject(s->value.s_lambda.body);
        mark_sobject(s->value.s_lambda.capture);
        break;
    case GTKML_S_MACRO:
        mark_sobject(s->value.s_macro.args);
        mark_sobject(s->value.s_macro.body);
        mark_sobject(s->value.s_macro.capture);
        break;
    }
}

GTKML_PRIVATE void mark_program(GtkMl_Program *program) {
    for (GtkMl_Static i = 1; i < program->n_static; i++) {
        mark_sobject(program->statics[i]);
    }
}

GTKML_PRIVATE void mark_builder(GtkMl_Builder *b) {
    for (GtkMl_Static i = 1; i < b->len_static; i++) {
        mark_sobject(b->statics[i]);
    }
    mark_sobject(b->bindings);
}

GTKML_PRIVATE void mark(GtkMl_Context *ctx) {
    for (size_t sp = 0; sp < ctx->gc->stack_len; sp++) {
        mark_sobject(ctx->gc->stack[sp]);
    }
    for (int64_t sp = 0; sp < ctx->gc->local_len; sp++) {
        mark_sobject(ctx->gc->local[sp]);
    }
    if (ctx->gc->static_stack) {
        mark_sobject(ctx->gc->static_stack);
    }
    for (size_t i = 0; i < ctx->gc->program_len; i++) {
        mark_program(ctx->gc->programs[i]);
    }
    if (ctx->gc->builder) {
        mark_builder(ctx->gc->builder);
    }
}

void gtk_ml_delete(GtkMl_Context *ctx, GtkMl_SObj s) {
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
    case GTKML_S_CHAR:
    case GTKML_S_LIGHTDATA:
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
        gtk_ml_delete(ctx, gtk_ml_cdr(s));
        gtk_ml_delete(ctx, gtk_ml_car(s));
        break;
    case GTKML_S_MAP:
        gtk_ml_del_hash_trie(ctx, &s->value.s_map.map, gtk_ml_delete_value);
        break;
    case GTKML_S_SET:
        gtk_ml_del_hash_set(ctx, &s->value.s_set.set, gtk_ml_delete_value);
        break;
    case GTKML_S_ARRAY:
        gtk_ml_del_array_trie(ctx, &s->value.s_array.array, gtk_ml_delete_value);
        break;
    case GTKML_S_VAR:
        gtk_ml_delete(ctx, s->value.s_var.expr);
        break;
    case GTKML_S_VARARG:
        gtk_ml_delete(ctx, s->value.s_vararg.expr);
        break;
    case GTKML_S_QUOTE:
        gtk_ml_delete(ctx, s->value.s_quote.expr);
        break;
    case GTKML_S_QUASIQUOTE:
        gtk_ml_delete(ctx, s->value.s_quasiquote.expr);
        break;
    case GTKML_S_UNQUOTE:
        gtk_ml_delete(ctx, s->value.s_unquote.expr);
        break;
    case GTKML_S_ADDRESS:
        gtk_ml_delete(ctx, s->value.s_address.linkage_name);
        break;
    case GTKML_S_PROGRAM:
        gtk_ml_delete(ctx, s->value.s_program.linkage_name);
        gtk_ml_delete(ctx, s->value.s_program.args);
        gtk_ml_delete(ctx, s->value.s_program.capture);
        break;
    case GTKML_S_LAMBDA:
        gtk_ml_delete(ctx, s->value.s_lambda.args);
        gtk_ml_delete(ctx, s->value.s_lambda.body);
        gtk_ml_delete(ctx, s->value.s_lambda.capture);
        break;
    case GTKML_S_MACRO:
        gtk_ml_delete(ctx, s->value.s_macro.args);
        gtk_ml_delete(ctx, s->value.s_macro.body);
        gtk_ml_delete(ctx, s->value.s_macro.capture);
        break;
    }
    if (ctx->gc->free_len == ctx->gc->free_cap) {
        ctx->gc->free_cap *= 2;
        ctx->gc->free_all = realloc(ctx->gc->free_all, sizeof(GtkMl_SObj) * ctx->gc->free_cap);
    }
    ctx->gc->free_all[ctx->gc->free_len++] = s;
    --ctx->gc->n_values;
}

void gtk_ml_del(GtkMl_Context *ctx, GtkMl_SObj s) {
    switch (s->kind) {
    case GTKML_S_NIL:
    case GTKML_S_TRUE:
    case GTKML_S_FALSE:
    case GTKML_S_INT:
    case GTKML_S_FLOAT:
    case GTKML_S_CHAR:
    case GTKML_S_LIGHTDATA:
    case GTKML_S_LIST:
    case GTKML_S_VAR:
    case GTKML_S_VARARG:
    case GTKML_S_QUOTE:
    case GTKML_S_QUASIQUOTE:
    case GTKML_S_UNQUOTE:
    case GTKML_S_LAMBDA:
    case GTKML_S_ADDRESS:
    case GTKML_S_PROGRAM:
    case GTKML_S_MACRO:
        break;
    case GTKML_S_MAP:
        gtk_ml_del_hash_trie(ctx, &s->value.s_map.map, gtk_ml_delete_value);
        break;
    case GTKML_S_SET:
        gtk_ml_del_hash_set(ctx, &s->value.s_set.set, gtk_ml_delete_value);
        break;
    case GTKML_S_ARRAY:
        gtk_ml_del_array_trie(ctx, &s->value.s_array.array, gtk_ml_delete_value);
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
    }
    if (ctx->gc->free_len == ctx->gc->free_cap) {
        ctx->gc->free_cap *= 2;
        ctx->gc->free_all = realloc(ctx->gc->free_all, sizeof(GtkMl_SObj) * ctx->gc->free_cap);
    }
    ctx->gc->free_all[ctx->gc->free_len++] = s;
    --ctx->gc->n_values;
}

GTKML_PRIVATE void sweep(GtkMl_Context *ctx) {
    GtkMl_SObj *value = &ctx->gc->first;
    while (*value) {
        if ((*value)->flags & GTKML_FLAG_REACHABLE) {
            (*value)->flags &= ~GTKML_FLAG_REACHABLE;
            value = &(*value)->next;
        } else {
            GtkMl_SObj unreachable = *value;
            *value = (*value)->next;
            gtk_ml_del(ctx, unreachable);
        }
    }
}

// simple mark & sweep gc
gboolean gtk_ml_collect(GtkMl_Context *ctx) {
    if (!ctx->gc->gc_enabled) {
        return 0;
    }

    if (ctx->gc->n_values < ctx->gc->m_values) {
        return 0;
    }

    size_t n_values = ctx->gc->n_values;
    mark(ctx);
    sweep(ctx);
    free_all(ctx->gc);
    ctx->gc->m_values = 2 * n_values;

    return 1;
}

gboolean gtk_ml_disable_gc(GtkMl_Context *ctx) {
    gboolean enabled = ctx->gc->gc_enabled;
    ctx->gc->gc_enabled = 0;
    return enabled;
}

void gtk_ml_enable_gc(GtkMl_Context *ctx, gboolean enabled) {
    ctx->gc->gc_enabled = enabled;
}

gboolean gtk_ml_equal(GtkMl_SObj lhs, GtkMl_SObj rhs) {
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
    case GTKML_S_CHAR:
        return lhs->value.s_char.value == rhs->value.s_char.value;
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
        return gtk_ml_array_trie_equal(&lhs->value.s_array.array, &rhs->value.s_array.array);
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

gboolean gtk_ml_equal_value(GtkMl_TaggedValue lhs, GtkMl_TaggedValue rhs) {
    if (gtk_ml_is_sobject(lhs) && gtk_ml_is_sobject(rhs)) {
        return gtk_ml_equal(lhs.value.sobj, rhs.value.sobj);
    } else if (gtk_ml_is_sobject(lhs) && gtk_ml_is_primitive(rhs)) {
        switch (lhs.value.sobj->kind) {
        case GTKML_S_NIL:
            return rhs.tag == GTKML_TAG_NIL;
        case GTKML_S_TRUE:
            return rhs.tag == GTKML_TAG_BOOL && rhs.value.boolean != 0;
        case GTKML_S_FALSE:
            return rhs.tag == GTKML_TAG_BOOL && rhs.value.boolean == 0;
        case GTKML_S_INT:
            return (rhs.tag & GTKML_TAG_INT) && lhs.value.sobj->value.s_int.value == rhs.value.s64;
        case GTKML_S_FLOAT:
            return rhs.tag == GTKML_TAG_FLOAT && lhs.value.sobj->value.s_float.value == rhs.value.flt;
        case GTKML_S_CHAR:
            return rhs.tag == GTKML_TAG_CHAR && lhs.value.sobj->value.s_char.value == rhs.value.unicode;
        case GTKML_S_LIGHTDATA:
            return rhs.tag == GTKML_TAG_USERDATA && lhs.value.sobj->value.s_lightdata.userdata == rhs.value.userdata;
        case GTKML_S_KEYWORD:
        case GTKML_S_SYMBOL:
        case GTKML_S_USERDATA:
        case GTKML_S_LAMBDA:
        case GTKML_S_PROGRAM:
        case GTKML_S_ADDRESS:
        case GTKML_S_MACRO:
        case GTKML_S_LIST:
        case GTKML_S_MAP:
        case GTKML_S_SET:
        case GTKML_S_ARRAY:
        case GTKML_S_VAR:
        case GTKML_S_VARARG:
        case GTKML_S_QUOTE:
        case GTKML_S_QUASIQUOTE:
        case GTKML_S_UNQUOTE:
            return 0;
        }
    } else if (gtk_ml_is_primitive(lhs) && gtk_ml_is_sobject(rhs)) {
        switch (rhs.value.sobj->kind) {
        case GTKML_S_NIL:
            return lhs.tag == GTKML_TAG_NIL;
        case GTKML_S_TRUE:
            return lhs.tag == GTKML_TAG_BOOL && lhs.value.boolean != 0;
        case GTKML_S_FALSE:
            return lhs.tag == GTKML_TAG_BOOL && lhs.value.boolean == 0;
        case GTKML_S_INT:
            return (lhs.tag & GTKML_TAG_INT) && rhs.value.sobj->value.s_int.value == lhs.value.s64;
        case GTKML_S_FLOAT:
            return lhs.tag == GTKML_TAG_FLOAT && rhs.value.sobj->value.s_float.value == lhs.value.flt;
        case GTKML_S_CHAR:
            return lhs.tag == GTKML_TAG_CHAR && rhs.value.sobj->value.s_char.value == lhs.value.unicode;
        case GTKML_S_LIGHTDATA:
            return lhs.tag == GTKML_TAG_USERDATA && rhs.value.sobj->value.s_lightdata.userdata == lhs.value.userdata;
        case GTKML_S_KEYWORD:
        case GTKML_S_SYMBOL:
        case GTKML_S_USERDATA:
        case GTKML_S_LAMBDA:
        case GTKML_S_PROGRAM:
        case GTKML_S_ADDRESS:
        case GTKML_S_MACRO:
        case GTKML_S_LIST:
        case GTKML_S_MAP:
        case GTKML_S_SET:
        case GTKML_S_ARRAY:
        case GTKML_S_VAR:
        case GTKML_S_VARARG:
        case GTKML_S_QUOTE:
        case GTKML_S_QUASIQUOTE:
        case GTKML_S_UNQUOTE:
            return 0;
        }
    } else {
        return lhs.tag == rhs.tag && lhs.value.u64 == rhs.value.u64;
    }
}

gboolean gtk_ml_hash(GtkMl_Hasher *hasher, GtkMl_Hash *hash, GtkMl_TaggedValue value) {
    hasher->start(hash);
    if (hasher->update(hash, value)) {
        hasher->finish(hash);
        return 1;
    }
    return 0;
}

gboolean gtk_ml_hash_debug(GtkMl_Context *ctx, GtkMl_Hasher *hasher, GtkMl_Hash *hash, GtkMl_TaggedValue value) {
    (void) ctx;
    (void) hasher;
    (void) hash;
    (void) value;
    fprintf(stderr, "warning: hashing in debug mode is currently unavailable\n");
    return 0;
}

void gtk_ml_setmetamap(GtkMl_SObj value, GtkMl_SObj mm) {
    if (value->kind == GTKML_S_MAP) {
        value->value.s_map.metamap = mm;
    }
}

GtkMl_SObj gtk_ml_getmetamap(GtkMl_SObj value) {
    if (value->kind == GTKML_S_MAP) {
        return value->value.s_map.metamap;
    } else {
        return NULL;
    }
}

GtkMl_SObj gtk_ml_error(GtkMl_Context *ctx, const char *err, const char *description, gboolean has_loc, int64_t line, int64_t col, size_t n, ...) {
    char *desc = malloc(strlen(description) + 1);
    strcpy(desc, description);

    GtkMl_SObj s_err = gtk_ml_new_symbol(ctx, NULL, 0, err, strlen(err));
    GtkMl_SObj s_desc = gtk_ml_new_string(ctx, NULL, desc, strlen(desc));
    GtkMl_SObj s_has_loc = has_loc? gtk_ml_new_true(ctx, NULL) : gtk_ml_new_false(ctx, NULL);
    GtkMl_SObj s_line = has_loc? gtk_ml_new_int(ctx, NULL, line) : NULL;
    GtkMl_SObj s_col = has_loc? gtk_ml_new_int(ctx, NULL, col) : NULL;

    GtkMl_SObj s_loc = gtk_ml_new_array(ctx, NULL);
    GtkMl_SObj new_loc = gtk_ml_new_array(ctx, NULL);
    gtk_ml_array_trie_push(&new_loc->value.s_array.array, &s_loc->value.s_array.array, gtk_ml_value_sobject(s_has_loc));
    s_loc = new_loc;
    if (has_loc) {
        new_loc = gtk_ml_new_array(ctx, NULL);

        gtk_ml_array_trie_push(&new_loc->value.s_array.array, &s_loc->value.s_array.array, gtk_ml_value_sobject(s_line));
        s_loc = new_loc;

        new_loc = gtk_ml_new_array(ctx, NULL);
        gtk_ml_array_trie_push(&new_loc->value.s_array.array, &s_loc->value.s_array.array, gtk_ml_value_sobject(s_col));
        s_loc = new_loc;
    }

    GtkMl_SObj s_stacktrace = NULL;
    if (ctx->vm->program && ctx->vm->program->text) {
        s_stacktrace = gtk_ml_new_array(ctx, NULL);
        for (size_t i = 0; i < ctx->vm->call_stack_ptr; i += 2) {
            GtkMl_SObj program = gtk_ml_new_nil(ctx, NULL);
            size_t call_at = ctx->vm->call_stack[i + 1];
            GtkMl_SObj export = NULL;
            for (size_t ptr = 0; ptr <= call_at; ptr += 8) {
                GtkMl_Instruction instr = ctx->vm->program->text[ptr >> 3];
                if (instr.category == GTKML_I_EXPORT) {
                    if (ctx->vm->program->statics[ctx->vm->program->data[instr.data].value.u64]->kind == GTKML_S_PROGRAM
                            || ctx->vm->program->statics[ctx->vm->program->data[instr.data].value.u64]->kind == GTKML_S_ADDRESS) {
                        export = ctx->vm->program->statics[ctx->vm->program->data[instr.data].value.u64];
                    }
                }
            }

            if (export) {
                program = gtk_ml_new_array(ctx, NULL);

                if (export->kind == GTKML_S_PROGRAM) {
                    GtkMl_SObj new = gtk_ml_new_array(ctx, NULL);
                    gtk_ml_array_trie_push(&new->value.s_array.array, &program->value.s_array.array, gtk_ml_value_sobject(export->value.s_program.linkage_name));
                    program = new;

                    new = gtk_ml_new_array(ctx, NULL);
                    gtk_ml_array_trie_push(&new->value.s_array.array, &program->value.s_array.array, gtk_ml_value_sobject(gtk_ml_new_int(ctx, NULL, export->value.s_program.addr)));
                    program = new;
                } else if (export->kind == GTKML_S_ADDRESS) {
                    GtkMl_SObj new = gtk_ml_new_array(ctx, NULL);
                    gtk_ml_array_trie_push(&new->value.s_array.array, &program->value.s_array.array, gtk_ml_value_sobject(export->value.s_address.linkage_name));
                    program = new;

                    new = gtk_ml_new_array(ctx, NULL);
                    gtk_ml_array_trie_push(&new->value.s_array.array, &program->value.s_array.array, gtk_ml_value_sobject(gtk_ml_new_int(ctx, NULL, export->value.s_address.addr)));
                    program = new;
                }

                GtkMl_SObj new = gtk_ml_new_array(ctx, NULL);
                gtk_ml_array_trie_push(&new->value.s_array.array, &program->value.s_array.array, gtk_ml_value_sobject(gtk_ml_new_int(ctx, NULL, call_at)));
                program = new;
            }
            GtkMl_SObj new = gtk_ml_new_array(ctx, NULL);
            gtk_ml_array_trie_push(&new->value.s_array.array, &s_stacktrace->value.s_array.array, gtk_ml_value_sobject(program));
            s_stacktrace = new;
        }

        GtkMl_SObj program = gtk_ml_new_nil(ctx, NULL);
        size_t call_at = ctx->vm->pc;
        GtkMl_SObj export = NULL;
        for (size_t ptr = 0; ptr <= call_at; ptr += 8) {
            GtkMl_Instruction instr = ctx->vm->program->text[ptr >> 3];
            if ((instr.category & GTKML_I_EXPORT) == GTKML_I_EXPORT) {
                GtkMl_SObj exp = ctx->vm->program->statics[ctx->vm->program->data[instr.data].value.u64];
                if (exp->kind == GTKML_S_PROGRAM || exp->kind == GTKML_S_ADDRESS) {
                    export = exp;
                }
            }
        }

        if (export) {
            program = gtk_ml_new_array(ctx, NULL);

            if (export->kind == GTKML_S_PROGRAM) {
                GtkMl_SObj new = gtk_ml_new_array(ctx, NULL);
                gtk_ml_array_trie_push(&new->value.s_array.array, &program->value.s_array.array, gtk_ml_value_sobject(export->value.s_program.linkage_name));
                program = new;

                new = gtk_ml_new_array(ctx, NULL);
                gtk_ml_array_trie_push(&new->value.s_array.array, &program->value.s_array.array, gtk_ml_value_sobject(gtk_ml_new_int(ctx, NULL, export->value.s_program.addr)));
                program = new;
            } else if (export->kind == GTKML_S_ADDRESS) {
                GtkMl_SObj new = gtk_ml_new_array(ctx, NULL);
                gtk_ml_array_trie_push(&new->value.s_array.array, &program->value.s_array.array, gtk_ml_value_sobject(export->value.s_address.linkage_name));
                program = new;

                new = gtk_ml_new_array(ctx, NULL);
                gtk_ml_array_trie_push(&new->value.s_array.array, &program->value.s_array.array, gtk_ml_value_sobject(gtk_ml_new_int(ctx, NULL, export->value.s_address.addr)));
                program = new;
            }

            GtkMl_SObj new = gtk_ml_new_array(ctx, NULL);
            gtk_ml_array_trie_push(&new->value.s_array.array, &program->value.s_array.array, gtk_ml_value_sobject(gtk_ml_new_int(ctx, NULL, call_at)));
            program = new;
        }
        GtkMl_SObj new = gtk_ml_new_array(ctx, NULL);
        gtk_ml_array_trie_push(&new->value.s_array.array, &s_stacktrace->value.s_array.array, gtk_ml_value_sobject(program));
        s_stacktrace = new;
    }

    GtkMl_SObj error = gtk_ml_new_map(ctx, NULL, NULL);

    GtkMl_SObj new = gtk_ml_new_map(ctx, NULL, NULL);
    gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_value_sobject(gtk_ml_new_keyword(ctx, NULL, 0, "err", 3)), gtk_ml_value_sobject(s_err));
    error = new;

    new = gtk_ml_new_map(ctx, NULL, NULL);
    gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_value_sobject(gtk_ml_new_keyword(ctx, NULL, 0, "desc", 4)), gtk_ml_value_sobject(s_desc));
    error = new;

    new = gtk_ml_new_map(ctx, NULL, NULL);
    gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_value_sobject(gtk_ml_new_keyword(ctx, NULL, 0, "loc", 3)), gtk_ml_value_sobject(s_loc));
    error = new;

    if (s_stacktrace) {
        new = gtk_ml_new_map(ctx, NULL, NULL);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_value_sobject(gtk_ml_new_keyword(ctx, NULL, 0, "stacktrace", 10)), gtk_ml_value_sobject(s_stacktrace));
        error = new;
    }

    va_list args;
    va_start(args, n);

    for (size_t i = 0; i < n; i++) {
        GtkMl_SObj key = va_arg(args, GtkMl_SObj);
        GtkMl_SObj value = va_arg(args, GtkMl_SObj);

        new = gtk_ml_new_map(ctx, NULL, NULL);
        gtk_ml_hash_trie_insert(&new->value.s_map.map, &error->value.s_map.map, gtk_ml_value_sobject(key), gtk_ml_value_sobject(value));
        error = new;
    }

    va_end(args);

    return error;
}

void gtk_ml_delete_sobject_reference(GtkMl_Context *ctx, GtkMl_TaggedValue value) {
    (void) ctx;
    (void) value;
}

void gtk_ml_delete_sobject(GtkMl_Context *ctx, GtkMl_TaggedValue s) {
    gtk_ml_delete(ctx, s.value.sobj);
}

void gtk_ml_delete_value(GtkMl_Context *ctx, GtkMl_TaggedValue reference) {
    (void) ctx;
    (void) reference;
}

void gtk_ml_free(GtkMl_Context *ctx, void *ptr) {
    (void) ctx;
    free(ptr);
}

#ifdef GTKML_ENABLE_GTK
void gtk_ml_object_unref(GtkMl_Context *ctx, void *obj) {
    (void) ctx;
    g_object_unref(obj);
}
#endif /* GTKML_ENABLE_GTK */

#ifdef GTKML_ENABLE_POSIX
struct DumpfDebugData {
    FILE *stream;
    size_t n;
};

GTKML_PRIVATE GtkMl_VisitResult dumpf_debug_hash_trie(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_HashTrie *ht, GtkMl_TaggedValue key, GtkMl_TaggedValue value, GtkMl_TaggedValue _data) {
    struct DumpfDebugData *data = _data.value.userdata;

    gtk_ml_dumpf_debug(ctx, data->stream, err, key.value.sobj);
    if (*err) {
        return GTKML_VISIT_BREAK;
    }
    fprintf(data->stream, " ");
    gtk_ml_dumpf_debug(ctx, data->stream, err, value.value.sobj);
    if (*err) {
        return GTKML_VISIT_BREAK;
    }
    ++data->n;
    if (data->n < gtk_ml_hash_trie_len_debug(ctx, ht)) {
        fprintf(data->stream, " ");
    }

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult dumpf_debug_hash_set(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_HashSet *hs, GtkMl_TaggedValue key, GtkMl_TaggedValue _data) {
    struct DumpfDebugData *data = _data.value.userdata;

    gtk_ml_dumpf_debug(ctx, data->stream, err, key.value.sobj);
    if (*err) {
        return GTKML_VISIT_BREAK;
    }
    ++data->n;
    if (data->n < gtk_ml_hash_set_len_debug(ctx, hs)) {
        fprintf(data->stream, " ");
    }

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult dumpf_debug_array(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Array *array, size_t idx, GtkMl_TaggedValue value, GtkMl_TaggedValue _data) {
    (void) idx;

    struct DumpfDebugData *data = _data.value.userdata;

    gtk_ml_dumpf_debug(ctx, data->stream, err, value.value.sobj);
    if (*err) {
        return GTKML_VISIT_BREAK;
    }
    ++data->n;
    if (data->n < gtk_ml_array_trie_len(array)) {
        fprintf(data->stream, " ");
    }

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult dumpf_debug_string(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_Array *array, size_t idx, GtkMl_TaggedValue value, GtkMl_TaggedValue data) {
    (void) ctx;
    (void) err;
    (void) array;
    (void) idx;
    (void) data;

    if (isprint(value.value.unicode)) {
        fprintf(data.value.userdata, "%c", value.value.unicode);
    } else if (value.value.unicode < 0x100) {
        fprintf(data.value.userdata, "\\x%02x", value.value.unicode);
    } else if (value.value.unicode < 0x10000) {
        fprintf(data.value.userdata, "\\u%04x", value.value.unicode);
    } else {
        fprintf(data.value.userdata, "\\U%08x", value.value.unicode);
    }

    return GTKML_VISIT_RECURSE;
}

gboolean gtk_ml_dumpf_debug(GtkMl_Context *ctx, FILE *stream, GtkMl_SObj *err, GtkMl_SObj expr_ptr) {
    GtkMl_S expr_value;
    expr_value.next = NULL;
    gtk_ml_dbg_read_sobject(ctx, err, &expr_value, expr_ptr);
    if (*err) {
        return 0;
    }
    GtkMl_SObj expr = &expr_value;
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
        fprintf(stream, "%"GTKML_FMT_64"d", expr->value.s_int.value);
        return 1;
    case GTKML_S_FLOAT:
        fprintf(stream, "%f", expr->value.s_float.value);
        return 1;
    case GTKML_S_CHAR:
        switch (expr->value.s_char.value) {
        case '\n':
            fprintf(stream, "\\newline");
            break;
        case ' ':
            fprintf(stream, "\\space");
            break;
        case '\t':
            fprintf(stream, "\\tab");
            break;
        case '\x1b':
            fprintf(stream, "\\escape");
            break;
        default:
            if (isgraph(expr->value.s_char.value)) {
                fprintf(stream, "\\%c", expr->value.s_char.value);
            } else if (expr->value.s_char.value < 0x100) {
                fprintf(stream, "\\x%02x", expr->value.s_char.value);
            } else if (expr->value.s_char.value < 0x10000) {
                fprintf(stream, "\\u%04x", expr->value.s_char.value);
            } else {
                fprintf(stream, "\\U%08x", expr->value.s_char.value);
            }
            break;
        }
        return 1;
    case GTKML_S_KEYWORD:
        fprintf(stream, ":");
        for (size_t i = 0; i < expr->value.s_keyword.len; i++) {
            fprintf(stream, "%c", gtk_ml_dbg_read_u8(ctx, err, &expr->value.s_keyword.ptr[i]));
            if (*err) {
                return 0;
            }
        }
        return 1;
    case GTKML_S_SYMBOL:
        fprintf(stream, "'");
        for (size_t i = 0; i < expr->value.s_symbol.len; i++) {
            fprintf(stream, "%c", gtk_ml_dbg_read_u8(ctx, err, &expr->value.s_symbol.ptr[i]));
            if (*err) {
                return 0;
            }
        }
        return 1;
    case GTKML_S_LIST:
        fprintf(stream, "(");
        while (expr->kind != GTKML_S_NIL) {
            gtk_ml_dumpf_debug(ctx, stream, err, gtk_ml_car(expr));
            gtk_ml_dbg_read_sobject(ctx, err, expr, gtk_ml_cdr(expr));
            if (*err) {
                return 0;
            }
            if (expr->kind != GTKML_S_NIL) {
                fprintf(stream, " ");
            }
        }
        fprintf(stream, ")");
        return 1;
    case GTKML_S_MAP: {
        fprintf(stream, "{");
        struct DumpfDebugData data = { stream, 0 };
        if (!gtk_ml_hash_trie_foreach_debug(ctx, err, &expr->value.s_map.map, dumpf_debug_hash_trie, gtk_ml_value_userdata(&data))) {
            return 0;
        }
        fprintf(stream, "}");
        return 1;
    }
    case GTKML_S_SET: {
        fprintf(stream, "#{");
        struct DumpfDebugData data = { stream, 0 };
        if (!gtk_ml_hash_set_foreach_debug(ctx, err, &expr->value.s_set.set, dumpf_debug_hash_set, gtk_ml_value_userdata(&data))) {
            return 0;
        }
        fprintf(stream, "}");
        return 1;
    }
    case GTKML_S_ARRAY: {
        if (gtk_ml_array_trie_is_string_debug(ctx, err, &expr->value.s_array.array)) {
            fprintf(stream, "\"");
            if (!gtk_ml_array_trie_foreach_debug(ctx, err, &expr->value.s_array.array, dumpf_debug_string, gtk_ml_value_userdata(stream))) {
                return 0;
            }
            fprintf(stream, "\"");
            return 1;
        } else {
            fprintf(stream, "[");
            struct DumpfDebugData data = { stream, 0 };
            if (!gtk_ml_array_trie_foreach_debug(ctx, err, &expr->value.s_array.array, dumpf_debug_array, gtk_ml_value_userdata(&data))) {
                return 0;
            }
            fprintf(stream, "]");
            return 1;
        }
    }
    case GTKML_S_VAR:
        fprintf(stream, "(var ");
        if (!gtk_ml_dumpf_debug(ctx, stream, err, expr->value.s_vararg.expr)) {
            return 0;
        }
        fprintf(stream, ")");
        return 1;
    case GTKML_S_VARARG:
        fprintf(stream, "...");
        return gtk_ml_dumpf_debug(ctx, stream, err, expr->value.s_vararg.expr);
    case GTKML_S_QUOTE:
        fprintf(stream, "'");
        return gtk_ml_dumpf_debug(ctx, stream, err, expr->value.s_vararg.expr);
    case GTKML_S_QUASIQUOTE:
        fprintf(stream, "`");
        return gtk_ml_dumpf_debug(ctx, stream, err, expr->value.s_vararg.expr);
    case GTKML_S_UNQUOTE:
        fprintf(stream, ",");
        return gtk_ml_dumpf_debug(ctx, stream, err, expr->value.s_vararg.expr);
    case GTKML_S_LAMBDA: {
        fprintf(stream, "(lambda ");
        if (!gtk_ml_dumpf_debug(ctx, stream, err, expr->value.s_lambda.args)) {
            return 0;
        }
        fprintf(stream, " ");
        GtkMl_S body_value;
        body_value.next = NULL;
        gtk_ml_dbg_read_sobject(ctx, err, &body_value, expr->value.s_lambda.body);
        if (*err) {
            return 0;
        }
        GtkMl_SObj body = &body_value;
        while (body->kind != GTKML_S_NIL) {
            if (!gtk_ml_dumpf_debug(ctx, stream, err, gtk_ml_car(body))) {
                return 0;
            }
            gtk_ml_dbg_read_sobject(ctx, err, body, gtk_ml_cdr(body));
            if (*err) {
                return 0;
            }
            if (body->kind != GTKML_S_NIL) {
                fprintf(stream, " ");
            }
        }
        fprintf(stream, ")");
        return 1;
    }
    case GTKML_S_PROGRAM: {
        switch (expr->value.s_program.kind) {
        case GTKML_PROG_INTRINSIC:
            fprintf(stream, "(program-intrinsic ");
            gtk_ml_dumpf_debug(ctx, stream, err, expr->value.s_program.linkage_name);
            fprintf(stream, " 0x%"GTKML_FMT_64"x ", expr->value.s_program.addr);
            break;
        case GTKML_PROG_MACRO:
            fprintf(stream, "(program-macro ");
            gtk_ml_dumpf_debug(ctx, stream, err, expr->value.s_program.linkage_name);
            fprintf(stream, " 0x%"GTKML_FMT_64"x ", expr->value.s_program.addr);
            break;
        case GTKML_PROG_RUNTIME:
            fprintf(stream, "(program ");
            gtk_ml_dumpf_debug(ctx, stream, err, expr->value.s_program.linkage_name);
            fprintf(stream, " 0x%"GTKML_FMT_64"x ", expr->value.s_program.addr);
            break;
        }
        if (!gtk_ml_dumpf_debug(ctx, stream, err, expr->value.s_program.args)) {
            return 0;
        }
        fprintf(stream, " ");
        GtkMl_S body_value;
        gtk_ml_dbg_read_sobject(ctx, err, &body_value, expr->value.s_program.body);
        if (*err) {
            return 0;
        }
        GtkMl_SObj body = &body_value;
        while (body->kind != GTKML_S_NIL) {
            if (!gtk_ml_dumpf_debug(ctx, stream, err, gtk_ml_car(body))) {
                return 0;
            }
            gtk_ml_dbg_read_sobject(ctx, err, body, gtk_ml_cdr(body));
            if (*err) {
                return 0;
            }
            if (body->kind != GTKML_S_NIL) {
                fprintf(stream, " ");
            }
        }
        fprintf(stream, ")");
        return 1;
    }
    case GTKML_S_ADDRESS:
        fprintf(stream, "(address ");
        if (!gtk_ml_dumpf(ctx, stream, err, expr->value.s_address.linkage_name)) {
            return 0;
        }
        fprintf(stream, " 0x%"GTKML_FMT_64"x)", expr->value.s_address.addr);
        return 1;
    case GTKML_S_MACRO: {
        fprintf(stream, "(macro ");
        gtk_ml_dumpf_debug(ctx, stream, err, expr->value.s_macro.args);
        fprintf(stream, " ");
        GtkMl_S body_value;
        gtk_ml_dbg_read_sobject(ctx, err, &body_value, expr->value.s_macro.body);
        if (*err) {
            return 0;
        }
        GtkMl_SObj body = &body_value;
        while (body->kind != GTKML_S_NIL) {
            gtk_ml_dumpf_debug(ctx, stream, err, gtk_ml_car(body));
            gtk_ml_dbg_read_sobject(ctx, err, body, gtk_ml_cdr(body));
            if (*err) {
                return 0;
            }
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
        *err = gtk_ml_error(ctx, "invalid-sexpr", GTKML_ERR_INVALID_SEXPR, expr->span.ptr != NULL, expr->span.line, expr->span.col, 1, gtk_ml_new_keyword(ctx, NULL, 0, "kind", strlen("kind")), gtk_ml_new_int(ctx, NULL, expr->kind));
        return 0;
    }
}
#endif /* GTKML_ENABLE_POSIX */

gboolean gtk_ml_dumpf_value_internal(GtkMl_Context *ctx, FILE *stream, GtkMl_SObj *err, GtkMl_TaggedValue value, gboolean dump_ptr, gboolean dump_sobj) {
    if (gtk_ml_is_sobject(value)) {
        if (dump_ptr) {
            fprintf(stream, "(s-object %p", (void *) value.value.sobj);
        }
        if (dump_sobj) {
            if (dump_ptr) {
                fprintf(stream, " ");
            }
            if (!gtk_ml_dumpf(ctx, stream, err, value.value.sobj)) {
                return 0;
            }
        }
        if (dump_ptr) {
            fprintf(stream, ")");
        }
        return 1;
    } else {
        switch (value.tag) {
        case GTKML_TAG_NIL:
            fprintf(stream, "#nil");
            return 1;
        case GTKML_TAG_BOOL:
            fprintf(stream, "%s", value.value.boolean? "#t" : "#f");
            return 1;
        case GTKML_TAG_CHAR:
            if (isgraph(value.value.unicode)) {
                fprintf(stream, "\\%c", value.value.unicode);
            } else if (value.value.unicode < 0x100) {
                fprintf(stream, "\\x%02x", value.value.unicode);
            } else if (value.value.unicode < 0x10000) {
                fprintf(stream, "\\u%04x", value.value.unicode);
            } else {
                fprintf(stream, "\\U%08x", value.value.unicode);
            }
            return 1;
        case GTKML_TAG_INT64:
            fprintf(stream, "%"GTKML_FMT_64"d", value.value.s64);
            return 1;
        case GTKML_TAG_UINT64:
            fprintf(stream, "%"GTKML_FMT_64"u", value.value.u64);
            return 1;
        case GTKML_TAG_FLOAT:
            fprintf(stream, "%f", value.value.flt);
            return 1;
        case GTKML_TAG_USERDATA:
            fprintf(stream, "%p", value.value.userdata);
            return 1;
        default:
            *err = gtk_ml_error(ctx, "primitive-error", GTKML_ERR_TAG_ERROR, 0, 0, 0, 0);
            return 0;
        }
    }
}

gboolean gtk_ml_dumpf_value(GtkMl_Context *ctx, FILE *stream, GtkMl_SObj *err, GtkMl_TaggedValue value) {
    return gtk_ml_dumpf_value_internal(ctx, stream, err, value, 0, 1);
}

struct DumpfData {
    GtkMl_Context *ctx;
    FILE *stream;
    GtkMl_SObj *err;
    size_t n;
};

GTKML_PRIVATE GtkMl_VisitResult dumpf_hash_trie(GtkMl_HashTrie *ht, GtkMl_TaggedValue key, GtkMl_TaggedValue value, GtkMl_TaggedValue _data) {
    struct DumpfData *data = _data.value.userdata;

    if (!gtk_ml_dumpf_value(data->ctx, data->stream, data->err, key)) {
        return 0;
    }
    fprintf(data->stream, " ");
    if (!gtk_ml_dumpf_value(data->ctx, data->stream, data->err, value)) {
        return 0;
    }
    ++data->n;
    if (data->n < gtk_ml_hash_trie_len(ht)) {
        fprintf(data->stream, " ");
    }

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult dumpf_hash_set(GtkMl_HashSet *hs, GtkMl_TaggedValue key, GtkMl_TaggedValue _data) {
    struct DumpfData *data = _data.value.userdata;

    if (!gtk_ml_dumpf_value(data->ctx, data->stream, data->err, key)) {
        return 0;
    }
    ++data->n;
    if (data->n < gtk_ml_hash_set_len(hs)) {
        fprintf(data->stream, " ");
    }

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult dumpf_array(GtkMl_Array *array, size_t idx, GtkMl_TaggedValue key, GtkMl_TaggedValue _data) {
    (void) idx;

    struct DumpfData *data = _data.value.userdata;

    if (!gtk_ml_dumpf_value(data->ctx, data->stream, data->err, key)) {
        return 0;
    }
    ++data->n;
    if (data->n < gtk_ml_array_trie_len(array)) {
        fprintf(data->stream, " ");
    }

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult dumpf_string(GtkMl_Array *array, size_t idx, GtkMl_TaggedValue key, GtkMl_TaggedValue data) {
    (void) array;
    (void) idx;
    (void) data;

    if (isprint(key.value.unicode)) {
        fprintf(data.value.userdata, "%c", key.value.unicode);
    } else if (key.value.unicode < 0x100) {
        fprintf(data.value.userdata, "\\x%02x", key.value.unicode);
    } else if (key.value.unicode < 0x10000) {
        fprintf(data.value.userdata, "\\u%04x", key.value.unicode);
    } else {
        fprintf(data.value.userdata, "\\U%08x", key.value.unicode);
    }

    return GTKML_VISIT_RECURSE;
}

gboolean gtk_ml_dumpf(GtkMl_Context *ctx, FILE *stream, GtkMl_SObj *err, GtkMl_SObj expr) {
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
        fprintf(stream, "%"GTKML_FMT_64"d", expr->value.s_int.value);
        return 1;
    case GTKML_S_FLOAT:
        fprintf(stream, "%f", expr->value.s_float.value);
        return 1;
    case GTKML_S_CHAR:
        if (isgraph(expr->value.s_char.value)) {
            fprintf(stream, "\\%c", expr->value.s_char.value);
        } else if (expr->value.s_char.value < 0x100) {
            fprintf(stream, "\\x%02x", expr->value.s_char.value);
        } else if (expr->value.s_char.value < 0x10000) {
            fprintf(stream, "\\u%04x", expr->value.s_char.value);
        } else {
            fprintf(stream, "\\U%08x", expr->value.s_char.value);
        }
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
            if (!gtk_ml_dumpf(ctx, stream, err, gtk_ml_car(expr))) {
                return 0;
            }
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
        gtk_ml_hash_trie_foreach(&expr->value.s_map.map, dumpf_hash_trie, gtk_ml_value_userdata(&data));
        fprintf(stream, "}");
        return 1;
    }
    case GTKML_S_SET: {
        fprintf(stream, "#{");
        struct DumpfData data = { ctx, stream, err, 0 };
        gtk_ml_hash_set_foreach(&expr->value.s_set.set, dumpf_hash_set, gtk_ml_value_userdata(&data));
        fprintf(stream, "}");
        return 1;
    }
    case GTKML_S_ARRAY: {
        if (gtk_ml_array_trie_is_string(&expr->value.s_array.array)) {
            fprintf(stream, "\"");
            gtk_ml_array_trie_foreach(&expr->value.s_array.array, dumpf_string, gtk_ml_value_userdata(stream));
            fprintf(stream, "\"");
            return 1;
        } else {
            fprintf(stream, "[");
            struct DumpfData data = { ctx, stream, err, 0 };
            gtk_ml_array_trie_foreach(&expr->value.s_array.array, dumpf_array, gtk_ml_value_userdata(&data));
            fprintf(stream, "]");
            return 1;
        }
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
        GtkMl_SObj body = expr->value.s_lambda.body;
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
        switch (expr->value.s_program.kind) {
        case GTKML_PROG_INTRINSIC:
            fprintf(stream, "(program-intrinsic ");
            if (!gtk_ml_dumpf(ctx, stream, err, expr->value.s_program.linkage_name)) {
                return 0;
            }
            fprintf(stream, " 0x%"GTKML_FMT_64"x ", expr->value.s_program.addr);
            break;
        case GTKML_PROG_MACRO:
            fprintf(stream, "(program-macro ");
            if (!gtk_ml_dumpf(ctx, stream, err, expr->value.s_program.linkage_name)) {
                return 0;
            }
            fprintf(stream, " 0x%"GTKML_FMT_64"x ", expr->value.s_program.addr);
            break;
        case GTKML_PROG_RUNTIME:
            fprintf(stream, "(program ");
            if (!gtk_ml_dumpf(ctx, stream, err, expr->value.s_program.linkage_name)) {
                return 0;
            }
            fprintf(stream, " 0x%"GTKML_FMT_64"x ", expr->value.s_program.addr);
            break;
        }
        if (!gtk_ml_dumpf(ctx, stream, err, expr->value.s_program.args)) {
            return 0;
        }
        fprintf(stream, " ");
        GtkMl_SObj body = expr->value.s_program.body;
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
        fprintf(stream, "(address ");
        if (!gtk_ml_dumpf(ctx, stream, err, expr->value.s_address.linkage_name)) {
            return 0;
        }
        fprintf(stream, " 0x%"GTKML_FMT_64"x)", expr->value.s_address.addr);
        return 1;
    case GTKML_S_MACRO: {
        fprintf(stream, "(macro ");
        if (!gtk_ml_dumpf(ctx, stream, err, expr->value.s_macro.args)) {
            return 0;
        }
        fprintf(stream, " ");
        GtkMl_SObj body = expr->value.s_macro.body;
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
    case GTKML_S_LIGHTDATA:
        fprintf(stream, "%p", expr->value.s_lightdata.userdata);
        return 1;
    case GTKML_S_USERDATA:
        fprintf(stream, "%p", expr->value.s_lightdata.userdata);
        return 1;
    default:
        *err = gtk_ml_error(ctx, "invalid-sexpr", GTKML_ERR_INVALID_SEXPR, expr->span.ptr != NULL, expr->span.line, expr->span.col, 1, gtk_ml_new_keyword(ctx, NULL, 0, "kind", strlen("kind")), gtk_ml_new_int(ctx, NULL, expr->kind));
        return 0;
    }
}

char *gtk_ml_dumpsn(GtkMl_Context *ctx, char *ptr, size_t n, GtkMl_SObj *err, GtkMl_SObj expr) {
    (void) ctx;
    (void) ptr;
    (void) n;
    (void) err;
    (void) expr;
    return NULL;
}

#define rprintf(buffer, fmt, ...) \
    do { \
        size_t cap = 1024; \
        char *_buf; \
        while (1) { \
            _buf = malloc(cap); \
            size_t n = snprintf(_buf, cap, fmt, __VA_ARGS__); \
            if (n == cap - 1) { \
                cap *= 2; \
                free(_buf); \
            } \
            break; \
        } \
        buffer = _buf; \
    } while (0)

#define rprint(buffer, fmt) \
    do { \
        size_t cap = 1024; \
        char *_buf; \
        while (1) { \
            _buf = malloc(cap); \
            size_t n = snprintf(_buf, cap, fmt); \
            if (n == cap - 1) { \
                cap *= 2; \
                free(_buf); \
            } \
            break; \
        } \
        buffer = _buf; \
    } while (0)

#define snrprintf_at(buffer, at, size, fmt, ...) \
    do { \
        char *buf; \
        rprintf(buf, fmt, __VA_ARGS__);\
        if (strlen(buf) >= size - at) { \
            size += strlen(buf) + 1; \
            buffer = realloc(buffer, size); \
        } \
        strcpy(buffer + at, buf); \
        at += strlen(buf); \
        free(buf); \
    } while (0)

#define snrprint_at(buffer, at, size, fmt) \
    do { \
        char *buf; \
        rprint(buf, fmt);\
        if (strlen(buf) >= size - at) { \
            size += strlen(buf) + 1; \
            buffer = realloc(buffer, size); \
        } \
        strcpy(buffer + at, buf); \
        at += strlen(buf); \
        free(buf); \
    } while (0)

struct DumpsnrData {
    GtkMl_Context *ctx;
    char *buffer;
    size_t *offset;
    size_t size;
    GtkMl_SObj *err;
    size_t n;
};

GTKML_PRIVATE GtkMl_VisitResult dumpsnr_hash_trie(GtkMl_HashTrie *ht, GtkMl_TaggedValue key, GtkMl_TaggedValue value, GtkMl_TaggedValue _data) {
    struct DumpsnrData *data = _data.value.userdata;

    if (!gtk_ml_dumpsnr_internal(data->ctx, data->buffer, data->offset, data->size, data->err, key.value.sobj)) {
        return 0;
    }
    snrprint_at(data->buffer, *data->offset, data->size, " ");
    if (!gtk_ml_dumpsnr_internal(data->ctx, data->buffer, data->offset, data->size, data->err, value.value.sobj)) {
        return 0;
    }
    ++data->n;
    if (data->n < gtk_ml_hash_trie_len(ht)) {
        snrprint_at(data->buffer, *data->offset, data->size, " ");
    }

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult dumpsnr_hash_set(GtkMl_HashSet *hs, GtkMl_TaggedValue key, GtkMl_TaggedValue _data) {
    struct DumpsnrData *data = _data.value.userdata;

    if (!gtk_ml_dumpsnr_internal(data->ctx, data->buffer, data->offset, data->size, data->err, key.value.sobj)) {
        return 0;
    }
    ++data->n;
    if (data->n < gtk_ml_hash_set_len(hs)) {
        snrprint_at(data->buffer, *data->offset, data->size, " ");
    }

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult dumpsnr_array(GtkMl_Array *array, size_t idx, GtkMl_TaggedValue key, GtkMl_TaggedValue _data) {
    (void) idx;

    struct DumpsnrData *data = _data.value.userdata;

    if (!gtk_ml_dumpsnr_internal(data->ctx, data->buffer, data->offset, data->size, data->err, key.value.sobj)) {
        return 0;
    }
    ++data->n;
    if (data->n < gtk_ml_array_trie_len(array)) {
        snrprint_at(data->buffer, *data->offset, data->size, " ");
    }

    return GTKML_VISIT_RECURSE;
}

GTKML_PRIVATE GtkMl_VisitResult dumpsnr_string(GtkMl_Array *array, size_t idx, GtkMl_TaggedValue key, GtkMl_TaggedValue _data) {
    (void) array;
    (void) idx;

    struct DumpsnrData *data = _data.value.userdata;
    if (isprint(key.value.unicode)) {
        snrprintf_at(data->buffer, *data->offset, data->size, "\\%c", key.value.unicode);
    } else if (key.value.unicode < 0x100) {
        snrprintf_at(data->buffer, *data->offset, data->size, "\\x%02x", key.value.unicode);
    } else if (key.value.unicode < 0x10000) {
        snrprintf_at(data->buffer, *data->offset, data->size, "\\u%04x", key.value.unicode);
    } else {
        snrprintf_at(data->buffer, *data->offset, data->size, "\\U%08x", key.value.unicode);
    }

    return GTKML_VISIT_RECURSE;
}

char *gtk_ml_dumpsnr_internal(GtkMl_Context *ctx, char *buffer, size_t *offset, size_t size, GtkMl_SObj *err, GtkMl_SObj expr) {
    switch (expr->kind) {
    case GTKML_S_NIL:
        snrprint_at(buffer, *offset, size, "#nil");
        return buffer;
    case GTKML_S_TRUE:
        snrprint_at(buffer, *offset, size, "#t");
        return buffer;
    case GTKML_S_FALSE:
        snrprint_at(buffer, *offset, size, "#f");
        return buffer;
    case GTKML_S_INT:
        snrprintf_at(buffer, *offset, size, "%"GTKML_FMT_64"d", expr->value.s_int.value);
        return buffer;
    case GTKML_S_FLOAT:
        snrprintf_at(buffer, *offset, size, "%f", expr->value.s_float.value);
        return buffer;
    case GTKML_S_CHAR:
        if (isgraph(expr->value.s_char.value)) {
            snrprintf_at(buffer, *offset, size, "\\%c", expr->value.s_char.value);
        } else if (expr->value.s_char.value < 0x100) {
            snrprintf_at(buffer, *offset, size, "\\x%02x", expr->value.s_char.value);
        } else if (expr->value.s_char.value < 0x10000) {
            snrprintf_at(buffer, *offset, size, "\\u%04x", expr->value.s_char.value);
        } else {
            snrprintf_at(buffer, *offset, size, "\\U%08x", expr->value.s_char.value);
        }
        return buffer;
    case GTKML_S_KEYWORD:
        snrprintf_at(buffer, *offset, size, ":%.*s", (int) expr->value.s_keyword.len, expr->value.s_keyword.ptr);
        return buffer;
    case GTKML_S_SYMBOL:
        snrprintf_at(buffer, *offset, size, "'%.*s", (int) expr->value.s_symbol.len, expr->value.s_symbol.ptr);
        return buffer;
    case GTKML_S_LIST:
        snrprint_at(buffer, *offset, size, "(");
        while (expr->kind != GTKML_S_NIL) {
            if (!gtk_ml_dumpsnr_internal(ctx, buffer, offset, size, err, gtk_ml_car(expr))) {
                return 0;
            }
            expr = gtk_ml_cdr(expr);
            if (expr->kind != GTKML_S_NIL) {
                snrprint_at(buffer, *offset, size, " ");
            }
        }
        snrprint_at(buffer, *offset, size, ")");
        return buffer;
    case GTKML_S_MAP: {
        snrprint_at(buffer, *offset, size, "{");
        struct DumpsnrData data = { ctx, buffer, offset, size, err, 0 };
        gtk_ml_hash_trie_foreach(&expr->value.s_map.map, dumpsnr_hash_trie, gtk_ml_value_userdata(&data));
        snrprint_at(buffer, *offset, size, "}");
        return buffer;
    }
    case GTKML_S_SET: {
        snrprint_at(buffer, *offset, size, "#{");
        struct DumpsnrData data = { ctx, buffer, offset, size, err, 0 };
        gtk_ml_hash_set_foreach(&expr->value.s_set.set, dumpsnr_hash_set, gtk_ml_value_userdata(&data));
        snrprint_at(buffer, *offset, size, "}");
        return buffer;
    }
    case GTKML_S_ARRAY: {
        if (gtk_ml_array_trie_is_string(&expr->value.s_array.array)) {
            snrprint_at(buffer, *offset, size, "\"");
            struct DumpsnrData data = { ctx, buffer, offset, size, err, 0 };
            gtk_ml_array_trie_foreach(&expr->value.s_array.array, dumpsnr_string, gtk_ml_value_userdata(&data));
            snrprint_at(buffer, *offset, size, "\"");
            return buffer;
        } else {
            snrprint_at(buffer, *offset, size, "[");
            struct DumpsnrData data = { ctx, buffer, offset, size, err, 0 };
            gtk_ml_array_trie_foreach(&expr->value.s_array.array, dumpsnr_array, gtk_ml_value_userdata(&data));
            snrprint_at(buffer, *offset, size, "]");
            return buffer;
        }
    }
    case GTKML_S_VAR:
        snrprint_at(buffer, *offset, size, "(var ");
        if (!gtk_ml_dumpsnr_internal(ctx, buffer, offset, size, err, expr->value.s_vararg.expr)) {
            return NULL;
        }
        snrprint_at(buffer, *offset, size, ")");
        return buffer;
    case GTKML_S_VARARG:
        snrprint_at(buffer, *offset, size, "...");
        return gtk_ml_dumpsnr_internal(ctx, buffer, offset, size, err, expr->value.s_vararg.expr);
    case GTKML_S_QUOTE:
        snrprint_at(buffer, *offset, size, "'");
        return gtk_ml_dumpsnr_internal(ctx, buffer, offset, size, err, expr->value.s_vararg.expr);
    case GTKML_S_QUASIQUOTE:
        snrprint_at(buffer, *offset, size, "`");
        return gtk_ml_dumpsnr_internal(ctx, buffer, offset, size, err, expr->value.s_vararg.expr);
    case GTKML_S_UNQUOTE:
        snrprint_at(buffer, *offset, size, ",");
        return gtk_ml_dumpsnr_internal(ctx, buffer, offset, size, err, expr->value.s_vararg.expr);
    case GTKML_S_LAMBDA: {
        snrprint_at(buffer, *offset, size, "(lambda ");
        if (!gtk_ml_dumpsnr_internal(ctx, buffer, offset, size, err, expr->value.s_lambda.args)) {
            return NULL;
        }
        snrprint_at(buffer, *offset, size, " ");
        GtkMl_SObj body = expr->value.s_lambda.body;
        while (body->kind != GTKML_S_NIL) {
            if (!gtk_ml_dumpsnr_internal(ctx, buffer, offset, size, err, gtk_ml_car(body))) {
                return NULL;
            }
            body = gtk_ml_cdr(body);
            if (body->kind != GTKML_S_NIL) {
                snrprint_at(buffer, *offset, size, " ");
            }
        }
        snrprint_at(buffer, *offset, size, ")");
        return buffer;
    }
    case GTKML_S_PROGRAM:
        switch (expr->value.s_program.kind) {
        case GTKML_PROG_INTRINSIC:
            snrprint_at(buffer, *offset, size, "(program-intrinsic ");
            if (!gtk_ml_dumpsnr_internal(ctx, buffer, offset, size, err, expr->value.s_program.linkage_name)) {
                return 0;
            }
            snrprintf_at(buffer, *offset, size, " 0x%"GTKML_FMT_64"x ", expr->value.s_program.addr);
            break;
        case GTKML_PROG_MACRO:
            snrprint_at(buffer, *offset, size, "(program-macro ");
            if (!gtk_ml_dumpsnr_internal(ctx, buffer, offset, size, err, expr->value.s_program.linkage_name)) {
                return 0;
            }
            snrprintf_at(buffer, *offset, size, " 0x%"GTKML_FMT_64"x ", expr->value.s_program.addr);
            break;
        case GTKML_PROG_RUNTIME:
            snrprint_at(buffer, *offset, size, "(program ");
            if (!gtk_ml_dumpsnr_internal(ctx, buffer, offset, size, err, expr->value.s_program.linkage_name)) {
                return 0;
            }
            snrprintf_at(buffer, *offset, size, " 0x%"GTKML_FMT_64"x ", expr->value.s_program.addr);
            break;
        }
        if (!gtk_ml_dumpsnr_internal(ctx, buffer, offset, size, err, expr->value.s_program.args)) {
            return NULL;
        }
        snrprint_at(buffer, *offset, size, " ");
        GtkMl_SObj body = expr->value.s_program.body;
        while (body->kind != GTKML_S_NIL) {
            if (!gtk_ml_dumpsnr_internal(ctx, buffer, offset, size, err, gtk_ml_car(body))) {
                return NULL;
            }
            body = gtk_ml_cdr(body);
            if (body->kind != GTKML_S_NIL) {
                snrprint_at(buffer, *offset, size, " ");
            }
        }
        snrprint_at(buffer, *offset, size, ")");
        return buffer;
    case GTKML_S_ADDRESS:
        snrprintf_at(buffer, *offset, size, "(address 0x%"GTKML_FMT_64"x)", expr->value.s_address.addr);
        return buffer;
    case GTKML_S_MACRO: {
        snrprint_at(buffer, *offset, size, "(macro ");
        if (!gtk_ml_dumpsnr_internal(ctx, buffer, offset, size, err, expr->value.s_macro.args)) {
            return 0;
        }
        snrprint_at(buffer, *offset, size, " ");
        GtkMl_SObj body = expr->value.s_macro.body;
        while (body->kind != GTKML_S_NIL) {
            if (!gtk_ml_dumpsnr_internal(ctx, buffer, offset, size, err, gtk_ml_car(body))) {
                return 0;
            }
            body = gtk_ml_cdr(body);
            if (body->kind != GTKML_S_NIL) {
                snrprint_at(buffer, *offset, size, " ");
            }
        }
        snrprint_at(buffer, *offset, size, ")");
        return buffer;
    }
    case GTKML_S_LIGHTDATA:
        snrprintf_at(buffer, *offset, size, "%p", expr->value.s_lightdata.userdata);
        return buffer;
    case GTKML_S_USERDATA:
        snrprintf_at(buffer, *offset, size, "%p", expr->value.s_lightdata.userdata);
        return buffer;
    default:
        *err = gtk_ml_error(ctx, "invalid-sexpr", GTKML_ERR_INVALID_SEXPR, expr->span.ptr != NULL, expr->span.line, expr->span.col, 1, gtk_ml_new_keyword(ctx, NULL, 0, "kind", strlen("kind")), gtk_ml_new_int(ctx, NULL, expr->kind));
        return NULL;
    }
}

char *gtk_ml_dumpsnr(GtkMl_Context *ctx, char *buffer, size_t size, GtkMl_SObj *err, GtkMl_SObj expr) {
    size_t offset = 0;
    return gtk_ml_dumpsnr_internal(ctx, buffer, &offset, size, err, expr);
}

char *gtk_ml_dumpsnr_value_internal(GtkMl_Context *ctx, char *buffer, size_t *offset, size_t size, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    if (gtk_ml_is_sobject(expr)) {
        return gtk_ml_dumpsnr_internal(ctx, buffer, offset, size, err, expr.value.sobj);
    } else {
        switch (expr.tag) {
        case GTKML_TAG_NIL:
            snrprint_at(buffer, *offset, size, "#nil");
            return buffer;
        case GTKML_TAG_BOOL:
            if (expr.value.boolean) {
                snrprint_at(buffer, *offset, size, "#t");
            } else {
                snrprint_at(buffer, *offset, size, "#f");
            }
            return buffer;
        case GTKML_TAG_INT64:
            snrprintf_at(buffer, *offset, size, "%"GTKML_FMT_64"d", expr.value.s64);
            return buffer;
        case GTKML_TAG_UINT64:
            snrprintf_at(buffer, *offset, size, "%"GTKML_FMT_64"d", expr.value.u64);
            return buffer;
        case GTKML_TAG_FLOAT:
            snrprintf_at(buffer, *offset, size, "%f", expr.value.flt);
            return buffer;
        case GTKML_TAG_CHAR:
            if (isgraph(expr.value.unicode)) {
                snrprintf_at(buffer, *offset, size, "\\%c", expr.value.unicode);
            } else if (expr.value.unicode < 0x100) {
                snrprintf_at(buffer, *offset, size, "\\x%02x", expr.value.unicode);
            } else if (expr.value.unicode < 0x10000) {
                snrprintf_at(buffer, *offset, size, "\\u%04x", expr.value.unicode);
            } else {
                snrprintf_at(buffer, *offset, size, "\\U%08x", expr.value.unicode);
            }
            return buffer;
        case GTKML_TAG_USERDATA:
            snrprintf_at(buffer, *offset, size, "%p", expr.value.userdata);
            return buffer;
        default:
            *err = gtk_ml_error(ctx, "invalid-sexpr", GTKML_ERR_INVALID_SEXPR, 0, 0, 0, 0);
            return NULL;
        }
    }
}

char *gtk_ml_dumpsnr_value(GtkMl_Context *ctx, char *buffer, size_t size, GtkMl_SObj *err, GtkMl_TaggedValue expr) {
    size_t offset = 0;
    return gtk_ml_dumpsnr_value_internal(ctx, buffer, &offset, size, err, expr);
}

gboolean gtk_ml_dumpf_program_internal(GtkMl_Context *ctx, FILE *stream, GtkMl_SObj *err, gboolean line, gboolean debug) {
    if (!ctx->vm->program || !ctx->vm->program->text) {
        return 0;
    }

    size_t start;
    size_t end;
    if (line) {
        start = ctx->vm->pc >> 3;
        end = (ctx->vm->pc >> 3) + 1;
    } else {
        start = 0;
        end = ctx->vm->program->n_text;
        fprintf(stream, "section TEXT\n\n");
    }
    for (size_t pc = start; pc < end; pc++) {
        GtkMl_Instruction instr = ctx->vm->program->text[pc];
        if (debug) {
            char current_line = (pc == ctx->vm->pc)? '*' : ' ';
            fprintf(stream, "%c %zx ", current_line, 8 * pc);
        } else {
            fprintf(stream, "%zx ", 8 * pc);
        }
        if (instr.category == GTKML_I_GENERIC) {
            if (S_OPCODES[instr.opcode]) {
                if (instr.data) {
                    fprintf(stream, "%s %"GTKML_FMT_64"x\n", S_OPCODES[instr.opcode], instr.data);
                } else {
                    fprintf(stream, "%s\n", S_OPCODES[instr.opcode]);
                }
            } else {
                if (instr.data) {
                    fprintf(stream, "invalid generic %x %"GTKML_FMT_64"x\n", instr.opcode, instr.data);
                } else {
                    fprintf(stream, "invalid generic %x\n", instr.opcode);
                }
            }
        } else if (instr.category == GTKML_I_EXTERN) {
            if (S_OPCODES[instr.opcode]) {
                if (instr.data) {
                    fprintf(stream, "extern %s %"GTKML_FMT_64"x\n", S_OPCODES[instr.opcode], instr.data);
                } else {
                    fprintf(stream, "extern %s\n", S_OPCODES[instr.opcode]);
                }
            } else {
                if (instr.data) {
                    fprintf(stream, "invalid extern generic %x %"GTKML_FMT_64"x\n", instr.opcode, instr.data);
                } else {
                    fprintf(stream, "invalid extern generic %x\n", instr.opcode);
                }
            }
        } else if (instr.category == GTKML_I_EXTERN_LOCAL) {
            if (S_OPCODES[instr.opcode]) {
                if (instr.data) {
                    fprintf(stream, "extern-local %s %"GTKML_FMT_64"x\n", S_OPCODES[instr.opcode], instr.data);
                } else {
                    fprintf(stream, "extern-local %s\n", S_OPCODES[instr.opcode]);
                }
            } else {
                if (instr.data) {
                    fprintf(stream, "invalid extern-local generic %x %"GTKML_FMT_64"x\n", instr.opcode, instr.data);
                } else {
                    fprintf(stream, "invalid extern-local generic %x\n", instr.opcode);
                }
            }
        } else if (instr.category == GTKML_I_EXPORT) {
            fprintf(stream, "export %"GTKML_FMT_64"x\n", instr.data);
        } else if (instr.category == GTKML_I_EXPORT_LOCAL) {
            fprintf(stream, "export-local %"GTKML_FMT_64"x\n", instr.data);
        } else {
            fprintf(stream, "invalid %x %x %"GTKML_FMT_64"x\n", instr.category, instr.opcode, instr.data);
        }
    }

    if (!line) {
        fprintf(stream, "\n");
        fprintf(stream, "section DATA\n\n");
        for (size_t i = 1; i < ctx->vm->program->n_data; i++) {
            GtkMl_TaggedValue v = ctx->vm->program->data[i];
            fprintf(stream, "%zx ", i);
            if (!gtk_ml_dumpf_value_internal(ctx, stream, err, v, 1, 0)) {
                return 0;
            }
            fprintf(stream, "\n");
        }

        fprintf(stream, "\n");
        fprintf(stream, "section STATIC\n\n");
        for (size_t i = 1; i < ctx->vm->program->n_static; i++) {
            GtkMl_SObj s = ctx->vm->program->statics[i];
            fprintf(stream, "%zx ", i);
            if (!gtk_ml_dumpf(ctx, stream, err, s)) {
                return 0;
            }
            fprintf(stream, "\n");
        }
    }

    return 1;
}

gboolean gtk_ml_dumpf_stack(GtkMl_Context *ctx, FILE *stream, GtkMl_SObj *err) {
    size_t start = 0;
    size_t end = ctx->gc->stack_len;

    fprintf(stream, "gc stack\n\n");
    for (size_t i = start; i < end; i++) {
        GtkMl_SObj s = ctx->gc->stack[i];
        fprintf(stream, "%zu ", i);
        if (!gtk_ml_dumpf(ctx, stream, err, s)) {
            return 0;
        }
        fprintf(stream, "\n");
    }

    start = 0;
    end = ctx->vm->stack_len;
    fprintf(stream, "\n\nvm stack\n\n");
    for (size_t i = start; i < end; i++) {
        GtkMl_TaggedValue s = ctx->vm->stack[i];
        fprintf(stream, "%zu %"GTKML_FMT_64"x\n", i, s.value.u64);
    }

    return 1;
}

#ifdef GTKML_ENABLE_POSIX
GTKML_PRIVATE gboolean gtk_ml_dumpf_program_debug_internal(GtkMl_Context *ctx, FILE *stream, GtkMl_SObj *err, gboolean line, gboolean debug) {
    GtkMl_Vm *vm = gtk_ml_dbg_read_ptr(ctx, err, &ctx->dbg_ctx->vm);
    if (*err) {
        return 0;
    }
    GtkMl_Program *program = gtk_ml_dbg_read_ptr(ctx, err, &vm->program);
    GtkMl_Instruction *text = gtk_ml_dbg_read_ptr(ctx, err, &program->text);
    if (*err) {
        return 0;
    }
    GtkMl_SObj *stat = gtk_ml_dbg_read_ptr(ctx, err, &program->statics);
    if (!text) {
        return 0;
    }

    uint64_t dbg_pc = gtk_ml_dbg_read_u64(ctx, err, &vm->pc);
    if (*err) {
        return 0;
    }
    size_t start;
    size_t end;
    if (line) {
        start = dbg_pc >> 3;
        end = (dbg_pc >> 3) + 1;
    } else {
        start = 0;
        end = gtk_ml_dbg_read_u64(ctx, err, &program->n_text);
        if (*err) {
            return 0;
        }
        fprintf(stream, "section TEXT\n\n");
    }
    for (size_t pc = start; pc < end;) {
        uint64_t word = gtk_ml_dbg_read_u64(ctx, err, &text[pc]);
        if (*err) {
            return 0;
        }
        GtkMl_Instruction instr = *(GtkMl_Instruction *) &word;
        if (debug) {
            char current_line = (pc == ctx->vm->pc)? '*' : ' ';
            fprintf(stream, "%c %zx ", current_line, 8 * pc);
        } else {
            fprintf(stream, "%zx ", 8 * pc);
        }
        if (instr.category == GTKML_I_GENERIC) {
            if (S_OPCODES[instr.opcode]) {
                if (instr.data) {
                    fprintf(stream, "%s %"GTKML_FMT_64"x\n", S_OPCODES[instr.opcode], instr.data);
                } else {
                    fprintf(stream, "%s\n", S_OPCODES[instr.opcode]);
                }
            } else {
                if (instr.data) {
                    fprintf(stream, "invalid generic %x %"GTKML_FMT_64"x\n", instr.opcode, instr.data);
                } else {
                    fprintf(stream, "invalid generic %x\n", instr.opcode);
                }
            }
        } else if (instr.category == GTKML_I_EXTERN) {
            if (S_OPCODES[instr.opcode]) {
                if (instr.data) {
                    fprintf(stream, "extern %s %"GTKML_FMT_64"x\n", S_OPCODES[instr.opcode], instr.data);
                } else {
                    fprintf(stream, "extern %s\n", S_OPCODES[instr.opcode]);
                }
            } else {
                if (instr.data) {
                    fprintf(stream, "invalid extern generic %x %"GTKML_FMT_64"x\n", instr.opcode, instr.data);
                } else {
                    fprintf(stream, "invalid extern generic %x\n", instr.opcode);
                }
            }
        } else if (instr.category == GTKML_I_EXTERN_LOCAL) {
            if (S_OPCODES[instr.opcode]) {
                if (instr.data) {
                    fprintf(stream, "extern-local %s %"GTKML_FMT_64"x\n", S_OPCODES[instr.opcode], instr.data);
                } else {
                    fprintf(stream, "extern-local %s\n", S_OPCODES[instr.opcode]);
                }
            } else {
                if (instr.data) {
                    fprintf(stream, "invalid extern-local generic %x %"GTKML_FMT_64"x\n", instr.opcode, instr.data);
                } else {
                    fprintf(stream, "invalid extern-local generic %x\n", instr.opcode);
                }
            }
        } else if (instr.category == GTKML_I_EXPORT) {
            fprintf(stream, "export %"GTKML_FMT_64"x\n", instr.data);
        } else if (instr.category == GTKML_I_EXPORT_LOCAL) {
            fprintf(stream, "export-local %"GTKML_FMT_64"x\n", instr.data);
        } else {
            fprintf(stream, "invalid %x %x %"GTKML_FMT_64"x\n", instr.category, instr.opcode, instr.data);
        }
    }

    if (!line) {
        size_t start = 1;
        size_t end;
        end = gtk_ml_dbg_read_u64(ctx, err, &program->n_static);
        if (*err) {
            return 0;
        }

        fprintf(stream, "\n");
        fprintf(stream, "section STATIC\n\n");
        for (size_t i = start; i < end; i++) {
            GtkMl_SObj s = gtk_ml_dbg_read_ptr(ctx, err, &stat[i]);
            if (*err) {
                return 0;
            }
            fprintf(stream, "%zu ", i);
            if (!gtk_ml_dumpf_debug(ctx, stream, err, s)) {
                return 0;
            }
            fprintf(stream, "\n");
        }
    }

    return 1;
}

gboolean gtk_ml_dumpf_program_debug(GtkMl_Context *ctx, FILE *stream, GtkMl_SObj *err) {
    return gtk_ml_dumpf_program_debug_internal(ctx, stream, err, 0, 1);
}

gboolean gtk_ml_dumpf_line_debug(GtkMl_Context *ctx, FILE *stream, GtkMl_SObj *err) {
    return gtk_ml_dumpf_program_debug_internal(ctx, stream, err, 1, 1);
}

gboolean gtk_ml_dumpf_stack_debug(GtkMl_Context *ctx, FILE *stream, GtkMl_SObj *err) {
    {
        GtkMl_Gc *gc = gtk_ml_dbg_read_ptr(ctx, err, &ctx->gc);
        if (*err) {
            return 0;
        }
        GtkMl_SObj *stack = gtk_ml_dbg_read_ptr(ctx, err, &gc->stack);
        if (*err) {
            return 0;
        }
        size_t start = 0;
        size_t end = gtk_ml_dbg_read_u64(ctx, err, &gc->stack_len);
        if (*err) {
            return 0;
        }

        fprintf(stream, "gc stack\n\n");
        for (size_t i = start; i < end; i++) {
            GtkMl_SObj s = gtk_ml_dbg_read_ptr(ctx, err, &stack[i]);
            if (*err) {
                return 0;
            }
            fprintf(stream, "%zu ", i);
            if (!gtk_ml_dumpf_debug(ctx, stream, err, s)) {
                return 0;
            }
            fprintf(stream, "\n");
        }
    }

    {
        GtkMl_Vm *vm = gtk_ml_dbg_read_ptr(ctx, err, &ctx->vm);
        if (*err) {
            return 0;
        }
        GtkMl_TaggedValue *stack = gtk_ml_dbg_read_ptr(ctx, err, &vm->stack);
        if (*err) {
            return 0;
        }
        size_t start = 0;
        size_t end = gtk_ml_dbg_read_u64(ctx, err, &vm->stack_len);
        fprintf(stream, "\n\nvm stack\n\n");
        for (size_t i = start; i < end; i++) {
            GtkMl_TaggedValue s = gtk_ml_dbg_read_value(ctx, err, &stack[i]);
            if (*err) {
                return 0;
            }
            fprintf(stream, "%zu %"GTKML_FMT_64"x\n", i, s.value.u64);
        }
    }

    return 1;
}

gboolean gtk_ml_backtracef_debug(GtkMl_Context *ctx, FILE *stream, GtkMl_SObj *err) {
    GtkMl_Vm *vm = gtk_ml_dbg_read_ptr(ctx, err, &ctx->dbg_ctx->vm);
    if (*err) {
        return 0;
    }
    GtkMl_Program *program = gtk_ml_dbg_read_ptr(ctx, err, &vm->program);
    GtkMl_Instruction *text = gtk_ml_dbg_read_ptr(ctx, err, &program->text);
    if (*err) {
        return 0;
    }
    GtkMl_TaggedValue *data = gtk_ml_dbg_read_ptr(ctx, err, &program->data);
    if (*err) {
        return 0;
    }
    GtkMl_SObj *statics = gtk_ml_dbg_read_ptr(ctx, err, &program->statics);
    if (*err) {
        return 0;
    }
    fprintf(stream, "callstack backtrace\n\n");
    gboolean first = 1;
    fprintf(stream, "[");
    if (text) {
        size_t call_stack_ptr = gtk_ml_dbg_read_u64(ctx, err, &vm->call_stack_ptr);
        if (*err) {
            return 0;
        }
        uint64_t *call_stack = gtk_ml_dbg_read_ptr(ctx, err, &vm->call_stack);
        if (*err) {
            return 0;
        }
        for (size_t i = 0; i < call_stack_ptr; i += 2) {
            size_t call_at = gtk_ml_dbg_read_u64(ctx, err, &call_stack[i + 1]);
            if (*err) {
                return 0;
            }
            GtkMl_SObj export = NULL;
            for (size_t ptr = 0; ptr <= call_at; ptr += 8) {
                uint64_t word = gtk_ml_dbg_read_u64(ctx, err, &text[ptr >> 3]);
                if (*err) {
                    return 0;
                }
                GtkMl_Instruction instr = *(GtkMl_Instruction *) &word;
                if (instr.category == GTKML_I_EXPORT) {
                    GtkMl_SObj _export = gtk_ml_dbg_read_ptr(ctx, err, &statics[data[instr.data].value.u64]);
                    if (*err) {
                        return 0;
                    }
                    GtkMl_SKind kind = (GtkMl_SKind) gtk_ml_dbg_read_u32(ctx, err, &_export->kind);
                    if (kind == GTKML_S_PROGRAM || kind == GTKML_S_ADDRESS) {
                        export = _export;
                    }
                }
            }

            if (export) {
                GtkMl_S export_value;
                export_value.next = NULL;
                gtk_ml_dbg_read_sobject(ctx, err, &export_value, export);
                GtkMl_SObj export = &export_value;

                if (!first) {
                    fprintf(stream, " ");
                } else {
                    first = 0;
                }
                if (export->kind == GTKML_S_PROGRAM) {
                    gtk_ml_dumpf_debug(ctx, stream, err, export->value.s_program.linkage_name);
                    fprintf(stream, " 0x%08"GTKML_FMT_64"x 0x%08"GTKML_FMT_64"x\n", export->value.s_program.addr, call_at);
                } else if (export->kind == GTKML_S_ADDRESS) {
                    gtk_ml_dumpf_debug(ctx, stream, err, export->value.s_address.linkage_name);
                    fprintf(stream, " 0x%08"GTKML_FMT_64"x 0x%08"GTKML_FMT_64"x\n", export->value.s_address.addr, call_at);
                }
            } else {
                fprintf(stream, " #nil\n");
            }
        }

        size_t call_at = gtk_ml_dbg_read_u64(ctx, err, &vm->pc);
        if (*err) {
            return 0;
        }
        GtkMl_SObj export = NULL;
        for (size_t ptr = 0; ptr <= call_at; ptr += 8) {
            uint64_t word = gtk_ml_dbg_read_u64(ctx, err, &text[ptr >> 3]);
            if (*err) {
                return 0;
            }
            GtkMl_Instruction instr = *(GtkMl_Instruction *) &word;
            if (instr.category == GTKML_I_EXPORT) {
                GtkMl_SObj _export = gtk_ml_dbg_read_ptr(ctx, err, &statics[data[instr.data].value.u64]);
                if (*err) {
                    return 0;
                }
                GtkMl_SKind kind = (GtkMl_SKind) gtk_ml_dbg_read_u32(ctx, err, &_export->kind);
                if (kind == GTKML_S_PROGRAM || kind == GTKML_S_ADDRESS) {
                    export = _export;
                }
            }
        }

        if (export) {
            GtkMl_S export_value;
            export_value.next = NULL;
            gtk_ml_dbg_read_sobject(ctx, err, &export_value, export);
            GtkMl_SObj export = &export_value;

            if (!first) {
                fprintf(stream, " ");
            } else {
                first = 0;
            }
            if (export->kind == GTKML_S_PROGRAM) {
                gtk_ml_dumpf_debug(ctx, stream, err, export->value.s_program.linkage_name);
                fprintf(stream, " 0x%08"GTKML_FMT_64"x 0x%08"GTKML_FMT_64"x", export->value.s_program.addr, call_at);
            } else if (export->kind == GTKML_S_ADDRESS) {
                gtk_ml_dumpf_debug(ctx, stream, err, export->value.s_address.linkage_name);
                fprintf(stream, " 0x%08"GTKML_FMT_64"x 0x%08"GTKML_FMT_64"x", export->value.s_address.addr, call_at);
            }
        } else {
            fprintf(stream, " #nil");
        }
    }
    fprintf(stream, "]\n");
    return 1;
}
#endif /* GTKML_ENABLE_POSIX */

gboolean gtk_ml_dumpf_program(GtkMl_Context *ctx, FILE *stream, GtkMl_SObj *err) {
    return gtk_ml_dumpf_program_internal(ctx, stream, err, 0, 0);
}

char *gtk_ml_dumpsn_program(GtkMl_Context *ctx, char *ptr, size_t n, GtkMl_SObj *err) {
    (void) ctx;
    (void) ptr;
    (void) n;
    (void) err;
    return NULL;
}

char *gtk_ml_dumpsnr_program(GtkMl_Context *ctx, char *ptr, size_t n, GtkMl_SObj *err) {
    (void) ctx;
    (void) ptr;
    (void) n;
    (void) err;
    return NULL;
}

#ifdef GTKML_ENABLE_ASM
void gtk_ml_breakpoint(GtkMl_Context *ctx) {
    gtk_ml_breakpoint_internal(ctx, ctx->enable_breakpoint);
}

void gtk_ml_breakpoint_internal(GtkMl_Context *ctx, gboolean enable) {
    if (enable) {
        __asm__ volatile ( "int $3" : : "a" (ctx) :);
    }
}
#endif /* GTKML_ENABLE_ASM */

#ifdef GTKML_ENABLE_POSIX
uint64_t gtk_ml_dbg_read_memory(GtkMl_Context *ctx, GtkMl_SObj *err, const void *addr) {
    *err = NULL;
    errno = 0;
    uint64_t result = ptrace(PTRACE_PEEKDATA, ctx->dbg_process, addr, NULL);
    if (errno) {
        int errnum = errno;
        *err = gtk_ml_error(ctx, "debugger-error", strerror(errnum), 0, 0, 0, 1, gtk_ml_new_keyword(ctx, NULL, 0, "errno", strlen("errno")), gtk_ml_new_int(ctx, NULL, errnum));
    }
    return result;
}

void gtk_ml_dbg_write_memory(GtkMl_Context *ctx, GtkMl_SObj *err, void *addr, uint64_t value) {
    *err = NULL;
    errno = 0;
    ptrace(PTRACE_POKEDATA, ctx->dbg_process, addr, value);
    if (errno) {
        int errnum = errno;
        *err = gtk_ml_error(ctx, "debugger-error", strerror(errnum), 0, 0, 0, 1, gtk_ml_new_keyword(ctx, NULL, 0, "errno", strlen("errno")), gtk_ml_new_int(ctx, NULL, errnum));
    }
}

gboolean gtk_ml_dbg_read_boolean(GtkMl_Context *ctx, GtkMl_SObj *err, const void *addr) {
    return gtk_ml_dbg_read_memory(ctx, err, addr) & 0xffffffff;
}

void gtk_ml_dbg_write_boolean(GtkMl_Context *ctx, GtkMl_SObj *err, void *addr, gboolean value) {
    uint64_t word = gtk_ml_dbg_read_memory(ctx, err, addr);
    if (*err) {
        return;
    }
    word &= ~0xffffffff;
    word |= value;
    gtk_ml_dbg_write_memory(ctx, err, addr, word);
}

uint8_t gtk_ml_dbg_read_u8(GtkMl_Context *ctx, GtkMl_SObj *err, const void *addr) {
    return gtk_ml_dbg_read_memory(ctx, err, addr) & 0xff;
}

void gtk_ml_dbg_write_u8(GtkMl_Context *ctx, GtkMl_SObj *err, void *addr, uint8_t value) {
    uint64_t word = gtk_ml_dbg_read_memory(ctx, err, addr);
    if (*err) {
        return;
    }
    word &= ~0xff;
    word |= value;
    gtk_ml_dbg_write_memory(ctx, err, addr, word);
}

uint32_t gtk_ml_dbg_read_u32(GtkMl_Context *ctx, GtkMl_SObj *err, const void *addr) {
    return gtk_ml_dbg_read_memory(ctx, err, addr) & 0xffffffff;
}

void gtk_ml_dbg_write_u32(GtkMl_Context *ctx, GtkMl_SObj *err, void *addr, uint32_t value) {
    uint64_t word = gtk_ml_dbg_read_memory(ctx, err, addr);
    if (*err) {
        return;
    }
    word &= ~0xffffffff;
    word |= value;
    gtk_ml_dbg_write_memory(ctx, err, addr, word);
}

uint64_t gtk_ml_dbg_read_u64(GtkMl_Context *ctx, GtkMl_SObj *err, const void *addr) {
    return gtk_ml_dbg_read_memory(ctx, err, addr);
}

void gtk_ml_dbg_write_u64(GtkMl_Context *ctx, GtkMl_SObj *err, void *addr, uint64_t value) {
    gtk_ml_dbg_write_memory(ctx, err, addr, value);
}

void *gtk_ml_dbg_read_ptr(GtkMl_Context *ctx, GtkMl_SObj *err, const void *addr) {
    return (void *) gtk_ml_dbg_read_memory(ctx, err, addr);
}

void gtk_ml_dbg_write_ptr(GtkMl_Context *ctx, GtkMl_SObj *err, void *addr, void *value) {
    gtk_ml_dbg_write_memory(ctx, err, addr, (uint64_t) value);
}

void gtk_ml_dbg_read_sobject(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_SObj out, const GtkMl_SObj addr) {
    GtkMl_SObj next = out->next;
    void *ptr = out;
    for (size_t i = 0; i < sizeof(GtkMl_S); i += 8) {
        *((uint64_t *) ((uint8_t *) ptr + i)) = gtk_ml_dbg_read_u64(ctx, err, (uint8_t *) addr + i);
        if (*err) {
            return;
        }
    }
    out->next = next;
}

void gtk_ml_dbg_write_sobject(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_SObj addr, GtkMl_SObj value) {
    // read the original value at address
    GtkMl_S at_addr;
    gtk_ml_dbg_read_sobject(ctx, err, &at_addr, addr);
    if (*err) {
        return;
    }

    void *ptr = value;
    for (size_t i = 0; i < sizeof(GtkMl_S); i += 8) {
        gtk_ml_dbg_write_u64(ctx, err, (uint8_t *) addr + i, *((uint64_t *) ((uint8_t *) ptr + i)));
        if (*err) {
            return;
        }
    }

    // write the original `next` pointer
    gtk_ml_dbg_write_ptr(ctx, err, addr, at_addr.next);
}

GtkMl_TaggedValue gtk_ml_dbg_read_value(GtkMl_Context *ctx, GtkMl_SObj *err, const GtkMl_TaggedValue *addr) {
    return gtk_ml_value_uint(gtk_ml_dbg_read_memory(ctx, err, addr));
}

void gtk_ml_dbg_write_value(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue *addr, GtkMl_TaggedValue value) {
    gtk_ml_dbg_write_memory(ctx, err, addr, value.value.u64);
}
#endif /* GTKML_ENABLE_POSIX */
