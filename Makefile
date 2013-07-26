CC=clang
CFLAGS=-g -std=c99 -O3 -Wall -Werror -Isrc -DNDEBUG -Ideps/libsweeper/include $(OPTFLAGS)
LIBS=$(OPTLIBS)
LDFLAGS=deps/libsweeper/build/libsweeper.a

PREFIX?=/usr/local

DEPS=gc

SOURCES=$(wildcard src/**/*.c src/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

TEST_SRC=$(wildcard tests/*_tests.c)
TESTS=$(patsubst %.c,%,$(TEST_SRC))

PROGRAMS_SRC=$(wildcard bin/*.c)
PROGRAMS=$(patsubst %.c,%,$(PROGRAMS_SRC))

TARGET=build/libterror.a
SO_TARGET=$(patsubst %.a,%.so,$(TARGET))

# The Target Build
all: $(DEPS) $(TARGET) tests $(PROGRAMS)

dev: CFLAGS=-g -std=c99 -Wall -Isrc -Wall -Werror -Ideps/libsweeper/include $(OPTFLAGS)
dev: all

deps/libsweeper/Makefile:
	git submodule init && git submodule update

gc: deps/libsweeper/Makefile
	$(MAKE) -C deps/libsweeper

gc_debug: deps/libsweeper/Makefile
	$(MAKE) dev -C deps/libsweeper

rubinius:
	which rbx || (echo '\nYou need rubinius to compile the kernel.\n'; which rbx)
kernel: rubinius
	rm -rf `find kernel -name "*.tvm" -print`
	cd compiler && rake kernel
examples: rubinius
	rm -rf `find examples -name "*.tvm" -print`
	cd compiler && rake examples
lib: rubinius
	rm -rf `find lib -name "*.tvm" -print`
	cd compiler && rake stdlib

$(TARGET): CFLAGS += -fPIC -fblocks $(LIBS)
$(TARGET): build $(OBJECTS)
				ar rcs $@ $(OBJECTS)
				ranlib $@

$(PROGRAMS): CFLAGS += $(TARGET)

$(SO_TARGET): $(TARGET) $(OBJECTS)
				$(CC) -shared -o $@ $(OBJECTS)

build:
				@mkdir -p build
				@mkdir -p bin

# The Unit Tests
.PHONY: tests kernel rubinius
$(TESTS): tests/%: tests/%.c
				$(CC) $(CFLAGS) $< -o $@ $(TARGET) $(LDFLAGS)
tests: $(TESTS)
				sh ./tests/runtests.sh

valgrind:
				VALGRIND="valgrind" $(MAKE)

# The Cleaner
clean:
				rm -rf build $(OBJECTS) $(TESTS) $(PROGRAMS)
				rm -f tests/tests.log
				find . -name "*.gc*" -exec rm {} \;
				rm -rf `find . -name "*.dSYM" -print`
				$(MAKE) clean -C deps/libsweeper

# The Install
install: all
				install -d $(DESTDIR)/$(PREFIX)/lib/
				install $(TARGET) $(DESTDIR)/$(PREFIX)/lib/

# The Checker
BADFUNCS='[^_.>a-zA-Z0-9](str(n?cpy|n?cat|xfrm|n?dup|str|pbrk|tok|_)|stpn?cpy|a?sn?printf|byte_)'
check:
				@echo Files with potentially dangerous functions.
				@egrep $(BADFUNCS) $(SOURCES) || true

