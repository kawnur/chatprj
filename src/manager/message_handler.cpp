#include "message_handler.hpp"

#include "companion.hpp"
#include "manager.hpp"
#include "message.hpp"
#include "widgets.hpp"

MessageHandler::MessageHandler() : messageStorage_() {}

void MessageHandler::setManager(std::shared_ptr<Manager> manager)
{
    manager_ = manager;
}

MessageInfoPtr MessageHandler::getMessageInfo(CompanionPtr companion, MessagePtr message)
{
    return messageStorage_.getMessageInfo(companion->id(), message->id());
}

MessageMetaDataPtr MessageHandler::buildMessageMetaData(
    CompanionPtr companion, std::shared_ptr<DBReplyData> messagesData, std::size_t index)
{
    // network id is created by storage
    return std::make_shared<MessageMetaData>(
        MessageType::TEXT, NetworkMessageType::UNKNOWN,
        getIntFromString(ID_BAD_VALUE, messagesData->getValue(index, "id")), companion->id(),
        companion->name(),
        getIntFromString(ID_BAD_VALUE, messagesData->getValue(index, "author_id")), "",
        messagesData->getValue(index, "timestamp_tz"), "", "");
}

MessageDataPtr MessageHandler::buildMessageData(
    std::shared_ptr<DBReplyData> messagesData, std::size_t index)
{
    return std::make_shared<MessageData>(messagesData->getValue(index, "message"), "");
}

MessageStatePtr MessageHandler::buildMessageState(
    std::shared_ptr<DBReplyData> messagesData, std::size_t index)
{
    return std::make_shared<MessageState>(
        false,
        getBoolFromDBValue(messagesData->getValue(index, "is_sent")),
        getBoolFromDBValue(messagesData->getValue(index, "is_received")));
}

std::shared_ptr<Message> MessageHandler::createMessageAndAddToMapping(
    CompanionPtr companion, std::shared_ptr<DBReplyData> messagesData, std::size_t index)
{
    auto meta = buildMessageMetaData(companion, messagesData, index);
    auto data = buildMessageData(messagesData, index);
    auto state = buildMessageState(messagesData, index);

    return createMessage(companion, meta, data, state);
}

std::shared_ptr<Message> MessageHandler::createMessage(
    // MessageType type, /*uint32_t messageId, uint8_t authorId, const std::string &messageTime,*/
    // MessageMetaDataPtr meta, const std::string &messageText, bool isAntecedent,
    // bool isSent, bool isReceived, std::string networkId)
    CompanionPtr companion, MessageMetaDataPtr meta,
    MessageDataPtr data, MessageStatePtr state)
{
    // auto messageState = std::make_shared<MessageState>(
    //     companionId, isAntecedent, isSent, isReceived, networkId);

    auto message = std::make_shared<Message>(meta, data, state);
    auto info = std::make_shared<MessageInfo>(message, nullptr);

    // auto result = messageMapping_.emplace(meta->messageId_, info);
    auto result = messageStorage_.updateAndInsert(companion->id(), info);

    // return result;
    return (result) ? message : nullptr;
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

    // auto message = companion->createMessage(pushMeta, data, state);
    auto message = createMessage(companion, pushMeta, data, state);

    if (!message)
        return;

    // add to widget
    if (type == MessageType::FILE) {
        // TODO modify
        auto cast = std::dynamic_pointer_cast<FileAction>(action);

        if (!cast)
            logArgsError("file action cast error");
        else
            manager_->addSenderOperator(message->networkId(), cast->getPath());
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
    CompanionPtr companion, std::shared_ptr<Message> message, MessageMetaDataPtr meta)
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
    auto networkEntity = manager_->getNetworkEntity(companion->id());

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

    auto message = createMessage(companion, pushMeta, data, state);

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
    manager_->addReceiverOperator(meta, HOME_PATH);

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

    auto message = createMessage(companion, meta, data, state);

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
        // auto info = companion->getMessageInfoByNetworkId(meta->networkId_);
        auto info = messageStorage_.getMessageInfoByNetworkId(companion->id(), meta->networkId_);
        auto state = info->message()->state();

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
    // auto info = companion->getMessageInfoByNetworkId(meta->networkId_);
    auto info = messageStorage_.getMessageInfoByNetworkId(companion->id(), meta->networkId_);

    if (!info) {
        // probably old message from previous sessions, search for it in db
        logArgsInfo("probably old message from previous sessions, search for it in db");

        return;
    }

    auto message = info->message();

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
    auto sender = manager_->getFileOperator<SenderOperator>(networkId);

    if (sender)
        sender->sendFile(companion, networkId);
    else
        logTemplateError("companion has no file operator for networkId = {}", networkId);
}

void MessageHandler::receiveFileData(CompanionPtr companion, MessageMetaDataPtr meta, MessageDataPtr data)
{
    logArgs(__FUNCTION__);

    auto networkId = meta->networkId_;
    auto receiver = manager_->getFileOperator<ReceiverOperator>(networkId);

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

    logTemplateInfo(entryTemplate, manager_->getFilePathString(networkId));

    manager_->removeFileOperator<SenderOperator>(networkId);
}

void MessageHandler::receiveFileDataTransmissionEnd(CompanionPtr companion, MessageMetaDataPtr meta)
{
    logArgs(__FUNCTION__);

    auto networkId = meta->networkId_;
    auto receiver = manager_->getFileOperator<ReceiverOperator>(networkId);

    if (!receiver) {
        logTemplateError("companion has no file operator for networkId '{}'", networkId);

        return;
    }

    auto resultType = (receiver->receiveFile())
        ? NetworkMessageType::FILE_DATA_CHECK_SUCCESS
        : NetworkMessageType::FILE_DATA_CHECK_FAILURE;

    if (resultType == NetworkMessageType::FILE_DATA_CHECK_SUCCESS) {
        logArgs("file received successfully");

        manager_->removeFileOperator<ReceiverOperator>(networkId);
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

    logTemplateInfo("companion did not receive file '{}'", manager_->getFilePathString(networkId));

    manager_->removeFileOperator<SenderOperator>(networkId);
}

void MessageHandler::addEarlyMessages(CompanionPtr companion)
{
    if (companion)
        return;

    // get earliest message in current messages
    // auto earliestMessage = companion->getEarliestMessage();
    auto companionId = companion->id();
    auto earliestMessage = messageStorage_.getEarliestMessage(companionId);
    auto messageId = earliestMessage->id();

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

        // auto info = companion->createMessageAndAddToMapping(messagesData, i);
        auto message = createMessageAndAddToMapping(companion, messagesData, i);

        if (!message) {
            logArgsError("could not add message to messageMapping_");

            continue;
        }

        widgetGroup->addMessageWidgetToCentralPanelChatHistory(message);
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

        // auto message = companion->findMessage(messageId);
        auto message = messageStorage_.getMessage(companion->id(), messageId);

        std::string networkId;

        if (message) {
            if (message->state()) {
                networkId = message->networkId();
            }
            else {
                logArgsError(
                    "strange case: unsent message found in companions messages, "
                    "but not found in companion's messageMapping_");
            }
        }
        else {
            // add to companion's messages if needed
            // networkId is created by storage
            // networkId = companion->generateNetworkId(true);

            auto meta = std::make_shared<MessageMetaData>();
            meta->messageType_ = MessageType::TEXT;
            meta->messageId_ = messageId;
            meta->companionId_ = companion->id();
            meta->authorId_ = 1;
            meta->timestampTz_ = messagesData->getValue(i, "timestamp_tz");
            // meta->networkId_ = networkId;

            auto data = std::make_shared<MessageData>();
            data->text_ = messagesData->getValue(i, "message");

            auto state = std::make_shared<MessageState>();
            state->isAntecedent_ = true;
            state->isSent_ = false;
            state->isReceived_ = getBoolFromDBValue(messagesData->getValue(i, "is_received"));

            message = createMessage(companion, meta, data, state);
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
            // auto info = companion->getMessageInfoByMessageId(messageId);
            auto info = messageStorage_.getMessageInfo(companion->id(), messageId);

            if (info && info->message()->state()) {
                // companion->addMessage(const_cast<MessagePtr>(pair.second));
            } else {
                // companion->createMessageAndAddToMapping(messagesData, i);
                createMessageAndAddToMapping(companion, messagesData, i);
            }
        }
        else {
            // companion->createMessageAndAddToMapping(messagesData, i);
            createMessageAndAddToMapping(companion, messagesData, i);
        }
    }
}

void MessageHandler::requestHistoryFromCompanion(CompanionPtr companion)
{
    auto meta = std::make_shared<MessageMetaData>();
    meta->networkMessageType_ = NetworkMessageType::CHAT_HISTORY_REQUEST;

    auto state = std::make_shared<MessageState>();
    state->isAntecedent_ = true;

    auto message = createMessage(companion, meta, nullptr, state);

    bool result = sendMessageByNetwork(companion, message, message->meta());
}

void MessageHandler::waitForMessageReceptionConfirmation(CompanionPtr companion, MessagePtr message)
{
    auto lambda = [=, this]()
    {
        auto sleepDuration = SLEEP_DURATION_INITIAL_MS;

        sleepForMS(sleepDuration);

        while (true) {
            if (message->isReceived())
                return;

            // send message reception confirmation request
            auto meta = std::make_shared<MessageMetaData>();
            meta->networkMessageType_ = NetworkMessageType::RECEIVE_CONFIRMATION_REQUEST;
            meta->networkId_ = message->networkId();

            bool result = sendMessageByNetwork(companion, message, meta);

            // sleep
            sleepForMS(sleepDuration);
            sleepDuration *= SLEEP_DURATION_INCREASE_RATE;
        }
    };

    runInDetachedThread(lambda);
}

std::string buildMessageJSONString(
    // bool isAntecedent, NetworkMessageType type, std::shared_ptr<Companion> companion,
    // const std::string &networkId, std::shared_ptr<Message> message)
    std::shared_ptr<Companion> companion, std::shared_ptr<Message> message,
    MessageMetaDataPtr meta)
{
    using json = nlohmann::json;

    json jsonData;

    auto networkId = message->networkId();
    auto type = meta->networkMessageType_;

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
    std::shared_ptr<Companion> companion, const std::string &networkId, const std::string &data)
{
    using json = nlohmann::json;

    json jsonData;

    jsonData["type"] = NetworkMessageType::FILE_DATA;
    jsonData["id"] = networkId;
    jsonData["companion_id"] = companion->id();
    jsonData["data"] = data;

    return jsonData.dump();
}
