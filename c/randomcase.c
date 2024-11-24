#include <stdio.h>
#include <string.h>
#include <unistd.h>

int isLower(char c) {
	return c >= 'a' && c <= 'z';
}

int main(int argc, char** argv) {
	if(argc < 2) {
		fprintf(stderr, "Usage: %s <word>\n", argv[0]);
		return 1;
	}

	char*        word = argv[1];
	const size_t len  = strlen(word);
	const size_t iter = 1 << len; // 2^len iterations since each letter can be upper- or lowercase

	const size_t size = (len + 1) * iter;
	double size_d     = size;
	fprintf(
			stderr, "%ld iterations, %ld bytes (%f KiB, %f MiB)\n",
			iter, size,
			size_d / (1 << 10), size_d / (1 << 20)
	);
	for(int i = 4; i > 0; --i) {
		fprintf(stderr, "\r%d seconds to cancel...", i);
		usleep(1e6);
	}
	fputc('\n', stderr);

	for(size_t i = 0; i < iter; ++i) { // for all iterations:
		for(size_t p = 0; p < len; ++p) { // for all letters:
			char* c = word + p; // current letter
			if(isLower(*c)) {
				*c &= ~32; // change to uppercase
			} else {
				*c |= 32; // change to lowercase
				break;    // and break to print word NOW
			}
		}
		printf("%s\n", word);
	}
}
