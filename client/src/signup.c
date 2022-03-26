#include "signup.h"

static GtkWidget *usernameLabel,*passwordLabel,*confirmPasswordLabel;
static GtkWidget *userNameEntry,*passwordEntry, *confirmPasswordEntry;
static GtkWidget *cancelButton,*signupButton;

void signupButtonHandler(GtkWidget *widget, gpointer data){
    (void) widget;
    (void) data;
    if(!gtk_widget_get_sensitive(signupButton))
        return;
    const gchar *ip, *port, *username, *password;
    if(isEmptyText(userNameEntry) || isEmptyText(passwordEntry) || isEmptyText(confirmPasswordEntry)){
        return;
    }
    if(strcmp(gtk_entry_get_text(GTK_ENTRY(passwordEntry)),gtk_entry_get_text(GTK_ENTRY(confirmPasswordEntry)))!=0){
        show_warning("Password must match!");
        return;
    }
    username = gtk_entry_get_text(GTK_ENTRY(userNameEntry));
    password = gtk_entry_get_text(GTK_ENTRY(passwordEntry));
    connectToServer(username,password,"1");
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

void signupForm(GtkWidget *window){

    GtkWidget *table;

    usernameLabel = gtk_label_new("Username");
    passwordLabel = gtk_label_new("Password");
    confirmPasswordLabel = gtk_label_new("confirm Password");
    signupButton = gtk_button_new_with_label("sign up");
    cancelButton = gtk_button_new_with_mnemonic("Cancel");
    userNameEntry = gtk_entry_new();
    passwordEntry = gtk_entry_new();
    confirmPasswordEntry = gtk_entry_new();

    table = gtk_table_new(4, 2, TRUE);
    gtk_container_add(GTK_CONTAINER(window), table);

    gtk_table_attach(GTK_TABLE(table), usernameLabel, 0, 1, 0, 1, GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);
    gtk_table_attach(GTK_TABLE(table), passwordLabel, 0, 1, 1, 2, GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);
    gtk_table_attach(GTK_TABLE(table), confirmPasswordLabel, 0, 1, 2, 3, GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);
    gtk_table_attach(GTK_TABLE(table), cancelButton, 0, 1, 3, 4, GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);
    gtk_table_attach(GTK_TABLE(table), userNameEntry, 1, 2, 0, 1, GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);
    gtk_table_attach(GTK_TABLE(table), passwordEntry, 1, 2, 1, 2, GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);
    gtk_table_attach(GTK_TABLE(table), confirmPasswordEntry, 1, 2, 2, 3, GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);
    gtk_table_attach(GTK_TABLE(table), signupButton, 1, 2, 3, 4, GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);

    g_signal_connect(cancelButton, "clicked",G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(signupButton, "clicked",G_CALLBACK(signupButtonHandler), NULL);

}


