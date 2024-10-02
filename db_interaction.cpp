#include "db_interaction.hpp"

const char* getValueFromEnvironmentVariable(const char* variableName)
{
    const char* valuePtr { std::getenv(variableName) };

    if(!valuePtr)
    {
        logArgsError("Did not find environment variable", variableName);
    }

    return valuePtr;
}

PGconn* getDBConnection()
{
    // TODO make connection to db secure

    PGconn* dbConnection = nullptr;

    auto connectLambda = [&]()
    {
        const char* dbAddress = getValueFromEnvironmentVariable("CHATAPP_DB_ADDRESS");
        const char* dbPort = getValueFromEnvironmentVariable("CHATAPP_DB_PORT");
        const char* dbLogin = getValueFromEnvironmentVariable("CHATAPP_DB_USER");
        const char* dbPassword = getValueFromEnvironmentVariable("CHATAPP_DB_PASSWORD");

        for(auto& value : { dbAddress, dbPort, dbLogin, dbPassword })
        {
            if(!value)
            {
                return;
            }
        }

        logArgs("DB connection; address:", dbAddress,
                "port:", dbPort,
                "login:", dbLogin,
                "password:", dbPassword);

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
    };

    runAndLogException(connectLambda);

    return dbConnection;
}

PGresult* sendDBRequestAndReturnResult(
    const PGconn* dbConnection, const char* command)
{
    logArgs(command);

    PGresult* result = PQexec(const_cast<PGconn*>(dbConnection), command);
    return result;
}

PGresult* getCompanionsDBResult(const PGconn* dbConnection)
{
    const char* command = "SELECT id, name FROM companions";

    return sendDBRequestAndReturnResult(dbConnection, command);
}

PGresult* getSocketInfoDBResult(const PGconn* dbConnection, int id)
{
    std::string command = std::string(
        "SELECT ipaddress, server_port, client_port "
        "FROM sockets WHERE id = ") + std::to_string(id);

    return sendDBRequestAndReturnResult(dbConnection, command.data());
}

// TODO test sorting by timestamp with different timezones
PGresult* getMessagesDBResult(const PGconn* dbConnection, int id)
{
    std::string command = std::string(
        "SELECT author_id, timestamp_tz, message, issent "
        "FROM messages WHERE companion_id = ")
        + std::to_string(id)
        + std::string(" ORDER BY timestamp_tz LIMIT 50");

    return sendDBRequestAndReturnResult(dbConnection, command.data());
}

PGresult* pushMessageToDBAndReturn(
    const PGconn* dbConnection, const std::string& name, const std::string& message)
{
    std::string command = std::string(
        "insert into messages "
        "(companion_id, author_id, timestamp_tz, message, issent) "
        "values ((select id from companions where name = '")
        + name
        + std::string("'), 1, now(), '")
        + message
        + std::string("', false) returning companion_id, timestamp_tz");

    return sendDBRequestAndReturnResult(dbConnection, command.data());
}

void logUnknownField(const PGresult* result, int row, int column)
{
    char* value = PQgetvalue(result, row, column);
    auto logMark = (value) ? std::string(value) : "nullptr";

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
            std::string fnameString = (fname) ? std::string(fname) : "nullptr";

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
