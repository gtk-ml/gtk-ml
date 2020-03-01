SRCDIR=src
INCDIR=include
BINDIR=bin
OBJDIR=$(BINDIR)/obj
DBDIR=$(BINDIR)/db
CC=clang

INCLUDE_NAME=gtk-ml
LIB_NAME=libgtk-ml.so
TARGET=$(BINDIR)/$(LIB_NAME)
TEST=$(BINDIR)/hello
OBJ=$(OBJDIR)/gtk-ml.c.o

CFLAGS:=-O0 -g -Wall -Wextra -Werror -pedantic -fPIC -std=c11 -pthread $(shell pkg-config --cflags gtk+-3.0)
LDFLAGS:=$(shell pkg-config --libs gtk+-3.0)
INCLUDE:=-I$(INCDIR)

.PHONY: default all test install clean

default: all

all: $(TARGET) $(TEST) compile_commands.json

test: $(TEST)

install: $(TARGET)
	rm -rf ~/.local/include/$(INCLUDE_NAME)
	mkdir -p ~/.local/include
	mkdir -p ~/.local/lib64
	cp -r include ~/.local/include/$(INCLUDE_NAME)
	cp $(TARGET) ~/.local/lib64/

$(OBJDIR)/%.c.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDE) -MJ $(DBDIR)/$*.c.o.json -c -o $@ $<

compile_commands.json: $(OBJ)
	sed -e '1s/^/[\n/' -e '$$s/,$$/\n]/' $(patsubst $(OBJDIR)/%.c.o,$(DBDIR)/%.c.o.json,$^) > $@

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) -shared -o $@ $^ $(LIB)

$(TEST): test/hello.c $(TARGET)
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE) -L./bin -lgtk-ml -o $@ $<

$(OBJDIR): $(BINDIR)
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

clean:
	rm -rf bin/

