#include "message_handler.hpp"

#include "companion.hpp"
#include "manager.hpp"
#include "message.hpp"
#include "widgets.hpp"

void MessageHandler::setManager(std::shared_ptr<Manager> manager)
{
    manager_ = manager;
}

void MessageHandler::sendMessage(
    MessageType type, CompanionPtr companion, ActionPtr action, const std::string &text)
{
    auto group = manager_->getMappedWidgetGroupByCompanion(companion);

    if (!group)
        return;

    // encrypt message

    // add to DB and get timestamp    
    auto meta = std::make_shared<MessageMetaData>();
    meta->companionName_ = companion->name();
    meta->authorName_ = getString(ME_NAME);
    meta->timestampTz_ = "now()"s;

    auto data = std::make_shared<MessageData>();
    data->text_ = text;

    auto state = std::make_shared<MessageState>();
    state->isSent_ = false;
    state->isReceived_ = false;

    auto pushMeta = manager_->pushMessageToDB(meta, data, state);

    if (!pushMeta || !pushMeta->isValid())
        return;

    pushMeta->messageType_ = type;
    pushMeta->networkMessageType_ = NetworkMessageType::TEXT;
    pushMeta->companionName_ = meta->companionName_;
    pushMeta->authorId_ = 1;
    pushMeta->authorName_ = meta->authorName_;

    state->isAntecedent_ = false;

    auto message = companion->createMessage(pushMeta, data, state);

    if (!message)
        return;

    // add to widget
    if (type == MessageType::FILE) {
        auto storage = companion->getFileOperatorStorage();

        if (!storage)
            return;

        // TODO modify
        auto cast = std::dynamic_pointer_cast<FileAction>(action);

        if (!cast)
            logArgsError("action cast error");
        else
            storage->addSenderOperator(message->getNetworkId(), cast->getPath());
    }

    group->addMessageWidgetToCentralPanelChatHistory(message);

    // define NetworkMessageType
    auto networkMessageType = defineNetworkMessageType(type);
    message->meta()->networkMessageType_ = networkMessageType;

    // send over network
    bool result = sendMessageByNetwork(companion, message, message->meta());

    // mark message as sent
    if (result)
        manager_->markMessageAsSent(companion, message);

    // wait for message reception confirmation
    waitForMessageReceptionConfirmation(companion, message);
}

bool MessageHandler::sendMessageByNetwork(
    CompanionPtr companion, std::shared_ptr<Message> message, std::shared_ptr<MessageMetaData> meta)
{
    if (meta->networkMessageType_ == NetworkMessageType::UNKNOWN)
        return false;

    if (meta->networkMessageType_ == NetworkMessageType::NO_ACTION)
        return true;

    // build json
    auto json = buildMessageJSONString(companion, message, meta);

    // send json over network
    return sendJsonByNetwork(companion, json);
}

bool MessageHandler::sendJsonByNetwork(CompanionPtr companion, const std::string &json)
{
    auto networkEntity = getManager()->getNetworkEntity(companion->id());

    if (!networkEntity)
        return false;

    auto result = networkEntity->sendJsonViaClient(json);

    return result;
}

bool MessageHandler::sendChatHistoryByNetwork(
    CompanionPtr companion, std::shared_ptr<DBReplyData> data, std::vector<std::string> &keys)
{
    // build json
    std::string json = buildChatHistoryJSONString(data, keys);

    // send json over network
    return sendJsonByNetwork(companion, json);
}

bool MessageHandler::sendFileBlockByNetwork(
    CompanionPtr companion, const std::string &networkId, const std::string &data)
{
    // build json
    auto json = buildFileBlockJSONString(companion, networkId, data);

    // send json over network
    return sendJsonByNetwork(companion, json);
}

void MessageHandler::sendFile(CompanionPtr companion, const std::filesystem::path &path)
{
    logArgs(__FUNCTION__);
}

void MessageHandler::receiveMessage(CompanionPtr companion, const std::string &json)
{
    nlohmann::json jsonData = buildJsonObject(json);

    // build meta
    auto meta = buildObjectFromJson<MessageMetaData>(companion, jsonData);

    if (!meta)
        return;

    // build data
    auto data = buildObjectFromJson<MessageData>(companion, jsonData);

    if (!data)
        return;

    // build state
    auto state = buildObjectFromJson<MessageState>(companion, jsonData);

    if (!state)
        return;

    switch (meta->networkMessageType_) {
    case NetworkMessageType::TEXT:
        receiveTextMessage(companion, meta, data, state);

    break;

    case NetworkMessageType::FILE_PROPOSAL:
        receiveFileProposalMessage(companion, meta, data, state);

    break;

    case NetworkMessageType::RECEIVE_CONFIRMATION:
        receiveConfirmation(companion, meta, data, state);

    break;

    case NetworkMessageType::RECEIVE_CONFIRMATION_REQUEST:
        receiveConfirmationRequest(companion, meta, data, state);

    break;

    case NetworkMessageType::CHAT_HISTORY_REQUEST:
        reciveChatHistoryRequest(companion);

    break;

    case NetworkMessageType::CHAT_HISTORY_DATA:
        receiveChatHistoryData(companion, jsonData);

    break;

    case NetworkMessageType::FILE_REQUEST:
        receiveFileRequest(companion, meta);

    break;

    case NetworkMessageType::FILE_DATA:
        receiveFileData(companion, meta, data);

    break;

    case NetworkMessageType::FILE_DATA_CHECK_SUCCESS:
    case NetworkMessageType::FILE_DATA_CHECK_FAILURE:
        receiveFileDataCheck(companion, meta);

    break;

    case NetworkMessageType::FILE_DATA_TRANSMISSON_END:
        receiveFileDataTransmissionEnd(companion, meta);

    break;

    case NetworkMessageType::FILE_DATA_TRANSMISSON_FAILURE:
        receiveFileDataTransmissionFailure(companion, meta);

    break;

    default:
        break;
    }
}

void MessageHandler::receiveTextMessage(
    CompanionPtr companion, MessageMetaDataPtr meta, MessageDataPtr data, MessageStatePtr state)
{
    auto name = companion->name();

    meta->companionName_ = name;
    meta->authorName_ = name;
    state->isSent_ = false;
    state->isReceived_ = true;

    // add to DB and get timestamp
    auto pushMeta = manager_->pushMessageToDB(meta, data, state);

    if (!pushMeta || !pushMeta->isValid())
        return;

    pushMeta->messageType_ = MessageType::TEXT;
    pushMeta->networkMessageType_ = NetworkMessageType::TEXT;
    pushMeta->companionName_ = meta->companionName_;
    pushMeta->authorId_ = meta->authorId_;
    pushMeta->authorName_ = meta->authorName_;

    // auto message = companion->createMessage(meta, data, state);
    auto message = companion->createMessage(pushMeta, data, state);

    if (!message)
        return;

    // decrypt message

    // add to widget
    manager_->addMessageWidget(companion, message);

    // send reply message to sender
    pushMeta->networkMessageType_ = NetworkMessageType::RECEIVE_CONFIRMATION;
    bool result = sendMessageByNetwork(companion, message, pushMeta);
}

void MessageHandler::receiveFileProposalMessage(
    CompanionPtr companion, MessageMetaDataPtr meta, MessageDataPtr data, MessageStatePtr state)
{
    // create receiver operator
    companion->addReceiverOperator(meta, HOME_PATH);

    // update message info
    auto name = companion->name();

    meta->companionName_ = name;
    meta->authorName_ = name;
    state->isSent_ = false;
    state->isReceived_ = true;

    // add to DB and get timestamp
    auto replyMeta = manager_->pushMessageToDB(meta, data, state);

    if (!replyMeta || !replyMeta->isValid())
        return;

    // create message
    replyMeta->messageType_ = MessageType::FILE;
    replyMeta->networkMessageType_ = NetworkMessageType::NO_ACTION;

    auto message = companion->createMessage(meta, data, state);

    if (!message)
        return;

    // decrypt message

    // add to widget
    manager_->addMessageWidget(companion, message);

    // send reply message to sender
    bool result = sendMessageByNetwork(companion, message, replyMeta);
}

void MessageHandler::receiveConfirmation(
    CompanionPtr companion, MessageMetaDataPtr meta, MessageDataPtr data, MessageStatePtr state)
{
    if (state->isReceived_) {  // successfully received
        // mark message as received
        auto info = companion->getMessageInfoByNetworkId(meta->networkId_);
        auto state = info->getMessage()->state();

        if (state) {
            // found message in mapping
            state->isReceived_ = true;
            manager_->markMessageAsReceived(info);
        }
        else {
            // strange situation
            logArgsError("received confirmation for message which is not in messageMapping_");
        }
    }
    else {
        // message was not received, resend message
    }
}

void MessageHandler::receiveConfirmationRequest(
    CompanionPtr companion, MessageMetaDataPtr meta, MessageDataPtr data, MessageStatePtr state)
{
    // search for message in companions's mapping
    auto info = companion->getMessageInfoByNetworkId(meta->networkId_);

    if (!info) {
        // probably old message from previous sessions, search for it in db
        logArgsInfo("probably old message from previous sessions, search for it in db");

        return;
    }

    auto message = info->getMessage();

    logArgs("message:", message.get(), "info:", info.get());

    auto messageState = message->state();

    if (!messageState)
        return;

    // message found in managers's mapping
    if (messageState->isReceived_) {
        auto replyMeta = std::make_shared<MessageMetaData>();
        replyMeta->networkMessageType_ = NetworkMessageType::RECEIVE_CONFIRMATION;
        messageState->isAntecedent_ = false;  // ???

        bool result = sendMessageByNetwork(companion, message, replyMeta);
    }
    else {
        // strange situation
        logArgsError(
            "received reception confirmation request "
            "for message in messageMapping_ with isReceived = false");
    }
}

void MessageHandler::reciveChatHistoryRequest(CompanionPtr companion)
{
    logTemplateInfo("got history request from {}", companion->name());

    auto group = manager_->getMappedWidgetGroupByCompanion(companion);

    if (group)
        emit group->askUserForHistorySendingConfirmationSignal();
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
    fillCompanionMessageMapping(companion, true);

    // build chat history
    auto group = manager_->getMappedWidgetGroupByCompanion(companion);

    if (group)
        emit group->buildChatHistorySignal();
}

void MessageHandler::receiveFileRequest(CompanionPtr companion, MessageMetaDataPtr meta)
{
    logArgs(__FUNCTION__);

    auto networkId = meta->networkId_;
    auto sender = companion->getFileOperatorByNetworkId<SenderOperator>(networkId);

    if (sender)
        sender->sendFile(companion, networkId);
    else
        logTemplateError("companion has no file operator for networkId = {}", networkId);
}

void MessageHandler::receiveFileData(CompanionPtr companion, MessageMetaDataPtr meta, MessageDataPtr data)
{
    logArgs(__FUNCTION__);

    auto networkId = meta->networkId_;
    auto receiver = companion->getFileOperatorByNetworkId<ReceiverOperator>(networkId);

    if (receiver)
        receiver->receiveFilePart(data->data_);
    else
        logTemplateError("companion has no file operator for networkId = {}", networkId);
}

void MessageHandler::receiveFileDataCheck(CompanionPtr companion, MessageMetaDataPtr meta)
{
    bool success = (meta->networkMessageType_ == NetworkMessageType::FILE_DATA_CHECK_SUCCESS);

    logArgs(__FUNCTION__, success);

    auto entryTemplate = (success)
        ? "file {} received by companion successfully"
        : "file {} WAS NOT received by companion";

    auto networkId = meta->networkId_;

    logTemplateInfo(entryTemplate, companion->getFilePathString(networkId));

    companion->removeFileOperator<SenderOperator>(networkId);
}

void MessageHandler::receiveFileDataTransmissionEnd(CompanionPtr companion, MessageMetaDataPtr meta)
{
    logArgs(__FUNCTION__);

    auto networkId = meta->networkId_;
    auto receiver = companion->getFileOperatorByNetworkId<ReceiverOperator>(networkId);

    if (!receiver) {
        logTemplateError("companion has no file operator for networkId '{}'", networkId);

        return;
    }

    auto resultType = (receiver->receiveFile())
        ? NetworkMessageType::FILE_DATA_CHECK_SUCCESS
        : NetworkMessageType::FILE_DATA_CHECK_FAILURE;

    if (resultType == NetworkMessageType::FILE_DATA_CHECK_SUCCESS) {
        logArgs("file received successfully");

        companion->removeFileOperator<ReceiverOperator>(networkId);
    }

    auto replyMeta = std::make_shared<MessageMetaData>();
    replyMeta->networkMessageType_ = resultType;
    replyMeta->networkId_ = networkId;

    auto state = std::make_shared<MessageState>();
    state->isAntecedent_ = false;

    auto message = std::make_shared<Message>(nullptr, nullptr, state);

    bool result = sendMessageByNetwork(companion, message, replyMeta);
}

void MessageHandler::receiveFileDataTransmissionFailure(
    CompanionPtr companion, MessageMetaDataPtr meta)
{
    logArgs(__FUNCTION__);

    auto networkId = meta->networkId_;

    logTemplateInfo(
        "file {} WAS NOT received by companion",
        companion->getFilePathString(networkId));

    companion->removeFileOperator<SenderOperator>(networkId);
}

// MessageDataPtr MessageHandler::buildMessageDataFromJson(const nlohmann::json &data)
// {
//     return buildObjectFromJson<MessageData>(data, "text");
// }

// MessageMetaDataPtr MessageHandler::buildMessageMetaDataFromJson(const nlohmann::json &data)
// {
//     return buildObjectFromJson<MessageMetaData>(data, "type", "id", "time");
// }

// MessageStatePtr MessageHandler::buildMessageStateFromJson(const nlohmann::json &data)
// {
//     return buildObjectFromJson<MessageState>(data, "antecedent");
// }

void MessageHandler::addEarlyMessages(CompanionPtr companion)
{
    if (companion)
        return;

    // get earliest message in current messages
    auto earliestMessage = companion->getEarliestMessage();

    auto messageId = earliestMessage->getId();
    auto companionId = companion->id();

    // get messages data
    auto messagesData = manager_->getDBData(
        DBRequestType::GET_EARLY_MESSAGES_BY_MESSAGE_ID, companionId, messageId);

    if (!messagesData)
        return;

    if (messagesData->isEmpty()) {
        logTemplateWarning(
            "no messages earlier than id = {0} in db with companion {1}", messageId, companionId);

        return;
    }

    auto widgetGroup = manager_->getMappedWidgetGroupByCompanion(companion);

    for (std::size_t i = 0; i < messagesData->size(); i++) {  // TODO switch to iterators
        logArgs("adding message with id", messagesData->getValue(i, "id"));

        auto info = companion->createMessageAndAddToMapping(messagesData, i);

        if (!info) {
            logArgsError("could not add message to messageMapping_");

            continue;
        }

        widgetGroup->addMessageWidgetToCentralPanelChatHistory(info->getMessage());
    }

    widgetGroup->sortChatHistoryElements();
}

void MessageHandler::sendUnsentMessages(CompanionPtr companion)
{
    // get unsent messages from db
    auto messagesData = manager_->getDBData(
        DBRequestType::GET_UNSENT_MESSAGES_BY_COMPANION_NAME, companion->name());

    if (!messagesData || messagesData->isEmpty())
        return;

    for (std::size_t i = 0; i < messagesData->size(); i++) {  // TODO switch to iterators
        uint32_t messageId = getIntFromString(ID_BAD_VALUE, messagesData->getValue(i, "id"));
        auto message = companion->findMessage(messageId);
        std::string networkId;

        if (message) {
            if (message->state()) {
                networkId = message->getNetworkId();
            }
            else {
                logArgsError(
                    "strange case: unsent message found in companions messages, "
                    "but not found in companion's messageMapping_");
            }
        }
        else {
            // add to companion's messages if needed
            networkId = companion->generateNetworkId(true);

            auto meta = std::make_shared<MessageMetaData>();
            meta->messageType_ = MessageType::TEXT;
            meta->messageId_ = messageId;
            meta->companionId_ = companion->id();
            meta->authorId_ = 1;
            meta->timestampTz_ = messagesData->getValue(i, "timestamp_tz");
            meta->networkId_ = networkId;

            auto data = std::make_shared<MessageData>();
            data->text_ = messagesData->getValue(i, "message");

            auto state = std::make_shared<MessageState>();
            state->isAntecedent_ = true;
            state->isSent_ = false;
            state->isReceived_ = getBoolFromDBValue(messagesData->getValue(i, "is_received"));

            message = companion->createMessage(meta, data, state);
        }

        // send over network
        bool result = sendMessageByNetwork(companion, message, message->meta());

        // mark message as sent
        if (result)
            manager_->markMessageAsSent(companion, message);
    }
}

void MessageHandler::fillCompanionMessageMapping(CompanionPtr companion, bool containersNotEmpty)
{
    uint8_t companionId = companion->id();

    // get messages data
    auto messagesData = manager_->getDBData(
        DBRequestType::GET_MESSAGES, companionId, NUMBER_OF_MESSAGES_TO_GET_FROM_DB);

    if (!messagesData || messagesData->isEmpty())
        return;

    for (std::size_t i = 0; i < messagesData->size(); i++) {  // TODO switch to iterators
        auto messageId = getIntFromString(ID_BAD_VALUE, messagesData->getValue(i, "id"));

        if (containersNotEmpty) {
            auto info = companion->getMessageInfoByMessageId(messageId);

            if (info && info->getMessage()->state()) {
                // companion->addMessage(const_cast<MessagePtr>(pair.second));
            } else {
                companion->createMessageAndAddToMapping(messagesData, i);
            }
        }
        else {
            companion->createMessageAndAddToMapping(messagesData, i);
        }
    }
}

void MessageHandler::requestHistoryFromCompanion(CompanionPtr companion)
{
    auto meta = std::make_shared<MessageMetaData>();
    meta->networkMessageType_ = NetworkMessageType::CHAT_HISTORY_REQUEST;

    auto state = std::make_shared<MessageState>();
    state->isAntecedent_ = true;

    auto message = companion->createMessage(meta, nullptr, state);

    bool result = sendMessageByNetwork(companion, message, message->meta());
}

void MessageHandler::waitForMessageReceptionConfirmation(CompanionPtr companion, MessagePtr message)
{
    auto lambda = [=]()
    {
        auto sleepDuration = SLEEP_DURATION_INITIAL_MS;

        sleepForMS(sleepDuration);

        while (true) {
            if (message->isReceived())
                return;

            // send message reception confirmation request
            auto meta = std::make_shared<MessageMetaData>();
            meta->networkMessageType_ = NetworkMessageType::RECEIVE_CONFIRMATION_REQUEST;
            meta->networkId_ = message->getNetworkId();

            bool result = sendMessageByNetwork(companion, message, meta);

            // sleep
            sleepForMS(sleepDuration);
            sleepDuration *= SLEEP_DURATION_INCREASE_RATE;
        }
    };

    runInDetachedThread(lambda);
}
