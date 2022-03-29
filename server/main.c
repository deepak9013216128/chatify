#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>

#include "sockit.h"
#include "sql.h"


int main(int argc, char *argv[]){


    createSockit(argc, argv);
    createSqlConnection();
	printf("=== WELCOME TO THE CHATIFY ===\n");
    getUserData("SELECT * FROM Users WHERE username = 'test'");
    listenSocktetConnection();
    closeSQLConnection();

	return EXIT_SUCCESS;
}

