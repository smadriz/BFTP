#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8889
#define MAX_CMD_SIZE 1024
#define MAX_IP_SIZE 15

int client_running = 0;
int server_running = 1;

// Server function
void *start_server(void *arg) {
    int server_socket, new_socket;
    struct sockaddr_in server_addr, new_addr;
    socklen_t addr_size;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Failed to create socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Failed to bind to port");
        exit(1);
    }

    if (listen(server_socket, 10) < 0) {
        perror("Failed to listen");
        exit(1);
    }

    addr_size = sizeof(new_addr);
    while (server_running) {
        new_socket = accept(server_socket, (struct sockaddr*)&new_addr, &addr_size);
        
        // Handle the accepted connection in a new thread...
        // Process commands and manage file transfer here
    }
    
    // Cleanup and close server socket before exit
    close(server_socket);

        return NULL;

}

// Client function
void *start_client(void *arg) {
    char *server_ip = (char*)arg;
    int client_socket;
    struct sockaddr_in server_addr;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Failed to create socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Failed to connect to server");
        exit(1);
    }

    // Handle the connected server here
    // Process commands and manage file transfer here

    while (client_running) {
        // Keep doing client work...
    }

    // Cleanup and close client socket before exit
    close(client_socket);

    return NULL;
}

// Main function
int main() {
    char cmd[MAX_CMD_SIZE];
    pthread_t server_thread, client_thread;

    // Start the server thread
    pthread_create(&server_thread, NULL, start_server, NULL);

    while (1) {
        printf("bftp> ");
        fgets(cmd, MAX_CMD_SIZE, stdin);
        cmd[strlen(cmd)-1] = '\0';  // Remove the trailing newline

        if (strncmp(cmd, "open", 4) == 0) {
            char ip[MAX_IP_SIZE];
            sscanf(cmd, "open %s", ip);
            client_running = 1;
            pthread_create(&client_thread, NULL, start_client, (void*)ip);
        } else if (strcmp(cmd, "close") == 0) {
            client_running = 0; // Stop the client
            pthread_join(client_thread, NULL); // Wait for the client thread to finish
        } else if (strcmp(cmd, "quit") == 0) {
            client_running = 0; // Stop the client
            server_running = 0; // Stop the server
            pthread_join(client_thread, NULL); // Wait for the client thread to finish
            pthread_join(server_thread, NULL); // Wait for the server thread to finish
            break;
        }
    }
    return 0;
}

