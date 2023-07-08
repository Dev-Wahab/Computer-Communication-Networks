#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void communicate_with_server(int client_number) {
    int client_socket;
    struct sockaddr_in server_addr;
    char message[1024];

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error in socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(message,0, 1024);

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error in connect");
        exit(EXIT_FAILURE);
    }

    sprintf(message, "Hello from Client %d", client_number);
    send(client_socket, message, strlen(message), 0);

    char response[1024];
    int bytes_received = recv(client_socket, response, sizeof(response) - 1, 0);
    if (bytes_received > 0) {
        response[bytes_received] = '\0';
        printf("Response from Server for Client %d: %s\n", client_number, response);
    }

    close(client_socket);
}

int main() {
    int i;

    for (i = 1; i <= 2; i++) {
        if (fork() == 0) {
            communicate_with_server(i);
            exit(EXIT_SUCCESS);
        }
    }

    return 0;
}
