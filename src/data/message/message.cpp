#include "message.hpp"

#include "logging.hpp"
#include "message_data.hpp"
#include "message_meta_data.hpp"

Message::Message(MetaPtr meta, DataPtr data, StatePtr state)
    : meta_(meta), data_(data), state_(state)
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

uint8_t Message::authorId() const
{
    return meta_->authorId();
}

std::string Message::time() const
{
    return meta_->timestampTz();
}

std::string Message::_data() const
{
    return data_->data();
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

std::string Message::getNetworkReadyData()
{
    return getJsonData().dump();
}

nlohmann::json Message::buildJsonData(NetworkMessageType type)
{
    nlohmann::json data;

    data["type"] = type;

    auto metaJsonData = meta_->getJsonData();
    data.update(metaJsonData);

    auto dataJsonData = data_->getJsonData();
    data.update(dataJsonData);

    auto stateJsonData = state_->getJsonData();
    data.update(stateJsonData);

    return data;
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

TextMessage::TextMessage(MetaPtr meta, DataPtr data, StatePtr state) : Message(meta, data, state) {}

nlohmann::json TextMessage::getJsonData()
{
    return buildJsonData(NetworkMessageType::TEXT);
}

ReceiveConfirmationMessage::ReceiveConfirmationMessage(MetaPtr meta, DataPtr data, StatePtr state)
    : Message(meta, data, state) {}

nlohmann::json ReceiveConfirmationMessage::getJsonData()
{
    return buildJsonData(NetworkMessageType::RECEIVE_CONFIRMATION);
}
