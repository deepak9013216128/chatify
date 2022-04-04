#include "auth.h"
#include "window.h"
#include<login.h>
#include<signup.h>

extern GtkWidget *window;
GtkWidget *loginBox,*signupBox,*authBox;
int activeForm=0;

void toggleForm(GtkWidget *widget, gpointer data){

    GtkWidget *activateWidget;
    activateWidget = (GtkWidget *)data;
    gtk_widget_set_sensitive(widget, FALSE);
    gtk_widget_set_sensitive(activateWidget, TRUE);
    if(activeForm == 1){
        gtk_container_remove(GTK_CONTAINER(authBox),signupBox);
        loginBox = gtk_box_new(FALSE, 0);
        gtk_container_add(GTK_CONTAINER(authBox), loginBox);
        loginForm(loginBox);
        gtk_widget_show_all(loginBox);
        activeForm=0;
    }else if(activeForm==0){
        gtk_container_remove(GTK_CONTAINER(authBox),loginBox);
        signupBox = gtk_box_new(FALSE, 0);
        gtk_container_add(GTK_CONTAINER(authBox), signupBox);
        signupForm(signupBox);
        gtk_widget_show_all(signupBox);
        activeForm=1;
    }
    resizeWindow();
}

void authFrom(){

    GtkWidget *menubar,*loginMenu,*signupMenu;

    menubar = gtk_menu_bar_new();
    loginMenu = gtk_menu_item_new_with_label("Login");
    signupMenu = gtk_menu_item_new_with_label("Sign up");

    authBox = gtk_box_new(TRUE, 0);
    // if(isAdd == 1 )gtk_container_add(GTK_CONTAINER(window), authBox);

    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), loginMenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), signupMenu);
    gtk_box_pack_start(GTK_BOX(authBox), menubar, FALSE, FALSE, 0);

    g_signal_connect(loginMenu, "activate",G_CALLBACK(toggleForm), (gpointer)signupMenu);
    g_signal_connect(signupMenu, "activate",G_CALLBACK(toggleForm), (gpointer)loginMenu);

    loginBox = gtk_box_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(authBox), loginBox);

    signupBox = gtk_box_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(authBox), signupBox);

    loginForm(loginBox);
    gtk_container_add(GTK_CONTAINER(window), authBox);

    resizeWindow();
    // gtk_widget_show_all(authBox);
}

