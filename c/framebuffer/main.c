#include <err.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define XRES 640
#define YRES 480
#define PXS    2

#define check(x) if(x) err(1, "%s:%d", __FILE__, __LINE__);

uint16_t px(uint8_t r, uint8_t g, uint8_t b) {
	return (r << 11) | (g << 5) | b;
}

int main() {
	size_t len = 8;
	char*  buf = malloc(len);
	check(read(0, buf, len) != (ssize_t) len);
	check(strncmp(buf, "farbfeld", len) != 0);
	free(buf);

	uint32_t xres;
	check(read(0, &xres, sizeof(xres)) != (ssize_t) sizeof(xres));
	check(be32toh(xres) != XRES);

	uint32_t yres;
	check(read(0, &yres, sizeof(yres)) != (ssize_t) sizeof(yres));
	check(be32toh(yres) != YRES);

	for(size_t i = 0; i < XRES * YRES; ++i) {
		uint16_t r, g, b, a;
		check(read(0, &r, sizeof(uint16_t)) != (ssize_t) sizeof(uint16_t));
		check(read(0, &g, sizeof(uint16_t)) != (ssize_t) sizeof(uint16_t));
		check(read(0, &b, sizeof(uint16_t)) != (ssize_t) sizeof(uint16_t));
		check(read(0, &a, sizeof(uint16_t)) != (ssize_t) sizeof(uint16_t));
		a = be16toh(a);
		r = (be16toh(r) * a) >> 16;
		g = (be16toh(g) * a) >> 16;
		b = (be16toh(b) * a) >> 16;

		uint16_t out = px(r >> 11, g >> 10, b >> 11);
		write(1, &out, sizeof(out));
	}
}
