#include "socket.h"
#include "window.h"
#include "chat.h"
#include "ssl.h"

#define LENGTH 2048

// Global variables
int sockfd = 0;
char name[32];
pthread_t watcher;
SSL *ssl;

void str_overwrite_stdout() {
  printf("%s", "> ");
  fflush(stdout);
}

void str_trim_lf (char* arr, int length) {
  int i;
  for (i = 0; i < length; i++) { // trim \n
    if (arr[i] == '\n') {
      arr[i] = '\0';
      break;
    }
  }
}

void send_msg_handler(gchar *message) {

    if (strcmp(message, "exit") == 0) {
            close(sockfd);
            closeWindow();
			return;
    }
    SSL_write(ssl, message, strlen(message));
}

void *  recv_msg_handler() {
	char msg[LENGTH];
    while (1) {
        int receive = SSL_read(ssl, msg, LENGTH);
        if (receive > 0) {
            printf("%s", msg);
            recv_item(msg);
            str_overwrite_stdout();
        } else if (receive == 0) {
            break;
        } 
        memset(msg, 0, sizeof(msg));
    }

   pthread_exit(NULL);
}


int connectToServer(char* username, char *password,char *isLoginForm){

    SSL_write(ssl, username, sizeof(username)); 
    SSL_write(ssl, password, sizeof(password));
    SSL_write(ssl, isLoginForm, sizeof(isLoginForm)); 

    char msg[100];
    int bytes;
    bytes = SSL_read(ssl, msg, sizeof(msg));
  ;
    if(strcmp(msg,"success") == 0){
        show_info(msg);
        hideAuthForm();
        int rc=0;
        int i=0;
        rc = pthread_create(&watcher, NULL, recv_msg_handler,(void *)i);
        if (rc) {
            printf("Error:unable to create thread, %d\n", rc);
            exit(-1);
        }
        return 1;
    }
    show_error(msg);
	return 0;
}
int createSockit(int argc, char *argv[]){

	if(argc != 3){
		printf("Usage: %s <port>\n", argv[0]);
		return EXIT_FAILURE;
	}

	char *ip = argv[1]?argv[1]:"127.0.0.1";
	int port = atoi(argv[2])?atoi(argv[2]):4444;
	signal(SIGINT,SIG_IGN);

     // Initialize the SSL library
    SSL_CTX *ctx;
    ctx = InitCTX();  


	struct sockaddr_in server_addr;

	/* Socket settings */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);
    puts(ip);
    printf("%d\n",port);

  // Connect to Server
    int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (err == -1) {
        printf("ERROR: connect\n");
        close(sockfd);
        return EXIT_FAILURE;
    }
    puts(ip);
    printf("%d\n",port);
    // ===================================== SSL ===================================== //
    ssl = SSL_new(ctx);      /* create new SSL connection state */
    SSL_set_fd(ssl, sockfd);    /* attach the socket descriptor */  
    if ( SSL_connect(ssl) == FAIL ) {  /* perform the connection */
        ERR_print_errors_fp(stderr);
        close(sockfd);
        return EXIT_FAILURE;
    }

    ShowCerts(ssl);        /* get any certs */
    // ===================================== SSL ===================================== //
    

	printf("=== WELCOME TO THE CHATROOM ===\n");


    // close(sockfd);          close socket 
    // SSL_CTX_free(ctx);        /* release context */

	return EXIT_SUCCESS;
}
