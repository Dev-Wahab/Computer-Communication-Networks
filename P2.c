#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080

void communicate_with_server(const char *base_string) {
    int client_socket;
    struct sockaddr_in server_addr;
    char password[1024];

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error in socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(&(server_addr.sin_zero), '\0', 8);

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error in connect");
        exit(EXIT_FAILURE);
    }

    // Send the base string to the server
    if (send(client_socket, base_string, strlen(base_string), 0) == -1) {
        perror("Error in sending base string");
        exit(EXIT_FAILURE);
    }

    // Receive the password from the server
    int bytes_received = recv(client_socket, password, sizeof(password), 0);
    if (bytes_received <= 0) {
        perror("Error in receiving password");
        exit(EXIT_FAILURE);
    }

    password[bytes_received] = '\0';

    printf("Strong Password: %s\n", password);

    close(client_socket);
}

int main() {
    const char *base_string = "BaseString";

    communicate_with_server(base_string);

    return 0;
}
