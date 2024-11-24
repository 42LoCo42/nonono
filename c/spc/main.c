#include <math.h>
#include <stdio.h>
#include <readline/readline.h>
#include <stdlib.h>
#include <string.h>

#define stop {run = 0; goto end;}

char numToDigit(int num) {
	if(num < 10) {
		return num + '0';
	} else {
		return num + 'a' - 10;
	}
}

char* toBase(long num, long base) {
	if(num == 0) {
		return "0";
	}

	size_t i = floor(log(num) / log(base)) + 2; // 1 char more to store \0
	char* result = malloc(i);
	result[i - 1] = 0;
	i -= 2; // last real index
	long qut = 0;
	long rst = 0;
	while(num > 0) {
		qut = num / base;
		rst = num % base;
		result[i] = numToDigit(rst);
		--i;
		num = qut;
	}
	return result;
}

int main() {
	printf("Simple Programmer's Converter v1.2\n");
	printf("Developed by Leon Schumacher\n");
	printf("Prefixes: [b]inary, [o]ctal, he[x]adecimal\n");

	int run = 1;
	while(run) {
		char* line = NULL;
		char* b2   = NULL;
		char* b8   = NULL;
		char* b16  = NULL;

		size_t len = 0;

		if((line = readline("> ")) == NULL) stop
		len = strlen(line);
		if(len < 1) stop

		int base;
		switch(line[0]) {
			case 'b': base =  2; ++line; break;
			case 'o': base =  8; ++line; break;
			case 'x': base = 16; ++line; break;
			default:  base = 10; break;
		}


		long b10 = strtol(line, NULL, base);
		b2  = toBase(b10, 2);
		b8  = toBase(b10, 8);
		b16 = toBase(b10, 16);

		printf("Binary:      %s\n",  b2);
		printf("Octal:       %s\n",  b8);
		printf("Decimal:     %ld\n", b10);
		printf("Hexadecimal: %s\n",  b16);

		int nextBinarySize = pow(2, ceil(log(strlen(b2)) / log(2)));
		if(nextBinarySize < 8) {
			nextBinarySize = 8;
		}
		printf("As signed %d bit decimal: ", nextBinarySize);
		switch(nextBinarySize) {
			case  8: printf( "%d\n", (int8_t ) b10); break;
			case 16: printf( "%d\n", (int16_t) b10); break;
			case 32: printf( "%d\n", (int32_t) b10); break;
			case 64: printf("%ld\n", (int64_t) b10); break;
		}

end:
		if(line != NULL) free(line - (base == 10 ? 0 : 1));
		if(b2   != NULL) free(b2);
		if(b8   != NULL) free(b8);
		if(b16  != NULL) free(b16);
	}
}
