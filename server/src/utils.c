#include "utils.h"
#include "sql.h"

client_t *clients[MAX_CLIENTS];

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void str_overwrite_stdout(){
    printf("\r%s", "> ");
    fflush(stdout);
}

void str_trim_lf (char* arr, int length){
    int i;
    for (i = 0; i < length; i++)   // trim \n
    {
        if (arr[i] == '\n')
        {
            arr[i] = '\0';
            break;
        }
    }
}

void print_client_addr(struct sockaddr_in addr){
    printf("%d.%d.%d.%d",
           addr.sin_addr.s_addr & 0xff,
           (addr.sin_addr.s_addr & 0xff00) >> 8,
           (addr.sin_addr.s_addr & 0xff0000) >> 16,
           (addr.sin_addr.s_addr & 0xff000000) >> 24);
}


/* Add clients to queue */
void queue_add(client_t *cl){
	pthread_mutex_lock(&clients_mutex);

	for(int i=0; i < MAX_CLIENTS; ++i){
		if(!clients[i]){
			clients[i] = cl;
			break;
		}
	}

	pthread_mutex_unlock(&clients_mutex);
}

/* Remove clients to queue */
void queue_remove(int uid){
	pthread_mutex_lock(&clients_mutex);

	for(int i=0; i < MAX_CLIENTS; ++i){
		if(clients[i]){
			if(clients[i]->uid == uid){
				clients[i] = NULL;
				break;
			}
		}
	}

	pthread_mutex_unlock(&clients_mutex);
}

/* Send message to all clients except sender */
void send_message(char *s, int uid){
	pthread_mutex_lock(&clients_mutex);

	for(int i=0; i<MAX_CLIENTS; ++i){
		if(clients[i]){
			if(clients[i]->uid != uid){
				if(write(clients[i]->sockfd, s, strlen(s)) < 0){
					perror("ERROR: write to descriptor failed");
					break;
				}
			}
		}
	}

	pthread_mutex_unlock(&clients_mutex);
}


/* Handle all communication with the client */
void *handle_client(void *arg){
	char buff_out[BUFFER_SZ];
	char username[32];
    char password[32];
    char isLogin[2]="0";
	int leave_flag = 0;

	cli_count++;
	client_t *cli = (client_t *)arg;

	recv(cli->sockfd, username, 32, 0);
    strcpy(cli->name, username);
    recv(cli->sockfd, password, 32, 0);
    recv(cli->sockfd, isLogin, 32, 0);

	if( strlen(username) < 2){
		printf("Didn't enter the name.\n");
		leave_flag = 1;
		send(cli->sockfd,"Login Failed!",32,0);
	} else{

        char query[100] ;
        puts(isLogin);
        if(strcmp(isLogin,"0")==0){
            strcat( query,"SELECT password from Users WHERE username = '");
            strcat(query,cli->name);
            strcat(query,"' AND password = '");
            strcat(query,password);
            strcat(query,"'");
            sqlite3_stmt* res = getUserData(query);
            strcpy(query,"");
            int step = sqlite3_step(res);
            char *dbPassword ;
            if (step == SQLITE_ROW) {
                dbPassword =sqlite3_column_text(res, 0);
            }
            if(dbPassword &&strcmp(dbPassword,password)==0){
                puts(dbPassword);
                sprintf(buff_out, "%s has joined\n", cli->name);

                printf("%s", buff_out);
                send(cli->sockfd,"success",32,0);
                send_message(buff_out, cli->uid);
            }else{
                leave_flag = 1;
                send(cli->sockfd,"Login Failed!",32,0);
            }
        }else{
            strcat(query,"INSERT INTO Users (username, password, ip, port) VALUES('");
            strcat(query,cli->name);
            strcat(query,"', '");
            strcat(query,password);
            strcat(query,"', '127.0.0.1', 4444);");
            puts(query);
            sqlite3_stmt* res = getUserData(query);
            strcpy(query,"");
            sprintf(buff_out, "%s has joined\n", cli->name);

            printf("%s", buff_out);
            send(cli->sockfd,"success",32,0);
            send_message(buff_out, cli->uid);
        }
	}

	bzero(buff_out, BUFFER_SZ);

	while(1){
		if (leave_flag) {
			break;
		}

		int receive = recv(cli->sockfd, buff_out, BUFFER_SZ, 0);
		if (receive > 0){
			if(strlen(buff_out) > 0){
				send_message(buff_out, cli->uid);

				str_trim_lf(buff_out, strlen(buff_out));
				printf("%s -> %s\n", buff_out, cli->name);
			}
		} else if (receive == 0 || strcmp(buff_out, "exit") == 0){
			sprintf(buff_out, "%s has left\n", cli->name);
			printf("%s", buff_out);
			send_message(buff_out, cli->uid);
			leave_flag = 1;
		} else {
			printf("ERROR: -1\n");
			leave_flag = 1;
		}

		bzero(buff_out, BUFFER_SZ);
	}

  /* Delete client from queue and yield thread */
	close(cli->sockfd);
    queue_remove(cli->uid);
    free(cli);
    cli_count--;
    pthread_detach(pthread_self());

	return NULL;
}
