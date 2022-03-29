#include<window.h>
#include "auth.h"
#include "chat.h"

int main(int argc, char *argv[]) {
    createSockit(argc,argv);
    GtkWidget *window;
    window = createWindow(argc,argv);
    chatForm(argc,argv);
    authFrom(window);
    showWindow(window);
    return 0;
}
