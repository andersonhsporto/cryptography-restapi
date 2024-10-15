#include "api.h"
#include "server.h"

static volatile sig_atomic_t interrupt_received = 1;

void handle_sigint(int sig __attribute__((unused))) {
    printf("Sinal de interrupção recebido!\n");
    interrupt_received = 0;
}

struct sigaction init_sigaction() {
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigint;
    sa.sa_flags = 0;
    return sa;
}

int main() {
    t_api_server *server = init_server();
    struct sigaction sa = init_sigaction();

    printf("Servidor rodando na porta %d...\n", PORT);

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    while (interrupt_received) {
        server->client_socket = accept(server->server_socket, (struct sockaddr *)&server->client_addr, &server->client_addr_len);
        if (server->client_socket == -1) {
            perror("Erro no accept");
            continue;
        }

        handle_client(server->client_socket);
        close(server->client_socket);
    }

    printf("Fechando o servidor...\n");
    close(server->server_socket);
    free(server);
    return 0;
}
