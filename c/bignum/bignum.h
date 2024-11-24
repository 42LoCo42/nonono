#ifndef _BIGNUM_H
#define _BIGNUM_H

#include <stddef.h>

#include "utils.h"
#include "vector.h"

#define PLUS   1
#define MINUS -1

// run body with automatic management of a bignum
#define withB(sym, gen, body) with(bignum*, sym, gen, body, freeBignum(sym));

// run body with automatic management of a bignum string
#define withBS(b, sym, body) withF(char*, sym, bignumToStr(b), body);

typedef struct {
	int    sign;
	size_t point;
	vector digits;
} bignum;

typedef enum {
	LT = -1, EQ = 0, GT = 1
} ord;

typedef bignum* (*bignum_1f)(bignum*);
typedef bignum* (*bignum_2f)(bignum*, bignum*);

// Utilities

char ordChar(ord o); // return the character representation of o

bignum* newBignum  (size_t  size); // create a bignum with the specified size, set to 0
bignum* newBignumOf(int     n   ); // create a bignum of a specified int
bignum* copyBignum (bignum* b   ); // return a copy of b
void    freeBignum (bignum* b   ); // release dynamic resources

char*   bignumToStr(bignum* b   ); // return a dynamically allocated string representing this bignum
void    printBignum(bignum* b   ); // print the bignum directly to stdout

size_t  bignumLen(bignum* b); // return the total length = digit count
size_t  bignumLSP(bignum* b); // return the number of digits after point
size_t  bignumMSP(bignum* b); // return the number of digits before point

void    setPoint(bignum* b, size_t point); // set the point of a bignum

bignum* absorbA(bignum_2f func, bignum* a, bignum* b); // return func(a, b) with a freed
bignum* absorbB(bignum_2f func, bignum* a, bignum* b); // return func(a, b) with b freed
bignum* absorb2(bignum_2f func, bignum* a, bignum* b); // return func(a, b) with a and b freed

// Arithmetic

ord compareBignum   (bignum* a, bignum* b); // returns: a < b = LT, a = b = EQ, a > b = GT
ord compareAbsBignum(bignum* a, bignum* b); // like compareBignum, but absolute values are used

bignum* addBignum(bignum* a, bignum* b); // add two bignums
bignum* subBignum(bignum* a, bignum* b); // subtract two bignums
bignum* mulBignum(bignum* a, bignum* b); // multiply two bignums
bignum* divBignum(bignum* a, bignum* b); // divide two bignums

bignum* shlBignum(bignum* b, size_t s ); // b *= 10^s
bignum* shrBignum(bignum* b, size_t s ); // b /= 10^s

// Binary splitting

bignum* calculateLoop(bignum_1f func, bignum** bs, size_t start, size_t end);
bignum* bsP(bignum* b);
bignum* bsQ(bignum* b);
bignum* calculateT(bignum** bs, size_t start, size_t end);

#endif // _BIGNUM_H
