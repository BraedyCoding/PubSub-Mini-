#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>     // read(), write(), close()
#include <arpa/inet.h>  // sockaddr_in, inet_addr
#include <cstring>
#include "message.hpp"

void receive_messages(int sock) {
    char buffer[1024];
    while (true) {
        ssize_t bytes = read(sock, buffer, sizeof(buffer));
        if (bytes <= 0) {
            std::cout << "Disconnected from server.\n";
            break;
        }

        // Deserialize the message
        Message msg = Message::deserialize(buffer, bytes);
        std::cout << "[Topic: " << msg.getTopic() 
                  << "] From: " << msg.getSenderID()
                  << " => " << msg.getBody() << "\n";
    }
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12345);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // connect to local server

    if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection failed\n";
        return 1;
    }

    std::cout << "Connected to server!\n";

    // Start a thread to continuously receive messages from the server
    std::thread recv_thread(receive_messages, sock);
    recv_thread.detach();

    // Main thread handles user input to send commands
    std::string input;
    while (true) {
        std::getline(std::cin, input);
        if (input == "QUIT") break;

        // Send the command to the server
        ssize_t sent = write(sock, input.c_str(), input.size());
        if (sent < 0) {
            std::cerr << "Failed to send command\n";
        }
    }

    close(sock);
    std::cout << "Client terminated.\n";
    return 0;
}
