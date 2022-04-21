#include "aes.h"

EVP_CIPHER_CTX *en, *de;

/**
 * Create a 256 bit key and IV using the supplied key_data. salt can be added for taste.
 * Fills in the encryption and decryption ctx objects and returns 0 on success
 **/
int aes_init(unsigned char *key_data, int key_data_len, unsigned char *salt, EVP_CIPHER_CTX *e_ctx,
             EVP_CIPHER_CTX *d_ctx)
{
    int i, nrounds = 5;
    unsigned char key[32], iv[32];

    /*
     * Gen key & IV for AES 256 CBC mode. A SHA1 digest is used to hash the supplied key material.
     * nrounds is the number of times the we hash the material. More rounds are more secure but
     * slower.
     */
    i = EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha1(), salt, key_data, key_data_len, nrounds, key, iv);
    if (i != 32)
    {
        printf("Key size is %d bits - should be 256 bits\n", i * 8);
        return -1;
    }

    EVP_CIPHER_CTX_init(e_ctx);
    EVP_EncryptInit_ex(e_ctx, EVP_aes_256_cbc(), NULL, key, iv);
    EVP_CIPHER_CTX_init(d_ctx);
    EVP_DecryptInit_ex(d_ctx, EVP_aes_256_cbc(), NULL, key, iv);

    return 0;
}

/*
 * Encrypt *len bytes of data
 * All data going in & out is considered binary (unsigned char[])
 */
unsigned char *aes_encrypt(EVP_CIPHER_CTX *e, unsigned char *plaintext, int *len)
{
    /* max ciphertext len for a n bytes of plaintext is n + AES_BLOCK_SIZE -1 bytes */
    int c_len = *len + AES_BLOCK_SIZE, f_len = 0;
    unsigned char *ciphertext = malloc(c_len);

    /* allows reusing of 'e' for multiple encryption cycles */
    EVP_EncryptInit_ex(e, NULL, NULL, NULL, NULL);

    /* update ciphertext, c_len is filled with the length of ciphertext generated,
     *len is the size of plaintext in bytes */
    EVP_EncryptUpdate(e, ciphertext, &c_len, plaintext, *len);

    /* update ciphertext with the final remaining bytes */
    EVP_EncryptFinal_ex(e, ciphertext + c_len, &f_len);

    *len = c_len + f_len;
    return ciphertext;
}

/*
 * Decrypt *len bytes of ciphertext
 */
unsigned char *aes_decrypt(EVP_CIPHER_CTX *e, unsigned char *ciphertext, int *len)
{
    /* plaintext will always be equal to or lesser than length of ciphertext*/
    int p_len = *len, f_len = 0;
    unsigned char *plaintext = malloc(p_len);

    EVP_DecryptInit_ex(e, NULL, NULL, NULL, NULL);
    EVP_DecryptUpdate(e, plaintext, &p_len, ciphertext, *len);
    EVP_DecryptFinal_ex(e, plaintext + p_len, &f_len);

    *len = p_len + f_len;
    return plaintext;
}

int aes(char *argv)
{
    en = EVP_CIPHER_CTX_new();
    de = EVP_CIPHER_CTX_new();
    /* 8 bytes to salt the key_data during key generation. This is an example of
       compiled in salt. We just read the bit pattern created by these two 4 byte
       integers on the stack as 64 bits of contigous salt material -
       ofcourse this only works if sizeof(int) >= 4 */
    unsigned int salt[] = {12345, 54321};
    unsigned char *key_data;
    int key_data_len, i;
 

    /* the key_data is read from the argument list */
    key_data = (unsigned char *)argv;
    key_data_len = strlen(argv);

    /* gen key and iv. init the cipher ctx object */
    if (aes_init(key_data, key_data_len, (unsigned char *)&salt, en, de))
    {
        printf("Couldn't initialize AES cipher\n");
        return -1;
    }

    return 0;
}
char *aes_enc(char *msg)
{
    unsigned char *ciphertext;
    int len;

    len = strlen(msg) + 1;

    ciphertext = aes_encrypt(en, (unsigned char *)msg, &len);

    printf("CipherText  : ");
    for (int i = 0; i < strlen((char *)ciphertext); i++)
        printf("%d", ciphertext[i]);
    puts("");

    return ciphertext;
    // free(ciphertext);
}
char *aes_dec(char *ciphertext)
{
    int len;
    char *plaintext;

    len = strlen(ciphertext) + 1;

    plaintext = (char *)aes_decrypt(de, (unsigned char *)ciphertext, &len);

    printf("PlainText  : ");
    for (int i = 0; i < strlen(plaintext); i++)
        printf("%d", plaintext[i]);
    puts("");

    return plaintext;
}
void delete_aes()
{
    EVP_CIPHER_CTX_cleanup(en);
    EVP_CIPHER_CTX_cleanup(de);
}

char *gen_rand_string()
{
    srand(time(NULL)); // define a seed for the random number generator
    const char ALLOWED[] = "abcdefghijklmnopqrstuvwxyz1234567890";
    char *random;
    random = (char*)malloc(33);
    int i = 0;
    int c = 0;
    int nbAllowed = sizeof(ALLOWED) - 1;
    for (i = 0; i < 33; i++)
    {
        c = rand() % nbAllowed;
        random[i] = ALLOWED[c];
    }
    random[32] = '\0';

    return random;
}
