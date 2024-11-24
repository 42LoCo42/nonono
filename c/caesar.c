#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define die(s) perror(s); exit(1)

char caesar(char c, int radix) {
	if(c < ' ' || c > '~') {
		return c;
	}
	int tmp = c - ' ' + radix;
	tmp = tmp < 0 ? tmp + '~' - ' ' + 1 : tmp;
	tmp %= '~' - ' ' + 1;
	tmp += ' ';
	return tmp;
}

int main() {
	char* input = NULL;
	size_t buflen = 0;
	ssize_t rdlen = 0;

	// read radix
	printf("Radix: ");
	if((rdlen = getline(&input, &buflen, stdin)) < 0) {
		free(input);
		die("getline");
	}
	input[rdlen - 1] = 0; // remove newline from input

	// convert radix
	char* invalid = NULL;
	int radix = (int) strtol(input, &invalid, 10);
	if(*invalid != 0) {
		fprintf(stderr, "Error at %s\n", invalid);
		free(input);
		exit(1);
	}

	// read text
	printf("Input: ");
	if((rdlen = getline(&input, &buflen, stdin)) < 0) {
		free(input);
		die("getline");
	}

	for(size_t i = 0; i < (size_t) rdlen; ++i) {
		input[i] = caesar(input[i], radix);
	}
	printf("%s", input);
	free(input);
}
