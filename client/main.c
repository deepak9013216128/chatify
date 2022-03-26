#include<window.h>
#include "auth.h"
#include "chat.h"

int main(int argc, char *argv[]) {

    GtkWidget *window;
    window = createWindow(argc,argv);
    chatForm(argc,argv);
//    loginForm(window);
//    signupForm(window);
//    createSockit();
    authFrom(window);

    //showWindow(window);
    return 0;
}
