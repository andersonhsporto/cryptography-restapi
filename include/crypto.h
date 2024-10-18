#ifndef CRYPTO_H
# define CRYPTO_H

#include <openssl/evp.h>
#include <openssl/sha.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


char *sha512_string(char *string);
#endif
