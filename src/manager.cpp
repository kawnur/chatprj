#include "manager.hpp"

Manager::Manager() :
    initialized_(false),
    messageStateToMessageMapMutex_(std::mutex()),
    dbConnectionPtr_(nullptr), userIsAuthenticated_(false) {
    mapCompanionIdToCompanionInfo_ = std::map<int, std::pair<Companion*, WidgetGroup*>>();
    selectedCompanionPtr_ = nullptr;
    lastOpenedPath_ = homePath;
}

Manager::~Manager() {
    free(dbConnectionPtr_);

    for(auto& pair : this->mapCompanionIdToCompanionInfo_) {
        delete pair.second.first;
        delete pair.second.second;
    }
}

const Companion* Manager::getSelectedCompanionPtr() {
    return this->selectedCompanionPtr_;
}

bool Manager::getUserIsAuthenticated() {
    return this->userIsAuthenticated_;
}

const Companion* Manager::getMappedCompanionBySocketInfoBaseWidget(
    SocketInfoBaseWidget* widget) const {

    auto findWidget = [&](auto& pair){
        return pair.second.second->getSocketInfoBasePtr() == widget;
    };

    auto result = std::find_if(
        this->mapCompanionIdToCompanionInfo_.cbegin(),
        this->mapCompanionIdToCompanionInfo_.cend(),
        findWidget);

    return result->second.first;
}

WidgetGroup* Manager::getMappedWidgetGroupPtrByCompanionPtr(
    const Companion* companionPtr) const {
    WidgetGroup* groupPtr = nullptr;

    try {
        groupPtr = this->mapCompanionIdToCompanionInfo_.at(companionPtr->getId()).second;
    }
    catch(std::out_of_range) {}

    return groupPtr;
}

void Manager::set() {
    bool connectedToDB = this->connectToDb();
    // logArgs("connectedToDB:", connectedToDB);

    if(connectedToDB) {
        bool companionsBuilt = this->buildCompanions();
        logArgs("companionsBuilt:", companionsBuilt);

        if(companionsBuilt) {  // TODO rewrite
            this->buildWidgetGroups();
        }
        else {
            logArgsError("problem with companions initialization");
        }
    }
    else {
        showErrorDialogAndLogError(nullptr, "problem with DB connection");
    }

    this->initialized_ = true;
}

void Manager::sendMessage(
    MessageType type, Companion* companionPtr,
    Action* actionPtr, const std::string& text) {
    WidgetGroup* groupPtr = this->getMappedWidgetGroupPtrByCompanionPtr(companionPtr);

    // encrypt message

    // add to DB and get timestamp
    auto tuple = this->pushMessageToDB(
        companionPtr->getName(), std::string("me"), std::string("now()"),
        text, false, false);

    uint32_t id = std::get<0>(tuple);
    uint8_t companion_id = std::get<1>(tuple);
    std::string timestamp = std::get<2>(tuple);

    if(companion_id == 0 || timestamp == "") {
        logArgsError("error adding message to db");
        return;
    }

    auto pair = companionPtr->createMessageAndAddToMapping(
        type, id, 1, timestamp, text, false, false, false, "");

    if(pair.second) {
        // add to widget
        const Message* messagePtr = &(pair.first->first);
        MessageState* messageStatePtr = pair.first->second.getStatePtr();

        if(type == MessageType::FILE) {
            companionPtr->getFileOperatorStoragePtr()->addSenderOperator(
                messageStatePtr->getNetworkId(),
                dynamic_cast<FileAction*>(actionPtr)->getPath());
        }

        groupPtr->addMessageWidgetToCentralPanelChatHistory(messagePtr, messageStatePtr);

        NetworkMessageType networkMessageType;

        switch(type) {
        case MessageType::TEXT:
            networkMessageType = NetworkMessageType::TEXT;

            break;

        case MessageType::FILE:
            networkMessageType = NetworkMessageType::FILE_PROPOSAL;

            break;
        }

        // send over network
        bool result = companionPtr->sendMessage(
            false, networkMessageType,
            messageStatePtr->getNetworkId(), messagePtr);

        // mark message as sent
        if(result) {
            this->markMessageAsSent(companionPtr, messagePtr);
        }

        // wait for message reception confirmation
        this->waitForMessageReceptionConfirmation(companionPtr, messageStatePtr, messagePtr);
    }
}

void Manager::sendFile(Companion* companionPtr, const std::filesystem::path& path) {
    logArgs("Manager::sendFile");
}

void Manager::receiveMessage(Companion* companionPtr, const std::string& jsonString) {
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

    switch(type) {
    // message is data message
    case NetworkMessageType::TEXT:
    case NetworkMessageType::FILE_PROPOSAL: {
            MessageType messageType;
            NetworkMessageType replyMessageType;

            switch(type) {
            case NetworkMessageType::TEXT:
                messageType = MessageType::TEXT;
                replyMessageType = NetworkMessageType::RECEIVE_CONFIRMATION;

                break;

            case NetworkMessageType::FILE_PROPOSAL:
                messageType = MessageType::FILE;
                replyMessageType = NetworkMessageType::NO_ACTION;

                std::string hashMD5FromSender = jsonData.at("hashMD5");

                // create receiver operator
                companionPtr->getFileOperatorStoragePtr()->
                    addReceiverOperator(networkId, hashMD5FromSender, homePath);

                break;
            }

            auto timestamp = jsonData.at("time");
            auto text = jsonData.at("text");
            auto name = companionPtr->getName();

            // add to DB and get timestamp
            auto tuple = this->pushMessageToDB(name, name, timestamp, text, false, true);

            uint32_t id = std::get<0>(tuple);
            uint8_t companion_id = std::get<1>(tuple);

            if(companion_id == 0 || timestamp == "") {
                logArgsError("error adding message to db");
                return;
            }

            auto pair = companionPtr->createMessageAndAddToMapping(
                messageType, id, companion_id, timestamp, text,
                isAntecedent, false, true, networkId);

            if(pair.second) {
                const Message* messagePtr = &(pair.first->first);
                MessageState* messageStatePtr = pair.first->second.getStatePtr();

                // decrypt message

                // add to widget
                WidgetGroup* groupPtr =
                    this->getMappedWidgetGroupPtrByCompanionPtr(companionPtr);

                emit groupPtr->addMessageWidgetToCentralPanelChatHistorySignal(
                    messageStatePtr, messagePtr);

                // send message to sender
                bool result = companionPtr->sendMessage(
                    false, replyMessageType, networkId, messagePtr);
            }
        }

        break;

    // message is confirmation
    case NetworkMessageType::RECEIVE_CONFIRMATION: {
            auto received = jsonData.at("received");

            if(received == 1) {  // successfully received
                // mark message as received
                // std::string key = generateMessageKey(networkId, companionPtr->getId());

                // auto pair = this->getMessageStateAndMessageMappingPairByMessageMappingKey(key);
                // auto pairPtr = companionPtr->getMessageMappingPairPtrByMessageKey(key);
                auto pairPtr = companionPtr->getMessageMappingPairPtrByNetworkId(networkId);

                // TODO rewrite
                if(pairPtr && pairPtr->second.getStatePtr()) {
                    // found message in mapping
                    pairPtr->second.getStatePtr()->setIsReceived(true);
                    this->markMessageAsReceived(companionPtr, &(pairPtr->first));
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

    case NetworkMessageType::RECEIVE_CONFIRMATION_REQUEST: {
            // search for message in managers's mapping
            // std::string key = generateMessageKey(networkId, companionPtr->getId());

            // auto pairPtr = companionPtr->getMessageMappingPairPtrByMessageKey(key);
            auto pairPtr = companionPtr->getMessageMappingPairPtrByNetworkId(networkId);

            logArgs("pairPtr:", pairPtr);

            if(pairPtr && pairPtr->second.getStatePtr()) {
                logArgs("pairPtr->second.getStatePtr():", pairPtr->second.getStatePtr());

                // message found in managers's mapping
                if(pairPtr->second.getStatePtr()->getIsReceived()) {
                    bool result = companionPtr->sendMessage(
                        false, NetworkMessageType::RECEIVE_CONFIRMATION,
                        pairPtr->second.getStatePtr()->getNetworkId(), &(pairPtr->first));
                }
                else {
                    // strange situation
                    logArgsError(
                        "received reception confirmation request "
                        "for message in messageMapping_ with isReceived = false");
                }
            }
            else {
                // probably old message from previous sessions, search for it in db
                logArgsInfo("probably old message from previous sessions, search for it in db");
            }
        }

        break;

    case NetworkMessageType::CHAT_HISTORY_REQUEST: {
            logArgsInfo("got history request from " + companionPtr->getName());

            emit this->getMappedWidgetGroupPtrByCompanionPtr(companionPtr)->
                askUserForHistorySendingConfirmationSignal();
        }

        break;

    case NetworkMessageType::CHAT_HISTORY_DATA: {
            logArgsInfo("got chat history from " + companionPtr->getName());
            logArgs("jsonString:", jsonString);

            auto json = buildJsonObject(jsonString);

            for(size_t i = 0; i < json["messages"].size(); i++) {
                uint8_t authorId = std::stoi(
                    json["messages"][i]["author_id"].get<std::string>());

                authorId = (authorId == 1) ? companionPtr->getId() : 1;

                std::string timestamp = json["messages"][i]["timestamp_tz"];
                std::string message = json["messages"][i]["message"];

                uint8_t companionId = companionPtr->getId();

                // check if message from this companion with such timestamp
                // already exists
                std::shared_ptr<DBReplyData> messageGetDataPtr = this->getDBDataPtr(
                    logDBInteraction,
                    "getMessageByCompanionIdAndTimestampDBResult",
                    &getMessageByCompanionIdAndTimestampDBResult,
                    buildStringVector("id"),
                    companionId, timestamp);

                if(!messageGetDataPtr) {
                    showErrorDialogAndLogError(nullptr, "Error getting data from db");
                    return;
                }

                if(!messageGetDataPtr->isEmpty()) {
                    showInfoDialogAndLogInfo(
                        nullptr,
                        getQString(
                            std::format(
                                "Message with timestamp {0} from companion "
                                "with id {1} already exists",
                                timestamp, companionId)));

                    continue;
                }

                std::string idString { "id" };

                // push message to db
                std::shared_ptr<DBReplyData> messageAddDataPtr = this->getDBDataPtr(
                    logDBInteraction,
                    "pushMessageToDBAndReturn",
                    &pushMessageToDBWithAuthorIdAndReturn,
                    buildStringVector("id", "companion_id", "timestamp_tz"),
                    companionPtr->getName(), std::to_string(authorId), timestamp,
                    idString, message, true, true);

                if(!messageAddDataPtr) {
                    showErrorDialogAndLogError(nullptr, "Error getting data from db");
                    return;
                }

                if(messageAddDataPtr->isEmpty()) {
                    showErrorDialogAndLogError(nullptr, "Error pushing chat history to db");
                    return;
                }
            }

            // clear chat history widget
            this->clearChatHistory(companionPtr);

            // fill container with messages
            this->fillCompanionMessageMapping(companionPtr, true);

            // build chat history
            emit this->getMappedWidgetGroupPtrByCompanionPtr(companionPtr)->
                buildChatHistorySignal();
        }

        break;

    case NetworkMessageType::FILE_REQUEST: {
            logArgs("got NetworkMessageType::FILE_REQUEST");

            auto senderPtr =
                companionPtr->getFileOperatorPtrByNetworkId<SenderOperator>(networkId);

            if(senderPtr) {
                senderPtr->sendFile(companionPtr, networkId);
            }
            else {
                logArgsErrorWithTemplate(
                    "companion has no file operator for networkId = {}", networkId);
            }
        }

        break;

    case NetworkMessageType::FILE_DATA: {
            logArgs("got NetworkMessageType::FILE_DATA");

            auto receiverPtr =
                companionPtr->getFileOperatorPtrByNetworkId<ReceiverOperator>(networkId);

            if(receiverPtr) {
                receiverPtr->receiveFilePart(jsonData.at("data"));
            }
            else {
                logArgsErrorWithTemplate(
                    "companion has no file operator for networkId = {}", networkId);
            }
        }

        break;

    case NetworkMessageType::FILE_DATA_CHECK_SUCCESS: {
            logArgs("got NetworkMessageType::FILE_DATA_CHECK_SUCCESS");

            logArgsInfoWithTemplate(
                "file {} received by companion successfully",
                companionPtr->getFileOperatorFilePathStringByNetworkId(networkId));

            companionPtr->removeFileOperator<SenderOperator>(networkId);
        }

        break;

    case NetworkMessageType::FILE_DATA_CHECK_FAILURE: {
            logArgs("got NetworkMessageType::FILE_DATA_CHECK_FAILURE");

            logArgsInfoWithTemplate(
                "file {} WAS NOT received by companion",
                companionPtr->getFileOperatorFilePathStringByNetworkId(networkId));

            companionPtr->removeFileOperator<SenderOperator>(networkId);
        }

        break;

    case NetworkMessageType::FILE_DATA_TRANSMISSON_END: {
            logArgs("got NetworkMessageType::FILE_DATA_TRANSMISSON_END");

            auto receiverPtr =
                companionPtr->getFileOperatorPtrByNetworkId<ReceiverOperator>(networkId);

            if(receiverPtr) {
                auto resultType = (receiverPtr->receiveFile()) ?
                    NetworkMessageType::FILE_DATA_CHECK_SUCCESS :
                    NetworkMessageType::FILE_DATA_CHECK_FAILURE;

                if(resultType == NetworkMessageType::FILE_DATA_CHECK_SUCCESS) {
                    logArgs("file received successfully");

                    companionPtr->removeFileOperator<ReceiverOperator>(networkId);
                }

                bool result =
                    companionPtr->sendMessage(false, resultType, networkId, nullptr);
            }
            else {
                logArgsErrorWithTemplate(
                    "companion has no file operator for networkId = {}", networkId);
            }
        }

        break;

    case NetworkMessageType::FILE_DATA_TRANSMISSON_FAILURE: {
            logArgs("got NetworkMessageType::FILE_DATA_TRANSMISSON_FAILURE");

            logArgsInfoWithTemplate(
                "file {} WAS NOT received by companion",
                companionPtr->getFileOperatorFilePathStringByNetworkId(networkId));

            companionPtr->removeFileOperator<SenderOperator>(networkId);
        }

        break;
    }
}

void Manager::addEarlyMessages(const Companion* companionPtr) {
    if(companionPtr) {
        // get earliest message in current messages
        const Message* earliestMessagePtr = companionPtr->getEarliestMessagePtr();

        auto messageId = earliestMessagePtr->getId();
        auto companionId = companionPtr->getId();

        // get messages data
        std::shared_ptr<DBReplyData> messagesDataPtr = this->getDBDataPtr(
            logDBInteraction,
            "getEarlyMessagesByMessageIdDBResult",
            &getEarlyMessagesByMessageIdDBResult,
            buildStringVector(
                "id", "companion_id", "author_id",
                "timestamp_tz", "message", "is_sent", "is_received"),
            companionId, messageId);

        if(!messagesDataPtr) {
            showErrorDialogAndLogError(nullptr, "Error getting data from db");
            return;
        }

        if(messagesDataPtr->isEmpty()) {
            logArgsWarningWithTemplate(
                "no messages earlier than id = {0} in db with companion {1}",
                messageId, companionId);

            return;
        }

        WidgetGroup* widgetGroupPtr =
            this->getMappedWidgetGroupPtrByCompanionPtr(companionPtr);

        for(size_t i = 0; i < messagesDataPtr->size(); i++) {  // TODO switch to iterators
            logArgs("adding message with id", messagesDataPtr->getValue(i, "id"));
            auto pair = const_cast<Companion*>(companionPtr)->
                createMessageAndAddToMapping(messagesDataPtr, i);

            if(pair.second) {
                widgetGroupPtr->
                    addMessageWidgetToCentralPanelChatHistory(
                        &(pair.first->first),
                        pair.first->second.getStatePtr());
            }
            else {
                logArgsError("could not add message to messageMapping_");
            }
        }

        widgetGroupPtr->sortChatHistoryElements();
    }
}

void Manager::resetSelectedCompanion(const Companion* newSelected) {  // TODO rewrite
    GraphicManager* graphicManagerPtr = getGraphicManagerPtr();

    if(this->selectedCompanionPtr_) {
        auto widgetGroupPtr =
            this->getMappedWidgetGroupPtrByCompanionPtr(this->selectedCompanionPtr_);

        dynamic_cast<SocketInfoWidget*>(widgetGroupPtr->getSocketInfoBasePtr())->unselect();

        widgetGroupPtr->hideCentralPanel();
    }
    else {
        graphicManagerPtr->hideCentralPanelStub();
    }

    this->selectedCompanionPtr_ = newSelected;

    if(this->selectedCompanionPtr_) {
        auto widgetGroupPtr =
            this->getMappedWidgetGroupPtrByCompanionPtr(this->selectedCompanionPtr_);

        dynamic_cast<SocketInfoWidget*>(widgetGroupPtr->getSocketInfoBasePtr())->
            select();

        widgetGroupPtr->showCentralPanel();
    }
    else {
        graphicManagerPtr->showCentralPanelStub();
    }
}

void Manager::createCompanion(CompanionAction* companionActionPtr) {
    // data validation and checking
    if(!(companionDataValidation(companionActionPtr) &&
          this->checkCompanionDataForExistanceAtCreation(companionActionPtr))) {
        return;
    }

    auto name = companionActionPtr->getName();
    auto ipAddress = companionActionPtr->getIpAddress();
    auto clientPortStr = companionActionPtr->getClientPort();

    // push companion data to db
    std::shared_ptr<DBReplyData> companionIdDataPtr = this->getDBDataPtr(
        logDBInteraction,
        "pushCompanionToDBAndReturn",
        &pushCompanionToDBAndReturn,
        buildStringVector("id"),
        name);

    if(!companionIdDataPtr) {
        showErrorDialogAndLogError(nullptr, "Error getting data from db");
        return;
    }

    if(companionIdDataPtr->isEmpty()) {
        showErrorDialogAndLogError(nullptr, "Empty db reply to new companion pushing");
        return;
    }

    int id = std::atoi(companionIdDataPtr->getValue(0, "id"));

    // push socket data to db
    uint16_t serverPort = 5000 + id + 1;  // TODO change

    std::shared_ptr<DBReplyData> socketDataPtr = this->getDBDataPtr(
        logDBInteraction,
        "pushSocketToDB",
        &pushSocketToDBAndReturn,
        buildStringVector("id"),
        name, ipAddress, std::to_string(serverPort), clientPortStr);

    if(!socketDataPtr) {
        showErrorDialogAndLogError(nullptr, "Error getting data from db");
        return;
    }

    if(socketDataPtr->isEmpty()) {
        showErrorDialogAndLogError(nullptr, "Empty db reply to new socket pushing");
        return;
    }

    // create Companion object
    Companion* companionPtr = this->addCompanionObject(id, name);

    if(!companionPtr) {
        logArgsError("companionPtr is nullptr");
        return;
    }

    // create SocketInfo object
    SocketInfo* socketInfoPtr = new SocketInfo(
        ipAddress, serverPort, std::stoi(clientPortStr));

    companionPtr->setSocketInfo(socketInfoPtr);

    // add companion and widget group to mapping
    this->createWidgetGroupAndAddToMapping(companionPtr);

    // show info dialog
    getGraphicManagerPtr()->showCompanionInfoDialog(
        companionActionPtr, std::string { "New companion added:\n\n" });
}

void Manager::updateCompanion(CompanionAction* companionActionPtr) {
    // data validation and checking
    if(!(companionDataValidation(companionActionPtr) &&
          this->checkCompanionDataForExistanceAtUpdate(companionActionPtr))) {
        return;
    }

    // update companion data at db
    std::shared_ptr<DBReplyData> companionIdDataPtr = this->getDBDataPtr(
        logDBInteraction,
        "updateCompanionAndSocketAndReturn",
        &updateCompanionAndSocketAndReturn,
        buildStringVector("id"),
        *companionActionPtr);

    if(!companionIdDataPtr) {
        showErrorDialogAndLogError(nullptr, "Error getting data from db");
        return;
    }

    if(companionIdDataPtr->isEmpty()) {
        showErrorDialogAndLogError(nullptr, "Empty db reply to companion update");
        return;
    }

    // update Companion and SocketInfo object
    companionActionPtr->updateCompanionObjectData();

    // update SocketInfoWidget
    auto widgetGroupPtr = this->getMappedWidgetGroupPtrByCompanionPtr(
        companionActionPtr->getCompanionPtr());

    dynamic_cast<SocketInfoWidget*>(widgetGroupPtr->getSocketInfoBasePtr())->update();

    // show info dialog
    getGraphicManagerPtr()->showCompanionInfoDialog(
        companionActionPtr, std::string { "Companion updated:\n\n" });
}

void Manager::deleteCompanion(CompanionAction* companionActionPtr) {
    // delete companion chat messages from db
    std::shared_ptr<DBReplyData> companionIdMessagesDataPtr = this->getDBDataPtr(
        logDBInteraction,
        "deleteMessagesFromDBAndReturn",
        &deleteMessagesFromDBAndReturn,
        buildStringVector("id"),
        *companionActionPtr);

    if(!companionIdMessagesDataPtr) {
        showErrorDialogAndLogError(nullptr, "Error getting data from db");
        return;
    }

    if(companionIdMessagesDataPtr->isEmpty()) {
        // no return, may be companion without messages
        // showWarningDialogAndLogWarning("Empty db reply to companion messages deletion");
    }

    // delete companion and socket from db
    std::shared_ptr<DBReplyData> companionIdCompanionDataPtr = this->getDBDataPtr(
        logDBInteraction,
        "deleteCompanionAndSocketAndReturn",
        &deleteCompanionAndSocketAndReturn,
        buildStringVector("id"),
        *companionActionPtr);

    if(!companionIdCompanionDataPtr) {
        showErrorDialogAndLogError(nullptr, "Error getting data from db");
        return;
    }

    if(companionIdCompanionDataPtr->isEmpty()) {
        showErrorDialogAndLogError(nullptr, "Empty db reply to companion deletion");
        return;
    }

    // delete companion object
    this->deleteCompanionObject(companionActionPtr->getCompanionPtr());

    // show info dialog
    getGraphicManagerPtr()->showCompanionInfoDialog(
        companionActionPtr, std::string { "Companion deleted:\n\n" });
}

void Manager::clearChatHistory(Companion* companionPtr) {
    WidgetGroup* widgetGroupPtr =
        this->getMappedWidgetGroupPtrByCompanionPtr(companionPtr);

    getGraphicManagerPtr()->clearChatHistory(widgetGroupPtr);
}

void Manager::clearCompanionHistory(CompanionAction* companionActionPtr) {
    // delete companion chat messages from db
    std::shared_ptr<DBReplyData> companionIdMessagesDataPtr = this->getDBDataPtr(
        logDBInteraction,
        "deleteMessagesFromDBAndReturn",
        &deleteMessagesFromDBAndReturn,
        buildStringVector("companion_id"),
        *companionActionPtr);

    if(!companionIdMessagesDataPtr) {
        showErrorDialogAndLogError(nullptr, "Error getting data from db");
        return;
    }

    if(companionIdMessagesDataPtr->isEmpty()) {
        // no return, may be companion without messages
        // showWarningDialogAndLogWarning("Empty db reply to companion messages deletion");
    }

    // clear companion's message mapping
    companionActionPtr->getCompanionPtr()->clearMessageMapping();

    // clear chat history widget
    this->clearChatHistory(companionActionPtr->getCompanionPtr());

    // show info dialog
    getGraphicManagerPtr()->showCompanionInfoDialog(
        companionActionPtr, std::string { "Companion chat history cleared:\n\n" });
}

void Manager::createUserPassword(PasswordAction* actionPtr) {
    // data validation and checking
    if(!(this->passwordDataValidation(actionPtr))) {
        return;
    }

    // push password data to db
    std::shared_ptr<DBReplyData> passwordIdDataPtr = this->getDBDataPtr(
        logDBInteraction,
        "pushPasswordToDBAndReturn",
        &pushPasswordToDBAndReturn,
        buildStringVector("id"),
        actionPtr->getPassword());

    if(!passwordIdDataPtr) {
        showErrorDialogAndLogError(nullptr, "Error getting data from db");
        return;
    }

    if(passwordIdDataPtr->isEmpty()) {
        showErrorDialogAndLogError(nullptr, "Empty db reply to new password pushing");
        return;
    }

    // show dialog
    showInfoDialogAndLogInfo(
        actionPtr->getDialogPtr(),
        newPasswordCreatedLabel,
        &TextDialog::unsetMainWindowBlurAndCloseDialogs);

    // delete action object
    delete actionPtr;
}

void Manager::authenticateUser(PasswordAction* actionPtr) {
    GraphicManager* graphicManagerPtr = getGraphicManagerPtr();

    // do we have password in db?
    std::shared_ptr<DBReplyData> passwordDataPtr = this->getDBDataPtr(
        logDBInteraction,
        "getPasswordDBResult",
        &getPasswordDBResult,
        buildStringVector("id", "password"));

    if(!passwordDataPtr) {
        showErrorDialogAndLogError(nullptr, "Error getting password from db");
        return;
    }

    if(passwordDataPtr->isEmpty()) {
        showErrorDialogAndLogError(nullptr, "Db password data is empty");
        return;
    }
    else {
        if(passwordDataPtr->getValue(0, "password") == actionPtr->getPassword()) {
            this->userIsAuthenticated_ = true;

            logArgsInfo("user successfully authenticated");
            graphicManagerPtr->disableMainWindowBlurEffect();

            // dialog is closed in action dtor
            delete actionPtr;
        }
        else {
            showErrorDialogAndLogError(nullptr, "Password is not correct");
            return;
        }
    }
}

void Manager::hideSelectedCompanionCentralPanel() {
    if(this->selectedCompanionPtr_) {
        auto groupPtr =
            this->getMappedWidgetGroupPtrByCompanionPtr(this->selectedCompanionPtr_);

        getGraphicManagerPtr()->hideWidgetGroupCentralPanel(groupPtr);
    }
}

void Manager::showSelectedCompanionCentralPanel() {
    if(this->selectedCompanionPtr_) {
        auto groupPtr =
            this->getMappedWidgetGroupPtrByCompanionPtr(this->selectedCompanionPtr_);

        getGraphicManagerPtr()->showWidgetGroupCentralPanel(groupPtr);
    }
}

void Manager::startUserAuthentication() {
    GraphicManager* graphicManagerPtr = getGraphicManagerPtr();

    graphicManagerPtr->enableMainWindowBlurEffect();

    // do we have password in db?
    std::shared_ptr<DBReplyData> passwordDataPtr = this->getDBDataPtr(
        logDBInteraction,
        "getPasswordDBResult",
        &getPasswordDBResult,
        buildStringVector("id", "password"));

    if(!passwordDataPtr) {
        showErrorDialogAndLogError(nullptr, "Error getting password from db");
        return;
    }

    if(passwordDataPtr->isEmpty()) {
        graphicManagerPtr->createEntrancePassword();
    }
    else {
        graphicManagerPtr->getEntrancePassword();
    }
}

void Manager::sendUnsentMessages(const Companion* companionPtr) {
    Companion* companionCastPtr = const_cast<Companion*>(companionPtr);

    // get unsent messages from db
    std::shared_ptr<DBReplyData> messagesDataPtr = this->getDBDataPtr(
        logDBInteraction,
        "getUnsentMessagesByCompanionNameDBResult",
        &getUnsentMessagesByCompanionNameDBResult,
        buildStringVector(
            "id", "author_id", "companion_id", "timestamp_tz",
            "message", "is_received"),
        companionPtr->getName());

    if(!messagesDataPtr) {
        showErrorDialogAndLogError(nullptr, "Error getting data from db");
        return;
    }

    if(messagesDataPtr->isEmpty()) {
        logArgsInfo("Empty db reply to unsent messages selection");
        return;
    }

    for(size_t i = 0; i < messagesDataPtr->size(); i++) {  // TODO switch to iterators
        uint32_t messageId = std::atoi(messagesDataPtr->getValue(i, "id"));
        Message* messagePtr = companionCastPtr->findMessage(messageId);
        std::string networkId;

        if(messagePtr) {
            const MessageState* messageStatePtr =
                const_cast<Companion*>(companionPtr)->
                    getMappedMessageStatePtrByMessagePtr(messagePtr);

            if(!messageStatePtr) {
                logArgsError(
                    "strange case: unsent message found in companions messages, "
                    "but not found in companion's messageMapping_");
            }
            else {
                networkId = messageStatePtr->getNetworkId();
            }
        }
        else {
            // add to companion's messages if needed
            networkId = getRandomString(5);

            const_cast<Companion*>(companionPtr)->createMessageAndAddToMapping(
                MessageType::TEXT,
                messageId,
                1,
                messagesDataPtr->getValue(i, "timestamp_tz"),
                messagesDataPtr->getValue(i, "message"),
                false,
                false,
                getBoolFromDBValue(messagesDataPtr->getValue(i, "is_received")),
                networkId);
        }

        // send over network
        bool result = companionCastPtr->sendMessage(
            true, NetworkMessageType::TEXT, networkId, messagePtr);

        // mark message as sent
        if(result) {
            this->markMessageAsSent(const_cast<Companion*>(companionPtr), messagePtr);
        }
    }
}

void Manager::requestHistoryFromCompanion(const Companion* companionPtr) {
    Companion* companionCastPtr = const_cast<Companion*>(companionPtr);

    bool result = companionCastPtr->sendMessage(
        true, NetworkMessageType::CHAT_HISTORY_REQUEST, "", nullptr);
}

void Manager::sendChatHistoryToCompanion(const Companion* companionPtr) {
    logArgs("Manager::sendChatHistoryToCompanion");

    std::vector<std::string> keys =
        buildStringVector("author_id", "timestamp_tz", "message");

    // get messages from db
    std::shared_ptr<DBReplyData> messagesDataPtr = this->getDBDataPtr(
        logDBInteraction,
        "getAllMessagesByCompanionIdDBResult",
        &getAllMessagesByCompanionIdDBResult,
        keys,
        companionPtr->getId());

    if(!messagesDataPtr) {
        showErrorDialogAndLogError(nullptr, "Error getting data from db");
        return;
    }

    if(messagesDataPtr->isEmpty()) {
        logArgsInfo("Empty db reply to messages selection");
        return;
    }

    bool result = companionPtr->sendChatHistory(messagesDataPtr, keys);
}

bool Manager::isInitialised() {
    return this->initialized_;
}

std::filesystem::path Manager::getLastOpenedPath() {
    return this->lastOpenedPath_;
}

void Manager::setLastOpenedPath(const std::filesystem::path& path) {
    this->lastOpenedPath_ = path;
}

const Companion* Manager::getMappedCompanionByWidgetGroup(
    WidgetGroup* groupPtr) const {
    auto findWidget = [&](auto& pair){
        return pair.second.second == groupPtr;
    };

    auto result = std::find_if(
        this->mapCompanionIdToCompanionInfo_.cbegin(),
        this->mapCompanionIdToCompanionInfo_.cend(),
        findWidget);

    return result->second.first;
}

void Manager::fillCompanionMessageMapping(
    Companion* companionPtr, bool containersAlreadyHaveMessages) {
    uint8_t companionId = companionPtr->getId();

    // get messages data
    std::shared_ptr<DBReplyData> messagesDataPtr = this->getDBDataPtr(
        logDBInteraction,
        "getMessagesDBResult",
        &getMessagesDBResult,
        buildStringVector(
            "id", "companion_id", "author_id",
            "timestamp_tz", "message", "is_sent", "is_received"),
        companionId);

    if(!messagesDataPtr) {
        showErrorDialogAndLogError(nullptr, "Error getting data from db");
        // return false;
        return;
    }

    if(messagesDataPtr->isEmpty()) {
        logArgsWarningWithTemplate(
            "no messages in db with companion {}", companionPtr->getName());

        // return false;
        return;
    }

    for(size_t i = 0; i < messagesDataPtr->size(); i++) {  // TODO switch to iterators
        auto messageId = std::atoi(messagesDataPtr->getValue(i, "id"));

        if(containersAlreadyHaveMessages) {
            auto pairPtr = companionPtr->getMessageMappingPairPtrByMessageId(messageId);

            if(pairPtr && pairPtr->second.getStatePtr()) {
                // companionPtr->addMessage(const_cast<Message*>(pair.second));
            }
            else {
                companionPtr->createMessageAndAddToMapping(messagesDataPtr, i);
            }
        }
        else {
            companionPtr->createMessageAndAddToMapping(messagesDataPtr, i);
        }
    }
}

bool Manager::connectToDb() {
    bool connected = false;
    this->dbConnectionPtr_ = getDBConnection();

    if(!this->dbConnectionPtr_) {
        return connected;
    }

    ConnStatusType status = PQstatus(dbConnectionPtr_);
    // logArgs("DB connection status: ", status);

    if(status == ConnStatusType::CONNECTION_OK) {  // TODO raise exception
        connected = true;
    }

    return connected;
}

bool Manager::buildCompanions() {
    bool companionsDataIsOk = true;

    // get companion data
    std::shared_ptr<DBReplyData> companionsDataPtr = this->getDBDataPtr(
        logDBInteraction,
        "getCompanionsDBResult",
        &getCompanionsDBResult,
        buildStringVector("id", "name"));

    if(!companionsDataPtr) {
        return false;
    }

    // std::sort(
    //     companionsDataPtr->getDataPtr()->begin(),
    //     companionsDataPtr->getDataPtr()->end(),
    //     [&](auto& iterator1, auto& iterator2)
    //     {
    //         return iterator1.at("id") < iterator2.at("id");
    //     }
    // );

    for(size_t index = 0; index < companionsDataPtr->size(); index++) {  // TODO switch to iterators
        int id = std::atoi(companionsDataPtr->getValue(index, "id"));

        // create companion object
        Companion* companionPtr = this->addCompanionObject(
            id,
            std::string(companionsDataPtr->getValue(index, "name")));

        if(!companionPtr) {
            logArgsError("companionPtr is nullptr");
            continue;
        }

        // get socket data object
        std::shared_ptr<DBReplyData> socketsDataPtr = this->getDBDataPtr(
            logDBInteraction,
            "getSocketInfoDBResult",
            &getSocketInfoDBResult,
            buildStringVector("ipaddress", "server_port", "client_port"),
            id);

        if(socketsDataPtr->size() > 0) {
            // TODO use port number pool
            SocketInfo* socketInfoPtr = new SocketInfo(
                socketsDataPtr->getValue(0, "ipaddress"),
                std::atoi(socketsDataPtr->getValue(0, "server_port")),
                std::atoi(socketsDataPtr->getValue(0, "client_port")));

            companionPtr->setSocketInfo(socketInfoPtr);
        }

        if(companionPtr->getId() > 1) {  // TODO change condition
            this->fillCompanionMessageMapping(companionPtr, false);

            if(!companionPtr->startServer()) {
                logArgsError("problem with server start for companion id", id);
            }

            if(!companionPtr->createClient()) {
                logArgsError("problem with client creation for companion id", id);
            }
        }
    }

    return companionsDataIsOk;
}

void Manager::buildWidgetGroups() {
    GraphicManager* graphicManagerPtr = getGraphicManagerPtr();

    auto companionsNumber = this->mapCompanionIdToCompanionInfo_.size();
    auto childrenSize = graphicManagerPtr->getCompanionPanelChildrenSize();

    logArgsWithTemplate(
        "companionsNumber: {0}, childrenSize: {1}", companionsNumber, childrenSize);

    if(companionsNumber == 0 && childrenSize == 0) {
        logArgsWarning("strange case, empty sockets panel");
    }
    else {
        // TODO check if sockets already are children

        // hide companion panel stub widget
        graphicManagerPtr->hideCompanionPanelStub();

        for(auto& pair : this->mapCompanionIdToCompanionInfo_) {
            this->createWidgetGroupAndAddToMapping(pair.second.first);
        }
    }
}

Companion* Manager::addCompanionObject(int id, const std::string& name) {
    if(id == 0) {
        logArgsError("companion id == 0");
        return nullptr;
    }

    auto result = this->mapCompanionIdToCompanionInfo_.emplace(
        std::make_pair(
            id,
            std::pair<Companion*, WidgetGroup*>(new Companion(id, name), nullptr)));

    return (result.second) ? result.first->second.first : nullptr;
}

void Manager::createWidgetGroupAndAddToMapping(Companion* companionPtr) {
    WidgetGroup* widgetGroupPtr = new WidgetGroup(companionPtr);

    widgetGroupPtr->set();

    this->mapCompanionIdToCompanionInfo_[companionPtr->getId()].second =
        widgetGroupPtr;

    companionPtr->addMessageWidgetsToChatHistory();
}

void Manager::deleteCompanionObject(Companion* companionPtr) {
    this->deleteWidgetGroupAndDeleteFromMapping(companionPtr);
}

void Manager::deleteWidgetGroupAndDeleteFromMapping(const Companion* companionPtr) {
    auto findMapLambda = [&](auto& iterator){
        return iterator.second.first == companionPtr;
    };

    auto findMapResult = std::find_if(
        this->mapCompanionIdToCompanionInfo_.begin(),
        this->mapCompanionIdToCompanionInfo_.end(),
        findMapLambda);

    if(findMapResult == this->mapCompanionIdToCompanionInfo_.end()) {
        showErrorDialogAndLogError(
            nullptr, "Companion was not found in mapping at deletion");
    }
    else {
        if(this->selectedCompanionPtr_ == companionPtr) {
            this->selectedCompanionPtr_ = nullptr;
        }

        // delete companion
        delete findMapResult->second.first;

        // delete widget group
        delete findMapResult->second.second;

        this->mapCompanionIdToCompanionInfo_.erase(findMapResult);
    }
}

bool Manager::companionDataValidation(CompanionAction* companionActionPtr) {
    std::vector<std::string> validationErrors {};

    bool validationResult = validateCompanionData(validationErrors, companionActionPtr);

    if(!validationResult) {
        showErrorDialogAndLogError(
            nullptr,
            getQString(
                buildDialogText(std::string { "Error messages:\n\n" }, validationErrors)));

        return false;
    }

    return true;
}

bool Manager::passwordDataValidation(PasswordAction* passwordActionPtr) {
    std::vector<std::string> validationErrors {};

    bool validationResult = validatePassword(
        validationErrors, passwordActionPtr->getPassword());

    if(!validationResult) {
        showErrorDialogAndLogError(
            nullptr,
            getQString(
                buildDialogText(std::string { "Error messages:\n\n" }, validationErrors)));

        return false;
    }

    return true;
}

bool Manager::checkCompanionDataForExistanceAtCreation(CompanionAction* companionActionPtr) {
    // check if companion with such name already exists
    std::shared_ptr<DBReplyData> companionIdDataPtr = this->getDBDataPtr(
        logDBInteraction,
        "getCompanionByNameDBResult",
        &getCompanionByNameDBResult,
        buildStringVector("id"),
        companionActionPtr->getName());

    if(!companionIdDataPtr) {
        showErrorDialogAndLogError(nullptr, "Error getting data from db");
        return false;
    }

    if(!companionIdDataPtr->isEmpty()) {
        showErrorDialogAndLogError(nullptr, "Companion with such name already exists");
        return false;
    }

    // check if such socket already exists
    std::shared_ptr<DBReplyData> socketIdDataPtr = this->getDBDataPtr(
        logDBInteraction,
        "getSocketByIpAddressAndPortDBResult",
        &getSocketByIpAddressAndPortDBResult,
        buildStringVector("id"),
        companionActionPtr->getIpAddress(),
        companionActionPtr->getClientPort());

    if(!socketIdDataPtr) {
        showErrorDialogAndLogError(nullptr, "Error getting data from db");
        return false;
    }

    if(!socketIdDataPtr->isEmpty()) {
        showErrorDialogAndLogError(nullptr, "Companion with such socket already exists");
        return false;
    }

    return true;
}

bool Manager::checkCompanionDataForExistanceAtUpdate(CompanionAction* companionActionPtr) {
    // check if companion with such name already exists
    std::shared_ptr<DBReplyData> companionIdDataPtr = this->getDBDataPtr(
        logDBInteraction,
        "getCompanionByNameDBResult",
        &getCompanionByNameDBResult,
        buildStringVector("id"),
        companionActionPtr->getName());

    if(!companionIdDataPtr) {
        showErrorDialogAndLogError(nullptr, "Error getting data from db");
        return false;
    }

    bool findNameResult = companionIdDataPtr->findValue(
        std::string("id"),
        std::to_string(companionActionPtr->getCompanionId()));

    bool nameExistsAtOtherCompanion =
        (findNameResult && companionIdDataPtr->size() > 1) ||
        (!findNameResult && companionIdDataPtr->size() > 0);

    if(nameExistsAtOtherCompanion) {
        // no return
        showWarningDialogAndLogWarning(nullptr, "Companion with such name already exists");
    }

    // check if such socket already exists
    std::shared_ptr<DBReplyData> socketIdDataPtr = this->getDBDataPtr(
        logDBInteraction,
        "getSocketByIpAddressAndPortDBResult",
        &getSocketByIpAddressAndPortDBResult,
        buildStringVector("id"),
        companionActionPtr->getIpAddress(),
        companionActionPtr->getClientPort());

    if(!socketIdDataPtr) {
        showErrorDialogAndLogError(nullptr, "Error getting data from db");
        return false;
    }

    bool findSocketResult = socketIdDataPtr->findValue(
        std::string("id"),
        std::to_string(companionActionPtr->getCompanionId()));

    bool socketExistsAtOtherCompanion =
        (findSocketResult && socketIdDataPtr->size() > 1) ||
        (!findSocketResult && socketIdDataPtr->size() > 0);

    if(socketExistsAtOtherCompanion) {
        showErrorDialogAndLogError(nullptr, "Companion with such socket already exists");
        return false;
    }

    return true;
}

void Manager::waitForMessageReceptionConfirmation(
    Companion* companionPtr, MessageState* messageStatePtr, const Message* messagePtr) {
    auto lambda = [=](){
        uint32_t sleepDuration = sleepDurationInitial;

        sleepForMilliseconds(sleepDuration);

        while(true) {
            if(messageStatePtr->getIsReceived()) {
                return;
            }
            else {
                // send message reception confirmation request
                bool result = companionPtr->sendMessage(
                    false, NetworkMessageType::RECEIVE_CONFIRMATION_REQUEST,
                    messageStatePtr->getNetworkId(), messagePtr);

                // sleep
                sleepForMilliseconds(sleepDuration);
                sleepDuration *= sleepDurationIncreaseRate;
            }
        }
    };

    std::thread(lambda).detach();
}

bool Manager::markMessageAsSent(Companion* companionPtr, const Message* messagePtr) {
    // mark in db
    std::shared_ptr<DBReplyData> messageIdDataPtr = this->getDBDataPtr(
        logDBInteraction,
        "setMessageInDbAndReturn",
        &setMessageIsSentInDbAndReturn,
        buildStringVector("id"),
        messagePtr->getId());

    if(!messageIdDataPtr) {
        showErrorDialogAndLogError(nullptr, "Error updating data in db");
        return false;
    }

    // mark in widget
    getGraphicManagerPtr()->markMessageWidgetAsSent(companionPtr, messagePtr);

    return true;
}

bool Manager::markMessageAsReceived(Companion* companionPtr, const Message* messagePtr) {
    // mark in widget
    getGraphicManagerPtr()->markMessageWidgetAsReceived(companionPtr, messagePtr);

    // mark in db
    std::shared_ptr<DBReplyData> messageIdDataPtr = this->getDBDataPtr(
        logDBInteraction,
        "setMessageIsReceivedInDbAndReturn",
        &setMessageIsReceivedInDbAndReturn,
        buildStringVector("id"),
        messagePtr->getId());

    if(!messageIdDataPtr) {
        showErrorDialogAndLogError(nullptr, "Error getting data from db");
        return false;
    }

    if(messageIdDataPtr->isEmpty()) {
        showErrorDialogAndLogError(nullptr, "Error setting message is_received in Db");
        return false;
    }

    return true;
}

std::tuple<uint32_t, uint8_t, std::string> Manager::pushMessageToDB(
    const std::string& companionName, const std::string& authorName,
    const std::string& timestamp, const std::string& text,
    const bool& isSent, const bool& isReceived) {

    const std::string companionIdString("companion_id");

    std::shared_ptr<DBReplyData> messageDataPtr = this->getDBDataPtr(
        logDBInteraction,
        "pushMessageToDBAndReturn",
        &pushMessageToDBAndReturn,
        buildStringVector("id", "companion_id", "timestamp_tz"),
        companionName, authorName, timestamp, companionIdString, text,
        isSent, isReceived);

    if(!messageDataPtr) {
        showErrorDialogAndLogError(nullptr, "Error getting data from db");
        return std::tuple<uint32_t, uint8_t, std::string>(0, 0, "");
    }

    if(messageDataPtr->isEmpty()) {
        logArgsError("messageDataPtr->isEmpty()");
        return std::tuple<uint32_t, uint8_t, std::string>(0, 0, "");
    }

    uint32_t id = std::atoi(messageDataPtr->getValue(0, "id"));
    uint8_t companionId = std::atoi(messageDataPtr->getValue(0, "companion_id"));
    std::string timestampTz { messageDataPtr->getValue(0, "timestamp_tz") };

    if(logDBInteraction) {
        logArgsWithTemplate("companionId: {0}, timestampTz: {1}", companionId, timestampTz);
    }

    return std::tuple<uint32_t, uint8_t, std::string>(id, companionId, timestampTz);
}

Manager* getManagerPtr() {
    QCoreApplication* coreApp = QCoreApplication::instance();
    ChatApp* app = dynamic_cast<ChatApp*>(coreApp);
    return app->managerPtr_;
}
