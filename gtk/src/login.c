#include "login.h"

static GtkWidget *loginUsernameLabel,*loginPasswordLabel;
static GtkWidget *loginUserNameEntry,*loginPasswordEntry;
static GtkWidget *cancelButton,*loginButton;

void loginButtonHandler(GtkWidget *widget, gpointer data){
    (void) widget;
    (void) data;
    if(!gtk_widget_get_sensitive(loginButton))
        return;
    const gchar *ip, *port, *username, *password;
    if(isEmptyText(loginUserNameEntry) || isEmptyText(loginPasswordEntry) ){
        return;
    }

    username = gtk_entry_get_text(GTK_ENTRY(loginUserNameEntry));
    password = gtk_entry_get_text(GTK_ENTRY(loginPasswordEntry));

    connectToServer(username,password,"0");

//    ip = gtk_entry_get_text(GTK_ENTRY(ipEntry));
//    if(!ip || !*ip){
//        gtk_widget_grab_focus(ipEntry);
//        return;
//    }
//    port = gtk_entry_get_text(GTK_ENTRY(portEntry));
//    if(!port || !*port)
//    {
//        gtk_widget_grab_focus(portEntry);
//        return;
//    }
//    int iport;
//    if(!sscanf(port, "%d", &iport))
//    {
//        gtk_label_set_text(GTK_LABEL(statusLabel), "Invalid port");
//        return;
//    }
//    gtk_widget_set_sensitive(loginButton, 0);
//    struct login_info *li = malloc(sizeof(struct login_info));
//    li->ip = ip;
//    li->iport = iport;
//    li->login = login;
//    li->password = password;
//    pthread_create(&loginner, 0, login_thread, (void *)li);
}

void loginForm(GtkWidget *window){

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

    g_signal_connect(cancelButton, "clicked",G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(loginButton, "clicked",G_CALLBACK(loginButtonHandler), NULL);

}

