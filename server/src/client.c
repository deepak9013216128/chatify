#include <ctype.h>
#include "client.h"
#include "sql.h"
#include "path.h"
#include "aes.h"

unsigned int cli_count = 0;
int uid = 10;
client_t *clients[MAX_CLIENTS];

char loginFailed[] = "Login Failed!";
char success[] = "success";
char query[BUFFER_SZ];
char buff_out[BUFFER_SZ];
char username[BUFFER_SZ];
char password[BUFFER_SZ];

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void str_overwrite_stdout()
{
	printf("\r%s", "> ");
	fflush(stdout);
}

void str_trim_lf(char *arr, int length)
{
	int i;
	for (i = 0; i < length; i++) // trim \n
	{
		if (arr[i] == '\n')
		{
			arr[i] = '\0';
			break;
		}
	}
}

/* Add clients to queue */
void queue_add(client_t *cl)
{
	pthread_mutex_lock(&clients_mutex);

	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		if (!clients[i])
		{
			clients[i] = cl;
			break;
		}
	}

	pthread_mutex_unlock(&clients_mutex);
}

/* Remove clients to queue */
void queue_remove(int uid)
{
	pthread_mutex_lock(&clients_mutex);

	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		if (clients[i])
		{
			if (clients[i]->uid == uid)
			{
				clients[i] = NULL;
				break;
			}
		}
	}

	pthread_mutex_unlock(&clients_mutex);
}

/* Send message to all clients except sender */
void send_message_to_all(char *s, int uid)
{
	pthread_mutex_lock(&clients_mutex);
	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		if (clients[i])
		{
			if (clients[i]->uid != uid)
			{
				strcpy(buff_out, aes_enc(clients[i], s));
				if (send(clients[i]->sockfd, buff_out, BUFFER_SZ, 0) <= 0)
				{
					perror("ERROR: write to descriptor failed");
					break;
				}
			}
		}
	}

	pthread_mutex_unlock(&clients_mutex);
}
void send_message(char *s, int uid)
{
	pthread_mutex_lock(&clients_mutex);
	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		if (clients[i])
		{
			if (clients[i]->uid != uid)
			{
				strcpy(buff_out, aes_enc(clients[i], s));
				if (send(clients[i]->sockfd, buff_out, BUFFER_SZ, 0) <= 0)
				{
					perror("ERROR: write to descriptor failed");
					break;
				}
			}
		}
	}

	pthread_mutex_unlock(&clients_mutex);
}

void delete_client(client_t *cli)
{
	/* Delete client from queue and yield thread */
	close(cli->sockfd);
	queue_remove(cli->uid);
	// delete_aes(cli);
	free(cli);
	cli_count--;
	pthread_detach(pthread_self());
}

void login(client_t *cli)
{
	memset(username, 0, sizeof(username));
	memset(password, 0, sizeof(password));
	memset(buff_out, 0, sizeof(buff_out));
	// int bytes;
	// bytes = recv(cli->sockfd, username, BUFFER_SZ, 0);
	// printf("%d", bytes);
	// bytes = recv(cli->sockfd, password, BUFFER_SZ, 0);
	// printf("%d", bytes);
	if (
		recv(cli->sockfd, username, BUFFER_SZ, 0) <= 0 ||
		recv(cli->sockfd, password, BUFFER_SZ, 0) <= 0)
	{
		delete_client(cli);
		return;
	}

	if (strlen(username) < 2 && strlen(password) < 2)
	{
		printf("Didn't enter the name and password.\n");
		strcpy(buff_out, aes_enc(cli, loginFailed));
		send(cli->sockfd, buff_out, BUFFER_SZ, 0);
		return;
	}

	memset(query, 0, sizeof(query));
	memset(buff_out, 0, sizeof(buff_out));

	strcpy(username, aes_dec(cli, username));
	strcpy(password, aes_dec(cli, password));

	strcat(query, "SELECT password from Users WHERE username = '");
	strcat(query, username);
	strcat(query, "' AND password = '");
	strcat(query, password);
	strcat(query, "'");
	sqlite3_stmt *res = getUserData(query);
	puts(query);
	str_overwrite_stdout();
	int step = sqlite3_step(res);
	if (step != SQLITE_ROW)
	{
		strcpy(buff_out, aes_enc(cli, loginFailed));
		send(cli->sockfd, buff_out, BUFFER_SZ, 0);
		return;
	}
	const char *dbPassword = (const char *)sqlite3_column_text(res, 0);
	if (dbPassword && strcmp(dbPassword, password) == 0)
	{

		/* Add client to the queue and fork thread */
		queue_add(cli);

		str_overwrite_stdout();
		strcpy(cli->name, username);
		strcpy(buff_out, aes_enc(cli, success));
		send(cli->sockfd, buff_out, BUFFER_SZ, 0);
		memset(buff_out, 0, sizeof(buff_out));
		sprintf(buff_out, "%s has joined\n", username);
		send_message_to_all(buff_out, cli->uid);
		return;
	}
	strcpy(buff_out, aes_enc(cli, success));
	send(cli->sockfd, buff_out, BUFFER_SZ, 0);
	return;
}

void signup(client_t *cli)
{
	memset(username, 0, sizeof(username));
	memset(password, 0, sizeof(password));
	memset(buff_out, 0, sizeof(buff_out));

	if (
		recv(cli->sockfd, username, BUFFER_SZ, 0) <= 0 ||
		recv(cli->sockfd, password, BUFFER_SZ, 0) <= 0)
	{
		delete_client(cli);
		return;
	}

	if (strlen(username) < 2 && strlen(password) < 2)
	{
		printf("Didn't enter the name and password.\n");
		strcpy(buff_out, aes_enc(cli, loginFailed));
		send(cli->sockfd, buff_out, BUFFER_SZ, 0);
		return;
	}

	memset(buff_out, 0, sizeof(buff_out));
	memset(query, 0, sizeof(query));

	strcpy(username, aes_dec(cli, username));
	strcpy(password, aes_dec(cli, password));

	strcat(query, "INSERT INTO Users (username, password, ip, port) VALUES('");
	strcat(query, username);
	strcat(query, "', '");
	strcat(query, password);
	strcat(query, "', '127.0.0.1', 4444);");
	puts(query);
	sqlite3_stmt *res = getUserData(query);
	sqlite3_step(res);

	/* Add client to the queue and fork thread */
	queue_add(cli);

	strcpy(cli->name, username);
	strcpy(buff_out, aes_enc(cli, success));
	send(cli->sockfd, buff_out, BUFFER_SZ, 0);
	sprintf(buff_out, "%s has joined\n", cli->name);
	str_overwrite_stdout();
	send_message_to_all(buff_out, cli->uid);
	return;
}

void send_all(client_t *cli)
{

	memset(buff_out, 0, sizeof(buff_out));
	char msg[BUFFER_SZ];
	memset(msg, 0, sizeof(msg));
	strcat(msg, "[");
	strcat(msg, cli->name);
	strcat(msg, "]: ");
	int receive = recv(cli->sockfd, buff_out, BUFFER_SZ, 0);
	if (receive > 0)
	{
		strcpy(buff_out, aes_dec(cli, buff_out));
		if (strlen(buff_out) > 0)
		{
			strcat(msg, buff_out);
			send_message_to_all(msg, cli->uid);

			str_trim_lf(buff_out, strlen(buff_out));
			printf("%s -> %s\n", msg, cli->name);
		}
	}
	else
	{
		printf("ERROR: -1\n");
	}

	memset(buff_out, 0, sizeof(buff_out));
}

void handle_who(client_t *cli)
{

	pthread_mutex_lock(&clients_mutex);
	char res[BUFFER_SZ];
	memset(res,0,sizeof(res));
	strcat(res, "List of online user are:\n");
	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		if (clients[i] && clients[i]->uid != cli->uid)
		{
			strcat(res, clients[i]->name);
			strcat(res, " \n");
		}
	}
	strcpy(buff_out, aes_enc(cli, res));
	if (send(cli->sockfd, buff_out, BUFFER_SZ, 0) <= 0)
	{
		perror("ERROR: write to descriptor failed");
	}
	memset(res, 0, sizeof(res));
	pthread_mutex_unlock(&clients_mutex);
}

/* Responds to clients requesting a private message to another user.
 * If the user does not exist(or has no nickname), the client will
 * be notified.  */
void handle_say(client_t *cli, char *buff)
{
	pthread_mutex_lock(&clients_mutex);
	// Initialize data types
	char receiver[BUFFER_SZ];
	char message[BUFFER_SZ];
	memset(message,0,sizeof(message));
	memset(receiver,0,sizeof(receiver));
	strcpy(message, "[");
	strcat(message, cli->name);
	strcat(message, "] => ");

	/* Parse through the buffer and find the receiver
	 * of the message and the message to send */
	int i = strlen(SAY);
	while (buff[i] != '\0' && isspace(buff[i]))
	{
		i++;
	}
	int j = i + 1;
	while (buff[j] != '\0' && isgraph(buff[j]))
	{
		j++;
	}

	strncpy(receiver, buff + i, j - i);
	strncat(message, buff + j + 1, strlen(buff));
	puts(message);
	str_overwrite_stdout();
	puts(receiver);
	if (strlen(receiver) > 0 && strlen(message) > 0)
	{
		for (int i = 0; i < MAX_CLIENTS; ++i)
		{
			if (clients[i] && strcmp(clients[i]->name, receiver) == 0)
			{
				strcpy(buff_out, aes_enc(clients[i], message));
				if (send(clients[i]->sockfd, buff_out, BUFFER_SZ, 0) <= 0)
				{
					perror("ERROR: write to descriptor failed");
				}
				memset(receiver, 0, sizeof(receiver));
				memset(message, 0, sizeof(message));

				pthread_mutex_unlock(&clients_mutex);
				return;
			}
		}
	}
	strcpy(buff_out, aes_enc(cli, "sent failed"));
	if (send(cli->sockfd, buff_out, BUFFER_SZ, 0) <= 0)
	{
		perror("ERROR: write to descriptor failed");
	}

	memset(receiver, 0, sizeof(receiver));
	memset(message, 0, sizeof(message));

	pthread_mutex_unlock(&clients_mutex);
}

/* Handle all communication with the client */
void *handle_client(void *arg)
{

	cli_count++;
	client_t *cli = (client_t *)arg;
	aes(cli);
	while (1)
	{
		memset(buff_out, 0, sizeof(buff_out));
		if (recv(cli->sockfd, buff_out, BUFFER_SZ, 0) > 0)
		{

			strcpy(buff_out, aes_dec(cli, buff_out));
			puts(buff_out);
			// Cases of different requests. Else case serves for standard messages.
			if (strncmp(buff_out, LOGIN, strlen(LOGIN)) == 0)
				login(cli);
			else if (strncmp(buff_out, SIGNUP, strlen(SIGNUP)) == 0)
				signup(cli);
			else if (strncmp(buff_out, SENDALL, strlen(SENDALL)) == 0)
				send_all(cli);
			else if (strncmp(buff_out, WHO, strlen(WHO)) == 0)
				handle_who(cli);
			else if (strncmp(buff_out, SAY, strlen(SAY)) == 0)
				handle_say(cli, buff_out);
			else if (strncmp(buff_out, LOGOUT, strlen(LOGOUT)) == 0)
			{
				memset(buff_out, 0, sizeof(buff_out));
				strcpy(buff_out, aes_enc(cli, "Logout successfully"));
				send(cli->sockfd, buff_out, BUFFER_SZ, 0);
				memset(buff_out, 0, sizeof(buff_out));
				sprintf(buff_out, "%s has left\n", cli->name);
				str_overwrite_stdout();
				send_message_to_all(buff_out, cli->uid);
				queue_remove(cli->uid);
				pthread_detach(pthread_self());
			}
		}
		else
		{
			memset(buff_out, 0, sizeof(buff_out));
			sprintf(buff_out, "%s has left\n", cli->name);
			str_overwrite_stdout();
			send_message_to_all(buff_out, cli->uid);
			delete_client(cli); // Assumed dead
			break;
		}
		memset(buff_out, 0, sizeof(buff_out));
	}

	return;
}
