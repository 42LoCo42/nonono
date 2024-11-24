CFLAGS  := $(CFLAGS) -Wall -Wextra -g
LDFLAGS := $(LDFLAGS)

SRCS := $(wildcard *.c)
# HDRS := $(wildcard *.h)

all: main asmtest

%: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

main: main.c bignum.c vector.c

asmtest: asmtest.c libmult.so
	$(CC) $(CFLAGS) $(LDFLAGS) -L. -lmult -Wl,-rpath,. $< -o $@

libmult.so: mult.S
	$(CC) $(CFLAGS) -fPIC -shared $< -o $@

clean:
	rm -f main

run: main
	./$<

.PHONY: all clean run
