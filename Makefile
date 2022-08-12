CC := gcc

CFLAGS := -g -Wall -Wpedantic -Werror

all: build/hexdump_colorful

build/hexdump_colorful: src/hexdump_colorful.c
	mkdir -p build/
	${CC} ${CFLAGS} src/hexdump_colorful.c -o $@

test: all
	./build/hexdump_colorful Makefile

clean:
	rm -rf build/

.PHONY: clean test
