#include <gtk/gtk.h>

static void printHello(GtkWidget* widget, gpointer user_data) {
	(void) widget;
	(void) user_data;

	g_print("Hello, World!\n");
}

static void activate(GtkApplication* app, gpointer user_data) {
	(void) user_data;

	GtkWidget* window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(window), "Window");
	gtk_container_set_border_width(GTK_CONTAINER(window), 10);

	GtkGrid* grid = GTK_GRID(gtk_grid_new());
	gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(grid));

	GtkWidget* hello_btn_1 = gtk_button_new_with_label("Button 1");
	g_signal_connect(hello_btn_1, "clicked", G_CALLBACK(printHello), NULL);
	gtk_grid_attach(grid, hello_btn_1, 0, 0, 1, 1);

	GtkWidget* hello_btn_2 = gtk_button_new_with_label("Button 2");
	g_signal_connect(hello_btn_2, "clicked", G_CALLBACK(printHello), NULL);
	gtk_grid_attach(grid, hello_btn_2, 1, 0, 1, 1);

	GtkWidget* quit_btn = gtk_button_new_with_label("Quit");
	g_signal_connect_swapped(quit_btn, "clicked", G_CALLBACK(gtk_widget_destroy), window);
	gtk_grid_attach(grid, quit_btn, 0, 1, 2, 1);

	gtk_widget_show_all(window);
}

int main(int argc, char** argv) {
	GtkApplication* app = gtk_application_new(
		"org.gtk.example",
		G_APPLICATION_DEFAULT_FLAGS
	);
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
	int status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	return status;
}
