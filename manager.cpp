#include "manager.hpp"

//SocketInfo::SocketInfo(QString& name, QString& ipaddress, QString& port) :
//    name_(name), ipaddress_(ipaddress), port_(port) {
//}

SocketInfo::SocketInfo(QString& ipaddress, QString& port) :
    ipaddress_(ipaddress), port_(port) {
}

//SocketInfo::SocketInfo(QString&& name, QString&& ipaddress, QString&& port) :
//    name_(std::move(name)),
//    ipaddress_(std::move(ipaddress)),
//    port_(std::move(port)) {
//}

SocketInfo::SocketInfo(QString&& ipaddress, QString&& port) :
    ipaddress_(std::move(ipaddress)),
    port_(std::move(port)) {
}

SocketInfo::SocketInfo(const SocketInfo& si) {
//    name_ = si.name_;
    ipaddress_ = si.ipaddress_;
    port_ = si.port_;
}

//SocketInfo::SocketInfo(std::string& name, std::string& ipaddress, std::string& port) :
SocketInfo::SocketInfo(std::string& ipaddress, std::string& port) :
//    name_(QString::fromStdString(name)),
    ipaddress_(QString::fromStdString(ipaddress)),
    port_(QString::fromStdString(port)) {
}

void SocketInfo::print() {
//    logArgs("name:", this->name_);
    logArgs("ipaddress:", this->ipaddress_);
    logArgs("port:", this->port_);
}

//QString SocketInfo::getName() {
//    return this->name_;
//}

QString SocketInfo::getIpaddress() const {
    return this->ipaddress_;
}

QString SocketInfo::getPort() const {
    return this->port_;
}

Message::Message(int companion_id, std::tm time, const QString& text, bool isSent) :
    companion_id_(companion_id), time_(time), text_(text), isSent_(isSent) {}

Companion::Companion(int id, QString& name) : id_(id), name_(name) {
    inputBuffer_ = new QString;
}

void Companion::setSocketInfo(SocketInfo* socketInfo) {
    socketInfo_ = socketInfo;
}

void Companion::addMessage(
        int companion_id, std::tm time, const QString& text, bool isSent) {

    this->messages_.emplace_back(companion_id, time, text, isSent);
}

int Companion::getId() {
    return this->id_;
}

QString Companion::getName() const {
    return this->name_;
}

SocketInfo* Companion::getSocketInfo() const {
    return this->socketInfo_;
}

QString* Companion::getInputBuffer() const {
    return this->inputBuffer_;
}

void Companion::setInputBuffer(const QString& text) {
    *this->inputBuffer_ = text;
}

Manager::Manager() {
    dbConnection_ = nullptr;
}

void Manager::set() {
    this->connectToDb();
    this->buildCompanions();

    MainWindow* mainWindow = getMainWindowPtr();
    mainWindow->buildSocketInfoWidgets(&this->companions_);
}

void Manager::buildCompanions() {
    PGresult* companionsDBResult = getCompanionsDBResult(dbConnection_);
    logArgs("companionsDBResult:", companionsDBResult);

    if(companionsDBResult == nullptr) {
        return;
    }

    std::vector<std::pair<int, QString>> companionsData;
    getCompanionsDataFromDBResult(&companionsData, companionsDBResult);

    for(auto& data : companionsData) {
        Companion* companion = new Companion(data.first, data.second);
        this->companions_.push_back(companion);

        PGresult* socketInfoDBResult =
                getSocketInfoDBResult(dbConnection_, data.first);  // TODO switch to getName?

        logArgs("socketInfoDBResult:", socketInfoDBResult);

        if(socketInfoDBResult == nullptr) {
            return;
        }

        std::pair<QString, QString> socketInfoData =
                getSocketInfoDataFromDBResult(socketInfoDBResult);

        SocketInfo* socketInfo = new SocketInfo(
                    socketInfoData.first,
                    socketInfoData.second);

        companion->setSocketInfo(socketInfo);

        PGresult* messagesDBResult =
                getMessagesDBResult(dbConnection_, data.first);

        if(messagesDBResult == nullptr) {
            return;
        }

        getMessagesDataFromDBResultAndAdd(companion, messagesDBResult);

    }
}

void Manager::connectToDb() {
    dbConnection_ = getDBConnection();

    ConnStatusType status = PQstatus(dbConnection_);
    logArgs("status: ", std::to_string(status));

    if(status == ConnStatusType::CONNECTION_BAD) {  // TODO raise exception
        logArgs("DB connection status: CONNECTION_BAD");
    }
}

Manager* getManagerPtr() {
    QCoreApplication* coreApp = QCoreApplication::instance();
    ChatApp* app = dynamic_cast<ChatApp*>(coreApp);
    return app->manager_;
}
