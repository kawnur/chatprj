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

        logArgs("address: ", dbAddress);
        logArgs("port: ", dbPort);
        logArgs("login: ", dbLogin);
        logArgs("password: ", dbPassword);

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

        if(status == ConnStatusType::CONNECTION_BAD) {  // TODO raise exception
            logLine("DB connection status: CONNECTION_BAD");
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

PGresult* getsSocketsInfo(PGconn* dbConnection) {
    const char* command = "SELECT name, ipaddress, port FROM sockets";
    PGresult* result = PQexec(dbConnection, command);
    return result;
}
