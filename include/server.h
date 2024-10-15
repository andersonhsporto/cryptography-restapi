#ifndef SERVER_H
# define SERVER_H

#include <arpa/inet.h>
#include "api.h"

#define PORT 8080
#define BUFFER_SIZE 4096

typedef struct api_server {
    int server_socket;
    int client_socket;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len;
    int port;
    int buffer_size;
} t_api_server;

t_api_server    *init_server();
void send_response(int client_socket, const char *status, const char *content_type, const char *body);
void handle_client(int client_socket);
void route_request(int client_socket, const char *request);

#endif
