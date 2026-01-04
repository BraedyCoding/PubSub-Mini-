#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> // close()
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <thread>
#include <vector>
#include "broker.hpp"
#include "client_session.hpp"

//First, it creates a listening socket,
//it hten accepts connections from clients
//for each connection, it creates a ClientSession object


int main() {
    Broker broker;

    int port = 12345;

    //this returns a index into the file table and instead of it being 
    //an indode, it points to a socket object
    int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock < 0) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    //now that we have the socket object created, we need to create the
    //address structure which tells the OS which IP and port to listen on

    sockaddr_in server_addr{}; //structure which tells OS which IP and Port socket will listen on
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port); //converts hosts byte order to network byte order
    server_addr.sin_addr.s_addr = INADDR_ANY; //tells it that it can accept any type of local network interfaces (LAN, WIFI)

    //binding it makes the socket now basically own that IP address on the computer
    if (bind(listen_sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind failed\n";
        return 1;
    }

    //listen() tells the OS: “I want to accept incoming connections on this socket.”
    // The second argument (5) is the backlog, i.e., the max number of pending connections waiting to be accepted.
    if (listen(listen_sock, 5) < 0) {
        std::cerr << "Listen failed\n";
        return 1;
    }

    std::cout << "Server listening on port " << port << "\n";


    //here we create a dynamic array of session threads, it is easier to manage them this way
    std::vector<std::thread> session_threads;
   

    while (true) {
      
        //This blocks the main thread until a client connects to the listening socket
        int client_sock = accept(listen_sock, nullptr, nullptr);

        //If the connection fails, we go back up and wait for connection again. If not, then we go down
        //and run the code after the if statement
        if (client_sock < 0) {
            std::cerr << "Accept failed\n";
            continue;
        }

        std::cout << "Client connected!\n";

        // Create a session for this client (session is  a smart pointer to ClientSession object on heap)
        std::shared_ptr session = std::make_shared<ClientSession>(client_sock, broker);

        // Launch a thread for this session
        session_threads.emplace_back([session]() { session->start(); });
        session_threads.back().detach(); //WE DON'T WANT TO USE JOIN, THAT WOULD PAUSE THE MAIN THREAD AND NOT ALLOW OTHER CONNECTIONS, SO WE DETACH IT
        //eplace_back constructs a new thread object and places it at the end of the vector
        //and the thread runs the lambda function immediately
    }

    //close the listening socket
    close(listen_sock);
    return 0;
}

//I think we will have to create a literal client thread which connects to the servers listening socket, and also calls on wait_and_receive.