CFLAGS := -Wall -Wextra -fPIC -shared -O2

pwdhack.so: pwdhack.c
	$(CC) $(CFLAGS) $^ -o $@

test: pwdhack.so
	rlwrap -c ./test
.PHONY: test
