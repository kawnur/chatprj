#include "manager.hpp"

SocketInfo::SocketInfo(QString& name, QString& ipaddress, QString& port) :
    name_(name), ipaddress_(ipaddress), port_(port) {
}

SocketInfo::SocketInfo(QString&& name, QString&& ipaddress, QString&& port) :
    name_(std::move(name)),
    ipaddress_(std::move(ipaddress)),
    port_(std::move(port)) {
}

SocketInfo::SocketInfo(const SocketInfo& si) {
    name_ = si.name_;
    ipaddress_ = si.ipaddress_;
    port_ = si.port_;
}

SocketInfo::SocketInfo(std::string& name, std::string& ipaddress, std::string& port) :
    name_(QString::fromStdString(name)),
    ipaddress_(QString::fromStdString(ipaddress)),
    port_(QString::fromStdString(port)) {
}

void SocketInfo::print() {
    logArgs("name:", this->name_);
    logArgs("ipaddress:", this->ipaddress_);
    logArgs("port:", this->port_);
}

QString SocketInfo::getName() {
    return this->name_;
}

QString SocketInfo::getIpaddress() {
    return this->ipaddress_;
}

QString SocketInfo::getPort() {
    return this->port_;
}

Manager::Manager() {
    dbConnection_ = nullptr;
    sockets_ = std::vector<SocketInfo>();
}

Manager::~Manager() {
    delete[] &sockets_;
}

void Manager::set() {
    this->connectToDb();
    this->buildSockets();

    MainWindow* mainWindow = getMainWindowPtr();
    mainWindow->buildSocketInfoWidgets(&this->sockets_);
}

void Manager::buildSockets() {
    QString name, ipaddress, port;

    PGresult* result = getsSocketsInfo(dbConnection_);
    logArgs("result:", result);

    if(result == nullptr) {
//        this->sockets_.emplace_back("No socket info from DB...", "", "");
        return;
    }

    int ntuples = PQntuples(result);
    logArgs("ntuples:", ntuples);

    int nfields = PQnfields(result);
    logArgs("nfields:", nfields);

    for(int i = 0; i < ntuples; i++) {
        for(int j = 0; j < nfields; j++) {
            char* fname = PQfname(result, j);
            std::string fnameStr = (fname == nullptr)
                    ? "nullptr" : std::string(fname);
            logArgs("fname:", fnameStr);

            if(fnameStr == "name") {
                name = PQgetvalue(result, i, j);
                logArgs("value:", name);
            }
            else if(fnameStr == "ipaddress") {
                ipaddress = PQgetvalue(result, i, j);
                logArgs("value:", ipaddress);
            }
            else if(fnameStr == "port") {
                port = PQgetvalue(result, i, j);
                logArgs("value:", port);
            }
            else {
                char* value = PQgetvalue(result, i, j);
                auto logMark = (value == nullptr)
                        ? "nullptr" : std::string(value);

                logArgs("ERROR: unknown field name:", logMark);
                break;
            }
        }

        this->sockets_.emplace_back(name, ipaddress, port);
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
