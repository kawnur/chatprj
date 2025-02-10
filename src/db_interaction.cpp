#include "db_interaction.hpp"

std::mutex dbMutex;

DBReplyData::DBReplyData(int count, ...)
    : data_(std::vector<std::map<std::string, const char*>>(1))
{
    va_list args;
    va_start(args, count);

    for(int i = 0; i < count; i++)
    {
        auto key = std::string(va_arg(args, char*));

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

DBReplyData::~DBReplyData() {}

std::vector<std::map<std::string, const char*>>* DBReplyData::getDataPtr()
{
    return &this->data_;
}

std::vector<QString> DBReplyData::buildDataQStringVector()
{
    std::vector<QString> result {};

    for(auto& element : this->data_)
    {
        QString representation { "" };

        for(auto& pair : element)
        {
            representation += getArgumentedQString("%1: %2 ", pair.first, pair.second);
        }

        result.push_back(representation);
    }

    return result;
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
    auto findLambda = [&](auto& iterator)
    {
        return std::string(iterator.at(key)) == value;
    };

    auto findMapResult = std::find_if(
        this->data_.begin(), this->data_.end(), findLambda);

    return !(findMapResult == this->data_.end());
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

        logArgs("DB connection; address:", dbAddress, "port:", dbPort,
                "login:", dbLogin, "password:", dbPassword);

        dbConnection = PQsetdbLogin(
                    dbAddress,
                    dbPort,
                    "",
                    "",
                    "postgres",
                    dbLogin,
                    dbPassword);

        ConnStatusType status = PQstatus(dbConnection);
        std::string mark = (status == 0) ? "OK" : "";

        logArgs("DB connection status: ", std::to_string(status), mark);

        if(status == ConnStatusType::CONNECTION_BAD)  // TODO raise exception
        {
            logArgsError("DB connection status: CONNECTION_BAD");
        }
    };

    runAndLogException(connectLambda);

    return dbConnection;
}

PGresult* sendDBRequestAndReturnResult(
    const PGconn* dbConnection, const bool& logging, const char* command)
{
    if(logging)
    {
        logArgs(command);
    }

    std::lock_guard<std::mutex> lock(dbMutex);

    PGresult* result = PQexec(const_cast<PGconn*>(dbConnection), command);

    return result;
}

PGresult* getCompanionsDBResult(const PGconn* dbConnection, const bool& logging)
{
    const char* command = "SELECT id, name FROM companions";

    return sendDBRequestAndReturnResult(dbConnection, logging, command);
}

PGresult* getCompanionByNameDBResult(
    const PGconn* dbConnection, const bool& logging, const std::string& name)
{
    QString command = getArgumentedQString(
        "SELECT id FROM companions WHERE name = '%1'", name);

    return sendDBRequestAndReturnResult(dbConnection, logging, command.toStdString().data());
}

PGresult* getCompanionAndSocketDBResult(
    const PGconn* dbConnection, const bool& logging, const int& id)
{
    QString command = getArgumentedQString(
        "SELECT companions.name, sockets.ipaddress, sockets.client_port "
        "FROM companions JOIN sockets ON companions.id = sockets.id "
        "WHERE companions.id = %1", id);

    return sendDBRequestAndReturnResult(dbConnection, logging, command.toStdString().data());
}

PGresult* getSocketInfoDBResult(
    const PGconn* dbConnection, const bool& logging, const int& id)
{
    QString command = getArgumentedQString(
        "SELECT ipaddress, server_port, client_port FROM sockets WHERE id = %1", id);

    return sendDBRequestAndReturnResult(dbConnection, logging, command.toStdString().data());
}

PGresult* getSocketByIpAddressAndPortDBResult(
    const PGconn* dbConnection, const bool& logging,
    const std::string& ipAddress, const std::string& port)
{
    QString command = getArgumentedQString(
        "SELECT id FROM sockets WHERE ipaddress = '%1' AND client_port = '%2'",
        ipAddress, port);

    return sendDBRequestAndReturnResult(dbConnection, logging, command.toStdString().data());
}

PGresult* getMessagesDBResult(
    const PGconn* dbConnection, const bool& logging, const uint8_t& companionId)
{
    QString command = getArgumentedQString(
        "WITH select_id AS "
        "(SELECT id FROM messages WHERE companion_id = %1 "
        "ORDER BY timestamp_tz DESC LIMIT %2) "
        "SELECT id, companion_id, author_id, timestamp_tz, message, is_sent, is_received "
        "FROM messages WHERE id IN (SELECT id FROM select_id) "
        "ORDER BY timestamp_tz ASC", companionId, numberOfMessagesToGetFromDB);

    return sendDBRequestAndReturnResult(dbConnection, logging, command.toStdString().data());
}

PGresult* getAllMessagesByCompanionIdDBResult(
    const PGconn* dbConnection, const bool& logging, const int& companionId)
{
    QString command = getArgumentedQString(
        "SELECT author_id, timestamp_tz, message "
        "FROM messages WHERE companion_id = %1 "
        "ORDER BY timestamp_tz ASC", companionId);

    return sendDBRequestAndReturnResult(dbConnection, logging, command.toStdString().data());
}

PGresult* getEarlyMessagesByMessageIdDBResult(
    const PGconn* dbConnection, const bool& logging,
    const int& companionId,const uint32_t& messageId)
{
    QString command = getArgumentedQString(
        "WITH select_id AS "
        "(SELECT id FROM messages WHERE companion_id = %1 AND id < %2 "
        "ORDER BY timestamp_tz DESC LIMIT %3) "
        "SELECT id, companion_id, author_id, timestamp_tz, message, is_sent, is_received "
        "FROM messages WHERE id IN (SELECT id FROM select_id) "
        "ORDER BY timestamp_tz ASC", companionId, messageId, numberOfMessagesToGetFromDB);

    return sendDBRequestAndReturnResult(dbConnection, logging, command.toStdString().data());
}

PGresult* getMessageByCompanionIdAndTimestampDBResult(
    const PGconn* dbConnection, const bool& logging,
    const uint8_t& companionId, const std::string& timestamp)
{
    QString command = getArgumentedQString(
        "SELECT id FROM messages WHERE companion_id = %1 AND timestamp_tz = '%2'",
        companionId, timestamp);

    return sendDBRequestAndReturnResult(dbConnection, logging, command.toStdString().data());
}

PGresult* getUnsentMessagesByCompanionNameDBResult(
    const PGconn* dbConnection, const bool& logging, const std::string& companionName)
{
    QString command = getArgumentedQString(
        "SELECT id, author_id, companion_id, timestamp_tz, message, is_received "
        "FROM messages WHERE companion_id = (SELECT id FROM companions WHERE name = '%1') "
        "AND author_id = (SELECT id FROM companions WHERE name = 'me') "
        "AND is_sent IS false", companionName);

    return sendDBRequestAndReturnResult(dbConnection, logging, command.toStdString().data());
}

PGresult* getPasswordDBResult(const PGconn* dbConnection, const bool& logging)
{
    const char* command = "SELECT password FROM passwords";

    return sendDBRequestAndReturnResult(dbConnection, logging, command);
}

PGresult* setMessageIsSentInDbAndReturn(
    const PGconn* dbConnection, const bool& logging, const uint32_t& messageId)
{
    QString command = getArgumentedQString(
        "UPDATE messages SET is_sent = 'true' WHERE id = %1 RETURNING id", messageId);

    return sendDBRequestAndReturnResult(dbConnection, logging, command.toStdString().data());
}

PGresult* setMessageIsReceivedInDbAndReturn(
    const PGconn* dbConnection, const bool& logging, const uint32_t& messageId)
{
    QString command = getArgumentedQString(
        "UPDATE messages SET is_received = 'true' WHERE id = %1 RETURNING id", messageId);

    return sendDBRequestAndReturnResult(dbConnection, logging, command.toStdString().data());
}

PGresult* pushCompanionToDBAndReturn(
    const PGconn* dbConnection, const bool& logging, const std::string& companionName)
{
    QString command = getArgumentedQString(
        "INSERT INTO companions (name) VALUES ('%1') RETURNING id", companionName);

    return sendDBRequestAndReturnResult(dbConnection, logging, command.toStdString().data());
}

PGresult* updateCompanionAndReturn(  // TODO change function names
    const PGconn* dbConnection, const bool& logging, const std::string& companionName)
{
    QString command = getArgumentedQString(
        "INSERT INTO companions (name) VALUES ('%1') RETURNING id", companionName);

    return sendDBRequestAndReturnResult(dbConnection, logging, command.toStdString().data());
}

PGresult* updateCompanionAndSocketAndReturn(
    const PGconn* dbConnection, const bool& logging, const CompanionAction& companionAction)
{
    QString command = getArgumentedQString(
        "WITH update_name AS (UPDATE companions SET name = '%1' WHERE id = %2 "
        "RETURNING id) UPDATE sockets SET ipaddress = '%3', client_port = '%4' "
        "WHERE id IN (SELECT id FROM update_name) RETURNING id",
        companionAction.getName(), companionAction.getCompanionId(),
        companionAction.getIpAddress(), companionAction.getClientPort());

    return sendDBRequestAndReturnResult(dbConnection, logging, command.toStdString().data());
}

PGresult* pushSocketToDBAndReturn(
    const PGconn* dbConnection, const bool& logging, const std::string& companionName,
    const std::string& ipAddress, const std::string& serverPort,
    const std::string& clientPort)
{
    QString command = getArgumentedQString(
        "INSERT INTO sockets (id, ipaddress, server_port, client_port) "
        "VALUES ((SELECT id FROM companions WHERE name = '%1'), '%2', %3, %4) RETURNING id",
        companionName, ipAddress, serverPort, clientPort);

    return sendDBRequestAndReturnResult(dbConnection, logging, command.toStdString().data());
}

PGresult* pushMessageToDBAndReturn(
    const PGconn* dbConnection, const bool& logging, const std::string& companionName,
    const std::string& authorName, const std::string& timestamp,
    const std::string& returningFieldName, const std::string& message,
    const bool& isSent, const bool& isReceived)
{
    QString command = getArgumentedQString(
        "INSERT INTO messages "
        "(companion_id, author_id, timestamp_tz, message, is_sent, is_received) "
        "VALUES ((SELECT id FROM companions WHERE name = '%1'), "
        "(SELECT id FROM companions WHERE name = '%2'), '%3', '%4', %5, %6) "
        "RETURNING id, %7, timestamp_tz",
        companionName, authorName, timestamp, message, isSent, isReceived,
        returningFieldName);

    return sendDBRequestAndReturnResult(dbConnection, logging, command.toStdString().data());
}

PGresult* pushMessageToDBWithAuthorIdAndReturn(
    const PGconn* dbConnection, const bool& logging, const std::string& companionName,
    const std::string& authorIdString, const std::string& timestamp,
    const std::string& returningFieldName, const std::string& message,
    const bool& isSent, const bool& isReceived)
{
    QString command = getArgumentedQString(
        "INSERT INTO messages "
        "(companion_id, author_id, timestamp_tz, message, is_sent, is_received) "
        "VALUES ((SELECT id FROM companions WHERE name = '%1'), %2, '%3', '%4', %5, %6) "
        "RETURNING id, %7, timestamp_tz", companionName, authorIdString, timestamp,
        message, isSent, isReceived, returningFieldName);

    return sendDBRequestAndReturnResult(dbConnection, logging, command.toStdString().data());
}

PGresult* pushPasswordToDBAndReturn(
    const PGconn* dbConnection, const bool& logging, const std::string& password)
{
    QString command = getArgumentedQString(
        "INSERT INTO passwords (password) VALUES ('%1') RETURNING id", password);

    return sendDBRequestAndReturnResult(dbConnection, logging, command.toStdString().data());
}

PGresult* deleteMessagesFromDBAndReturn(
    const PGconn* dbConnection, const bool& logging, const CompanionAction& companionAction)
{
    QString command = getArgumentedQString(
        "DELETE FROM messages WHERE companion_id = %1 RETURNING companion_id",
        companionAction.getCompanionId());

    return sendDBRequestAndReturnResult(dbConnection, logging, command.toStdString().data());
}

PGresult* deleteCompanionAndSocketAndReturn(
    const PGconn* dbConnection, const bool& logging, const CompanionAction& companionAction)
{
    QString command = getArgumentedQString(
        "WITH delete_socket AS (DELETE FROM sockets WHERE id = %1 RETURNING id) "
        "DELETE FROM companions WHERE id IN (SELECT id FROM delete_socket) RETURNING id",
        companionAction.getCompanionId());

    return sendDBRequestAndReturnResult(dbConnection, logging, command.toStdString().data());
}

int getDataFromDBResult(
    const bool& logging,
    std::shared_ptr<DBReplyData>& dataPtr,
    const PGresult*& result,
    int maxTuples)
{
    int dataIsOk = 0;

    int ntuples = PQntuples(result);
    int nfields = PQnfields(result);

    if(logging)
    {
        logArgs("ntuples:", ntuples, "nfields:", nfields);
    }

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
                logDBResultUnknownField(result, i, j);
            }
        }

        // logArgs(logString);
    }

    return dataIsOk;
}
