CC := gcc

all: build/hexdump_colorful

build/hexdump_colorful: hexdump_colorful.c
	mkdir -p build/
	${CC} hexdump_colorful.c -o $@

clean:
	rm -rf build/

