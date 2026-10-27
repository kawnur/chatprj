#include "message_handler.hpp"

#include "companion.hpp"
#include "manager.hpp"
#include "message.hpp"

MessageHandler::MessageHandler() : manager_(nullptr), storage_(), builder_() {}

void MessageHandler::setManager(ManagerPtr manager)
{
    manager_ = manager;
}

MessageInfoPtr MessageHandler::getMessageInfo(CompanionPtr companion, MessagePtr message)
{
    return storage_.getMessageInfo(companion->id(), message->id());
}

void MessageHandler::buildMessageAndStore(
    CompanionPtr companion, DBReplyDataPtr data, int index, std::vector<MessagePtr> &messages)
{
    logTemplateInfo("adding message with id '{}'", data->getValue(index, "id"));

    // create message
    auto message = builder_.buildMessageFromDbData(companion, data, index);

    // add to storage
    bool storageResult = addToStorage(companion, message);

    if (!storageResult)
        logArgsError("adding message to storage error");

    messages.push_back(message);
}

void MessageHandler::addMessages(CompanionPtr companion, DBReplyDataPtr data)
{
    std::vector<MessagePtr> messages {};

    for (std::size_t i = 0; i < data->size(); ++i)
        buildMessageAndStore(companion, data, i, messages);

    // add widgets
    manager_->addMessageWidgets(companion, messages);
}

void MessageHandler::addMessagesByIndexes(
    CompanionPtr companion, DBReplyDataPtr data, const std::vector<int> &indexes)
{
    std::vector<MessagePtr> messages {};

    for (const auto &i : indexes)
        buildMessageAndStore(companion, data, i, messages);

    // add widgets
    manager_->addMessageWidgets(companion, messages);
}

bool MessageHandler::addToStorage(CompanionPtr companion, MessagePtr message)
{
    auto info = std::make_shared<MessageInfo>(message, nullptr);
    auto result = storage_.updateAndInsert(companion->id(), info);

    return result;
}

bool MessageHandler::buildAndSendConfirmation(CompanionPtr companion, MessagePtr message)
{
    auto replyMessage = builder_.buildReceiveConfirmationMessage(message);
    bool result = sendMessageByNetwork(companion, replyMessage);

    return result;
}

void MessageHandler::sendMessage(
    MessageType type, CompanionPtr companion, ActionPtr action, const std::string &text)
{
    // encrypt message

    auto message = builder_.buildMyTextMessage(type, companion, text);

    // add to DB and get timestamp
    auto pushResult = manager_->pushMessageToDB(message);

    if (!pushResult || !pushResult->meta()->isValid())
        return;

    // store message
    auto storeResult = addToStorage(companion, message);

    if (!storeResult)
        return;

    // add file sender
    if (type == MessageType::FILE)
        manager_->addSenderOperator(message->networkId(), action);

    // add message widget
    manager_->addMessageWidget(companion, message);

    // send over network
    bool result = sendMessageByNetwork(companion, message);

    // mark message as sent
    if (result)
        manager_->markMessageAsSent(companion, message);

    // wait for message reception confirmation
    waitForMessageReceptionConfirmation(companion, message);
}

bool MessageHandler::sendMessageByNetwork(CompanionPtr companion, MessagePtr message)
{
    auto type = message->networkMessageType();

    if (type == NetworkMessageType::UNKNOWN)
        return false;

    if (type == NetworkMessageType::NO_ACTION)
        return true;

    // build json
    auto json = buildMessageJSONString(companion, message);

    // send json over network
    return manager_->sendJsonByNetwork(companion, json);
}

bool MessageHandler::sendChatHistory(
    CompanionPtr companion, DBReplyDataPtr data, std::vector<std::string> &keys)
{
    // build json
    std::string json = buildChatHistoryJSONString(data, keys);

    // send json over network
    return manager_->sendJsonByNetwork(companion, json);
}

bool MessageHandler::sendFileBlock(
    CompanionPtr companion, const std::string &networkId, const std::string &data)
{
    // build json
    auto json = buildFileBlockJSONString(companion, networkId, data);

    // send json over network
    return manager_->sendJsonByNetwork(companion, json);
}

bool MessageHandler::sendFileRequest(CompanionPtr companion, const std::string &networkId)
{
    auto message = builder_.buildFileRequestMessage(networkId);

    bool result = sendMessageByNetwork(companion, message);

    return result;
}

bool MessageHandler::sendFileDataTransmissionResultMessage(
    CompanionPtr companion, NetworkMessageType type, const std::string &networkId)
{
    auto message = builder_.buildFileDataTransmissionResultMessage(type, networkId);

    bool result = sendMessageByNetwork(companion, message);

    return result;
}

// void MessageHandler::sendFile(CompanionPtr companion, const std::filesystem::path &path)
// {
//     logArgs(__FUNCTION__);
// }

void MessageHandler::receiveMessage(CompanionPtr companion, const std::string &json)
{
    nlohmann::json jsonData = buildJsonObject(json);

    // build meta
    auto meta = buildObjectFromJson<MessageMetaData>(companion, jsonData);
    auto name = companion->name();
    meta->setCompanionName(name);
    meta->setAuthorName(name);

    if (!meta)
        return;

    // build data
    auto data = buildObjectFromJson<MessageData>(companion, jsonData);

    if (!data)
        return;

    // build state
    auto state = buildObjectFromJson<MessageState>(companion, jsonData);
    state->setIsReceived(true);

    if (!state)
        return;

    auto message = std::make_shared<Message>(meta, data, state);

    switch (message->networkMessageType()) {
    case NetworkMessageType::TEXT:
        receiveTextMessage(companion, message);

    break;

    case NetworkMessageType::FILE_PROPOSAL:
        receiveFileProposal(companion, message);

    break;

    case NetworkMessageType::RECEIVE_CONFIRMATION:
        receiveConfirmation(companion, message);

    break;

    case NetworkMessageType::RECEIVE_CONFIRMATION_REQUEST:
        receiveConfirmationRequest(companion, message);

    break;

    case NetworkMessageType::CHAT_HISTORY_REQUEST:
        reciveChatHistoryRequest(companion);

    break;

    case NetworkMessageType::CHAT_HISTORY_DATA:
        receiveChatHistoryData(companion, jsonData);

    break;

    case NetworkMessageType::FILE_REQUEST:
        receiveFileRequest(companion, message);

    break;

    case NetworkMessageType::FILE_DATA:
        receiveFileData(companion, message);

    break;

    case NetworkMessageType::FILE_DATA_CHECK_SUCCESS:
    case NetworkMessageType::FILE_DATA_CHECK_FAILURE:
        receiveFileDataCheck(companion, message);

    break;

    case NetworkMessageType::FILE_DATA_TRANSMISSON_END:
        receiveFileDataTransmissionEnd(companion, message);

    break;

    case NetworkMessageType::FILE_DATA_TRANSMISSON_FAILURE:
        receiveFileDataTransmissionFailure(companion, message);

    break;

    default:
        break;
    }
}

void MessageHandler::receiveTextMessage(CompanionPtr companion, MessagePtr message)
{
    // add to DB and get timestamp
    auto pushResult = manager_->pushMessageToDB(message);

    if (!pushResult || !pushResult->meta()->isValid())
        return;

    auto storeResult = addToStorage(companion, message);

    if (!storeResult)
        return;

    // decrypt message

    // add to widget
    manager_->addMessageWidget(companion, message);

    // send reply message to sender
    bool result = buildAndSendConfirmation(companion, message);
}

void MessageHandler::receiveFileProposal(CompanionPtr companion, MessagePtr message)
{
    // // create receiver operator
    // manager_->addReceiverOperator(meta, HOME_PATH);

    // // update message info
    // auto name = companion->name();

    // meta->companionName_ = name;
    // meta->authorName_ = name;
    // state->isSent_ = false;
    // state->isReceived_ = true;

    // // add to DB and get timestamp
    // auto replyMeta = manager_->pushMessageToDB(meta, data, state);

    // if (!replyMeta || !replyMeta->isValid())
    //     return;

    // // create message
    // replyMeta->messageType_ = MessageType::FILE;
    // replyMeta->networkMessageType_ = NetworkMessageType::NO_ACTION;

    // auto message = createMessage(companion, meta, data, state);

    // if (!message)
    //     return;

    // // decrypt message

    // // add to widget
    // manager_->addMessageWidget(companion, message);

    // // send reply message to sender
    // bool result = sendMessageByNetwork(companion, message, replyMeta);

    // update message info
    // auto name = companion->name();

    // meta->companionName_ = name;
    // meta->authorName_ = name;
    // state->isSent_ = false;
    // state->isReceived_ = true;

    // create message
    // meta->messageType_ = MessageType::FILE;
    // meta->networkMessageType_ = NetworkMessageType::NO_ACTION;

    // auto message = createMessage(companion, meta, data, state);

    // if (!message)
    //     return;

    auto storeResult = addToStorage(companion, message);

    if (!storeResult)
        return;

    auto reply = builder_.buildFileProposalReplyMessage(message);
    manager_->receiveFile(companion, reply);
}

void MessageHandler::receiveConfirmation(CompanionPtr companion, MessagePtr message)
{
    if (!message->isReceived())
        return;  // TODO message was not received, resend message

    // successfully received
    // mark message as received
    auto info = storage_.getMessageInfoByNetworkId(companion->id(), message->networkId());

    if (!info)
        return;

    auto state = info->message()->state();

    if (!state)
        return;

    // message found
    state->setIsReceived(true);
    manager_->markMessageAsReceived(info);
}

void MessageHandler::receiveConfirmationRequest(CompanionPtr companion, MessagePtr message)
{
    // search for message in storage
    auto info = storage_.getMessageInfoByNetworkId(companion->id(), message->networkId());

    if (!info)
        return;

    // verifiable message
    auto verifiable = info->message();
    auto state = verifiable->state();

    if (!state || !state->isReceived())
        return;

    // send confirmation
    bool result = buildAndSendConfirmation(companion, message);
}

void MessageHandler::reciveChatHistoryRequest(CompanionPtr companion)
{
    logTemplateInfo("got history request from {}", companion->name());
}

void MessageHandler::receiveChatHistoryData(CompanionPtr companion, const nlohmann::json &data)
{
    logTemplateInfo("got chat history from {}", companion->name());

    // push data to db
    if (!manager_->pushMessageHistoryToDb(companion, data))
        return;

    // clear chat history widget
    manager_->clearChatHistory(companion);

    // fill container with messages
    fillMessageStorageForCompanion(companion, true);
}

void MessageHandler::receiveFileRequest(CompanionPtr companion, MessagePtr message)
{
    logArgsInfo(__FUNCTION__);

    manager_->sendFile(companion, message);
}

void MessageHandler::receiveFileData(CompanionPtr companion, MessagePtr message)
{
    manager_->receiveFileData(companion, message);
}

void MessageHandler::receiveFileDataCheck(CompanionPtr companion, MessagePtr message)
{
    bool success = (message->networkMessageType() == NetworkMessageType::FILE_DATA_CHECK_SUCCESS);

    logArgsInfo(__FUNCTION__, success);

    auto entryTemplate = (success)
        ? "file '{}' received by companion successfully"
        : "file '{}' WAS NOT received by companion";

    auto networkId = message->networkId();

    logTemplateInfo(entryTemplate, manager_->getFilePathString(networkId));

    manager_->removeSenderOperator(networkId);
}

void MessageHandler::receiveFileDataTransmissionEnd(CompanionPtr companion, MessagePtr message)
{
    logArgsInfo(__FUNCTION__);

    auto receiveResult = manager_->stopFileDataReceiving(message);

    auto resultType = (receiveResult)
        ? NetworkMessageType::FILE_DATA_CHECK_SUCCESS
        : NetworkMessageType::FILE_DATA_CHECK_FAILURE;

    auto reply = builder_.buildFileDataCheckResultMessage(resultType, message);
    bool result = sendMessageByNetwork(companion, reply);
}

void MessageHandler::receiveFileDataTransmissionFailure(CompanionPtr companion, MessagePtr message)
{
    logArgsInfo(__FUNCTION__);

    manager_->stopFileDataSending(message);
}

void MessageHandler::addEarlyMessages(CompanionPtr companion)
{
    if (!companion)
        return;

    // get earliest message in current messages
    auto companionId = companion->id();
    auto earliestMessage = storage_.getEarliestMessage(companionId);
    auto messageId = earliestMessage->id();

    // get messages data
    auto data = manager_->getEarlyMessagesDBData(companionId, messageId);

    if (!data)
        return;

    // add messages
    addMessages(companion, data);
}

void MessageHandler::sendUnsentMessages(CompanionPtr companion)
{
    // get unsent messages from db
    auto data = manager_->getDBData(
        DBRequestType::GET_UNSENT_MESSAGES_BY_COMPANION_NAME, companion->name());

    if (!data || data->isEmpty())
        return;

    for (std::size_t i = 0; i < data->size(); ++i) {  // TODO use iterators
        uint32_t messageId = getIdFromDbData(data, i, "id");
        auto info = storage_.getMessageInfo(companion->id(), messageId);

        if (!info)
            continue;

        auto message = info->message();

        std::string networkId;

        if (message && message->state())
            networkId = message->networkId();
        else
            // add to companion's messages if needed
            message = builder_.buildUnsentTextMessageFromDbData(companion, data, i);

        // send over network
        bool result = sendMessageByNetwork(companion, message);

        // mark message as sent
        if (result)
            manager_->markMessageAsSent(companion, message);
    }
}

void MessageHandler::fillMessageStorageForCompanion(CompanionPtr companion, bool containersAreEmpty)
{
    uint8_t companionId = companion->id();

    // get messages data
    auto data = manager_->getDBData(
        DBRequestType::GET_MESSAGES, companionId, NUMBER_OF_MESSAGES_TO_GET_FROM_DB);

    if (!data || data->isEmpty())
        return;

    if (containersAreEmpty) {
        addMessages(companion, data);

        return;
    }

    // if containers are not empty check every message for existance
    std::vector<int> indexes {};

    for (std::size_t i = 0; i < data->size(); ++i) {
        auto messageId = getIdFromDbData(data, i, "id");
        auto info = storage_.getMessageInfo(companion->id(), messageId);
        bool messageExists = info && info->message()->state();

        if (!messageExists)
            indexes.push_back(i);
    }

    addMessagesByIndexes(companion, data, indexes);
}

void MessageHandler::requestHistoryFromCompanion(CompanionPtr companion)
{
    auto message = builder_.buildChatHistoryRequestMessage(companion);
    bool result = sendMessageByNetwork(companion, message);
}

void MessageHandler::waitForMessageReceptionConfirmation(CompanionPtr companion, MessagePtr message)
{
    auto lambda = [=, this]()  // ???
    {
        auto sleep = SLEEP_DURATION_INITIAL_MS;

        sleepForMS(sleep);

        while (true) {
            if (message->isReceived())
                return;

            // send message reception confirmation request
            auto request = builder_.buildReceiveConfirmationRequestMessage(message);  // TODO chek state
            bool result = sendMessageByNetwork(companion, message);

            // sleep
            sleepForMS(sleep);
            sleep *= SLEEP_DURATION_INCREASE_RATE;
        }
    };

    runInDetachedThread(lambda);
}

bool MessageHandler::setMappedMessageWidget(
    CompanionPtr companion, MessagePtr message, MessageWidgetPtr widget)
{
    return storage_.setMessageWidget(companion->id(), message->id(), widget);
}

bool MessageHandler::clearMessageMapping(CompanionPtr companion)
{
    return storage_.clearMessageMapping(companion->id());
}

bool MessageHandler::addCompanionToMessageStorage(CompanionPtr companion)
{
    return storage_.addCompanion(companion->id());
}

std::string buildMessageJSONString(CompanionPtr companion, MessagePtr message)
{
    using json = nlohmann::json;

    json jsonData;

    auto networkId = message->networkId();
    auto type = message->networkMessageType();

    jsonData["type"] = type;
    jsonData["id"] = networkId;
    // jsonData["companion_id"] = companion->getId();
    jsonData["antecedent"] = message->isAntecedent();
    jsonData["time"] = message->time();
    jsonData["text"] = message->text();

    switch (type) {
    case NetworkMessageType::TEXT: {
        // jsonData["time"] = message->getTime();
        // jsonData["text"] = message->getText();
    }

    break;

    case NetworkMessageType::FILE_PROPOSAL: {
        // jsonData["time"] = message->getTime();
        // jsonData["text"] = message->getText();
        jsonData["hashMD5"] = getManager()->getFileOperatorMD5Hash(networkId);
    }

    break;

    case NetworkMessageType::RECEIVE_CONFIRMATION:
        // jsonData["received"] = 1;
        jsonData["received"] = message->isSent();

        break;

    default:
        break;
    }

    return jsonData.dump();
}

std::string buildFileBlockJSONString(
    CompanionPtr companion, const std::string &networkId, const std::string &data)
{
    using json = nlohmann::json;

    json jsonData;

    jsonData["type"] = NetworkMessageType::FILE_DATA;
    jsonData["id"] = networkId;
    jsonData["companion_id"] = companion->id();
    jsonData["data"] = data;

    return jsonData.dump();
}
