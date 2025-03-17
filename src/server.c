#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// XOR Encryption function (same for encryption and decryption)
void xor_encrypt_decrypt(char *input, char *key, char *output) {
    int i;
    int key_len = strlen(key);
    for (i = 0; i < strlen(input); i++) {
        output[i] = input[i] ^ key[i % key_len];  // XOR each byte with the key
    }
    output[i] = '\0';  // Null-terminate the output string
}

void handleErrors(void) {
    fprintf(stderr, "An error occurred.\n");
    exit(EXIT_FAILURE);
}

void *client_handler(void *socket_desc) {
    int sock = *(int *)socket_desc;
    free(socket_desc);
    unsigned char buffer[BUFFER_SIZE];
    int read_size;

    // Receive encrypted data from client
    if ((read_size = recv(sock, buffer, BUFFER_SIZE, 0)) > 0) {
        unsigned char decrypted_text[BUFFER_SIZE];
        char key[] = "mysecretkey";  // Example XOR key
        xor_encrypt_decrypt((char *)buffer, key, (char *)decrypted_text);
        printf("Received (decrypted): %s\n", decrypted_text);

        // Prepare acknowledgment message
        char ack[] = "Message received";
        unsigned char encrypted_ack[BUFFER_SIZE];
        xor_encrypt_decrypt(ack, key, (char *)encrypted_ack);

        // Send encrypted acknowledgment to client
        send(sock, encrypted_ack, strlen((char *)encrypted_ack), 0);
    }
    close(sock);
    pthread_exit(NULL);
}

int main() {
    int server_fd, new_socket, *new_sock;
    struct sockaddr_in server, client;
    socklen_t client_len = sizeof(struct sockaddr_in);

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Could not create socket");
        exit(EXIT_FAILURE);
    }
    
    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // Bind
    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d\n", PORT);

    // Listen
    listen(server_fd, 3);

    // Accept incoming connections
    while ((new_socket = accept(server_fd, (struct sockaddr *)&client, &client_len))) {
        printf("Connection accepted\n");
        pthread_t sniffer_thread;
        new_sock = malloc(sizeof(int));
        if (new_sock == NULL) {
            perror("Memory allocation failed");
            close(new_socket);
            continue;
        }
        *new_sock = new_socket;
        if (pthread_create(&sniffer_thread, NULL, client_handler, (void *)new_sock) < 0) {
            perror("Could not create thread");
            close(new_socket);
            continue;
        }
        pthread_detach(sniffer_thread);
    }

    if (new_socket < 0) {
        perror("Accept failed");
        close(server_fd);
        return 1;
    }

    close(server_fd);
    return 0;
}
