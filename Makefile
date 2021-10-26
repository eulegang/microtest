CFLAGS=-O2 -g -Wall -Wextra -Isrc -DNDEBUG $(OPTFLAGS)
LIBS=-ldl $(OPTLIBS)

PREFIX?=/usr/local

SOURCES=$(wildcard src/*.c src/**/*.c)
OBJECTS=$(patsubst src/%.c,objs/%.o,$(SOURCES))
HEADERS=$(wildcard src/*.h src/**/*.h)
EXT_HEADERS=src/microunit.h

TARGET=build/microunit

TEST_SOURCES=$(wildcard test/*.c)
TEST_TARGETS=$(patsubst test/%.c,build/%.microunit,$(TEST_SOURCES))

all: $(TARGET) $(TEST_TARGETS)

dev: CFLAGS=-g -Wall -Wextra -Isrc $(OPTFLAGS)
dev: all

$(TARGET): CFLAGS += -rdynamic -Wl,-rpath=.
$(TARGET): build $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(CFLAGS) $(LIBS)


objs/lib%.o: CFLAGS += -fPIC
objs/lib%.o: test/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

objs/%.o: src/%.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

build/%.microunit: objs/lib%.o
	$(CC) -shared -o $@ $<

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
	install -t $(DESTDIR)/$(PREFIX)/share/man/man1/microunit.1 man/microunit.1

