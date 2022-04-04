#include "window.h" 
#include "auth.h"

extern GtkWidget *authBox;
extern GtkWidget *chatBox;
int main(int argc, char *argv[]) {
    createSockit(argc,argv);
    createWindow(argc,argv);
    authFrom();
    chatForm();
    changeWidget(chatBox, authBox);
    return 0;
}
