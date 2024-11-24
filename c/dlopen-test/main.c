#include <stdlib.h>
#include <stdio.h>

#include "libfoo.h"

int main() {
	libfoo_init();

	libfoo_foo();
	char* str = libfoo_bar(1337, 4.2);
	printf("%s", str);
	free(str);
	dlclose(libfoo);
}
