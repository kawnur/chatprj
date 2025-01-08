#include "manager.hpp"

SocketInfo::SocketInfo(
    std::string& ipAddress, uint16_t& serverPort, uint16_t& clientPort) :
    ipAddress_(ipAddress), serverPort_(serverPort), clientPort_(clientPort) {}

SocketInfo::SocketInfo(
    std::string&& ipAddress, uint16_t&& serverPort, uint16_t&& clientPort) :  // TODO ???
    ipAddress_(ipAddress),
    serverPort_(serverPort),
    clientPort_(clientPort) {}

SocketInfo::SocketInfo(const SocketInfo& si)
{
    ipAddress_ = si.ipAddress_;
    serverPort_ = si.serverPort_;
    clientPort_ = si.clientPort_;
}

void SocketInfo::print()
{
    logArgs("ipAddress:", this->ipAddress_);
    logArgs("serverPort_:", this->serverPort_);
    logArgs("clientPort_:", this->clientPort_);
}

std::string SocketInfo::getIpAddress() const
{
    return this->ipAddress_;
}

uint16_t SocketInfo::getServerPort() const
{
    return this->serverPort_;
}

uint16_t SocketInfo::getClientPort() const
{
    return this->clientPort_;
}

void SocketInfo::updateData(const CompanionData* dataPtr)
{
    this->ipAddress_ = dataPtr->getIpAddress();
    this->clientPort_ = std::stoi(dataPtr->getClientPort());
}

Message::Message(
        uint32_t id, uint8_t companion_id, uint8_t author_id,
        const std::string& time, const std::string& text, bool isSent) :
    id_(id), companion_id_(companion_id), author_id_(author_id), time_(time),
    text_(text), isSent_(isSent) {}

uint32_t Message::getId() const
{
    return this->id_;
}

uint8_t Message::getCompanionId() const
{
    return this->companion_id_;
}

uint8_t Message::getAuthorId() const
{
    return this->author_id_;
}

bool Message::isMessageFromMe() const
{
    return !(this->author_id_ == this->companion_id_);
}

std::string Message::getTime() const
{
    return this->time_;
}

std::string Message::getText() const
{
    return this->text_;
}

bool Message::getIsSent() const
{
    return this->isSent_;
}

Companion::Companion(int id, const std::string& name) :
    messagesMutex_(std::mutex()), id_(id), name_(name), socketInfoPtr_(nullptr),
    clientPtr_(nullptr), serverPtr_(nullptr),
    messagePointersPtr_(new std::vector<Message*>)
{}

Companion::Companion(int id, std::string&& name) :
    messagesMutex_(std::mutex()), id_(id), name_(name), socketInfoPtr_(nullptr),
    clientPtr_(nullptr), serverPtr_(nullptr),
    messagePointersPtr_(new std::vector<Message*>)
{}

Companion::~Companion()
{
    delete socketInfoPtr_;
    delete clientPtr_;
    delete serverPtr_;

    for(auto& messagePtr : *this->messagePointersPtr_)
    {
        delete messagePtr;
    }

    delete this->messagePointersPtr_;
}

bool Companion::startServer()
{
    bool started = false;

    auto startLambda = [this](bool& value)
    {
        // io_contextPtr_ = new boost::asio::io_context;

        // serverPtr_ = new ChatServer(
        //     *io_contextPtr_,
        //     this->socketInfoPtr_->getPortInt());

        this->serverPtr_ = new ChatServer(this, this->socketInfoPtr_->getServerPort());

        this->serverPtr_->run();

        value = true;
    };

    runAndLogException(startLambda, started);

    return started;
}

bool Companion::createClient()
{
    bool created = false;

    auto createLambda = [this](bool& value)
    {
        // io_contextPtr_ = new boost::asio::io_context;

        // serverPtr_ = new ChatServer(
        //     *io_contextPtr_,
        //     this->socketInfoPtr_->getPortInt());

        this->clientPtr_ = new ChatClient(
            this->socketInfoPtr_->getIpAddress(),
            this->socketInfoPtr_->getClientPort());

        // created = this->clientPtr_->connect();
        value = true;
    };

    runAndLogException(createLambda, created);

    return created;
}

// bool Companion::connectClient()
// {
//     bool connected = false;

//     auto connectLambda = [this](bool& value)
//     {
//         // io_contextPtr_ = new boost::asio::io_context;

//         // serverPtr_ = new ChatServer(
//         //     *io_contextPtr_,
//         //     this->socketInfoPtr_->getPortInt());

//         // this->clientPtr_ = new ChatClient(
//         //     this->socketInfoPtr_->getIpAddress(),
//         //     this->socketInfoPtr_->getClientPort());

//         value = this->clientPtr_->connect();
//     };

//     runAndLogException(connectLambda, connected);

//     return connected;
// }

// bool Companion::disconnectClient()
// {
//     bool disconnected = false;

//     auto disconnectLambda = [this](bool& value)
//     {
//         // io_contextPtr_ = new boost::asio::io_context;

//         // serverPtr_ = new ChatServer(
//         //     *io_contextPtr_,
//         //     this->socketInfoPtr_->getPortInt());

//         // this->clientPtr_ = new ChatClient(
//         //     this->socketInfoPtr_->getIpAddress(),
//         //     this->socketInfoPtr_->getClientPort());

//         value = this->clientPtr_->disconnect();
//     };

//     runAndLogException(disconnectLambda, disconnected);

//     return disconnected;
// }

bool Companion::connectClient()
{
    return this->clientPtr_->connect();
}

bool Companion::disconnectClient()
{
    return this->clientPtr_->disconnect();
}

void Companion::setSocketInfo(SocketInfo* socketInfo)
{
    socketInfoPtr_ = socketInfo;
}

void Companion::addMessage(Message* messagePtr)
{
    std::lock_guard<std::mutex> lock(this->messagesMutex_);
    this->messagePointersPtr_->push_back(messagePtr);
}

const std::vector<Message*>* Companion::getMessagesPtr() const
{
    return this->messagePointersPtr_;
}

bool Companion::sendMessage(
    NetworkMessageType type, std::string networkId, const Message* messagePtr)
{
    if(this->clientPtr_)
    {
        // build json
        std::string jsonData = buildMessageJSONString(type, networkId, messagePtr);

        // send json over network
        auto result = this->clientPtr_->send(jsonData);

        if(!result)
        {
            logArgsError("client message sending error");
        }

        return result;
    }

    return false;
}

void Companion::updateData(const CompanionData* dataPtr)
{
    this->name_ = dataPtr->getName();
    this->socketInfoPtr_->updateData(dataPtr);
}

Message* Companion::findMessage(uint32_t messageId)
{
    std::lock_guard<std::mutex> lock(this->messagesMutex_);

    auto result = std::find_if(
        this->messagePointersPtr_->begin(),
        this->messagePointersPtr_->end(),
        // [&](std::iterator<std::vector<Message*>> iter)
        [&](auto iter)
        {
            return iter->getId() == messageId;
        });

    return (result == this->messagePointersPtr_->end()) ? nullptr : *result;
}

int Companion::getId()
{
    return this->id_;
}

std::string Companion::getName() const
{
    return this->name_;
}

std::string Companion::getIpAddress() const
{
    return this->socketInfoPtr_->getIpAddress();
}

uint16_t Companion::getServerPort() const
{
    return this->socketInfoPtr_->getServerPort();
}

uint16_t Companion::getClientPort() const
{
    return this->socketInfoPtr_->getClientPort();
}

SocketInfo* Companion::getSocketInfoPtr() const
{
    return this->socketInfoPtr_;
}

void Action::set()
{
    this->dialogPtr_->setAction(this);
    this->dialogPtr_->set();
    this->dialogPtr_->show();
}

Dialog* Action::getDialogPtr()
{
    return this->dialogPtr_;
}

CompanionAction::CompanionAction(
    CompanionActionType actionType, MainWindow* mainWindowPtr, Companion* companionPtr) :
    actionType_(actionType), mainWindowPtr_(mainWindowPtr), companionPtr_(companionPtr),
    dataPtr_(nullptr), Action(nullptr)
{
    switch(actionType)
    {
    case CompanionActionType::CREATE:
    case CompanionActionType::UPDATE:
        dialogPtr_ = new CompanionDataDialog(actionType_, mainWindowPtr_, companionPtr_);
        break;

    case CompanionActionType::DELETE:
        dialogPtr_ = new TextDialog(
            nullptr,
            DialogType::WARNING,
            deleteCompanionDialogText,
            new std::vector<ButtonInfo> {
                ButtonInfo(
                    cancelButtonText, QDialogButtonBox::RejectRole, &TextDialog::reject),
                ButtonInfo(
                    deleteCompanionButtonText, QDialogButtonBox::AcceptRole,
                    &TextDialog::acceptAction) });
        break;

    case CompanionActionType::CLEAR_HISTORY:
        dialogPtr_ = new TextDialog(
            nullptr,
            DialogType::WARNING,
            clearCompanionHistoryDialogText,
            new std::vector<ButtonInfo> {
                ButtonInfo(
                    cancelButtonText, QDialogButtonBox::RejectRole, &TextDialog::reject),
                ButtonInfo(
                    clearHistoryButtonText, QDialogButtonBox::AcceptRole,
                    &TextDialog::acceptAction) });
        break;
    }
}

CompanionAction::~CompanionAction()
{
    delete this->dataPtr_;
}

CompanionActionType CompanionAction::getActionType()
{
    return this->actionType_;
}

std::string CompanionAction::getName() const
{
    return this->dataPtr_->getName();
}

std::string CompanionAction::getIpAddress() const
{
    return this->dataPtr_->getIpAddress();
}

std::string CompanionAction::getServerPort() const
{
    return this->dataPtr_->getServerPort();
}

std::string CompanionAction::getClientPort() const
{
    return this->dataPtr_->getClientPort();
}

int CompanionAction::getCompanionId() const
{
    return this->companionPtr_->getId();
}

Companion* CompanionAction::getCompanionPtr() const
{
    return this->companionPtr_;
}

void CompanionAction::sendData()
{
    std::string name;
    std::string ipAddress;
    std::string serverPort { "" };
    std::string clientPort;

    switch(this->actionType_)
    {
    case CompanionActionType::CREATE:
    case CompanionActionType::UPDATE:
        {
            CompanionDataDialog* dataDialogPtr =
                dynamic_cast<CompanionDataDialog*>(this->dialogPtr_);

            name = dataDialogPtr->getNameString();
            ipAddress = dataDialogPtr->getIpAddressString();
            clientPort = dataDialogPtr->getPortString();
        }
        break;

    case CompanionActionType::DELETE:
    case CompanionActionType::CLEAR_HISTORY:
        {
            name = this->companionPtr_->getName();
            ipAddress = this->companionPtr_->getIpAddress();
            clientPort = std::to_string(this->companionPtr_->getClientPort());
        }
        break;

    }

    logArgs("name:", name, "ipAddress:", ipAddress, "clientPort:", clientPort);

    this->dataPtr_ = new CompanionData(name, ipAddress, serverPort, clientPort);

    getGraphicManagerPtr()->sendCompanionDataToManager(this);
}

void CompanionAction::updateCompanionObjectData()
{
    this->companionPtr_->updateData(this->dataPtr_);
}

PasswordAction::PasswordAction(PasswordActionType actionType) : Action(nullptr)
{
    actionType_ = actionType;

    switch(actionType)
    {
    case PasswordActionType::CREATE:
        dialogPtr_ = new CreatePasswordDialog;
        break;

    case PasswordActionType::GET:
        dialogPtr_ = new GetPasswordDialog;
        break;
    }
}

PasswordAction::~PasswordAction()
{
    if(this->actionType_ == PasswordActionType::GET)
    {
        this->dialogPtr_->close();
    }
}

std::string PasswordAction::getPassword()
{
    return *this->passwordPtr_;
}

void PasswordAction::sendData()
{
    switch(this->actionType_)
    {
    case PasswordActionType::CREATE:
        {
            CreatePasswordDialog* passwordDialogPtr =
                dynamic_cast<CreatePasswordDialog*>(this->dialogPtr_);

            auto text1 = passwordDialogPtr->getFirstEditText();
            auto text2 = passwordDialogPtr->getSecondEditText();

            if(text1 == text2)
            {
                if(text1.size() == 0)
                {
                    showErrorDialogAndLogError(this->getDialogPtr(), "Empty password is invalid");
                    return;
                }

                this->passwordPtr_ = &text1;
                getGraphicManagerPtr()->sendNewPasswordDataToManager(this);
            }
            else
            {
                showErrorDialogAndLogError(this->getDialogPtr(), "Entered passwords are not equal");
            }
        }

        break;

    case PasswordActionType::GET:
        {
            GetPasswordDialog* passwordDialogPtr =
                dynamic_cast<GetPasswordDialog*>(this->dialogPtr_);

            auto text = passwordDialogPtr->getEditText();

            if(text.size() == 0)
            {
                showErrorDialogAndLogError(this->getDialogPtr(), "Empty password is invalid");
                return;
            }

            this->passwordPtr_ = &text;
            getGraphicManagerPtr()->sendExistingPasswordDataToManager(this);
        }

        break;
    }
}

Manager::Manager() :
    networkIdToMessageMapMutex_(std::mutex()),
    dbConnectionPtr_(nullptr), userIsAuthenticated_(false),
    companionPtrs_(std::vector<Companion*>())
{
    mapCompanionToWidgetGroup_ = std::map<const Companion*, WidgetGroup*>();
    selectedCompanionPtr_ = nullptr;

    mapNetworkIdToMessage_ = std::map<std::string, const Message*>();
}

Manager::~Manager()
{
    delete dbConnectionPtr_;

    for(auto& companion : this->companionPtrs_)
    {
        delete companion;
    }
}

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

std::tuple<uint32_t, uint8_t, std::string> Manager::pushMessageToDB(
    const std::string& companionName, const std::string& authorName,
    const std::string& timestamp, const std::string& text)
{
    std::shared_ptr<DBReplyData> messageDataPtr = this->getDBDataPtr(
        logDBInteraction,
        "pushMessageToDBAndReturn",
        &pushMessageToDBAndReturn,
        buildStringVector("id", "companion_id", "timestamp_tz"),
        companionName, authorName, timestamp, std::string("companion_id"), text);

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

void Manager::sendMessage(Companion* companionPtr, const std::string& text)
{
    WidgetGroup* groupPtr = this->mapCompanionToWidgetGroup_.at(companionPtr);

    // encrypt message

    // add to DB and get timestamp
    auto tuple = this->pushMessageToDB(
        companionPtr->getName(), std::string("me"), std::string("now()"), text);

    uint32_t id = std::get<0>(tuple);
    uint8_t companion_id = std::get<1>(tuple);
    std::string timestamp = std::get<2>(tuple);

    if(companion_id == 0 || timestamp == "")
    {
        logArgsError("error adding message to db");
        return;
    }

    // add to companion's messages
    Message* messagePtr = new Message(id, companion_id, 1, timestamp, text, false);
    companionPtr->addMessage(messagePtr);

    // add to widget
    groupPtr->addMessageWidgetToChatHistory(messagePtr);

    // add to mapping
    std::string networkId = this->addToNetworkIdToMessageMapping(messagePtr);

    logArgsWithCustomMark("send:", networkId);

    // send over network
    bool result = companionPtr->sendMessage(
        NetworkMessageType::SEND_DATA, networkId, messagePtr);

    // mark message as sent
    if(result)
    {
        this->markMessageAsSent(messagePtr);
    }
}

void Manager::receiveMessage(Companion* companionPtr, const std::string& jsonString)
{
    nlohmann::json jsonData = buildMessageJsonObject(jsonString);

    NetworkMessageType type = jsonData.at("type");
    std::string networkId = jsonData.at("id");

    logArgsWithCustomMark("received:", networkId);

    switch(type)
    {
    // message is confirmation
    case NetworkMessageType::RECEIVE_CONFIRMATION:
        {
            auto received = jsonData.at("received");
            if(received == 1)
            {
                // successfully received
                // mark message as received
                std::lock_guard<std::mutex> lock(this->networkIdToMessageMapMutex_);

                runAndLogException(
                    [&]()
                    {
                        this->markMessageAsReceived(this->mapNetworkIdToMessage_.at(networkId));
                    });

                auto iter = this->mapNetworkIdToMessage_.find(networkId);

                if(iter != this->mapNetworkIdToMessage_.end())
                {
                    this->mapNetworkIdToMessage_.erase(iter);
                }
            }
            else
            {
                // message was not received, resend message
            }
        }

        break;

    // message is not confirmation
    case NetworkMessageType::SEND_DATA:
        {
            auto timestamp = jsonData.at("time");
            auto text = jsonData.at("text");

            // add to DB and get timestamp
            auto tuple = this->pushMessageToDB(
                companionPtr->getName(), companionPtr->getName(), timestamp, text);

            uint32_t id = std::get<0>(tuple);
            uint8_t companion_id = std::get<1>(tuple);

            if(companion_id == 0 || timestamp == "")
            {
                logArgsError("error adding message to db");
                return;
            }

            // add to companion's messages
            Message* messagePtr = new Message(id, companion_id, companion_id, timestamp, text, false);
            companionPtr->addMessage(messagePtr);

            // decrypt message

            // add to widget
            WidgetGroup* groupPtr = this->mapCompanionToWidgetGroup_.at(companionPtr);  // TODO try catch
            // groupPtr->addMessageWidgetToChatHistory(messagePtr);
            groupPtr->addMessageWidgetToChatHistoryFromThread(messagePtr);

            // sort elements
            getGraphicManagerPtr()->sortChatHistoryElementsForWidgetGroup(groupPtr);

            logArgsWithCustomMark("send:", networkId);

            // send reception confirmation to sender
            bool result = companionPtr->sendMessage(
                NetworkMessageType::RECEIVE_CONFIRMATION, networkId, messagePtr);
        }

        break;
    }
}

bool Manager::getUserIsAuthenticated()
{
    return this->userIsAuthenticated_;
}

const Companion* Manager::getMappedCompanionBySocketInfoBaseWidget(
    SocketInfoBaseWidget* widget) const
{
    auto findWidget = [&](const std::pair<const Companion*, WidgetGroup*> pair)
    {
        return pair.second->getSocketInfoBasePtr() == widget;
    };

    auto result = std::find_if(
        this->mapCompanionToWidgetGroup_.cbegin(),
        this->mapCompanionToWidgetGroup_.cend(),
        findWidget);

    return result->first;
}

std::string Manager::addToNetworkIdToMessageMapping(const Message* messagePtr)
{
    std::lock_guard<std::mutex> lock(this->networkIdToMessageMapMutex_);

    std::string networkId = getRandomString(5);

    auto lambda = [&]()
    {
        if(this->mapNetworkIdToMessage_.count(networkId) == 0)
        {
            this->mapNetworkIdToMessage_[networkId] = messagePtr;
            return true;
        }
        else
        {
            return false;
        }
    };

    while(!lambda())
    {
        networkId = getRandomString(5);
    }

    return networkId;
}

const Companion* Manager::getMappedCompanionByWidgetGroup(
    WidgetGroup* groupPtr) const
{
    auto findWidget = [&](const std::pair<const Companion*, WidgetGroup*> pair){
        return pair.second == groupPtr;
    };

    auto result = std::find_if(
        this->mapCompanionToWidgetGroup_.cbegin(),
        this->mapCompanionToWidgetGroup_.cend(),
        findWidget);

    return result->first;
}

std::string Manager::getMappedNetworkIdByMessagePtr(Message* messagePtr)
{
    std::lock_guard<std::mutex> lock(this->networkIdToMessageMapMutex_);

    auto result = std::find_if(
        this->mapNetworkIdToMessage_.begin(),
        this->mapNetworkIdToMessage_.end(),
        [&](auto iter)
        {
            return iter.second == messagePtr;
        });

    return (result == this->mapNetworkIdToMessage_.end()) ?
               std::string("") : result->first;
}

void Manager::resetSelectedCompanion(const Companion* newSelected)
{
    GraphicManager* graphicManagerPtr = getGraphicManagerPtr();

    if(this->selectedCompanionPtr_)
    {
        auto widgetGroup = this->mapCompanionToWidgetGroup_.at(this->selectedCompanionPtr_);  // TODO try catch

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
        auto widgetGroup = this->mapCompanionToWidgetGroup_.at(this->selectedCompanionPtr_);

        dynamic_cast<SocketInfoWidget*>(widgetGroup->getSocketInfoBasePtr())->select();

        widgetGroup->showCentralPanel();
    }
    else
    {
        graphicManagerPtr->showCentralPanelStub();
    }
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

bool Manager::markMessageAsSent(const Message* messagePtr)
{
    // mark in db
    std::shared_ptr<DBReplyData> messageIdDataPtr = this->getDBDataPtr(
        logDBInteraction,
        "setMessageInDbAndReturn",
        &setMessageInDbAndReturn,
        buildStringVector("id"),
        messagePtr->getId());

    if(!messageIdDataPtr)
    {
        showErrorDialogAndLogError(nullptr, "Error updating data in db");
        return false;
    }

    // mark in widget
    getGraphicManagerPtr()->markMessageWidgetAsSent(messagePtr);

    return true;
}

bool Manager::markMessageAsReceived(const Message* messagePtr)
{
    // mark in widget
    getGraphicManagerPtr()->markMessageWidgetAsReceived(messagePtr);

    return true;
}

void Manager::deleteCompanionObject(Companion* companionPtr)
{
    this->deleteWidgetGroupAndDeleteFromMapping(companionPtr);
}

void Manager::deleteWidgetGroupAndDeleteFromMapping(Companion* companionPtr)
{
    auto findMapLambda = [&](auto iterator)
    {
        return iterator.first == companionPtr;
    };

    auto findMapResult = std::find_if(
        this->mapCompanionToWidgetGroup_.begin(),
        this->mapCompanionToWidgetGroup_.end(),
        findMapLambda);

    if(findMapResult == this->mapCompanionToWidgetGroup_.end())
    {
        showErrorDialogAndLogError(nullptr, "Companion was not found in mapping at deletion");
    }
    else
    {
        auto findVectorLambda = [&](auto iterator)
        {
            return iterator == companionPtr;
        };

        auto findVectorResult = std::find_if(
            this->companionPtrs_.begin(),
            this->companionPtrs_.end(),
            findVectorLambda);

        if(findVectorResult == this->companionPtrs_.end())
        {
            showErrorDialogAndLogError(nullptr, "Companion was not found in vector at deletion");
        }
        else
        {
            this->companionPtrs_.erase(findVectorResult);
        }

        if(this->selectedCompanionPtr_ == companionPtr)
        {
            this->selectedCompanionPtr_ = nullptr;
        }

        // delete companion
        delete findMapResult->first;

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

    bool validationResult = validatePassword(validationErrors, passwordActionPtr->getPassword());

    if(!validationResult)
    {
        showErrorDialogAndLogError(
            nullptr,
            buildDialogText(std::string { "Error messages:\n\n" }, validationErrors));

        return false;
    }

    return true;
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
    auto widgetGroup = this->mapCompanionToWidgetGroup_.at(
        companionActionPtr->getCompanionPtr());  // TODO try catch

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
    WidgetGroup* widgetGroupPtr =
        this->mapCompanionToWidgetGroup_.at(companionActionPtr->getCompanionPtr());

    getGraphicManagerPtr()->clearChatHistory(widgetGroupPtr);

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

void Manager::hideSelectedCompanionCentralPanel()
{
    if(this->selectedCompanionPtr_)
    {
        auto groupPtr = this->mapCompanionToWidgetGroup_.at(this->selectedCompanionPtr_);
        getGraphicManagerPtr()->hideWidgetGroupCentralPanel(groupPtr);
    }
}

void Manager::showSelectedCompanionCentralPanel()
{
    if(this->selectedCompanionPtr_)
    {
        auto groupPtr = this->mapCompanionToWidgetGroup_.at(this->selectedCompanionPtr_);
        getGraphicManagerPtr()->showWidgetGroupCentralPanel(groupPtr);
    }
}

bool Manager::isSelectedCompanionNullptr()
{
    return this->selectedCompanionPtr_ == nullptr;
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
        buildStringVector("id", "author_id", "companion_id", "timestamp_tz", "message"),
        companionPtr->getName());

    if(!messagesDataPtr)
    {
        showErrorDialogAndLogError(nullptr, "Error getting data from db");
        return;
    }

    if(messagesDataPtr->isEmpty())
    {
        logArgsWarning("Empty db reply to unsent messages selection");
        return;
    }

    // add to companion's messages if needed
    for(size_t i = 0; i < messagesDataPtr->size(); i++)  // TODO switch to iterators
    {
        uint32_t messageId = std::atoi(messagesDataPtr->getValue(i, "id"));
        Message* messagePtr = companionCastPtr->findMessage(messageId);
        std::string networkId;

        if(messagePtr)
        {
            networkId = this->getMappedNetworkIdByMessagePtr(messagePtr);

            if(networkId.empty())
            {
                showErrorDialogAndLogError(
                    nullptr,
                    "strange case: unsent message found in companions messages, "
                    "but not found in manager's mapNetworkIdToMessage_");
            }
        }
        else
        {
            uint8_t companion_id = std::atoi(messagesDataPtr->getValue(i, "companion_id"));

            messagePtr = new Message(
                messageId,
                companion_id,
                1,
                messagesDataPtr->getValue(i, "timestamp_tz"),
                messagesDataPtr->getValue(i, "message"),
                false);

            companionCastPtr->addMessage(messagePtr);

            networkId = getRandomString(5);
        }

        // send over network
        bool result = companionCastPtr->sendMessage(
            NetworkMessageType::SEND_DATA, networkId, messagePtr);

        // mark message as sent
        if(result)
        {
            this->markMessageAsSent(messagePtr);
        }
    }

    // sort elements
    // WidgetGroup* groupPtr = this->mapCompanionToWidgetGroup_.at(companionPtr);
    // getGraphicManagerPtr()->sortChatHistoryElementsForWidgetGroup(groupPtr);
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

    for(size_t index = 0; index < companionsDataPtr->size(); index++)  // TODO switch to iterators
    {
        int id = std::atoi(companionsDataPtr->getValue(index, "id"));

        // create companion object
        Companion* companionPtr = this->addCompanionObject(
            id,
            std::string(companionsDataPtr->getValue(index, "name")));

        if(!companionPtr)
        {
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
            // get messages data
            std::shared_ptr<DBReplyData> messagesDataPtr = this->getDBDataPtr(
                logDBInteraction,
                "getMessagesDBResult",
                &getMessagesDBResult,
                buildStringVector(
                    "id", "companion_id", "author_id",
                    "timestamp_tz", "message", "issent"),
                id);

            for(size_t i = 0; i < messagesDataPtr->size(); i++)  // TODO switch to iterators
            {
                Message* messagePtr = new Message(
                    std::atoi(messagesDataPtr->getValue(i, "id")),
                    id,
                    std::atoi(messagesDataPtr->getValue(i, "author_id")),
                    messagesDataPtr->getValue(i, "timestamp_tz"),
                    messagesDataPtr->getValue(i, "message"),
                    messagesDataPtr->getValue(i, "issent"));

                companionPtr->addMessage(messagePtr);
            }

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

    auto companionsSize = this->companionPtrs_.size();
    auto childrenSize = graphicManagerPtr->getCompanionPanelChildrenSize();

    logArgs("companionsSize:", companionsSize, "childrenSize:", childrenSize);

    if(companionsSize == 0 && childrenSize == 0)
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

            for(auto& companion : this->companionPtrs_)
            {
                this->createWidgetGroupAndAddToMapping(companion);
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

    Companion* companionPtr = new Companion(id, name);
    this->companionPtrs_.push_back(companionPtr);

    return companionPtr;
}

void Manager::createWidgetGroupAndAddToMapping(Companion* companionPtr)
{
    WidgetGroup* widgetGroupPtr = new WidgetGroup(companionPtr);
    this->mapCompanionToWidgetGroup_[companionPtr] = widgetGroupPtr;
}

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

Manager* getManagerPtr()
{
    QCoreApplication* coreApp = QCoreApplication::instance();
    ChatApp* app = dynamic_cast<ChatApp*>(coreApp);
    return app->managerPtr_;
}

GraphicManager::GraphicManager() :
    messageToMessageWidgetMapMutex_(std::mutex()),
    mapMessageToMessageWidget_(std::map<const Message*, const MessageWidget*>())
{
    // stubWidgetsPtr_ = new StubWidgetGroup;
    // mainWindowPtr_ = new MainWindow;

    // stubWidgetsPtr_ = nullptr;
    // mainWindowPtr_ = nullptr;
}

void GraphicManager::set()
{
    stubWidgetsPtr_ = new StubWidgetGroup;
    mainWindowPtr_ = new MainWindow;

    // this->stubWidgetsPtr_->set();
    this->mainWindowPtr_->set();
    this->mainWindowPtr_->show();
}

void GraphicManager::setParentsForStubs(
    QWidget* leftContainerPtr, QWidget* centralContainerPtr)
{
    this->stubWidgetsPtr_->setParents(leftContainerPtr, centralContainerPtr);
}

void GraphicManager::setStubWidgets()
{
    this->stubWidgetsPtr_->set();
}

void GraphicManager::sendMessage(Companion* companionPtr, const std::string& text)
{
    getManagerPtr()->sendMessage(companionPtr, text);
}

void GraphicManager::addTextToAppLogWidget(const QString& text)
{
    this->mainWindowPtr_->addTextToAppLogWidget(text);
}

size_t GraphicManager::getCompanionPanelChildrenSize()
{
    return this->mainWindowPtr_->getCompanionPanelChildrenSize();
}

void GraphicManager::hideWidgetGroupCentralPanel(WidgetGroup* groupPtr)
{
    groupPtr->hideCentralPanel();
}

void GraphicManager::showWidgetGroupCentralPanel(WidgetGroup* groupPtr)
{
    groupPtr->showCentralPanel();
}

void GraphicManager::addWidgetToMainWindowContainerAndSetParentTo(
    MainWindowContainerPosition position, QWidget* widgetPtr)
{
    this->mainWindowPtr_->addWidgetToContainerAndSetParentTo(
        position, widgetPtr);
}

void GraphicManager::removeWidgetFromCompanionPanel(SocketInfoBaseWidget* widget)
{
    this->mainWindowPtr_->removeWidgetFromCompanionPanel(widget);
}

void GraphicManager::addWidgetToCompanionPanel(SocketInfoBaseWidget* widget)
{
    this->mainWindowPtr_->addWidgetToCompanionPanel(widget);
}

void GraphicManager::createTextDialogAndShow(
    QWidget* parentPtr, DialogType dialogType, const QString& text,
    std::vector<ButtonInfo>* buttonInfoPtr)
{
    // TODO delete objects for closed dialoges?
    TextDialog* dialogPtr = new TextDialog(parentPtr, dialogType, text, buttonInfoPtr);

    dialogPtr->set();
    dialogPtr->show();
}

void GraphicManager::createCompanion()
{
    CompanionAction* actionPtr = new CompanionAction(
        CompanionActionType::CREATE, this->mainWindowPtr_, nullptr);
    actionPtr->set();
}

void GraphicManager::updateCompanion(Companion* companionPtr)
{
    CompanionAction* actionPtr = new CompanionAction(
        CompanionActionType::UPDATE, this->mainWindowPtr_, companionPtr);
    actionPtr->set();
}

void GraphicManager::clearCompanionHistory(Companion* companionPtr)
{
    CompanionAction* actionPtr = new CompanionAction(
        CompanionActionType::CLEAR_HISTORY, this->mainWindowPtr_, companionPtr);
    actionPtr->set();
}

void GraphicManager::clearChatHistory(WidgetGroup* widgetGroupPtr)
{
    widgetGroupPtr->clearChatHistory();
}

void GraphicManager::deleteCompanion(Companion* companionPtr)
{
    CompanionAction* actionPtr = new CompanionAction(
        CompanionActionType::DELETE, this->mainWindowPtr_, companionPtr);
    actionPtr->set();
}

void GraphicManager::sendCompanionDataToManager(CompanionAction* actionPtr)
{
    switch(actionPtr->getActionType())
    {
    case CompanionActionType::CREATE:
        getManagerPtr()->createCompanion(actionPtr);
        break;
    case CompanionActionType::UPDATE:
        getManagerPtr()->updateCompanion(actionPtr);
        break;
    case CompanionActionType::DELETE:
        getManagerPtr()->deleteCompanion(actionPtr);
        break;
    case CompanionActionType::CLEAR_HISTORY:
        getManagerPtr()->clearCompanionHistory(actionPtr);
        break;
    }
}

void GraphicManager::showCompanionInfoDialog(
    CompanionAction* companionActionPtr, std::string&& header)
{
    QWidget* parentPtr = nullptr;
    void (TextDialog::*functionPtr)() = nullptr;

    auto formDialogPtr = companionActionPtr->getDialogPtr();

    if(formDialogPtr)
    {
        parentPtr = formDialogPtr;
        functionPtr = &TextDialog::closeSelfAndParentDialog;
    }
    else
    {
        functionPtr = &TextDialog::closeSelf;
    }

    this->createTextDialogAndShow(
        parentPtr,
        DialogType::INFO,
        buildDialogText(
            std::move(header),
            std::vector<std::string> {
                std::string("name: ") + companionActionPtr->getName(),
                std::string("ipAddress: ") + companionActionPtr->getIpAddress(),
                std::string("port: ") + companionActionPtr->getClientPort() }),
        createOkButtonInfoVector(functionPtr));

    delete companionActionPtr;
}

void GraphicManager::sendNewPasswordDataToManager(PasswordAction* actionPtr)
{
    getManagerPtr()->createUserPassword(actionPtr);
}

void GraphicManager::sendExistingPasswordDataToManager(PasswordAction* actionPtr)
{
    getManagerPtr()->authenticateUser(actionPtr);
}

void GraphicManager::hideCompanionPanelStub()
{
    this->stubWidgetsPtr_->hideSocketInfoStubWidget();
}

void GraphicManager::hideCentralPanelStub()
{
    this->stubWidgetsPtr_->hideCentralPanel();
}

void GraphicManager::showCentralPanelStub()
{
    this->stubWidgetsPtr_->showCentralPanel();
}

void GraphicManager::hideInfoViaBlur()
{
    this->enableMainWindowBlurEffect();
}

void GraphicManager::showInfoViaBlur()
{
    this->disableMainWindowBlurEffect();
}

void GraphicManager::hideInfoViaStubs()
{
    this->stubWidgetsPtr_->setLeftPanelWidth(
        this->mainWindowPtr_->getLeftPanelWidgetWidth());

    this->mainWindowPtr_->hideLeftAndRightPanels();
    getManagerPtr()->hideSelectedCompanionCentralPanel();
    this->stubWidgetsPtr_->showStubPanels();
}

void GraphicManager::showInfoViaStubs()
{
    this->mainWindowPtr_->showLeftAndRightPanels();
    getManagerPtr()->showSelectedCompanionCentralPanel();
    this->stubWidgetsPtr_->hideStubPanels();
}

void GraphicManager::hideInfo()
{    
    this->hideInfoViaBlur();
    // this->hideInfoViaStubs();
}

void GraphicManager::showInfo()
{
    getManagerPtr()->startUserAuthentication();

    // this->showInfoViaBlur();
    // this->showInfoViaStubs();
}

MainWindow* GraphicManager::getMainWindowPtr()
{
    return this->mainWindowPtr_;
}

void GraphicManager::createEntrancePassword()
{
    PasswordAction* actionPtr = new PasswordAction(PasswordActionType::CREATE);
    actionPtr->set();
}

void GraphicManager::enableMainWindowBlurEffect()
{
    this->mainWindowPtr_->enableBlurEffect();
}

void GraphicManager::disableMainWindowBlurEffect()
{
    this->mainWindowPtr_->disableBlurEffect();
}

void GraphicManager::getEntrancePassword()
{
    PasswordAction* actionPtr = new PasswordAction(PasswordActionType::GET);
    actionPtr->set();
}

void GraphicManager::addToMessageMapping(
    const Message* messagePtr, const MessageWidget* messageWidgetPtr)
{
    std::lock_guard<std::mutex> lock(this->messageToMessageWidgetMapMutex_);

    this->mapMessageToMessageWidget_[messagePtr] = messageWidgetPtr;
}

void GraphicManager::markMessageWidgetAsSent(const Message* messagePtr)
{
    auto setLambda = [&, this]()
    {
        std::lock_guard<std::mutex> lock(this->messageToMessageWidgetMapMutex_);

        try
        {
            const_cast<MessageWidget*>(
                this->mapMessageToMessageWidget_.at(messagePtr))->setMessageAsSent();
        }
        catch(std::out_of_range)
        {
            return;
        }
    };

    runAndLogException(setLambda);
}

void GraphicManager::markMessageWidgetAsReceived(const Message* messagePtr)
{
    auto setLambda = [&, this]()
    {
        std::lock_guard<std::mutex> lock(this->messageToMessageWidgetMapMutex_);

        try
        {
            const_cast<MessageWidget*>(
                this->mapMessageToMessageWidget_.at(messagePtr))->setMessageAsReceived();
        }
        catch(std::out_of_range)
        {
            return;
        }
    };

    runAndLogException(setLambda);
}

void GraphicManager::sortChatHistoryElementsForWidgetGroup(WidgetGroup* groupPtr)
{
    groupPtr->sortChatHistoryElements();
}

std::string GraphicManager::getMappedMessageTimeByMessageWidgetPtr(
    MessageWidget* widgetPtr)
{
    std::lock_guard<std::mutex> lock(this->messageToMessageWidgetMapMutex_);

    auto result = std::find_if(
        this->mapMessageToMessageWidget_.begin(),
        this->mapMessageToMessageWidget_.end(),
        [&](auto iter)
        {
            return iter.second == widgetPtr;
        });

    return (result == this->mapMessageToMessageWidget_.end()) ?
               std::string("") : result->first->getTime();
}

GraphicManager* getGraphicManagerPtr()
{
    QCoreApplication* coreApp = QCoreApplication::instance();
    ChatApp* app = dynamic_cast<ChatApp*>(coreApp);
    return app->graphicManagerPtr_;
}
