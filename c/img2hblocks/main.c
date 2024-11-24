#include <endian.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

typedef struct {
	uint16_t r;
	uint16_t g;
	uint16_t b;
	uint16_t a;
} Pixel;

typedef enum {
	MODE_FG = 3,
	MODE_BG = 4,
} Mode;

void printPixel(Pixel p, Mode mode) {
	uint8_t r = p.r * UINT8_MAX / UINT16_MAX;
	uint8_t g = p.g * UINT8_MAX / UINT16_MAX;
	uint8_t b = p.b * UINT8_MAX / UINT16_MAX;
	uint8_t a = p.a * UINT8_MAX / UINT16_MAX;

	r = r * a / UINT8_MAX;
	g = g * a / UINT8_MAX;
	b = b * a / UINT8_MAX;

	printf("%d8;2;%d;%d;%d", mode, r, g, b);
}

int main(int argc, char** argv) {
	if(argc < 2) errx(1, "Usage: %s <width in pixels>", argv[0]);

	char trash[16] = {0};
	fread(trash, 16, 1, stdin);

	size_t width = atoi(argv[1]);
	size_t count = 2 * width;
	Pixel* buffer = calloc(sizeof(Pixel), count);

	for(;;) {
		fread(buffer, sizeof(Pixel), count, stdin);

		if(feof(stdin)) return 0;
		if(ferror(stdin)) err(1, "Read error");

		for(size_t i = 0; i < width; i++) {
			printf("\e[");
			printPixel(buffer[i], MODE_FG);
			printf(";");
			printPixel(buffer[i + width], MODE_BG);
			printf("m▀");
		}
		puts("");
	}

	free(buffer);
}
