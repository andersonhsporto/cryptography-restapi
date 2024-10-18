#include "json.h"

char *convert_to_json(sqlite3_stmt *res) {
    char json[BUFFER_SIZE] = "{";
    if (sqlite3_step(res) == SQLITE_ROW) {
        int id = sqlite3_column_int(res, 0);
        const unsigned char *name = sqlite3_column_text(res, 1);
        const unsigned char *email = sqlite3_column_text(res, 2);
        const long value = (long) sqlite3_column_double(res, 3);

        snprintf(json + strlen(json), sizeof(json) - strlen(json), "\"id\": %d, \"userDocument\": \"%s\", \"creditCardToken\": \"%s\", \"value\": %ld", id, name, email, value);
    }
    strcat(json, "}");
    return strdup(json);
}

char *convert_to_json_array(sqlite3_stmt *res) {
    char json[BUFFER_SIZE] = "[";
    int first_row = 1;

    while (sqlite3_step(res) == SQLITE_ROW) {
        int id = sqlite3_column_int(res, 0);
        const unsigned char *name = sqlite3_column_text(res, 1);
        const unsigned char *email = sqlite3_column_text(res, 2);
        const long value = (long) sqlite3_column_double(res, 3);

        if (first_row) {
            first_row = 0;
        } else {
            strcat(json, ",");
        }

        snprintf(json + strlen(json), sizeof(json) - strlen(json), "{\"id\":%d,\"userDocument\":\"%s\",\"creditCardToken\":\"%s\",\"value\":%ld}", id, name, email, value);
    }

    strcat(json, "]");

    return strdup(json);
}

char *extract_value(const char *body, const char *key) {
    char key_with_quotes[256];
    snprintf(key_with_quotes, sizeof(key_with_quotes), "\"%s\":", key);

    char *key_position = strstr(body, key_with_quotes);
    if (!key_position) {
        printf("Chave não encontrada: %s\n", key);
        return NULL;
    }

    char *value_start = strchr(key_position, ':');
    if (!value_start) {
        return NULL;
    }
    value_start++;

    while (*value_start == ' ') {
        value_start++;
    }

    if (*value_start != '\"') {
        return NULL;
    }

    value_start++;

    char *value_end = strchr(value_start, '\"');
    if (!value_end) {
        return NULL;
    }

    size_t value_length = value_end - value_start;
    char *value = malloc(value_length + 1);
    if (!value) {
        return NULL;
    }

    strncpy(value, value_start, value_length);
    value[value_length] = '\0';

    return value; 
}

char *extract_long_number(const char *body, const char *key) {
    char key_with_quotes[256];
    snprintf(key_with_quotes, sizeof(key_with_quotes), "\"%s\":", key);

    char *key_position = strstr(body, key_with_quotes);
    if (!key_position) {
        printf("Key not found: %s\n", key);
        return NULL;
    }

    char *value_start = strchr(key_position, ':');
    if (!value_start) {
        return NULL;
    }
    value_start++;

    while (*value_start == ' ') {
        value_start++;
    }

    if (!isdigit(*value_start) && *value_start != '-') {
        return NULL;
    }

    char *value_end = value_start;
    while (isdigit(*value_end) || *value_end == '.') {
        value_end++;
    }

    size_t value_length = value_end - value_start;
    char *value = malloc(value_length + 1);
    if (!value) {
        return NULL;
    }

    strncpy(value, value_start, value_length);
    value[value_length] = '\0';

    return value;
}

char *to_json(const char *value, const char *message) {
    char json[256];

    snprintf(json, sizeof(json), "{\"%s\": \"%s\"}", value, message);
    return strdup(json);
}

char *to_json_list(const char *value, char **messages) {
    char json[256] = "{\"";

    strcat(json, value);
    strcat(json, "\": [");

    for (int i = 0; i  < 3; i++) {
        if (i > 0) {
            strcat(json, ", ");
        }

        strcat(json, messages[i]);
    }

    strcat(json, "]}");
    return strdup(json);
}