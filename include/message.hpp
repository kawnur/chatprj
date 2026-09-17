#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>

#include "utils.hpp"

class MessageWidget;

using LambdaMap = std::unordered_map<std::string, std::function<void()>>;

template<typename...Ts>
bool setFieldsFromJson(const LambdaMap &lambdaMap, const nlohmann::json &data, Ts &&...args)
{
    bool result = true;

    ([&]{ result &= runAndReturnBool(lambdaMap.at(args)); }, ...);

    return result;
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

    template<typename...Ts>
    bool setFields(const nlohmann::json &data, Ts &&...args)
    {
        const LambdaMap lambdaMap
        {
            { "type", [&]() { networkMessageType_ = data.at("type"); } },
            { "companion_id", [&]() { companionId_ = data.at("companion_id"); } },
            { "id", [&]() { networkId_ = data.at("text"); } }
        };

        return setFieldsFromJson(lambdaMap, data, args...);
    };

    bool isValid();

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
    ~MessageData() = default;

    template<typename...Ts>
    bool setFields(const nlohmann::json &data, Ts &&...args)
    {
        const LambdaMap lambdaMap
        {
            { "text", [&]() { text_ = data.at("text"); } },
            { "data", [&]() { data_ = data.at("data"); } }
        };

        return setFieldsFromJson(lambdaMap, data, args...);
    };

    std::string text_;
    std::string data_;  // for file transfer
};

class MessageState
{
public:

    //     MessageState(
    //         uint8_t companionId, bool isAntecedent, bool isSent, bool isReceived,
    //         std::string networkId);
    MessageState() = default;
    MessageState(bool isAntecedent, bool isSent, bool isReceived);
    ~MessageState() = default;

    //     bool isAntecedent() const;
        // bool isSent() const;
        // bool isReceived() const;
    //     std::string getNetworkId() const;

    //     void setIsAntecedent(bool);
    //     void setIsReceived(bool);
    //     void setNetworkId(const std::string &);

    template<typename...Ts>
    bool setFields(const nlohmann::json &data, Ts &&...args)
    {
        const LambdaMap lambdaMap
            {
                { "antecedent", [&]() { isAntecedent_ = data.at("antecedent"); } },
                { "received", [&]() { isReceived_ = data.at("received"); } }
            };

        return setFieldsFromJson(lambdaMap, data, args...);
    };

    // private:
    bool isAntecedent_;
    bool isSent_;
    bool isReceived_;
    // std::string networkId_;
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

    std::shared_ptr<MessageMetaData> meta() const;
    std::shared_ptr<MessageData> data() const;
    std::shared_ptr<MessageState> state() const;

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

class MessageInfo
{
public:
    MessageInfo(std::shared_ptr<Message> message, std::shared_ptr<MessageWidget> widget);
    ~MessageInfo() = default;

    std::shared_ptr<Message> getMessage() const;
    std::shared_ptr<MessageWidget> getWidget() const;

    void setWidget(std::shared_ptr<MessageWidget> widget);

private:
    std::shared_ptr<Message> message_;
    std::shared_ptr<MessageWidget> widget_;
};

#endif // MESSAGE_HPP
