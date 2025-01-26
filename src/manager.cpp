#include "manager.hpp"

Manager::Manager() :
    messageStateToMessageMapMutex_(std::mutex()),
    dbConnectionPtr_(nullptr), userIsAuthenticated_(false)
    // companionPtrs_(std::vector<Companion*>())
{
    // mapCompanionToWidgetGroup_ = std::map<const Companion*, WidgetGroup*>();
    mapCompanionToWidgetGroup_ = std::map<Companion, WidgetGroup*>();
    selectedCompanionPtr_ = nullptr;
    // mapMessageStateToMessage_ = std::map<const MessageState*, const Message*>();
}

Manager::~Manager()
{
    free(dbConnectionPtr_);

    // for(auto& companion : this->companionPtrs_)
    for(auto& pair : this->mapCompanionToWidgetGroup_)
    {
        // delete companion;        
        delete &(pair.first);
        delete pair.second;
    }
}

const Companion* Manager::getSelectedCompanionPtr()
{
    return this->selectedCompanionPtr_;
}

bool Manager::getUserIsAuthenticated()
{
    return this->userIsAuthenticated_;
}

const Companion* Manager::getMappedCompanionBySocketInfoBaseWidget(
    SocketInfoBaseWidget* widget) const
{
    auto findWidget = [&](auto& pair)
    {
        return pair.second->getSocketInfoBasePtr() == widget;
    };

    auto result = std::find_if(
        this->mapCompanionToWidgetGroup_.cbegin(),
        this->mapCompanionToWidgetGroup_.cend(),
        findWidget);

    // return result->first;
    return &(result->first);
}

WidgetGroup* Manager::getMappedWidgetGroupByCompanion(const Companion* companionPtr) const
{
    WidgetGroup* groupPtr = nullptr;

    try
    {
        // groupPtr = this->mapCompanionToWidgetGroup_.at(companionPtr);
        groupPtr = this->mapCompanionToWidgetGroup_.at(*companionPtr);
    }
    catch(std::out_of_range) {}

    return groupPtr;
}

// const MessageState* Manager::getMappedMessageStateByMessagePtr(const Message* messagePtr)
// {
//     std::lock_guard<std::mutex> lock(this->messageStateToMessageMapMutex_);

//     auto result = std::find_if(
//         this->mapMessageStateToMessage_.begin(),
//         this->mapMessageStateToMessage_.end(),
//         [&](auto iter)
//         {
//             return iter.second == messagePtr;
//         });

//     return (result == this->mapMessageStateToMessage_.end()) ? nullptr : result->first;
// }

void Manager::set()
{
    bool connectedToDB = this->connectToDb();
    // logArgs("connectedToDB:", connectedToDB);

    if(connectedToDB)
    {
        bool companionsBuilt = this->buildCompanions();
        logArgs("companionsBuilt:", companionsBuilt);

        if(companionsBuilt)  // TODO rewrite
        {
            this->buildWidgetGroups();
        }
        else
        {
            logArgsError("problem with companions initialization");
        }
    }
    else
    {
        showErrorDialogAndLogError(nullptr, "problem with DB connection");
    }
}

void Manager::sendMessage(Companion* companionPtr, const std::string& text)
{
    // WidgetGroup* groupPtr = this->mapCompanionToWidgetGroup_.at(companionPtr);
    WidgetGroup* groupPtr = this->mapCompanionToWidgetGroup_.at(*companionPtr);

    // encrypt message

    // add to DB and get timestamp
    auto tuple = this->pushMessageToDB(
        companionPtr->getName(), std::string("me"), std::string("now()"),
        text, false, false);

    uint32_t id = std::get<0>(tuple);
    uint8_t companion_id = std::get<1>(tuple);
    std::string timestamp = std::get<2>(tuple);

    if(companion_id == 0 || timestamp == "")
    {
        logArgsError("error adding message to db");
        return;
    }

    // // add to companion's messages
    // Message* messagePtr = new Message(id, companion_id, 1, timestamp, text);
    // // companionPtr->addMessage(messagePtr);

    // // create message state object and add to mapping
    // MessageState* messageStatePtr = new MessageState(
    //     companion_id, false, false, false, "");

    // bool addResult = this->addToMessageStateToMessageMapping(
    //     messageStatePtr, messagePtr);

    // logArgs("addResult:", addResult);

    auto pair = companionPtr->createMessageAndAddToMapping(
        id, 1, timestamp, text, false, false, false, "");

    if(pair.second)
    {
        // add to widget
        // groupPtr->addMessageWidgetToCentralPanelChatHistory(messagePtr);

        const Message* messagePtr = &(pair.first->first);
        MessageState* messageStatePtr = pair.first->second.getStatePtr();

        groupPtr->addMessageWidgetToCentralPanelChatHistory(
            groupPtr, messagePtr, messageStatePtr);

        // add to mapping
        // std::string networkId = this->addToNetworkIdToMessageMapping(messagePtr);

        // logArgsWithCustomMark("send:", networkId);

        // send over network
        bool result = companionPtr->sendMessage(
            false, NetworkMessageType::SEND_DATA,
            messageStatePtr->getNetworkId(), messagePtr);

        // mark message as sent
        if(result)
        {
            this->markMessageAsSent(companionPtr, messagePtr);
        }

        // wait for message reception confirmation
        this->waitForMessageReceptionConfirmation(companionPtr, messageStatePtr, messagePtr);
    }
}

void Manager::receiveMessage(Companion* companionPtr, const std::string& jsonString)
{
    nlohmann::json jsonData = buildJsonObject(jsonString);

    NetworkMessageType type;
    std::string networkId;
    bool isAntecedent;

    runAndLogException(
        [&]()
        {
            type = jsonData.at("type");
            networkId = jsonData.at("id");
            isAntecedent = jsonData.at("antecedent");
        });

    // logArgsWithCustomMark("received:", networkId);

    switch(type)
    {
    // message is data message
    case NetworkMessageType::SEND_DATA:
        {
            auto timestamp = jsonData.at("time");
            auto text = jsonData.at("text");

            // add to DB and get timestamp
            auto tuple = this->pushMessageToDB(
                companionPtr->getName(), companionPtr->getName(),
                timestamp, text, false, true);

            uint32_t id = std::get<0>(tuple);
            uint8_t companion_id = std::get<1>(tuple);

            if(companion_id == 0 || timestamp == "")
            {
                logArgsError("error adding message to db");
                return;
            }

            // // add to companion's messages
            // Message* messagePtr = new Message(
            //     id, companion_id, companion_id, timestamp, text);

            // // companionPtr->addMessage(messagePtr);

            // // create message state object and add to mapping
            // MessageState* messageStatePtr = new MessageState(
            //     companion_id, isAntecedent, false, true, networkId);

            // this->addToMessageStateToMessageMapping(messageStatePtr, messagePtr);

            auto pair = companionPtr->createMessageAndAddToMapping(
                id, companion_id, timestamp, text, isAntecedent, false, true, networkId);

            if(pair.second)
            {
                const Message* messagePtr = &(pair.first->first);
                MessageState* messageStatePtr = pair.first->second.getStatePtr();

                // decrypt message

                // add to widget
                // WidgetGroup* groupPtr = this->mapCompanionToWidgetGroup_.at(companionPtr);  // TODO try catch
                WidgetGroup* groupPtr = this->mapCompanionToWidgetGroup_.at(*companionPtr);  // TODO try catch

                groupPtr->addMessageWidgetToCentralPanelChatHistoryFromThread(
                    messageStatePtr, messagePtr);

                // send reception confirmation to sender
                bool result = companionPtr->sendMessage(
                    false, NetworkMessageType::RECEIVE_CONFIRMATION, networkId, messagePtr);
            }
        }

        break;

    // message is confirmation
    case NetworkMessageType::RECEIVE_CONFIRMATION:
        {
            auto received = jsonData.at("received");

            if(received == 1)  // successfully received
            {                
                // mark message as received
                std::string key = generateMessageMappingKey(
                    networkId, companionPtr->getId());

                // auto pair = this->getMessageStateAndMessageMappingPairByMessageMappingKey(key);
                auto pairPtr = companionPtr->getMessageMappingPairPtrByMessageMappingKey(key);

                // TODO rewrite
                // if(pair.first && pair.second)
                if(pairPtr && pairPtr->second.getStatePtr())
                {
                    // found message in mapping
                    // const_cast<MessageState*>(pair.first)->setIsReceived(true);
                    pairPtr->second.getStatePtr()->setIsReceived(true);
                    // this->markMessageAsReceived(pair.second);
                    this->markMessageAsReceived(companionPtr, &(pairPtr->first));
                }
                else
                {
                    // strange situation
                    logArgsError(
                        // "received confirmation for message not in mapMessageStateToMessage_");
                        "received confirmation for message which is not in messageMapping_");
                }
            }
            else
            {
                // message was not received, resend message
            }
        }

        break;

    case NetworkMessageType::RECEIVE_CONFIRMATION_REQUEST:
        {
            // search for message in managers's mapping
            std::string key = generateMessageMappingKey(networkId, companionPtr->getId());

            // auto pair = this->getMessageStateAndMessageMappingPairByMessageMappingKey(key);
            auto pairPtr = companionPtr->getMessageMappingPairPtrByMessageMappingKey(key);

            logArgs("pair:", pairPtr, "pair.second.getStatePtr():", pairPtr->second.getStatePtr());

            // if(pair.first && pair.second)
            if(pairPtr && pairPtr->second.getStatePtr())
            {
                // message found in managers's mapping
                // if(pair.first->getIsReceived())
                if(pairPtr->second.getStatePtr()->getIsReceived())
                {
                    bool result = companionPtr->sendMessage(
                        false, NetworkMessageType::RECEIVE_CONFIRMATION,
                        // pair.first->getNetworkId(), pair.second);
                        pairPtr->second.getStatePtr()->getNetworkId(), &(pairPtr->first));
                }
                else
                {
                    // strange situation
                    logArgsError(
                        "received reception confirmation request "
                        // "for message in mapMessageStateToMessage_ with isReceived = false");
                        "for message in messageMapping_ with isReceived = false");
                }
            }
            else
            {
                // probably old message from previous sessions, search for it in db
                logArgsInfo("probably old message from previous sessions, search for it in db");
            }
        }

        break;

    case NetworkMessageType::HISTORY_REQUEST:
        {
            logArgsInfo("got history request from " + companionPtr->getName());

            this->getMappedWidgetGroupByCompanion(companionPtr)->
                askUserForHistorySendingConfirmationFromThread();
        }

        break;

    case NetworkMessageType::CHAT_HISTORY_DATA:
        {
            logArgsInfo("got chat history from " + companionPtr->getName());
            logArgs("jsonString:", jsonString);

            auto json = buildJsonObject(jsonString);

            for(size_t i = 0; i < json["messages"].size(); i++)
            {
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

                if(!messageGetDataPtr)
                {
                    showErrorDialogAndLogError(nullptr, "Error getting data from db");
                    return;
                }

                if(!messageGetDataPtr->isEmpty())
                {
                    showInfoDialogAndLogInfo(
                        nullptr,
                        QString(
                            "Message with timestamp %1 from companion "
                            "with id %2 already exists")
                            .arg(
                                getQString(timestamp),
                                getQString(std::to_string(companionId))));

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

                if(!messageAddDataPtr)
                {
                    showErrorDialogAndLogError(nullptr, "Error getting data from db");
                    return;
                }

                if(messageAddDataPtr->isEmpty())
                {
                    showErrorDialogAndLogError(nullptr, "Error pushing chat history to db");
                    return;
                }
            }

            // clear chat history widget
            this->clearChatHistory(companionPtr);

            // fill continers with messages
            this->fillWithMessages(companionPtr, true);

            // build chat history
            emit this->getMappedWidgetGroupByCompanion(companionPtr)->
                buildChatHistorySignal();
        }

        break;
    }
}

void Manager::resetSelectedCompanion(const Companion* newSelected)
{
    GraphicManager* graphicManagerPtr = getGraphicManagerPtr();

    if(this->selectedCompanionPtr_)
    {
        // auto widgetGroup = this->mapCompanionToWidgetGroup_.at(this->selectedCompanionPtr_);  // TODO try catch
        auto widgetGroup = this->mapCompanionToWidgetGroup_.at(*(this->selectedCompanionPtr_));  // TODO try catch

        dynamic_cast<SocketInfoWidget*>(widgetGroup->getSocketInfoBasePtr())->unselect();

        widgetGroup->hideCentralPanel();
    }
    else
    {
        graphicManagerPtr->hideCentralPanelStub();
    }

    this->selectedCompanionPtr_ = newSelected;

    if(this->selectedCompanionPtr_)
    {
        // auto widgetGroup = this->mapCompanionToWidgetGroup_.at(this->selectedCompanionPtr_);
        auto widgetGroup = this->mapCompanionToWidgetGroup_.at(*(this->selectedCompanionPtr_));

        dynamic_cast<SocketInfoWidget*>(widgetGroup->getSocketInfoBasePtr())->select();

        widgetGroup->showCentralPanel();
    }
    else
    {
        graphicManagerPtr->showCentralPanelStub();
    }
}

void Manager::createCompanion(CompanionAction* companionActionPtr)
{
    // data validation and checking
    if(!(companionDataValidation(companionActionPtr) &&
          this->checkCompanionDataForExistanceAtCreation(companionActionPtr)))
    {
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

    if(!companionIdDataPtr)
    {
        showErrorDialogAndLogError(nullptr, "Error getting data from db");
        return;
    }

    if(companionIdDataPtr->isEmpty())
    {
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

    if(!socketDataPtr)
    {
        showErrorDialogAndLogError(nullptr, "Error getting data from db");
        return;
    }

    if(socketDataPtr->isEmpty())
    {
        showErrorDialogAndLogError(nullptr, "Empty db reply to new socket pushing");
        return;
    }

    // create Companion object
    Companion* companionPtr = this->addCompanionObject(id, name);

    if(!companionPtr)
    {
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

void Manager::updateCompanion(CompanionAction* companionActionPtr)
{
    // data validation and checking
    if(!(companionDataValidation(companionActionPtr) &&
          this->checkCompanionDataForExistanceAtUpdate(companionActionPtr)))
    {
        return;
    }

    // update companion data at db
    std::shared_ptr<DBReplyData> companionIdDataPtr = this->getDBDataPtr(
        logDBInteraction,
        "updateCompanionAndSocketAndReturn",
        &updateCompanionAndSocketAndReturn,
        buildStringVector("id"),
        *companionActionPtr);

    if(!companionIdDataPtr)
    {
        showErrorDialogAndLogError(nullptr, "Error getting data from db");
        return;
    }

    if(companionIdDataPtr->isEmpty())
    {
        showErrorDialogAndLogError(nullptr, "Empty db reply to companion update");
        return;
    }

    // update Companion and SocketInfo object
    companionActionPtr->updateCompanionObjectData();

    // update SocketInfoWidget
    // auto widgetGroup = this->mapCompanionToWidgetGroup_.at(
    //     companionActionPtr->getCompanionPtr());  // TODO try catch
    auto widgetGroup = this->mapCompanionToWidgetGroup_.at(
        *(companionActionPtr->getCompanionPtr()));  // TODO try catch

    dynamic_cast<SocketInfoWidget*>(widgetGroup->getSocketInfoBasePtr())->update();

    // show info dialog
    getGraphicManagerPtr()->showCompanionInfoDialog(
        companionActionPtr, std::string { "Companion updated:\n\n" });
}

void Manager::deleteCompanion(CompanionAction* companionActionPtr)
{
    // delete companion chat messages from db
    std::shared_ptr<DBReplyData> companionIdMessagesDataPtr = this->getDBDataPtr(
        logDBInteraction,
        "deleteMessagesFromDBAndReturn",
        &deleteMessagesFromDBAndReturn,
        buildStringVector("id"),
        *companionActionPtr);

    if(!companionIdMessagesDataPtr)
    {
        showErrorDialogAndLogError(nullptr, "Error getting data from db");
        return;
    }

    if(companionIdMessagesDataPtr->isEmpty())
    {
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

    if(!companionIdCompanionDataPtr)
    {
        showErrorDialogAndLogError(nullptr, "Error getting data from db");
        return;
    }

    if(companionIdCompanionDataPtr->isEmpty())
    {
        showErrorDialogAndLogError(nullptr, "Empty db reply to companion deletion");
        return;
    }

    // delete companion object
    this->deleteCompanionObject(companionActionPtr->getCompanionPtr());

    // show info dialog
    getGraphicManagerPtr()->showCompanionInfoDialog(
        companionActionPtr, std::string { "Companion deleted:\n\n" });
}

void Manager::clearChatHistory(Companion* companionPtr)
{
    // WidgetGroup* widgetGroupPtr = this->mapCompanionToWidgetGroup_.at(companionPtr);
    WidgetGroup* widgetGroupPtr = this->mapCompanionToWidgetGroup_.at(*(companionPtr));
    getGraphicManagerPtr()->clearChatHistory(widgetGroupPtr);
}

void Manager::clearCompanionHistory(CompanionAction* companionActionPtr)
{
    // delete companion chat messages from db
    std::shared_ptr<DBReplyData> companionIdMessagesDataPtr = this->getDBDataPtr(
        logDBInteraction,
        "deleteMessagesFromDBAndReturn",
        &deleteMessagesFromDBAndReturn,
        buildStringVector("companion_id"),
        *companionActionPtr);

    if(!companionIdMessagesDataPtr)
    {
        showErrorDialogAndLogError(nullptr, "Error getting data from db");
        return;
    }

    if(companionIdMessagesDataPtr->isEmpty())
    {
        // no return, may be companion without messages
        // showWarningDialogAndLogWarning("Empty db reply to companion messages deletion");
    }

    // clear chat history widget
    // WidgetGroup* widgetGroupPtr =
    //     this->mapCompanionToWidgetGroup_.at(companionActionPtr->getCompanionPtr());

    // getGraphicManagerPtr()->clearChatHistory(widgetGroupPtr);
    this->clearChatHistory(companionActionPtr->getCompanionPtr());

    // show info dialog
    getGraphicManagerPtr()->showCompanionInfoDialog(
        companionActionPtr, std::string { "Companion chat history cleared:\n\n" });
}

void Manager::createUserPassword(PasswordAction* actionPtr)
{
    // data validation and checking
    if(!(this->passwordDataValidation(actionPtr)))
    {
        return;
    }

    // push password data to db
    std::shared_ptr<DBReplyData> passwordIdDataPtr = this->getDBDataPtr(
        logDBInteraction,
        "pushPasswordToDBAndReturn",
        &pushPasswordToDBAndReturn,
        buildStringVector("id"),
        actionPtr->getPassword());

    if(!passwordIdDataPtr)
    {
        showErrorDialogAndLogError(nullptr, "Error getting data from db");
        return;
    }

    if(passwordIdDataPtr->isEmpty())
    {
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

void Manager::authenticateUser(PasswordAction* actionPtr)
{
    GraphicManager* graphicManagerPtr = getGraphicManagerPtr();

    // do we have password in db?
    std::shared_ptr<DBReplyData> passwordDataPtr = this->getDBDataPtr(
        logDBInteraction,
        "getPasswordDBResult",
        &getPasswordDBResult,
        buildStringVector("id", "password"));

    if(!passwordDataPtr)
    {
        showErrorDialogAndLogError(nullptr, "Error getting password from db");
        return;
    }

    if(passwordDataPtr->isEmpty())
    {
        showErrorDialogAndLogError(nullptr, "Db password data is empty");
        return;
    }
    else
    {
        if(passwordDataPtr->getValue(0, "password") == actionPtr->getPassword())
        {
            this->userIsAuthenticated_ = true;

            logArgsInfo("user successfully authenticated");
            graphicManagerPtr->disableMainWindowBlurEffect();

            // dialog is closed in action dtor
            delete actionPtr;
        }
        else
        {
            showErrorDialogAndLogError(nullptr, "Password is not correct");
            return;
        }
    }
}

// void Manager::createMessageAndAddToContainers(
//     Companion* companionPtr, std::shared_ptr<DBReplyData>& messagesDataPtr, size_t index)
// {
//     auto companionId = companionPtr->getId();

//     Message* messagePtr = new Message(
//         std::atoi(messagesDataPtr->getValue(index, "id")),
//         companionId,
//         std::atoi(messagesDataPtr->getValue(index, "author_id")),
//         messagesDataPtr->getValue(index, "timestamp_tz"),
//         messagesDataPtr->getValue(index, "message"));

//     // companionPtr->addMessage(messagePtr);

//     MessageState* messageStatePtr = new MessageState(
//         companionId, false,
//         messagesDataPtr->getValue(index, "is_sent"),
//         messagesDataPtr->getValue(index, "is_received"), "");

//     this->addToMessageStateToMessageMapping(messageStatePtr, messagePtr);
// }

void Manager::hideSelectedCompanionCentralPanel()
{
    if(this->selectedCompanionPtr_)
    {
        // auto groupPtr = this->mapCompanionToWidgetGroup_.at(this->selectedCompanionPtr_);
        auto groupPtr = this->mapCompanionToWidgetGroup_.at(*(this->selectedCompanionPtr_));
        getGraphicManagerPtr()->hideWidgetGroupCentralPanel(groupPtr);
    }
}

void Manager::showSelectedCompanionCentralPanel()
{
    if(this->selectedCompanionPtr_)
    {
        // auto groupPtr = this->mapCompanionToWidgetGroup_.at(this->selectedCompanionPtr_);
        auto groupPtr = this->mapCompanionToWidgetGroup_.at(*(this->selectedCompanionPtr_));
        getGraphicManagerPtr()->showWidgetGroupCentralPanel(groupPtr);
    }
}

void Manager::startUserAuthentication()
{
    GraphicManager* graphicManagerPtr = getGraphicManagerPtr();

    graphicManagerPtr->enableMainWindowBlurEffect();

    // do we have password in db?
    std::shared_ptr<DBReplyData> passwordDataPtr = this->getDBDataPtr(
        logDBInteraction,
        "getPasswordDBResult",
        &getPasswordDBResult,
        buildStringVector("id", "password"));

    if(!passwordDataPtr)
    {
        showErrorDialogAndLogError(nullptr, "Error getting password from db");
        return;
    }

    if(passwordDataPtr->isEmpty())
    {
        graphicManagerPtr->createEntrancePassword();
    }
    else
    {
        graphicManagerPtr->getEntrancePassword();
    }
}

void Manager::sendUnsentMessages(const Companion* companionPtr)
{
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

    if(!messagesDataPtr)
    {
        showErrorDialogAndLogError(nullptr, "Error getting data from db");
        return;
    }

    if(messagesDataPtr->isEmpty())
    {
        logArgsInfo("Empty db reply to unsent messages selection");
        return;
    }

    for(size_t i = 0; i < messagesDataPtr->size(); i++)  // TODO switch to iterators
    {
        uint32_t messageId = std::atoi(messagesDataPtr->getValue(i, "id"));
        Message* messagePtr = companionCastPtr->findMessage(messageId);
        std::string networkId;

        if(messagePtr)
        {
            // const MessageState* messageStatePtr =
            //     this->getMappedMessageStateByMessagePtr(messagePtr);
            const MessageState* messageStatePtr =
                const_cast<Companion*>(companionPtr)->
                    getMappedMessageStateByMessagePtr(messagePtr);

            if(!messageStatePtr)
            {
                logArgsError(
                    "strange case: unsent message found in companions messages, "
                    // "but not found in manager's mapMessageStateToMessage_");
                    "but not found in companion's messageMapping_");
            }
            else
            {
                networkId = messageStatePtr->getNetworkId();
            }
        }
        else
        {
            // add to companion's messages if needed
            // uint8_t companion_id = std::atoi(messagesDataPtr->getValue(i, "companion_id"));
            networkId = getRandomString(5);

            // messagePtr = new Message(
            //     messageId,
            //     companion_id,
            //     1,
            //     messagesDataPtr->getValue(i, "timestamp_tz"),
            //     messagesDataPtr->getValue(i, "message"));

            // // companionCastPtr->addMessage(messagePtr);

            // MessageState* messageStatePtr = new MessageState(
            //     companion_id, false, false,
            //     messagesDataPtr->getValue(i, "is_received"), networkId);

            // this->addToMessageStateToMessageMapping(messageStatePtr, messagePtr);

            const_cast<Companion*>(companionPtr)->createMessageAndAddToMapping(
                messageId,
                1,
                messagesDataPtr->getValue(i, "timestamp_tz"),
                messagesDataPtr->getValue(i, "message"),
                false,
                false,
                messagesDataPtr->getValue(i, "is_received"),
                networkId);
        }

        // send over network
        bool result = companionCastPtr->sendMessage(
            true, NetworkMessageType::SEND_DATA, networkId, messagePtr);

        // mark message as sent
        if(result)
        {
            this->markMessageAsSent(const_cast<Companion*>(companionPtr), messagePtr);
        }
    }
}

void Manager::requestHistoryFromCompanion(const Companion* companionPtr)
{
    Companion* companionCastPtr = const_cast<Companion*>(companionPtr);

    bool result = companionCastPtr->sendMessage(
        true, NetworkMessageType::HISTORY_REQUEST, "", nullptr);
}

void Manager::sendChatHistoryToCompanion(const Companion* companionPtr)
{
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

    if(!messagesDataPtr)
    {
        showErrorDialogAndLogError(nullptr, "Error getting data from db");
        return;
    }

    if(messagesDataPtr->isEmpty())
    {
        logArgsInfo("Empty db reply to messages selection");
        return;
    }

    bool result = companionPtr->sendChatHistory(messagesDataPtr, keys);
}

const Companion* Manager::getMappedCompanionByWidgetGroup(
    WidgetGroup* groupPtr) const
{
    auto findWidget = [&](auto& pair)
    {
        return pair.second == groupPtr;
    };

    auto result = std::find_if(
        this->mapCompanionToWidgetGroup_.cbegin(),
        this->mapCompanionToWidgetGroup_.cend(),
        findWidget);

    return &(result->first);
}

// std::pair<const MessageState*, const Message*>
// Manager::getMessageStateAndMessageMappingPairByMessageMappingKey(const std::string& key)
// {
//     using pair = std::pair<const MessageState*, const Message*>;

//     std::lock_guard<std::mutex> lock(this->messageStateToMessageMapMutex_);

//     auto iterator = std::find_if(
//         this->mapMessageStateToMessage_.begin(),
//         this->mapMessageStateToMessage_.end(),
//         [&](auto iter)
//         {
//             return iter.first->getMessageMappingKey() == key;
//         });

//     return (iterator == this->mapMessageStateToMessage_.end()) ?
//         pair(nullptr, nullptr) : pair(iterator->first, iterator->second);
// }

// std::pair<const MessageState*, const Message*>
// Manager::getMessageStateAndMessageMappingPairByMessageId(uint32_t messageId)
// {
//     using pair = std::pair<const MessageState*, const Message*>;

//     std::lock_guard<std::mutex> lock(this->messageStateToMessageMapMutex_);

//     auto iterator = std::find_if(
//         this->mapMessageStateToMessage_.begin(),
//         this->mapMessageStateToMessage_.end(),
//         [&](auto iter)
//         {
//             return iter.second->getId() == messageId;
//         });

//     return (iterator == this->mapMessageStateToMessage_.end()) ?
//         pair(nullptr, nullptr) : pair(iterator->first, iterator->second);
// }

void Manager::fillWithMessages(Companion* companionPtr, bool containersAlreadyHaveMessages)
{
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

    if(!messagesDataPtr)
    {
        showErrorDialogAndLogError(nullptr, "Error getting data from db");
        // return false;
    }

    if(messagesDataPtr->isEmpty())
    {
        logArgsWarning(
            nullptr,
            QString("no messages in db with companion %1")
                .arg(getQString(companionPtr->getName())));

        // return false;
    }

    // if(containersAlreadyHaveMessages)
    // {
    //     companionPtr->clearMessages();
    // }

    for(size_t i = 0; i < messagesDataPtr->size(); i++)  // TODO switch to iterators
    {
        auto messageId = std::atoi(messagesDataPtr->getValue(i, "id"));

        if(containersAlreadyHaveMessages)
        {
            // auto pair = this->getMessageStateAndMessageMappingPairByMessageId(messageId);
            auto pairPtr = companionPtr->getMessageMappingPairPtrByMessageId(messageId);

            // if(pair.first && pair.second)
            if(pairPtr && pairPtr->second.getStatePtr())
            {
                // companionPtr->addMessage(const_cast<Message*>(pair.second));
            }
            else
            {
                // this->createMessageAndAddToContainers(companionPtr, messagesDataPtr, i);
                companionPtr->createMessageAndAddToMapping(messagesDataPtr, i);
            }
        }
        else
        {
            // this->createMessageAndAddToContainers(companionPtr, messagesDataPtr, i);
            companionPtr->createMessageAndAddToMapping(messagesDataPtr, i);
        }
    }
}

// bool Manager::addToMessageStateToMessageMapping(
//     const MessageState* messageStatePtr, const Message* messagePtr)
// {
//     std::lock_guard<std::mutex> lock(this->messageStateToMessageMapMutex_);

//     if(messagePtr->isMessageFromMe())
//     {
//         std::string networkId = getRandomString(5);

//         std::string messageMappingKey =
//             generateMessageMappingKey(
//             networkId, messagePtr->getCompanionId());

//         auto lambda = [&]()
//         {
//             auto iterator = std::find_if(
//                 this->mapMessageStateToMessage_.begin(),
//                 this->mapMessageStateToMessage_.end(),
//                 [&](auto iter)
//                 {
//                     return iter.first->getMessageMappingKey() ==
//                         messageMappingKey;
//                 });

//             return !(iterator == this->mapMessageStateToMessage_.end());
//         };

//         // run in loop while generated key is not unique
//         while(lambda())
//         {
//             networkId = getRandomString(5);

//             messageMappingKey =
//                 generateMessageMappingKey(
//                 networkId, messagePtr->getCompanionId());
//         }

//         MessageState* messageStateCastPtr = const_cast<MessageState*>(messageStatePtr);

//         messageStateCastPtr->setNetworkId(networkId);

//         messageStateCastPtr->setMessageMappingKey(
//             messageMappingKey);

//         this->mapMessageStateToMessage_[messageStatePtr] = messagePtr;

//         return true;
//     }
//     else
//     {
//         if(this->mapMessageStateToMessage_.count(messageStatePtr) == 0)
//         {
//             this->mapMessageStateToMessage_[messageStatePtr] = messagePtr;

//             return true;
//         }
//         else
//         {
//             logArgsError(
//                 QString("mapMessageStateToMessage_ already contains"
//                         "entry with key object having messageMappingKey_: %1")
//                     .arg(getQString(messageStatePtr->getMessageMappingKey())));

//             return false;
//         }
//     }
// }

bool Manager::connectToDb()
{
    bool connected = false;
    this->dbConnectionPtr_ = getDBConnection();

    if(!this->dbConnectionPtr_)
    {
        return connected;
    }

    ConnStatusType status = PQstatus(dbConnectionPtr_);
    // logArgs("DB connection status: ", status);

    if(status == ConnStatusType::CONNECTION_OK)  // TODO raise exception
    {
        connected = true;
    }

    return connected;
}

bool Manager::buildCompanions()
{
    bool companionsDataIsOk = true;

    // get companion data
    std::shared_ptr<DBReplyData> companionsDataPtr = this->getDBDataPtr(
        logDBInteraction,
        "getCompanionsDBResult",
        &getCompanionsDBResult,
        buildStringVector("id", "name"));

    if(!companionsDataPtr)
    {
        return false;
    }

    std::sort(
        companionsDataPtr->getDataPtr()->begin(),
        companionsDataPtr->getDataPtr()->end(),
        [&](auto& iterator1, auto& iterator2)
        {
            return iterator1.at("id") < iterator2.at("id");
        }
    );

    for(size_t index = 0; index < companionsDataPtr->size(); index++)  // TODO switch to iterators
    {
        int id = std::atoi(companionsDataPtr->getValue(index, "id"));

        // create companion object
        Companion* companionPtr = this->addCompanionObject(
            id,
            std::string(companionsDataPtr->getValue(index, "name")));

        if(!companionPtr)
        {
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

        if(socketsDataPtr->size() > 0)
        {
            // TODO use port number pool
            SocketInfo* socketInfoPtr = new SocketInfo(
                socketsDataPtr->getValue(0, "ipaddress"),
                std::atoi(socketsDataPtr->getValue(0, "server_port")),
                std::atoi(socketsDataPtr->getValue(0, "client_port")));

            companionPtr->setSocketInfo(socketInfoPtr);
        }

        if(companionPtr->getId() > 1)  // TODO change condition
        {
            this->fillWithMessages(companionPtr, false);

            if(!companionPtr->startServer())
            {
                logArgsError("problem with server start for companion id", id);
            }

            if(!companionPtr->createClient())
            {
                logArgsError("problem with client creation for companion id", id);
            }
        }
    }

    return companionsDataIsOk;
}

void Manager::buildWidgetGroups()
{
    GraphicManager* graphicManagerPtr = getGraphicManagerPtr();

    // auto companionsSize = this->companionPtrs_.size();
    auto companionsNumber = this->mapCompanionToWidgetGroup_.size();
    auto childrenSize = graphicManagerPtr->getCompanionPanelChildrenSize();

    // logArgs("companionsSize:", companionsSize, "childrenSize:", childrenSize);
    logArgs("companionsNumber:", companionsNumber, "childrenSize:", childrenSize);

    // if(companionsSize == 0 && childrenSize == 0)
    if(companionsNumber == 0 && childrenSize == 0)
    {
        logArgsWarning("strange case, empty sockets panel");
    }
    else
    {
        if(childrenSize != 0)
        {
            // TODO check if sockets already are children

            // hide companion panel stub widget
            graphicManagerPtr->hideCompanionPanelStub();

            // for(auto& companion : this->companionPtrs_)
            for(auto& pair : this->mapCompanionToWidgetGroup_)
            {
                // this->createWidgetGroupAndAddToMapping(companion);
                // this->createWidgetGroupAndAddToMapping(pair.first);
                this->createWidgetGroupAndAddToMapping(&(pair.first));
            }
        }
    }
}

Companion* Manager::addCompanionObject(int id, const std::string& name)
{
    if(id == 0)
    {
        logArgsError("companion id == 0");
        return nullptr;
    }

    // Companion* companionPtr = new Companion(id, name);
    // this->companionPtrs_.push_back(companionPtr);
    // this->mapCompanionToWidgetGroup_[companionPtr] = nullptr;
    // this->mapCompanionToWidgetGroup_[*companionPtr] = nullptr;

    auto result = this->mapCompanionToWidgetGroup_.emplace(
        // std::make_pair(Companion(id, name), nullptr));
        Companion(id, name), nullptr);

    // return companionPtr;
    return (result.second) ? const_cast<Companion*>(&(result.first->first)) : nullptr;
}

void Manager::createWidgetGroupAndAddToMapping(const Companion* companionPtr)
{
    WidgetGroup* widgetGroupPtr = new WidgetGroup(companionPtr);
    widgetGroupPtr->set();
    // this->mapCompanionToWidgetGroup_[companionPtr] = widgetGroupPtr;
    this->mapCompanionToWidgetGroup_[*companionPtr] = widgetGroupPtr;
}

void Manager::deleteCompanionObject(Companion* companionPtr)
{
    this->deleteWidgetGroupAndDeleteFromMapping(companionPtr);
}

void Manager::deleteWidgetGroupAndDeleteFromMapping(const Companion* companionPtr)
{
    auto findMapLambda = [&](auto& iterator)
    {
        // return iterator.first == companionPtr;
        return &(iterator.first) == companionPtr;
    };

    auto findMapResult = std::find_if(
        this->mapCompanionToWidgetGroup_.begin(),
        this->mapCompanionToWidgetGroup_.end(),
        findMapLambda);

    if(findMapResult == this->mapCompanionToWidgetGroup_.end())
    {
        showErrorDialogAndLogError(
            nullptr, "Companion was not found in mapping at deletion");
    }
    else
    {
        // auto findVectorLambda = [&](auto iterator)
        // {
        //     return iterator == companionPtr;
        // };

        // auto findVectorResult = std::find_if(
        //     this->companionPtrs_.begin(),
        //     this->companionPtrs_.end(),
        //     findVectorLambda);

        // if(findVectorResult == this->companionPtrs_.end())
        // {
        //     showErrorDialogAndLogError(
        //         nullptr, "Companion was not found in vector at deletion");
        // }
        // else
        // {
        //     this->companionPtrs_.erase(findVectorResult);
        // }

        if(this->selectedCompanionPtr_ == companionPtr)
        {
            this->selectedCompanionPtr_ = nullptr;
        }

        // delete companion
        // delete findMapResult->first;
        delete &(findMapResult->first);

        // delete widget group
        delete findMapResult->second;

        this->mapCompanionToWidgetGroup_.erase(findMapResult);
    }
}

bool Manager::companionDataValidation(CompanionAction* companionActionPtr)
{
    std::vector<std::string> validationErrors {};

    bool validationResult = validateCompanionData(validationErrors, companionActionPtr);

    if(!validationResult)
    {
        showErrorDialogAndLogError(
            nullptr,
            buildDialogText(std::string { "Error messages:\n\n" }, validationErrors));

        return false;
    }

    return true;
}

bool Manager::passwordDataValidation(PasswordAction* passwordActionPtr)
{
    std::vector<std::string> validationErrors {};

    bool validationResult = validatePassword(
        validationErrors, passwordActionPtr->getPassword());

    if(!validationResult)
    {
        showErrorDialogAndLogError(
            nullptr,
            buildDialogText(std::string { "Error messages:\n\n" }, validationErrors));

        return false;
    }

    return true;
}

bool Manager::checkCompanionDataForExistanceAtCreation(CompanionAction* companionActionPtr)
{
    // check if companion with such name already exists
    std::shared_ptr<DBReplyData> companionIdDataPtr = this->getDBDataPtr(
        logDBInteraction,
        "getCompanionByNameDBResult",
        &getCompanionByNameDBResult,
        buildStringVector("id"),
        companionActionPtr->getName());

    if(!companionIdDataPtr)
    {
        showErrorDialogAndLogError(nullptr, "Error getting data from db");
        return false;
    }

    if(!companionIdDataPtr->isEmpty())
    {
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

    if(!socketIdDataPtr)
    {
        showErrorDialogAndLogError(nullptr, "Error getting data from db");
        return false;
    }

    if(!socketIdDataPtr->isEmpty())
    {
        showErrorDialogAndLogError(nullptr, "Companion with such socket already exists");
        return false;
    }

    return true;
}

bool Manager::checkCompanionDataForExistanceAtUpdate(CompanionAction* companionActionPtr)
{
    // check if companion with such name already exists
    std::shared_ptr<DBReplyData> companionIdDataPtr = this->getDBDataPtr(
        logDBInteraction,
        "getCompanionByNameDBResult",
        &getCompanionByNameDBResult,
        buildStringVector("id"),
        companionActionPtr->getName());

    if(!companionIdDataPtr)
    {
        showErrorDialogAndLogError(nullptr, "Error getting data from db");
        return false;
    }

    bool findNameResult = companionIdDataPtr->findValue(
        std::string("id"),
        std::to_string(companionActionPtr->getCompanionId()));

    bool nameExistsAtOtherCompanion =
        (findNameResult && companionIdDataPtr->size() > 1) ||
        (!findNameResult && companionIdDataPtr->size() > 0);

    if(nameExistsAtOtherCompanion)
    {
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

    if(!socketIdDataPtr)
    {
        showErrorDialogAndLogError(nullptr, "Error getting data from db");
        return false;
    }

    bool findSocketResult = socketIdDataPtr->findValue(
        std::string("id"),
        std::to_string(companionActionPtr->getCompanionId()));

    bool socketExistsAtOtherCompanion =
        (findSocketResult && socketIdDataPtr->size() > 1) ||
        (!findSocketResult && socketIdDataPtr->size() > 0);

    if(socketExistsAtOtherCompanion)
    {
        showErrorDialogAndLogError(nullptr, "Companion with such socket already exists");
        return false;
    }

    return true;
}

void Manager::waitForMessageReceptionConfirmation(
    Companion* companionPtr, MessageState* messageStatePtr, const Message* messagePtr)
{
    auto lambda = [=]()
    {
        uint32_t sleepDuration = sleepDurationInitial;

        sleepForMilliseconds(sleepDuration);

        while(true)
        {
            if(messageStatePtr->getIsReceived())
            {
                return;
            }
            else
            {
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

bool Manager::markMessageAsSent(Companion* companionPtr, const Message* messagePtr)
{
    // mark in db
    std::shared_ptr<DBReplyData> messageIdDataPtr = this->getDBDataPtr(
        logDBInteraction,
        "setMessageInDbAndReturn",
        &setMessageIsSentInDbAndReturn,
        buildStringVector("id"),
        messagePtr->getId());

    if(!messageIdDataPtr)
    {
        showErrorDialogAndLogError(nullptr, "Error updating data in db");
        return false;
    }

    // mark in widget
    getGraphicManagerPtr()->markMessageWidgetAsSent(companionPtr, messagePtr);

    return true;
}

bool Manager::markMessageAsReceived(Companion* companionPtr, const Message* messagePtr)
{
    // mark in widget
    getGraphicManagerPtr()->markMessageWidgetAsReceived(companionPtr, messagePtr);

    // mark in db
    std::shared_ptr<DBReplyData> messageIdDataPtr = this->getDBDataPtr(
        logDBInteraction,
        "setMessageIsReceivedInDbAndReturn",
        &setMessageIsReceivedInDbAndReturn,
        buildStringVector("id"),
        messagePtr->getId());

    if(!messageIdDataPtr)
    {
        showErrorDialogAndLogError(nullptr, "Error getting data from db");
        return false;
    }

    if(messageIdDataPtr->isEmpty())
    {
        showErrorDialogAndLogError(nullptr, "Error setting message is_received in Db");
        return false;
    }

    return true;
}

std::tuple<uint32_t, uint8_t, std::string> Manager::pushMessageToDB(
    const std::string& companionName, const std::string& authorName,
    const std::string& timestamp, const std::string& text,
    const bool& isSent, const bool& isReceived)
{
    const std::string companionIdString("companion_id");

    std::shared_ptr<DBReplyData> messageDataPtr = this->getDBDataPtr(
        logDBInteraction,
        "pushMessageToDBAndReturn",
        &pushMessageToDBAndReturn,
        buildStringVector("id", "companion_id", "timestamp_tz"),
        companionName, authorName, timestamp, companionIdString, text,
        isSent, isReceived);

    if(!messageDataPtr)
    {
        showErrorDialogAndLogError(nullptr, "Error getting data from db");
        return std::tuple<uint32_t, uint8_t, std::string>(0, 0, "");
    }

    if(messageDataPtr->isEmpty())
    {
        logArgsError("messageDataPtr->isEmpty()");
        return std::tuple<uint32_t, uint8_t, std::string>(0, 0, "");
    }

    uint32_t id = std::atoi(messageDataPtr->getValue(0, "id"));
    uint8_t companionId = std::atoi(messageDataPtr->getValue(0, "companion_id"));
    std::string timestampTz { messageDataPtr->getValue(0, "timestamp_tz") };

    if(logDBInteraction)
    {
        logArgs("companionId:", companionId, "timestampTz:", timestampTz);
    }

    return std::tuple<uint32_t, uint8_t, std::string>(id, companionId, timestampTz);
}

Manager* getManagerPtr()
{
    QCoreApplication* coreApp = QCoreApplication::instance();
    ChatApp* app = dynamic_cast<ChatApp*>(coreApp);
    return app->managerPtr_;
}
