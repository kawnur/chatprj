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

//        coutArgsWithSpaceSeparator("address:", dbAddress.toStdString());
//        coutArgsWithSpaceSeparator("port:", dbPort.toStdString());

//        QString textAddress = QString("address: ") + dbAddress;
//        mainWindow->addText(textAddress);
        mainWindow->addText(QString("address: ") + dbAddress);

//        QString textPort = QString("port: ") + dbPort;
//        mainWindow->addText(textPort);
        mainWindow->addText(QString("port: ") + dbPort);
        mainWindow->addText(QString("login: ") + dbLogin);
        mainWindow->addText(QString("password: ") + dbPassword);

        dbConnection = PQsetdbLogin(
                    std::getenv("CHATAPP_DB_ADDRESS"),
                    std::getenv("CHATAPP_DB_PORT"),
                    "",
                    "",
                    "postgres",
                    std::getenv("CHATAPP_DB_USER"),
                    std::getenv("CHATAPP_DB_PASSWORD"));

//        return dbConnection;
    }
    catch(std::exception& e) {
//        coutWithEndl(e.what());
        mainWindow->addText(QString(e.what()));
    }

    return dbConnection;
}

PGresult* getsSocketsInfo(PGconn* dbConnection) {
    const char* command = "SELECT name, ipaddress, port FROM sockets";
    PGresult* result = PQexec(dbConnection, command);
    return result;
}
