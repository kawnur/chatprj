#include "message_builder.hpp"

#include "companion.hpp"
#include "db_interaction.hpp"
#include "message.hpp"

MessageMetaDataPtr MessageBuilder::buildMessageMetaData(
    CompanionPtr companion, DBReplyDataPtr data, std::size_t index)
{
    // network id is created by storage
    return std::make_shared<MessageMetaData>(
        MessageType::TEXT, NetworkMessageType::UNKNOWN, getIdFromDbData(data, index, "id"),
        companion->id(), companion->name(), getIdFromDbData(data, index, "author_id"), "",
        data->getValue(index, "timestamp_tz"), "", "");
}

MessageMetaDataPtr MessageBuilder::buildMyTextMessageMetaData(MessageType type, CompanionPtr companion)
{
    // network id is created by storage
    auto meta = std::make_shared<MessageMetaData>();
    meta->messageType_ = type;
    meta->networkMessageType_ = defineNetworkMessageType(type);
    meta->companionName_ = companion->name();
    meta->authorId_ = 1;
    meta->authorName_ = getString(ME_NAME);
    meta->timestampTz_ = "now()"s;

    return meta;
}

MessageDataPtr MessageBuilder::buildMessageData(DBReplyDataPtr data, std::size_t index)
{
    return std::make_shared<MessageData>(data->getValue(index, "message"), "");
}

MessageStatePtr MessageBuilder::buildMessageState(DBReplyDataPtr data, std::size_t index)
{
    return std::make_shared<MessageState>(
        false, getBoolFromDbData(data, index, "is_sent"),
        getBoolFromDbData(data, index, "is_received"));
}

MessagePtr MessageBuilder::buildMessageFromDbData(
    CompanionPtr companion, DBReplyDataPtr dbData, std::size_t index)
{
    auto meta = buildMessageMetaData(companion, dbData, index);
    auto data = buildMessageData(dbData, index);
    auto state = buildMessageState(dbData, index);

    auto message = std::make_shared<Message>(meta, data, state);

    return message;
}

MessagePtr MessageBuilder::buildMyTextMessage(
    MessageType type, CompanionPtr companion, const std::string &text)
{
    auto meta = buildMyTextMessageMetaData(type, companion);
    auto data = std::make_shared<MessageData>(text, "");
    auto state = std::make_shared<MessageState>(false, false, false);

    auto message = std::make_shared<Message>(meta, data, state);

    return message;
}
