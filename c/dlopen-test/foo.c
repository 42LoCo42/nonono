#include <stdio.h>
#include <stdlib.h>

#include "foo.h"

void foo() {
	printf("foo\n");
}

char* bar(int a, double b) {
	char* result = malloc(BUFSIZ);
	sprintf(result, "bar %i %f\n", a, b);

	return result;
}
