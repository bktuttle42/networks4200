#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

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

int main() {
    int sock;
    struct sockaddr_in server;
    char message[BUFFER_SIZE], encrypted_message[BUFFER_SIZE];
    char key[] = "mysecretkey";  // Example XOR key

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Could not create socket");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    // Resolve server address
    if (inet_pton(AF_INET, "127.0.0.1", &server.sin_addr) <= 0) {
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // Get message from user
    printf("Enter message: ");
    fgets(message, BUFFER_SIZE, stdin);
    message[strcspn(message, "\n")] = '\0';  // Remove trailing newline

    // Encrypt the message using XOR
    xor_encrypt_decrypt(message, key, encrypted_message);

    // Send encrypted message to server
    if (send(sock, encrypted_message, strlen(encrypted_message), 0) < 0) {
        perror("Send failed");
        exit(EXIT_FAILURE);
    }

    // Receive acknowledgment from server
    int read_size;
    if ((read_size = recv(sock, encrypted_message, BUFFER_SIZE, 0)) > 0) {
        char decrypted_ack[BUFFER_SIZE];
        xor_encrypt_decrypt((char *)encrypted_message, key, decrypted_ack);
        printf("Server reply (decrypted): %s\n", decrypted_ack);
    }

    close(sock);
    return 0;
}
