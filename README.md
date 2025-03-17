# Basic TCP Client-Server Communication

## Overview
This project implements a basic TCP client-server communication system using C and TCP sockets.  
Messages are encrypted using AES-256-CBC with OpenSSL, ensuring secure transmission between the client and server.

## Features
- Multi-threaded TCP server that handles multiple client connections concurrently.
- Encrypted communication using AES-256-CBC.
- Client sends a user-provided message and receives an encrypted acknowledgment from the server.
- Simple Makefile to build, run, and clean the project.

## Requirements
- GCC
- OpenSSL library (e.g., `libssl-dev` on Ubuntu)
- pthread library (usually part of the standard C library)

## Setup and Installation

1. **Clone the repository:**
   ```bash
   git clone <repository-url>
   cd <repository-directory>
