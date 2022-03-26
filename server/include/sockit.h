#ifndef SOCKIT_H
#define SOCKIT_H
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "client.h"

int createSockit();
int listenSocktetConnection();
#endif // SOCKIT_H
