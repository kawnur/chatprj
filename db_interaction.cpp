#include "db_interaction.hpp"

PGconn* getDBConnection()
{
    // TODO make connection to db secure
    // MainWindow* mainWindow = getMainWindowPtr();

    PGconn* dbConnection = nullptr;

    try
    {
        const char* dbAddress { std::getenv("CHATAPP_DB_ADDRESS") };
        const char* dbPort { std::getenv("CHATAPP_DB_PORT") };
        const char* dbLogin { std::getenv("CHATAPP_DB_USER") };
        const char* dbPassword { std::getenv("CHATAPP_DB_PASSWORD") };

        logArgs("DB connection address:", dbAddress);
        logArgs("DB connection port:", dbPort);
        logArgs("DB connection login:", dbLogin);
        logArgs("DB connection password:", dbPassword);

        dbConnection = PQsetdbLogin(
                    dbAddress,
                    dbPort,
                    "",
                    "",
                    "postgres",
                    dbLogin,
                    dbPassword);

        ConnStatusType status = PQstatus(dbConnection);
        logArgs("DB connection status: ", std::to_string(status));

        if(status == ConnStatusType::CONNECTION_BAD)  // TODO raise exception
        {
            logArgsError("DB connection status: CONNECTION_BAD");
        }
    }
    catch(std::exception& e)
    {
        logLine(e.what());
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
    std::string command = std::string(
            "SELECT ipaddress, port FROM sockets "
            "WHERE id = ") + std::to_string(id);

    logArgs(command);

    PGresult* result = PQexec(dbConnection, command.data());
    return result;
}

// TODO test sorting by timestamp with different timezones
PGresult* getMessagesDBResult(PGconn* dbConnection, int id)
{
    std::string command = std::string(
            "SELECT author_id, timestamp_tz, message, issent "
            "FROM messages WHERE companion_id = ")
            + std::to_string(id)
            + std::string(" ORDER BY timestamp_tz LIMIT 50");

    logArgs(command);

    PGresult* result = PQexec(dbConnection, command.data());
    return result;
}

PGresult* pushMessageToDBAndReturn(
    PGconn* dbConnection, const std::string& name, const std::string& message)
{
    std::string command = std::string(
            "insert into messages "
            "(companion_id, author_id, timestamp_tz, message, issent) "
            "values ((select id from companions where name = '")
            + name
            + std::string("'), 1, now(), '")
            + message
            + std::string("', false) returning companion_id, timestamp_tz");

    logArgs(command);

    PGresult* result = PQexec(dbConnection, command.data());
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
    bool dataIsOk = false;

    int ntuples = PQntuples(result);
    int nfields = PQnfields(result);
    logArgs("ntuples:", ntuples, "nfields:", nfields);

    if(ntuples == 0)
    {
        data.pop_back();
        return dataIsOk;
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

    dataIsOk = true;

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
                dataIsOk = false;
                logUnknownField(result, i, j);
            }
        }

        logArgs(logString);
    }

    return dataIsOk;
}
