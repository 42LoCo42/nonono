#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef void* (*vp1_f)(void*);

/**
	pmap - parallel map
	This function runs lambda in parallel for each element of the array.
	The return value of lambda is the new value of the element.
*/
int pmap(vp1_f lambda, void** array, size_t size) {
	int ret = -1;
	pthread_t* threads = NULL;

	if((threads = calloc(size, sizeof(pthread_t))) == NULL) goto exit;

	for(size_t i = 0; i < size; ++i) {
		if((ret = pthread_create(threads + i, NULL, lambda, array[i])) != 0) goto exit;
	}

	for(size_t i = 0; i < size; ++i) {
		if((ret = pthread_join(threads[i], array + i)) != 0) goto exit;
	}

	ret = 0;
exit:
	free(threads);
	return ret;
}

int map(vp1_f lambda, void** array, size_t size) {
	for(size_t i = 0; i < size; ++i) {
		array[i] = lambda(array[i]);
	}
	return 0;
}

void* child(void* arg) {
	long num = (long) arg;
	usleep(num * 1e6);
	printf("child %ld\n", num);
	return arg;
}

int main() {
	long nums[] = {1, 2, 3, 4, 5};
	printf("ret = %d\n", pmap(child, (void**) nums, 5));
	for(size_t i = 0; i < 5; ++i) {
		printf("%ld ", nums[i]);
	}
	putchar('\n');
}
