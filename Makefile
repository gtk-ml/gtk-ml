SRCDIR=src
INCDIR=include
BINDIR=bin
OBJDIR=$(BINDIR)/obj
DBDIR=$(BINDIR)/db
CC=clang

INCLUDE_NAME=gtk-ml
LIB_NAME=libgtk-ml.so
TARGET=$(BINDIR)/$(LIB_NAME)
TEST_HELLO=$(BINDIR)/hello 
TEST_MATCH=$(BINDIR)/match 
TESTS=$(TEST_HELLO) $(TEST_MATCH)
OBJ=$(OBJDIR)/gtk-ml.c.o $(OBJDIR)/value.c.o $(OBJDIR)/builder.c.o \
	$(OBJDIR)/lex.c.o $(OBJDIR)/parse.c.o $(OBJDIR)/code-gen.c.o \
	$(OBJDIR)/serf.c.o $(OBJDIR)/vm.c.o $(OBJDIR)/bytecode.c.o \
	$(OBJDIR)/hashtrie.c.o $(OBJDIR)/hashset.c.o $(OBJDIR)/array.c.o

CFLAGS:=-O0 -g -Wall -Wextra -Werror -pedantic -fPIC -std=c11 -pthread $(shell pkg-config --cflags gtk+-3.0)
LDFLAGS:=$(shell pkg-config --libs gtk+-3.0) -lm
INCLUDE:=-I$(INCDIR)

.PHONY: default all test install clean

default: all

all: $(TARGET) $(TESTS) compile_commands.json

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
	sed -e '1s/^/[\n/' -e '$$s/,$$/\n]/' $(patsubst $(OBJDIR)/%.c.o,$(DBDIR)/%.c.o.json,$<) > $@

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) -shared -o $@ $^ $(LIB)

$(TEST_HELLO): test/hello.c $(TARGET)
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE) -L./bin -lgtk-ml -o $@ $<

$(TEST_MATCH): test/match.c $(TARGET)
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE) -L./bin -lgtk-ml -o $@ $<

$(OBJDIR): $(BINDIR)
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

$(DBDIR): 
	mkdir -p $(DBDIR)

clean:
	rm -rf bin/

