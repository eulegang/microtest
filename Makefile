CFLAGS=-O2 -g -Wall -Wextra -Isrc -DNDEBUG $(OPTFLAGS)
LIBS=-ldl $(OPTLIBS)

PREFIX?=/usr/local

SOURCES=$(wildcard src/*.c src/**/*.c)
OBJECTS=$(patsubst src/%.c,objs/%.o,$(SOURCES))
HEADERS=$(wildcard src/*.h src/**/*.h)
EXT_HEADERS=src/microunit.h

TARGET=build/microtest

TEST_TARGETS=build/succ.microtest build/fail.microtest

all: $(TARGET) $(TEST_TARGETS)

dev: CFLAGS=-g -Wall -Wextra -Isrc $(OPTFLAGS)
dev: all

$(TARGET): CFLAGS += -rdynamic
$(TARGET): build $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) -rdynamic -Wl,-rpath=. $(LIBS)

objs/lib%.o: test/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

objs/%.o: src/%.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

build/%.microtest: objs/lib%.o
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

