#include <arena.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void Arena_init(Arena* a, size_t capacity) {
	a->capacity = capacity;
	a->free     = capacity;
	a->data     = (char*) malloc(capacity);
	assert(a->data != NULL && "Arena backend memory allocation");
}

void* Arena_alloc(Arena* a, size_t n) {
	if(n > a->free)
		return NULL;

	void* ptr = a->data;
	a->data += n;
	a->free -= n;
	return ptr;
}

void* Arena_zalloc(Arena* a, size_t n) {
	void* ptr = Arena_alloc(a, n);
	memset(ptr, 0, n);
	return ptr;
}

void Arena_free(Arena* a, size_t n) {
	a->data -= n;
	a->free += n;
}

void Arena_reset(Arena* a) {
	a->data -= (a->capacity - a->free);
	a->free = a->capacity;
}

void Arena_destroy(Arena* a) {
	Arena_reset(a);
	a->capacity = 0;
	a->free     = 0;
	free(a->data);
}
