#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VECTOR_INIT_CAP 16
#define VECTOR_GROWTH 1.5

typedef struct {
	char* items;
	size_t len;
	size_t cap;
} Vector;

void Vector_grow(Vector* v) {
	size_t old_cap = v->cap;
	v->cap = v->cap == 0 ? VECTOR_INIT_CAP : v->cap * VECTOR_GROWTH;
	v->items = realloc(v->items, v->cap);
	printf("growing vector: %zu -> %zu\n", old_cap, v->cap);
}

void* Vector_push(Vector* v, size_t size) {
	printf("pushing %zu to vector\n", size);
	while(v->len + size > v->cap) {
		Vector_grow(v);
	}

	void* ptr = &v->items[v->len];
	v->len += size;
	return ptr;
}

void Vector_free(Vector* v) {
	free(v->items);
	memset(v, 0, sizeof(*v));
}
int main() {
	Vector v = {0};

	void* x0 = Vector_push(&v, sizeof(int));
	memcpy(x0, &((int[]){0xA0B0C0D0}), sizeof(int));

	void* x1 = Vector_push(&v, sizeof(int));
	memcpy(x1, &((int[]){0x10203040}), sizeof(int));

	Vector_free(&v);
}
