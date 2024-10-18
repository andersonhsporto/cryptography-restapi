#include "server.h"
#include "database.h"
#include "json.h"

static void handle_delete_transactions(int client_socket, const char *request, const char *path);
static void handle_patch_transactions(int client_socket, const char *request, const char *path);
static void handle_get_transactions(int client_socket, const char *path);

t_api_server    *init_server() {
    t_api_server    *server = malloc(sizeof(t_api_server));

    int server_socket;
    socklen_t client_addr_len = sizeof(struct sockaddr_in);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
    if (server_socket == -1) {
        free(server);
        perror("Erro ao criar o socket");
        exit(EXIT_FAILURE);
    }

    server->server_addr.sin_family = AF_INET;
    server->server_addr.sin_addr.s_addr = INADDR_ANY;
    server->server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server->server_addr, sizeof(server->server_addr)) == -1) {
        perror("Erro no bind");
        free(server);
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 10) == -1) {
        perror("Erro no listen");
        free(server);
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    server->server_socket = server_socket;
    server->client_addr_len = client_addr_len;
    server->port = PORT;
    server->buffer_size = BUFFER_SIZE;

    return server;
}

void send_response(int client_socket, const char *status, const char *content_type, const char *body) {
    char response[BUFFER_SIZE];
    size_t content_length = strlen(body);

    snprintf(response, sizeof(response),
             "HTTP/1.1 %s\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %zu\r\n"
             "\r\n"
             "%s",
             status, content_type, content_length, body);

    write(client_socket, response, strlen(response));
}

void parse_request(const char *header, char *method, char *path) {
    sscanf(header, "%s %s", method, path);
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);

    read(client_socket, buffer, BUFFER_SIZE);
    route_request(client_socket, buffer);
}

void route_request(int client_socket, const char *request) {
    char method[100];
    char path[100];

    parse_request(request, method, path);
    if (strncmp(method, "GET", 3) == 0 && strncmp(path, "/transactions", 13) == 0) {
        handle_get_transactions(client_socket, path);
    } else if (strncmp(method, "POST", 4) == 0 && strncmp(path, "/transactions", 13) == 0) {
        add_data(client_socket, request);
    } else if (strncmp(method, "PATCH", 5) == 0 && strncmp(path, "/transactions", 13) == 0) {
        handle_patch_transactions(client_socket, request, path);
    } else if (strncmp(method, "DELETE", 6) == 0 && strncmp(path, "/transactions", 13) == 0) {
        handle_delete_transactions(client_socket, request, path);
    } else {
        const char *body = "{\"error\": \"Route not found\"}";
        send_response(client_socket, "404 Not Found", "application/json", body);
    }
}

static void handle_get_transactions(int client_socket, const char *path) {
    if (strlen(path) < 14) {
        fetch_data(client_socket, 0, 1);
    } else {
        long id = atol(path + 14);
        fetch_data(client_socket, id, 0);
    }
}

static void handle_patch_transactions(int client_socket, const char *request __attribute__((unused)), const char *path) {
    long id = atol(path + 14);
    if (strlen(path) > 14 && id > 0) {
        patch_data(client_socket, request, id);
    } else {
        const char *body = "{\"error\": \"ID not found\"}";
        send_response(client_socket, "404 Not Found", "application/json", body);
    }
}

static void handle_delete_transactions(int client_socket, const char *request __attribute__((unused)), const char *path) {
    if (strlen(path) > 14) {
        long id = atol(path + 14);
        delete_data(client_socket, id);
    } else {
        const char *body = "{\"error\": \"ID not found\"}";
        send_response(client_socket, "404 Not Found", "application/json", body);
    }
}
