CFLAGS := -Wall -Wextra -fPIC -shared -g
LDFLAGS := -ldl

main.so: main.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@
