#include "socket.h"
#include "window.h"
#include "chat.h"
#include "auth.h"
#include "ssl.h"
#include "path.h"
#include "aes.h"

#define LENGTH 2048

extern GtkWidget *chatBox;
extern GtkWidget *authBox;
// Global variables
int sockfd = 0;
char name[32];
pthread_t watcher;
SSL *ssl;
char buff[LENGTH];
int loginStatus = 0;

void str_overwrite_stdout()
{
    printf("%s", "> ");
    fflush(stdout);
}

void str_trim_lf(char *arr, int length)
{
    int i;
    for (i = 0; i < length; i++)
    { // trim \n
        if (arr[i] == '\n')
        {
            arr[i] = '\0';
            break;
        }
    }
}

void send_msg_handler(gchar *message)
{
    memset(buff, 0, strlen(buff));
    if (strcmp(message, "exit") == 0)
    {
        close(sockfd);
        closeWindow();
        exit(0);
        return;
    }
    else if (strncmp(message, WHO, strlen(WHO)) == 0)
    {
        strcpy(buff, aes_enc(message));
        send(sockfd, buff, LENGTH, 0);
    }
    else if (strncmp(message, SAY, strlen(SAY)) == 0)
    {
        strcpy(buff, aes_enc(message));
        send(sockfd, buff, LENGTH, 0);
    }
    else
    {
        strcpy(buff, aes_enc(SENDALL));
        send(sockfd, buff, LENGTH, 0);

        memset(buff, 0, strlen(buff));
        strcpy(buff, aes_enc(message));
        send(sockfd, buff, LENGTH, 0);
    }
}

void *recv_msg_handler()
{
    char msg[LENGTH];
    while (1)
    {
        memset(msg, 0, sizeof(msg));

        int receive = recv(sockfd, msg, LENGTH, 0);
        strcpy(msg, aes_dec(msg));
        if (receive > 0 && loginStatus == 1)
        {
            puts(msg);
            recv_item(msg);
            str_overwrite_stdout();
        }
        else
        {
            break;
        }
    }

    pthread_exit(NULL);
}

void logout()
{
    g_print("logout");
    strcpy(buff, aes_enc(LOGOUT));
    send(sockfd, buff, LENGTH, 0);
    loginStatus = 0;
    // pthread_kill(watcher, SIGUSR1);
    pthread_detach(watcher);
    // closeWindow();
    changeWidget(chatBox, authBox);
}

int connectToServer(char *username, char *password, char *isLoginForm)
{

    strcpy(buff, aes_enc(isLoginForm));
    send(sockfd, buff, LENGTH, 0);

    strcpy(buff, aes_enc(username));
    send(sockfd, buff, LENGTH, 0);

    strcpy(buff, aes_enc(password));
    send(sockfd, buff, LENGTH, 0);
    memset(buff, 0, sizeof(buff));
    g_print("reading");
    char msg[LENGTH];
    int bytes;
    bytes = recv(sockfd, msg, LENGTH, 0);
    strcpy(msg, aes_dec(msg));
    g_print(msg);
    if (bytes > 0 && strcmp(msg, "success") == 0)
    {
        loginStatus = 1;
        show_info(msg);
        int rc = 0;
        int i = 0;
        rc = pthread_create(&watcher, NULL, recv_msg_handler, (void *)i);
        changeWidget(authBox, chatBox);
        if (rc)
        {
            printf("Error:unable to create thread, %d\n", rc);
            exit(-1);
        }
        return 1;
    }
    show_error(msg);
    return 0;
}
int createSockit(int argc, char *argv[])
{

    if (argc != 3)
    {
        printf("Usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *ip = argv[1] ? argv[1] : "127.0.0.1";
    int port = atoi(argv[2]) ? atoi(argv[2]) : 4444;
    signal(SIGINT, SIG_IGN);

    // Initialize the SSL library
    SSL_CTX *ctx;
    ctx = InitCTX();

    struct sockaddr_in server_addr;

    /* Socket settings */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    // Connect to Server
    int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (err == -1)
    {
        printf("ERROR: connect\n");
        close(sockfd);
        return EXIT_FAILURE;
    }

    // ===================================== SSL ===================================== //
    ssl = SSL_new(ctx);      /* create new SSL connection state */
    SSL_set_fd(ssl, sockfd); /* attach the socket descriptor */
    if (SSL_connect(ssl) == FAIL)
    { /* perform the connection */
        ERR_print_errors_fp(stderr);
        close(sockfd);
        return EXIT_FAILURE;
    }

    ShowCerts(ssl); /* get any certs */
                    // ===================================== SSL ===================================== //

    printf("=== WELCOME TO THE CHATROOM ===\n");

    char *aes_key;
    aes_key = gen_rand_string();
    aes(aes_key);
    SSL_write(ssl, aes_key, strlen(aes_key));
    // close(sockfd);          close socket
    // SSL_CTX_free(ctx);        /* release context */

    return EXIT_SUCCESS;
}
