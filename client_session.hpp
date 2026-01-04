#ifndef CLIENT_SESSION_HPP
#define CLIENT_SESSION_HPP

#include <memory>
#include <string>
#include <thread>
#include <atomic>
#include "broker.hpp"
#include "message.hpp"

class ClientSession : public std::enable_shared_from_this<ClientSession> {
public:
    ClientSession(int socket_fd, Broker& broker);

    void start();  // Start the session (read/write loop)

private:
    void read_loop();            // Reads commands from the client
    void write_message(const Message& msg); // Sends a message to the client
    void message_loop(); // Handles message receiving for a subscribed topic

    
    void handle_command(const std::string& line); 

    int socket_fd_;
    Broker& broker_;
    std::atomic<bool> running_;
    std::vector<std::string> subscribed_topics_;
};

#endif // CLIENT_SESSION_HPP
