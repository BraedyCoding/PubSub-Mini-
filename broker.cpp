#include "broker.hpp"
#include <iostream>




//summary of function: Adds the clientID to the list of subscribers for a particular topic (clientID is the socket_fd of the client)
void Broker::subscribe(const std::string &topicName, int clientID) {
    std::unique_lock<std::mutex> lock(brokerMutex);

    Topic &topic = topics[topicName];  // creates topic if doesn't exist
    topic.subscribers.insert(clientID);


    //I just added this, the only thing about this is that the subscriber can't publish or subscribe to multiple topics at once, its blocked here until a message is received
    //wait_and_receive(topicName, clientID); //ensures that the subscriber is ready to receive messages right after subscribing

    std::cout << "Client " << clientID << " subscribed to " << topicName << "\n";
}


//summary: Removed a clientID from the list of subscribers for a particular object
void Broker::unsubscribe(const std::string &topicName, int clientID) {
    std::unique_lock<std::mutex> lock(brokerMutex);

    auto it = topics.find(topicName);
    if (it == topics.end()) return;

    it->second.subscribers.erase(clientID);

    if (it->second.subscribers.empty())
        topics.erase(it);

    std::cout << "Client " << clientID << " unsubscribed from " << topicName << "\n";
}

// Publish a message to a topic
//summary: Takes a message object (which has a topic string and body string) and adds the message object to the map of topic objects,
//by using the string topic name in the message object.
void Broker::publish(const Message &msg) {
    std::unique_lock<std::mutex> lock(brokerMutex);

    //Important: so a message object has a string topic. Not a Topic object
    //So, in our topic dictionary we have string topic name : Topic Object
    //so we are basically just getting the topic object reference from dictionary
    Topic &topic = topics[msg.getTopic()];
    topic.messages.push(msg);

    std::cout << "Published on " << msg.getTopic()
              << ": " << msg.getBody() << "\n";

    cv.notify_all();  // wake any subscribers waiting
}

//Subscriber thread uses this. (Remeber, it first subscribes them and then calls this function where it puts them to sleep).
Message Broker::wait_and_receive(const std::string &topicName, int clientID) {
    std::unique_lock<std::mutex> lock(brokerMutex);

    //we get pointer to map "Topic object" which has list of subscribers
    Topic &topic = topics[topicName];



    // Wait until client is subscribed AND messages exist
    cv.wait(lock, [&]() {
        return topic.subscribers.count(clientID) && !topic.messages.empty();
    });


    //figure out what to do next from here, we want to return the message to the socket to send to the client and have it displayed on their console.

    Message msg = topic.messages.front(); //copy the message object at front of queue
    topic.messages.pop();

    //write message to client
    return msg;
}

