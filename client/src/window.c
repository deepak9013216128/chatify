#include "window.h"

GtkWidget *WINDOW;
GtkWidget *window;

GtkWidget *createWindow(int argc, char *argv[])
{

	gtk_init(&argc, &argv);

	WINDOW = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(WINDOW), GTK_WIN_POS_CENTER);

	gtk_window_set_default_size(GTK_WINDOW(WINDOW), 360, 250);
	gtk_window_set_title(GTK_WINDOW(WINDOW), "Chatify");
	gtk_container_set_border_width(GTK_CONTAINER(WINDOW), 10);
	window = gtk_box_new(TRUE, 0);
    gtk_container_add(GTK_CONTAINER(WINDOW), window);
	g_signal_connect(WINDOW, "destroy", G_CALLBACK(closeWindow), NULL);

	return window;
}
void showWindow()
{
	gtk_widget_show_all(window);

	g_signal_connect(window, "destroy", G_CALLBACK(closeWindow), NULL);

	gtk_main();
}

int isEmptyText(GtkWidget *widget)
{

	const gchar *text;
	text = gtk_entry_get_text(GTK_ENTRY(widget));

	if (!text || !*text)
	{
		gtk_widget_grab_focus(widget);
		return 1;
	}
	return 0;
}

void show_info(gchar *msg)
{

	GtkWidget *dialog;
	dialog = gtk_message_dialog_new(GTK_WINDOW(WINDOW),
									GTK_DIALOG_DESTROY_WITH_PARENT,
									GTK_MESSAGE_INFO,
									GTK_BUTTONS_OK,
									msg);
	gtk_window_set_title(GTK_WINDOW(dialog), "Information");
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

void show_error(gchar *msg)
{

	GtkWidget *dialog;
	dialog = gtk_message_dialog_new(GTK_WINDOW(WINDOW),
									GTK_DIALOG_DESTROY_WITH_PARENT,
									GTK_MESSAGE_ERROR,
									GTK_BUTTONS_OK,
									msg);
	gtk_window_set_title(GTK_WINDOW(dialog), "Error");
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

void show_warning(gchar *msg)
{

	GtkWidget *dialog;
	dialog = gtk_message_dialog_new(GTK_WINDOW(WINDOW),
									GTK_DIALOG_DESTROY_WITH_PARENT,
									GTK_MESSAGE_WARNING,
									GTK_BUTTONS_OK,
									msg);
	gtk_window_set_title(GTK_WINDOW(dialog), "Warning");
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

void changeWidget(GtkWidget *hideWidget, GtkWidget *showWidget)
{
	gtk_widget_show_all(WINDOW);
	gtk_widget_hide( hideWidget);
	resizeWindow();
	gtk_main();
}

void closeWindow()
{
	gtk_widget_destroy(window);
	// exit(0);
}

void resizeWindow(){
	gtk_window_resize(WINDOW,360,200);
}
