#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>
#include <cstdint>  // for uint32_t

class Message {
public:
    // Constructors
    Message() = default; // default empty message

    Message(const std::string& topic, const std::string& body, int senderID = -1)
        : topic_(topic), body_(body), senderID_(senderID) {}

    // Accessors
    const std::string& getTopic() const { return topic_; }
    const std::string& getBody() const { return body_; }
    int getSenderID() const { return senderID_; }

    // Mutators 
    void setTopic(const std::string& topic) { topic_ = topic; }
    void setBody(const std::string& body) { body_ = body; }
    void setSenderID(int id) { senderID_ = id; }

    // Serialize message to a byte string for network transmission
    std::string serialize() const;

    // Deserialize message from a byte buffer (we make static because the client will have a string of serialized data and doesn't have access to the original message object)
    static Message deserialize(const char* data, size_t len);

private:
    std::string topic_;
    std::string body_;
    int senderID_ = -1; // optional: -1 means unknown sender
};

#endif // MESSAGE_HPP
