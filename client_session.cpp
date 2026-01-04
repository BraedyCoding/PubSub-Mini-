#include "client_session.hpp"
#include <iostream>
#include <sstream>
#include <unistd.h> // read(), write(), close()
#include <cstring> // std::memcpy
#include <thread>



//takes a socket discriptor and a reference to the broker object 
ClientSession::ClientSession(int socket_fd, Broker& broker)
    : socket_fd_(socket_fd), broker_(broker), running_(true) {}

void ClientSession::start() {

    //initialize the thread which runs the message loop in this function
   std::thread t([this]() { message_loop(); }); 
   t.detach(); //we detach it so that it runs independently

    // Start reading commands from the client
    read_loop();
    // Close socket when done
    close(socket_fd_);
    std::cout << "Client disconnected.\n";
}

void ClientSession::read_loop() {
    char buffer[1024];

    while (running_) {
        //read stores the input string in buffer, and returns the 
        //number of bytes it read 
        ssize_t bytes = read(socket_fd_, buffer, sizeof(buffer) - 1);
        if (bytes <= 0) {
            running_ = false;
            break; // Client disconnected or error
        }

        //sets the last index to null terminator to make it a valid string
        buffer[bytes] = '\0';
        std::string line(buffer); //C string --> std::string (C++ string)

        // Handle multiple lines if client sent them
        std::istringstream iss(line);  //create a object called iss type of input string stream
        //istringstream allows us to treat a string like a stream (like reading from a file or stdin)
        std::string command;

        //each iteration, it reads a line from iss into command
        //when there are no more lines, getline returns false and loop ends
        while (std::getline(iss, command)) {
            if (!command.empty()) {
                handle_command(command);

                //example: 
                // SUBSCRIBE news
                // PUBLISH news Hello World
            }
        }
    }
}


//example: lets say the command was: SUBSCRIBE news
void ClientSession::handle_command(const std::string& line) {
    std::istringstream iss(line);
    std::string cmd;

    //gets the command word (first word) from the input stream and stores it in cmd and removes it from the stream
    iss >> cmd;


    //we do not know yet how the client will write the commands
    //we are just using read, but not sure how they will write commands on their
    //end and pass it into the socket
    if (cmd == "SUBSCRIBE") {
        std::string topic;
        // int clientID;

        //extract topic and clientID from the command
        iss >> topic;
        broker_.subscribe(topic, socket_fd_);
        std::cout << "Client " << socket_fd_ << " subscribed to " << topic << "\n";
        
        // Track this topic locally
        subscribed_topics_.push_back(topic);


    
    
    }
    else if (cmd == "PUBLISH") {

        //first we construct a message object from the command string
        std::string topic;
        iss >> topic;

        std::string body;
        std::getline(iss, body);
        if (!body.empty() && body[0] == ' ') body.erase(0, 1); // remove leading space

        Message msg(topic, body);

        //then we call broker.publish with the message object
        broker_.publish(msg);
        std::cout << "Published to " << topic << ": " << body << "\n";
    }
    else {
        std::cout << "Unknown command: " << line << "\n";
    }
}

//This writes to the client socket with the serialized message string 
void ClientSession::write_message(const Message& msg) {
    std::string out = msg.serialize() + "\n";
    ssize_t sent = write(socket_fd_, out.c_str(), out.size());
    if (sent < 0) {
        std::cerr << "Error sending message to client.\n";
    }
}

//summary: each client has two threads, one is for reading commands and preforming actiosn like subscribea and publish,
//the other one (this function) is for receiving messages when publish occurs.
//all it does is it calls broker.wait_and_receive for each subscribed topic 
void ClientSession::message_loop() {
    while (running_) {
        for (const auto& topic : subscribed_topics_) {
            // This blocks until a message is available for this topic and client
            Message msg = broker_.wait_and_receive(topic, socket_fd_);
            
            // Write it to the client socket
            write_message(msg);
        }
        
    }
}



