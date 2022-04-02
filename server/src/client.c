#include "client.h"
#include "sql.h"

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
				if (SSL_write(clients[i]->ssl, s, BUFFER_SZ) <= 0)
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
				if (SSL_write(clients[i]->ssl, s, BUFFER_SZ) <= 0)
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
	free(cli);
	cli_count--;
	pthread_detach(pthread_self());
}

void login(client_t *cli)
{
	memset(username, 0, sizeof(username));
	memset(password, 0, sizeof(password));
	memset(buff_out, 0, sizeof(buff_out));

	if (
		SSL_read(cli->ssl, username, sizeof(username)) <= 0 ||
		SSL_read(cli->ssl, password, sizeof(password)) <= 0)
	{
		delete_client(cli);
		return;
	}

	if (strlen(username) < 2 && strlen(password) < 2)
	{
		printf("Didn't enter the name and password.\n");
		SSL_write(cli->ssl, loginFailed, sizeof(loginFailed));
		return;
	}

	memset(query, 0, sizeof(query));
	memset(buff_out, 0, sizeof(buff_out));

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
		SSL_write(cli->ssl, loginFailed, sizeof(loginFailed));
		return;
	}
	const char *dbPassword = (const char *)sqlite3_column_text(res, 0);
	if (dbPassword && strcmp(dbPassword, password) == 0)
	{

		/* Add client to the queue and fork thread */
		queue_add(cli);

		sprintf(buff_out, "%s has joined\n", username);
		str_overwrite_stdout();
		strcpy(cli->name, username);
		SSL_write(cli->ssl, success, sizeof(success));
		send_message_to_all(buff_out, cli->uid);
		return;
	}
	SSL_write(cli->ssl, loginFailed, sizeof(loginFailed));
	return;
}

void signup(client_t *cli)
{
	memset(username, 0, sizeof(username));
	memset(password, 0, sizeof(password));
	memset(buff_out, 0, sizeof(buff_out));

	if (
		SSL_read(cli->ssl, username, sizeof(username)) <= 0 ||
		SSL_read(cli->ssl, password, sizeof(password)) <= 0)
	{
		delete_client(cli);
		return;
	}

	if (strlen(username) < 2 && strlen(password) < 2)
	{
		printf("Didn't enter the name and password.\n");
		SSL_write(cli->ssl, loginFailed, sizeof(loginFailed));
		return;
	}

	memset(buff_out, 0, sizeof(buff_out));
	memset(query, 0, sizeof(query));

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
	sprintf(buff_out, "%s has joined\n", cli->name);
	str_overwrite_stdout();
	SSL_write(cli->ssl, success, sizeof(success));
	send_message_to_all(buff_out, cli->uid);
	return;
}

void send_all(client_t *cli)
{

	memset(buff_out, 0, sizeof(buff_out));
	char msg[BUFFER_SZ];
	strcat(msg, "[");
	strcat(msg, cli->name);
	strcat(msg, "]: ");
	int receive = SSL_read(cli->ssl, buff_out, BUFFER_SZ);
	if (receive > 0)
	{
		if (strlen(buff_out) > 0)
		{
			strcat(msg, buff_out);
			send_message_to_all(msg, cli->uid);

			str_trim_lf(buff_out, strlen(buff_out));
			printf("%s -> %s\n", buff_out, cli->name);
		}
	}
	else
	{
		printf("ERROR: -1\n");
	}

	memset(buff_out, 0, sizeof(buff_out));
	memset(msg, 0, sizeof(msg));
}

/* Handle all communication with the client */
void *handle_client(void *arg)
{

	cli_count++;
	client_t *cli = (client_t *)arg;

	while (1)
	{
		memset(buff_out, 0, sizeof(buff_out));
		if (SSL_read(cli->ssl, buff_out, sizeof(buff_out) - 1) > 0)
		{

			// Cases of different requests. Else case serves for standard messages.
			if (strncmp(buff_out, "/login", 5) == 0)
				login(cli);
			else if (strncmp(buff_out, "/signup", 6) == 0)
				signup(cli);
			else if (strncmp(buff_out, "/sendall", 7) == 0)
				send_all(cli);
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
	}

	return NULL;
}
