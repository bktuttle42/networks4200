# Design Explanation

## Overview
This document explains the design and implementation of the Basic TCP Client-Server Communication project. The project demonstrates secure, multi-threaded communication between a client and a server using TCP sockets and AES encryption.

## Communication Protocol
- The server listens on port 8080 for incoming TCP connections.
- Clients connect to the server, send an encrypted message, and receive an encrypted acknowledgment.
- The communication flow ensures that the data is protected during transit.

## Encryption Implementation
- **Algorithm:** AES-256-CBC using OpenSSL.
- **Key and IV:** Both the client and server use a fixed 32-byte key and a 16-byte IV.
- **Functions:** The `encrypt()` and `decrypt()` functions are implemented to handle encryption and decryption, ensuring confidentiality of messages.

## Multi-threading
- The server uses multi-threading (via the pthread library) to manage multiple client connections simultaneously.
- A new thread is created for each client, allowing concurrent processing of incoming messages without blocking the main thread.

## Error Handling
- The code includes error checking for socket creation, connection, and data transmission.
- Encryption and decryption errors are handled by printing error messages and exiting gracefully.

## Build and Deployment
- A Makefile is provided to build, run, and clean the project.
- Dependencies include the OpenSSL and pthread libraries.
- The build process compiles both server and client applications with the necessary flags.

## Future Enhancements
- Implement a dynamic key exchange mechanism to enhance security.
- Add message integrity verification (e.g., HMAC) to ensure data has not been tampered with.

