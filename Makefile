CC=clang
CFLAGS=-g -std=c11 -O3 -Wall -Werror -Isrc -DNDEBUG $(OPTFLAGS)
LIBS=$(OPTLIBS)
PREFIX?=/usr/local

SOURCES=$(wildcard src/**/*.c src/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

TEST_SRC=$(wildcard tests/*_tests.c)
TESTS=$(patsubst %.c,%,$(TEST_SRC))

PROGRAMS_SRC=$(wildcard bin/*.c)
PROGRAMS=$(patsubst %.c,%,$(PROGRAMS_SRC))

TARGET=build/libterror.a
SO_TARGET=$(patsubst %.a,%.so,$(TARGET))


# The Target Build
all: $(TARGET) $(SO_TARGET) tests $(PROGRAMS)

dev: CFLAGS=-g -std=c11 -Wall -Isrc -Wall -Werror $(OPTFLAGS)
dev: all

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
tests: CFLAGS += $(TARGET)
tests: $(TESTS)
				sh ./tests/runtests.sh

valgrind:
				VALGRIND="valgrind --log-file=/tmp/valgrind-%p.log" $(MAKE)

# The Cleaner
clean:
				rm -rf build $(OBJECTS) $(TESTS) $(PROGRAMS)
				rm -f tests/tests.log
				find . -name "*.gc*" -exec rm {} \;
				rm -rf `find . -name "*.dSYM" -print`

# The Install
install: all
				install -d $(DESTDIR)/$(PREFIX)/lib/
				install $(TARGET) $(DESTDIR)/$(PREFIX)/lib/

# The Checker
BADFUNCS='[^_.>a-zA-Z0-9](str(n?cpy|n?cat|xfrm|n?dup|str|pbrk|tok|_)|stpn?cpy|a?sn?printf|byte_)'
check:
				@echo Files with potentially dangerous functions.
				@egrep $(BADFUNCS) $(SOURCES) || true
