#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <cstdint>
#include <string>

#include "utils.hpp"

class Message
{
public:
    //    Message(int, int, std::tm, const std::string&, bool);
    Message(uint32_t, uint8_t, uint8_t, const std::string&, const std::string&);
    ~Message() = default;

    uint32_t getId() const;
    uint8_t getCompanionId() const;
    uint8_t getAuthorId() const;
    bool isMessageFromMe() const;
    std::string getTime() const;
    std::string getText() const;
    // bool getIsSent() const;

private:
    uint32_t id_;
    uint8_t companion_id_;
    uint8_t author_id_;
    //    std::tm time_;  // TODO add timezone support
    std::string time_;  // TODO add timezone support
    std::string text_;
    // bool isSent_;
};

class MessageState
{
public:
    MessageState(uint8_t, bool, bool, bool, std::string);
    ~MessageState() = default;

    bool getIsAntecedent() const;
    bool getIsSent() const;
    bool getIsReceived() const;
    std::string getNetworkId() const;
    std::string getMessageMappingKey() const;

    void setIsReceived(bool);
    void setNetworkId(const std::string&);
    void setgetMessageMappingKey(const std::string&);

private:
    bool isAntecedent_;
    bool isSent_;
    bool isReceived_;
    std::string networkId_;
    std::string messageMappingKey_;
};

#endif // MESSAGE_HPP
