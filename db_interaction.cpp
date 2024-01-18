#include "db_interaction.hpp"

PGconn* getDBConnection() {
    // TODO make connection to db secure
    MainWindow* mainWindow = getMainWindowPtr();

    PGconn* dbConnection = nullptr;

    try {
        QString dbAddress { std::getenv("CHATAPP_DB_ADDRESS") };
        QString dbPort { std::getenv("CHATAPP_DB_PORT") };
        QString dbLogin { std::getenv("CHATAPP_DB_USER") };
        QString dbPassword { std::getenv("CHATAPP_DB_PASSWORD") };

        logArgs("address:", dbAddress);
        logArgs("port:", dbPort);
        logArgs("login:", dbLogin);
        logArgs("password:", dbPassword);

        dbConnection = PQsetdbLogin(
                    std::getenv("CHATAPP_DB_ADDRESS"),
                    std::getenv("CHATAPP_DB_PORT"),
                    "",
                    "",
                    "postgres",
                    std::getenv("CHATAPP_DB_USER"),
                    std::getenv("CHATAPP_DB_PASSWORD"));

        ConnStatusType status = PQstatus(dbConnection);
        logArgs("status: ", std::to_string(status));
//        logArgs("status: ", status);

        if(status == ConnStatusType::CONNECTION_BAD) {  // TODO raise exception
            logArgs("DB connection status: CONNECTION_BAD");
        }

//        return dbConnection;
    }
    catch(std::exception& e) {
//        coutWithEndl(e.what());
//        mainWindow->addTextToCentralPanel(QString(e.what()));
        logLine(QString(e.what()));
    }

    return dbConnection;
}

PGresult* getCompanionsDBResult(PGconn* dbConnection) {
    const char* command = "SELECT id, name FROM companions";
    PGresult* result = PQexec(dbConnection, command);
    return result;
}

PGresult* getSocketInfoDBResult(PGconn* dbConnection, int id) {
    QString command =
            "SELECT ipaddress, port FROM sockets "
            "WHERE id = " + QString::fromStdString(std::to_string(id));
    PGresult* result = PQexec(dbConnection, qPrintable(command));
    return result;
}

PGresult* getMessagesDBResult(PGconn* dbConnection, int id) {
    QString commandQString =
            "SELECT time, message, issent FROM sockets "
            "ORDER BY time LIMIT 50 WHERE companion_id = ";
    QString idQString = QString::fromStdString(std::to_string(id));
    const char* command = qPrintable(commandQString + idQString);

    logArgs(command);

    PGresult* result = PQexec(dbConnection, command);
    return result;
}

void logUnknownField(const PGresult* result, int row, int column) {
    char* value = PQgetvalue(result, row, column);
    auto logMark = (value == nullptr)
            ? "nullptr" : std::string(value);

    logArgs("ERROR: unknown field name:", logMark);
}

void getCompanionsDataFromDBResult(
        std::vector<std::pair<int, QString>>* data,
        const PGresult* result) {

    int ntuples = PQntuples(result);
    logArgs("ntuples:", ntuples);

    int nfields = PQnfields(result);
    logArgs("nfields:", nfields);

    for(int i = 0; i < ntuples; i++) {
        int id;
        QString name;

        for(int j = 0; j < nfields; j++) {
            char* fname = PQfname(result, j);
            std::string fnameStr = (fname == nullptr)
                    ? "nullptr" : std::string(fname);
            logArgs("fname:", fnameStr);

            if(fnameStr == "id") {
                id = std::atoi(PQgetvalue(result, i, j));
                logArgs("value:", id);
            }
            else if(fnameStr == "name") {
                name = PQgetvalue(result, i, j);
                logArgs("value:", name);
            }
            else {
//                char* value = PQgetvalue(result, i, j);
//                auto logMark = (value == nullptr)
//                        ? "nullptr" : std::string(value);

//                logArgs("ERROR: unknown field name:", logMark);
                logUnknownField(result, i, j);
                break;
            }
        }

        data->emplace_back(id, name);
    }
}

std::pair<QString, QString> getSocketInfoDataFromDBResult(const PGresult* result) {
    QString ipaddress, port;

    int ntuples = PQntuples(result);
    logArgs("ntuples:", ntuples);

    if(ntuples > 1) {
        logArgs("ERROR:", ntuples, "lines from OneToOne DB request for socket info");
    }

    int nfields = PQnfields(result);
    logArgs("nfields:", nfields);

    for(int i = 0; i < nfields; i++) {
        char* fname = PQfname(result, i);
        std::string fnameStr = (fname == nullptr)
                ? "nullptr" : std::string(fname);
        logArgs("fname:", fnameStr);

        if(fnameStr == "ipaddress") {
            ipaddress = PQgetvalue(result, 0, i);
            logArgs("value:", ipaddress);
        }
        else if(fnameStr == "port") {
            port = PQgetvalue(result, 0, i);
            logArgs("value:", port);
        }
        else {
//                char* value = PQgetvalue(result, i, j);
//                auto logMark = (value == nullptr)
//                        ? "nullptr" : std::string(value);

//                logArgs("ERROR: unknown field name:", logMark);
            logUnknownField(result, 0, i);
            break;
        }
    }

    return std::pair<QString, QString>(ipaddress, port);
}

void getMessagesDataFromDBResultAndAdd(Companion* companion, const PGresult* result) {
    int ntuples = PQntuples(result);
    logArgs("ntuples:", ntuples);

    int nfields = PQnfields(result);
    logArgs("nfields:", nfields);

    for(int i = 0; i < ntuples; i++) {
        int companion_id;
        std::tm time;
        QString text;
        bool isSent;

        for(int j = 0; j < nfields; j++) {
            char* fname = PQfname(result, i);
            std::string fnameStr = (fname == nullptr)
                    ? "nullptr" : std::string(fname);
            logArgs("fname:", fnameStr);

            if(fnameStr == "companion_id") {
                companion_id = std::atoi(PQgetvalue(result, i, j));
                logArgs("value:", companion_id);
            }
            else if(fnameStr == "time") {

                // TODO add modification to std::tm
                const char* timeStr = PQgetvalue(result, 0, i);
                logArgs("value:", timeStr);
            }
            else if(fnameStr == "text") {
                text = PQgetvalue(result, 0, i);
                logArgs("value:", text);
            }
            else if(fnameStr == "issent") {
                isSent = PQgetvalue(result, 0, i);
                logArgs("value:", isSent);
            }
            else {
    //                char* value = PQgetvalue(result, i, j);
    //                auto logMark = (value == nullptr)
    //                        ? "nullptr" : std::string(value);

    //                logArgs("ERROR: unknown field name:", logMark);
                logUnknownField(result, 0, i);
                break;
            }
        }

        companion->addMessage(companion_id, time, text, isSent);
    }
}
