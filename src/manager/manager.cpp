#include "manager.hpp"

#include "action.hpp"
#include "action_result.hpp"
#include "application.hpp"
#include "companion.hpp"
#include "data.hpp"
#include "data_checker.hpp"
#include "message.hpp"
#include "utils.hpp"
#include "validator.hpp"
#include "widgets.hpp"

Manager::Manager(GraphicManagerPtr graphicManager)
    : graphicManager_(graphicManager), dbOperator_(), messageHandler_(), fileManager_(),
    networkManager_(new NetworkManager), messageStateToMessageMapMutex_(),
    /*userIsAuthenticated_(false), */currentAction_(nullptr) {}

Manager::~Manager() {}

// bool Manager::userIsAuthenticated()
// {
//     return userIsAuthenticated_;
// }

void Manager::set()
{
    // if (!dbRequester_.isReady())
    //     exitUtil(EXIT_FAILURE);

    messageHandler_.setManager(shared_from_this());
    fileManager_.setManager(shared_from_this());
    networkManager_->setManager(shared_from_this());

    bool companionsBuilt = buildCompanions();
    logTemplateInfo("companionsBuilt: {}", companionsBuilt);

    if (companionsBuilt)  // TODO rewrite
        graphicManager_->buildWidgetGroups();
    else
        logArgsError("companion initialization error");
}

void Manager::addEarlyMessages(CompanionPtr companion)
{
    messageHandler_.addEarlyMessages(companion);
}

bool Manager::pushMessageHistoryToDb(CompanionPtr companion, const nlohmann::json &data)
{
    auto messages = data["messages"];

    for (const auto &message : data["messages"]) {
        uint8_t authorId = getIntFromString(ID_BAD_VALUE, message["author_id"].get<std::string>());
        uint8_t companionId = companion->id();
        authorId = (authorId == 1) ? companionId : 1;
        std::string timestamp = message["timestamp_tz"];
        std::string text = message["message"];

        // check if message from this companion with such timestamp already exists
        auto messageGetData = getDBData(
            DBRequestType::GET_MESSAGE_BY_COMPANION_ID_AND_TIMESTAMP, companionId, timestamp);

        if (!messageGetData)
            return false;

        if (!messageGetData->isEmpty()) {
            // auto entryTemplate =
            //     "Message with timestamp {0} from companion with id {1} already exists";

            // showInfoDialogAndLogInfo(getStringByFormat(entryTemplate, timestamp, companionId));

            continue;
        }

        std::string idString { "id" };

        // push message to db
        auto messageAddData = getDBData(
            DBRequestType::PUSH_MESSAGE_AND_RETURN, companion->name(),
            getString(authorId), timestamp, idString, text, true, true);

        if (!messageAddData || messageAddData->isEmpty())
            return false;
    }

    return true;
}

void Manager::receiveMessage(int companionId, const std::string &json)
{
    auto companion = graphicManager_->getCompanion(companionId);

    if (!companion)
        return;

    messageHandler_.receiveMessage(companion, json);
}

void Manager::initCompanionCreation()
{
    initAction<CompanionAction>(ActionType::CREATE_COMPANION, nullptr);
}

void Manager::initCompanionUpdate(CompanionPtr companion)
{
    initAction<CompanionAction>(ActionType::UPDATE_COMPANION, companion);
}

void Manager::initCompanionDeletion(CompanionPtr companion)
{
    initAction<CompanionAction>(ActionType::DELETE_COMPANION, companion);
}

void Manager::initCompanionHistoryClearing(CompanionPtr companion)
{
    initAction<CompanionAction>(ActionType::CLEAR_HISTORY, companion);
}

void Manager::initEntrancePasswordCreation()
{
    initAction<PasswordAction>(ActionType::CREATE_PASSWORD);
}

void Manager::initEntrancePasswordReception()
{
    initAction<PasswordAction>(ActionType::GET_PASSWORD);
}

void Manager::initFileSend(CompanionPtr companion)
{
    initAction<FileAction>(ActionType::SEND_FILE, ""s, companion);
}

void Manager::initFileReception(const std::string &networkId, CompanionPtr companion)
{
    initAction<FileAction>(ActionType::SAVE_FILE, networkId, companion);
}

ActionResultPtr Manager::getActionResultByKeyDBData(
    DBReplyDataPtr data, const std::string &key, bool allowEmptyResult)
{
    // TODO define db schema as code and switch from string field names to enum values
    if (!data)
        return std::make_shared<ActionResult>(false, DB_REPLY_NULL);

    if (data->isEmpty()) {
        if (allowEmptyResult)
            return std::make_shared<ActionResult>(true, ""s);
        else
            return std::make_shared<ActionResult>(false, DB_REPLY_EMPTY);
    }

    auto result = getIdFromDbData(data, 0, key);

    if (result == ID_BAD_VALUE)
        return std::make_shared<ActionResult>(false, VALUE_BUILDING_ERROR);
    else
        return std::make_shared<ActionValueResult<int>>(result, true, ""s);
}

ActionResultPtr Manager::pushCompanionToDbAndReturnId(CompanionActionPtr action)
{
    auto data = getDBData(DBRequestType::PUSH_COMPANION_AND_RETURN, action->name());

    return getActionResultByKeyDBData(data, "id");
}

uint16_t Manager::getServerPortByCompanionId(int id)
{
    uint16_t serverPort = 5000 + id + 1;  // TODO change

    return serverPort;
}

ActionResultPtr Manager::pushSocketToDb(CompanionActionPtr action, int serverPort)
{
    return getActionResult(
        DBRequestType::PUSH_SOCKET_AND_RETURN, action->name(), action->ipAddress(),
        serverPort, action->clientPort());
}

ActionResultPtr Manager::createCompanion(CompanionActionPtr action)
{
    // data validation
    auto validationResult = validateActionData<CompanionAction>(action);

    if (!validationResult->status())
        return validationResult;

    // data checking
    auto checkResult = checkDataForExistance(shared_from_this(), action);

    if (!checkResult->status())
        return checkResult;

    // push companion data to db
    auto idResult = pushCompanionToDbAndReturnId(action);

    if (!idResult->status())
        return idResult;

    // push socket data to db
    auto cast = std::dynamic_pointer_cast<ActionValueResult<int>>(idResult);

    if (!cast)
        return std::make_shared<ActionResult>(false, POINTER_CASTING_ERROR);

    auto id = cast->value();
    auto serverPort = getServerPortByCompanionId(id);
    auto pushSocketResult = pushSocketToDb(action, serverPort);

    if (!pushSocketResult->status())
        return pushSocketResult;

    // create companion object
    auto managerMapResult = addCompanionAndRetutnResult(id, action->name());

    if (!managerMapResult->status())
        return managerMapResult;

    // update data
    auto data = action->getCompanionData();
    data->setServerPort(serverPort);

    // update network manager
    auto networkResult = addCompanionToNetworkManagerMap(id, data);

    if (!networkResult->status())
        return networkResult;

    // add companion and widget group to mapping
    auto companion = managerMapResult->value();
    graphicManager_->updateMapping(companion);

    return std::make_shared<ActionResult>(true, ""s);
}

ActionResultPtr Manager::updateCompanionInDbAndReturnId(CompanionActionPtr action)
{
    auto data = getDBData(
        DBRequestType::UPDATE_COMPANION_AND_SOCKET_AND_RETURN, action->name(),
        action->companionId(), action->ipAddress(), action->clientPort());

    return getActionResultByKeyDBData(data, "id");
}

ActionResultPtr Manager::updateCompanion(CompanionActionPtr action)
{
    // TODO check new data before data dialog is closed, move to Action::act
    // check if data was modified
    auto companion = action->getCompanion();
    auto oldData = getCompanionData(companion);
    auto newData = action->getCompanionData();

    if (compareCompanionData(oldData, newData))
        return std::make_shared<ActionResult>(false, "companion data was not modified"s);

    // data validation
    auto validationResult = validateActionData<CompanionAction>(action);

    if (!validationResult->status())
        return validationResult;

    // data checking
    auto checkResult = checkDataForExistance(shared_from_this(), action);

    if (!checkResult->status())
        return checkResult;

    // update companion data at db
    auto idResult = updateCompanionInDbAndReturnId(action);

    if (!idResult->status())
        return idResult;

    // update Companion and SocketInfo object
    // action->updateCompanionObjectData();
    companion->setName(newData->name());
    networkManager_->updateSocketInfo(companion->id(), newData);

    // update SocketInfoWidget
    graphicManager_->updateCompanionWidget(companion);

    return std::make_shared<ActionResult>(true, ""s);
}

ActionResultPtr Manager::deleteCompanionMessagesFromDbAndReturnId(CompanionActionPtr action)
{
    auto data = getDBData(DBRequestType::DELETE_MESSAGES_AND_RETURN, action->companionId());

    return getActionResultByKeyDBData(data, "companion_id", true);
}

ActionResultPtr Manager::deleteCompanionAndSocketFromDbAndReturnId(CompanionActionPtr action)
{
    auto data = getDBData(
        DBRequestType::DELETE_COMPANION_AND_SOCKET_AND_RETURN, action->companionId());

    return getActionResultByKeyDBData(data, "id");
}

ActionResultPtr Manager::deleteCompanion(CompanionActionPtr action)
{
    // delete companion chat messages from db
    auto deleteMessagesResult = deleteCompanionMessagesFromDbAndReturnId(action);

    if (!deleteMessagesResult->status())
        return deleteMessagesResult;

    // delete companion and socket from db
    auto deleteCompanionAndSocketResult = deleteCompanionAndSocketFromDbAndReturnId(action);

    if (!deleteCompanionAndSocketResult->status())
        return deleteCompanionAndSocketResult;

    // delete companion object
    deleteWidgetGroupAndDeleteFromMapping(action->getCompanion());

    return std::make_shared<ActionResult>(true, ""s);
}

void Manager::clearChatHistory(CompanionPtr companion)
{
    graphicManager_->clearHistory(companion);
}

ActionResultPtr Manager::clearCompanionHistory(CompanionActionPtr action)
{
    // delete companion chat messages from db
    auto deleteMessagesResult = deleteCompanionMessagesFromDbAndReturnId(action);

    if (!deleteMessagesResult->status())
        return deleteMessagesResult;

    // clear companion's message mapping
    messageHandler_.clearMessageMapping(action->getCompanion());

    // clear chat history widget
    clearChatHistory(action->getCompanion());

    return std::make_shared<ActionResult>(true, ""s);
}

ActionResultPtr Manager::pushPasswordToDbAndReturnId(PasswordActionPtr action)
{
    auto data = getDBData(DBRequestType::PUSH_PASSWORD_AND_RETURN, action->password());

    return getActionResultByKeyDBData(data, "id");
}

ActionResultPtr Manager::createUserPassword(PasswordActionPtr action)
{
    // data validation
    auto validationResult = validateActionData<PasswordAction>(action);

    if (!validationResult->status())
        return validationResult;

    // push password data to db
    auto idResult = pushPasswordToDbAndReturnId(action);

    if (!idResult->status())
        return idResult;

    return std::make_shared<ActionResult>(true, ""s);
}

ActionResultPtr Manager::authenticateUser(PasswordActionPtr action)
{
    // data checking
    auto checkResult = checkDataForExistance(shared_from_this(), action);

    if (!checkResult->status())
        return checkResult;

    // userIsAuthenticated_ = true;

    return std::make_shared<ActionResult>(true, ""s);
}

void Manager::startUserAuthentication()
{
    graphicManager_->enableMainWindowBlurEffect();

    // do we have password in db?
    auto passwordData = getDBData(DBRequestType::GET_PASSWORD);

    if (!passwordData)
        return;

    if (passwordData->isEmpty())
        initEntrancePasswordCreation();
    else
        initEntrancePasswordReception();
}

void Manager::requestHistoryFromCompanion(CompanionPtr companion)
{
    messageHandler_.requestHistoryFromCompanion(companion);
}

void Manager::sendHistoryToCompanion(CompanionPtr companion)
{
    logArgsInfo(__FUNCTION__);

    auto keys = buildStringVector("author_id", "timestamp_tz", "message");

    // get messages from db
    auto data = getDBData(DBRequestType::GET_ALL_MESSAGES_BY_COMPANION_ID, companion->id());

    if (!data || data->isEmpty())
        return;

    bool result = messageHandler_.sendChatHistory(companion, data, keys);
}

std::filesystem::path Manager::getLastOpenedPath()
{
    return fileManager_.lastOpenedPath();
}

void Manager::setLastOpenedPath(const std::filesystem::path &path)
{
    fileManager_.setLastOpenedPath(path);
}

void Manager::endAction(ActionPtr action)
{
    checkAndResetCurrentAction(action);
}

void Manager::sendMessage(
    MessageType type, CompanionPtr companion, ActionPtr action, const std::string &text)
{
    messageHandler_.sendMessage(type, companion, action, text);
}

bool Manager::sendFileRequestMessage(CompanionPtr companion, const std::string &networkId)
{
    return messageHandler_.sendFileRequest(companion, networkId);
}

bool Manager::sendFileDataTransmissionResultMessage(
    CompanionPtr companion, NetworkMessageType type, const std::string &networkId)
{
    return messageHandler_.sendFileRequest(companion, networkId);
}

bool Manager::buildCompanions()
{
    bool result = true;

    // get companion data
    auto companionsData = getDBData(DBRequestType::GET_COMPANIONS);

    if (!companionsData)
        return false;

    // companionsData->sortDataById();

    for (std::size_t index = 0; index < companionsData->size(); ++index) {  // TODO switch to iterators
        int id = getIdFromDbData(companionsData, index, "id");

        // create companion object
        auto companion = addCompanion(id, companionsData->getValue(index, "name"));

        if (!companion)
            continue;

        // get socket data object
        auto socketsData = getDBData(DBRequestType::GET_SOCKET_INFO, id);

        if (!socketsData || socketsData->isEmpty())
            return false;

        // add to network manager
        // TODO use port number pool
        auto ipAddress = socketsData->getValue(0, "ipaddress");
        auto clientPort = getPortFromDbData(socketsData, 0, "client_port");
        auto clientSocket = std::make_shared<SocketInfo>(ipAddress, clientPort);

        auto serverSocket = std::make_shared<SocketInfo>(
            ""s, getPortFromDbData(socketsData, 0, "server_port"));

        auto entityResult = networkManager_->addToMap(id, clientSocket, serverSocket);

        if (!entityResult.second)
            continue;

        if (companion->id() <= 1)  // TODO change condition
            continue;

        // add to message storage
        if (!messageHandler_.addCompanionToMessageStorage(companion))
            continue;

        bool started = (*entityResult.first).second->start();
    }

    return result;
}

CompanionPtr Manager::addCompanion(int id, const std::string &name)
{
    auto s = ""s;

    return addCompanion(id, name, s);
}

CompanionPtr Manager::addCompanion(int id, const std::string &name, std::string &entry)
{
    if (id == 0) {
        entry = "companion id == 0"s;

        logArgsError(entry);  // TODO move logging to action

        return nullptr;
    }

    auto companion = std::make_shared<Companion>(id, name);
    bool result = graphicManager_->addCompanion(companion);

    if (!result) {
        entry = "companion is nullptr"s;

        logArgsError(entry);  // TODO move logging to action

        return nullptr;
    }

    return companion;
}

std::shared_ptr<CompanionResult> Manager::addCompanionAndRetutnResult(
    int id, const std::string &name)
{
    std::string entry { "" };

    auto companion = addCompanion(id, name, entry);

    if (companion)
        return std::make_shared<CompanionResult>(companion, true, ""s);
    else
        return std::make_shared<CompanionResult>(nullptr, false, entry);
}

ActionResultPtr Manager::addCompanionToNetworkManagerMap(int id, CompanionDataPtr data)
{
    // update network manager
    auto clientSocket = std::make_shared<SocketInfo>(
        data->ipAddress(), getIntFromString(PORT_BAD_VALUE, data->clientPort()));

    auto serverSocket = std::make_shared<SocketInfo>(
        ""s, getIntFromString(PORT_BAD_VALUE, data->serverPort()));

    auto networkResult = networkManager_->addToMap(id, clientSocket, serverSocket);

    if (!networkResult.second) {
        auto entry = getStringByFormat(
            "adding to network manager map error, companion id '{}'", id);

        return std::make_shared<ActionResult>(false, entry);
    }

    return std::make_shared<ActionResult>(true, ""s);
}

void Manager::deleteWidgetGroupAndDeleteFromMapping(CompanionPtr companion)
{
    graphicManager_->deleteWidgetGroup(companion);
}

void Manager::sendUnsentMessages(CompanionPtr companion)
{
    messageHandler_.sendUnsentMessages(companion);
}

bool Manager::sendFileBlock(
    CompanionPtr companion, const std::string &networkId, const std::string &data)
{
    return messageHandler_.sendFileBlock(companion, networkId, data);
}

CompanionDataPtr Manager::getCompanionData(CompanionPtr companion)
{
    auto data = networkManager_->getCompanionData(companion->id());
    data->setName(companion->name());

    return data;
}

bool Manager::connectClient(CompanionPtr companion)
{
    return networkManager_->connectClient(companion->id());
}

bool Manager::disconnectClient(CompanionPtr companion)
{
    return networkManager_->disconnectClient(companion->id());
}

void Manager::addSenderOperator(const std::string &networkId, ActionPtr action)
{
    fileManager_.addSenderOperator(networkId, action->getPath());
}

void Manager::addReceiverOperator(MessagePtr message)
{
    fileManager_.addReceiverOperator(message);
}

void Manager::removeSenderOperator(const std::string &networkId)
{
    fileManager_.removeFileOperator<SenderOperator>(networkId);
}

std::string Manager::getFilePathString(const std::string &networkId)
{
    return fileManager_.getFilePathString(networkId);
}

bool Manager::setFileOperatorFilePath(
    const std::string &networkId, const std::filesystem::path &path)
{
    return fileManager_.setFilePath(networkId, path);
}

std::string Manager::getFileOperatorMD5Hash(const std::string &networkId)
{
    return fileManager_.getFileOperatorMD5Hash(networkId);
}

bool Manager::setMappedMessageWidget(
    CompanionPtr companion, MessagePtr message, MessageWidgetPtr widget)
{
    return messageHandler_.setMappedMessageWidget(companion, message, widget);
}

void Manager::fillCompanionMessageMapping(CompanionPtr companion, bool containersAreEmpty)
{
    messageHandler_.fillMessageStorageForCompanion(companion, containersAreEmpty);
}

void Manager::addMessageWidget(CompanionPtr companion, MessagePtr message)
{
    graphicManager_->addMessageWidget(companion, message);
}

void Manager::sortHistory(CompanionPtr companion)
{
    graphicManager_->sortHistory(companion);
}

void Manager::disableBlur()
{
    graphicManager_->disableBlur();
}

bool Manager::clientAction(CompanionPtr companion, bool isConnected)
{
    bool result = false;

    // TODO change to states
    if (isConnected) {
        result = disconnectClient(companion);
    }
    else {
        result = connectClient(companion);
        sendUnsentMessages(companion);
    }

    return result;
}

void Manager::sendFile(CompanionPtr companion, MessagePtr message)
{
    fileManager_.sendFile(companion, message->networkId());
}

void Manager::receiveFileData(CompanionPtr companion, MessagePtr message)
{
    fileManager_.receiveFileData(companion, message);
}

void Manager::receiveFile(CompanionPtr companion, MessagePtr message)
{
    // add to DB and get timestamp
    auto reply = pushMessageToDB(message);

    if (!reply || !reply->meta()->isValid())
        return;

    // decrypt message

    // create receiver operator
    addReceiverOperator(message);

    // add to widget
    addMessageWidget(companion, message);

    // send reply message to sender
    // bool result = sendMessageByNetwork(companion, message, replyMeta);
}

std::optional<bool> Manager::stopFileDataReceiving(MessagePtr message)
{
    return fileManager_.stopFileDataReceiving(message);
}

void Manager::stopFileDataSending(MessagePtr message)
{
    fileManager_.stopFileDataSending(message);
}

void Manager::markMessageAsSent(CompanionPtr companion, MessagePtr message)
{
    // mark in db
    auto messageIdData = getDBData(DBRequestType::SET_MESSAGE_IS_SENT_AND_RETURN, message->id());

    if (!messageIdData)
        return;

    // update message
    message->state()->setIsSent(true);

    // get message info
    auto info = messageHandler_.getMessageInfo(companion, message);

    if (!info)
        return;

    // set widget state
    graphicManager_->setMessageWidgetState(MessageWidgetState::SENT, info);
}

void Manager::markMessageAsReceived(MessageInfoPtr info)
{
    // set widget state
    graphicManager_->setMessageWidgetState(MessageWidgetState::RECEIVED, info);

    // if (!result)
    //     logArgsError("marking wiget as received error");

    // mark in db
    auto result = getDBData(
        DBRequestType::SET_MESSAGE_IS_RECEIVED_AND_RETURN, info->message()->id());
}

bool Manager::sendJsonByNetwork(CompanionPtr companion, const std::string &json)
{
    return networkManager_->sendJson(companion->id(), json);
}

DBReplyDataPtr Manager::getEarlyMessagesDBData(int companionId, int messageId)
{
    // get messages data
    auto data = getDBData(
        DBRequestType::GET_EARLY_MESSAGES_BY_MESSAGE_ID, companionId, messageId,
        NUMBER_OF_MESSAGES_TO_GET_FROM_DB);

    if (data && data->isEmpty()) {
        logTemplateWarning(
            "companion '{0}' has no messages earlier than id '{1}' in db", companionId, messageId);

        return nullptr;
    }

    return data;
}

MessagePtr Manager::pushMessageToDB(MessagePtr message)
{
    // push data to db
    auto messageData = getDBData(
        DBRequestType::PUSH_MESSAGE_AND_RETURN,
        message->companionName(),
        message->authorName(),
        message->time(),
        message->text(),
        message->isSent(),
        message->isReceived());

    if (!messageData || messageData->isEmpty())
        return nullptr;

    // update message
    uint32_t id = getIdFromDbData(messageData, 0, "id");
    uint8_t companionId = getIdFromDbData(messageData, 0, "companion_id");
    std::string timestampTz { messageData->getValue(0, "timestamp_tz") };

    if (LOG_DB_INTERACTION)
        logTemplateInfo("companionId: {0}, timestampTz: {1}", companionId, timestampTz);

    message->setId(id);
    message->setCompanionId(companionId);
    message->setTime(timestampTz);

    return message;
}

void Manager::setCurrentAction(ActionPtr action)
{
    currentAction_ = action;
}

void Manager::checkAndResetCurrentAction(ActionPtr action)
{
    if (currentAction_ != action) {
        logArgsError(Q_FUNC_INFO, "action mismatch");

        return;
    }

    // currentAction_.reset();
    currentAction_ = nullptr;
}

template<>
std::function<ActionResultPtr(CompanionActionPtr)>
Manager::getActionLambda(CompanionActionPtr action)
{
    switch (action->type()) {
    case ActionType::CREATE_COMPANION:
        return [=, this](auto action) { return createCompanion(action); };

    case ActionType::UPDATE_COMPANION:
        return [=, this](auto action) { return updateCompanion(action); };

    case ActionType::DELETE_COMPANION:
        return [=, this](auto action) { return deleteCompanion(action); };

    case ActionType::CLEAR_HISTORY:
        return [=, this](auto action) { return clearCompanionHistory(action); };

    default:
        return std::function<ActionResultPtr(CompanionActionPtr)>();
    }
}

template<>
std::function<ActionResultPtr(PasswordActionPtr)>
Manager::getActionLambda(PasswordActionPtr action)
{
    switch (action->type()) {
    case ActionType::CREATE_PASSWORD:
        return [=, this](auto action) { return createUserPassword(action); };

    case ActionType::GET_PASSWORD:
        return [=, this](auto action) { return authenticateUser(action); };

    default:
        return std::function<ActionResultPtr(PasswordActionPtr)>();
    }
}

template<>
std::function<ActionResultPtr(FileActionPtr)>
Manager::getActionLambda(FileActionPtr action)
{
    switch (action->type()) {
    case ActionType::SEND_FILE:
        // return [=, this](auto action) { return createUserPassword(action); };

    case ActionType::SAVE_FILE:
        // return [=, this](auto action) { return authenticateUser(action); };

    default:
        return std::function<ActionResultPtr(FileActionPtr)>();
    }
}

ManagerPtr getManager()
{
    QCoreApplication *coreApp = QCoreApplication::instance();
    ChatApp *app = dynamic_cast<ChatApp *>(coreApp);

    if (!app)
        return nullptr;

    return app->manager_;
}
