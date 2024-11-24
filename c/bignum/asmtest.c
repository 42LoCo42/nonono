#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define breakpoint asm("int $3");

typedef void* bignum;

bignum copyBignum(bignum b);
bignum add_bignum(bignum a, bignum b);
bignum mult(bignum a, bignum b);

bignum newBignum(size_t len);
void   freeBignum(bignum b);

int* sign(bignum b) {
	return b;
}

size_t* point(bignum b) {
	return b + sizeof(int);
}

int8_t** data(bignum b) {
	return b + sizeof(int) + sizeof(size_t);
}

size_t* size(bignum b) {
	return b + sizeof(int) + sizeof(size_t) + sizeof(int8_t*);
}

int8_t* d(bignum b, size_t pos) {
	return *data(b) + pos;
}

void printBignum(bignum b) {
	if(*sign(b) == -1) putchar('-');
	for(size_t i = *size(b); i > 0; --i) {
		printf("%d", *d(b, i - 1));
	}
	printf("\n");
}

int main() {
	bignum a = newBignum(2);
	bignum b = newBignum(1);
	*d(a, 0) = 6;
	*d(a, 1) = 1;
	*d(b, 0) = 2;
	// *d(b, 1) = 9;
	*sign(a) = -1;
	*sign(b) = 1;

	printBignum(a);
	printBignum(b);

	bignum c = mult(a, b);
	printf("%d\n", c);

	// printBignum(c);

	freeBignum(a);
	freeBignum(b);
	// freeBignum(c);
}
