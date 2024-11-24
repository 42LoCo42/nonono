#include <dlfcn.h>
void* libfoo;
void (*__libfoo_foo)(void);
char* (*__libfoo_bar)(int, double);
#define libfoo_foo (*__libfoo_foo)
#define libfoo_bar (*__libfoo_bar)
int libfoo_init() {
	libfoo = dlopen("./libfoo.so", RTLD_LOCAL | RTLD_LAZY);
	if(libfoo == 0) {
		printf("Error: %s\n", dlerror());
		return 1;
	}
	__libfoo_foo = dlsym(libfoo, "foo");
	__libfoo_bar = dlsym(libfoo, "bar");
	return 0;
}
