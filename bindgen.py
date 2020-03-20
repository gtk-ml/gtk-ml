#!/usr/bin/env python3

import sys
import io
import os
import clang.cindex as cindex
from clang.cindex import Index, CursorKind, TypeKind, LinkageKind

import argparse
from argparse import ArgumentParser

TYPE_CHAR = [
    TypeKind.CHAR_S,
    TypeKind.SCHAR,
]
TYPE_INT = [
    TypeKind.BOOL,
    TypeKind.CHAR_U,
    TypeKind.UCHAR,
    TypeKind.CHAR16,
    TypeKind.CHAR32,
    TypeKind.USHORT,
    TypeKind.UINT,
    TypeKind.ULONG,
    TypeKind.ULONGLONG,
    TypeKind.UINT128,
    TypeKind.CHAR_S,
    TypeKind.SCHAR,
    TypeKind.WCHAR,
    TypeKind.SHORT,
    TypeKind.INT,
    TypeKind.LONG,
    TypeKind.LONGLONG,
    TypeKind.INT128,
]
TYPE_FLOAT = [
    TypeKind.FLOAT,
    TypeKind.DOUBLE,
    TypeKind.LONGDOUBLE,
]

def with_prefix(name, prefix=None, divider='/'):
    if prefix:
        return f"{prefix}{divider}{name}"
    else:
        return name

def is_function_declaration(args, c):
    return CursorKind.is_declaration(c.kind) \
        and not c.is_definition() \
        and (c.linkage == LinkageKind.EXTERNAL \
            or c.linkage == LinkageKind.NO_LINKAGE) \
        and c.result_type != None \
        and c.result_type.get_canonical().kind != TypeKind.INVALID \
        and c.spelling not in args.skip

def gen_const_bindings(header, out, args):
    with open(header) as f:
        for line in f.readlines():
            line = line.strip()
            if line.startswith('#'):
                line = line[1:-1].strip()
                if line.startswith('define '):
                    line = line[7:-1].strip()
                    space = 0
                    for i in range(0, len(line)):
                        if line[i].isspace():
                            space = i
                            break
                    if line[space + 1:-1].strip()[0].isdigit():
                        name = line[0:space]
                        out.write(f'gtk_ml_bind(ctx, gtk_ml_new_symbol(ctx, 0, 0, "{with_prefix(name, args.prefix)}", strlen("{with_prefix(name, args.prefix)}")), gtk_ml_value_int({name}));\n')

def gen_core_libs_bind(header, out, tu, args):
    for c in tu.cursor.get_children():
        if is_function_declaration(args, c):
            out.write(f'[GTKML_CORE_{with_prefix(c.spelling, args.prefix, "_").upper()}] = vm_core_{with_prefix(c.spelling, args.prefix, "_")},\n')

def gen_code_gen_bind(header, out, tu, args):
    for c in tu.cursor.get_children():
        if is_function_declaration(args, c):
            out.write(f'gtk_ml_add_builder(b, "{with_prefix(c.spelling, args.prefix)}", gtk_ml_builder_{with_prefix(c.spelling, args.prefix, "_")}, 0, 0, 1);\n')

def gen_core_libs_header(header, out, tu, args, core_number=0x500):
    for c in tu.cursor.get_children():
        if is_function_declaration(args, c):
            out.write(f'#define GTKML_CORE_{with_prefix(c.spelling, args.prefix, "_").upper()} 0x{core_number:x}\n')
            core_number += 1
    for c in tu.cursor.get_children():
        if is_function_declaration(args, c):
            out.write(f'GTKML_PUBLIC GtkMl_TaggedValue vm_core_{with_prefix(c.spelling, args.prefix, "_")}(GtkMl_Context *, GtkMl_SObj *, GtkMl_TaggedValue);\n')

def gen_code_gen_header(header, out, tu, args):
    for c in tu.cursor.get_children():
        if is_function_declaration(args, c):
            out.write(f'GTKML_PUBLIC gboolean gtk_ml_builder_{with_prefix(c.spelling, args.prefix, "_")}(GtkMl_Index *index, GtkMl_Context *, GtkMl_Builder *, GtkMl_BasicBlock **, GtkMl_SObj *, GtkMl_SObj *, gboolean, gboolean, gboolean, gboolean);\n')

def gen_core_libs_impl(header, out, tu, args):
    for c in tu.cursor.get_children():
        if is_function_declaration(args, c):
            return_type = c.type.get_result().get_canonical()
            params = []
            for arg in c.type.argument_types():
                params.append(arg.get_canonical())
            out.write(f'''
GtkMl_TaggedValue vm_core_{with_prefix(c.spelling, args.prefix, "_")}(GtkMl_Context *ctx, GtkMl_SObj *err, GtkMl_TaggedValue expr) {{
    (void) ctx;
    (void) expr;

    *err = NULL;
    GtkMl_TaggedValue result = gtk_ml_value_none();
    GtkMl_TaggedValue tmp = gtk_ml_value_none();
    (void) tmp;
''')

            c_args = []
            arg_num = len(params) - 1
            for param in reversed(params):
                if param.kind in TYPE_INT:
                    out.write(f'''
    tmp = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    if (*err) {{ return result; }}
    if (tmp.tag != GTKML_TAG_FLOAT && (tmp.tag & GTKML_TAG_INT) != GTKML_TAG_INT) {{ 
        *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 3,
            gtk_ml_new_keyword(ctx, 0, 0, "function", strlen("function")),
            gtk_ml_new_string(ctx, 0, "{with_prefix(c.spelling, args.prefix, "/")}", strlen("{with_prefix(c.spelling, args.prefix, "/")}")),
            gtk_ml_new_keyword(ctx, 0, 0, "got", strlen("got")),
            gtk_ml_to_sobj(ctx, NULL, tmp).value.sobj,
            gtk_ml_new_keyword(ctx, 0, 0, "expected", strlen("expected")),
            gtk_ml_new_keyword(ctx, 0, 0, "int", strlen("int")));
        return result;
    }}
    int64_t arg{arg_num} = gtk_ml_prim_to_int(tmp);
''')
                elif param.kind in TYPE_FLOAT:
                    out.write(f'''
    tmp = gtk_ml_to_prim(ctx, err, gtk_ml_pop(ctx));
    if (*err) {{ return result; }}
    if (tmp.tag != GTKML_TAG_FLOAT && (tmp.tag & GTKML_TAG_INT) != GTKML_TAG_INT) {{ 
        *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 3,
            gtk_ml_new_keyword(ctx, 0, 0, "function", strlen("function")),
            gtk_ml_new_string(ctx, 0, "{with_prefix(c.spelling, args.prefix, "/")}", strlen("{with_prefix(c.spelling, args.prefix, "/")}")),
            gtk_ml_new_keyword(ctx, 0, 0, "got", strlen("got")),
            gtk_ml_to_sobj(ctx, NULL, tmp).value.sobj,
            gtk_ml_new_keyword(ctx, 0, 0, "expected", strlen("expected")),
            gtk_ml_new_keyword(ctx, 0, 0, "float", strlen("float")));
        return result;
    }}
    double arg{arg_num} = gtk_ml_prim_to_float(tmp);
''')
                elif param.kind == TypeKind.POINTER and param.get_pointee().get_canonical().kind in TYPE_CHAR and c.spelling not in args.no_string:
                    out.write(f'''
    tmp = gtk_ml_pop(ctx);
    char *arg{arg_num};
    if (gtk_ml_is_sobject(tmp)) {{
        switch (tmp.value.sobj->kind) {{
        case GTKML_S_ARRAY:
            if (gtk_ml_array_trie_is_string(&tmp.value.sobj->value.s_array.array)) {{
                arg{arg_num} = gtk_ml_to_c_str(tmp.value.sobj);
            }} else {{
                *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 3,
                    gtk_ml_new_keyword(ctx, 0, 0, "function", strlen("function")),
                    gtk_ml_new_string(ctx, 0, "{with_prefix(c.spelling, args.prefix, "/")}", strlen("{with_prefix(c.spelling, args.prefix, "/")}")),
                    gtk_ml_new_keyword(ctx, 0, 0, "got", strlen("got")),
                    gtk_ml_to_sobj(ctx, NULL, tmp).value.sobj,
                    gtk_ml_new_keyword(ctx, 0, 0, "expected", strlen("expected")),
                    gtk_ml_new_keyword(ctx, 0, 0, "string", strlen("string")));
                return result;
            }}
            break;
        default:
            *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 3,
                gtk_ml_new_keyword(ctx, 0, 0, "function", strlen("function")),
                gtk_ml_new_string(ctx, 0, "{with_prefix(c.spelling, args.prefix, "/")}", strlen("{with_prefix(c.spelling, args.prefix, "/")}")),
                gtk_ml_new_keyword(ctx, 0, 0, "got", strlen("got")),
                gtk_ml_to_sobj(ctx, NULL, tmp).value.sobj,
                gtk_ml_new_keyword(ctx, 0, 0, "expected", strlen("expected")),
                gtk_ml_new_keyword(ctx, 0, 0, "string", strlen("string")));
            return result;
        }}
    }} else {{
        *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 3,
            gtk_ml_new_keyword(ctx, 0, 0, "function", strlen("function")),
            gtk_ml_new_string(ctx, 0, "{with_prefix(c.spelling, args.prefix, "/")}", strlen("{with_prefix(c.spelling, args.prefix, "/")}")),
            gtk_ml_new_keyword(ctx, 0, 0, "got", strlen("got")),
            gtk_ml_to_sobj(ctx, NULL, tmp).value.sobj,
            gtk_ml_new_keyword(ctx, 0, 0, "expected", strlen("expected")),
            gtk_ml_new_keyword(ctx, 0, 0, "string", strlen("string")));
        return result;
    }}
''')
                elif param.kind == TypeKind.POINTER:
                    out.write(f'''
    tmp = gtk_ml_pop(ctx);
    void *arg{arg_num};
    if (gtk_ml_is_sobject(tmp)) {{
        switch (tmp.value.sobj->kind) {{
        case GTKML_S_LIGHTDATA:
            arg{arg_num} = tmp.value.sobj->value.s_lightdata.userdata;
            break;
        case GTKML_S_USERDATA:
            arg{arg_num} = tmp.value.sobj->value.s_userdata.userdata;
            break;
        default:
            *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 3,
                gtk_ml_new_keyword(ctx, 0, 0, "function", strlen("function")),
                gtk_ml_new_string(ctx, 0, "{with_prefix(c.spelling, args.prefix, "/")}", strlen("{with_prefix(c.spelling, args.prefix, "/")}")),
                gtk_ml_new_keyword(ctx, 0, 0, "got", strlen("got")),
                gtk_ml_to_sobj(ctx, NULL, tmp).value.sobj,
                gtk_ml_new_keyword(ctx, 0, 0, "expected", strlen("expected")),
                gtk_ml_new_keyword(ctx, 0, 0, "userdata", strlen("userdata")));
            return result;
        }}
    }} else {{
        if (tmp.tag != GTKML_TAG_USERDATA) {{
            *err = gtk_ml_error(ctx, "type-error", GTKML_ERR_TYPE_ERROR, 0, 0, 0, 3,
                gtk_ml_new_keyword(ctx, 0, 0, "function", strlen("function")),
                gtk_ml_new_string(ctx, 0, "{with_prefix(c.spelling, args.prefix, "/")}", strlen("{with_prefix(c.spelling, args.prefix, "/")}")),
                gtk_ml_new_keyword(ctx, 0, 0, "got", strlen("got")),
                gtk_ml_to_sobj(ctx, NULL, tmp).value.sobj,
                gtk_ml_new_keyword(ctx, 0, 0, "expected", strlen("expected")),
                gtk_ml_new_keyword(ctx, 0, 0, "userdata", strlen("userdata")));
            return result;
        }}
        arg{arg_num} = tmp.value.userdata;
    }}
''')
                else:
                    raise Exception(f'unknown type kind: {param.spelling} at {c.spelling}')
                c_args.append(f'arg{arg_num}')
                arg_num -= 1

            if return_type.kind == TypeKind.VOID:
                out.write(f'\n')
                out.write(f'    {c.spelling}({", ".join(reversed(c_args))});\n')
                out.write(f'    result = gtk_ml_value_nil();\n')
            elif return_type.kind in TYPE_INT:
                out.write(f'\n')
                out.write(f'    result = gtk_ml_value_int({c.spelling}({", ".join(reversed(c_args))}));\n')
            elif return_type.kind in TYPE_FLOAT:
                out.write(f'\n')
                out.write(f'    result = gtk_ml_value_float({c.spelling}({", ".join(reversed(c_args))}));\n')
            elif return_type.kind == TypeKind.POINTER and return_type.get_pointee().get_canonical().kind in TYPE_CHAR and c.spelling not in args.no_string:
                out.write(f'\n')
                out.write(f'    const char *c_str = (const char *) {c.spelling}({", ".join(reversed(c_args))});')
                out.write(f'    result = gtk_ml_value_sobject(gtk_ml_new_string(ctx, 0, c_str, strlen(c_str)));\n')
            elif return_type.kind == TypeKind.POINTER:
                out.write(f'\n')
                out.write(f'    result = gtk_ml_value_userdata((void *) {c.spelling}({", ".join(reversed(c_args))}));\n')
            else:
                raise Exception(f'unknown type kind: {return_type.spelling} at {c.spelling}')

            out.write(f'''
    return result;
}}
''')

def gen_code_gen_impl(header, out, tu, args):
    for c in tu.cursor.get_children():
        if is_function_declaration(args, c):
            args_len = len([arg for arg in c.type.argument_types()])
            out.write(f'''
gboolean gtk_ml_builder_{with_prefix(c.spelling, args.prefix, "_")}(
    GtkMl_Index *index,
    GtkMl_Context *ctx,
    GtkMl_Builder *b,
    GtkMl_BasicBlock **basic_block,
    GtkMl_SObj *err,
    GtkMl_SObj *stmt,
    gboolean allow_intr,
    gboolean allow_macro,
    gboolean allow_runtime,
    gboolean allow_macro_expansion
) {{
    GtkMl_SObj args = gtk_ml_cdr(*stmt);
    GtkMl_Span span = {{0}};
    if ((*stmt)->loc != 0) {{
        span = *gtk_ml_get_span(&index->debug_data, (*stmt)->loc);
    }}
    if (gtk_ml_list_len(args) != {args_len}) {{
        *err = gtk_ml_error(
            ctx,
            "arity-error",
            GTKML_ERR_ARITY_ERROR,
            (*stmt)->loc != 0,
            span.line,
            span.col,
            3,
            gtk_ml_new_keyword(ctx, 0, 0, "function", strlen("function")),
            gtk_ml_new_string(ctx, 0, "{with_prefix(c.spelling, args.prefix, "/")}", strlen("{with_prefix(c.spelling, args.prefix, "/")}")),
            gtk_ml_new_keyword(ctx, 0, 0, "expected", strlen("expected")),
            gtk_ml_new_int(ctx, 0, {args_len}),
            gtk_ml_new_keyword(ctx, 0, 0, "got", strlen("got")),
            gtk_ml_new_int(ctx, 0, gtk_ml_list_len(args))
        );
        return 0;
    }}

    return compile_core_call(
        index,
        ctx,
        b,
        basic_block,
        err,
        GTKML_CORE_{with_prefix(c.spelling, args.prefix, "_").upper()},
        *stmt,
        0,
        allow_intr,
        allow_macro,
        allow_runtime,
        allow_macro_expansion
    );
}}
            ''')

class CheckKind(argparse.Action):
    def __init__(self, option_strings, dest, nargs=None, **kwargs):
        if nargs is not None:
            raise ValueError("nargs not allowed")
        super(CheckKind, self).__init__(option_strings, dest, **kwargs)

    def __call__(self, parser, namespace, value, option_string=None):
        if value != 'bind/core-libs' \
            and value != 'bind/code-gen' \
            and value != 'header/core-libs' \
            and value != 'header/code-gen' \
            and value != 'impl/core-libs' \
            and value != 'impl/code-gen' \
            and value != 'all':
            raise ValueError(f'value {value} not allowed for KIND')
        else:
            namespace.kind = value

def main():
    parser = ArgumentParser(description="generate gtk-ml bindings")
    parser.add_argument("-o", "--out", required=True, dest="output", help="write to FILE", metavar="FILE")
    parser.add_argument("-k", "--kind", required=True, dest="kind", help="generate KIND", metavar="KIND", action=CheckKind)
    parser.add_argument("-N", "--core-number", default=0x500, dest="core_number", help="start numbering core APIs from N", metavar="N")
    parser.add_argument("-p", "--prefix", dest="prefix", help="prefix all bindings with PREFIX/", metavar="PREFIX")
    parser.add_argument("-s", "--skip", nargs=1, dest="skip", help="skip binding", metavar="NAME(-S)")
    parser.add_argument("--skip-file", dest="skip_file", help="skip bindings from file", metavar="FILE")
    parser.add_argument("-S", "--no-string", nargs=1, dest="no_string", help="don't generate string bindings for function", metavar="NAME(-S)")
    parser.add_argument(dest="input", help="read from header", metavar="HEADER")
    parser.add_argument(dest="clang_args", nargs=argparse.REMAINDER, help="pass ... to clang", metavar="...")
    args = parser.parse_args()
    if args.skip and len(args.skip) == 1:
        args.skip = args.skip[0].split(',')
    else:
        args.skip = []
    if args.no_string and len(args.no_string) == 1:
        args.no_string = args.no_string[0].split(',')
    else:
        args.no_string = []
    with open(args.skip_file) as f:
        for line in f.readlines():
            args.skip.append(line.strip())

    header = args.input
    index = Index.create()
    tu = index.parse(args.input, args.clang_args, options=0)
    if not tu:
        parser.error("unable to load input")

    if args.kind == 'all':
        os.makedirs(args.output, exist_ok=True)

        const_bindings = open(f'{args.output}/const-bindings.h', 'w')
        bind_core_libs = open(f'{args.output}/bind-core-libs.h', 'w')
        bind_code_gen = open(f'{args.output}/bind-code-gen.h', 'w')
        header_core_libs = open(f'{args.output}/core-libs.h', 'w')
        header_code_gen = open(f'{args.output}/code-gen.h', 'w')
        impl_core_libs = open(f'{args.output}/core-libs.c', 'w')
        impl_code_gen = open(f'{args.output}/code-gen.c', 'w')

        gen_const_bindings(header, const_bindings, args)
        gen_core_libs_bind(header, bind_core_libs, tu, args)
        gen_code_gen_bind(header, bind_code_gen, tu, args)
        gen_core_libs_header(header, header_core_libs, tu, args, args.core_number)
        gen_code_gen_header(header, header_code_gen, tu, args)
        gen_core_libs_impl(header, impl_core_libs, tu, args)
        gen_code_gen_impl(header, impl_code_gen, tu, args)
    else:
        out = open(args.output, 'w')

        if args.kind == 'bind/const':
            gen_const_bindings(header, out, args)
        elif args.kind == 'bind/core-libs':
            gen_core_libs_bind(header, out, tu, args)
        elif args.kind == 'bind/code-gen':
            gen_code_gen_bind(header, out, tu, args)
        elif args.kind == 'header/core-libs':
            gen_core_libs_header(header, out, tu, args, args.core_number)
        elif args.kind == 'header/code-gen':
            gen_code_gen_header(header, out, tu, args)
        elif args.kind == 'impl/core-libs':
            gen_core_libs_impl(header, out, tu, args)
        elif args.kind == 'impl/code-gen':
            gen_code_gen_impl(header, out, tu, args)

if __name__ == '__main__':
    main()
