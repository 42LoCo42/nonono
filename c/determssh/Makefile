CFLAGS  := -Wall -Wextra -O3

all: main determssh.so

main: main.c

determssh.so: determssh.c
	$(CC) $(CFLAGS) -fPIC $(shell pkg-config --cflags --libs libsodium) -shared $< -o $@
