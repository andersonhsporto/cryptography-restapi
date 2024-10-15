#ifndef CRYPTO_H
# define CRYPTO_H

#include <openssl/evp.h>
#include <openssl/sha.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#define SHA256_DIGEST_LENGTH 32

void sha512_string(char *string, unsigned char outputBuffer[SHA512_DIGEST_LENGTH]);
#endif
