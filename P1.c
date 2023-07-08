#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080

void generate_password(const char *base_string, char *password) {
    // Generate a strong password using the base string and a random string of characters
    const char *random_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int random_chars_len = strlen(random_chars);
    strcpy(password, base_string);

    for (int i = 0; i < 8; i++) {
        int index = rand() % random_chars_len;
        password[strlen(password)] = random_chars[index];
    }

    password[strlen(password)] = '\0';
}

void handle_client(int client_socket) {
    char buffer[1024];
    char password[1024];

    // Receive the base string from the client
    int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received <= 0) {
        perror("Error in receiving base string");
        close(client_socket);
        return;
    }

    buffer[bytes_received] = '\0';

    // Generate the strong password
    generate_password(buffer, password);

    // Send the password back to the client
    if (send(client_socket, password, strlen(password), 0) == -1) {
        perror("Error in sending password");
    }

    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    srand(time(NULL));

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error in socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(server_addr.sin_zero), '\0', 8);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error in bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) == -1) {
        perror("Error in listen");
        exit(EXIT_FAILURE);
    }

    printf("Server started, waiting for connections...\n");

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket == -1) {
            perror("Error in accept");
            exit(EXIT_FAILURE);
        }

        if (fork() == 0) {
            close(server_socket);
            handle_client(client_socket);
            exit(EXIT_SUCCESS);
        } else {
            close(client_socket);
        }
    }

    close(server_socket);
    return 0;
}
