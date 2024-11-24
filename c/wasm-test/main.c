#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define die(...) {                               \
	fprintf(stderr, __VA_ARGS__);                \
	fprintf(stderr, ": %s\n", strerror(errno));  \
	exit(1);                                     \
}                                                \

#define dieX(...) {               \
	fprintf(stderr, __VA_ARGS__); \
	fputc('\n', stderr);          \
	exit(1);                      \
}

int main(int argc, char **argv) {
	if(argc != 3) dieX("Usage: %s <source> <target>", argv[0]);

	FILE* from = fopen(argv[1], "r");
	if(from == NULL) die("Could not open source file %s", argv[1]);

	FILE* to = fopen(argv[2], "w");
	if(to == NULL) die("Could not open target file %s", argv[2]);

	char buf[BUFSIZ] = {0};
	while(!feof(from)) {
		size_t numRead = fread (buf, sizeof(buf[0]), sizeof(buf), from);
		if(ferror(from)) die("Read error");
		fwrite(buf, sizeof(buf[0]), numRead, to);
		if(ferror(to)) die("Write error");
	}

	fclose(from);
	fclose(to);
}
