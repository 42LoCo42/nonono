#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "lodepng.h"

int main(int argc, char** argv) {
	if(argc < 2) errx(1, "Usage: %s input.png > output.br", argv[0]);

	unsigned char* image = NULL;
	unsigned width  = 0;
	unsigned height = 0;
	unsigned error  = lodepng_decode32_file(&image, &width, &height, argv[1]);
	if(error) errx(1, "Could not load %s: %s", argv[1], lodepng_error_text(error));

	printf("buntraum %d %d\n", width, height);
	fwrite(image, sizeof(char), width * height * 4, stdout);
	free(image);
}
