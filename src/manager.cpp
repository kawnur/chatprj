#include "manager.hpp"

#include "action.hpp"
#include "application.hpp"
#include "companion.hpp"
#include "logging.hpp"
#include "message.hpp"
#include "utils.hpp"
#include "widgets.hpp"
#include "widgets_dialog.hpp"

using namespace std::string_literals;

Manager::Manager()
    : /*initialized_(false), */dbRequester_(logDBInteraction),
    messageStateToMessageMapMutex_(), dbConnection_(nullptr),
    userIsAuthenticated_(false), selectedCompanion_(nullptr), mapCompanionToWidgetGroup_(),
    lastOpenedPath_(HOME_PATH)
{
    if (!dbRequester_.isReady())
        exitUtil(EXIT_FAILURE);
}

Manager::~Manager()
{
    // free(dbConnection_);
}

std::shared_ptr<Companion> Manager::getSelectedCompanion()
{
    return selectedCompanion_;
}

bool Manager::userIsAuthenticated()
{
    return userIsAuthenticated_;
}

void Manager::set()
{
    bool companionsBuilt = buildCompanions();
    logArgs("companionsBuilt:", companionsBuilt);

    if (companionsBuilt)  // TODO rewrite
        buildWidgetGroups();
    else
        logArgsError("problem with companions initialization");

    // initialized_ = true;
}

std::shared_ptr<Companion> Manager::getMappedCompanionBySocketInfoBaseWidget(
    // std::shared_ptr<SocketInfoBaseWidget> widget) const
    SocketInfoBaseWidget *widget) const
{
    auto lambda = [&](const auto &pair)
    {
        return pair.second.second->getSocketInfoBase().get() == widget;
    };

    auto result = std::ranges::find_if(mapCompanionToWidgetGroup_, lambda);

    return (result == mapCompanionToWidgetGroup_.end()) ? nullptr : result->second.first;
}

std::shared_ptr<WidgetGroup> Manager::getMappedWidgetGroupByCompanion(
    std::shared_ptr<Companion> companion) const
{
    std::shared_ptr<WidgetGroup> group = nullptr;

    auto id = companion->getId();

    try {
        group = mapCompanionToWidgetGroup_.at(id).second;
    } catch(std::out_of_range) {
        logTemplateError("mapCompanionToWidgetGroup_ does not conain id {0}", id);

        return nullptr;
    }

    return group;
}

NetworkMessageType Manager::defineNetworkMessageType(MessageType type)
{
    switch (type) {
    case MessageType::TEXT:
        return NetworkMessageType::TEXT;
    case MessageType::FILE:
        return NetworkMessageType::FILE_PROPOSAL;
    default:
        return NetworkMessageType::UNKNOWN;
    }
}

void Manager::sendMessage(
    MessageType type, std::shared_ptr<Companion> companion, std::shared_ptr<Action> action,
    const std::string &text)
{
    auto group = getMappedWidgetGroupByCompanion(companion);

    if (!group)
        return;

    // encrypt message

    // add to DB and get timestamp
    auto meta = std::make_shared<MessageMetaData>();
    meta->companionName_ = companion->getName();
    meta->authorName_ = "me"s;
    meta->timestampTz_ = "now()"s;

    auto data = std::make_shared<MessageData>();
    data->text_ = text;

    auto state = std::make_shared<MessageState>();
    state->isSent_ = false;
    state->isReceived_ = false;

    auto pushMeta = pushMessageToDB(meta, data, state);

    if (!pushMeta->isValid())
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

        storage->addSenderOperator(message->getNetworkId(), action->getPath());
    }

    group->addMessageWidgetToCentralPanelChatHistory(message);

    // define NetworkMessageType
    auto networkMessageType = defineNetworkMessageType(type);

    // TODO move to manager field object
    // send over network
    bool result = companion->sendMessage(message, message->meta());

    // mark message as sent
    if (result)
        markMessageAsSent(companion, message);

    // wait for message reception confirmation
    waitForMessageReceptionConfirmation(companion, message);
}

void Manager::sendFile(std::shared_ptr<Companion> companion, const std::filesystem::path &path)
{
    logArgs(__FUNCTION__);
}

void Manager::receiveTextMessage(
    std::shared_ptr<Companion> companion, std::shared_ptr<MessageMetaData> meta,
    std::shared_ptr<MessageData> data, std::shared_ptr<MessageState> state)
{
    auto name = companion->getName();

    meta->companionName_ = name;
    meta->authorName_ = name;
    state->isSent_ = false;
    state->isReceived_ = true;

    // add to DB and get timestamp
    auto replyMeta = pushMessageToDB(meta, data, state);

    if (!replyMeta || !replyMeta->isValid())
        return;

    replyMeta->messageType_ = MessageType::TEXT;
    replyMeta->networkMessageType_ = NetworkMessageType::RECEIVE_CONFIRMATION;

    auto message = companion->createMessage(meta, data, state);

    if (!message)
        return;

    // decrypt message

    // add to widget
    auto group = getMappedWidgetGroupByCompanion(companion);
    emit group->addMessageWidgetToCentralPanelChatHistorySignal(message);

    // send reply message to sender
    bool result = companion->sendMessage(message, replyMeta);
}

void Manager::receiveFileProposalMessage(
    std::shared_ptr<Companion> companion, std::shared_ptr<MessageMetaData> meta,
    std::shared_ptr<MessageData> data, std::shared_ptr<MessageState> state)
{
    // create receiver operator
    companion->addReceiverOperator(meta, HOME_PATH);

    auto name = companion->getName();

    meta->companionName_ = name;
    meta->authorName_ = name;
    state->isSent_ = false;
    state->isReceived_ = true;

    // add to DB and get timestamp
    auto replyMeta = pushMessageToDB(meta, data, state);

    if (!replyMeta || !replyMeta->isValid())
        return;

    replyMeta->messageType_ = MessageType::FILE;
    replyMeta->networkMessageType_ = NetworkMessageType::NO_ACTION;

    auto message = companion->createMessage(meta, data, state);

    if (!message)
        return;

    // decrypt message

    // add to widget
    auto group = getMappedWidgetGroupByCompanion(companion);
    emit group->addMessageWidgetToCentralPanelChatHistorySignal(message);

    // send reply message to sender
    bool result = companion->sendMessage(message, replyMeta);
}

void Manager::receiveConfirmation(
    std::shared_ptr<Companion> companion, std::shared_ptr<MessageMetaData> meta,
    std::shared_ptr<MessageData> data, std::shared_ptr<MessageState> state)
{
    if (state->isReceived_) {  // successfully received
        // mark message as received
        auto info = companion->getMessageInfoByNetworkId(meta->networkId_);
        auto state = info->getMessage()->state();

        if (state) {
            // found message in mapping
            state->isReceived_ = true;
            markMessageAsReceived(info);
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

void Manager::receiveConfirmationRequest(
    std::shared_ptr<Companion> companion, std::shared_ptr<MessageMetaData> meta,
    std::shared_ptr<MessageData> data, std::shared_ptr<MessageState> state)
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

void Manager::reciveChatHistoryRequest(std::shared_ptr<Companion> companion)
{
    logTemplateInfo("got history request from {}", companion->getName());

    emit getMappedWidgetGroupByCompanion(companion)->askUserForHistorySendingConfirmationSignal();
}

void Manager::receiveChatHistoryData(
    std::shared_ptr<Companion> companion, const nlohmann::json &data)
{
    logTemplateInfo("got chat history from {}", companion->getName());

    // push data to db
    if (!pushMessageHistoryToDb(companion, data))
        return;

    // clear chat history widget
    clearChatHistory(companion);

    // fill container with messages
    fillCompanionMessageMapping(companion, true);

    // build chat history
    emit getMappedWidgetGroupByCompanion(companion)->buildChatHistorySignal();
}

void Manager::receiveFileRequest(
    std::shared_ptr<Companion> companion, std::shared_ptr<MessageMetaData> meta)
{
    logArgs(__FUNCTION__);

    auto networkId = meta->networkId_;
    auto sender = companion->getFileOperatorByNetworkId<SenderOperator>(networkId);

    if (sender)
        sender->sendFile(companion, networkId);
    else
        logTemplateError("companion has no file operator for networkId = {}", networkId);
}

void Manager::receiveFileData(
    std::shared_ptr<Companion> companion, std::shared_ptr<MessageMetaData> meta,
    std::shared_ptr<MessageData> data)
{
    logArgs(__FUNCTION__);

    auto networkId = meta->networkId_;
    auto receiver = companion->getFileOperatorByNetworkId<ReceiverOperator>(networkId);

    if (receiver)
        receiver->receiveFilePart(data->data_);
    else
        logTemplateError("companion has no file operator for networkId = {}", networkId);
}

void Manager::receiveFileDataCheck(
    std::shared_ptr<Companion> companion, std::shared_ptr<MessageMetaData> meta,
    bool success)
{
    logArgs(__FUNCTION__, success);

    std::string entryTemplate = (success)
        ? "file {} received by companion successfully"
        : "file {} WAS NOT received by companion";

    auto networkId = meta->networkId_;

    logTemplateInfo(
        entryTemplate, companion->getFileOperatorFilePathStringByNetworkId(networkId));

    companion->removeFileOperator<SenderOperator>(networkId);
}

void Manager::receiveFileDataTransmissionEnd(
    std::shared_ptr<Companion> companion, std::shared_ptr<MessageMetaData> meta)
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

void Manager::receiveFileDataTransmissionFailure(
    std::shared_ptr<Companion> companion, std::shared_ptr<MessageMetaData> meta)
{
    logArgs(__FUNCTION__);

    auto networkId = meta->networkId_;

    logTemplateInfo(
        "file {} WAS NOT received by companion",
        companion->getFileOperatorFilePathStringByNetworkId(networkId));

    companion->removeFileOperator<SenderOperator>(networkId);
}

std::shared_ptr<MessageData> Manager::buildMessageDataFromJson(const nlohmann::json &data)
{
    return buildObjectFromJson<MessageData>(data, "text");
}

std::shared_ptr<MessageMetaData> Manager::buildMessageMetaDataFromJson(const nlohmann::json &data)
{
    return buildObjectFromJson<MessageMetaData>(data, "type", "companion_id", "id");
}

std::shared_ptr<MessageState> Manager::buildMessageStateFromJson(const nlohmann::json &data)
{
    return buildObjectFromJson<MessageState>(data, "antecedent");
}

bool Manager::pushMessageHistoryToDb(
    std::shared_ptr<Companion> companion, const nlohmann::json &data)
{
    // TODO wrap in util

    for (std::size_t i = 0; i < data["messages"].size(); i++) {
        uint8_t authorId = std::stoi(data["messages"][i]["author_id"].get<std::string>());
        authorId = (authorId == 1) ? companion->getId() : 1;
        std::string timestamp = data["messages"][i]["timestamp_tz"];
        std::string text = data["messages"][i]["message"];
        uint8_t companionId = companion->getId();

        // check if message from this companion with such timestamp already exists
        auto messageGetData = getDBData(
            DBRequestType::GET_MESSAGE_BY_COMPANION_ID_AND_TIMESTAMP, companionId, timestamp);

        if (!messageGetData)
            return false;

        if (!messageGetData->isEmpty()) {
            auto entryTemplate =
                "Message with timestamp {0} from companion with id {1} already exists";

            showInfoDialogAndLogInfo(getStringByFormat(entryTemplate, timestamp, companionId));

            continue;
        }

        std::string idString { "id" };

        // push message to db
        auto messageAddData = getDBData(
            DBRequestType::PUSH_MESSAGE_AND_RETURN, companion->getName(),
            std::to_string(authorId), timestamp, idString, text, true, true);

        if (!messageAddData || messageAddData->isEmpty())
            return false;
    }

    return true;
}

void Manager::receiveMessage(std::shared_ptr<Companion> companion, const std::string &json)
{
    nlohmann::json jsonData = buildJsonObject(json);

    // build meta
    auto meta = buildMessageMetaDataFromJson(jsonData);

    if (!meta)
        return;

    // build data
    auto data = buildMessageDataFromJson(jsonData);

    if (!data)
        return;

    // build state
    auto state = buildMessageStateFromJson(jsonData);

    if (!state)
        return;

    switch (meta->networkMessageType_) {
    case NetworkMessageType::TEXT:
        receiveTextMessage(companion, meta, data, state);

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

void Manager::addEarlyMessages(std::shared_ptr<Companion> companion)
{
    if (companion)
        return;

    // get earliest message in current messages
    auto earliestMessage = companion->getEarliestMessage();

    auto messageId = earliestMessage->getId();
    auto companionId = companion->getId();

    // get messages data
    auto messagesData = getDBData(
        DBRequestType::GET_EARLY_MESSAGES_BY_MESSAGE_ID, companionId, messageId);

    if (!messagesData)
        return;

    if (messagesData->isEmpty()) {
        logTemplateWarning(
            "no messages earlier than id = {0} in db with companion {1}", messageId, companionId);

        return;
    }

    auto widgetGroup = getMappedWidgetGroupByCompanion(companion);

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

void Manager::resetSelectedCompanion(std::shared_ptr<Companion> companion)  // TODO rewrite
{
    auto graphicManager = getGraphicManager();

    if (selectedCompanion_) {
        auto widgetGroup = getMappedWidgetGroupByCompanion(selectedCompanion_);
        auto cast = dynamic_pointer_cast<SocketInfoWidget>(widgetGroup->getSocketInfoBase());

        if (cast)
            cast->unselect();

        widgetGroup->hideCentralPanel();
    }
    else {
        graphicManager->hideCentralPanelStub();
    }

    selectedCompanion_ = companion;

    if (selectedCompanion_) {
        auto widgetGroup = getMappedWidgetGroupByCompanion(selectedCompanion_);
        auto cast = dynamic_pointer_cast<SocketInfoWidget>(widgetGroup->getSocketInfoBase());

        if (cast)
            cast->select();

        widgetGroup->showCentralPanel();
    }
    else {
        graphicManager->showCentralPanelStub();
    }
}

void Manager::createCompanion(std::shared_ptr<CompanionAction> action)
{
    // data validation and checking
    if (!(companionDataValidation(action) && checkCompanionDataForExistanceAtCreation(action)))
        return;

    auto name = action->getName();
    auto ipAddress = action->getIpAddress();
    auto clientPortStr = action->getClientPort();

    // push companion data to db
    auto companionIdData = getDBData(DBRequestType::PUSH_COMPANION_AND_RETURN, name);

    if (!companionIdData || companionIdData->isEmpty())
        return;

    int id = std::stoi(companionIdData->getValue(0, "id"));

    // push socket data to db
    uint16_t serverPort = 5000 + id + 1;  // TODO change

    auto socketData = getDBData(
        DBRequestType::PUSH_SOCKET_AND_RETURN, name, ipAddress, std::to_string(serverPort),
        clientPortStr);

    if (!socketData || socketData->isEmpty())
        return;

    // create Companion object
    auto companion = addCompanionObject(id, name);

    if (!companion) {
        logArgsError("companion is nullptr");

        return;
    }

    // create SocketInfo object
    auto socketInfo = std::make_shared<SocketInfo>(ipAddress, serverPort, std::stoi(clientPortStr));

    companion->setSocketInfo(socketInfo);

    // add companion and widget group to mapping
    createWidgetGroupAndAddToMapping(companion);

    // show info dialog
    getGraphicManager()->showCompanionInfoDialog(action, "New companion added:\n\n");
}

void Manager::updateCompanion(std::shared_ptr<CompanionAction> action)
{
    // data validation and checking
    if (!(companionDataValidation(action) && checkCompanionDataForExistanceAtUpdate(action)))
        return;

    // update companion data at db
    auto companionIdData = getDBData(
        DBRequestType::UPDATE_COMPANION_AND_SOCKET_AND_RETURN, action->getName(),
        action->getCompanionId(), action->getIpAddress(), action->getClientPort());

    if (!companionIdData || companionIdData->isEmpty())
        return;

    // update Companion and SocketInfo object
    action->updateCompanionObjectData();

    // update SocketInfoWidget
    auto widgetGroup = getMappedWidgetGroupByCompanion(action->getCompanion());
    widgetGroup->getSocketInfoBase()->update();

    // show info dialog
    getGraphicManager()->showCompanionInfoDialog(action, "Companion updated:\n\n");
}

void Manager::deleteCompanion(std::shared_ptr<CompanionAction> action)
{
    // delete companion chat messages from db
    auto companionIdMessagesData = getDBData(
        DBRequestType::DELETE_MESSAGES_AND_RETURN, action->getCompanionId());

    if (!companionIdMessagesData)
        return;

    if (companionIdMessagesData->isEmpty()) {
        // no return, may be companion without messages
        // showWarningDialogAndLogWarning("Empty db reply to companion messages deletion");
    }

    // delete companion and socket from db
    auto companionIdCompanionData = getDBData(
        DBRequestType::DELETE_COMPANION_AND_SOCKET_AND_RETURN, action->getCompanionId());

    if (!companionIdCompanionData || companionIdCompanionData->isEmpty())
        return;

    // delete companion object
    deleteCompanionObject(action->getCompanion());

    // show info dialog
    getGraphicManager()->showCompanionInfoDialog(action, "Companion deleted:\n\n");
}

void Manager::clearChatHistory(std::shared_ptr<Companion> companion)
{
    auto widgetGroup = getMappedWidgetGroupByCompanion(companion);
    getGraphicManager()->clearChatHistory(widgetGroup);
}

void Manager::clearCompanionHistory(std::shared_ptr<CompanionAction> action)
{
    // delete companion chat messages from db
    auto companionIdMessagesData = getDBData(
        DBRequestType::DELETE_MESSAGES_AND_RETURN, action->getCompanionId());

    if (!companionIdMessagesData)
        return;

    if (companionIdMessagesData->isEmpty()) {
        // no return, may be companion without messages
        // showWarningDialogAndLogWarning("Empty db reply to companion messages deletion");
    }

    // clear companion's message mapping
    action->getCompanion()->clearMessageMapping();

    // clear chat history widget
    clearChatHistory(action->getCompanion());

    // show info dialog
    getGraphicManager()->showCompanionInfoDialog(action, "Companion chat history cleared:\n\n");
}

void Manager::createUserPassword(std::shared_ptr<PasswordAction> action)
{
    // data validation and checking
    if (!(passwordDataValidation(action)))
        return;

    // push password data to db
    auto passwordIdData = getDBData(DBRequestType::PUSH_PASSWORD_AND_RETURN, action->getPassword());

    if (!passwordIdData || passwordIdData->isEmpty())
        return;

    // show dialog
    showInfoDialogAndLogInfo(
        newPasswordCreatedLabel, &TextDialog::unsetMainWindowBlurAndCloseDialogs,
        action->getDialog());
}

void Manager::authenticateUser(std::shared_ptr<PasswordAction> action)
{
    auto graphicManager = getGraphicManager();

    // do we have password in db?
    auto passwordData = getDBData(DBRequestType::GET_PASSWORD);

    if (!passwordData || passwordData->isEmpty())
        return;

    if (passwordData->getValue(0, "password") != action->getPassword()) {
        showErrorDialogAndLogError("Password is not correct");

        return;
    }

    userIsAuthenticated_ = true;

    logArgsInfo("user successfully authenticated");

    graphicManager->disableMainWindowBlurEffect();
}

void Manager::hideSelectedCompanionCentralPanel()
{
    if (selectedCompanion_) {
        auto group = getMappedWidgetGroupByCompanion(selectedCompanion_);
        getGraphicManager()->hideWidgetGroupCentralPanel(group);
    }
}

void Manager::showSelectedCompanionCentralPanel()
{
    if (selectedCompanion_) {
        auto group = getMappedWidgetGroupByCompanion(selectedCompanion_);
        getGraphicManager()->showWidgetGroupCentralPanel(group);
    }
}

void Manager::startUserAuthentication()
{
    auto graphicManager = getGraphicManager();
    graphicManager->enableMainWindowBlurEffect();

    // do we have password in db?
    auto passwordData = getDBData(DBRequestType::GET_PASSWORD);

    if (!passwordData)
        return;

    if (passwordData->isEmpty())
        graphicManager->createEntrancePassword();
    else
        graphicManager->getEntrancePassword();
}

void Manager::sendUnsentMessages(std::shared_ptr<Companion> companion)
{
    // get unsent messages from db
    auto messagesData = getDBData(
        DBRequestType::GET_UNSENT_MESSAGES_BY_COMPANION_NAME, companion->getName());

    if (!messagesData || messagesData->isEmpty())
        return;

    for (std::size_t i = 0; i < messagesData->size(); i++) {  // TODO switch to iterators
        uint32_t messageId = std::stoi(messagesData->getValue(i, "id"));
        auto message = companion->findMessage(messageId);
        std::string networkId;

        if (message) {
            if (!message->state()) {
                logArgsError(
                    "strange case: unsent message found in companions messages, "
                    "but not found in companion's messageMapping_");
            }
            else {
                networkId = message->getNetworkId();
            }
        }
        else {
            // add to companion's messages if needed
            networkId = getRandomString(5);

            auto meta = std::make_shared<MessageMetaData>();
            meta->messageType_ = MessageType::TEXT;
            meta->messageId_ = messageId;
            meta->companionId_ = companion->getId();
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
        bool result = companion->sendMessage(message, message->meta());

        // mark message as sent
        if (result)
            markMessageAsSent(companion, message);
    }
}

void Manager::requestHistoryFromCompanion(std::shared_ptr<Companion> companion)
{
    auto meta = std::make_shared<MessageMetaData>();
    meta->networkMessageType_ = NetworkMessageType::CHAT_HISTORY_REQUEST;

    auto state = std::make_shared<MessageState>();
    state->isAntecedent_ = true;

    auto message = companion->createMessage(meta, nullptr, state);

    bool result = companion->sendMessage(message, message->meta());
}

void Manager::sendChatHistoryToCompanion(std::shared_ptr<Companion> companion)
{
    logArgs(__FUNCTION__);

    auto keys = buildStringVector("author_id", "timestamp_tz", "message");

    // get messages from db
    auto messagesData = getDBData(
        DBRequestType::GET_ALL_MESSAGES_BY_COMPANION_ID, companion->getId());

    if (!messagesData || messagesData->isEmpty())
        return;

    bool result = companion->sendChatHistory(messagesData, keys);
}

// bool Manager::isInitialised()
// {
//     return initialized_;
// }

std::filesystem::path Manager::getLastOpenedPath()
{
    return lastOpenedPath_;
}

void Manager::setLastOpenedPath(const std::filesystem::path &path)
{
    lastOpenedPath_ = path;
}

std::shared_ptr<Companion> Manager::getMappedCompanionByWidgetGroup(
    std::shared_ptr<WidgetGroup> group) const
{
    auto lambda = [&](const auto &pair)
    {
        return pair.second.second == group;
    };

    auto result = std::ranges::find_if(mapCompanionToWidgetGroup_, lambda);

    return result->second.first;
}

void Manager::fillCompanionMessageMapping(
    std::shared_ptr<Companion> companion, bool containersNotEmpty)
{
    uint8_t companionId = companion->getId();

    // get messages data
    auto messagesData = getDBData(
        DBRequestType::GET_MESSAGES, companionId, NUMBER_OF_MESSAGES_TO_GET_FROM_DB);

    if (!messagesData || messagesData->isEmpty())
        return;

    for (std::size_t i = 0; i < messagesData->size(); i++) {  // TODO switch to iterators
        auto messageId = std::stoi(messagesData->getValue(i, "id"));

        if (containersNotEmpty) {
            auto info = companion->getMessageInfoByMessageId(messageId);

            if (info && info->getMessage()->state()) {
                // companion->addMessage(const_cast<std::shared_ptr<Message>>(pair.second));
            } else {
                companion->createMessageAndAddToMapping(messagesData, i);
            }
        }
        else {
            companion->createMessageAndAddToMapping(messagesData, i);
        }
    }
}

bool Manager::buildCompanions()
{
    bool companionsDataIsOk = true;

    // get companion data
    auto companionsData = getDBData(DBRequestType::GET_COMPANIONS);

    if (!companionsData)
        return false;

    // auto lambda = [&](auto &iterator1, auto &iterator2)
    // {
    //     return iterator1.at("id") < iterator2.at("id");
    // };

    // std::ranges::sort(companionsData->getData(), lambda);

    for (std::size_t index = 0; index < companionsData->size(); index++) {  // TODO switch to iterators
        int id = std::stoi(companionsData->getValue(index, "id"));

        // create companion object
        auto companion = addCompanionObject(id, companionsData->getValue(index, "name"));

        if (!companion) {
            logArgsError("companion is nullptr");

            continue;
        }

        // get socket data object
        auto socketsData = getDBData(DBRequestType::GET_SOCKET_INFO, id);

        if (!socketsData || socketsData->isEmpty())
            return false;

        // TODO use port number pool
        auto socketInfo = std::make_shared<SocketInfo>(
            socketsData->getValue(0, "ipaddress"),
            std::stoi(socketsData->getValue(0, "server_port")),
            std::stoi(socketsData->getValue(0, "client_port")));

        companion->setSocketInfo(socketInfo);

        if (companion->getId() > 1) {  // TODO change condition
            fillCompanionMessageMapping(companion, false);

            if (!companion->startServer())
                logArgsError("problem with server start for companion id", id);

            if (!companion->createClient())
                logArgsError("problem with client creation for companion id", id);
        }
    }

    return companionsDataIsOk;
}

void Manager::buildWidgetGroups()
{
    auto graphicManager = getGraphicManager();

    auto companionsNumber = mapCompanionToWidgetGroup_.size();
    auto childrenSize = graphicManager->getCompanionPanelChildrenSize();

    logTemplateInfo("companionsNumber: {0}, childrenSize: {1}", companionsNumber, childrenSize);

    if (companionsNumber == 0 && childrenSize == 0) {
        logArgsWarning("strange case, empty sockets panel");
    }
    else {
        // TODO check if sockets already are children

        // hide companion panel stub widget
        graphicManager->hideCompanionPanelStub();

        for (const auto &pair : mapCompanionToWidgetGroup_)
            createWidgetGroupAndAddToMapping(pair.second.first);
    }
}

std::shared_ptr<Companion> Manager::addCompanionObject(int id, const std::string &name)
{
    if (id == 0) {
        logArgsError("companion id == 0");

        return nullptr;
    }

    // auto result = mapCompanionIdToCompanionInfo_.emplace(
    //     std::make_pair(
    //         id,
    //         std::pair<std::shared_ptr<Companion>, std::shared_ptr<WidgetGroup>>(new Companion(id, name), nullptr)));

    auto companion = std::make_shared<Companion>(id, name);
    std::shared_ptr<WidgetGroup> group = nullptr;
    auto value = std::pair(companion, group);
    auto result = mapCompanionToWidgetGroup_.emplace(id, value);

    return (result.second) ? result.first->second.first : nullptr;
}

void Manager::createWidgetGroupAndAddToMapping(std::shared_ptr<Companion> companion)
{
    auto widgetGroup = std::make_shared<WidgetGroup>(companion);
    widgetGroup->set();
    mapCompanionToWidgetGroup_[companion->getId()].second = widgetGroup;
    companion->addMessageWidgetsToChatHistory();
}

void Manager::deleteCompanionObject(std::shared_ptr<Companion> companion)
{
    deleteWidgetGroupAndDeleteFromMapping(companion);
}

void Manager::deleteWidgetGroupAndDeleteFromMapping(std::shared_ptr<Companion> companion)
{
    auto lambda = [&](const auto &iterator)
    {
        return iterator.second.first == companion;
    };

    // TODO use range
    auto result = std::ranges::find_if(mapCompanionToWidgetGroup_, lambda);

    if (result == mapCompanionToWidgetGroup_.end()) {
        showErrorDialogAndLogError("Companion was not found in mapping at deletion");
    }
    else {
        if (selectedCompanion_ == companion)
            selectedCompanion_ = nullptr;

        mapCompanionToWidgetGroup_.erase(result);
    }
}

bool Manager::companionDataValidation(std::shared_ptr<CompanionAction> action)
{
    std::vector<std::string> validationErrors {};

    bool validationResult = validateCompanionData(validationErrors, action);

    if (!validationResult) {
        showErrorDialogAndLogError(buildDialogText("Error messages:\n\n", validationErrors));

        return false;
    }

    return true;
}

bool Manager::passwordDataValidation(std::shared_ptr<PasswordAction> action)
{
    std::vector<std::string> validationErrors {};

    bool validationResult = validatePassword(validationErrors, action->getPassword());

    if (!validationResult) {
        showErrorDialogAndLogError(buildDialogText("Error messages:\n\n", validationErrors));

        return false;
    }

    return true;
}

bool Manager::checkCompanionDataForExistanceAtCreation(
    std::shared_ptr<CompanionAction> action)
{
    // check if companion with such name already exists
    auto companionIdData = getDBData(DBRequestType::GET_COMPANION_BY_NAME, action->getName());

    if (!companionIdData)
        return false;

    if (!companionIdData->isEmpty()) {
        showErrorDialogAndLogError("Companion with such name already exists");

        return false;
    }

    // check if such socket already exists
    auto socketIdData = getDBData(
        DBRequestType::GET_SOCKET_BY_IP_ADDRESS_AND_PORT, action->getIpAddress(),
        action->getClientPort());

    if (!socketIdData)
        return false;

    if (!socketIdData->isEmpty()) {
        showErrorDialogAndLogError("Companion with such socket already exists");

        return false;
    }

    return true;
}

bool Manager::checkCompanionDataForExistanceAtUpdate(
    std::shared_ptr<CompanionAction> action)
{
    // check if companion with such name already exists
    auto companionData = getDBData(DBRequestType::GET_COMPANION_BY_NAME, action->getName());

    if (!companionData)
        return false;

    bool findNameResult = companionData->findValue("id"s, std::to_string(action->getCompanionId()));

    bool nameExistsAtOtherCompanion =
        (findNameResult && companionData->size() > 1) ||
        (!findNameResult && companionData->size() > 0);

    if (nameExistsAtOtherCompanion)
        // no return
        showWarningDialogAndLogWarning("Companion with such name already exists");

    // check if such socket already exists
    auto socketData = getDBData(
        DBRequestType::GET_SOCKET_BY_IP_ADDRESS_AND_PORT, action->getIpAddress(),
        action->getClientPort());

    if (!socketData)
        return false;

    bool findSocketResult = socketData->findValue("id"s, std::to_string(action->getCompanionId()));

    bool socketExistsAtOtherCompanion =
        (findSocketResult && socketData->size() > 1) ||
        (!findSocketResult && socketData->size() > 0);

    if (socketExistsAtOtherCompanion) {
        showErrorDialogAndLogError("Companion with such socket already exists");

        return false;
    }

    return true;
}

void Manager::waitForMessageReceptionConfirmation(
    std::shared_ptr<Companion> companion, std::shared_ptr<Message> message)
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

            bool result = companion->sendMessage(message, meta);

            // sleep
            sleepForMS(sleepDuration);
            sleepDuration *= SLEEP_DURATION_INCREASE_RATE;
        }
    };

    runInDetachedThread(lambda);
}

void Manager::markMessageAsSent(
    std::shared_ptr<Companion> companion, std::shared_ptr<Message> message)
{
    // mark in db
    auto messageIdData = getDBData(DBRequestType::SET_MESSAGE_IS_SENT_AND_RETURN, message->getId());

    if (!messageIdData)
        return;

    // mark in widget
    if (!getGraphicManager()->markMessageWidgetAsSent(companion, message))
        logArgsError("marking widget as sent error");
}

void Manager::markMessageAsReceived(
    // std::shared_ptr<Companion> companion, std::shared_ptr<Message> message)
    std::shared_ptr<MessageInfo> info)
{
    // mark in widget
    getGraphicManager()->markMessageWidgetAsReceived(info->getWidget());

    // if (!result)
    //     logArgsError("marking wiget as received error");

    // mark in db
    auto result = getDBData(
        DBRequestType::SET_MESSAGE_IS_RECEIVED_AND_RETURN, info->getMessage()->getId());
}

std::shared_ptr<MessageMetaData> Manager::pushMessageToDB(
    // const std::string &companionName, const std::string &authorName, const std::string &timestamp,
    // const std::string &text, const bool &isSent, const bool &isReceived)
    std::shared_ptr<MessageMetaData> meta, std::shared_ptr<MessageData> data,
    std::shared_ptr<MessageState> state)
{
    const std::string companionIdString("companion_id");

    auto messageData = getDBData(
        DBRequestType::PUSH_MESSAGE_AND_RETURN, meta->companionName_, meta->authorName_,
        meta->timestampTz_, companionIdString, data->text_, state->isSent_, state->isReceived_);

    // TODO get rid of copy constructing
    if (!messageData || messageData->isEmpty())
        return nullptr;

    uint32_t id = std::stoi(messageData->getValue(0, "id"));
    uint8_t companionId = std::stoi(messageData->getValue(0, "companion_id"));
    std::string timestampTz { messageData->getValue(0, "timestamp_tz") };

    if (logDBInteraction)
        logTemplateInfo("companionId: {0}, timestampTz: {1}", companionId, timestampTz);

    // TODO get rid of copy constructing
    auto result = std::make_shared<MessageMetaData>();

    result->messageId_ = id;
    result->companionId_ = companionId;
    result->authorId_ = 0;
    result->timestampTz_ = timestampTz;

    return result;
}

std::shared_ptr<Manager> getManager()
{
    QCoreApplication *coreApp = QCoreApplication::instance();
    ChatApp *app = dynamic_cast<ChatApp *>(coreApp);

    if (!app)
        return nullptr;

    return app->manager_;
}
