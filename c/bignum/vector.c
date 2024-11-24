#include "vector.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

int8_t* d(vector* v, size_t pos) {
	assert(v != NULL);
	assert(v->data != NULL);

	return
		pos < v->size
		? &v->data[pos] : NULL;
}

void resizeV(vector* v, size_t size) {
	assert(v != NULL);

	v->data = realloc(v->data, size);
	v->size = size;

	assert(v->data != NULL);
}

void freeV(vector* v) {
	assert(v != NULL);

	free(v->data);
	v->data = NULL;
}
