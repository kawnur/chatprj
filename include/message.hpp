#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <cstdint>
#include <memory>
#include <string>

#include "utils.hpp"

class MessageWidget;

class MessageMetaData
{
public:
    bool isValid();

    MessageType messageType_;
    NetworkMessageType networkMessageType_;
    uint32_t messageId_;
    uint8_t companionId_;
    std::string companionName_;
    uint8_t authorId_;
    std::string authorName_;
    std::string timestampTz_;
};

class MessageData
{
public:
    std::string text_;
};

class MessageState
{
public:
    //     MessageState(
    //         uint8_t companionId, bool isAntecedent, bool isSent, bool isReceived,
    //         std::string networkId);

    //     ~MessageState() = default;

    //     bool isAntecedent() const;
        // bool isSent() const;
        // bool isReceived() const;
    //     std::string getNetworkId() const;

    //     void setIsAntecedent(bool);
    //     void setIsReceived(bool);
    //     void setNetworkId(const std::string &);

    // private:
    bool isAntecedent_;
    bool isSent_;
    bool isReceived_;
    std::string networkId_;
};

class Message
{
public:
    // Message(
    //     // MessageType type, uint32_t id, uint8_t companion_id, uint8_t author_id,
    //     // const std::string &time, const std::string &text);
    //     MessageType type, const MessageMetaData &meta, const std::string &text);

    Message(
        std::shared_ptr<MessageMetaData> meta, std::shared_ptr<MessageData> data,
        std::shared_ptr<MessageState> state);

    ~Message();

    MessageType getType() const;
    uint32_t getId() const;
    uint8_t getCompanionId() const;
    uint8_t getAuthorId() const;
    std::string getTime() const;
    std::string getText() const;
    bool isMessageFromMe() const;
    bool isSent() const;
    bool isReceived() const;
    bool isAntecedent() const;
    std::string getNetworkId() const;

    friend bool operator<(const Message &message1, const Message &message2);

private:
    // MessageType type_;
    // uint32_t id_;
    // uint8_t companion_id_;
    // uint8_t author_id_;
    // std::string time_;  // TODO add timezone support
    // std::string text_;
    std::shared_ptr<MessageMetaData> meta_;
    std::shared_ptr<MessageData> data_;
    std::shared_ptr<MessageState> state_;
};

// class MessageInfo
// {
// public:
//     MessageInfo(std::shared_ptr<MessageState>, std::shared_ptr<MessageWidget>);
//     ~MessageInfo() = default;

//     std::shared_ptr<MessageState> getState() const;
//     std::shared_ptr<MessageWidget> getWidget() const;

//     void setWidget(std::shared_ptr<MessageWidget>);

// private:
//     std::shared_ptr<MessageState> state_;
//     std::shared_ptr<MessageWidget> widget_;
// };

#endif // MESSAGE_HPP
