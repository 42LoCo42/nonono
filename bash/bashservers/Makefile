.PHONY: test build

build: crypt

test: crypt
	valgrind --leak-check=full --show-leak-kinds=all ./crypt -s

crypt: crypt.c
	gcc -g -o crypt -l crypt crypt.c
