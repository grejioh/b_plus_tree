#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define SERVER_PORT 12345
#define BUFFER_SIZE 1024

int main(void) {
    // Create a socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("socket");
        exit(1);
    }

    // Connect to the server
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("connect");
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    while (1) {
        // Read a command from the user
        printf("> ");
        fgets(buffer, sizeof(buffer), stdin);

        // Send the command to the server
        if (send(client_socket, buffer, strlen(buffer), 0) == -1) {
            perror("send");
            break;
        }

        // Receive the response from the server
        int len = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (len <= 0) {
            break;
        }
        buffer[len] = '\0';

        // Print the response
        printf("%s\n", buffer);
    }

    close(client_socket);
    return 0;

}
