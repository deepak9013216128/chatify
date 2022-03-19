#include "socket.h"
#include "window.h"
#include "chat.h"

#define LENGTH 2048

// Global variables
volatile sig_atomic_t flag = 0;
int sockfd = 0;
char name[32];
pthread_t watcher;

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

void catch_ctrl_c_and_exit(int sig) {
    flag = 1;
}

void send_msg_handler(gchar *message) {

    if (strcmp(message, "exit") == 0) {
            close(sockfd);
            closeWindow();
			return;
    }
    send(sockfd, message, strlen(message), 0);
    catch_ctrl_c_and_exit(2);
}

void *  recv_msg_handler(void *threadid) {
    long tid;
    tid = (long)threadid;
	char msg[LENGTH] = {};
    while (1) {
        int receive = recv(sockfd, msg, LENGTH, 0);
        if (receive > 0) {
            printf("%s", msg);
            recv_item(msg);
            str_overwrite_stdout();
        } else if (receive == 0) {
            break;
        } else {
//             -1
        }
        memset(msg, 0, sizeof(msg));
    }

   pthread_exit(NULL);
}


int connectToServer(char* username, char *password,char *isLoginForm){
    char *ip = "127.0.0.1";
    int port = 4444;
    struct sockaddr_in server_addr;

	/* Socket settings */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

  // Connect to Server
    int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (err == -1) {
        printf("ERROR: connect\n");
        return "ERROR: connect\n";
    }

	printf("=== WELCOME TO THE CHATROOM ===\n");
	send(sockfd, username, 32, 0);
	send(sockfd, password, 32, 0);
	send(sockfd, isLoginForm, 32, 0);
    char msg[100];
    recv(sockfd, msg,32,0);
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
int createSockit(){
//	if(argc != 2){
//		printf("Usage: %s <port>\n", argv[0]);
//		return EXIT_FAILURE;
//	}

	char *ip = "127.0.0.1";
	//int port = atoi(argv[1]);
    int port = 4444;
	signal(SIGINT, catch_ctrl_c_and_exit);

	printf("Please enter your name: ");
    fgets(name, 32, stdin);
    str_trim_lf(name, strlen(name));


//	if (strlen(name) > 32 || strlen(name) < 2){
//		printf("Name must be less than 30 and more than 2 characters.\n");
//		return EXIT_FAILURE;
//	}

	struct sockaddr_in server_addr;

	/* Socket settings */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);


  // Connect to Server
    int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (err == -1) {
        printf("ERROR: connect\n");
        return EXIT_FAILURE;
    }
	printf("=== WELCOME TO THE CHATROOM ===\n");

	// Send name
	send(sockfd, name, 32, 0);


//	pthread_t send_msg_thread;

//    if(pthread_create(&send_msg_thread, NULL, (void *) send_msg_handler, NULL) != 0){
//        printf("ERROR: pthread\n");
//        return EXIT_FAILURE;
//    }
//
//	pthread_t recv_msg_thread;
//    if(pthread_create(&recv_msg_thread, NULL, (void *) recv_msg_handler, NULL) != 0){
//        printf("ERROR: pthread\n");
//        return EXIT_FAILURE;
//    }

//	while (1){
//		if(flag){
//			printf("\nBye\n");
//			break;
//        }
//	}

	close(sockfd);

	return EXIT_SUCCESS;
}
