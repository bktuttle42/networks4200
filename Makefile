CC = gcc
CFLAGS = -Wall -Wextra -pthread
LDFLAGS = -pthread
SRC_DIR = src
OBJ_DIR = obj

SERVER_SRC = $(SRC_DIR)/server.c
CLIENT_SRC = $(SRC_DIR)/client.c

SERVER_OBJ = $(OBJ_DIR)/server.o
CLIENT_OBJ = $(OBJ_DIR)/client.o

TARGET_SERVER = server
TARGET_CLIENT = client

# Create object directory if it doesn't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Rule to build the server executable
$(TARGET_SERVER): $(SERVER_OBJ)
	$(CC) $(SERVER_OBJ) -o $(TARGET_SERVER) $(LDFLAGS)

# Rule to build the client executable
$(TARGET_CLIENT): $(CLIENT_OBJ)
	$(CC) $(CLIENT_OBJ) -o $(TARGET_CLIENT) $(LDFLAGS)

# Compile server source into object file
$(OBJ_DIR)/server.o: $(SERVER_SRC) $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(SERVER_SRC) -o $(OBJ_DIR)/server.o

# Compile client source into object file
$(OBJ_DIR)/client.o: $(CLIENT_SRC) $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(CLIENT_SRC) -o $(OBJ_DIR)/client.o

# Clean up object files and executables
clean:
	rm -rf $(OBJ_DIR) $(TARGET_SERVER) $(TARGET_CLIENT)

# Run both server and client
run: $(TARGET_SERVER) $(TARGET_CLIENT)
	./$(TARGET_SERVER) & ./$(TARGET_CLIENT)
