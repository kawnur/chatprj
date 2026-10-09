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
    meta->companionName_ = companion->getName();
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

    pushMeta->authorId_ = 1;
    pushMeta->messageType_ = type;

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
    bool result = companion->sendMessage(message, message->meta());

    // mark message as sent
    if (result)
        manager_->markMessageAsSent(companion, message);

    // wait for message reception confirmation
    manager_->waitForMessageReceptionConfirmation(companion, message);
}

void MessageHandler::receiveMessage(CompanionPtr companion, const std::string &json)
{
    nlohmann::json jsonData = buildJsonObject(json);

    // build meta
    auto meta = buildMessageMetaDataFromJson(jsonData);

    if (!meta)
        return;

    meta->companionId_ = companion->getId();

    // build data
    auto data = buildMessageDataFromJson(jsonData);

    if (!data)
        return;

    // build state
    auto state = buildMessageStateFromJson(jsonData);

    if (!state)
        return;

    switch (meta->networkMessageType_) {
    case NetworkMessageType::TEXT: {
        meta->authorId_ = companion->getId();

        receiveTextMessage(companion, meta, data, state);
    }

    break;

    case NetworkMessageType::FILE_PROPOSAL: {
        // add hash to meta
        if (!updateObjectFromJson(meta, jsonData, "hashMD5"))
            return;

        receiveFileProposalMessage(companion, meta, data, state);
    }

    break;

    case NetworkMessageType::RECEIVE_CONFIRMATION: {
        // add 'received' to state
        if (!updateObjectFromJson(state, jsonData, "received"))
            return;

        receiveConfirmation(companion, meta, data, state);
    }

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

    case NetworkMessageType::FILE_DATA: {
        // add data
        if (!updateObjectFromJson(data, jsonData, "data"))
            return;

        receiveFileData(companion, meta, data);
    }

    break;

    case NetworkMessageType::FILE_DATA_CHECK_SUCCESS:
        receiveFileDataCheck(companion, meta, true);

        break;

    case NetworkMessageType::FILE_DATA_CHECK_FAILURE:
        receiveFileDataCheck(companion, meta, false);

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
    auto name = companion->getName();

    meta->companionName_ = name;
    meta->authorName_ = name;
    state->isSent_ = false;
    state->isReceived_ = true;

    // add to DB and get timestamp
    auto replyMeta = manager_->pushMessageToDB(meta, data, state);

    if (!replyMeta || !replyMeta->isValid())
        return;

    replyMeta->messageType_ = MessageType::TEXT;
    replyMeta->networkMessageType_ = NetworkMessageType::RECEIVE_CONFIRMATION;

    // auto message = companion->createMessage(meta, data, state);
    auto message = companion->createMessage(replyMeta, data, state);

    if (!message)
        return;

    // decrypt message

    // add to widget
    manager_->addMessageWidget(companion, message);

    // send reply message to sender
    bool result = companion->sendMessage(message, replyMeta);
}

void MessageHandler::receiveFileProposalMessage(
    CompanionPtr companion, MessageMetaDataPtr meta, MessageDataPtr data, MessageStatePtr state)
{
    // create receiver operator
    companion->addReceiverOperator(meta, HOME_PATH);

    auto name = companion->getName();

    meta->companionName_ = name;
    meta->authorName_ = name;
    state->isSent_ = false;
    state->isReceived_ = true;

    // add to DB and get timestamp
    auto replyMeta = manager_->pushMessageToDB(meta, data, state);

    if (!replyMeta || !replyMeta->isValid())
        return;

    replyMeta->messageType_ = MessageType::FILE;
    replyMeta->networkMessageType_ = NetworkMessageType::NO_ACTION;

    auto message = companion->createMessage(meta, data, state);

    if (!message)
        return;

    // decrypt message

    // add to widget
    manager_->addMessageWidget(companion, message);

    // send reply message to sender
    bool result = companion->sendMessage(message, replyMeta);
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
    // search for message in managers's mapping
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

        bool result = companion->sendMessage(message, replyMeta);
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
    logTemplateInfo("got history request from {}", companion->getName());

    auto group = manager_->getMappedWidgetGroupByCompanion(companion);

    if (group)
        emit group->askUserForHistorySendingConfirmationSignal();
}

void MessageHandler::receiveChatHistoryData(CompanionPtr companion, const nlohmann::json &data)
{
    logTemplateInfo("got chat history from {}", companion->getName());

    // push data to db
    if (!manager_->pushMessageHistoryToDb(companion, data))
        return;

    // clear chat history widget
    manager_->clearChatHistory(companion);

    // fill container with messages
    manager_->fillCompanionMessageMapping(companion, true);

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

void MessageHandler::receiveFileDataCheck(CompanionPtr companion, MessageMetaDataPtr meta, bool success)
{
    logArgs(__FUNCTION__, success);

    std::string entryTemplate = (success)
        ? "file {} received by companion successfully"
        : "file {} WAS NOT received by companion";

    auto networkId = meta->networkId_;

    logTemplateInfo(
        entryTemplate, companion->getFilePathString(networkId));

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

    bool result = companion->sendMessage(message, replyMeta);
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

MessageDataPtr MessageHandler::buildMessageDataFromJson(const nlohmann::json &data)
{
    return buildObjectFromJson<MessageData>(data, "text");
}

MessageMetaDataPtr MessageHandler::buildMessageMetaDataFromJson(const nlohmann::json &data)
{
    return buildObjectFromJson<MessageMetaData>(data, "type", "id", "time");
}

MessageStatePtr MessageHandler::buildMessageStateFromJson(const nlohmann::json &data)
{
    return buildObjectFromJson<MessageState>(data, "antecedent");
}
