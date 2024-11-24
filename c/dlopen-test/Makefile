CFLAGS := -Wall -Wextra

all: test

libfoo.h: foo.h
	./gen-lib-loader $< > $@

libfoo.so: foo.c
	$(CC) $(CFLAGS) -shared $< -o $@

test: main.c libfoo.h libfoo.so
	$(CC) $(CFLAGS) -ldl $< -o $@

.PHONY: clean

clean:
	rm -f libfoo.so libfoo.h test
