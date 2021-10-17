CFLAGS=-O2 -g -Wall -Wextra -Isrc -DNDEBUG $(OPTFLAGS)
LIBS=$(OPTLIBS)

PREFIX?=/usr/local

SOURCES=$(wildcard src/*.c src/**/*.c)
OBJECTS=$(patsubst src/%.c,objs/%.o,$(SOURCES))
HEADERS=$(wildcard src/*.h src/**/*.h)
EXT_HEADERS=src/microunit.h

TARGET=build/microtest

TEST_SOURCES=$(wildcard tests/*_tests.c)
TEST_TARGETS=$(patsubst tests/%.c,suite/%,$(TEST_SOURCES))

all: $(TARGET)

dev: CFLAGS=-g -Wall -Wextra -Isrc $(OPTFLAGS)
dev: all

$(TARGET): CFLAGS += -rdynamic
$(TARGET): build $(OBJECTS)
	gcc -o $@ $(OBJECTS) -rdynamic

objs/%.o: src/%.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

build:
	@mkdir -p build
	@mkdir -p objs

.PHONY: clean
clean:
	rm -rf build/*
	rm -rf objs/*

install:
	install -d $(DESTDIR)/$(PREFIX)/include
	install -t $(DESTDIR)/$(PREFIX)/include -m 0755 $(EXT_HEADERS)
	install -d $(DESTDIR)/$(PREFIX)/lib
	install -t $(DESTDIR)/$(PREFIX)/lib $(TARGET)

