#include "message.hpp"
#include <cstring>  // for std::memcpy

// Serialize the Message into a byte string for sending over TCP
std::string Message::serialize() const { //promise not to modify any attributes of the class
    std::string data;

    //get the lengths of the class attributes topic_ and body_
    //since it is unint32_t it will be 4 bytes long, so the size could be for example 00 00 00 07 for length of 7 characters
    uint32_t topicLen = topic_.size();
    uint32_t bodyLen = body_.size();//.size() returns the numbers of characters in the string (length of the string), and we are storing it in a 4 byte integer

    // Append topic length                                 //uint32_t is 4 bytes long
    data.append(reinterpret_cast<const char*>(&topicLen), sizeof(topicLen)); //we pass in size of topicLen which is 4 bytes (convienent.)
    // Append topic content
    data.append(topic_);
    // Append body length
    data.append(reinterpret_cast<const char*>(&bodyLen), sizeof(bodyLen));
    // Append body content
    data.append(body_);
    // Append sender ID
    data.append(reinterpret_cast<const char*>(&senderID_), sizeof(senderID_));

    return data;

    //example: Lets say we had a message object with: "weather" as topic, "rain coming" as body and 42 as senderID
// [00 00 00 07]      // topicLen = 7, stored in 4 byte integer
// [ w e a t h e r ]  // topic 7 bytes
// [00 00 00 0C]      // bodyLen = 12
// [ r a i n   c o m i n g ]  // body   is 12 bytes 
// [00 00 00 2A]      // senderID = 42   (0x2A) , stored in 4 byte interger

//each character is one byte, so the length which is stored in a 4 byte integer tells us how many bytes to read for the topic and body respectively.
}


// Static method to reconstruct a Message from a byte buffer
Message Message::deserialize(const char* data, size_t len) {
    size_t offset = 0;

    // Read topic length
    uint32_t topicLen;
    std::memcpy(&topicLen, data + offset, sizeof(topicLen));
    offset += sizeof(topicLen);

    // Read topic content
    std::string topic(data + offset, topicLen);
    offset += topicLen;

    // Read body length
    uint32_t bodyLen;
    std::memcpy(&bodyLen, data + offset, sizeof(bodyLen));
    offset += sizeof(bodyLen);

    // Read body content
    std::string body(data + offset, bodyLen);
    offset += bodyLen;

    // Read sender ID
    int senderID;
    std::memcpy(&senderID, data + offset, sizeof(senderID));

    return Message(topic, body, senderID);
}
