#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <openssl/evp.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Fixed key and IV (same for both encryption and decryption)
unsigned char key[32] = "This is a key123This is a key123"; // 32 bytes
unsigned char iv[16]  = "1234567890123456";                  // 16 bytes

void handleErrors(void) {
    fprintf(stderr, "An error occurred.\n");
    exit(EXIT_FAILURE);
}

int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext) {
    EVP_CIPHER_CTX *ctx;
    int len, ciphertext_len;

    if (!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handleErrors();
    ciphertext_len = len;

    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        handleErrors();
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    return ciphertext_len;
}

int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext) {
    EVP_CIPHER_CTX *ctx;
    int len, plaintext_len;

    if (!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleErrors();
    plaintext_len = len;

    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
        handleErrors();
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    return plaintext_len;
}

void *client_handler(void *socket_desc) {
    int sock = *(int *)socket_desc;
    free(socket_desc);
    unsigned char buffer[BUFFER_SIZE];
    int read_size;

    // Receive encrypted data from client
    if ((read_size = recv(sock, buffer, BUFFER_SIZE, 0)) > 0) {
        unsigned char decrypted_text[BUFFER_SIZE];
        int decrypted_len = decrypt(buffer, read_size, key, iv, decrypted_text);
        decrypted_text[decrypted_len] = '\0';
        printf("Received (decrypted): %s\n", decrypted_text);

        // Prepare acknowledgment message
        char ack[] = "Message received";
        unsigned char encrypted_ack[BUFFER_SIZE];
        int ack_len = encrypt((unsigned char *)ack, strlen(ack), key, iv, encrypted_ack);

        // Send encrypted acknowledgment to client
        send(sock, encrypted_ack, ack_len, 0);
    }
    close(sock);
    pthread_exit(NULL);
}

int main() {
    int server_fd, new_socket, *new_sock;
    struct sockaddr_in server, client;
    socklen_t client_len = sizeof(struct sockaddr_in);

    // Initialize OpenSSL algorithms
    OpenSSL_add_all_algorithms();

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

