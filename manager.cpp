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
    id_(id), name_(name), socketInfoPtr_(nullptr),
    clientPtr_(nullptr), serverPtr_(nullptr), messages_(std::vector<Message>())
{}

Companion::Companion(int id, std::string&& name) :
    id_(id), name_(name), socketInfoPtr_(nullptr),
    clientPtr_(nullptr), serverPtr_(nullptr), messages_(std::vector<Message>())
{}

Companion::~Companion()
{
    delete socketInfoPtr_;
    delete clientPtr_;
    delete serverPtr_;
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

void Companion::addMessage(
        int companion_id, int author_id, const std::string& time,
        const std::string& text, bool isSent)
{
    this->messages_.emplace_back(companion_id, author_id, time, text, isSent);
}

const std::vector<Message>* Companion::getMessagesPtr() const
{
    return &this->messages_;
}

void Companion::sendLastMessage()
{
    auto text = this->messages_.back().getText();
    auto sendResult = this->clientPtr_->send(text);

    if(!sendResult)
    {
        logArgsError("client message sending error");
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

CompanionAction::CompanionAction(
    CompanionActionType actionType, MainWindow* mainWindowPtr, Companion* companionPtr) :
    actionType_(actionType), mainWindowPtr_(mainWindowPtr), companionPtr_(companionPtr),
    dataPtr_(nullptr)
{
    switch(actionType)
    {
    case CompanionActionType::CREATE:
    case CompanionActionType::UPDATE:
        formDialogPtr_ = new CompanionDataDialog(actionType_, mainWindowPtr_, companionPtr_);
        deleteDialogPtr_ = nullptr;
        break;

    case CompanionActionType::DELETE:
        formDialogPtr_ = nullptr;
        deleteDialogPtr_ = new TwoButtonsTextDialog(
            nullptr, this->mainWindowPtr_, DialogType::WARNING,
            deleteCompanionDialogText, std::string("Delete companion"));
        break;

    case CompanionActionType::CLEAR_HISTORY:
        formDialogPtr_ = nullptr;
        deleteDialogPtr_ = new TwoButtonsTextDialog(
            nullptr, this->mainWindowPtr_, DialogType::WARNING,
            clearCompanionHistoryDialogText, std::string("Clear history"));
        break;
    }
}

CompanionAction::~CompanionAction()
{
    delete this->dataPtr_;
}

void CompanionAction::set()
{
    switch(this->actionType_)
    {
    case CompanionActionType::CREATE:
    case CompanionActionType::UPDATE:
        this->formDialogPtr_->set(this);
        this->formDialogPtr_->show();
        break;

    case CompanionActionType::DELETE:
    case CompanionActionType::CLEAR_HISTORY:
        this->deleteDialogPtr_->set(this);
        this->deleteDialogPtr_->show();
        break;
    }
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

CompanionDataDialog* CompanionAction::getFormDialogPtr()
{
    return this->formDialogPtr_;
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
        name = this->formDialogPtr_->getNameString();
        ipAddress = this->formDialogPtr_->getIpAddressString();
        clientPort = this->formDialogPtr_->getPortString();
        break;

    case CompanionActionType::DELETE:
    case CompanionActionType::CLEAR_HISTORY:
        name = this->companionPtr_->getName();
        ipAddress = this->companionPtr_->getIpAddress();
        clientPort = std::to_string(this->companionPtr_->getClientPort());
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

Manager::Manager() :
    dbConnectionPtr_(nullptr), companionPtrs_(std::vector<Companion*>())
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
        showErrorDialogAndLogError("problem with DB connection");
    }
}

std::pair<int, std::string> Manager::pushMessageToDB(
    const std::string& companionName, const std::string& authorName, const std::string& text)
{
    std::shared_ptr<DBReplyData> messageDataPtr = this->getDBDataPtr(
        true,
        "pushMessageToDBAndReturn",
        &pushMessageToDBAndReturn,
        std::vector<std::string> {
            std::string("companion_id"), std::string("timestamp_tz")
        },
        companionName, authorName, std::string("companion_id"), text);

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

void Manager::sendMessage(WidgetGroup* groupPtr, const std::string& text)
{
    Companion* companionPtr =
        const_cast<Companion*>(this->getMappedCompanionByWidgetGroup(groupPtr));

    // encrypt message

    // add to DB and get timestamp

    auto pair = this->pushMessageToDB(companionPtr->getName(), std::string("me"), text);

    if(pair.first == 0 || pair.second == "")
    {
        logArgsError("error adding message to db");
        return;
    }

    // add to companion's messages

    companionPtr->addMessage(pair.first, 1, pair.second, text, false);

    // add to widget

    auto textFormatted = groupPtr->formatMessage(
        companionPtr, &companionPtr->getMessagesPtr()->back());

    groupPtr->addMessageToChatHistory(textFormatted);

    // send over network

    companionPtr->sendLastMessage();
}

void Manager::sendMessage(Companion* companionPtr, const std::string& text)
{
    WidgetGroup* groupPtr = this->mapCompanionToWidgetGroup_.at(companionPtr);

    // encrypt message

    // add to DB and get timestamp

    auto pair = this->pushMessageToDB(companionPtr->getName(), std::string("me"), text);

    if(pair.first == 0 || pair.second == "")
    {
        logArgsError("error adding message to db");
        return;
    }

    // add to companion's messages

    companionPtr->addMessage(pair.first, 1, pair.second, text, false);

    // add to widget

    auto textFormatted = groupPtr->formatMessage(
                companionPtr, &companionPtr->getMessagesPtr()->back());

    groupPtr->addMessageToChatHistory(textFormatted);

    // send over network

    companionPtr->sendLastMessage();
}

void Manager::receiveMessage(Companion* companionPtr, const std::string& text)
{
    // add to DB and get timestamp

    auto pair = this->pushMessageToDB(companionPtr->getName(), companionPtr->getName(), text);

    if(pair.first == 0 || pair.second == "")
    {
        logArgsError("error adding message to db");
        return;
    }

    // add to companion's messages

    companionPtr->addMessage(pair.first, pair.first, pair.second, text, false);

    // decrypt message

    // add to widget

    WidgetGroup* groupPtr = this->mapCompanionToWidgetGroup_.at(companionPtr);  // TODO try catch

    auto textFormatted = groupPtr->formatMessage(
        companionPtr, &companionPtr->getMessagesPtr()->back());

    groupPtr->addMessageToChatHistory(textFormatted);
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
        showErrorDialogAndLogError("Error getting data from db");
        return false;
    }

    if(!companionIdDataPtr->isEmpty())
    {
        showErrorDialogAndLogError("Companion with such name already exists");
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
        showErrorDialogAndLogError("Error getting data from db");
        return false;
    }

    if(!socketIdDataPtr->isEmpty())
    {
        showErrorDialogAndLogError("Companion with such socket already exists");
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
        showErrorDialogAndLogError("Error getting data from db");
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
        showWarningDialogAndLogWarning("Companion with such name already exists");
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
        showErrorDialogAndLogError("Error getting data from db");
        return false;
    }

    bool findSocketResult = socketIdDataPtr->findValue(
        std::string("id"),
        std::to_string(companionActionPtr->getCompanionId()));

    bool socketExistsAtOtherCompanion =
        (findSocketResult && companionIdDataPtr->size() > 1) ||
        (!findSocketResult && companionIdDataPtr->size() > 0);

    if(socketExistsAtOtherCompanion)
    {
        showErrorDialogAndLogError("Companion with such socket already exists");
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
        showErrorDialogAndLogError("Companion was not found in mapping at deletion");
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
            showErrorDialogAndLogError("Companion was not found in vector at deletion");
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
        getGraphicManagerPtr()->createTextDialog(
            nullptr,
            DialogType::ERROR,
            buildDialogText(
                std::string { "Error messages:\n\n" }, validationErrors));

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
        showErrorDialogAndLogError("Error getting data from db");
        return;
    }

    if(companionIdDataPtr->isEmpty())
    {
        showErrorDialogAndLogError("Empty db reply to new companion pushing");
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
        showErrorDialogAndLogError("Error getting data from db");
        return;
    }

    if(socketDataPtr->isEmpty())
    {
        showErrorDialogAndLogError("Empty db reply to new socket pushing");
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
        showErrorDialogAndLogError("Error getting data from db");
        return;
    }

    if(companionIdDataPtr->isEmpty())
    {
        showErrorDialogAndLogError("Empty db reply to companion update");
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
        showErrorDialogAndLogError("Error getting data from db");
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
        showErrorDialogAndLogError("Error getting data from db");
        return;
    }

    if(companionIdCompanionDataPtr->isEmpty())
    {
        showErrorDialogAndLogError("Empty db reply to companion deletion");
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
        showErrorDialogAndLogError("Error getting data from db");
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
                companionPtr->addMessage(
                    id,
                    std::atoi(messagesDataPtr->getValue(i, "author_id")),
                    messagesDataPtr->getValue(i, "timestamp_tz"),
                    messagesDataPtr->getValue(i, "message"),
                    messagesDataPtr->getValue(i, "issent"));
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

void GraphicManager::sendMessage(WidgetGroup* groupPtr, const std::string& text)
{
    getManagerPtr()->sendMessage(groupPtr, text);
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

void GraphicManager::createTextDialog(
    QDialog* parentDialog, const DialogType dialogType, const std::string& message)
{
    // TODO delete objects for closed dialoges
    TextDialog* dialog = new TextDialog(parentDialog, this->mainWindowPtr_, dialogType, message);
    dialog->set(parentDialog);
    dialog->show();
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
    this->createTextDialog(
        companionActionPtr->getFormDialogPtr(),
        DialogType::INFO,
        buildDialogText(
            std::move(header),
            std::vector<std::string> {
                std::string("name: ") + companionActionPtr->getName(),
                std::string("ipAddress: ") + companionActionPtr->getIpAddress(),
                std::string("port: ") + companionActionPtr->getClientPort() }));

    delete companionActionPtr;
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

void GraphicManager::hideInfo()
{
    this->stubWidgetsPtr_->setLeftPanelWidth(
        this->mainWindowPtr_->getLeftPanelWidgetWidth());

    this->mainWindowPtr_->hideLeftAndRightPanels();

    getManagerPtr()->hideSelectedCompanionCentralPanel();

    this->stubWidgetsPtr_->showStubPanels();
}

void GraphicManager::showInfo()
{
    this->mainWindowPtr_->showLeftAndRightPanels();

    getManagerPtr()->showSelectedCompanionCentralPanel();

    this->stubWidgetsPtr_->hideStubPanels();
}

MainWindow* GraphicManager::getMainWindowPtr()
{
    return this->mainWindowPtr_;
}

GraphicManager* getGraphicManagerPtr()
{
    QCoreApplication* coreApp = QCoreApplication::instance();
    ChatApp* app = dynamic_cast<ChatApp*>(coreApp);
    return app->graphicManagerPtr_;
}
