#ifndef JSON_H
# define JSON_H

#include "api.h"
#include "server.h"

#include <sqlite3.h>
#include <ctype.h>

char *convert_to_json(sqlite3_stmt *res);
char *convert_to_json_array(sqlite3_stmt *res);
char *extract_value(const char *body, const char *key);
char *extract_long_number(const char *body, const char *key);


char *to_json_list(const char *value, char **messages);
char *to_json(const char *value, const char *message);

#endif
