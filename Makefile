CC := gcc

all: build/hexdump_colorful

build/hexdump_colorful: src/hexdump_colorful.c
	mkdir -p build/
	${CC} src/hexdump_colorful.c -o $@

clean:
	rm -rf build/
