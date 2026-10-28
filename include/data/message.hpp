#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <cstdint>
#include <memory>
#include <string>

#include "alias.hpp"
#include "utils.hpp"

class PriorityTask
{
public:
    PriorityTask(
        uint8_t priority, const std::string &key, std::function<void()> function,
        bool isMandatory = true);

    ~PriorityTask();

    uint8_t priority() const;
    std::string key() const;
    std::function<void()> func() const;
    bool isMandatory() const;

private:
    uint8_t priority_;
    std::string key_;
    std::function<void()> func_;
    bool isMandatory_;
};

class PriorityTaskQueueWrapper
{
public:
    PriorityTaskQueueWrapper(const std::vector<PriorityTask> &tasks);
    ~PriorityTaskQueueWrapper();

    bool runTasksAndReturnResult();

private:
    PriorityTaskQueue queue_;
};

class MessageMetaData
{
public:
    MessageMetaData() = default;

    MessageMetaData(
        MessageType messageType, NetworkMessageType networkMessageType, uint32_t messageId,
        uint8_t companionId, const std::string &companionName, uint8_t authorId,
        const std::string &authorName, const std::string &timestampTz, const std::string &hashMD5,
        const std::string networkId);

    MessageMetaData(MessageMetaDataPtr meta);

    bool setFields(CompanionPtr companion, const nlohmann::json &data);
    bool isValid();
    void update(MessageMetaDataPtr meta);
    bool isMessageFromMe();

    MessageType messageType() const;
    NetworkMessageType networkMessageType() const;
    uint32_t messageId() const;
    uint8_t companionId() const;
    std::string companionName() const;
    uint8_t authorId() const;
    std::string authorName() const;
    std::string timestampTz() const;
    std::string hashMD5() const;
    std::string networkId() const;

    void setMessageId(uint32_t newMessageId);
    void setCompanionId(uint8_t newCompanionId);
    void setTimestampTz(const std::string &newTimestampTz);
    void setNetworkId(const std::string &newNetworkId);
    void setNetworkMessageType(NetworkMessageType newNetworkMessageType);
    void setMessageType(MessageType newMessageType);
    void setCompanionName(const std::string &newCompanionName);
    void setAuthorName(const std::string &newAuthorName);

    void log();

private:
    MessageType messageType_;
    NetworkMessageType networkMessageType_;
    uint32_t messageId_;
    uint8_t companionId_;
    std::string companionName_;
    uint8_t authorId_;
    std::string authorName_;
    std::string timestampTz_;
    std::string hashMD5_;
    std::string networkId_;
};

class MessageData
{
public:
    MessageData() = default;
    MessageData(const std::string &text, const std::string &data);
    MessageData(MessageDataPtr data);

    ~MessageData() = default;

    std::string text() const;
    std::string data() const;

    bool setFields(CompanionPtr companion, const nlohmann::json &data);
    void setText(const std::string &newText);
    void setData(const std::string &newData);

    void log();

private:
    std::string text_;
    std::string data_;  // for file transfer
};

class MessageState
{
public:
    MessageState() = default;
    MessageState(bool isAntecedent, bool isSent, bool isReceived);
    MessageState(MessageStatePtr state);

    ~MessageState() = default;

    bool isAntecedent() const;
    bool isSent() const;
    bool isReceived() const;

    bool setFields(CompanionPtr companion, const nlohmann::json &data);
    void setIsAntecedent(bool newIsAntecedent);
    void setIsSent(bool newIsSent);
    void setIsReceived(bool newIsReceived);

    void log();

private:
    bool isAntecedent_;
    bool isSent_;
    bool isReceived_;
};

class Message
{
public:
    // Message(
    //     // MessageType type, uint32_t id, uint8_t companion_id, uint8_t author_id,
    //     // const std::string &time, const std::string &text);
    //     MessageType type, const MessageMetaData &meta, const std::string &text);

    Message(MessageMetaDataPtr meta, MessageDataPtr data, MessageStatePtr state);
    Message(MessagePtr message);

    ~Message();

    MessageType type() const;
    uint32_t id() const;
    uint8_t companionId() const;
    std::string companionName() const;
    uint8_t authorId() const;
    std::string authorName() const;
    std::string time() const;
    std::string hashMD5() const;
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

class MessageInfo
{
public:
    MessageInfo(MessagePtr message, MessageWidgetPtr widget);
    ~MessageInfo() = default;

    MessagePtr message() const;
    MessageWidgetPtr widget() const;

    void setWidget(MessageWidgetPtr widget);

private:
    MessagePtr message_;
    MessageWidgetPtr widget_;
};

#endif // MESSAGE_HPP
