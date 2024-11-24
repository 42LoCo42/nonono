#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "bignum.h"

bignum* sqrt2(size_t n) {
	bignum** storage = malloc(sizeof(bignum*) * n);
	for(size_t i = 0; i < n; ++i) {
		storage[i] = newBignumOf(i + 1);
	}

	bignum* result = absorb2(divBignum,
		calculateT(storage, 0, n),
		calculateLoop(bsQ, storage, 0, n)
	);

	for(size_t i = 0; i < n; ++i) {
		freeBignum(storage[i]);
	}

	return result;
}

int main() {
	srand(clock());

	for(size_t i = 0; i < 10; ++i) {
		double r1 = rand() % 10000 - 5000;
		double r2 = rand() % 10000 - 5000;
		withB(a, newBignumOf(r1),
			withB(b, newBignumOf(r2),
				setPoint(a, 2);
				setPoint(b, 4);
				r1 /= 100;
				r2 /= 10000;
				withB(c, mulBignum(a, b),
					withBS(c, s, printf("%f * %f = %f = %s\n", r1, r2, r1 * r2, s))
				);
			);
		);
	}

	// withB(result, sqrt2(10), printBignum(result));

	return 0;
}
