#include "db_interaction.hpp"

PGconn* getDBConnection()
{
    // TODO make connection to db secure
    MainWindow* mainWindow = getMainWindowPtr();

    PGconn* dbConnection = nullptr;

    try
    {
        const char* dbAddress { std::getenv("CHATAPP_DB_ADDRESS") };
        const char* dbPort { std::getenv("CHATAPP_DB_PORT") };
        const char* dbLogin { std::getenv("CHATAPP_DB_USER") };
        const char* dbPassword { std::getenv("CHATAPP_DB_PASSWORD") };
        // const char* dbAddress = "172.21.0.12";
        // const char* dbPort = "5432";
        // const char* dbLogin = "postgres";
        // const char* dbPassword = "postgres123";

        logArgs("address:", QString(dbAddress));
        logArgs("port:", QString(dbPort));
        logArgs("login:", QString(dbLogin));
        logArgs("password:", QString(dbPassword));

        dbConnection = PQsetdbLogin(
                    dbAddress,
                    dbPort,
                    "",
                    "",
                    "postgres",
                    dbLogin,
                    dbPassword);

        ConnStatusType status = PQstatus(dbConnection);
        logArgs("status: ", std::to_string(status));
//        logArgs("status: ", status);

        if(status == ConnStatusType::CONNECTION_BAD)  // TODO raise exception
        {
            logArgs("DB connection status: CONNECTION_BAD");
        }

//        return dbConnection;
    }
    catch(std::exception& e)
    {
//        coutWithEndl(e.what());
//        mainWindow->addTextToCentralPanel(QString(e.what()));
        logLine(QString(e.what()));
    }

    return dbConnection;
}

PGresult* getCompanionsDBResult(PGconn* dbConnection)
{
    const char* command =
            "SELECT id, name FROM companions ";
//            "WHERE id > 1";  // me

    logArgs(command);

    PGresult* result = PQexec(dbConnection, command);
    return result;
}

PGresult* getSocketInfoDBResult(PGconn* dbConnection, int id)
{
    QString command = QString(
            "SELECT ipaddress, port FROM sockets "
            "WHERE id = %1").arg(QString::fromStdString(std::to_string(id)));

    PGresult* result = PQexec(dbConnection, qPrintable(command));
    return result;
}

// TODO test sorting by timestamp with different timezones
PGresult* getMessagesDBResult(PGconn* dbConnection, int id)
{
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
        PGconn* dbConnection, const QString& name, const QString& message)
{
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

void logUnknownField(const PGresult* result, int row, int column)
{
    char* value = PQgetvalue(result, row, column);
    auto logMark = (value == nullptr) ? "nullptr" : std::string(value);

    logArgsError("unknown field name:", logMark);
}

bool getDataFromDBResult(
    std::vector<std::map<std::string, const char*>>& data,
    const PGresult* result,
    int maxTuples)
{
    bool foundUnknownFields = false;

    int ntuples = PQntuples(result);
    int nfields = PQnfields(result);
    logArgs("ntuples:", ntuples, "nfields:", nfields);

    if(ntuples == 0)
    {
        data.pop_back();
        return foundUnknownFields;
    }

    if(maxTuples == 1 and ntuples > 1)
    {
        logArgsError(ntuples, "lines from OneToOne DB request");
    }

    // create additional elements in result vector
    for(int i = 0; i < ntuples - 1; i++)
    {
        data.push_back(data.at(0));
    }

    for(int i = 0; i < ntuples; i++)
    {
        std::string logString;

        for(int j = 0; j < nfields; j++)
        {
            char* fname = PQfname(result, j);
            std::string fnameString = (fname == nullptr) ? "nullptr" : std::string(fname);

            auto found = data.at(i).count(fnameString);

            if(found == 1)
            {
                const char* value = PQgetvalue(result, i, j);
                data.at(i).at(fnameString) = value;

                logString += (fnameString + ": " + std::string(value) + " ");
            }
            else
            {
                foundUnknownFields = true;
                logUnknownField(result, i, j);
            }
        }

        logArgs(logString);
    }

    return foundUnknownFields;
}

void testQString()
{
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
