#include <err.h>
#include <stdio.h>
#include <sys/random.h>

#define die(...) err(1, __VA_ARGS__);

int main(void) {
	for(size_t i = 0; i < 3; i++) {
		char buf[32] = {0};
		if(getrandom(buf, sizeof(buf), 0) < 0) die("getrandom");

		for(size_t i = 0; i < sizeof(buf); i++) {
			printf("%02hhx", buf[i]);
		}

		puts("");
	}
}
