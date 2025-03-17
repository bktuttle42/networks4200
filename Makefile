# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -pthread -lssl -lcrypto

# Targets
SERVER = server
CLIENT = client

# Source files
SERVER_SRC = src/server.c
CLIENT_SRC = src/client.c

# Default target
all: build

build: $(SERVER) $(CLIENT)

$(SERVER): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER_SRC)

$(CLIENT): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT_SRC)

run-server:
	./$(SERVER)

run-client:
	./$(CLIENT)

clean:
	rm -f $(SERVER) $(CLIENT)

.PHONY: clean

