#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <time.h>
#include <stdlib.h>

int aes(char *);
char *aes_enc(char *);
char *aes_dec(char *);
void aes_delete();
char *gen_rand_string();