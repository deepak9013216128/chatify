#ifndef SOCKET_H
#define SOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <gtk/gtk.h>

struct User{
    const char *ip;
    const char *username;
    const char *password;
    int iport;
};

void str_overwrite_stdout();
void str_trim_lf (char* arr, int length);
void send_msg_handler(gchar *msg) ;
void* recv_msg_handler() ;
int connectToServer(char* username, char *password,char *isLoginForm);
int createSockit(int argc, char *argv[]);

//void serialize_X(struct User *x, struct Buffer *output) {
//    serialize_string(x->ip, output);
//    serialize_int(x->iport, output);
//    serialize_string(x->username, output);
//    serialize_string(x->password, output);
//}


//char * serialize(struct User t) {
//    int str_len = strlen(t.username);
//
//    int size = 2 * sizeof(int) + strlen(t.username) + strlen(t.password) + strlen(t.ip);
//    char *buf = malloc(sizeof(char) * (size+1));
//
//    memcpy(buf, &t.iport, sizeof(int));
//    memcpy(buf + sizeof(int), &strlen(t.username), sizeof(int));
//    memcpy(buf + sizeof(int) * 2, t.username, strlen(t.username));
//    buf[size] = '\0';
//
//    return buf;
//}

//test_struct_t deserialize(char *buf) {
//    test_struct_t t;
//
//    memcpy(&t.a, buf, sizeof(int));
//
//    int str_len;
//    memcpy(&str_len, buf+sizeof(int), sizeof(int));
//
//    t.str = malloc(sizeof(char) * (str_len+1));
//    memcpy(t.str, buf+2*sizeof(int), str_len);
//    t.str[str_len] = '\0';
//
//    return t;
//}

#endif // SOCKET_H
