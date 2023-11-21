#include "execution.h"
#include "transaction.h"
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define SERVER_PORT 12345

Database *db;

int create_server_socket(void) {
    int server_socket;
    struct sockaddr_in server_address;

    // 创建socket选项
    if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    // 设置socket选项
    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 绑定socket到 IP 地址和端口号
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("bind");
        exit(1);
    }

    // 监听socket
    if(listen(server_socket, 5) < 0) {
        perror("listen");
        exit(1);
    }

    return server_socket;
}

int accept_client_connection(int server_socket) {
    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);

    int client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
    if(client_socket < 0) {
        perror("accept");
        exit(1);
    }
    printf("Accepted connection\n");
    return client_socket;
}



void *handle_client_request(void *arg) {
    int client_socket = *(int *)arg;
    char recv_buffer[1024];
    char response_buffer[1024];
    bool in_transaction = false;
    transaction_t* transaction;
    while(1) {
        int len = recv(client_socket, recv_buffer, sizeof(recv_buffer), 0);
        if(len < 0) {
            perror("recv");
            exit(1);
        }
        recv_buffer[len] = '\0';
        printf("Received: %s\n", recv_buffer);

        response_buffer[0] = '\0';
        char *response = response_buffer;

        if(strcmp(recv_buffer, "begin;") == 0) {
            in_transaction = true;
            transaction = transaction_create(0);

            response = "now in transaction";
            if (send(client_socket, response, strlen(response), 0) < 0) {
                perror("send");
                exit(1);
            }

            continue;
        }

        // check if in a transaction
        if(in_transaction) {
            int result = transaction_handle(db, transaction, recv_buffer, response_buffer);
            if(result == 0) {

                response = "committed";
                in_transaction = false;
                free(transaction);

            } else if(result == -1){

                response = "aborted";
                in_transaction = false;
                free(transaction);

            }else {
                response = "in_transaction";
                if (send(client_socket, response_buffer, strlen(response_buffer), 0) < 0) {
                    perror("send");
                    exit(1);
                }
            }
            if (send(client_socket, response, strlen(response), 0) < 0) {
                perror("send");
                exit(1);
            }

            continue;
        }


        // Parse the request
        char* command = strtok(recv_buffer, "(");
        if (strcmp(command, "insert") == 0) {
            int key = atoi(strtok(NULL, ","));
            int value = atoi(strtok(NULL, ")"));
            db_insert(db, key, value);
            response = "inserted";
        } else if (strcmp(command, "find") == 0) {
            int key = atoi(strtok(NULL, ")"));
            db_find(db, key, response_buffer);
        } else if (strcmp(command, "delete") == 0) {
            int key = atoi(strtok(NULL, ")"));
            db_delete(db, key);
            response = "deleted";
        } else if (strcmp(command, "findrange") == 0) {
            int lb = atoi(strtok(NULL, ","));
            int ub = atoi(strtok(NULL, ")"));
            db_find_range(db, lb, ub, response_buffer);
        } else {
            response = "Unknown command";
        }



        // Send data to client
        if (send(client_socket, response, strlen(response), 0) < 0) {
            perror("send");
            exit(1);
        }
    }

    close(client_socket);
    return NULL;
}

void handle_exit_signal(int signal) {
    db_destroy(db);
    printf("log: exit server\n");
    exit(0);
}
// Server process
void server(void) {
    db = db_create();

    // signal(SIGINT, handle_exit_signal);
    // signal(SIGTERM, handle_exit_signal);

    int server_socket = create_server_socket();
    while (1) {
        int client_socket = accept_client_connection(server_socket);
        
        pthread_t thread;
        if(pthread_create(&thread, NULL, handle_client_request, &client_socket) != 0) {
            perror("pthread_create");
            exit(1);
        }

        pthread_detach(thread);
    }

    db_destroy(db);
}

/*
// Client process
void client() {
    int server_socket = connect_to_server();
    send_request(server_socket);
    receive_response(server_socket);
    close(server_socket);
}
*/

int main(void) {
    printf("log: start server\n");
    server();
    return 0;
}

