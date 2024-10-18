#ifndef DATABASE_H
# define DATABASE_H

#include "api.h"
#include "server.h"
#include "json.h"
#include "crypto.h"

#include <sqlite3.h>

#define CREATE_TABLE_QUERY \
    "CREATE TABLE IF NOT EXISTS transactions (" \
    "id INTEGER PRIMARY KEY AUTOINCREMENT, " \
    "userDocument TEXT, " \
    "creditCardToken TEXT, "  \
    "value INTEGER);"


#define INSERT_QUERY \
    "INSERT INTO transactions (userDocument, creditCardToken, value) VALUES ('%s', '%s', %f);"

#define SELECT_QUERY \
    "SELECT * FROM transactions WHERE id = ?"

#define SELECT_ALL_QUERY \
    "SELECT * FROM transactions"

#define DELETE_QUERY \
    "DELETE FROM transactions WHERE id = %ld"

int init_database();
int insert_data(char *document, char *credit_token, long value);
int fetch_data(int client_socket, long id, int is_array);
int delete_data(int client_socket, long id);
int add_data(int client_socket, const char *body);
int patch_data(int client_socket, const char *body, long id);

#endif
