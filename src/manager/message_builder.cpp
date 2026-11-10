#include "message_builder.hpp"

#include "companion.hpp"
#include "db_interaction.hpp"
#include "message.hpp"
#include "message_data.hpp"
#include "message_meta_data.hpp"
#include "message_state.hpp"

MessagePtr MessageBuilder::buildMessage(CompanionPtr companion, const std::string &json)
{
    nlohmann::json jsonData = buildJsonObject(json);

    if (!jsonData.contains("type"))
        return nullptr;

    auto type = NetworkMessageType(jsonData.at("type"));

    switch (type) {
    case NetworkMessageType::TEXT:

        break;

    case NetworkMessageType::FILE_PROPOSAL:

        break;

    case NetworkMessageType::RECEIVE_CONFIRMATION:

        break;

    case NetworkMessageType::RECEIVE_CONFIRMATION_REQUEST:

        break;

    case NetworkMessageType::CHAT_HISTORY_REQUEST:

        break;

    case NetworkMessageType::CHAT_HISTORY_DATA:

        break;

    case NetworkMessageType::FILE_REQUEST:

        break;

    case NetworkMessageType::FILE_DATA:

        break;

    case NetworkMessageType::FILE_DATA_CHECK_SUCCESS:
    case NetworkMessageType::FILE_DATA_CHECK_FAILURE:

        break;

    case NetworkMessageType::FILE_DATA_TRANSMISSON_END:

        break;

    case NetworkMessageType::FILE_DATA_TRANSMISSON_FAILURE:

        break;

    default:
        break;
    }

    // build meta
    auto meta = buildObjectFromJson<MessageMetaData>(companion, jsonData);

    // build data
    auto data = buildObjectFromJson<MessageData>(companion, jsonData);

    // build state
    auto state = buildObjectFromJson<MessageState>(companion, jsonData);

    auto message = std::make_shared<Message>(meta, data, state);

    return message;
}

MessageMetaDataPtr MessageBuilder::buildMessageMetaData(
    CompanionPtr companion, DBReplyDataPtr data, std::size_t index)
{
    // network id is created by storage
    return std::make_shared<MessageMetaData>(
        NetworkMessageType::UNKNOWN,
        getIdFromDbData(data, index, "id"),
        companion->id(),
        // companion->name(),
        getIdFromDbData(data, index, "author_id"),
        // "",
        data->getValue(index, "timestamp_tz"),
        ""/*,
        ""*/);
}

MessageMetaDataPtr MessageBuilder::buildUnsentMessageMetaData(
    CompanionPtr companion, DBReplyDataPtr data, std::size_t index)  // TODO optimize
{
    // network id is created by storage
    return std::make_shared<MessageMetaData>(
        NetworkMessageType::TEXT,
        getIdFromDbData(data, index, "id"),
        companion->id(),
        // companion->name(),
        1,
        // "",
        data->getValue(index, "timestamp_tz"),
        ""/*,
        ""*/);
}

MessageMetaDataPtr MessageBuilder::buildMyTextMessageMetaData(CompanionPtr companion)
{
    // network id is created by storage

    return std::make_shared<MessageMetaData>(
        NetworkMessageType::TEXT,
        0,
        companion->id(),
        // companion->name(),
        1,
        // getString(ME_NAME),
        "now()"s,
        ""/*,
        ""*/);
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

MessagePtr MessageBuilder::buildMyTextMessage(CompanionPtr companion, const std::string &text)
{
    auto meta = buildMyTextMessageMetaData(companion);
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
    state->setIsSent(false);  // check db data

    auto message = std::make_shared<Message>(meta, data, state);

    return message;
}

MessagePtr MessageBuilder::buildReceiveConfirmationMessage(MessagePtr message)
{
    auto meta = std::make_shared<MessageMetaData>(message->meta());
    meta->setNetworkMessageType(NetworkMessageType::RECEIVE_CONFIRMATION);

    auto data = std::make_shared<MessageData>("", "");
    auto state = std::make_shared<MessageState>(false, false, false);

    auto newMessage = std::make_shared<Message>(meta, data, state);

    return newMessage;
}

MessagePtr MessageBuilder::buildReceiveConfirmationRequestMessage(MessagePtr message)
{
    auto meta = std::make_shared<MessageMetaData>();
    meta->setNetworkMessageType(NetworkMessageType::RECEIVE_CONFIRMATION_REQUEST);
    meta->setNetworkId(message->networkId());

    auto state = std::make_shared<MessageState>(false, false, false);

    auto newMessage = std::make_shared<Message>(meta, nullptr, state);

    return newMessage;
}

MessagePtr MessageBuilder::buildFileDataCheckResultMessage(
    NetworkMessageType type, MessagePtr message)
{
    auto meta = std::make_shared<MessageMetaData>();
    meta->setNetworkMessageType(type);
    meta->setNetworkId(message->networkId());

    auto state = std::make_shared<MessageState>();
    state->setIsAntecedent(false);

    auto newMessage = std::make_shared<Message>(meta, nullptr, state);

    return newMessage;
}

MessagePtr MessageBuilder::buildFileDataTransmissionResultMessage(
    NetworkMessageType type, const std::string &networkId)
{
    auto meta = std::make_shared<MessageMetaData>();
    meta->setNetworkMessageType(type);
    meta->setNetworkId(networkId);

    auto state = std::make_shared<MessageState>();
    state->setIsAntecedent(false);

    auto message = std::make_shared<Message>(meta, nullptr, state);

    return message;
}

MessagePtr MessageBuilder::buildFileProposalReplyMessage(MessagePtr message)
{
    auto meta = std::make_shared<MessageMetaData>(message->meta());
    meta->setNetworkMessageType(NetworkMessageType::NO_ACTION);

    auto data = std::make_shared<MessageData>(message->data());

    auto state = std::make_shared<MessageState>();
    state->setIsSent(false);

    auto newMessage = std::make_shared<Message>(meta, data, state);

    return newMessage;
}

MessagePtr MessageBuilder::buildChatHistoryRequestMessage(CompanionPtr companion)
{
    auto meta = std::make_shared<MessageMetaData>();
    meta->setNetworkMessageType(NetworkMessageType::CHAT_HISTORY_REQUEST);

    auto state = std::make_shared<MessageState>();
    state->setIsAntecedent(true);

    auto message = std::make_shared<Message>(meta, nullptr, state);

    return message;
}

MessagePtr MessageBuilder::buildFileRequestMessage(const std::string &networkId)
{
    auto meta = std::make_shared<MessageMetaData>();
    meta->setNetworkMessageType(NetworkMessageType::FILE_REQUEST);
    meta->setNetworkId(networkId);

    auto state = std::make_shared<MessageState>(false, false, false);

    auto message = std::make_shared<Message>(meta, nullptr, state);

    return message;
}
