#include "sockit.h"

extern int cli_count;
extern int uid;
char *ip = "127.0.0.1";
int port = 4444;//atoi(argv[1]);
int option = 1;
int listenfd = 0, connfd = 0;
struct sockaddr_in serv_addr;
struct sockaddr_in cli_addr;
pthread_t tid;
SSL_CTX *ctx;

int createSockit(int argc, char *argv[]){

    if(argc != 3){
		printf("Usage: %s <port>\n", argv[0]);
        exit(1);
		return EXIT_FAILURE;
	}
    ip = argv[1]?argv[1]:ip;
    port = atoi(argv[2])>0?atoi(argv[2]):port;

    // Initialize the SSL library
    SSL_library_init();
    ctx = InitServerCTX();        /* initialize SSL */
    LoadCertificates(ctx, "mycert.pem", "mycert.pem"); /* load certs */

    /* Socket settings */
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ip);
    serv_addr.sin_port = htons(port);

    /* Ignore pipe signals */
    signal(SIGPIPE, SIG_IGN);

    if(setsockopt(listenfd, SOL_SOCKET,(SO_REUSEADDR  ),(char*)&option,sizeof(option)) < 0){
        perror("ERROR: setsockopt failed");
        exit(1);
        return EXIT_FAILURE;
    }

    /* Bind */
    if(bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR: Socket binding failed");
        exit(1);
        return EXIT_FAILURE;
    }
    printf("Server is bind on IP: %s PORT: %d\n",ip,port);
    return 0;
}

int listenSocktetConnection(){

    /* Listen */
    if (listen(listenfd, 10) < 0) {
        perror("ERROR: Socket listening failed");
        exit(1);
        return EXIT_FAILURE;
    }

    printf("Server is listening on IP: %s PORT: %d\n",ip,port);

    while(1){
        socklen_t clilen = sizeof(cli_addr);
        connfd = accept(listenfd, (struct sockaddr*)&cli_addr, &clilen);
        /* Check if max clients is reached */
        if((cli_count + 1) == MAX_CLIENTS){
            printf("Max clients reached. Rejected: \n");
            print_client_addr(cli_addr);
            printf(":%d\n", cli_addr.sin_port);
            close(connfd);
            continue;
        }

        SSL *ssl;
        // ===================================== SSL ===================================== //
        ssl = SSL_new(ctx);              /* get new SSL state with context */
        SSL_set_fd(ssl, connfd);      /* set connection socket to SSL state */

        if ( SSL_accept(ssl) == FAIL ) {    /* do SSL-protocol accept */
            ERR_print_errors_fp(stderr);
            continue;    
        }
        ShowCerts(ssl);        /* get any certificates */
        
        /* Client settings */
        client_t *cli = (client_t *)malloc(sizeof(client_t));
        cli->address = cli_addr;
        cli->sockfd = connfd;
        cli->uid = uid++;
        cli->ssl = ssl;

        /* Add client to the queue and fork thread */
        queue_add(cli);


        pthread_create(&tid, NULL, &handle_client, (void*)cli);

        /* Reduce CPU usage */
        sleep(1);
    }
    SSL_CTX_free(ctx);         /* release context */
}
