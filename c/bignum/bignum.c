#include "bignum.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TODO 0

// PRIVATE API

typedef struct {
	bignum* hi;
	bignum* lo;
} bignumPair;

int  digitCount(int n);
void resizeBignum(bignum* b, size_t size);
void extendLSP(bignum* b, size_t size);
void extendMSP(bignum* b, size_t size);
void matchSizes(bignum* a, bignum* b);
void shrink(bignum* b);
bignumPair split(bignum* b, size_t pos);
bignum* divHelper(bignum* b);

int digitCount(int n) {
	n = abs(n);
	return
		n < 10 ? 1 :
		n < 100 ? 2:
		n < 1000 ? 3:
		n < 10000 ? 4:
		n < 100000 ? 5:
		n < 1000000 ? 6:
		n < 10000000 ? 7:
		n < 100000000 ? 8:
		n < 1000000000 ? 9:
		10;
}

void resizeBignum(bignum* b, size_t size) {
	assert(b != NULL);

	size_t old = bignumLen(b);
	resizeV(&b->digits, size);
	if(bignumLen(b) > old) memset(b->digits.data + old, 0, size - old);
}

void extendLSP(bignum* b, size_t size) {
	assert(b != NULL);

	ssize_t delta = size - bignumLSP(b);
	if(delta <= 0) return;
	shlBignum(b, delta);
	b->point = size;
}

void extendMSP(bignum* b, size_t size) {
	assert(b != NULL);

	resizeBignum(b, bignumLSP(b) + size);
}

void matchSizes(bignum* a, bignum* b) {
	assert(a != NULL && b != NULL);

	// match LSP
	int delta_lsp = bignumLSP(a) - bignumLSP(b);
	if     (delta_lsp > 0) extendLSP(b, bignumLSP(a));
	else if(delta_lsp < 0) extendLSP(a, bignumLSP(b));

	// match MSP
	int delta_msp = bignumMSP(a) - bignumMSP(b);
	if     (delta_msp > 0) extendMSP(b, bignumMSP(a));
	else if(delta_msp < 0) extendMSP(a, bignumMSP(b));

	assert(bignumLen(a) == bignumLen(b));
}

void shrink(bignum* b) {
	assert(b != NULL);

	size_t old = bignumLen(b);
	size_t lsp_zeroes = 0;
	size_t msp_zeroes = 0;

	for(size_t i = 0; i < bignumLSP(b); ++i) {
		if(*d(&b->digits, i) == 0) lsp_zeroes++;
		else break;
	}

	for(size_t i = old - 1; i > b->point; --i) {
		if(*d(&b->digits, i) == 0) msp_zeroes++;
		else break;
	}

	size_t new = old - lsp_zeroes - msp_zeroes;

	if(new == 0) {
		// all digits are zero
		resizeBignum(b, 1);
		*d(&b->digits, 0) = 0;
		b->point = 0;
	} else {
		memmove(b->digits.data, b->digits.data + lsp_zeroes, new);
		resizeBignum(b, new);
		b->point -= lsp_zeroes;
	}
}

bignumPair split(bignum* b, size_t loLen) {
	assert(b != NULL);

	size_t len = bignumLen(b);
	bignumPair result = {};

	if(loLen > len - 1) {
		result.hi = newBignum(1);
		result.lo = copyBignum(b);
	} else {
		result.hi = newBignum(len - loLen);
		result.lo = newBignum(loLen);

		memcpy(result.hi->digits.data, b->digits.data + loLen, bignumLen(result.hi));
		memcpy(result.lo->digits.data, b->digits.data, bignumLen(result.lo));

		shrink(result.hi);
		shrink(result.lo);
	}

	return result;
}

bignum* divHelper(bignum* b) {
	assert(b != NULL);

	size_t msp = bignumMSP(b);

	bignum* result = newBignum(msp + 1);
	setPoint(result, msp);
	*d(&result->digits, 0) = 1;

	return result;
}

// PUBLIC API

char ordChar(ord o) {
	switch(o) {
		case LT: return '<';
		case EQ: return '=';
		case GT: return '>';
		default: return '?';
	}
}

bignum* newBignum (size_t size) {
	bignum* result = malloc(sizeof(bignum));
	assert(result != NULL);

	memset(result, 0, sizeof(bignum));
	resizeBignum(result, size);
	memset(result->digits.data, 0, result->digits.size);
	result->sign = PLUS;
	result->point = 0;

	return result;
}

bignum* newBignumOf(int n) {
	bignum* result = newBignum(digitCount(n));

	if(n < 0) {
		result->sign = MINUS;
		n = abs(n);
	}

	forV(&result->digits,
		*c = n % 10;
		n /= 10;
	);

	return result;
}

bignum* copyBignum(bignum* b) {
	assert(b != NULL);

	bignum* result = newBignum(bignumLen(b));
	result->sign = b->sign;
	result->point = b->point;
	memcpy(result->digits.data, b->digits.data, bignumLen(b));
	return result;
}

void freeBignum (bignum* b) {
	assert(b != NULL);

	freeV(&b->digits);
	free(b);
}

char* bignumToStr(bignum* b) {
	assert(b != NULL);

	size_t r_len = bignumLen(b) + 3; // 3 for - . \0
	char* result = malloc(r_len);
	assert(result != NULL);

	size_t d_pos = r_len - 4; // r_len is length + 3 -> -4 = -1 -> most significant digit

	for(size_t r_pos = 0; r_pos < r_len; ++r_pos) {
		result[r_pos] =
			r_pos == 0 && b->sign == MINUS   ? '-'
			: r_pos > 0
				&& result[r_pos - 1] != '.'
				&& d_pos == bignumLSP(b) - 1 ? '.'
			: d_pos < bignumLen(b)           ? *d(&b->digits, d_pos--) + '0'
			: 0;
	}

	return result;
}

void printBignum(bignum* b) {
	assert(b != NULL);
	withBS(b, s, printf("%s\n", s));
}

size_t bignumLen(bignum* b) {
	assert(b != NULL);
	return b->digits.size;
}

size_t bignumLSP(bignum* b) {
	assert(b != NULL);
	return b->point;
}

size_t bignumMSP(bignum* b) {
	assert(b != NULL);
	return bignumLen(b) - bignumLSP(b);
}

void setPoint(bignum* b, size_t point) {
	assert(b != NULL);

	if(point >= bignumLen(b)) extendMSP(b, point + 1);
	b->point = point;
}

bignum* absorbA(bignum_2f func, bignum* a, bignum* b) {
	bignum* result = func(a, b);
	freeBignum(a);
	return result;
}

bignum* absorbB(bignum_2f func, bignum* a, bignum* b) {
	bignum* result = func(a, b);
	freeBignum(b);
	return result;
}

bignum* absorb2(bignum_2f func, bignum* a, bignum* b) {
	bignum* result = absorbA(func, a, b);
	freeBignum(b);
	return result;
}

ord compareBignum (bignum* a, bignum* b) {
	assert(a != NULL && b != NULL);

	int signdiff = a->sign - b->sign;
	return
		signdiff < 0   ? LT
		: signdiff > 0 ? GT
		: compareAbsBignum(a, b) * a->sign;
}

ord compareAbsBignum(bignum* a, bignum* b) {
	assert(a != NULL && b != NULL);

	matchSizes(a, b);

	for(size_t i = bignumLen(a) - 1;; --i) {
		int delta = *d(&a->digits, i) - *d(&b->digits, i);
		if(delta < 0) return LT;
		if(delta > 0) return GT;
		if(i == 0)    return EQ;
	}
}

bignum* addBignum(bignum* a, bignum* b) {
	assert(a != NULL && b != NULL);

	matchSizes(a, b);

	if(a->sign != b->sign) {
		switch(compareAbsBignum(a, b)) {
			case EQ: if(a->sign == PLUS) break; // FALLTHROUGH
			case LT: return addBignum(b, a);
			case GT: if(a->sign == MINUS) {
				a->sign *= MINUS;
				b->sign *= MINUS;
				bignum* result = addBignum(a, b);
				a->sign *= MINUS;
				b->sign *= MINUS;
				result->sign *= MINUS;
				return result;
			}
		}
	}

	bignum* result = copyBignum(a);
	resizeBignum(result, bignumLen(a) + 1);

	int carry = 0;
	forV(&a->digits,
		int8_t* r = d(&result->digits, i);
		*r = *c + *d(&b->digits, i) * a->sign * b->sign + carry;
		if(*r < 0) {
			carry = -1;
			*r += 10;
		} else {
			carry = *r / 10;
			*r %= 10;
		}
	);
	*d(&result->digits, bignumLen(a)) = carry;

	shrink(a);
	shrink(b);
	shrink(result);
	return result;
}

bignum* subBignum(bignum* a, bignum* b) {
	assert(a != NULL && b != NULL);

	b->sign *= MINUS;
	bignum* result = addBignum(a, b);
	b->sign *= MINUS;
	return result;
}

bignum* mulBignum(bignum* a, bignum* b) {
	assert(a != NULL && b != NULL);

	if(bignumLen(a) < 2 && bignumLen(b) < 2) {
		return newBignumOf(*d(&a->digits, 0) * *d(&b->digits, 0));
	}

	bignum* result = NULL;

	matchSizes(a, b);
	size_t m = bignumLen(a) >> 1; // where to split
	bignumPair ap = split(a, m);
	bignumPair bp = split(b, m);

	withB(z0, mulBignum(ap.lo, bp.lo),
	withB(z1, absorb2(mulBignum, addBignum(ap.lo, ap.hi), addBignum(bp.lo, bp.hi)),
	withB(z2, mulBignum(ap.hi, bp.hi),
		result = absorbA(addBignum,
			absorb2(addBignum,
				shlBignum(copyBignum(z2), m << 1),
				shlBignum(absorbA(subBignum, subBignum(z1, z2), z0), m)
			),
			z0
		);
		result->sign  = a->sign  * b->sign;
		result->point = a->point + b->point;
		setPoint(result, result->point);

		shrink(a);
		shrink(b);
		shrink(result);
	)));

	freeBignum(ap.hi);
	freeBignum(ap.lo);
	freeBignum(bp.hi);
	freeBignum(bp.lo);
	return result;
}

bignum* divBignum(bignum* a, bignum* b) {
	assert(a != NULL && b != NULL);

	int sign_a = a->sign;
	int sign_b = b->sign;
	a->sign = PLUS;
	b->sign = PLUS;

	int point = bignumLSP(b);
	shlBignum(a, point);
	b->point = 0;
	shrink(b);

	bignum* result = NULL;

	withB(q, divHelper(b),
	withB(two, newBignumOf(2),
		for(int i = 0; i < 10; ++i) {
			q = absorbA(mulBignum,
				absorbB(subBignum,
					two,
					mulBignum(q, b)
				),
				q
			);
		}
		result = mulBignum(a, q);
	));

	a->sign = sign_a;
	b->sign = sign_b;
	result->sign = sign_a * sign_b;
	b->point = point;
	return result;
}

bignum* shlBignum(bignum* b, size_t s) {
	assert(b != NULL);

	size_t old = bignumLen(b);
	resizeBignum(b, old + s);
	memmove(b->digits.data + s, b->digits.data, old);
	memset(b->digits.data, 0, s);
	return b;
}

bignum* shrBignum(bignum* b, size_t s) {
	assert(b != NULL);

	size_t new = bignumLen(b) - s;
	if(new <= 0) return b;
	memmove(b->digits.data, b->digits.data + s, new);
	resizeBignum(b, new);
	return b;
}

// BINARY SPLITTING

bignum* calculateLoop(bignum_1f func, bignum** bs, size_t start, size_t end) {
	bignum* result = newBignumOf(1);

	for(size_t i = start; i < end; ++i) {
		result = absorb2(mulBignum, result, func(bs[i]));
	}

	return result;
}

bignum* bsP(bignum* b) {
	bignum* result = NULL;
	withB(one, newBignumOf(1),
		withB(two, newBignumOf(2),
			result = absorbA(subBignum, mulBignum(b, two), one);
	));
	return result;
}

bignum* bsQ(bignum* b) {
	bignum* result = NULL;
	withB(four, newBignumOf(4),
		result = mulBignum(b, four);
	);
	return result;
}

bignum* calculateT(bignum** bs, size_t start, size_t end) {
	if(start == end - 1) {
		return bsP(bs[start]);
	}

	size_t m = (end + start) >> 1;
	bignum* result = absorb2(addBignum,
		absorb2(mulBignum, calculateLoop(bsQ, bs, m,     end), calculateT(bs, start, m  )),
		absorb2(mulBignum, calculateLoop(bsP, bs, start, m  ), calculateT(bs, m,     end))
	);

	return result;
}
