#include "manager.hpp"

#include <thread>

#include "action.hpp"
#include "application.hpp"
#include "companion.hpp"
#include "db_interaction.hpp"
#include "file_info.hpp"
#include "message.hpp"
#include "widgets.hpp"
#include "widgets_dialog.hpp"

using namespace std::string_literals;

Manager::Manager()
    : initialized_(false), messageStateToMessageMapMutex_(), dbConnection_(nullptr),
    userIsAuthenticated_(false)
{
    mapCompanionIdToCompanionInfo_ =
        std::map<int, std::pair<std::shared_ptr<Companion>, std::shared_ptr<WidgetGroup>>>();

    selectedCompanion_ = nullptr;
    lastOpenedPath_ = homePath;
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

std::shared_ptr<Companion> Manager::getMappedCompanionBySocketInfoBaseWidget(
    // std::shared_ptr<SocketInfoBaseWidget> widget) const
    SocketInfoBaseWidget *widget) const
{
    auto findWidget = [&](auto &pair)
    {
        return pair.second.second->getSocketInfoBase().get() == widget;
    };

    auto result = std::find_if (
        mapCompanionIdToCompanionInfo_.cbegin(),
        mapCompanionIdToCompanionInfo_.cend(),
        findWidget);

    return result->second.first;
}

std::shared_ptr<WidgetGroup> Manager::getMappedWidgetGroupByCompanion(
    std::shared_ptr<Companion> companion) const
{
    std::shared_ptr<WidgetGroup> group = nullptr;

    try {
        group = mapCompanionIdToCompanionInfo_.at(companion->getId()).second;
    }
    catch(std::out_of_range) {}

    return group;
}

void Manager::set()
{
    bool connectedToDB = connectToDb();
    // logArgs("connectedToDB:", connectedToDB);

    if (connectedToDB) {
        bool companionsBuilt = buildCompanions();
        logArgs("companionsBuilt:", companionsBuilt);

        if (companionsBuilt)  // TODO rewrite
            buildWidgetGroups();
        else
            logArgsError("problem with companions initialization");
    }
    else {
        showErrorDialogAndLogError("problem with DB connection");
    }

    initialized_ = true;
}

void Manager::sendMessage(
    MessageType type, std::shared_ptr<Companion> companion, std::shared_ptr<Action> action,
    const std::string &text)
{
    auto group = getMappedWidgetGroupByCompanion(companion);

    // encrypt message

    // add to DB and get timestamp
    auto tuple = pushMessageToDB(companion->getName(), "me"s, "now()"s, text, false, false);

    uint32_t id = std::get<0>(tuple);
    uint8_t companion_id = std::get<1>(tuple);
    std::string timestamp = std::get<2>(tuple);

    if (companion_id == 0 || timestamp == "") {
        logArgsError("error adding message to db");

        return;
    }

    auto pair = companion->createMessageAndAddToMapping(
        type, id, 1, timestamp, text, false, false, false, "");

    if (!pair.second)
        return;

    // add to widget
    auto message = pair.first->first;
    auto messageState = pair.first->second->getState();

    if (type == MessageType::FILE) {
        auto storage = companion->getFileOperatorStorage();

        if (!storage)
            return;

        storage->addSenderOperator(messageState->getNetworkId(), action->getPath());
    }

    group->addMessageWidgetToCentralPanelChatHistory(message, messageState);

    NetworkMessageType networkMessageType;

    switch (type) {
    case MessageType::TEXT:
        networkMessageType = NetworkMessageType::TEXT;

        break;

    case MessageType::FILE:
        networkMessageType = NetworkMessageType::FILE_PROPOSAL;

        break;
    }

    // send over network
    bool result = companion->sendMessage(
        false, networkMessageType, messageState->getNetworkId(), message);

    // mark message as sent
    if (result)
        markMessageAsSent(companion, message);

    // wait for message reception confirmation
    waitForMessageReceptionConfirmation(companion, messageState, message);
}

void Manager::sendFile(std::shared_ptr<Companion> companion, const std::filesystem::path &path)
{
    logArgs("Manager::sendFile");
}

void Manager::receiveMessage(std::shared_ptr<Companion> companion, const std::string &jsonString)
{
    nlohmann::json jsonData = buildJsonObject(jsonString);

    NetworkMessageType type;
    int companionId;
    std::string networkId;
    bool isAntecedent;

    runAndLogException(
        [&](){
            type = jsonData.at("type");
            companionId = jsonData.at("companion_id");
            networkId = jsonData.at("id");
            isAntecedent = jsonData.at("antecedent");
        });

    switch (type) {
    // message is data message
    case NetworkMessageType::TEXT:
    case NetworkMessageType::FILE_PROPOSAL:
    {
        MessageType messageType;
        NetworkMessageType replyMessageType;

        switch (type) {
        case NetworkMessageType::TEXT:
        {
            messageType = MessageType::TEXT;
            replyMessageType = NetworkMessageType::RECEIVE_CONFIRMATION;
        }

        break;

        case NetworkMessageType::FILE_PROPOSAL:
        {
            messageType = MessageType::FILE;
            replyMessageType = NetworkMessageType::NO_ACTION;

            std::string hashMD5FromSender = jsonData.at("hashMD5");

            // create receiver operator
            companion->getFileOperatorStorage()->
                addReceiverOperator(networkId, hashMD5FromSender, homePath);
        }

        break;
        }

        auto timestamp = jsonData.at("time");
        auto text = jsonData.at("text");
        auto name = companion->getName();

        // add to DB and get timestamp
        auto tuple = pushMessageToDB(name, name, timestamp, text, false, true);

        uint32_t id = std::get<0>(tuple);
        uint8_t companion_id = std::get<1>(tuple);

        if (companion_id == 0 || timestamp == "") {
            logArgsError("error adding message to db");

            return;
        }

        auto pair = companion->createMessageAndAddToMapping(
            messageType, id, companion_id, timestamp, text, isAntecedent, false, true,
            networkId);

        if (pair.second)
            return;

        auto message = pair.first->first;
        auto messageState = pair.first->second->getState();

        // decrypt message

        // add to widget
        auto group = getMappedWidgetGroupByCompanion(companion);
        emit group->addMessageWidgetToCentralPanelChatHistorySignal(messageState, message);

        // send message to sender
        bool result = companion->sendMessage(false, replyMessageType, networkId, message);
    }

    break;

    // message is confirmation
    case NetworkMessageType::RECEIVE_CONFIRMATION:
    {
        auto received = jsonData.at("received");

        if (received == 1) {  // successfully received
            // mark message as received
            // std::string key = generateMessageKey(networkId, companion->getId());

            // auto pair = getMessageStateAndMessageMappingPairByMessageMappingKey(key);
            // auto pair = companion->getMessageMappingPairByMessageKey(key);
            auto pair = companion->getMessageMappingPairByNetworkId(networkId);
            auto state = pair.second->getState();

            if (state) {
                // found message in mapping
                state->setIsReceived(true);
                markMessageAsReceived(companion, pair.first);
            }
            else {
                // strange situation
                logArgsError(
                    "received confirmation for message which is not in messageMapping_");
            }
        }
        else {
            // message was not received, resend message
        }
    }

    break;

    case NetworkMessageType::RECEIVE_CONFIRMATION_REQUEST:
    {
        // search for message in managers's mapping
        // std::string key = generateMessageKey(networkId, companion->getId());

        // auto pair = companion->getMessageMappingPairByMessageKey(key);
        auto pair = companion->getMessageMappingPairByNetworkId(networkId);

        auto message = pair.first;
        auto info = pair.second;

        logArgs("message:", message, "info:", info);

        if (!info) {
            // probably old message from previous sessions, search for it in db
            logArgsInfo("probably old message from previous sessions, search for it in db");

            break;
        }

        auto state = info->getState();

        if (!state)
            break;

        // message found in managers's mapping
        if (state->isReceived()) {
            auto type = NetworkMessageType::RECEIVE_CONFIRMATION;
            auto id = state->getNetworkId();

            bool result = companion->sendMessage(false, type, id, message);
        }
        else {
            // strange situation
            logArgsError(
                "received reception confirmation request "
                "for message in messageMapping_ with isReceived = false");
        }
    }

    break;

    case NetworkMessageType::CHAT_HISTORY_REQUEST:
    {
        logArgsInfo("got history request from " + companion->getName());

        emit getMappedWidgetGroupByCompanion(companion)->
            askUserForHistorySendingConfirmationSignal();
    }

    break;

    case NetworkMessageType::CHAT_HISTORY_DATA:
    {
        logArgsInfo("got chat history from " + companion->getName());
        logArgs("jsonString:", jsonString);

        auto json = buildJsonObject(jsonString);

        for (std::size_t i = 0; i < json["messages"].size(); i++) {
            uint8_t authorId = std::stoi(json["messages"][i]["author_id"].get<std::string>());
            authorId = (authorId == 1) ? companion->getId() : 1;
            std::string timestamp = json["messages"][i]["timestamp_tz"];
            std::string message = json["messages"][i]["message"];
            uint8_t companionId = companion->getId();

            // check if message from this companion with such timestamp
            // already exists
            auto messageGetData = getDBData(
                logDBInteraction,
                "getMessageByCompanionIdAndTimestampDBResult",
                &getMessageByCompanionIdAndTimestampDBResult,
                buildStringVector("id"),
                companionId, timestamp);

            if (!messageGetData) {
                showErrorDialogAndLogError("Error getting data from db");

                return;
            }

            if (!messageGetData->isEmpty()) {
                showInfoDialogAndLogInfo(
                    getQString(
                        std::format(
                            "Message with timestamp {0} from companion "
                            "with id {1} already exists",
                            timestamp, companionId)));

                continue;
            }

            std::string idString { "id" };

            // push message to db
            auto messageAddData = getDBData(
                logDBInteraction,
                "pushMessageToDBAndReturn",
                &pushMessageToDBWithAuthorIdAndReturn,
                buildStringVector("id", "companion_id", "timestamp_tz"),
                companion->getName(), std::to_string(authorId), timestamp,
                idString, message, true, true);

            if (!messageAddData) {
                showErrorDialogAndLogError("Error getting data from db");

                return;
            }

            if (messageAddData->isEmpty()) {
                showErrorDialogAndLogError("Error pushing chat history to db");

                return;
            }
        }

        // clear chat history widget
        clearChatHistory(companion);

        // fill container with messages
        fillCompanionMessageMapping(companion, true);

        // build chat history
        emit getMappedWidgetGroupByCompanion(companion)->buildChatHistorySignal();
    }

    break;

    case NetworkMessageType::FILE_REQUEST:
    {
        logArgs("got NetworkMessageType::FILE_REQUEST");

        auto sender = companion->getFileOperatorByNetworkId<SenderOperator>(networkId);

        if (sender)
            sender->sendFile(companion, networkId);
        else
            logTemplateError("companion has no file operator for networkId = {}", networkId);
    }

    break;

    case NetworkMessageType::FILE_DATA:
    {
        logArgs("got NetworkMessageType::FILE_DATA");

        auto receiver = companion->getFileOperatorByNetworkId<ReceiverOperator>(networkId);

        if (receiver)
            receiver->receiveFilePart(jsonData.at("data"));
        else
            logTemplateError("companion has no file operator for networkId = {}", networkId);
    }

    break;

    case NetworkMessageType::FILE_DATA_CHECK_SUCCESS:
    {
        logArgs("got NetworkMessageType::FILE_DATA_CHECK_SUCCESS");

        logTemplateInfo(
            "file {} received by companion successfully",
            companion->getFileOperatorFilePathStringByNetworkId(networkId));

        companion->removeFileOperator<SenderOperator>(networkId);
    }

    break;

    case NetworkMessageType::FILE_DATA_CHECK_FAILURE:
    {
        logArgs("got NetworkMessageType::FILE_DATA_CHECK_FAILURE");

        logTemplateInfo(
            "file {} WAS NOT received by companion",
            companion->getFileOperatorFilePathStringByNetworkId(networkId));

        companion->removeFileOperator<SenderOperator>(networkId);
    }

    break;

    case NetworkMessageType::FILE_DATA_TRANSMISSON_END:
    {
        logArgs("got NetworkMessageType::FILE_DATA_TRANSMISSON_END");

        auto receiver = companion->getFileOperatorByNetworkId<ReceiverOperator>(networkId);

        if (!receiver) {
            logTemplateError("companion has no file operator for networkId = {}", networkId);

            break;
        }

        auto resultType = (receiver->receiveFile())
            ? NetworkMessageType::FILE_DATA_CHECK_SUCCESS
            : NetworkMessageType::FILE_DATA_CHECK_FAILURE;

        if (resultType == NetworkMessageType::FILE_DATA_CHECK_SUCCESS) {
            logArgs("file received successfully");

            companion->removeFileOperator<ReceiverOperator>(networkId);
        }

        bool result = companion->sendMessage(false, resultType, networkId, nullptr);
    }

    break;

    case NetworkMessageType::FILE_DATA_TRANSMISSON_FAILURE:
    {
        logArgs("got NetworkMessageType::FILE_DATA_TRANSMISSON_FAILURE");

        logTemplateInfo(
            "file {} WAS NOT received by companion",
            companion->getFileOperatorFilePathStringByNetworkId(networkId));

        companion->removeFileOperator<SenderOperator>(networkId);
    }

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
        logDBInteraction,
        "getEarlyMessagesByMessageIdDBResult",
        &getEarlyMessagesByMessageIdDBResult,
        buildStringVector(
            "id", "companion_id", "author_id", "timestamp_tz", "message", "is_sent", "is_received"),
        companionId, messageId);

    if (!messagesData) {
        showErrorDialogAndLogError("Error getting data from db");

        return;
    }

    if (messagesData->isEmpty()) {
        logTemplateWarning(
            "no messages earlier than id = {0} in db with companion {1}", messageId, companionId);

        return;
    }

    auto widgetGroup = getMappedWidgetGroupByCompanion(companion);

    for (std::size_t i = 0; i < messagesData->size(); i++) {  // TODO switch to iterators
        logArgs("adding message with id", messagesData->getValue(i, "id"));

        auto pair = companion->createMessageAndAddToMapping(messagesData, i);
        auto result = pair.second;

        if (!result) {
            logArgsError("could not add message to messageMapping_");

            continue;
        }

        auto iter = pair.first;
        auto message = iter->first;
        auto info = iter->second;

        if (!info) {
            logArgsError("message info is nullptr");

            continue;
        }

        widgetGroup->addMessageWidgetToCentralPanelChatHistory(message, info->getState());
    }

    widgetGroup->sortChatHistoryElements();
}

void Manager::resetSelectedCompanion(std::shared_ptr<Companion> newSelected)  // TODO rewrite
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

    selectedCompanion_ = newSelected;

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

void Manager::createCompanion(std::shared_ptr<CompanionAction> companionAction)
{
    // data validation and checking
    if (!(companionDataValidation(companionAction) &&
          checkCompanionDataForExistanceAtCreation(companionAction))) {
        return;
    }

    auto name = companionAction->getName();
    auto ipAddress = companionAction->getIpAddress();
    auto clientPortStr = companionAction->getClientPort();

    // push companion data to db
    auto companionIdData = getDBData(
        logDBInteraction,
        "pushCompanionToDBAndReturn",
        &pushCompanionToDBAndReturn,
        buildStringVector("id"),
        name);

    if (!companionIdData) {
        showErrorDialogAndLogError("Error getting data from db");

        return;
    }

    if (companionIdData->isEmpty()) {
        showErrorDialogAndLogError("Empty db reply to new companion pushing");

        return;
    }

    int id = std::stoi(companionIdData->getValue(0, "id"));

    // push socket data to db
    uint16_t serverPort = 5000 + id + 1;  // TODO change

    auto socketData = getDBData(
        logDBInteraction,
        "pushSocketToDB",
        &pushSocketToDBAndReturn,
        buildStringVector("id"),
        name, ipAddress, std::to_string(serverPort), clientPortStr);

    if (!socketData) {
        showErrorDialogAndLogError("Error getting data from db");

        return;
    }

    if (socketData->isEmpty()) {
        showErrorDialogAndLogError("Empty db reply to new socket pushing");

        return;
    }

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
    getGraphicManager()->showCompanionInfoDialog(companionAction, "New companion added:\n\n");
}

void Manager::updateCompanion(std::shared_ptr<CompanionAction> companionAction)
{
    // data validation and checking
    if (!(companionDataValidation(companionAction) &&
          checkCompanionDataForExistanceAtUpdate(companionAction))) {
        return;
    }

    // update companion data at db
    auto companionIdData = getDBData(
        logDBInteraction,
        "updateCompanionAndSocketAndReturn",
        &updateCompanionAndSocketAndReturn,
        buildStringVector("id"),
        *companionAction);

    if (!companionIdData) {
        showErrorDialogAndLogError("Error getting data from db");

        return;
    }

    if (companionIdData->isEmpty()) {
        showErrorDialogAndLogError("Empty db reply to companion update");

        return;
    }

    // update Companion and SocketInfo object
    companionAction->updateCompanionObjectData();

    // update SocketInfoWidget
    auto widgetGroup = getMappedWidgetGroupByCompanion(companionAction->getCompanion());
    widgetGroup->getSocketInfoBase()->update();

    // show info dialog
    getGraphicManager()->showCompanionInfoDialog(companionAction, "Companion updated:\n\n");
}

void Manager::deleteCompanion(std::shared_ptr<CompanionAction> companionAction)
{
    // delete companion chat messages from db
    auto companionIdMessagesData = getDBData(
        logDBInteraction,
        "deleteMessagesFromDBAndReturn",
        &deleteMessagesFromDBAndReturn,
        buildStringVector("id"),
        *companionAction);

    if (!companionIdMessagesData) {
        showErrorDialogAndLogError("Error getting data from db");

        return;
    }

    if (companionIdMessagesData->isEmpty()) {
        // no return, may be companion without messages
        // showWarningDialogAndLogWarning("Empty db reply to companion messages deletion");
    }

    // delete companion and socket from db
    auto companionIdCompanionData = getDBData(
        logDBInteraction,
        "deleteCompanionAndSocketAndReturn",
        &deleteCompanionAndSocketAndReturn,
        buildStringVector("id"),
        *companionAction);

    if (!companionIdCompanionData) {
        showErrorDialogAndLogError("Error getting data from db");

        return;
    }

    if (companionIdCompanionData->isEmpty()) {
        showErrorDialogAndLogError("Empty db reply to companion deletion");

        return;
    }

    // delete companion object
    deleteCompanionObject(companionAction->getCompanion());

    // show info dialog
    getGraphicManager()->showCompanionInfoDialog(companionAction, "Companion deleted:\n\n");
}

void Manager::clearChatHistory(std::shared_ptr<Companion> companion)
{
    auto widgetGroup = getMappedWidgetGroupByCompanion(companion);
    getGraphicManager()->clearChatHistory(widgetGroup);
}

void Manager::clearCompanionHistory(std::shared_ptr<CompanionAction> companionAction)
{
    // delete companion chat messages from db
    auto companionIdMessagesData = getDBData(
        logDBInteraction,
        "deleteMessagesFromDBAndReturn",
        &deleteMessagesFromDBAndReturn,
        buildStringVector("companion_id"),
        *companionAction);

    if (!companionIdMessagesData) {
        showErrorDialogAndLogError("Error getting data from db");

        return;
    }

    if (companionIdMessagesData->isEmpty()) {
        // no return, may be companion without messages
        // showWarningDialogAndLogWarning("Empty db reply to companion messages deletion");
    }

    // clear companion's message mapping
    companionAction->getCompanion()->clearMessageMapping();

    // clear chat history widget
    clearChatHistory(companionAction->getCompanion());

    // show info dialog
    getGraphicManager()->showCompanionInfoDialog(
        companionAction, "Companion chat history cleared:\n\n");
}

void Manager::createUserPassword(std::shared_ptr<PasswordAction> action)
{
    // data validation and checking
    if (!(passwordDataValidation(action)))
        return;

    // push password data to db
    auto passwordIdData = getDBData(
        logDBInteraction,
        "pushPasswordToDBAndReturn",
        &pushPasswordToDBAndReturn,
        buildStringVector("id"),
        action->getPassword());

    if (!passwordIdData) {
        showErrorDialogAndLogError("Error getting data from db");

        return;
    }

    if (passwordIdData->isEmpty()) {
        showErrorDialogAndLogError("Empty db reply to new password pushing");

        return;
    }

    // show dialog
    showInfoDialogAndLogInfo(
        newPasswordCreatedLabel, &TextDialog::unsetMainWindowBlurAndCloseDialogs,
        action->getDialog());
}

void Manager::authenticateUser(std::shared_ptr<PasswordAction> action)
{
    auto graphicManager = getGraphicManager();

    // do we have password in db?
    auto passwordData = getDBData(
        logDBInteraction,
        "getPasswordDBResult",
        &getPasswordDBResult,
        buildStringVector("id", "password"));

    if (!passwordData) {
        showErrorDialogAndLogError("Error getting password from db");

        return;
    }

    if (passwordData->isEmpty()) {
        showErrorDialogAndLogError("Db password data is empty");

        return;
    }
    else {
        if (passwordData->getValue(0, "password") == action->getPassword()) {
            userIsAuthenticated_ = true;

            logArgsInfo("user successfully authenticated");
            graphicManager->disableMainWindowBlurEffect();
        }
        else {
            showErrorDialogAndLogError("Password is not correct");

            return;
        }
    }
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
    auto passwordData = getDBData(
        logDBInteraction,
        "getPasswordDBResult",
        &getPasswordDBResult,
        buildStringVector("id", "password"));

    if (!passwordData) {
        showErrorDialogAndLogError("Error getting password from db");

        return;
    }

    if (passwordData->isEmpty())
        graphicManager->createEntrancePassword();
    else
        graphicManager->getEntrancePassword();
}

void Manager::sendUnsentMessages(std::shared_ptr<Companion> companion)
{
    // get unsent messages from db
    auto messagesData = getDBData(
        logDBInteraction,
        "getUnsentMessagesByCompanionNameDBResult",
        &getUnsentMessagesByCompanionNameDBResult,
        buildStringVector(
            "id", "author_id", "companion_id", "timestamp_tz",
            "message", "is_received"),
        companion->getName());

    if (!messagesData) {
        showErrorDialogAndLogError("Error getting data from db");

        return;
    }

    if (messagesData->isEmpty()) {
        logArgsInfo("Empty db reply to unsent messages selection");

        return;
    }

    for (std::size_t i = 0; i < messagesData->size(); i++) {  // TODO switch to iterators
        uint32_t messageId = std::stoi(messagesData->getValue(i, "id"));
        auto message = companion->findMessage(messageId);
        std::string networkId;

        if (message) {
            auto messageState = companion->getMappedMessageStateByMessage(message);

            if (!messageState) {
                logArgsError(
                    "strange case: unsent message found in companions messages, "
                    "but not found in companion's messageMapping_");
            }
            else {
                networkId = messageState->getNetworkId();
            }
        }
        else {
            // add to companion's messages if needed
            networkId = getRandomString(5);

            companion->createMessageAndAddToMapping(
                MessageType::TEXT,
                messageId,
                1,
                messagesData->getValue(i, "timestamp_tz"),
                messagesData->getValue(i, "message"),
                false,
                false,
                getBoolFromDBValue(messagesData->getValue(i, "is_received")),
                networkId);
        }

        // send over network
        bool result = companion->sendMessage(true, NetworkMessageType::TEXT, networkId, message);

        // mark message as sent
        if (result)
            markMessageAsSent(companion, message);
    }
}

void Manager::requestHistoryFromCompanion(std::shared_ptr<Companion> companion)
{
    bool result = companion->sendMessage(
        true, NetworkMessageType::CHAT_HISTORY_REQUEST, "", nullptr);
}

void Manager::sendChatHistoryToCompanion(std::shared_ptr<Companion> companion)
{
    logArgs("Manager::sendChatHistoryToCompanion");

    std::vector<std::string> keys =
        buildStringVector("author_id", "timestamp_tz", "message");

    // get messages from db
    auto messagesData = getDBData(
        logDBInteraction,
        "getAllMessagesByCompanionIdDBResult",
        &getAllMessagesByCompanionIdDBResult,
        keys,
        companion->getId());

    if (!messagesData) {
        showErrorDialogAndLogError("Error getting data from db");

        return;
    }

    if (messagesData->isEmpty()) {
        logArgsInfo("Empty db reply to messages selection");

        return;
    }

    bool result = companion->sendChatHistory(messagesData, keys);
}

bool Manager::isInitialised()
{
    return initialized_;
}

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
    auto findWidget = [&](auto &pair)
    {
        return pair.second.second == group;
    };

    auto result = std::find_if (
        mapCompanionIdToCompanionInfo_.cbegin(),
        mapCompanionIdToCompanionInfo_.cend(),
        findWidget);

    return result->second.first;
}

void Manager::fillCompanionMessageMapping(
    std::shared_ptr<Companion> companion, bool containersAlreadyHaveMessages)
{
    uint8_t companionId = companion->getId();

    // get messages data
    auto messagesData = getDBData(
        logDBInteraction,
        "getMessagesDBResult",
        &getMessagesDBResult,
        buildStringVector(
            "id", "companion_id", "author_id",
            "timestamp_tz", "message", "is_sent", "is_received"),
        companionId);

    if (!messagesData) {
        showErrorDialogAndLogError("Error getting data from db");

        // return false;
        return;
    }

    if (messagesData->isEmpty()) {
        logTemplateWarning("no messages in db with companion {}", companion->getName());

        // return false;
        return;
    }

    for (std::size_t i = 0; i < messagesData->size(); i++) {  // TODO switch to iterators
        auto messageId = std::stoi(messagesData->getValue(i, "id"));

        if (containersAlreadyHaveMessages) {
            auto pair = companion->getMessageMappingPairByMessageId(messageId);

            auto info = pair.second;

            if (info && info->getState()) {
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

bool Manager::connectToDb()
{
    bool connected = false;
    dbConnection_ = getDBConnection();

    if (!dbConnection_)
        return connected;

    ConnStatusType status = PQstatus(dbConnection_.get());
    // logArgs("DB connection status: ", status);

    if (status == ConnStatusType::CONNECTION_OK)  // TODO raise exception
        connected = true;

    return connected;
}

bool Manager::buildCompanions()
{
    bool companionsDataIsOk = true;

    // get companion data
    auto companionsData = getDBData(
        logDBInteraction,
        "getCompanionsDBResult",
        &getCompanionsDBResult,
        buildStringVector("id", "name"));

    if (!companionsData)
        return false;

    // std::sort(
    //     companionsData->getData()->begin(),
    //     companionsData->getData()->end(),
    //     [&](auto &iterator1, auto &iterator2)
    //     {
    //         return iterator1.at("id") < iterator2.at("id");
    //     }
    // );

    for (std::size_t index = 0; index < companionsData->size(); index++) {  // TODO switch to iterators
        int id = std::stoi(companionsData->getValue(index, "id"));

        // create companion object
        auto companion = addCompanionObject(id, companionsData->getValue(index, "name"));

        if (!companion) {
            logArgsError("companion is nullptr");

            continue;
        }

        // get socket data object
        auto socketsData = getDBData(
            logDBInteraction,
            "getSocketInfoDBResult",
            &getSocketInfoDBResult,
            buildStringVector("ipaddress", "server_port", "client_port"),
            id);

        if (socketsData->size() > 0) {
            // TODO use port number pool
            auto socketInfo = std::make_shared<SocketInfo>(
                socketsData->getValue(0, "ipaddress"),
                std::stoi(socketsData->getValue(0, "server_port")),
                std::stoi(socketsData->getValue(0, "client_port")));

            companion->setSocketInfo(socketInfo);
        }

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

    auto companionsNumber = mapCompanionIdToCompanionInfo_.size();
    auto childrenSize = graphicManager->getCompanionPanelChildrenSize();

    logArgsWithTemplate("companionsNumber: {0}, childrenSize: {1}", companionsNumber, childrenSize);

    if (companionsNumber == 0 && childrenSize == 0) {
        logArgsWarning("strange case, empty sockets panel");
    }
    else {
        // TODO check if sockets already are children

        // hide companion panel stub widget
        graphicManager->hideCompanionPanelStub();

        for (auto &pair : mapCompanionIdToCompanionInfo_)
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
    auto result = mapCompanionIdToCompanionInfo_.emplace(id, value);

    return (result.second) ? result.first->second.first : nullptr;
}

void Manager::createWidgetGroupAndAddToMapping(std::shared_ptr<Companion> companion)
{
    auto widgetGroup = std::make_shared<WidgetGroup>(companion);
    widgetGroup->set();
    mapCompanionIdToCompanionInfo_[companion->getId()].second = widgetGroup;
    companion->addMessageWidgetsToChatHistory();
}

void Manager::deleteCompanionObject(std::shared_ptr<Companion> companion)
{
    deleteWidgetGroupAndDeleteFromMapping(companion);
}

void Manager::deleteWidgetGroupAndDeleteFromMapping(std::shared_ptr<Companion> companion)
{
    auto findMapLambda = [&](auto &iterator)
    {
        return iterator.second.first == companion;
    };

    // TODO use range
    auto findMapResult = std::find_if (
        mapCompanionIdToCompanionInfo_.begin(),
        mapCompanionIdToCompanionInfo_.end(),
        findMapLambda);

    if (findMapResult == mapCompanionIdToCompanionInfo_.end()) {
        showErrorDialogAndLogError("Companion was not found in mapping at deletion");
    }
    else {
        if (selectedCompanion_ == companion)
            selectedCompanion_ = nullptr;

        mapCompanionIdToCompanionInfo_.erase(findMapResult);
    }
}

bool Manager::companionDataValidation(std::shared_ptr<CompanionAction> companionAction)
{
    std::vector<std::string> validationErrors {};

    bool validationResult = validateCompanionData(validationErrors, companionAction);

    if (!validationResult) {
        showErrorDialogAndLogError(
            getQString(buildDialogText("Error messages:\n\n", validationErrors)));

        return false;
    }

    return true;
}

bool Manager::passwordDataValidation(std::shared_ptr<PasswordAction> passwordAction)
{
    std::vector<std::string> validationErrors {};

    bool validationResult = validatePassword(validationErrors, passwordAction->getPassword());

    if (!validationResult) {
        showErrorDialogAndLogError(
            getQString(buildDialogText("Error messages:\n\n", validationErrors)));

        return false;
    }

    return true;
}

bool Manager::checkCompanionDataForExistanceAtCreation(
    std::shared_ptr<CompanionAction> companionAction)
{
    // check if companion with such name already exists
    auto companionIdData = getDBData(
        logDBInteraction,
        "getCompanionByNameDBResult",
        &getCompanionByNameDBResult,
        buildStringVector("id"),
        companionAction->getName());

    if (!companionIdData) {
        showErrorDialogAndLogError("Error getting data from db");

        return false;
    }

    if (!companionIdData->isEmpty()) {
        showErrorDialogAndLogError("Companion with such name already exists");

        return false;
    }

    // check if such socket already exists
    auto socketIdData = getDBData(
        logDBInteraction,
        "getSocketByIpAddressAndPortDBResult",
        &getSocketByIpAddressAndPortDBResult,
        buildStringVector("id"),
        companionAction->getIpAddress(),
        companionAction->getClientPort());

    if (!socketIdData) {
        showErrorDialogAndLogError("Error getting data from db");

        return false;
    }

    if (!socketIdData->isEmpty()) {
        showErrorDialogAndLogError("Companion with such socket already exists");

        return false;
    }

    return true;
}

bool Manager::checkCompanionDataForExistanceAtUpdate(
    std::shared_ptr<CompanionAction> companionAction)
{
    // check if companion with such name already exists
    auto companionIdData = getDBData(
        logDBInteraction,
        "getCompanionByNameDBResult",
        &getCompanionByNameDBResult,
        buildStringVector("id"),
        companionAction->getName());

    if (!companionIdData) {
        showErrorDialogAndLogError("Error getting data from db");

        return false;
    }

    bool findNameResult = companionIdData->findValue(
        "id"s, std::to_string(companionAction->getCompanionId()));

    bool nameExistsAtOtherCompanion =
        (findNameResult && companionIdData->size() > 1) ||
        (!findNameResult && companionIdData->size() > 0);

    if (nameExistsAtOtherCompanion)
        // no return
        showWarningDialogAndLogWarning("Companion with such name already exists");

    // check if such socket already exists
    auto socketIdData = getDBData(
        logDBInteraction,
        "getSocketByIpAddressAndPortDBResult",
        &getSocketByIpAddressAndPortDBResult,
        buildStringVector("id"),
        companionAction->getIpAddress(),
        companionAction->getClientPort());

    if (!socketIdData) {
        showErrorDialogAndLogError("Error getting data from db");

        return false;
    }

    bool findSocketResult = socketIdData->findValue(
        "id"s, std::to_string(companionAction->getCompanionId()));

    bool socketExistsAtOtherCompanion =
        (findSocketResult && socketIdData->size() > 1) ||
        (!findSocketResult && socketIdData->size() > 0);

    if (socketExistsAtOtherCompanion) {
        showErrorDialogAndLogError("Companion with such socket already exists");

        return false;
    }

    return true;
}

void Manager::waitForMessageReceptionConfirmation(
    std::shared_ptr<Companion> companion, std::shared_ptr<MessageState> messageState,
    std::shared_ptr<Message> message)
{
    auto lambda = [=]()
    {
        uint32_t sleepDuration = sleepDurationInitial;

        sleepForMilliseconds(sleepDuration);

        while (true) {
            if (messageState->isReceived()) {
                return;
            }
            else {
                // send message reception confirmation request
                bool result = companion->sendMessage(
                    false, NetworkMessageType::RECEIVE_CONFIRMATION_REQUEST,
                    messageState->getNetworkId(), message);

                // sleep
                sleepForMilliseconds(sleepDuration);
                sleepDuration *= sleepDurationIncreaseRate;
            }
        }
    };

    std::thread(lambda).detach();
}

bool Manager::markMessageAsSent(
    std::shared_ptr<Companion> companion, std::shared_ptr<Message> message)
{
    // mark in db
    auto messageIdData = getDBData(
        logDBInteraction,
        "setMessageInDbAndReturn",
        &setMessageIsSentInDbAndReturn,
        buildStringVector("id"),
        message->getId());

    if (!messageIdData) {
        showErrorDialogAndLogError("Error updating data in db");

        return false;
    }

    // mark in widget
    getGraphicManager()->markMessageWidgetAsSent(companion, message);

    return true;
}

bool Manager::markMessageAsReceived(
    std::shared_ptr<Companion> companion, std::shared_ptr<Message> message)
{
    // mark in widget
    getGraphicManager()->markMessageWidgetAsReceived(companion, message);

    // mark in db
    auto messageIdData = getDBData(
        logDBInteraction,
        "setMessageIsReceivedInDbAndReturn",
        &setMessageIsReceivedInDbAndReturn,
        buildStringVector("id"),
        message->getId());

    if (!messageIdData) {
        showErrorDialogAndLogError("Error getting data from db");

        return false;
    }

    if (messageIdData->isEmpty()) {
        showErrorDialogAndLogError("Error setting message is_received in Db");

        return false;
    }

    return true;
}

std::tuple<uint32_t, uint8_t, std::string> Manager::pushMessageToDB(
    const std::string &companionName, const std::string &authorName, const std::string &timestamp,
    const std::string &text, const bool &isSent, const bool &isReceived)
{
    const std::string companionIdString("companion_id");

    auto messageData = getDBData(
        logDBInteraction,
        "pushMessageToDBAndReturn",
        &pushMessageToDBAndReturn,
        buildStringVector("id", "companion_id", "timestamp_tz"),
        companionName, authorName, timestamp, companionIdString, text,
        isSent, isReceived);

    if (!messageData) {
        showErrorDialogAndLogError("Error getting data from db");

        return std::tuple<uint32_t, uint8_t, std::string>(0, 0, "");
    }

    if (messageData->isEmpty()) {
        logArgsError("messageData->isEmpty()");

        return std::tuple<uint32_t, uint8_t, std::string>(0, 0, "");
    }

    uint32_t id = std::stoi(messageData->getValue(0, "id"));
    uint8_t companionId = std::stoi(messageData->getValue(0, "companion_id"));
    std::string timestampTz { messageData->getValue(0, "timestamp_tz") };

    if (logDBInteraction)
        logArgsWithTemplate("companionId: {0}, timestampTz: {1}", companionId, timestampTz);

    return std::tuple<uint32_t, uint8_t, std::string>(id, companionId, timestampTz);
}

std::shared_ptr<Manager> getManager()
{
    QCoreApplication *coreApp = QCoreApplication::instance();
    ChatApp *app = dynamic_cast<ChatApp *>(coreApp);

    if (!app)
        return nullptr;

    return app->manager_;
}
