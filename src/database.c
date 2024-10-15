#include "database.h"

int init_database() {
    sqlite3 *db;
    char *err_msg = NULL;
    int rc = sqlite3_open("database.db", &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Erro ao abrir o banco de dados: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return SQLITE_ERROR;
    }

    rc = sqlite3_exec(db, CREATE_TABLE_QUERY, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Erro ao criar a tabela: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return SQLITE_ERROR;
    }
    printf("Tabela criada com sucesso!\n");
    sqlite3_close(db);
    return SQLITE_OK;
}

int insert_data(unsigned char *document, unsigned char *credit_token, long value) {
    sqlite3 *db;
    char *err_msg = NULL;
    int rc = sqlite3_open("database.db", &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Erro ao abrir o banco de dados: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return SQLITE_ERROR;
    }

    char query[1024];
    sprintf(query, INSERT_QUERY, document, credit_token, (double)value);

    rc = sqlite3_exec(db, query, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Erro ao inserir dados: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return SQLITE_ERROR;
    }

    fprintf(stdout, "Dados inseridos com sucesso! %s %s %ld\n", document, credit_token, value);
    return SQLITE_OK;
}

int fetch_data(int client_socket, long id, int is_array) {
    sqlite3 *db;
    sqlite3_stmt *res;
    int rc;
    char *json;
    const char *sql = is_array ? SELECT_ALL_QUERY : SELECT_QUERY;

    rc = sqlite3_open("database.db", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Erro ao abrir o banco de dados: %s\n", sqlite3_errmsg(db));
        send_response(client_socket, "500 Internal Server Error", "application/json", "{\"error\": \"Database error\"}");
        return SQLITE_ERROR;
    }

    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    if (rc == SQLITE_OK && id > 0) {
        sqlite3_bind_int(res, 1, id);
    }

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Erro ao preparar a consulta: %s\n", sqlite3_errmsg(db));
        send_response(client_socket, "500 Internal Server Error", "application/json", "{\"error\": \"Database error\"}");
        sqlite3_close(db);
        return SQLITE_ERROR;
    }

    json = is_array ? convert_to_json_array(res) : convert_to_json(res);

    sqlite3_finalize(res);
    sqlite3_close(db);

    send_response(client_socket, "200 OK", "application/json", json);

    free(json);
    return SQLITE_OK;
}

int exist_by_id(long id) {
    sqlite3 *db;
    sqlite3_stmt *res;
    int rc;
    int exist = 0;

    rc = sqlite3_open("database.db", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Erro ao abrir o banco de dados: %s\n", sqlite3_errmsg(db));
        return SQLITE_ERROR;
    }

    rc = sqlite3_prepare_v2(db, SELECT_QUERY, -1, &res, 0);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(res, 1, id);
    }

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Erro ao preparar a consulta: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return SQLITE_ERROR;
    }

    if (sqlite3_step(res) == SQLITE_ROW) {
        exist = 1;
    }

    sqlite3_finalize(res);
    sqlite3_close(db);

    return exist;

}

int delete_data(int client_socket, long id) {
    sqlite3 *db;
    char *err_msg = NULL;
    int rc = sqlite3_open("database.db", &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Erro ao abrir o banco de dados: %s\n", sqlite3_errmsg(db));
        send_response(client_socket, "500 Internal Server Error", "application/json", "{\"error\": \"Database error\"}");
        sqlite3_close(db);
        return SQLITE_ERROR;
    }

    if (!exist_by_id(id)) {
        send_response(client_socket, "404 Not Found", "application/json", "{\"error\": \"Transaction not found\"}");
        sqlite3_close(db);
        return SQLITE_ERROR;
    }

    char query[1024];
    sprintf(query, DELETE_QUERY, id);

    rc = sqlite3_exec(db, query, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Erro ao deletar dados: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        send_response(client_socket, "500 Internal Server Error", "application/json", "{\"error\": \"Database error\"}");
        return SQLITE_ERROR;
    }

    fprintf(stdout, "Dados deletados com sucesso! %ld\n", id);
    send_response(client_socket, "200 OK", "application/json", "{\"message\": \"Transaction deleted\"}");
    sqlite3_close(db);
    return SQLITE_OK;
}

t_data *extract_data(const char *body) {
    t_data *data = malloc(sizeof(t_data));
    if (!data) {
        return NULL;
    }

    data->document = extract_value(body, "userDocument");
    data->credit_token = extract_value(body, "creditCardToken");
    data->value_str = extract_long_number(body, "value");

    return data;
}

int return_validation_error(t_data *data, int client_socket) {
    char **array = malloc(3 * sizeof(char *));

    array[0] = data->document ? NULL : to_json("error", "userDocument nao pode ser nulo");
    array[1] = data->credit_token ? NULL: to_json("error", "creditCardToken nao pode ser nulo");
    array[2] = data->value_str ? NULL : to_json("error", "value nao pode ser nulo");

    send_response(client_socket, "400 Bad Request", "application/json", to_json_list("errors", array));
    free(array);
    return SQLITE_ERROR;
}

int is_valid_data(t_data *data) {
    return data->document && data->credit_token && data->value_str;
}

void free_data(t_data *data) {
    free(data->document ? data->document : NULL);
    free(data->credit_token ? data->credit_token : NULL);
    free(data->value_str ? data->value_str : NULL);
    free(data);
    data = NULL;
}

void print_data(t_data *data) {
    printf("Dados recebidos: %s %s %s\n", data->document, data->credit_token ? "****": NULL, data->value_str);
}

int add_data(int client_socket, const char *body) {
    t_data *data = extract_data(body);

    if (!is_valid_data(data)) {
        return return_validation_error(data, client_socket);
    }

    unsigned char encrypted_user_document[SHA512_DIGEST_LENGTH];
    unsigned char encrypted_credit_token[SHA512_DIGEST_LENGTH];

    sha512_string(data->document, encrypted_user_document);
    sha512_string(data->credit_token, encrypted_credit_token);

    long value = atol(data->value_str);

    if (insert_data(encrypted_user_document, encrypted_credit_token, value) != SQLITE_OK) {
        send_response(client_socket, "500 Internal Server Error", "application/json", "{\"error\": \"Database error\"}");
        free_data(data);
        return SQLITE_ERROR;
    }

    send_response(client_socket, "201 Created", "application/json", "{\"message\": \"Transaction created\"}");
    free_data(data);
    return SQLITE_OK;
}

int patch_data(int client_socket, const char *body, long id) {
    t_data *data = extract_data(body);

    if (!is_valid_data(data)) {
        return return_validation_error(data, client_socket);
    }

    long value = atol(data->value_str);

    unsigned char encrypted_user_document[SHA512_DIGEST_LENGTH];
    unsigned char encrypted_credit_token[SHA512_DIGEST_LENGTH];

    sha512_string(data->document, encrypted_user_document);
    sha512_string(data->credit_token, encrypted_credit_token);

    char *query = "UPDATE transactions SET userDocument = ?, creditCardToken = ?, value = ? WHERE id = ?";

    sqlite3 *db;
    sqlite3_stmt *res;

    int rc = sqlite3_open("database.db", &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Erro ao abrir o banco de dados: %s\n", sqlite3_errmsg(db));
        send_response(client_socket, "500 Internal Server Error", "application/json", "{\"error\": \"Database error\"}");
        free_data(data);
        return SQLITE_ERROR;
    }

    rc = sqlite3_prepare_v2(db, query, -1, &res, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Erro ao preparar a consulta: %s\n", sqlite3_errmsg(db));
        send_response(client_socket, "500 Internal Server Error", "application/json", "{\"error\": \"Database error\"}");
        sqlite3_close(db);
        free_data(data);
        return SQLITE_ERROR;
    }

    sqlite3_bind_blob(res, 1, encrypted_user_document, SHA512_DIGEST_LENGTH, SQLITE_STATIC);
    sqlite3_bind_blob(res, 2, encrypted_credit_token, SHA512_DIGEST_LENGTH, SQLITE_STATIC);
    sqlite3_bind_double(res, 3, value);

    sqlite3_bind_int(res, 4, id);

    rc = sqlite3_step(res);

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Erro ao atualizar dados: %s\n", sqlite3_errmsg(db));
        send_response(client_socket, "500 Internal Server Error", "application/json", "{\"error\": \"Database error\"}");
        sqlite3_finalize(res);
        sqlite3_close(db);
        free_data(data);
        return SQLITE_ERROR;
    }

    sqlite3_finalize(res);

    send_response(client_socket, "200 OK", "application/json", "{\"message\": \"Transaction updated\"}");
    free_data(data);
    return SQLITE_OK;
}