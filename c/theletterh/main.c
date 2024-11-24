#include <stdio.h>
#include <stdlib.h>

void drawVertical(int size) {
	for(int i = 0; i < size; ++i) {
		putchar('H');
		for(int j = 0; j < size; ++j) {
			putchar(' ');
		}
		putchar('H');
		putchar('\n');
	}
}

void drawHorizontal(int size) {
	for(int i = 0; i < size + 2; ++i) {
		putchar('H');
	}
	putchar('\n');
}

int main(int argc, char** argv) {
	if(argc < 2) return 1;
	int size = strtol(argv[1], NULL, 0);
	drawVertical(size);
	drawHorizontal(size);
	drawVertical(size);
}
