#include "crypto.h"

void sha512_string(char *string, unsigned char outputBuffer[SHA512_DIGEST_LENGTH]) {
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    const EVP_MD *md = EVP_get_digestbyname("SHA512");

    if (!md) {
        fprintf(stderr, "SHA-512 algorithm not found.\n");
        exit(1);
    }

    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, string, strlen(string));
    unsigned int outputLength;
    EVP_DigestFinal_ex(mdctx, outputBuffer, &outputLength);
    EVP_MD_CTX_free(mdctx);
}
