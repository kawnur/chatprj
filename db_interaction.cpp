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
    const char* command =
            "SELECT id, name FROM companions ";
//            "WHERE id > 1";  // me

    logArgs(command);

    PGresult* result = PQexec(dbConnection, command);
    return result;
}

PGresult* getSocketInfoDBResult(PGconn* dbConnection, int id) {
    QString command = QString(
            "SELECT ipaddress, port FROM sockets "
            "WHERE id = %1").arg(QString::fromStdString(std::to_string(id)));
    PGresult* result = PQexec(dbConnection, qPrintable(command));
    return result;
}

// TODO test sorting by timestamp with different timezones
PGresult* getMessagesDBResult(PGconn* dbConnection, int id) {
    QString command = QString(
            "SELECT author_id, timestamp_tz, message, issent "
            "FROM messages WHERE companion_id = %1 "
            "ORDER BY timestamp_tz LIMIT 50")
            .arg(QString::fromStdString(std::to_string(id)));

    logArgs(command);

    PGresult* result = PQexec(dbConnection, qPrintable(command));
    return result;
}

PGresult* pushMessageToDBAndReturn(
        PGconn* dbConnection, const QString& name, const QString& message) {
    QString command = QString(
            "insert into messages "
            "(companion_id, author_id, timestamp_tz, message, issent) "
            "values ((select id from companions where name = '%1'), 1, "
            "now(), '%2', false) returning companion_id, timestamp_tz")
            .arg(name, message);

    logArgs(command);

    PGresult* result = PQexec(dbConnection, qPrintable(command));

    return result;
}

void logUnknownField(const PGresult* result, int row, int column) {
    char* value = PQgetvalue(result, row, column);
    auto logMark = (value == nullptr)
            ? "nullptr" : std::string(value);

    logArgsError("unknown field name:", logMark);
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
        logArgsError(ntuples, "lines from OneToOne DB request for socket info");
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
        int companion_id, author_id;
//        std::tm time;
        QString time;
        QString message;
        bool isSent;

        for(int j = 0; j < nfields; j++) {

            char* fname = PQfname(result, j);
            std::string fnameStr = (fname == nullptr)
                    ? "nullptr" : std::string(fname);
            logArgs("fname:", fnameStr);

            if(fnameStr == "companion_id") {
                companion_id = std::atoi(PQgetvalue(result, i, j));
                logArgs("value:", companion_id);
            }
            else if(fnameStr == "author_id") {
                author_id = std::atoi(PQgetvalue(result, i, j));
                logArgs("value:", author_id);
            }
            else if(fnameStr == "timestamp_tz") {

                // TODO add modification to std::tm
                const char* timeStr = PQgetvalue(result, i, j);
                time = QString(timeStr);
                logArgs("value:", timeStr);
            }
            else if(fnameStr == "message") {
                message = PQgetvalue(result, i, j);
                logArgs("value:", message);
            }
            else if(fnameStr == "issent") {
                isSent = PQgetvalue(result, i, j);
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

        companion->addMessage(companion_id, author_id, time, message, isSent);
    }
}

std::pair<int, QString> getPushedMessageInfo(const PGresult* result) {
    int companion_id;
    QString time;

    int ntuples = PQntuples(result);
    logArgs("ntuples:", ntuples);

    if(ntuples > 1) {
        logArgsError(ntuples, "lines from OneToOne DB request for socket info");
    }

    int nfields = PQnfields(result);
    logArgs("nfields:", nfields);

    for(int i = 0; i < nfields; i++) {
        char* fname = PQfname(result, i);
        std::string fnameStr = (fname == nullptr)
                ? "nullptr" : std::string(fname);
        logArgs("fname:", fnameStr);

        if(fnameStr == "companion_id") {
            companion_id = std::atoi(PQgetvalue(result, 0, i));
            logArgs("value:", companion_id);
        }
        else if(fnameStr == "timestamp_tz") {

            // TODO add modification to std::tm
            const char* timeStr = PQgetvalue(result, 0, i);
            time = QString(timeStr);
            logArgs("value:", timeStr);
        }
    }

    return std::pair<int, QString>(companion_id, time);
}

void testQString() {
    QString s1 = QString(
            "insert into messages "
            "(companion_id, timestamp_tz, message, issent) "
            "values ((select id from companions where name = %1), "
            "now(), %2, false) returning id").arg("'client2'").arg("'test'");

    QString s2 = QString(
            "insert into messages "
            "(companion_id, timestamp_tz, message, issent) "
            "values ((select id from companions where name = %1), "
            "now(), %2, false) returning id").arg("'client2'", "'test'");

    coutArgsWithSpaceSeparator("s1:", s1);
    coutArgsWithSpaceSeparator("s2:", s2);
}
