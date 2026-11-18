#include "message_builder.hpp"

#include "companion.hpp"
#include "db_interaction.hpp"
#include "message.hpp"
#include "message_data.hpp"
#include "message_meta_data.hpp"
#include "message_state.hpp"

MessagePtrVariant MessageBuilder::buildMessage(CompanionPtr companion, const std::string &json)
{
    nlohmann::json data = buildJsonObject(json);

    if (!data.contains("type"))
        return nullptr;

    auto type = NetworkMessageType(data.at("type"));

    switch (type) {
    case NetworkMessageType::TEXT:
        return buildMessageObject<IncomingTextMessage>(companion, data);

    case NetworkMessageType::FILE_PROPOSAL:
        return nullptr;

    case NetworkMessageType::RECEIVE_CONFIRMATION:
        return buildMessageObject<ReceiveConfirmationMessage>(companion, data);

    case NetworkMessageType::RECEIVE_CONFIRMATION_REQUEST:
        return buildMessageObject<ReceiveConfirmationRequestMessage>(companion, data);

    case NetworkMessageType::CHAT_HISTORY_REQUEST:
        return nullptr;

    case NetworkMessageType::CHAT_HISTORY_DATA:
        return nullptr;

    case NetworkMessageType::FILE_REQUEST:
        return nullptr;

    case NetworkMessageType::FILE_DATA:
        return nullptr;

    case NetworkMessageType::FILE_DATA_CHECK_SUCCESS:
        return nullptr;

    case NetworkMessageType::FILE_DATA_CHECK_FAILURE:
        return nullptr;

    case NetworkMessageType::FILE_DATA_TRANSMISSON_END:
        return nullptr;

    case NetworkMessageType::FILE_DATA_TRANSMISSON_FAILURE:
        return nullptr;

    default:
        return nullptr;
    }
}

MessageMetaDataPtr MessageBuilder::buildMessageMetaData(
    CompanionPtr companion, DBReplyDataPtr data, std::size_t index)
{
    // network id is created by storage
    return std::make_shared<MessageMetaData>(
        getIdFromDbData(data, index, "id"),
        companion->id(),
        getIdFromDbData(data, index, "author_id"),
        data->getValue(index, "timestamp_tz"),
        "");
}

MessageMetaDataPtr MessageBuilder::buildUnsentMessageMetaData(
    CompanionPtr companion, DBReplyDataPtr data, std::size_t index)  // TODO optimize
{
    // network id is created by storage
    return std::make_shared<MessageMetaData>(
        getIdFromDbData(data, index, "id"),
        companion->id(),
        1,
        data->getValue(index, "timestamp_tz"),
        "");
}

MessageMetaDataPtr MessageBuilder::buildMyTextMessageMetaData(CompanionPtr companion)
{
    // network id is created by storage
    return std::make_shared<MessageMetaData>(
        0,
        companion->id(),
        1,
        "now()"s,
        "");
}

TextMessageDataPtr MessageBuilder::buildTextMessageData(DBReplyDataPtr data, std::size_t index)
{
    return std::make_shared<TextMessageData>(data->getValue(index, "message"));
}

MessageStatePtr MessageBuilder::buildMessageState(
    /*bool isAntecedent, */DBReplyDataPtr data, std::size_t index)
{
    return std::make_shared<MessageState>(
        // isAntecedent, getBoolFromDbData(data, index, "is_sent"),
        getBoolFromDbData(data, index, "is_sent"),
        getBoolFromDbData(data, index, "is_received"));
}

IncomingTextMessageStatePtr MessageBuilder::buildIncomingTextMessageState(
    /*bool isAntecedent, */DBReplyDataPtr data, std::size_t index)
{
    return std::make_shared<IncomingTextMessageState>(
        // isAntecedent, getBoolFromDbData(data, index, "is_sent"),
        getBoolFromDbData(data, index, "is_sent"),
        getBoolFromDbData(data, index, "is_received"),
        getBoolFromDbData(data, index, "is_read"));
}

TextMessagePtr MessageBuilder::buildTextMessageFromDbData(
    CompanionPtr companion, DBReplyDataPtr dbData, std::size_t index)
{
    auto meta = buildMessageMetaData(companion, dbData, index);
    auto data = buildTextMessageData(dbData, index);

    if (meta->isMessageFromMe()) {
        auto state = buildMessageState(dbData, index);

        return std::make_shared<OutgoingTextMessage>(meta, data, state);
    } else {
        auto state = buildIncomingTextMessageState(dbData, index);

        return std::make_shared<IncomingTextMessage>(meta, data, state);
    }
}

OutgoingTextMessagePtr MessageBuilder::buildMyTextMessage(CompanionPtr companion, const std::string &text)
{
    using Type = OutgoingTextMessage;

    auto meta = buildMyTextMessageMetaData(companion);
    auto data = std::make_shared<Type::data_type>(text);
    // auto state = std::make_shared<MessageState>(false, false, false);
    auto state = std::make_shared<Type::state_type>(false, false);

    auto message = std::make_shared<Type>(meta, data, state);

    return message;
}

TextMessagePtr MessageBuilder::buildUnsentTextMessageFromDbData(
    CompanionPtr companion, DBReplyDataPtr dbData, std::size_t index)
{
    using Type = OutgoingTextMessage;

    auto meta = buildUnsentMessageMetaData(companion, dbData, index);
    auto data = buildTextMessageData(dbData, index);

    auto state = buildMessageState(/*true, */dbData, index);
    state->setIsSent(false);  // check db data

    auto message = std::make_shared<Type>(meta, data, state);

    return message;
}

ReceiveConfirmationMessagePtr MessageBuilder::buildReceiveConfirmationMessage(MessagePtr message)
{
    auto meta = std::make_shared<MessageMetaData>();
    meta->setNetworkId(message->networkId());

    // auto state = std::make_shared<ReceiveConfirmationMessageState>(false, false, true);
    auto state = std::make_shared<ReceiveConfirmationMessageState>(false, true);

    auto newMessage = std::make_shared<ReceiveConfirmationMessage>(meta, nullptr, state);

    return newMessage;
}

ReceiveConfirmationRequestMessagePtr MessageBuilder::buildReceiveConfirmationRequestMessage(MessagePtr message)
{
    auto meta = std::make_shared<MessageMetaData>();
    meta->setNetworkId(message->networkId());

    // auto state = std::make_shared<MessageState>(false, false, false);
    auto state = std::make_shared<MessageState>(false, false);

    auto newMessage = std::make_shared<ReceiveConfirmationRequestMessage>(meta, nullptr, state);

    return newMessage;
}

MessagePtr MessageBuilder::buildFileDataCheckResultMessage(
    NetworkMessageType type, MessagePtr message)
{
    auto meta = std::make_shared<MessageMetaData>();
    meta->setNetworkId(message->networkId());

    auto state = std::make_shared<MessageState>();
    // state->setIsAntecedent(false);

    auto newMessage = std::make_shared<Message>(meta, nullptr, state);

    return newMessage;
}

MessagePtr MessageBuilder::buildFileDataTransmissionResultMessage(
    NetworkMessageType type, const std::string &networkId)
{
    auto meta = std::make_shared<MessageMetaData>();
    meta->setNetworkId(networkId);

    auto state = std::make_shared<MessageState>();
    // state->setIsAntecedent(false);

    auto message = std::make_shared<Message>(meta, nullptr, state);

    return message;
}

MessagePtr MessageBuilder::buildFileProposalReplyMessage(MessagePtr message)
{
    auto meta = std::make_shared<MessageMetaData>(message->meta());
    auto data = std::make_shared<MessageData>(message->data());
    auto state = std::make_shared<MessageState>();
    state->setIsSent(false);

    auto newMessage = std::make_shared<Message>(meta, data, state);

    return newMessage;
}

MessagePtr MessageBuilder::buildChatHistoryRequestMessage(CompanionPtr companion)
{
    auto meta = std::make_shared<MessageMetaData>();
    auto state = std::make_shared<MessageState>();
    // state->setIsAntecedent(true);

    auto message = std::make_shared<Message>(meta, nullptr, state);

    return message;
}

MessagePtr MessageBuilder::buildFileRequestMessage(const std::string &networkId)
{
    auto meta = std::make_shared<MessageMetaData>();
    meta->setNetworkId(networkId);

    // auto state = std::make_shared<MessageState>(false, false, false);
    auto state = std::make_shared<MessageState>(false, false);

    auto message = std::make_shared<Message>(meta, nullptr, state);

    return message;
}
