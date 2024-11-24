#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "lodepng.h"

int main(int argc, char** argv) {
	if(argc < 2) errx(1, "Usage: %s output.png < input.br", argv[0]);

	char* signature = NULL;
	unsigned width;
	unsigned height;
	scanf("%ms %d %d", &signature, &width, &height);

	if(strcmp(signature, "buntraum") != 0) errx(1, "Not a buntraum image!");
}
