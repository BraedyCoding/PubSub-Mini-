# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -pthread

# Source files
SERVER_SRC = main.cpp client_session.cpp broker.cpp message.cpp
CLIENT_SRC = client.cpp message.cpp

# Object files
SERVER_OBJ = $(SERVER_SRC:.cpp=.o)
CLIENT_OBJ = $(CLIENT_SRC:.cpp=.o)

# Executables
SERVER_BIN = server
CLIENT_BIN = client

# Default target: build both server and client
all: $(SERVER_BIN) $(CLIENT_BIN)

# Server build
$(SERVER_BIN): $(SERVER_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Client build
$(CLIENT_BIN): $(CLIENT_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile .cpp to .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean compiled files
clean:
	rm -f $(SERVER_OBJ) $(CLIENT_OBJ) $(SERVER_BIN) $(CLIENT_BIN)

.PHONY: all clean

