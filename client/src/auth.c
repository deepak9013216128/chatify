#include "auth.h"
#include "window.h"
#include<login.h>
#include<signup.h>


static GtkWidget *loginBox,*signupBox;
static int activeForm=0;
static int createSignupForm = 1;
void toggleForm(GtkWidget *widget, gpointer data){

    char *isLoginForm;
    isLoginForm = (char *)data;
    if(strcmp(isLoginForm,"0")==0 && activeForm == 1){
        gtk_widget_hide(signupBox);
        gtk_widget_show_all(loginBox);
        activeForm=0;
    }else if(strcmp(isLoginForm,"1")==0 && activeForm==0){
        gtk_widget_hide(loginBox);
        if(createSignupForm)signupForm(signupBox);
        gtk_widget_show_all(signupBox);
        activeForm=1;
        createSignupForm=0;
    }
}

void authFrom(GtkWidget *window){

    GtkWidget *menubar,*loginMenu,*signupMenu;

    GtkWidget *vbox;
    menubar = gtk_menu_bar_new();
    loginMenu = gtk_menu_item_new_with_label("Login");
    signupMenu = gtk_menu_item_new_with_label("Sign up");

    vbox = gtk_box_new(TRUE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);



    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), loginMenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), signupMenu);
    gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);

    g_signal_connect(loginMenu, "activate",G_CALLBACK(toggleForm), (gpointer)"0");
    g_signal_connect(signupMenu, "activate",G_CALLBACK(toggleForm), (gpointer)"1");

    loginBox = gtk_box_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(vbox), loginBox);

    signupBox = gtk_box_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(vbox), signupBox);

    loginForm(loginBox);
    showWindow(window);
}

