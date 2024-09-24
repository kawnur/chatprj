#include "manager.hpp"

//SocketInfo::SocketInfo(QString& name, QString& ipaddress, QString& port) :
//    name_(name), ipaddress_(ipaddress), port_(port) {
//}

SocketInfo::SocketInfo(QString& ipaddress, QString& port) :
    ipaddress_(ipaddress), port_(port) {}

//SocketInfo::SocketInfo(QString&& name, QString&& ipaddress, QString&& port) :
//    name_(std::move(name)),
//    ipaddress_(std::move(ipaddress)),
//    port_(std::move(port)) {
//}

SocketInfo::SocketInfo(QString&& ipaddress, QString&& port) :
    ipaddress_(std::move(ipaddress)),
    port_(std::move(port)) {}

SocketInfo::SocketInfo(const SocketInfo& si)
{
//    name_ = si.name_;
    ipaddress_ = si.ipaddress_;
    port_ = si.port_;
}

//SocketInfo::SocketInfo(std::string& name, std::string& ipaddress, std::string& port) :
SocketInfo::SocketInfo(std::string& ipaddress, std::string& port) :
//    name_(QString::fromStdString(name)),
    ipaddress_(QString::fromStdString(ipaddress)),
    port_(QString::fromStdString(port)) {}

void SocketInfo::print()
{
//    logArgs("name:", this->name_);
    logArgs("ipaddress:", this->ipaddress_);
    logArgs("port:", this->port_);
}

//QString SocketInfo::getName() {
//    return this->name_;
//}

QString SocketInfo::getIpaddress() const
{
    return this->ipaddress_;
}

QString SocketInfo::getPort() const
{
    return this->port_;
}

uint16_t SocketInfo::getPortInt() const
{
    return this->port_.toInt(nullptr, 10);
}

Message::Message(
//        int companion_id, int author_id, std::tm time,
        int companion_id, int author_id, const QString& time,
        const QString& text, bool isSent) :
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

QString Message::getTime() const
{
    return this->time_;
}

QString Message::getText() const
{
    return this->text_;
}

bool Message::getIsSent() const
{
    return this->isSent_;
}

Companion::Companion(int id, QString&& name) : id_(id), name_(name) {}

bool Companion::initMessaging()
{
    bool initialized = false;

    try
    {
        clientPtr_ = new ChatClient;
        io_contextPtr_ = new boost::asio::io_context;

        serverPtr_ = new ChatServer(
            *io_contextPtr_,
            this->socketInfoPtr_->getPortInt());

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
        int companion_id, int author_id, const QString& time,
        const QString& text, bool isSent)
{
    this->messages_.emplace_back(companion_id, author_id, time, text, isSent);
}

const std::vector<Message>* Companion::getMessagesPtr() const
{
    return &this->messages_;
}

int Companion::getId()
{
    return this->id_;
}

QString Companion::getName() const
{
    return this->name_;
}

SocketInfo* Companion::getSocketInfo() const
{
    return this->socketInfoPtr_;
}

Manager::Manager()
{
    dbConnection_ = nullptr;
}

void Manager::set()
{
    this->connectToDb();
    this->buildCompanions();

    MainWindow* mainWindow = getMainWindowPtr();
    mainWindow->buildWidgetGroups(&this->companions_);
}

void Manager::sendMessage(const Companion* companion, WidgetGroup* group)
{
    auto findCompanion = [&](Companion* cmp){ return cmp == companion; };

    Companion* companionPtr = *std::find_if(
                this->companions_.cbegin(),
                this->companions_.cend(),
                findCompanion);

    auto text = group->textEdit_->toPlainText();

    // encrypt message
    // add to DB

    PGresult* pushToDBResult = pushMessageToDBAndReturn(
                this->dbConnection_,
                companion->getName(),
                text);

    std::map<std::string, const char*> messageInfoMapping
        {
            {std::string("companion_id"), nullptr},
            {std::string("timestamp_tz"), nullptr}
        };

    std::vector<std::map<std::string, const char*>> messageInfo { messageInfoMapping };

    if(!getDataFromDBResult(messageInfo, pushToDBResult, 1))
    {
        int companionId = std::atoi(messageInfo.at(0).at("companion_id"));
        QString timestampTz { messageInfo.at(0).at("timestamp_tz") };

        logArgs("companion_id:", companionId, "timestamp_tz:", timestampTz);

        // add to companion's messages

        companionPtr->addMessage(companionId, 1, timestampTz, text, false);

        // add to widget

        auto textFormatted = group->formatMessage(
                    companion, &companion->getMessagesPtr()->back());
        group->addMessageToChatHistory(textFormatted);

        // send over network

    }
}

void Manager::buildCompanions()
{
    PGresult* companionsDBResult = getCompanionsDBResult(dbConnection_);
    logArgs("companionsDBResult:", companionsDBResult);

    if(companionsDBResult == nullptr)
    {
        return;
    }

    std::map<std::string, const char*> companionsDataMapping
    {
        {std::string("id"), nullptr},
        {std::string("name"), nullptr}
    };

    std::vector<std::map<std::string, const char*>> companionsData { companionsDataMapping };

    if(!getDataFromDBResult(companionsData, companionsDBResult, 0))
    {
        for(auto& data : companionsData)
        {
            int id = std::atoi(data.at(std::string("id")));

            Companion* companion = new Companion(
                id,
                QString(data.at(std::string("name"))));

            this->companions_.push_back(companion);

            PGresult* socketInfoDBResult =
                getSocketInfoDBResult(dbConnection_, id);  // TODO switch to getName?

            logArgs("socketInfoDBResult:", socketInfoDBResult);

            if(socketInfoDBResult == nullptr)
            {
                return;
            }

            std::map<std::string, const char*> socketsDataMapping
            {
                {std::string("ipaddress"), nullptr},
                {std::string("port"), nullptr}
            };

            std::vector<std::map<std::string, const char*>> socketsData { socketsDataMapping };

            if(!getDataFromDBResult(socketsData, socketInfoDBResult, 1))
            {
                if(socketsData.size() == 0)
                {
                    continue;
                }

                SocketInfo* socketInfo = new SocketInfo(
                    socketsData.at(0).at("ipaddress"),
                    socketsData.at(0).at("port"));

                companion->setSocketInfo(socketInfo);

                PGresult* messagesDBResult = getMessagesDBResult(dbConnection_, id);

                if(messagesDBResult == nullptr)
                {
                    return;
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

                if(!getDataFromDBResult(messagesData, messagesDBResult, 0))
                {
                    for(auto& message : messagesData)
                    {
                        companion->addMessage(
                            id,
                            std::atoi(message.at("author_id")),
                            message.at("timestamp_tz"),
                            message.at("message"),
                            message.at("issent"));
                    }
                }
            }
        }

        for(auto& companionPtr : companions_)
        {
            companionPtr->initMessaging();
        }
    }
    else {}
}

bool Manager::connectToDb()
{
    bool connected = false;
    dbConnection_ = getDBConnection();

    ConnStatusType status = PQstatus(dbConnection_);
    logArgs("status: ", std::to_string(status));

    if(status == ConnStatusType::CONNECTION_BAD)  // TODO raise exception
    {
        ("DB connection status: CONNECTION_BAD");
    }

    return connected;
}

Manager* getManagerPtr()
{
    QCoreApplication* coreApp = QCoreApplication::instance();
    ChatApp* app = dynamic_cast<ChatApp*>(coreApp);
    return app->manager_;
}
