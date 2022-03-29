#include "client.h"
#include "sql.h"

unsigned int cli_count = 0;
int uid = 10;
client_t *clients[MAX_CLIENTS];

char loginFailed[] = "Login Failed!";
char success[] = "success";
char query[BUFFER_SZ];
char buff_out[BUFFER_SZ];

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
void send_message_to_all(char *s, int uid){
	pthread_mutex_lock(&clients_mutex);

	for(int i=0; i<MAX_CLIENTS; ++i){
		if(clients[i]){
			if(clients[i]->uid != uid){
				if(SSL_write(clients[i]->ssl, s, BUFFER_SZ) <= 0){
					perror("ERROR: write to descriptor failed");
					break;
				}
			}
		}
	}

	pthread_mutex_unlock(&clients_mutex);
}
void send_message(char *s, int uid){
	pthread_mutex_lock(&clients_mutex);
	
	for(int i=0; i<MAX_CLIENTS; ++i){
		if(clients[i]){
			if(clients[i]->uid != uid){
				if(SSL_write(clients[i]->ssl, s, BUFFER_SZ) <= 0){
					perror("ERROR: write to descriptor failed");
					break;
				}
			}
		}
	}

	pthread_mutex_unlock(&clients_mutex);
}

void delete_client(client_t *cli ){
	 /* Delete client from queue and yield thread */
	close(cli->sockfd);
    queue_remove(cli->uid);
    free(cli);
    cli_count--;
    pthread_detach(pthread_self());
}

int login(client_t *cli,char *username, char* password){

	strcat( query,"SELECT password from Users WHERE username = '");
    strcat(query,username);
    strcat(query,"' AND password = '");
    strcat(query,password);
    strcat(query,"'");
    sqlite3_stmt* res = getUserData(query);
    puts(query);
    int step = sqlite3_step(res);
    if (step != SQLITE_ROW) {
    	SSL_write(cli->ssl, loginFailed, sizeof(loginFailed));
    	// delete_client(cli);
		return 0;
    }
    const char *dbPassword = (const char *)sqlite3_column_text(res, 0);
    if(dbPassword &&strcmp(dbPassword,password)==0){
        sprintf(buff_out, "%s has joined\n", username);
        SSL_write(cli->ssl,success,sizeof(success));
        send_message_to_all(buff_out, cli->uid);
        return 1;
    }
    SSL_write(cli->ssl,loginFailed,sizeof(loginFailed));
    // delete_client(cli);
	return 0;
}

int signup(client_t *cli, char * username, char * password) {

	strcat(query,"INSERT INTO Users (username, password, ip, port) VALUES('");
    strcat(query,username);
    strcat(query,"', '");
    strcat(query,password);
    strcat(query,"', '127.0.0.1', 4444);");
    puts(query);
    sqlite3_stmt* res = getUserData(query);
    sqlite3_step(res);
    
    sprintf(buff_out, "%s has joined\n", cli->name);
    SSL_write(cli->ssl,success,sizeof(success));
    send_message_to_all(buff_out, cli->uid);
    return 1;
}

/* Handle all communication with the client */
void * handle_client(void *arg){
	char buff_out[BUFFER_SZ];
	char username[BUFFER_SZ];
    char password[BUFFER_SZ];
    char isLogin[BUFFER_SZ]="0";
	int leave_flag = 0;

	cli_count++;
	client_t *cli = (client_t *)arg;

   	while(1){
   		if(leave_flag){
    		strcpy(cli->name,username);
   			break;
   		}
   		bzero(username, BUFFER_SZ);
		bzero(password, BUFFER_SZ);
		bzero(isLogin, BUFFER_SZ);

		if(
			SSL_read(cli->ssl, username, sizeof(username))<=0 ||
			SSL_read(cli->ssl, password, sizeof(password)) <=0 ||
			SSL_read(cli->ssl, isLogin, sizeof(isLogin)) <=0
		){
			delete_client(cli);
			return NULL;
		}

		if( strlen(username) < 2 && strlen(password) < 2){
			printf("Didn't enter the name and password.\n");
			SSL_write(cli->ssl,loginFailed,sizeof(loginFailed));
			// delete_client(cli);
			continue;
		}
    	if(strcmp(isLogin,"0")==0){
    		leave_flag = login(cli,username,password);
    	}else if(strcmp(isLogin,"1")==0){
    		leave_flag = signup(cli,username,password);
    	}
		bzero(buff_out, BUFFER_SZ);
		bzero(query, BUFFER_SZ);
   	}

	leave_flag = 0;

	while(1){
		if (leave_flag) {
			break;
		}
		char msg[BUFFER_SZ];
        strcat( msg,"[");
        strcat(msg,cli->name);
        strcat(msg,"]: ");
		int receive = SSL_read(cli->ssl, buff_out, BUFFER_SZ);
		if (receive > 0){
			if(strlen(buff_out) > 0){
                strcat(msg,buff_out);
				send_message(msg, cli->uid);

				str_trim_lf(buff_out, strlen(buff_out));
				printf("%s -> %s\n", buff_out, cli->name);
			}
		} else if (receive == 0 || strcmp(buff_out, "exit") == 0){
			sprintf(buff_out, "%s has left\n", cli->name);
			send_message(buff_out, cli->uid);
			leave_flag = 1;
		} else {
			printf("ERROR: -1\n");
			leave_flag = 1;
		}

		bzero(msg, BUFFER_SZ);
		bzero(buff_out, BUFFER_SZ);
	}
	delete_client(cli);

	return NULL;
}
