TARGET := aarch64-linux-musl
# TARGET := x86_64-linux-musl
CFLAGS := -std=c11 -Wall -Wextra -pedantic -Wno-pointer-arith
LDFLAGS := -lsodium

main: main.c libzeoclient.a

libzeoclient.a: zeoclient.o
	ar rcs $@ $^

zeoclient.o: zeoclient.c cross.c
	zig cc \
		-target $(TARGET) \
		$(CFLAGS) \
		-I /usr/include/ \
		-fPIC -shared \
		$(LDFLAGS) \
		-L. \
		-c $^ \
		-o $@
