#ifndef API_H
# define API_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

typedef struct s_data {
    long id;
    char *document;
    char *credit_token;
    char *value_str;
} t_data;

#endif
