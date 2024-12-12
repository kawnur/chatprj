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

Message::Message(
//        int companion_id, int author_id, std::tm time,
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

int Companion::getId()
{
    return this->id_;
}

std::string Companion::getName() const
{
    return this->name_;
}

SocketInfo* Companion::getSocketInfoPtr() const
{
    return this->socketInfoPtr_;
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

        if(companionsBuilt)
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
        std::string message { "problem with DB connection" };
        std::vector<std::string> messageVector { message };
        getGraphicManagerPtr()->createErrorDialog(messageVector);
        logArgsError(message);
    }
}

std::pair<int, std::string> Manager::pushMessageToDB(
    const std::string& companionName, const std::string& authorName, const std::string& text)
{
    PGresult* pushToDBResultPtr = pushMessageToDBAndReturn(
        this->dbConnectionPtr_,
        companionName,
        authorName,
        std::string("companion_id"),
        text);

    DBReplyData messageData(2, "companion_id", "timestamp_tz");

    if(!getDataFromDBResult(messageData, pushToDBResultPtr, 1))
    {
        logArgsError("!getDataFromDBResult(messageInfo, pushToDBResultPtr, 1)");
        // companionsDataIsOk = false;
    }

    if(messageData.size() == 0)
    {
        logArgsError("messageInfo.size() == 0");
        return std::pair<int, std::string>(0, "");
    }

    int companionId = std::atoi(messageData.getValue(0, "companion_id"));
    std::string timestampTz { messageData.getValue(0, "timestamp_tz") };

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
    auto findWidget = [&](const std::pair<const Companion*, WidgetGroup*> pair){
        return pair.second->socketInfoBasePtr_ == widget;
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

        dynamic_cast<SocketInfoWidget*>(widgetGroup->socketInfoBasePtr_)->unselect();

        widgetGroup->chatHistoryPtr_->hide();
        widgetGroup->textEditPtr_->hide();
    }
    graphicManagerPtr->oldSelectedCompanionActions(this->selectedCompanionPtr_);

    this->selectedCompanionPtr_ = newSelected;

    if(this->selectedCompanionPtr_)
    {
        auto widgetGroup = this->mapCompanionToWidgetGroup_.at(this->selectedCompanionPtr_);

        dynamic_cast<SocketInfoWidget*>(widgetGroup->socketInfoBasePtr_)->select();

        widgetGroup->chatHistoryPtr_->show();
        widgetGroup->textEditPtr_->show();
    }
    graphicManagerPtr->newSelectedCompanionActions(this->selectedCompanionPtr_);
}

void Manager::addNewCompanion(
    const std::string& name,
    const std::string& ipAddress,
    const std::string& port)
{
    // validate data
    std::vector<std::string> validationErrors {};

    bool validationResult = validateCompanionData(validationErrors, name, ipAddress, port);

    if(validationResult)
    {
        // check if companion with such name already exists
        PGresult* companionDBResultPtr = getCompanionByNameDBResult(this->dbConnectionPtr_, name);
        logArgs("companionDBResultPtr:", companionDBResultPtr);

        if(!companionDBResultPtr)
        {
            std::vector<std::string> dbRequestErrors { "Database request error" };
            getGraphicManagerPtr()->createErrorDialog(dbRequestErrors);
            logArgsError("companionDBResultPtr is nullptr");
            // companionsDataIsOk = false;

            // return companionsDataIsOk;
        }

        DBReplyData companionIdData(1, "id");

        if(!getDataFromDBResult(companionIdData, companionDBResultPtr, 0))
        {
            logArgsError("!getDataFromDBResult(companionsData, companionsDBResultPtr, 0)");
            // companionsDataIsOk = false;

            // return companionsDataIsOk;
        }

        logArgs("companionIdData.size():", companionIdData.size());

        companionIdData.logData();

        // if(false)
        // {

        // }

        // check if such socket already exists
        PGresult* socketDBResultPtr = getSocketByIpAddressAndPortDBResult(this->dbConnectionPtr_, ipAddress, port);
        logArgs("socketDBResultPtr:", socketDBResultPtr);

        if(!socketDBResultPtr)
        {
            logArgsError("socketDBResultPtr is nullptr");
            // companionsDataIsOk = false;

            // return companionsDataIsOk;
        }

        DBReplyData socketIdData(1, "id");

        if(!getDataFromDBResult(socketIdData, socketDBResultPtr, 0))
        {
            logArgsError("!getDataFromDBResult(socketIdData, socketDBResultPtr, 0)");
            // companionsDataIsOk = false;

            // return companionsDataIsOk;
        }


    }
    else
    {
        getGraphicManagerPtr()->createErrorDialog(validationErrors);

    }


    // create Companion object
    // create SocketInfo object
    // add to mapping
    // push to db
    // show result widget

    // delete dialog;
}

bool Manager::buildCompanions()
{
    bool companionsDataIsOk = true;

    PGresult* companionsDBResultPtr = getCompanionsDBResult(this->dbConnectionPtr_);
    logArgs("companionsDBResultPtr:", companionsDBResultPtr);

    if(!companionsDBResultPtr)
    {
        logArgsError("companionsDBResultPtr is nullptr");
        companionsDataIsOk = false;

        return companionsDataIsOk;
    }

    DBReplyData companionsData(2, "id", "name");

    if(!getDataFromDBResult(companionsData, companionsDBResultPtr, 0))
    {
        logArgsError("!getDataFromDBResult(companionsData, companionsDBResultPtr, 0)");
        companionsDataIsOk = false;

        return companionsDataIsOk;
    }

    for(size_t index = 0; index < companionsData.size(); index++)  // TODO switch to iterators
    {
        int id = std::atoi(companionsData.getValue(index, "id"));

        if(id == 0)
        {
            logArgsError("id == 0");
            continue;
        }

        Companion* companionPtr = new Companion(
            id,
            std::string(companionsData.getValue(index, "name")));

        this->companionPtrs_.push_back(companionPtr);

        PGresult* socketInfoDBResultPtr =
            getSocketInfoDBResult(this->dbConnectionPtr_, id);  // TODO switch to getName?

        logArgs("socketInfoDBResultPtr:", socketInfoDBResultPtr);

        if(!socketInfoDBResultPtr)
        {
            logArgsError("socketInfoDBResultPtr is nullptr");
            companionsDataIsOk = false;
        }

        DBReplyData socketsData(3, "ipaddress", "server_port", "client_port");

        if(!getDataFromDBResult(socketsData, socketInfoDBResultPtr, 1))
        {
            logArgsError("!getDataFromDBResult(socketsData, socketInfoDBResultPtr, 1)");
            companionsDataIsOk = false;
        }

        if(socketsData.size() > 0)
        {
            // TODO use port number pool
            SocketInfo* socketInfoPtr = new SocketInfo(
                socketsData.getValue(0, "ipaddress"),
                std::atoi(socketsData.getValue(0, "server_port")),
                std::atoi(socketsData.getValue(0, "client_port")));

            companionPtr->setSocketInfo(socketInfoPtr);
        }

        if(companionPtr->getId() > 1)  // TODO change condition
        {
            PGresult* messagesDBResultPtr = getMessagesDBResult(dbConnectionPtr_, id);

            if(!messagesDBResultPtr)
            {
                logArgsError("messagesDBResultPtr is nullptr");
                companionsDataIsOk = false;
            }

            DBReplyData messagesData(
                5, "companion_id", "author_id", "timestamp_tz", "message", "issent");

            if(!getDataFromDBResult(messagesData, messagesDBResultPtr, 0))
            {
                logArgsWarning("!getDataFromDBResult(messagesData, messagesDBResultPtr, 0)");
            }

            for(size_t i = 0; i < messagesData.size(); i++)  // TODO switch to iterators
            {
                companionPtr->addMessage(
                    id,
                    std::atoi(messagesData.getValue(i, "author_id")),
                    messagesData.getValue(i, "timestamp_tz"),
                    messagesData.getValue(i, "message"),
                    messagesData.getValue(i, "issent"));
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
    auto childrenSize = graphicManagerPtr->getLeftPanelChildrenSize();

    logArgs("companionsSize:", companionsSize);
    logArgs("childrenSize:", childrenSize);

    if(companionsSize == 0 && childrenSize == 0)
    {
        logArgsWarning("strange case, empty sockets panel");
        graphicManagerPtr->addStubWidgetToLeftPanel();
    }
    else
    {
        if(childrenSize != 0)
        {
            // TODO check if sockets already are children

            graphicManagerPtr->removeStubsFromLeftPanel();

            for(auto& companion : this->companionPtrs_)
            {
                WidgetGroup* widgetGroup = new WidgetGroup(companion);
                this->mapCompanionToWidgetGroup_[companion] = widgetGroup;
            }
        }
    }
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
    mainWindowPtr_ = new MainWindow;    
}

void GraphicManager::set()
{
    mainWindowPtr_->set();
    mainWindowPtr_->show();
}

void GraphicManager::addTextToAppLogWidget(const QString& text)
{
    this->mainWindowPtr_->addTextToAppLogWidget(text);
}

void GraphicManager::oldSelectedCompanionActions(const Companion* companion)
{
    this->mainWindowPtr_->oldSelectedCompanionActions(companion);
}

void GraphicManager::newSelectedCompanionActions(const Companion* companion)
{
    this->mainWindowPtr_->newSelectedCompanionActions(companion);
}

size_t GraphicManager::getLeftPanelChildrenSize()
{
    return this->mainWindowPtr_->getLeftPanelChildrenSize();
}

void GraphicManager::addStubWidgetToLeftPanel()
{
    this->mainWindowPtr_->addStubWidgetToLeftPanel();
}

void GraphicManager::removeStubsFromLeftPanel()
{
    this->mainWindowPtr_->removeStubsFromLeftPanel();
}

void GraphicManager::addWidgetToLeftPanel(SocketInfoBaseWidget* widget)
{
    this->mainWindowPtr_->addWidgetToLeftPanel(widget);
}

void GraphicManager::addWidgetToCentralPanel(QWidget* widget)
{
    this->mainWindowPtr_->addWidgetToCentralPanel(widget);
}

void GraphicManager::createErrorDialog(const std::vector<std::string>& messages)
{
    // TODO delete objects for closed dialoges
    ErrorDialog* dialog = new ErrorDialog(this->mainWindowPtr_, messages);
    dialog->show();
}

GraphicManager* getGraphicManagerPtr()
{
    QCoreApplication* coreApp = QCoreApplication::instance();
    ChatApp* app = dynamic_cast<ChatApp*>(coreApp);
    return app->graphicManagerPtr_;
}
