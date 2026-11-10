#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <cstdint>
#include <string>

#include "alias.hpp"
#include "constants.hpp"

class Message
{
public:
    Message(MessageMetaDataPtr meta, MessageDataPtr data, MessageStatePtr state);
    Message(MessagePtr message);

    ~Message();

    uint32_t id() const;
    uint8_t companionId() const;
    // std::string companionName() const;
    uint8_t authorId() const;
    // std::string authorName() const;
    std::string time() const;
    // std::string hashMD5() const;
    std::string text() const;
    bool isMessageFromMe() const;
    bool isSent() const;
    bool isReceived() const;
    bool isAntecedent() const;
    std::string networkId() const;
    NetworkMessageType networkMessageType() const;

    void setId(int id);
    void setCompanionId(int id);
    void setTime(const std::string &time);

    friend bool operator<(const Message &message1, const Message &message2);

    MessageMetaDataPtr meta() const;
    MessageDataPtr data() const;
    MessageStatePtr state() const;

    void updateMeta(MessageMetaDataPtr meta);

private:
    void log();

    MessageMetaDataPtr meta_;
    MessageDataPtr data_;
    MessageStatePtr state_;
};

class TextMessage : public Message
{
public:
    TextMessage(MessageMetaDataPtr meta, MessageDataPtr data, MessageStatePtr state);
    ~TextMessage();

private:


};

#endif // MESSAGE_HPP
