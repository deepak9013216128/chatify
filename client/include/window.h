#ifndef WINDOW_H
#define WINDOW_H

#include <gtk/gtk.h>

GtkWidget * createWindow(int argc, char *argv[]);
void showWindow();
int isEmptyText(GtkWidget *widget);
void show_info(gchar *msg) ;
void show_error(gchar *msg);
void show_warning(gchar *msg);
void changeWidget(GtkWidget * hideWidget, GtkWidget *showWidget);
void closeWindow();
void resizeWindow();
#endif // WINDOW_H
