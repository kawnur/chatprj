#include "message.hpp"

#include "logging.hpp"
#include "message_data.hpp"
#include "message_meta_data.hpp"
#include "message_state.hpp"

Message::Message(
    MessageMetaDataPtr meta, MessageDataPtr data,
    MessageStatePtr state) : meta_(meta), data_(data), state_(state)
{
    log();
}

Message::Message(MessagePtr message)
{
    meta_ = message->meta();
    data_ = message->data();
    state_ = message->state();

    log();
}

Message::~Message() = default;

uint32_t Message::id() const
{
    return meta_->messageId();
}

uint8_t Message::companionId() const
{
    return meta_->companionId();
}

// std::string Message::companionName() const
// {
//     return meta_->companionName();
// }

uint8_t Message::authorId() const
{
    return meta_->authorId();
}

// std::string Message::authorName() const
// {
//     return meta_->authorName();
// }

std::string Message::time() const
{
    return meta_->timestampTz();
}

// std::string Message::hashMD5() const
// {
//     return meta_->hashMD5();
// }

std::string Message::text() const
{
    return data_->text();
}

bool Message::isMessageFromMe() const
{
    return meta_->isMessageFromMe();
}

bool Message::isSent() const
{
    return state_->isSent();
}

bool Message::isReceived() const
{
    return state_->isReceived();
}

bool Message::isAntecedent() const
{
    return state_->isAntecedent();
}

std::string Message::networkId() const
{
    return meta_->networkId();
}

NetworkMessageType Message::networkMessageType() const
{
    return meta_->networkMessageType();
}

void Message::setId(int id)
{
    meta_->setMessageId(id);
}

void Message::setCompanionId(int id)
{
    meta_->setCompanionId(id);
}

void Message::setTime(const std::string &time)
{
    meta_->setTimestampTz(time);
}

MessageMetaDataPtr Message::meta() const
{
    return meta_;
}

MessageDataPtr Message::data() const
{
    return data_;
}

MessageStatePtr Message::state() const
{
    return state_;
}

void Message::updateMeta(MessageMetaDataPtr meta)
{
    meta_->update(meta);
}

void Message::log()
{
    logArgsInfo(MESSAGE_LOG_DELIMITER);

    if (meta_)
        meta_->log();
    else
        logArgsInfo("meta is nullptr");

    if (data_)
        data_->log();
    else
        logArgsInfo("data is nullptr");

    if (state_)
        state_->log();
    else
        logArgsInfo("state is nullptr");

    logArgsInfo(MESSAGE_LOG_DELIMITER);
}

bool operator<(const Message &message1, const Message &message2)
{
    return message1.id() < message2.id();
}

TextMessage::TextMessage(MessageMetaDataPtr meta, MessageDataPtr data, MessageStatePtr state)
    : Message(meta, data, state) {}
