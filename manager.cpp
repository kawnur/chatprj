#include "manager.hpp"

//SocketInfo::SocketInfo(std::string& name, std::string& ipaddress, std::string& port) :
//    name_(name), ipaddress_(ipaddress), port_(port) {
//}

SocketInfo::SocketInfo(std::string& ipaddress, std::string& port) :
    ipaddress_(ipaddress), port_(port) {}

//SocketInfo::SocketInfo(std::string&& name, std::string&& ipaddress, std::string&& port) :
//    name_(std::move(name)),
//    ipaddress_(std::move(ipaddress)),
//    port_(std::move(port)) {
//}

SocketInfo::SocketInfo(std::string&& ipaddress, std::string&& port) :  // TODO ???
    ipaddress_(std::move(ipaddress)),
    port_(std::move(port)) {}

SocketInfo::SocketInfo(const SocketInfo& si)
{
//    name_ = si.name_;
    ipaddress_ = si.ipaddress_;
    port_ = si.port_;
}

//SocketInfo::SocketInfo(std::string& name, std::string& ipaddress, std::string& port) :
// SocketInfo::SocketInfo(std::string& ipaddress, std::string& port) :
// //    name_(std::string::fromStdString(name)),
//     ipaddress_(std::string::fromStdString(ipaddress)),
//     port_(std::string::fromStdString(port)) {}

void SocketInfo::print()
{
//    logArgs("name:", this->name_);
    logArgs("ipaddress:", this->ipaddress_);
    logArgs("port:", this->port_);
}

//std::string SocketInfo::getName() {
//    return this->name_;
//}

std::string SocketInfo::getIpaddress() const
{
    return this->ipaddress_;
}

std::string SocketInfo::getPort() const
{
    return this->port_;
}

uint16_t SocketInfo::getPortInt() const
{
    return std::stoi(this->port_, nullptr, 10);
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

Companion::Companion(int id, std::string&& name) : id_(id), name_(name) {}

Companion::~Companion()
{
    delete socketInfoPtr_;
    delete clientPtr_;
    delete serverPtr_;
}

bool Companion::initMessaging()
{
    bool initialized = false;

    try
    {
        // io_contextPtr_ = new boost::asio::io_context;

        // serverPtr_ = new ChatServer(
        //     *io_contextPtr_,
        //     this->socketInfoPtr_->getPortInt());
        this->serverPtr_ = new ChatServer(this->socketInfoPtr_->getPortInt());

        this->serverPtr_->run();

        this->clientPtr_ = new ChatClient(
            this->socketInfoPtr_->getIpaddress(),
            this->socketInfoPtr_->getPort());

        this->clientPtr_->connect();

        initialized = true;
    }
    catch(std::exception& e)
    {
        logArgsError(e.what());
    }

    return initialized;
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
}

int Companion::getId()
{
    return this->id_;
}

std::string Companion::getName() const
{
    return this->name_;
}

SocketInfo* Companion::getSocketInfo() const
{
    return this->socketInfoPtr_;
}

Manager::Manager()
{
    dbConnectionPtr_ = nullptr;
}

Manager::~Manager()
{
    for(auto& companion : this->companionPtrs_)
    {
        delete companion;
    }
}

void Manager::set()
{
    bool connectedToDB = this->connectToDb();
    logArgs("connectedToDB:", connectedToDB);

    if(connectedToDB)
    {
        bool companionsBuilt = this->buildCompanions();
        logArgs("companionsBuilt:", companionsBuilt);

        if(companionsBuilt)
        {
            MainWindow* mainWindow = getMainWindowPtr();
            mainWindow->buildWidgetGroups(&this->companionPtrs_);
        }
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

bool Manager::buildCompanions()
{
    bool companionsDataIsOk = true;

    PGresult* companionsDBResultPtr = getCompanionsDBResult(dbConnectionPtr_);
    logArgs("companionsDBResultPtr:", companionsDBResultPtr);

    if(companionsDBResultPtr == nullptr)
    {
        logArgsError("companionsDBResultPtr == nullptr");
        companionsDataIsOk = false;
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
    }

    for(auto& data : companionsData)
    {
        int id = std::atoi(data.at(std::string("id")));

        Companion* companionPtr = new Companion(
            id,
            std::string(data.at(std::string("name"))));

        this->companionPtrs_.push_back(companionPtr);

        PGresult* socketInfoDBResultPtr =
            getSocketInfoDBResult(dbConnectionPtr_, id);  // TODO switch to getName?

        logArgs("socketInfoDBResultPtr:", socketInfoDBResultPtr);

        if(socketInfoDBResultPtr == nullptr)
        {
            logArgsError("socketInfoDBResultPtr == nullptr");
            companionsDataIsOk = false;
        }

        std::map<std::string, const char*> socketsDataMapping
            {
                {std::string("ipaddress"), nullptr},
                {std::string("port"), nullptr}
            };

        std::vector<std::map<std::string, const char*>> socketsData { socketsDataMapping };

        if(!getDataFromDBResult(socketsData, socketInfoDBResultPtr, 1))
        {
            logArgsError("!getDataFromDBResult(socketsData, socketInfoDBResultPtr, 1)");
            companionsDataIsOk = false;
        }

        if(socketsData.size() == 0)
        {
            continue;
        }

        SocketInfo* socketInfoPtr = new SocketInfo(
            socketsData.at(0).at("ipaddress"),
            socketsData.at(0).at("port"));

        companionPtr->setSocketInfo(socketInfoPtr);

        if(companionPtr->getId() > 1)  // TODO change condition
        {
            PGresult* messagesDBResultPtr = getMessagesDBResult(dbConnectionPtr_, id);

            if(messagesDBResultPtr == nullptr)
            {
                logArgsError("messagesDBResultPtr == nullptr");
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

            companionPtr->initMessaging();
        }
    }

    return companionsDataIsOk;
}

bool Manager::connectToDb()
{
    bool connected = false;
    dbConnectionPtr_ = getDBConnection();

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
