SRCDIR=src
INCDIR=include
BINDIR=bin
OBJDIR=$(BINDIR)/obj
DBDIR=$(BINDIR)/db
CC=clang
CXX=clang++

INCLUDE_NAME=gtk-ml
LIB_NAME=libgtk-ml.so
TARGET=$(BINDIR)/$(LIB_NAME)
GTKMLI=$(BINDIR)/gtkmli
TEST_HELLO=$(BINDIR)/hello
BINARIES=$(GTKMLI)
TESTS=$(TEST_HELLO)
OBJ=$(OBJDIR)/gtk-ml.c.o $(OBJDIR)/value.c.o $(OBJDIR)/builder.c.o \
	$(OBJDIR)/lex.c.o $(OBJDIR)/parse.c.o $(OBJDIR)/code-gen.c.o \
	$(OBJDIR)/serf.c.o $(OBJDIR)/vm.c.o $(OBJDIR)/bytecode.c.o \
	$(OBJDIR)/hashtrie.c.o $(OBJDIR)/hashset.c.o $(OBJDIR)/array.c.o
LIB=/usr/local/lib/liblinenoise.a

CFLAGS:=-O0 -g -Wall -Wextra -Werror -pedantic -fPIC -std=c11 -pthread $(shell pkg-config --cflags gtk+-3.0)
LDFLAGS:=$(shell pkg-config --libs gtk+-3.0) -lm
INCLUDE:=-I$(INCDIR) -I/usr/local/include

# posix allows us to use the debugger
ifdef ENABLE_POSIX
CFLAGS+=-DGTKML_ENABLE_POSIX=1

GTKMLDBG=$(BINDIR)/gtkml-dbg
BINARIES+=$(GTKMLDBG)
endif

.PHONY: default all build test install clean

default: all

all: $(TARGET) $(BINARIES) $(TESTS) compile_commands.json

build: $(BINARIES)

test: $(TESTS)

install: $(TARGET)
	rm -rf ~/.local/include/$(INCLUDE_NAME)
	mkdir -p ~/.local/include
	mkdir -p ~/.local/lib64
	cp -r include ~/.local/include/$(INCLUDE_NAME)
	cp $(TARGET) ~/.local/lib64/

$(OBJDIR)/%.c.o: $(SRCDIR)/%.c $(OBJDIR) $(DBDIR)
	$(CC) $(CFLAGS) $(INCLUDE) -MJ $(DBDIR)/$*.c.o.json -c -o $@ $<

compile_commands.json: $(OBJ) $(DBDIR)
	sed -e '1s/^/[\n/' -e '$$s/,$$/\n]/' $(patsubst $(OBJDIR)/%.c.o,$(DBDIR)/%.c.o.json,$^) > $@

$(TARGET): $(OBJ) $(LIB)
	$(CXX) $(LDFLAGS) -shared -o $@ $^

$(GTKMLI): src/gtkmli.c $(TARGET) $(LIB)
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@.o $<
	$(CXX) $(LDFLAGS) -L./bin -lgtk-ml -o $@ $@.o $(LIB)
	rm $@.o

ifdef ENABLE_POSIX
$(GTKMLDBG): src/gtkml-dbg.c $(TARGET) $(LIB)
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@.o $<
	$(CXX) $(LDFLAGS) -L./bin -lgtk-ml -o $@ $@.o $(LIB)
	rm $@.o
endif

$(TEST_HELLO): test/hello.c $(TARGET) $(LIB)
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@.o $<
	$(CXX) $(LDFLAGS) -L./bin -lgtk-ml -o $@ $@.o $(LIB)
	rm $@.o

$(OBJDIR): $(BINDIR)
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

$(DBDIR):
	mkdir -p $(DBDIR)

clean:
	rm -rf bin/

