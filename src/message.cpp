#include "message.hpp"

#include "logging.hpp"

bool MessageMetaData::isValid()
{
    if (companionId_ == 0 || timestampTz_ == "") {
        logArgsError("MessageMetaData is invalid, error adding message to db");

        return false;
    }

    return true;
}

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

MessageType Message::getType() const
{
    return type_;
}

uint32_t Message::getId() const
{
    return id_;
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

MessageState::MessageState(
    uint8_t companionId, bool isAntecedent, bool isSent, bool isReceived, std::string networkId)
    : isAntecedent_(isAntecedent), isSent_(isSent), isReceived_(isReceived), networkId_(networkId)
{}

bool MessageState::isAntecedent() const
{
    return isAntecedent_;
}

bool MessageState::isSent() const
{
    return isSent_;
}

bool MessageState::isReceived() const
{
    return isReceived_;
}

std::string MessageState::getNetworkId() const
{
    return networkId_;
}

void MessageState::setIsAntecedent(bool value)
{
    isAntecedent_ = value;
}

void MessageState::setIsReceived(bool value)
{
    isReceived_ = value;
}

void MessageState::setNetworkId(const std::string &networkId)
{
    networkId_ = networkId;
}

MessageInfo::MessageInfo(
    std::shared_ptr<MessageState> state, std::shared_ptr<MessageWidget> widget)
    : state_(state), widget_(widget) {}

std::shared_ptr<MessageState> MessageInfo::getState() const
{
    return state_;
}

std::shared_ptr<MessageWidget> MessageInfo::getWidget() const
{
    return widget_;
}

void MessageInfo::setWidget(std::shared_ptr<MessageWidget> widget)
{
    widget_ = widget;
}
