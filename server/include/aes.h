#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <time.h>
#include <stdlib.h>
#include "client.h"

int aes(client_t *cli);
char *aes_enc(client_t *cli, char *);
char *aes_dec(client_t *cli, char *);
void aes_delete(client_t *cli);
char *gen_rand_string();