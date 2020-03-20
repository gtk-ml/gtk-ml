SRCDIR=src
INCDIR=include
BINDIR=bin
OBJDIR=$(BINDIR)/obj
DBDIR=$(BINDIR)/db
WEBDIR=public
EMCC?=emcc
CC=clang
CXX=clang++

INCLUDE_NAME=gtk-ml
LIB_NAME=libgtk-ml.so
TARGET=$(BINDIR)/$(LIB_NAME)
BINARIES=
SRC=$(SRCDIR)/gtk-ml.c $(SRCDIR)/value.c $(SRCDIR)/builder.c \
	$(SRCDIR)/lex.c $(SRCDIR)/parse.c $(SRCDIR)/code-gen.c \
	$(SRCDIR)/serf.c $(SRCDIR)/vm.c $(SRCDIR)/bytecode.c \
	$(SRCDIR)/hashtrie.c $(SRCDIR)/hashset.c $(SRCDIR)/array.c \
	$(SRCDIR)/index.c 
OBJ=$(patsubst $(SRCDIR)/%,$(OBJDIR)/%.o,$(SRC))
LIB=/usr/local/lib/liblinenoise.a
GTKMLWEB=$(WEBDIR)/gtk-ml.js
WEBGL=em_gles3

CFLAGS:=-O2 -g -Wall -Wextra -Werror -pedantic -std=c11 -fPIC -pthread \
	-DGTKML_ENABLE_ASM=1 -DGTKML_STACK_SIZE=16*1024 \
	-DGTKML_LONG_WIDTH=64 -DGTKML_LLONG_WIDTH=64 -DGTKML_INTWIDTH_DEFINED=1
EMFLAGS:=-O2 -Wall -Wextra -Werror -std=gnu11 \
	-s ASSERTIONS=1 -s NO_EXIT_RUNTIME=1 \
	-s MAX_WEBGL_VERSION=2 \
	-s EXPORTED_FUNCTIONS='["_main", "_gtk_ml_web_init_gl", "_gtk_ml_web_init", "_gtk_ml_web_deinit", "_gtk_ml_web_version", "_gtk_ml_web_eval"]' \
	-s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' \
	-DGTKML_ENABLE_WEB=1 \
	-DGTKML_STACK_SIZE=16*1024 \
	-DGTKML_LONG_WIDTH=32 -DGTKML_LLONG_WIDTH=64 -DGTKML_INTWIDTH_DEFINED=1
LDFLAGS:=$(shell pkg-config --libs gtk+-3.0) -lm
INCLUDE:=-I$(INCDIR) -I/usr/local/include -I.

ifdef ENABLE_ALL
ENABLE_GTK:=1
ENABLE_POSIX:=1
endif

ifdef ENABLE_GTK
CFLAGS+=-DGTKML_ENABLE_GTK=1 $(shell pkg-config --cflags gtk+-3.0)

GTKMLI=$(BINDIR)/gtkmli
BINARIES+=$(GTKMLI)
endif

# posix allows us to use the debugger
ifdef ENABLE_POSIX
CFLAGS+=-DGTKML_ENABLE_POSIX=1
LDFLAGS+=-ldl

GTKMLDBG=$(BINDIR)/gtkml-dbg
BINARIES+=$(GTKMLDBG)
endif

ifdef ENABLE_EMRUN
EMFLAGS+=--emrun
endif

.PHONY: default all build install clean

default: all

all: $(TARGET) $(BINARIES) compile_commands.json

web: $(WEBGL) $(WEBDIR) $(GTKMLWEB)
	cp src/gtkml-web.html public/index.html
	cp src/gtkml-web.js public/

build: $(BINARIES)

install: $(TARGET)
	rm -rf ~/.local/include/$(INCLUDE_NAME)
	mkdir -p ~/.local/include
	mkdir -p ~/.local/lib64
	cp -r include ~/.local/include/$(INCLUDE_NAME)
	cp $(TARGET) ~/.local/lib64/

$(OBJDIR)/%.c.o: $(SRCDIR)/%.c $(OBJDIR) $(DBDIR)
	$(CC) $(CFLAGS) $(INCLUDE) -MJ $(DBDIR)/$*.c.o.json -c -o $@ $<

compile_commands.json: $(OBJ) $(DBDIR)
	sed -e '1s/^/[\n/' -e '$$s/,$$/\n]/' $(patsubst $(OBJDIR)/%.c.o,$(DBDIR)/%.c.o.json,$<) > $@

$(TARGET): $(OBJ) $(LIB)
	$(CXX) $(LDFLAGS) -shared -o $@ $^

ifdef ENABLE_GTK
$(GTKMLI): src/gtkmli.c $(TARGET) $(LIB)
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@.o $<
	$(CXX) $(LDFLAGS) -L./bin -lgtk-ml -o $@ $@.o $(LIB)
	rm $@.o
endif

ifdef ENABLE_POSIX
$(GTKMLDBG): src/gtkml-dbg.c $(TARGET) $(LIB)
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@.o $<
	$(CXX) $(LDFLAGS) -L./bin -lgtk-ml -o $@ $@.o $(LIB)
	rm $@.o
endif

$(GTKMLWEB): $(WEBDIR) $(SRC) $(SRCDIR)/gtkml-web.c
	$(EMCC) $(EMFLAGS) $(INCLUDE) -o $@ $(SRC) $(SRCDIR)/gtkml-web.c

$(WEBGL): bindgen.py include/GLES3/gl32.h
	./bindgen.py --skip-file gles3Skip -S glGetShaderInfoLog,glGetProgramInfoLog -k all -p gles3 --out libs/em_gles3 include/GLES3/gl32.h

$(OBJDIR): $(BINDIR)
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

$(DBDIR):
	mkdir -p $(DBDIR)

$(WEBDIR):
	mkdir -p $(WEBDIR)

clean:
	rm -rf bin/
	rm -rf public/
