#include <gtk-3.0/gtk/gtk.h>

GtkWidget* gtk_file_chooser_dialog_new(
	const gchar* title,
	GtkWindow* parent,
	GtkFileChooserAction action,
	const gchar* first_button_text,
	...
) {
	puts("called");
}
