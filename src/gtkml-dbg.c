#ifndef GTKML_ENABLE_POSIX
#error "cannot compile gtkml-dbg without POSIX"
#endif /* GTKML_ENABLE_POSIX */

#define _GNU_SOURCE 1
#include <errno.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <gtk/gtk.h>
#include <linenoise.h>
#include "gtk-ml.h"

#define GTKML_DBG_VERSION "gtkml1dbg ver. 0.0.0"

struct ParameterPrototype {
    gboolean exists;
    gboolean takes_value;
    gboolean multiple_allowed;
    gboolean takes_rest;
    char short_opt;
    const char *long_opt;
    const char *value_name;
    const char *description;
};

GTKML_PRIVATE const struct ParameterPrototype PARAMS[] = {
    ['a'] = { 1, 1, 1, 1, 'a', "args", "ARGS", "Debugs a program with the specified arguments." },
    ['h'] = { 1, 0, 0, 0, 'h', "help", NULL, "Print this message and exit." },
    ['V'] = { 1, 0, 0, 0, 'V', "version", NULL, "Print the current version and exit." },
    ['v'] = { 1, 0, 0, 0, 'v', "verbose", NULL, "Print some additional information." },
    [255] = {0, 0, 0, 0, 0, NULL, NULL, NULL },
};

enum GtkMl_Reg {
    GTKML_REG_RAX, GTKML_REG_RBX, GTKML_REG_RCX, GTKML_REG_RDX,
    GTKML_REG_RDI, GTKML_REG_RSI, GTKML_REG_RBP, GTKML_REG_RSP,
    GTKML_REG_R8,  GTKML_REG_R9,  GTKML_REG_R10, GTKML_REG_R11,
    GTKML_REG_R12, GTKML_REG_R13, GTKML_REG_R14, GTKML_REG_R15,
    GTKML_REG_RIP, GTKML_REG_RFLAGS, GTKML_REG_CS,
    GTKML_REG_ORIG_RAX, GTKML_REG_FS_BASE,
    GTKML_REG_GS_BASE,
    GTKML_REG_FS, GTKML_REG_GS, GTKML_REG_SS, GTKML_REG_DS, GTKML_REG_ES
};

GTKML_PRIVATE const size_t n_registers = 27;

struct GtkMl_RegDescriptor {
    enum GtkMl_Reg r;
    int dwarf_r;
    const char *name;
};

GTKML_PRIVATE const struct GtkMl_RegDescriptor reg_descriptors[] = {
    { GTKML_REG_R15, 15, "r15" },
    { GTKML_REG_R14, 14, "r14" },
    { GTKML_REG_R13, 13, "r13" },
    { GTKML_REG_R12, 12, "r12" },
    { GTKML_REG_RBP, 6, "rbp" },
    { GTKML_REG_RBX, 3, "rbx" },
    { GTKML_REG_R11, 11, "r11" },
    { GTKML_REG_R10, 10, "r10" },
    { GTKML_REG_R9, 9, "r9" },
    { GTKML_REG_R8, 8, "r8" },
    { GTKML_REG_RAX, 0, "rax" },
    { GTKML_REG_RCX, 2, "rcx" },
    { GTKML_REG_RDX, 1, "rdx" },
    { GTKML_REG_RSI, 4, "rsi" },
    { GTKML_REG_RDI, 5, "rdi" },
    { GTKML_REG_ORIG_RAX, -1, "orig_rax" },
    { GTKML_REG_RIP, -1, "rip" },
    { GTKML_REG_CS, 51, "cs" },
    { GTKML_REG_RFLAGS, 49, "eflags" },
    { GTKML_REG_RSP, 7, "rsp" },
    { GTKML_REG_SS, 52, "ss" },
    { GTKML_REG_FS_BASE, 58, "fs_base" },
    { GTKML_REG_GS_BASE, 59, "gs_base" },
    { GTKML_REG_DS, 53, "ds" },
    { GTKML_REG_ES, 50, "es" },
    { GTKML_REG_FS, 54, "fs" },
    { GTKML_REG_GS, 55, "gs" },
};

#define array_len(array) (sizeof(array) / sizeof(array[0]))

void print_help(const char *arg0) {
    fprintf(stderr, "usage: %s PARAMS\n\n", arg0);
    fprintf(stderr, "# PARAMS in alphabetical order\n");
    size_t desc_offset = 0;
    for (size_t i = 0; i < array_len(PARAMS); i++) {
        if (PARAMS[i].exists) {
            size_t offset = 0;
            offset += strlen("  ");
            if (PARAMS[i].short_opt) {
                offset += strlen(":_  ");
            }
            offset += strlen(PARAMS[i].long_opt) + 2;
            if (PARAMS[i].takes_value) {
                if (PARAMS[i].multiple_allowed) {
                    offset += strlen("...");
                }
                offset += strlen(PARAMS[i].value_name) + 4;
                if (PARAMS[i].takes_rest) {
                    offset += strlen("...");
                }
            }
            if (offset > desc_offset) {
                desc_offset = offset;
            }
        }
    }
    for (size_t i = 0; i < array_len(PARAMS); i++) {
        if (PARAMS[i].exists) {
            size_t offset = 0;
            offset += fprintf(stderr, "  ");
            if (PARAMS[i].short_opt) {
                offset += fprintf(stderr, ":%c  ", PARAMS[i].short_opt);
            }
            offset += fprintf(stderr, ":%s ", PARAMS[i].long_opt);
            if (PARAMS[i].takes_value) {
                if (PARAMS[i].multiple_allowed) {
                    offset += fprintf(stderr, "...");
                }
                offset += fprintf(stderr, "%s", PARAMS[i].value_name);
                if (PARAMS[i].takes_rest) {
                    offset += fprintf(stderr, "...");
                }
                offset += fprintf(stderr, "    ");
            }
            for (size_t j = 0; j < desc_offset - offset; j++) {
                fprintf(stderr, " ");
            }
            fprintf(stderr, "%s\n", PARAMS[i].description);
        }
    }
}

void print_version(const char *arg0) {
    fprintf(stderr, "%s: %s", arg0, GTKML_DBG_VERSION);
    fprintf(stderr, "%s: %s", arg0, GTKML_VERSION);
}

int main(int argc, const char **argv, char *const *envp) {
    GtkMl_S *err = NULL;

    GtkMl_Context *ctx = gtk_ml_new_debugger(-1);

    const char *rest = NULL;

    GtkMl_HashTrie flags;
    gtk_ml_new_hash_trie(&flags, &GTKML_DEFAULT_HASHER);

    GtkMl_HashTrie opts;
    gtk_ml_new_hash_trie(&opts, &GTKML_DEFAULT_HASHER);

    for (size_t i = 0; i < array_len(PARAMS); i++) {
        if (PARAMS[i].exists) {
            if (PARAMS[i].takes_value) {
                const char *long_opt = PARAMS[i].long_opt;
                GtkMl_HashTrie new;
                gtk_ml_new_hash_trie(&new, &GTKML_DEFAULT_HASHER);
                GtkMl_S *default_value;
                if (PARAMS[i].multiple_allowed || PARAMS[i].takes_rest) {
                    default_value = gtk_ml_new_array(ctx, NULL);
                } else {
                    default_value = gtk_ml_new_nil(ctx, NULL);
                }
                if (PARAMS[i].takes_rest) {
                    rest = PARAMS[i].long_opt;
                }
                gtk_ml_hash_trie_insert(&new, &opts, gtk_ml_new_keyword(ctx, NULL, 0, long_opt, strlen(long_opt)), default_value);
                gtk_ml_del_hash_trie(ctx, &opts, gtk_ml_delete_void_reference);
                opts = new;
            } else {
                const char *long_opt = PARAMS[i].long_opt;
                GtkMl_HashTrie new;
                gtk_ml_new_hash_trie(&new, &GTKML_DEFAULT_HASHER);
                gtk_ml_hash_trie_insert(&new, &flags, gtk_ml_new_keyword(ctx, NULL, 0, long_opt, strlen(long_opt)), gtk_ml_new_false(ctx, NULL));
                gtk_ml_del_hash_trie(ctx, &flags, gtk_ml_delete_void_reference);
                flags = new;
            }
        }
    }

    for (int i = 1; i < argc; i++) {
        const char *long_opt = NULL;
        if (argv[i][0] == ':') {
            if (argv[i][2] == 0) {
                if (!PARAMS[(unsigned char) argv[i][1]].exists) {
                    err = gtk_ml_error(ctx, "param-error", "unrecognized short option", 0, 0, 0, 1, gtk_ml_new_keyword(ctx, NULL, 0, "param", strlen("param")), gtk_ml_new_string(ctx, NULL, argv[i], strlen(argv[i])));
                    gtk_ml_dumpf(ctx, stderr, NULL, err);
                    fprintf(stderr, "\n");
                    gtk_ml_del_context(ctx);
                    return 1;
                }
                long_opt = PARAMS[(unsigned char) argv[i][1]].long_opt;
            } else {
                long_opt = &argv[i][1];
            }
        } else {
            err = gtk_ml_error(ctx, "param-error", "parameters must be preceded by `:`", 0, 0, 0, 1, gtk_ml_new_keyword(ctx, NULL, 0, "param", strlen("param")), gtk_ml_new_string(ctx, NULL, argv[i], strlen(argv[i])));
            gtk_ml_dumpf(ctx, stderr, NULL, err);
            fprintf(stderr, "\n");
            gtk_ml_del_context(ctx);
            return 1;
        }

        GtkMl_S *keyword = gtk_ml_new_keyword(ctx, NULL, 0, long_opt, strlen(long_opt));
        if (rest && strcmp(rest, long_opt) == 0) {
            ++i;
            for (; i < argc; i++) {
                GtkMl_S *value = gtk_ml_new_string(ctx, NULL, argv[i], strlen(argv[i]));

                GtkMl_S *current = gtk_ml_hash_trie_get(&opts, keyword);
                if (current->kind == GTKML_S_ARRAY) {
                    GtkMl_Array new;
                    gtk_ml_array_trie_push(&new, &current->value.s_array.array, value);
                    gtk_ml_del_array_trie(ctx, &current->value.s_array.array, gtk_ml_delete_value_reference);
                    current->value.s_array.array = new;
                } else {
                    GtkMl_HashTrie new;
                    gtk_ml_hash_trie_insert(&new, &opts, keyword, value);
                    gtk_ml_del_hash_trie(ctx, &opts, gtk_ml_delete_void_reference);
                    opts = new;
                }
            }
        } else if (gtk_ml_hash_trie_contains(&flags, keyword)) {
            GtkMl_HashTrie new;
            gtk_ml_hash_trie_insert(&new, &flags, keyword, gtk_ml_new_true(ctx, NULL));
            gtk_ml_del_hash_trie(ctx, &flags, gtk_ml_delete_void_reference);
            flags = new;
        } else if (gtk_ml_hash_trie_contains(&opts, keyword)) {
            ++i;
            if (i == argc) {
                err = gtk_ml_error(ctx, "value-error", "missing value to parameter", 0, 0, 0, 1, gtk_ml_new_keyword(ctx, NULL, 0, "param", strlen("param")), keyword);
                gtk_ml_dumpf(ctx, stderr, NULL, err);
                fprintf(stderr, "\n");
                gtk_ml_del_context(ctx);
                return 1;
            }

            GtkMl_S *value = gtk_ml_new_string(ctx, NULL, argv[i], strlen(argv[i]));

            GtkMl_S *current = gtk_ml_hash_trie_get(&opts, keyword);
            if (current->kind == GTKML_S_ARRAY) {
                GtkMl_Array new;
                gtk_ml_array_trie_push(&new, &current->value.s_array.array, value);
                gtk_ml_del_array_trie(ctx, &current->value.s_array.array, gtk_ml_delete_value_reference);
                current->value.s_array.array = new;
            } else {
                GtkMl_HashTrie new;
                gtk_ml_hash_trie_insert(&new, &opts, keyword, value);
                gtk_ml_del_hash_trie(ctx, &opts, gtk_ml_delete_void_reference);
                opts = new;
            }
        } else {
            err = gtk_ml_error(ctx, "param-error", "unrecognized parameter", 0, 0, 0, 1, gtk_ml_new_keyword(ctx, NULL, 0, "param", strlen("param")), keyword);
            gtk_ml_dumpf(ctx, stderr, NULL, err);
            fprintf(stderr, "\n");
            gtk_ml_del_context(ctx);
            return 1;
        }
    }

    GtkMl_S *help_kw = gtk_ml_new_keyword(ctx, NULL, 0, PARAMS['h'].long_opt, strlen(PARAMS['h'].long_opt));
    GtkMl_S *help_opt = gtk_ml_hash_trie_get(&flags, help_kw);
    if (help_opt->kind == GTKML_S_TRUE) {
        print_help(argv[0]);
        return 0;
    }

    GtkMl_S *version_kw = gtk_ml_new_keyword(ctx, NULL, 0, PARAMS['V'].long_opt, strlen(PARAMS['V'].long_opt));
    GtkMl_S *version_opt = gtk_ml_hash_trie_get(&flags, version_kw);
    if (version_opt->kind == GTKML_S_TRUE) {
        print_version(argv[0]);
        return 0;
    }

    GtkMl_S *verbose_kw = gtk_ml_new_keyword(ctx, NULL, 0, PARAMS['v'].long_opt, strlen(PARAMS['v'].long_opt));
    GtkMl_S *verbose_opt = gtk_ml_hash_trie_get(&flags, verbose_kw);
    if (verbose_opt->kind == GTKML_S_TRUE) {
        GtkMl_S *s_flags = gtk_ml_new_map(ctx, NULL, NULL);
        s_flags->value.s_map.map = flags;
        GtkMl_S *s_opts = gtk_ml_new_map(ctx, NULL, NULL);
        s_opts->value.s_map.map = opts;

        fprintf(stderr, "running with flags: ");
        gtk_ml_dumpf(ctx, stderr, &err, s_flags);
        fprintf(stderr, "\nrunning with options: ");
        gtk_ml_dumpf(ctx, stderr, &err, s_opts);
        fprintf(stderr, "\n");
    }

    const char *exec = NULL;
    char **args = NULL;
    char **env = NULL;
    GtkMl_S *args_kw = gtk_ml_new_keyword(ctx, NULL, 0, PARAMS['a'].long_opt, strlen(PARAMS['a'].long_opt));
    GtkMl_S *args_opt = gtk_ml_hash_trie_get(&opts, args_kw);

    size_t len = gtk_ml_array_trie_len(&args_opt->value.s_array.array);
    if (len > 0) {
        size_t envc = 0;
        char *const *_envp = envp;
        while (*envp) {
            ++envc;
            ++envp;
        }
        envp = _envp;

        exec = gtk_ml_to_c_str(gtk_ml_array_trie_get(&args_opt->value.s_array.array, 0));
        args = malloc(sizeof(char *) * (len + 1));
        env = malloc(sizeof(char *) * (envc + 2));
        for (size_t i = 0; i < len; i++) {
            args[i] = gtk_ml_to_c_str(gtk_ml_array_trie_get(&args_opt->value.s_array.array, i));
        }
        size_t i = 0;
        while (*envp) {
            env[i] = *envp;
            ++i;
            ++envp;
        }
        env[i++] = "GTKML_ENABLE_DEBUG=1";
        env[i++] = NULL;
        envp = _envp;
        args[len] = NULL;
    } else {
        err = gtk_ml_error(ctx, "missing-parameter-error", "a required parameter is missing", 0, 0, 0, 1, gtk_ml_new_keyword(ctx, NULL, 0, "param", strlen("param")), args_kw);
        gtk_ml_dumpf(ctx, stderr, NULL, err);
        fprintf(stderr, "\n");
        gtk_ml_del_context(ctx);
        return 1;
    }

    pid_t pid = fork();
    if (pid == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execvpe(exec, args, env);

        err = gtk_ml_error(ctx, "exec-error", strerror(errno), 0, 0, 0, 1, gtk_ml_new_keyword(ctx, NULL, 0, "errno", strlen("errno")), gtk_ml_new_int(ctx, NULL, errno), gtk_ml_new_keyword(ctx, NULL, 0, "path", strlen("path")), gtk_ml_new_string(ctx, NULL, exec, strlen(exec)));
        gtk_ml_dumpf(ctx, stderr, NULL, err);
        fprintf(stderr, "\n");
        gtk_ml_del_context(ctx);
        return 1;
    } else if (pid > 0) {
        // TODO
        (void) n_registers;
        (void) reg_descriptors;

        // wait for the process to start
        int wait_status;
        waitpid(pid, &wait_status, 0);

        // run the process
        ptrace(PTRACE_CONT, pid, NULL, NULL);
        waitpid(pid, &wait_status, 0);

        // the process then stops at gtk_ml_breakpoint to send us its ctx
        struct user_regs_struct regs;
        ptrace(PTRACE_GETREGS, pid, NULL, &regs);

        ptrace(PTRACE_CONT, pid, NULL, NULL);
        waitpid(pid, &wait_status, 0);

        gtk_ml_set_debug(ctx, pid, (GtkMl_Context *) regs.rax);

        linenoiseInstallWindowChangeHandler();

        //linenoiseSetCompletionCallback(completionHook);

        char const *prompt = "dbg>>> ";

        char *src = NULL;
        while (1) {
            char *line = linenoise(prompt);
            if (!line) {
                break;
            }
            if (!*line) {
                continue;
            }

            if (!src) {
                src = malloc(strlen(line) + 1);
                src[0] = 0;
            } else {
                src = realloc(src, strlen(src) + strlen(line) + 1);
            }

            char *_line = line;
            size_t paren = 0;
            size_t i = strlen(src);
            while (*line) {
                switch (*line) {
                case '(':
                case '[':
                case '{':
                    ++paren;
                    break;
                case ')':
                case ']':
                case '}':
                    --paren;
                    break;
                }
                src[i] = *line;
                ++i;
                ++line;
            }
            line = _line;

            if (paren <= 0) {
                GtkMl_Program program;

                GtkMl_S *lambda;
                if (!(lambda = gtk_ml_loads(ctx, &err, src))) {
                    free(src);
                    src = NULL;
                    free(line);
                    gtk_ml_dumpf(ctx, stderr, NULL, err);
                    fprintf(stderr, "\n");
                    continue;
                }

                GtkMl_Builder *builder = gtk_ml_new_builder();

                if (!gtk_ml_compile_program(ctx, builder, &err, lambda)) {
                    free(src);
                    src = NULL;
                    free(line);
                    gtk_ml_dumpf(ctx, stderr, NULL, err);
                    fprintf(stderr, "\n");
                    continue;
                }

                if (!gtk_ml_build(ctx, &program, &err, builder)) {
                    free(src);
                    src = NULL;
                    free(line);
                    gtk_ml_dumpf(ctx, stderr, NULL, err);
                    fprintf(stderr, "\n");
                    continue;
                }

                gtk_ml_load_program(ctx, &program);

                GtkMl_S *start = gtk_ml_get_export(ctx, &err, program.start);
                if (!start) {
                    free(src);
                    src = NULL;
                    free(line);
                    gtk_ml_dumpf(ctx, stderr, NULL, err);
                    fprintf(stderr, "\n");
                    continue;
                }
                gtk_ml_del_program(&program);

                if (!gtk_ml_run_program(ctx, &err, start, NULL)) {
                    free(src);
                    src = NULL;
                    free(line);
                    gtk_ml_dumpf(ctx, stderr, NULL, err);
                    fprintf(stderr, "\n");
                    continue;
                }

                free(src);
                src = NULL;
            }
            free(line);
        }
    } else {
        err = gtk_ml_error(ctx, "fork-error", strerror(errno), 0, 0, 0, 1, gtk_ml_new_keyword(ctx, NULL, 0, "errno", strlen("errno")), gtk_ml_new_int(ctx, NULL, errno));
        gtk_ml_dumpf(ctx, stderr, NULL, err);
        fprintf(stderr, "\n");
        gtk_ml_del_context(ctx);
        return 1;
    }

    gtk_ml_del_context(ctx);

    return 0;
}
