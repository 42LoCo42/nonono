#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>

struct Arena {
	size_t capacity;
	size_t free;
	char*  data;
};

typedef struct Arena Arena;

// Initializes the arena with a given capacity.
// This is the only function that *actually* allocates memory from the OS.
void Arena_init(Arena*, size_t capacity);

// Allocates or frees (n < 0) the next/last n bytes of memory.
// Freeing must be done in the exact opposite order of allocations:
// Allocation order: 42 1337 23
// Free order: -23 -1337 -42
void* Arena_alloc(Arena*, size_t n);
void  Arena_free(Arena*, size_t n);

// Allocates & zeroes memory.
void* Arena_zalloc(Arena*, size_t n);

// Resets the arena allocation to 0.
// This does NOT free the backing memory allocated with Arena_init!
void Arena_reset(Arena*);

// Frees the backing memory of the arena.
// It must be reinitialized before further use.
void Arena_destroy(Arena*);

#endif // ARENA_H
