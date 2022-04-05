#include "login.h"
#include "path.h"

GtkWidget *loginUsernameLabel, *loginPasswordLabel;
GtkWidget *loginUserNameEntry, *loginPasswordEntry;
GtkWidget *cancelButton, *loginButton;

void loginButtonHandler(GtkWidget *widget, gpointer data)
{
    (void)widget;
    (void)data;
    if (!gtk_widget_get_sensitive(loginButton))
        return;
    const gchar *ip, *port, username[BUFSIZ], password[BUFSIZ];
    if (isEmptyText(loginUserNameEntry) || isEmptyText(loginPasswordEntry))
    {
        return;
    }

    strcpy(username, gtk_entry_get_text(GTK_ENTRY(loginUserNameEntry)));
    strcpy(password, gtk_entry_get_text(GTK_ENTRY(loginPasswordEntry)));
    gtk_entry_set_text(GTK_ENTRY(loginUserNameEntry), "");
    gtk_entry_set_text(GTK_ENTRY(loginPasswordEntry), "");
    connectToServer(username, password, LOGIN);
}

void loginForm(GtkWidget *window)
{

    GtkWidget *table;

    loginUsernameLabel = gtk_label_new("Username");
    loginPasswordLabel = gtk_label_new("Password");
    loginButton = gtk_button_new_with_label("Login");
    cancelButton = gtk_button_new_with_mnemonic("Cancel");
    loginUserNameEntry = gtk_entry_new();
    loginPasswordEntry = gtk_entry_new();

    table = gtk_table_new(3, 2, TRUE);
    gtk_container_add(GTK_CONTAINER(window), table);

    gtk_table_attach(GTK_TABLE(table), loginUsernameLabel, 0, 1, 0, 1, GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);
    gtk_table_attach(GTK_TABLE(table), loginPasswordLabel, 0, 1, 1, 2, GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);
    gtk_table_attach(GTK_TABLE(table), cancelButton, 0, 1, 2, 3, GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);
    gtk_table_attach(GTK_TABLE(table), loginUserNameEntry, 1, 2, 0, 1, GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);
    gtk_table_attach(GTK_TABLE(table), loginPasswordEntry, 1, 2, 1, 2, GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);

    gtk_table_attach(GTK_TABLE(table), loginButton, 1, 2, 2, 3, GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);
    //    struct GdkRGBA {
    //      gdouble red;
    //      gdouble green;
    //      gdouble blue;
    //      gdouble alpha;
    //    };
    //     struct GdkRGBA rgba;
    //    rgba.red=1;
    //    rgba.blue=0;
    //    rgba.green=0;
    //    rgba.alpha=0;
    //    const struct GdkRGBA *color = &rgba;
    //  GtkWidget *button1;
    //  button1 = gtk_color_button_new_with_rgba (color);
    //    gtk_table_attach(GTK_TABLE(table), button1, 1, 2, 2, 3, GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);
    g_signal_connect(cancelButton, "clicked", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(loginButton, "clicked", G_CALLBACK(loginButtonHandler), NULL);
}
