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
        int companion_id, int author_id, const std::string& time,
        const std::string& text, bool isSent) :
    companion_id_(companion_id), author_id_(author_id), time_(time),
    text_(text), isSent_(isSent) {}

int Message::getCompanionId() const
{
    return this->companion_id_;
}

int Message::getAuthorId() const
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

void Companion::sendMessage(const Message* messagePtr)
{
    if(this->clientPtr_)
    {
        // build json
        std::string jsonData = buildMessageJSONString(messagePtr);

        // send json over network
        // auto result = this->clientPtr_->send(messagePtr->getText());
        auto result = this->clientPtr_->send(jsonData);

        if(!result)
        {
            logArgsError("client message sending error");
        }
    }
}

void Companion::updateData(const CompanionData* dataPtr)
{
    this->name_ = dataPtr->getName();
    this->socketInfoPtr_->updateData(dataPtr);
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
        break;
    }

    case CompanionActionType::DELETE:
    case CompanionActionType::CLEAR_HISTORY:
    {
        name = this->companionPtr_->getName();
        ipAddress = this->companionPtr_->getIpAddress();
        clientPort = std::to_string(this->companionPtr_->getClientPort());
        break;
    }
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

            break;
        }
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

            break;
        }
    }
}


Manager::Manager() :
    dbConnectionPtr_(nullptr), userIsAuthenticated_(false),
    companionPtrs_(std::vector<Companion*>())
{
    mapCompanionToWidgetGroup_ = std::map<const Companion*, WidgetGroup*>();
    selectedCompanionPtr_ = nullptr;
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

std::pair<int, std::string> Manager::pushMessageToDB(
    const std::string& companionName, const std::string& authorName,
    const std::string& timestamp, const std::string& text)
{
    std::shared_ptr<DBReplyData> messageDataPtr = this->getDBDataPtr(
        true,
        "pushMessageToDBAndReturn",
        &pushMessageToDBAndReturn,
        std::vector<std::string> {
            std::string("companion_id"), std::string("timestamp_tz")
        },
        companionName, authorName, timestamp, std::string("companion_id"), text);

    if(!messageDataPtr)
    {
        showErrorDialogAndLogError(nullptr, "Error getting data from db");
        return std::pair<int, std::string>(0, "");
    }

    if(messageDataPtr->isEmpty())
    {
        logArgsError("messageDataPtr->isEmpty()");
        return std::pair<int, std::string>(0, "");
    }

    int companionId = std::atoi(messageDataPtr->getValue(0, "companion_id"));
    std::string timestampTz { messageDataPtr->getValue(0, "timestamp_tz") };

    logArgs("companionId:", companionId, "timestampTz:", timestampTz);

    return std::pair<int, std::string>(companionId, timestampTz);
}

// void Manager::sendMessage(WidgetGroup* groupPtr, const std::string& text)
// {
//     Companion* companionPtr =
//         const_cast<Companion*>(this->getMappedCompanionByWidgetGroup(groupPtr));

//     // encrypt message

//     // add to DB and get timestamp
//     auto pair = this->pushMessageToDB(
//         companionPtr->getName(), std::string("me"), std::string("now()"), text);

//     if(pair.first == 0 || pair.second == "")
//     {
//         logArgsError("error adding message to db");
//         return;
//     }

//     // add to companion's messages
//     Message* messagePtr = new Message(pair.first, 1, pair.second, text, false);
//     companionPtr->addMessage(messagePtr);

//     // add to widget
//     groupPtr->addMessageToChatHistory(messagePtr);

//     // send over network
//     companionPtr->sendMessage(messagePtr);
// }

void Manager::sendMessage(Companion* companionPtr, const std::string& text)
{
    WidgetGroup* groupPtr = this->mapCompanionToWidgetGroup_.at(companionPtr);

    // encrypt message

    // add to DB and get timestamp
    auto pair = this->pushMessageToDB(
        companionPtr->getName(), std::string("me"), std::string("now()"), text);

    if(pair.first == 0 || pair.second == "")
    {
        logArgsError("error adding message to db");
        return;
    }

    // add to companion's messages
    Message* messagePtr = new Message(pair.first, 1, pair.second, text, false);
    companionPtr->addMessage(messagePtr);

    // add to widget
    groupPtr->addMessageWidgetToChatHistory(messagePtr);

    // send over network
    companionPtr->sendMessage(messagePtr);
}

void Manager::receiveMessage(Companion* companionPtr, const std::string& jsonString)
{
    nlohmann::json jsonData = buildMessageJsonObject(jsonString);
    auto timestamp = jsonData.at("time");
    auto text = jsonData.at("text");

    // add to DB and get timestamp
    auto pair = this->pushMessageToDB(
        companionPtr->getName(), companionPtr->getName(), timestamp, text);

    if(pair.first == 0 || pair.second == "")
    {
        logArgsError("error adding message to db");
        return;
    }

    // add to companion's messages
    Message* messagePtr = new Message(pair.first, pair.first, timestamp, text, false);
    companionPtr->addMessage(messagePtr);

    // decrypt message

    // add to widget
    WidgetGroup* groupPtr = this->mapCompanionToWidgetGroup_.at(companionPtr);  // TODO try catch
    // groupPtr->addMessageWidgetToChatHistory(messagePtr);
    groupPtr->addMessageWidgetToChatHistoryFromThread(messagePtr);
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
        true,
        "getCompanionByNameDBResult",
        &getCompanionByNameDBResult,
        std::vector<std::string> { std::string("id") },
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
        true,
        "getSocketByIpAddressAndPortDBResult",
        &getSocketByIpAddressAndPortDBResult,
        std::vector<std::string> { std::string("id") },
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
        true,
        "getCompanionByNameDBResult",
        &getCompanionByNameDBResult,
        std::vector<std::string> { std::string("id") },
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
        true,
        "getSocketByIpAddressAndPortDBResult",
        &getSocketByIpAddressAndPortDBResult,
        std::vector<std::string> { std::string("id") },
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
        true,
        "pushCompanionToDBAndReturn",
        &pushCompanionToDBAndReturn,
        std::vector<std::string> { std::string("id") },
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
        true,
        "pushSocketToDB",
        &pushSocketToDBAndReturn,
        std::vector<std::string>{ std::string("id") },
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
        true,
        "updateCompanionAndSocketAndReturn",
        &updateCompanionAndSocketAndReturn,
        std::vector<std::string> { std::string("id") },
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
        true,
        "deleteMessagesFromDBAndReturn",
        &deleteMessagesFromDBAndReturn,
        std::vector<std::string> { std::string("id") },
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
        true,
        "deleteCompanionAndSocketAndReturn",
        &deleteCompanionAndSocketAndReturn,
        std::vector<std::string> { std::string("id") },
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
        true,
        "deleteMessagesFromDBAndReturn",
        &deleteMessagesFromDBAndReturn,
        std::vector<std::string> { std::string("companion_id") },
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
        true,
        "pushPasswordToDBAndReturn",
        &pushPasswordToDBAndReturn,
        std::vector<std::string> { std::string("id") },
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
        true,
        "getPasswordDBResult",
        &getPasswordDBResult,
        std::vector<std::string> { std::string("id"), std::string("password") });

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
        true,
        "getPasswordDBResult",
        &getPasswordDBResult,
        std::vector<std::string> { std::string("id"), std::string("password") });

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

bool Manager::buildCompanions()
{
    bool companionsDataIsOk = true;

    // get companion data
    std::shared_ptr<DBReplyData> companionsDataPtr = this->getDBDataPtr(
        true,
        "getCompanionsDBResult",
        &getCompanionsDBResult,
        std::vector<std::string> { std::string("id"), std::string("name") });

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
            true,
            "getSocketInfoDBResult",
            &getSocketInfoDBResult,
            std::vector<std::string> {
                std::string("ipaddress"), std::string("server_port"), std::string("client_port")
            },
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
                false,
                "getMessagesDBResult",
                &getMessagesDBResult,
                std::vector<std::string> {
                    std::string("companion_id"), std::string("author_id"),
                    std::string("timestamp_tz"), std::string("message"),
                    std::string("issent")
                },
                id);

            for(size_t i = 0; i < messagesDataPtr->size(); i++)  // TODO switch to iterators
            {
                Message* messagePtr = new Message(
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

GraphicManager::GraphicManager()
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

// void GraphicManager::sendMessage(WidgetGroup* groupPtr, const std::string& text)
// {
//     getManagerPtr()->sendMessage(groupPtr, text);
// }

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

GraphicManager* getGraphicManagerPtr()
{
    QCoreApplication* coreApp = QCoreApplication::instance();
    ChatApp* app = dynamic_cast<ChatApp*>(coreApp);
    return app->graphicManagerPtr_;
}
