#include "crypto.h"

char *sha512_string(char *string) {
    unsigned char hash[SHA512_DIGEST_LENGTH];

    char *output = (char *)malloc(SHA512_DIGEST_LENGTH * 2 + 1);
    if (!output) {
        return NULL;
    }

    SHA512((unsigned char *)string, strlen(string), hash);
    for (int i = 0; i < SHA512_DIGEST_LENGTH; i++) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }

    output[SHA512_DIGEST_LENGTH * 2] = '\0';
    return output;
}