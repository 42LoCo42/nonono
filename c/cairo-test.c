#include <cairo/cairo.h>
#include <err.h>

#define check(x, ...) if(x) err(1, __VA_ARGS__);

#define WIDTH 1920
#define HEIGHT 1080

#define c(x) (x / 255.0)

int main() {
	cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, WIDTH, HEIGHT);
	check(cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS,
		"Failed to create cairo surface");

	cairo_t* context = cairo_create(surface);
	check(cairo_status(context) != CAIRO_STATUS_SUCCESS,
		"Failed to create cairo context");

	cairo_set_source_rgba(context, c(0xbb), c(0x77), c(0xff), 1);
	cairo_move_to(context, WIDTH / 2, 0);
	cairo_line_to(context, WIDTH * 2 / 3, HEIGHT);
	cairo_line_to(context, 0, HEIGHT / 3);
	cairo_line_to(context, WIDTH, HEIGHT / 3);
	cairo_line_to(context, WIDTH / 3, HEIGHT);
	cairo_line_to(context, WIDTH / 2, 0);
	cairo_stroke(context);

	check(cairo_surface_write_to_png(surface, "out.png") != CAIRO_STATUS_SUCCESS,
		"Could not write to png");
}
