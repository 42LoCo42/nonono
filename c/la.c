#include <stddef.h>

#define GEN(name, suffix) \
	GEN1(2, name, suffix) \
	GEN1(3, name, suffix) \
	GEN1(4, name, suffix) \

#define GEN1(x, name, suffix)                         \
	GEN2(V ## x ## suffix, v ## x ## suffix, x, name) \

#define GEN2(big, small, x, name)       \
	typedef struct { name c[x]; } big;  \
	GEN3(big, small, x, name, sum, +=)  \
	GEN3(big, small, x, name, sub, -=)  \
	GEN3(big, small, x, name, mul, *=)  \
	GEN3(big, small, x, name, div, /=)  \

#define GEN3(big, small, x, name, op, act) \
	big small ## _ ## op (big a, big b) {  \
		for(size_t i = 0; i < x; i++) {    \
			a.c[i] act b.c[i];             \
		}                                  \
		return a;                          \
	}                                      \

GEN(int,    n)
GEN(float,  f)
GEN(size_t, s)
