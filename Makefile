CC := gcc

all: build/hexdump_colorful

build/hexdump_colorful: src/hexdump_colorful.c
	mkdir -p build/
	${CC} src/hexdump_colorful.c -o $@

test: all
	./build/hexdump_colorful Makefile

clean:
	rm -rf build/

.PHONY: clean test
