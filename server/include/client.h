#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <pthread.h>
#include "ssl.h"

#define MAX_CLIENTS 100
#define BUFFER_SZ 1024

/* Client structure */
typedef struct{
	struct sockaddr_in address;
	int sockfd;
	int uid;
	char name[32];
	SSL *ssl;
} client_t;

/* Path */
#define LOGIN "/login"
#define SIGNUP "/signup"
#define SENDALL "/sendall"

#define BYE "/bye"
#define QUIT "/quit"
#define GAME "/game"
#define JOIN "/join"
#define LIST "/list"
#define SAY "/say"
#define USER "/user"
#define WHO "/who"
#define YES "/yes"
#define NO "/no"

void str_overwrite_stdout();
void str_trim_lf (char* arr, int length);
void queue_add(client_t *cl);
void queue_remove(int uid);
void send_message(char *s, int uid);
void *handle_client(void *arg);

#endif // UTILS_H
