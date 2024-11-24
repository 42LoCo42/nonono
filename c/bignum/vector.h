#ifndef _VECTOR_H
#define _VECTOR_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
	int8_t* data;
	size_t  size;
} vector;

int8_t* d      (vector* v,   size_t pos ); // get char at pos (NULL if out of bounds)
void    resizeV(vector* v,   size_t size); // resize vector
void    freeV  (vector* v               ); // release dynamic resources

#define forV(vec, body) for(size_t i = 0; i < (vec)->size; ++i) {int8_t* c = d(vec, i); body;};

#endif // _VECTOR_H
