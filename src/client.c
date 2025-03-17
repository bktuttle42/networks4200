#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/evp.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Fixed key and IV (must match the server)
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

int main() {
    int sock;
    struct sockaddr_in server;
    char message[BUFFER_SIZE];
    unsigned char encrypted_message[BUFFER_SIZE];
    unsigned char server_reply[BUFFER_SIZE];
    unsigned char decrypted_reply[BUFFER_SIZE];
    int encrypted_len, decrypted_len;

    // Initialize OpenSSL algorithms
    OpenSSL_add_all_algorithms();

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Could not create socket");
        exit(EXIT_FAILURE);
    }
    
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // Change if server is remote
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Connect failed");
        close(sock);
        exit(EXIT_FAILURE);
    }
    
    printf("Connected to server\n");
    printf("Enter message: ");
    fgets(message, BUFFER_SIZE, stdin);
    message[strcspn(message, "\n")] = '\0'; // Remove newline

    // Encrypt the message
    encrypted_len = encrypt((unsigned char *)message, strlen(message), key, iv, encrypted_message);

    // Send encrypted message
    if (send(sock, encrypted_message, encrypted_len, 0) < 0) {
        perror("Send failed");
        close(sock);
        exit(EXIT_FAILURE);
    }
    
    // Receive encrypted reply from server
    int reply_len = recv(sock, server_reply, BUFFER_SIZE, 0);
    if (reply_len < 0) {
        perror("Receive failed");
        close(sock);
        exit(EXIT_FAILURE);
    }
    
    // Decrypt the reply
    decrypted_len = decrypt(server_reply, reply_len, key, iv, decrypted_reply);
    decrypted_reply[decrypted_len] = '\0';

    printf("Server reply (decrypted): %s\n", decrypted_reply);
    close(sock);
    return 0;
}
