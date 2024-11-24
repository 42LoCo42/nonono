#include <gtk/gtk.h>

#include "dbg.h"

static void printHello(GtkWidget* widget, gpointer user_data) {
	(void) widget;
	(void) user_data;

	g_print("Hello, World!\n");
	warnx("foobar");
}

int main(int argc, char** argv) {
	gtk_init(&argc, &argv);

	GError* error = NULL;
	GtkBuilder* builder = gtk_builder_new();
	if(!gtk_builder_add_from_file(builder, "builder.xml", &error)) {
		g_printerr("Error loading file: %s\n", error->message);
		g_clear_error(&error);
		return 1;
	}

	GObject* window = gtk_builder_get_object(builder, "window");
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	GObject* btn_hello_1 = gtk_builder_get_object(builder, "button1");
	g_signal_connect(btn_hello_1, "clicked", G_CALLBACK(printHello), NULL);

	GObject* btn_hello_2 = gtk_builder_get_object(builder, "button2");
	g_signal_connect(btn_hello_2, "clicked", G_CALLBACK(printHello), NULL);

	GObject* btn_quit = gtk_builder_get_object(builder, "quit");
	g_signal_connect(btn_quit, "clicked", G_CALLBACK(gtk_main_quit), NULL);

	gtk_main();
	return 0;
}
