#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <resolv.h>
#include "openssl/ssl.h"
#include "openssl/err.h"

#define FAIL -1

int OpenListener(int port);
int isRoot();
SSL_CTX *InitServerCTX(void);
void LoadCertificates(SSL_CTX *ctx, char *CertFile, char *KeyFile);
void ShowCerts(SSL *ssl);
void Servlet(SSL *ssl);
int createSSLConnection(int count, char *Argc[]);