Design Explanation
Overview

This document explains the design and implementation of the Basic TCP Client-Server Communication project. The project demonstrates secure, multi-threaded communication between a client and a server using TCP sockets and XOR encryption (without using OpenSSL or other libraries).
Communication Protocol

    The server listens on port 8080 for incoming TCP connections.
    Clients connect to the server, send an encrypted message, and receive an encrypted acknowledgment.
    The communication flow ensures that the data is protected during transit.

Encryption Implementation

    Algorithm: XOR encryption (symmetric encryption using a simple XOR operation).
    Key: Both the client and server use a fixed 16-byte key.
    Functions: The xor_encrypt_decrypt() function handles both encryption and decryption using XOR, ensuring confidentiality of messages.

Multi-threading

    The server uses multi-threading (via the pthread library) to manage multiple client connections simultaneously.
    A new thread is created for each client, allowing concurrent processing of incoming messages without blocking the main thread.

Error Handling

    The code includes error checking for socket creation, connection, and data transmission.
    Encryption and decryption errors are handled by printing error messages and exiting gracefully.

Build and Deployment

    A Makefile is provided to build, run, and clean the project.
    Dependencies include the pthread library.
    The build process compiles both server and client applications with the necessary flags.
