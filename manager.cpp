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
    bool initialized = false;

    try
    {
        // io_contextPtr_ = new boost::asio::io_context;

        // serverPtr_ = new ChatServer(
        //     *io_contextPtr_,
        //     this->socketInfoPtr_->getPortInt());

        this->serverPtr_ = new ChatServer(this->socketInfoPtr_->getServerPort());

        this->serverPtr_->run();

        initialized = true;
    }
    catch(std::exception& e)
    {
        logArgsError(e.what());
    }

    return initialized;
}

bool Companion::createClient()
{
    bool created = false;

    try
    {
        // io_contextPtr_ = new boost::asio::io_context;

        // serverPtr_ = new ChatServer(
        //     *io_contextPtr_,
        //     this->socketInfoPtr_->getPortInt());

        this->clientPtr_ = new ChatClient(
            this->socketInfoPtr_->getIpAddress(),
            this->socketInfoPtr_->getClientPort());

        // created = this->clientPtr_->connect();
        created = true;
    }
    catch(std::exception& e)
    {
        logArgsError(e.what());
    }

    return created;
}

bool Companion::connectClient()
{
    bool connected = false;

    try
    {
        // io_contextPtr_ = new boost::asio::io_context;

        // serverPtr_ = new ChatServer(
        //     *io_contextPtr_,
        //     this->socketInfoPtr_->getPortInt());

        // this->clientPtr_ = new ChatClient(
        //     this->socketInfoPtr_->getIpAddress(),
        //     this->socketInfoPtr_->getClientPort());

        connected = this->clientPtr_->connect();
    }
    catch(std::exception& e)
    {
        logArgsError(e.what());
    }

    return connected;
}

void Companion::setSocketInfo(SocketInfo* socketInfo)
{
    socketInfoPtr_ = socketInfo;
}

void Companion::addMessage(
//        int companion_id, int author_id, std::tm time,
        int companion_id, int author_id, const std::string& time,
        const std::string& text, bool isSent)
{
    this->messages_.emplace_back(companion_id, author_id, time, text, isSent);
}

const std::vector<Message>* Companion::getMessagesPtr() const
{
    return &this->messages_;
}

bool Companion::sendLastMessage()
{
    auto text = this->messages_.back().getText();
    auto sendResult = this->clientPtr_->send(text);

    return true;
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
            logArgsError("problems with companions initialization");
        }
    }
    else
    {
        logArgsError("problems with DB connection");
    }
}

void Manager::sendMessage(const Companion* companion, WidgetGroup* group)
{
    auto findCompanion = [&](Companion* cmp){ return cmp == companion; };

    Companion* companionPtr = *std::find_if(
        this->companionPtrs_.cbegin(),
        this->companionPtrs_.cend(),
        findCompanion);

    auto text = group->textEditPtr_->toPlainText().toStdString();

    // encrypt message

    // add to DB and get timestamp

    PGresult* pushToDBResultPtr = pushMessageToDBAndReturn(
        this->dbConnectionPtr_,
        companionPtr->getName(),
        text);

    std::map<std::string, const char*> messageInfoMapping
        {
            {std::string("companion_id"), nullptr},
            {std::string("timestamp_tz"), nullptr}
        };

    std::vector<std::map<std::string, const char*>> messageInfo { messageInfoMapping };

    if(!getDataFromDBResult(messageInfo, pushToDBResultPtr, 1))
    {
        logArgsError("!getDataFromDBResult(messageInfo, pushToDBResultPtr, 1)");
        // companionsDataIsOk = false;
    }

    int companionId = std::atoi(messageInfo.at(0).at("companion_id"));
    std::string timestampTz { messageInfo.at(0).at("timestamp_tz") };

    logArgs("companion_id:", companionId, "timestamp_tz:", timestampTz);

    // add to companion's messages

    companionPtr->addMessage(companionId, 1, timestampTz, text, false);

    // add to widget

    auto textFormatted = group->formatMessage(
                companionPtr, &companionPtr->getMessagesPtr()->back());
    group->addMessageToChatHistory(textFormatted);

    // send over network

    companionPtr->sendLastMessage();
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
    MainWindow* mainWindow = getMainWindowPtr();

    if(this->selectedCompanion_)
    {
        auto widgetGroup = this->mapCompanionToWidgetGroup_.at(this->selectedCompanion_);  // TODO try catch

        dynamic_cast<SocketInfoWidget*>(widgetGroup->socketInfoBasePtr_)->unselect();

        widgetGroup->chatHistoryPtr_->hide();
        widgetGroup->textEditPtr_->hide();
    }
    mainWindow->oldSelectedCompanionActions(this->selectedCompanion_);

    this->selectedCompanion_ = newSelected;

    if(this->selectedCompanion_)
    {
        auto widgetGroup = this->mapCompanionToWidgetGroup_.at(this->selectedCompanion_);

        dynamic_cast<SocketInfoWidget*>(widgetGroup->socketInfoBasePtr_)->select();

        widgetGroup->chatHistoryPtr_->show();
        widgetGroup->textEditPtr_->show();
    }
    mainWindow->newSelectedCompanionActions(this->selectedCompanion_);
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

    std::map<std::string, const char*> companionsDataMapping
    {
        {std::string("id"), nullptr},
        {std::string("name"), nullptr}
    };

    std::vector<std::map<std::string, const char*>> companionsData { companionsDataMapping };

    if(!getDataFromDBResult(companionsData, companionsDBResultPtr, 0))
    {
        logArgsError("!getDataFromDBResult(companionsData, companionsDBResultPtr, 0)");
        companionsDataIsOk = false;

        return companionsDataIsOk;
    }

    for(auto& data : companionsData)
    {
        int id = std::atoi(data.at(std::string("id")));

        if(id == 0)
        {
            logArgsError("id == 0");
            continue;
        }

        Companion* companionPtr = new Companion(
            id,
            std::string(data.at(std::string("name"))));

        this->companionPtrs_.push_back(companionPtr);

        PGresult* socketInfoDBResultPtr =
            getSocketInfoDBResult(this->dbConnectionPtr_, id);  // TODO switch to getName?

        logArgs("socketInfoDBResultPtr:", socketInfoDBResultPtr);

        if(!socketInfoDBResultPtr)
        {
            logArgsError("socketInfoDBResultPtr is nullptr");
            companionsDataIsOk = false;
        }

        std::map<std::string, const char*> socketsDataMapping
        {
            {std::string("ipaddress"), nullptr},
            {std::string("server_port"), nullptr},
            {std::string("client_port"), nullptr}
        };

        std::vector<std::map<std::string, const char*>> socketsData { socketsDataMapping };

        if(!getDataFromDBResult(socketsData, socketInfoDBResultPtr, 1))
        {
            logArgsError("!getDataFromDBResult(socketsData, socketInfoDBResultPtr, 1)");
            companionsDataIsOk = false;
        }

        if(socketsData.size() > 0)
        {
            // TODO use port number pool
            SocketInfo* socketInfoPtr = new SocketInfo(
                socketsData.at(0).at("ipaddress"),
                std::atoi(socketsData.at(0).at("server_port")),
                std::atoi(socketsData.at(0).at("client_port")));

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

            std::map<std::string, const char*> messagesDataMapping
            {
                {std::string("companion_id"), nullptr},
                {std::string("author_id"), nullptr},
                {std::string("timestamp_tz"), nullptr},
                {std::string("message"), nullptr},
                {std::string("issent"), nullptr},
            };

            std::vector<std::map<std::string, const char*>> messagesData { messagesDataMapping };

            if(!getDataFromDBResult(messagesData, messagesDBResultPtr, 0))
            {
                logArgsWarning("!getDataFromDBResult(messagesData, messagesDBResultPtr, 0)");
            }

            for(auto& message : messagesData)
            {
                companionPtr->addMessage(
                    id,
                    std::atoi(message.at("author_id")),
                    message.at("timestamp_tz"),
                    message.at("message"),
                    message.at("issent"));
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
    MainWindow* mainWindow = getMainWindowPtr();

    auto companionsSize = this->companionPtrs_.size();
    auto childrenSize = mainWindow->getLeftPanelChildrenSize();

    logArgs("companionsSize:", companionsSize);
    logArgs("childrenSize:", childrenSize);

    if(companionsSize == 0 && childrenSize == 0)
    {
        logArgsWarning("strange case, empty sockets panel");
        mainWindow->addStubWidgetToLeftPanel();
    }
    else
    {
        if(childrenSize != 0)
        {
            // TODO check if sockets already are children

            mainWindow->removeStubsFromLeftPanel();

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
