#include "message_builder.hpp"

#include "companion.hpp"
#include "db_interaction.hpp"
#include "message.hpp"

MessageMetaDataPtr MessageBuilder::buildMessageMetaData(
    CompanionPtr companion, DBReplyDataPtr data, std::size_t index)
{
    // network id is created by storage
    return std::make_shared<MessageMetaData>(
        MessageType::TEXT,
        NetworkMessageType::UNKNOWN,
        getIdFromDbData(data, index, "id"),
        companion->id(),
        companion->name(),
        getIdFromDbData(data, index, "author_id"),
        "",
        data->getValue(index, "timestamp_tz"),
        "",
        "");
}

MessageMetaDataPtr MessageBuilder::buildUnsentMessageMetaData(
    CompanionPtr companion, DBReplyDataPtr data, std::size_t index)  // TODO optimize
{
    // network id is created by storage
    return std::make_shared<MessageMetaData>(
        MessageType::TEXT,
        defineNetworkMessageType(MessageType::TEXT),
        getIdFromDbData(data, index, "id"),
        companion->id(),
        companion->name(),
        1,
        "",
        data->getValue(index, "timestamp_tz"),
        "",
        "");
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

MessageStatePtr MessageBuilder::buildMessageState(
    bool isAntecedent, DBReplyDataPtr data, std::size_t index)
{
    return std::make_shared<MessageState>(
        isAntecedent, getBoolFromDbData(data, index, "is_sent"),
        getBoolFromDbData(data, index, "is_received"));
}

MessagePtr MessageBuilder::buildMessageFromDbData(
    CompanionPtr companion, DBReplyDataPtr dbData, std::size_t index)
{
    auto meta = buildMessageMetaData(companion, dbData, index);
    auto data = buildMessageData(dbData, index);
    auto state = buildMessageState(false, dbData, index);

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

MessagePtr MessageBuilder::buildUnsentTextMessageFromDbData(
    CompanionPtr companion, DBReplyDataPtr dbData, std::size_t index)
{
    auto meta = buildUnsentMessageMetaData(companion, dbData, index);
    auto data = buildMessageData(dbData, index);

    auto state = buildMessageState(true, dbData, index);
    state->isSent_ = false;  // check db data

    auto message = std::make_shared<Message>(meta, data, state);

    return message;
}

MessagePtr MessageBuilder::buildReceiveConfirmationMessage(MessagePtr message)
{
    auto meta = std::make_shared<MessageMetaData>(message->meta());
    meta->networkMessageType_ = NetworkMessageType::RECEIVE_CONFIRMATION;

    auto data = std::make_shared<MessageData>("", "");
    auto state = std::make_shared<MessageState>(false, false, false);

    auto newMessage = std::make_shared<Message>(meta, data, state);

    return newMessage;
}

MessagePtr MessageBuilder::buildReceiveConfirmationRequestMessage(MessagePtr message)
{
    auto meta = std::make_shared<MessageMetaData>();
    meta->networkMessageType_ = NetworkMessageType::RECEIVE_CONFIRMATION_REQUEST;
    meta->networkId_ = message->networkId();

    auto state = std::make_shared<MessageState>(false, false, false);

    auto newMessage = std::make_shared<Message>(meta, nullptr, state);

    return newMessage;
}

MessagePtr MessageBuilder::buildFileDataCheckResultMessage(
    NetworkMessageType type, MessagePtr message)
{
    auto meta = std::make_shared<MessageMetaData>();
    meta->networkMessageType_ = type;
    meta->networkId_ = message->networkId();

    auto state = std::make_shared<MessageState>();
    state->isAntecedent_ = false;

    auto newMessage = std::make_shared<Message>(meta, nullptr, state);

    return newMessage;
}

MessagePtr MessageBuilder::buildFileDataTransmissionResultMessage(
    NetworkMessageType type, const std::string &networkId)
{
    auto meta = std::make_shared<MessageMetaData>();
    meta->networkMessageType_ = type;
    meta->networkId_ = networkId;

    auto state = std::make_shared<MessageState>();
    state->isAntecedent_ = false;

    auto message = std::make_shared<Message>(meta, nullptr, state);

    return message;
}

MessagePtr MessageBuilder::buildFileProposalReplyMessage(MessagePtr message)
{
    auto meta = std::make_shared<MessageMetaData>(message->meta());
    meta->messageType_ = MessageType::FILE;
    meta->networkMessageType_ = NetworkMessageType::NO_ACTION;

    auto state = std::make_shared<MessageState>();
    state->isSent_ = false;

    auto newMessage = std::make_shared<Message>(meta, nullptr, state);

    return newMessage;
}

MessagePtr MessageBuilder::buildChatHistoryRequestMessage(CompanionPtr companion)
{
    auto meta = std::make_shared<MessageMetaData>();
    meta->networkMessageType_ = NetworkMessageType::CHAT_HISTORY_REQUEST;

    auto state = std::make_shared<MessageState>();
    state->isAntecedent_ = true;

    auto message = std::make_shared<Message>(meta, nullptr, state);

    return message;
}

MessagePtr MessageBuilder::buildFileRequestMessage(const std::string &networkId)
{
    auto meta = std::make_shared<MessageMetaData>();
    meta->networkMessageType_ = NetworkMessageType::FILE_REQUEST;
    meta->networkId_ = networkId;

    auto state = std::make_shared<MessageState>(false, false, false);

    auto message = std::make_shared<Message>(meta, nullptr, state);

    return message;
}
