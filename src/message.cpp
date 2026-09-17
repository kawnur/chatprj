#include "message.hpp"

#include "logging.hpp"

MessageMetaData::MessageMetaData(
    MessageType messageType, NetworkMessageType networkMessageType, uint32_t messageId,
    uint8_t companionId, const std::string &companionName, uint8_t authorId,
    const std::string &authorName, const std::string &timestampTz, const std::string &hashMD5,
    const std::string networkId)
    : messageType_(messageType), networkMessageType_(networkMessageType), messageId_(messageId),
    companionId_(companionId), companionName_(companionName), authorId_(authorId),
    authorName_(authorName), timestampTz_(timestampTz), hashMD5_(hashMD5), networkId_(networkId) {}

bool MessageMetaData::isValid()
{
    if (companionId_ == 0 || timestampTz_ == "") {
        logArgsError("MessageMetaData is invalid, error adding message to db");

        return false;
    }

    return true;
}

MessageData::MessageData(const std::string &text, const std::string &data)
    : text_(text), data_(data) {}

Message::Message(
    MessageType type, /*uint32_t id, uint8_t companion_id, uint8_t author_id, const std::string &time,*/
     const MessageMetaData &meta, const std::string &text)
    // : type_(type), id_(id), companion_id_(companion_id), author_id_(author_id), time_(time),
    // text_(text) {}
{
    type_ = type;
    id_ = meta.messageId_;
    companion_id_ = meta.companionId_;
    author_id_ = meta.authorId_;
    time_ = meta.timestampTz_;
    text_ = text;
}

Message::Message(
    std::shared_ptr<MessageMetaData> meta, std::shared_ptr<MessageData> data,
    std::shared_ptr<MessageState> state) : meta_(meta), data_(data), state_(state) {}

Message::~Message() = default;

MessageType Message::getType() const
{
    return type_;
}

uint32_t Message::getId() const
{
    // return id_;
    return meta_->messageId_;
}

uint8_t Message::getCompanionId() const
{
    return companion_id_;
}

uint8_t Message::getAuthorId() const
{
    return author_id_;
}

std::string Message::getTime() const
{
    return time_;
}

std::string Message::getText() const
{
    return text_;
}

bool Message::isMessageFromMe() const
{
    return (author_id_ != companion_id_);
}

bool Message::isSent() const
{
    return state_->isSent_;
}

bool Message::isReceived() const
{
    return state_->isReceived_;
}

bool Message::isAntecedent() const
{
    return state_->isAntecedent_;
}

std::string Message::getNetworkId() const
{
    return meta_->networkId_;
}

std::shared_ptr<MessageMetaData> Message::meta() const
{
    return meta_;
}

std::shared_ptr<MessageData> Message::data() const
{
    return data_;
}

std::shared_ptr<MessageState> Message::state() const
{
    return state_;
}

// MessageState::MessageState(
//     uint8_t companionId, bool isAntecedent, bool isSent, bool isReceived, std::string networkId)
//     : isAntecedent_(isAntecedent), isSent_(isSent), isReceived_(isReceived), networkId_(networkId)
// {}

MessageState::MessageState(bool isAntecedent, bool isSent, bool isReceived)
    : isAntecedent_(isAntecedent), isSent_(isSent), isReceived_(isReceived) {}

// bool MessageState::isAntecedent() const
// {
//     return isAntecedent_;
// }

// bool MessageState::isSent() const
// {
//     return isSent_;
// }

// bool MessageState::isReceived() const
// {
//     return isReceived_;
// }

// std::string MessageState::getNetworkId() const
// {
//     return networkId_;
// }

// void MessageState::setIsAntecedent(bool value)
// {
//     isAntecedent_ = value;
// }

// void MessageState::setIsReceived(bool value)
// {
//     isReceived_ = value;
// }

// void MessageState::setNetworkId(const std::string &networkId)
// {
//     networkId_ = networkId;
// }

MessageInfo::MessageInfo(
    std::shared_ptr<Message> message, std::shared_ptr<MessageWidget> widget)
    : message_(message), widget_(widget) {}

std::shared_ptr<Message> MessageInfo::getMessage() const
{
    return message_;
}

std::shared_ptr<MessageWidget> MessageInfo::getWidget() const
{
    return widget_;
}

void MessageInfo::setWidget(std::shared_ptr<MessageWidget> widget)
{
    widget_ = widget;
}

bool operator<(const Message &message1, const Message &message2)
{
    return message1.getId() < message2.getId();
}
