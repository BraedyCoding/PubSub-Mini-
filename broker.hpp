#ifndef BROKER_HPP
#define BROKER_HPP

#include "message.hpp"
#include <cstdint>  // for uint32_t
#include <cstring>  // for std::memcpy
#include <string>
#include <set>
#include <map>
#include <mutex>
#include <queue>
#include <condition_variable>

class Topic {
public:
    std::set<int> subscribers;
    std::queue<Message> messages;  // store Message objects now

    std::mutex mtx;  // optional, per-topic mutex
};

class Broker {
public:
    void subscribe(const std::string &topicName, int clientID);
    void unsubscribe(const std::string &topicName, int clientID);

    // Publisher now takes a Message object
    void publish(const Message &msg);

    // Subscriber thread now returns a Message object
    Message wait_and_receive(const std::string &topicName, int clientID);

private:

//          topic name     Topic object
    std::map<std::string, Topic> topics;
    std::mutex brokerMutex;        // protects topics map
    std::condition_variable cv;    // wakes sleeping subscribers
};

#endif


//idea:We will create a broker object on stack in main.
//we will then create threads which represent clients. In each thread, we will on a method which 
//passes a client id and topic name to the broker.subscribe method. That method will subscribe the client
//to it.