#include <arena.h>
#include <stdio.h>

int main(void) {
	Arena a;
	Arena_init(&a, 1 << 30);

	printf("%p\n", a.data);

	void* p1 = Arena_zalloc(&a, 1 << 10);
	printf("%p %p %lx %lx\n", p1, a.data, a.capacity, a.free);
	printf("%d\n", *(int*) p1);

	Arena_free(&a, 1 << 10);

	void* p2 = Arena_alloc(&a, 1 << 10);
	printf("%p %p %lx %lx\n", p2, a.data, a.capacity, a.free);

	printf("%p\n", a.data);

	Arena_destroy(&a);
	return 0;
}
