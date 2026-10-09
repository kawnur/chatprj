#include "manager.hpp"

#include "action_wrapper.hpp"
#include "application.hpp"
#include "companion.hpp"
#include "data.hpp"
#include "data_checker.hpp"
#include "functional"
#include "logging.hpp"
#include "message.hpp"
#include "message_handler.hpp"
#include "utils.hpp"
#include "validator.hpp"
#include "widgets.hpp"
#include "widgets_dialog.hpp"

using namespace std::string_literals;

Manager::Manager()
    : /*initialized_(false), */dbRequester_(LOG_DB_INTERACTION),/* validator_(),*/
    messageHandler_(),
    messageStateToMessageMapMutex_(), dbConnection_(nullptr),
    userIsAuthenticated_(false), selectedCompanion_(nullptr), mapCompanionToWidgetGroup_(),
    lastOpenedPath_(HOME_PATH) {}

Manager::~Manager()
{
    // free(dbConnection_);
}

CompanionPtr Manager::getSelectedCompanion()
{
    return selectedCompanion_;
}

bool Manager::userIsAuthenticated()
{
    return userIsAuthenticated_;
}

void Manager::set()
{
    // if (!dbRequester_.isReady())
    //     exitUtil(EXIT_FAILURE);

    messageHandler_.setManager(shared_from_this());

    bool companionsBuilt = buildCompanions();
    logArgs("companionsBuilt:", companionsBuilt);

    if (companionsBuilt)  // TODO rewrite
        buildWidgetGroups();
    else
        logArgsError("problem with companions initialization");

    // initialized_ = true;
}

CompanionPtr Manager::getMappedCompanionBySocketInfoBaseWidget(SocketInfoBaseWidget *widget) const
{
    auto lambda = [&](const auto &pair)
    {
        return pair.second.second->getSocketInfoBase().get() == widget;
    };

    auto result = std::ranges::find_if(mapCompanionToWidgetGroup_, lambda);

    return (result == mapCompanionToWidgetGroup_.end()) ? nullptr : result->second.first;
}

std::shared_ptr<WidgetGroup> Manager::getMappedWidgetGroupByCompanion(CompanionPtr companion) const
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

void Manager::sendFile(CompanionPtr companion, const std::filesystem::path &path)
{
    logArgs(__FUNCTION__);
}

void Manager::addMessageWidget(CompanionPtr companion, MessagePtr message)
{
    auto group = getMappedWidgetGroupByCompanion(companion);
    emit group->addMessageWidgetToCentralPanelChatHistorySignal(message);
}

bool Manager::pushMessageHistoryToDb(CompanionPtr companion, const nlohmann::json &data)
{
    // TODO wrap in util

    for (std::size_t i = 0; i < data["messages"].size(); i++) {
        uint8_t authorId =
            getIntFromString(ID_BAD_VALUE, data["messages"][i]["author_id"].get<std::string>());

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

            // showInfoDialogAndLogInfo(getStringByFormat(entryTemplate, timestamp, companionId));

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

void Manager::receiveMessage(CompanionPtr companion, const std::string &json)
{
    messageHandler_.receiveMessage(companion, json);
}

void Manager::addEarlyMessages(CompanionPtr companion)
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

void Manager::resetSelectedCompanion(CompanionPtr companion)  // TODO rewrite
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

void Manager::initFileSend(std::shared_ptr<Companion> companion)
{
    initAction<FileAction>(ActionType::SEND_FILE, ""s, companion);
}

void Manager::initFileReception(const std::string &networkId, std::shared_ptr<Companion> companion)
{
    initAction<FileAction>(ActionType::SAVE_FILE, networkId, companion);
}

ActionResultPtr Manager::getActionResultByKeyDBData(
    std::shared_ptr<DBReplyData> data, const std::string &key, bool allowEmptyResult)
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

    auto result = getIntFromString(ID_BAD_VALUE, data->getValue(0, key));

    if (result == ID_BAD_VALUE)
        return std::make_shared<ActionResult>(false, VALUE_BUILDING_ERROR);
    else
        return std::make_shared<ActionValueResult<int>>(result, true, ""s);
}

ActionResultPtr Manager::pushCompanionToDbAndReturnId(CompanionActionPtr action)
{
    auto data = getDBData(DBRequestType::PUSH_COMPANION_AND_RETURN, action->getName());

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
        DBRequestType::PUSH_SOCKET_AND_RETURN, action->getName(), action->getIpAddress(),
        serverPort, action->getClientPort());
}

ActionResultPtr Manager::createCompanion(CompanionActionPtr action)
{
    // data validation
    auto validationResult = validateActionData<CompanionAction>(action);

    if (!validationResult->status())
        return validationResult;

    // data checking
    auto checkResult = checkCompanionDataForExistanceAtCreation(action);

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
    auto companionResult = getCompanionAdditionResult(id, action->getName());

    if (!companionResult->status())
        return companionResult;

    // update data
    auto data = action->getCompanionData();
    data->setServerPort(serverPort);

    // create socketInfo object
    auto socketInfo = std::make_shared<SocketInfo>(data);
    auto companion = companionResult->value();
    companion->setSocketInfo(socketInfo);

    // add companion and widget group to mapping
    createWidgetGroupAndAddToMapping(companion);

    return std::make_shared<ActionResult>(true, ""s);
}

ActionResultPtr Manager::updateCompanionInDbAndReturnId(CompanionActionPtr action)
{
    auto data = getDBData(
        DBRequestType::UPDATE_COMPANION_AND_SOCKET_AND_RETURN, action->getName(),
        action->getCompanionId(), action->getIpAddress(), action->getClientPort());

    return getActionResultByKeyDBData(data, "id");
}

ActionResultPtr Manager::updateCompanion(CompanionActionPtr action)
{
    // TODO check new data before data dialog is closed
    // check if data was modified
    auto oldData = action->getCompanion()->getData();
    auto newData = action->getCompanionData();

    if (compareCompanionData(oldData, newData))
        return std::make_shared<ActionResult>(false, "companion data was not modified"s);

    // data validation
    auto validationResult = validateActionData<CompanionAction>(action);

    if (!validationResult->status())
        return validationResult;

    // data checking
    auto checkResult = checkCompanionDataForExistanceAtUpdate(action);

    if (!checkResult->status())
        return checkResult;

    // update companion data at db
    auto idResult = updateCompanionInDbAndReturnId(action);

    if (!idResult->status())
        return idResult;

    // update Companion and SocketInfo object
    action->updateCompanionObjectData();

    // update SocketInfoWidget
    auto widgetGroup = getMappedWidgetGroupByCompanion(action->getCompanion());
    widgetGroup->updateSocketInfoWidget();

    return std::make_shared<ActionResult>(true, ""s);
}

ActionResultPtr Manager::deleteCompanionMessagesFromDbAndReturnId(CompanionActionPtr action)
{
    auto data = getDBData(DBRequestType::DELETE_MESSAGES_AND_RETURN, action->getCompanionId());

    return getActionResultByKeyDBData(data, "companion_id", true);
}

ActionResultPtr Manager::deleteCompanionAndSocketFromDbAndReturnId(CompanionActionPtr action)
{
    auto data = getDBData(
        DBRequestType::DELETE_COMPANION_AND_SOCKET_AND_RETURN, action->getCompanionId());

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
    deleteCompanionObject(action->getCompanion());

    return std::make_shared<ActionResult>(true, ""s);
}

void Manager::clearChatHistory(CompanionPtr companion)
{
    auto widgetGroup = getMappedWidgetGroupByCompanion(companion);
    getGraphicManager()->clearChatHistory(widgetGroup);
}

ActionResultPtr Manager::clearCompanionHistory(CompanionActionPtr action)
{
    // delete companion chat messages from db
    auto deleteMessagesResult = deleteCompanionMessagesFromDbAndReturnId(action);

    if (!deleteMessagesResult->status())
        return deleteMessagesResult;

    // clear companion's message mapping
    action->getCompanion()->clearMessageMapping();

    // clear chat history widget
    clearChatHistory(action->getCompanion());

    return std::make_shared<ActionResult>(true, ""s);
}

ActionResultPtr Manager::pushPasswordToDbAndReturnId(PasswordActionPtr action)
{
    auto data = getDBData(DBRequestType::PUSH_PASSWORD_AND_RETURN, action->getPassword());

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
    auto graphicManager = getGraphicManager();

    // data checking
    auto checkResult = checkPasswordForExistanceAtAuthentication(action);

    if (!checkResult->status())
        return checkResult;

    userIsAuthenticated_ = true;

    return std::make_shared<ActionResult>(true, ""s);
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
        initEntrancePasswordCreation();
    else
        initEntrancePasswordReception();
}

void Manager::sendUnsentMessages(CompanionPtr companion)
{
    // get unsent messages from db
    auto messagesData = getDBData(
        DBRequestType::GET_UNSENT_MESSAGES_BY_COMPANION_NAME, companion->getName());

    if (!messagesData || messagesData->isEmpty())
        return;

    for (std::size_t i = 0; i < messagesData->size(); i++) {  // TODO switch to iterators
        uint32_t messageId = getIntFromString(ID_BAD_VALUE, messagesData->getValue(i, "id"));
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

void Manager::requestHistoryFromCompanion(CompanionPtr companion)
{
    auto meta = std::make_shared<MessageMetaData>();
    meta->networkMessageType_ = NetworkMessageType::CHAT_HISTORY_REQUEST;

    auto state = std::make_shared<MessageState>();
    state->isAntecedent_ = true;

    auto message = companion->createMessage(meta, nullptr, state);

    bool result = companion->sendMessage(message, message->meta());
}

void Manager::sendChatHistoryToCompanion(CompanionPtr companion)
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

void Manager::endAction(ActionPtr action)
{
    checkAndResetCurrentAction(action);
}

void Manager::sendMessage(
    MessageType type, CompanionPtr companion, ActionPtr action, const std::string &text)
{
    messageHandler_.sendMessage(type, companion, action, text);
}

CompanionPtr Manager::getMappedCompanionByWidgetGroup(std::shared_ptr<WidgetGroup> group) const
{
    auto lambda = [&](const auto &pair)
    {
        return pair.second.second == group;
    };

    auto result = std::ranges::find_if(mapCompanionToWidgetGroup_, lambda);

    return result->second.first;
}

void Manager::fillCompanionMessageMapping(CompanionPtr companion, bool containersNotEmpty)
{
    uint8_t companionId = companion->getId();

    // get messages data
    auto messagesData = getDBData(
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
        int id = getIntFromString(ID_BAD_VALUE, companionsData->getValue(index, "id"));

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
            getIntFromString(PORT_BAD_VALUE, socketsData->getValue(0, "server_port")),
            getIntFromString(PORT_BAD_VALUE, socketsData->getValue(0, "client_port")));

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

CompanionPtr Manager::addCompanionObject(int id, const std::string &name)
{
    if (id == 0) {
        logArgsError("companion id == 0");

        return nullptr;
    }

    auto companion = std::make_shared<Companion>(id, name);
    std::shared_ptr<WidgetGroup> group = nullptr;
    auto value = std::pair(companion, group);
    auto result = mapCompanionToWidgetGroup_.emplace(id, value);

    if (!result.second) {
        logArgsError("companion is nullptr");

        return nullptr;
    }

    return result.first->second.first;
}

std::shared_ptr<CompanionResult> Manager::getCompanionAdditionResult(
    int id, const std::string &name)
{
    if (id == 0) {
        auto entry = "companion id == 0"s;
        logArgsError(entry);  // TODO move logging to action

        return std::make_shared<CompanionResult>(nullptr, false, entry);
    }

    auto companion = std::make_shared<Companion>(id, name);
    std::shared_ptr<WidgetGroup> group = nullptr;
    auto value = std::pair(companion, group);
    auto result = mapCompanionToWidgetGroup_.emplace(id, value);

    if (!result.second) {
        auto entry = "companion is nullptr"s;
        logArgsError(entry);

        return std::make_shared<CompanionResult>(nullptr, false, entry);
    }

    return std::make_shared<CompanionResult>(companion, true, ""s);
}

void Manager::createWidgetGroupAndAddToMapping(CompanionPtr companion)
{
    auto group = std::make_shared<WidgetGroup>(companion);
    group->set();
    mapCompanionToWidgetGroup_[companion->getId()].second = group;
    companion->addMessageWidgetsToChatHistory(group);
}

void Manager::deleteCompanionObject(CompanionPtr companion)
{
    deleteWidgetGroupAndDeleteFromMapping(companion);
}

void Manager::deleteWidgetGroupAndDeleteFromMapping(CompanionPtr companion)
{
    auto lambda = [&](const auto &iterator)
    {
        return iterator.second.first == companion;
    };

    // TODO use range
    auto result = std::ranges::find_if(mapCompanionToWidgetGroup_, lambda);

    if (result == mapCompanionToWidgetGroup_.end()) {
        // showErrorDialogAndLogError("Companion was not found in mapping at deletion");
    }
    else {
        if (selectedCompanion_ == companion)
            selectedCompanion_ = nullptr;

        mapCompanionToWidgetGroup_.erase(result);
    }
}

ActionResultPtr Manager::checkCompanionDataForExistanceAtCreation(CompanionActionPtr action)
{
    auto lambda = [](auto &checker) { return checker.checkCompanionDataForExistanceAtCreation(); };

    return checkDataForExistance(lambda, action);
}

ActionResultPtr Manager::checkCompanionDataForExistanceAtUpdate(CompanionActionPtr action)
{
    auto lambda = [](auto &checker) { return checker.checkCompanionDataForExistanceAtUpdate(); };

    return checkDataForExistance(lambda, action);
}

ActionResultPtr Manager::checkPasswordForExistanceAtAuthentication(PasswordActionPtr action)
{
    auto lambda = [](auto &checker) { return checker.checkPasswordForExistanceAtAuthentication(); };

    return checkDataForExistance(lambda, action);
}

void Manager::waitForMessageReceptionConfirmation(CompanionPtr companion, MessagePtr message)
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

void Manager::markMessageAsSent(CompanionPtr companion, MessagePtr message)
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
    // CompanionPtr companion, MessagePtr message)
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

MessageMetaDataPtr Manager::pushMessageToDB(
    MessageMetaDataPtr meta, MessageDataPtr data, MessageStatePtr state)
{
    const std::string companionIdString("companion_id");

    auto messageData = getDBData(
        DBRequestType::PUSH_MESSAGE_AND_RETURN, meta->companionName_, meta->authorName_,
        meta->timestampTz_, data->text_, state->isSent_, state->isReceived_);

    // TODO get rid of copy constructing
    if (!messageData || messageData->isEmpty())
        return nullptr;

    uint32_t id = getIntFromString(ID_BAD_VALUE, messageData->getValue(0, "id"));
    uint8_t companionId = getIntFromString(ID_BAD_VALUE, messageData->getValue(0, "companion_id"));
    std::string timestampTz { messageData->getValue(0, "timestamp_tz") };

    if (LOG_DB_INTERACTION)
        logTemplateInfo("companionId: {0}, timestampTz: {1}", companionId, timestampTz);

    // TODO get rid of copy constructing
    auto result = std::make_shared<MessageMetaData>();

    result->messageId_ = id;
    result->companionId_ = companionId;
    result->authorId_ = 0;
    result->timestampTz_ = timestampTz;

    return result;
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
    switch (action->getType()) {
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
    switch (action->getType()) {
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
    switch (action->getType()) {
    case ActionType::SEND_FILE:
        // return [=, this](auto action) { return createUserPassword(action); };

    case ActionType::SAVE_FILE:
        // return [=, this](auto action) { return authenticateUser(action); };

    default:
        return std::function<ActionResultPtr(FileActionPtr)>();
    }
}

std::shared_ptr<Manager> getManager()
{
    QCoreApplication *coreApp = QCoreApplication::instance();
    ChatApp *app = dynamic_cast<ChatApp *>(coreApp);

    if (!app)
        return nullptr;

    return app->manager_;
}
