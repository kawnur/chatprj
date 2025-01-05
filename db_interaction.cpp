#include "db_interaction.hpp"

DBReplyData::DBReplyData(int count, ...)
    : data_(std::vector<std::map<std::string, const char*>>(1))
{
    va_list args;
    va_start(args, count);

    for(int i = 0; i < count; i++)
    {
        auto key = std::string(va_arg(args, char*));

        // coutWithEndl(key);

        data_.at(0).insert({key, nullptr});
    }

    va_end(args);
}

DBReplyData::DBReplyData(const std::vector<std::string>& keys)
    : data_(std::vector<std::map<std::string, const char*>>(1))
{
    for(auto& key : keys)
    {
        data_.at(0).insert({key, nullptr});
    }
}

DBReplyData::~DBReplyData()
{
    // delete[] &(this->data_);
}

void DBReplyData::clear()
{
    this->data_.clear();
}

bool DBReplyData::isEmpty()
{
    return this->data_.empty();
}

void DBReplyData::fill(size_t count)
{
    size_t size = this->data_.size();

    for(int i = 0; i < count - size; i++)
    {
        this->data_.push_back(this->data_.at(0));
    }
}

size_t DBReplyData::count(size_t position, std::string key)
{
    return this->data_.at(position).count(key);
}

void DBReplyData::push(size_t position, std::string key, const char* value)
{
    this->data_.at(position).at(key) = value;
}

size_t DBReplyData::size()
{
    return this->data_.size();
}

const char* DBReplyData::getValue(size_t position, std::string key)
{
    return this->data_.at(position).at(key);
}

bool DBReplyData::findValue(const std::string& key, const std::string& value)
{
    auto findLambda = [&](
        // std::iterator<std::vector<std::map<std::string, const char*>>>& iterator)
        auto& iterator)
    {
        return std::string(iterator.at(key)) == value;
    };

    auto findMapResult = std::find_if(this->data_.begin(), this->data_.end(), findLambda);

    return (findMapResult == this->data_.end()) ? false : true;
}

void DBReplyData::logData()
{
    logArgs("############################");

    for(auto& elem : this->data_)
    {
        for(auto& item : elem)
        {
            logArgs(item.first, item.second);
        }
    }

    logArgs("############################");
}

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

PGresult* getCompanionByNameDBResult(const PGconn* dbConnection, const std::string& name)
{
    std::string command = std::string(
        "SELECT id FROM companions WHERE name = '")
        + name + std::string("'");

    return sendDBRequestAndReturnResult(dbConnection, command.data());
}

PGresult* getCompanionAndSocketDBResult(const PGconn* dbConnection, const int& id)
{
    std::string command = std::string(
        "SELECT companions.name, sockets.ipaddress, sockets.client_port "
        "FROM companions JOIN sockets ON companions.id = sockets.id "
        "WHERE companions.id = ")
        + std::to_string(id);

    return sendDBRequestAndReturnResult(dbConnection, command.data());
}

PGresult* getSocketInfoDBResult(const PGconn* dbConnection, const int& id)
{
    std::string command = std::string(
        "SELECT ipaddress, server_port, client_port "
        "FROM sockets WHERE id = ") + std::to_string(id);

    return sendDBRequestAndReturnResult(dbConnection, command.data());
}

PGresult* getSocketByIpAddressAndPortDBResult(
    const PGconn* dbConnection, const std::string& ipAddress, const std::string& port)
{
    std::string command = std::string(
        "SELECT id FROM sockets "
        "WHERE ipaddress = '")
        + ipAddress
        + std::string("' AND client_port = '")
        + port
        + std::string("'");

    return sendDBRequestAndReturnResult(dbConnection, command.data());
}

// TODO test sorting by timestamp with different timezones
PGresult* getMessagesDBResult(const PGconn* dbConnection, const int& id)
{
    std::string command = std::string(
        "SELECT id, author_id, timestamp_tz, message, issent "
        "FROM messages WHERE companion_id = ")
        + std::to_string(id)
        + std::string(" ORDER BY timestamp_tz LIMIT 50");

    return sendDBRequestAndReturnResult(dbConnection, command.data());
}

PGresult* getPasswordDBResult(const PGconn* dbConnection)
{
    const char* command = "SELECT password FROM passwords";

    return sendDBRequestAndReturnResult(dbConnection, command);
}

PGresult* setMessageInDbAndReturn(const PGconn* dbConnection, const uint32_t& messageId)
{
    std::string command = std::string(
        "UPDATE messages set issent = 'true' "
        "WHERE id = ")
        + std::to_string(messageId)
        + std::string(" RETURNING id");

    return sendDBRequestAndReturnResult(dbConnection, command.data());
}

PGresult* pushCompanionToDBAndReturn(
    const PGconn* dbConnection, const std::string& companionName)
{
    std::string command = std::string(
        "INSERT INTO companions "
        "(name) "
        "VALUES ('")
        + companionName
        + std::string("') RETURNING id");

    return sendDBRequestAndReturnResult(dbConnection, command.data());
}

PGresult* updateCompanionAndReturn(  // TODO change function names
    const PGconn* dbConnection, const std::string& companionName)
{
    std::string command = std::string(
        "INSERT INTO companions "
        "(name) "
        "VALUES ('")
        + companionName
        + std::string("') RETURNING id");

    return sendDBRequestAndReturnResult(dbConnection, command.data());
}

PGresult* updateCompanionAndSocketAndReturn(
    const PGconn* dbConnection, const CompanionAction& companionAction)
{
    std::string command = std::string(
        "WITH update_name AS (UPDATE companions set name = '")
        + companionAction.getName()
        + std::string("' WHERE id = ")
        + std::to_string(companionAction.getCompanionId())
        + std::string(" RETURNING id) UPDATE sockets "
        "set ipaddress = '")
        + companionAction.getIpAddress()
        + std::string("', client_port = '")
        + companionAction.getClientPort()
        + std::string("' WHERE id IN (SELECT id FROM update_name) RETURNING id");

    return sendDBRequestAndReturnResult(dbConnection, command.data());
}

PGresult* pushSocketToDBAndReturn(
    const PGconn* dbConnection, const std::string& companionName,
    const std::string& ipAddress, const std::string& serverPort,
    const std::string& clientPort)
{
    std::string command = std::string(
        "INSERT INTO sockets "
        "(id, ipaddress, server_port, client_port) "
        "VALUES ((SELECT id FROM companions WHERE name = '")
        + companionName
        + std::string("'), '")
        + ipAddress
        + std::string("', ")
        + serverPort
        + std::string(", ")
        + clientPort
        + std::string(") RETURNING id");

    return sendDBRequestAndReturnResult(dbConnection, command.data());
}

PGresult* pushMessageToDBAndReturn(
    const PGconn* dbConnection, const std::string& companionName,
    const std::string& authorName, const std::string& timestamp,
    const std::string& returningFieldName, const std::string& message)
{
    std::string command = std::string(
        "INSERT INTO messages "
        "(companion_id, author_id, timestamp_tz, message, issent) "
        "VALUES ((SELECT id FROM companions WHERE name = '")
        + companionName
        + std::string("'), (SELECT id FROM companions WHERE name = '")
        + authorName
        + std::string("'), '")
        + timestamp
        + std::string("', '")
        + message
        + std::string("', false) RETURNING id, ")
        + returningFieldName
        + std::string(", timestamp_tz");

    return sendDBRequestAndReturnResult(dbConnection, command.data());
}

PGresult* pushPasswordToDBAndReturn(const PGconn* dbConnection, const std::string& password)
{
    std::string command = std::string(
        "INSERT INTO passwords (password) "
        "VALUES ('")
        + password
        + std::string("') RETURNING id");

    return sendDBRequestAndReturnResult(dbConnection, command.data());
}

PGresult* deleteMessagesFromDBAndReturn(
    const PGconn* dbConnection, const CompanionAction& companionAction)
{
    std::string command = std::string(
        "DELETE FROM messages WHERE companion_id = ")
        + std::to_string(companionAction.getCompanionId())
        + std::string(" RETURNING companion_id");

    return sendDBRequestAndReturnResult(dbConnection, command.data());
}

PGresult* deleteCompanionAndSocketAndReturn(
    const PGconn* dbConnection, const CompanionAction& companionAction)
{
    std::string command = std::string(
        "WITH delete_socket AS (DELETE FROM sockets WHERE id = ")
        + std::to_string(companionAction.getCompanionId())
        + std::string(" RETURNING id) DELETE FROM companions WHERE id IN "
        "(SELECT id FROM delete_socket) RETURNING id");

    return sendDBRequestAndReturnResult(dbConnection, command.data());
}

void logUnknownField(const PGresult* result, int row, int column)
{
    char* value = PQgetvalue(result, row, column);
    auto logMark = (value) ? std::string(value) : "nullptr";

    logArgsError("unknown field name:", logMark);
}

int getDataFromDBResult(
    std::shared_ptr<DBReplyData>& dataPtr,
    const PGresult* result,
    int maxTuples)
{
    int dataIsOk = 0;

    int ntuples = PQntuples(result);
    int nfields = PQnfields(result);
    logArgs("ntuples:", ntuples, "nfields:", nfields);

    if(ntuples == 0)
    {
        dataPtr->clear();
        return dataIsOk;
    }

    if(maxTuples == 1 and ntuples > 1)
    {
        logArgsError(ntuples, "lines from OneToOne DB request");
    }

    // create additional elements in result vector
    dataPtr->fill(ntuples);

    dataIsOk = 1;

    for(int i = 0; i < ntuples; i++)
    {
        std::string logString;

        for(int j = 0; j < nfields; j++)
        {
            char* fname = PQfname(result, j);
            std::string fnameString = (fname) ? std::string(fname) : "nullptr";

            auto found = dataPtr->count(i, fnameString);

            if(found == 1)
            {
                const char* value = PQgetvalue(result, i, j);
                dataPtr->push(i, fnameString, value);

                logString += (fnameString + ": " + std::string(value) + " ");
            }
            else
            {
                dataIsOk = -1;  // TODO return ?
                logUnknownField(result, i, j);
            }
        }

        // logArgs(logString);
    }

    return dataIsOk;
}
