CFLAGS := -Wall -Wextra -g
LDFLAGS :=

%.so: %.c
	$(CC) $(CFLAGS) -fPIC -shared $^ -o $@

main: main.c
