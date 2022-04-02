#include<window.h>
#include "auth.h"

extern GtkWidget *authBox;

int main(int argc, char *argv[]) {
    createSockit(argc,argv);
    GtkWidget *window;
    window = createWindow(argc,argv);
    authFrom();
    chatForm();
    changeWidget(NULL,authBox);
    showWindow();
    return 0;
}
