#include "db_interaction.hpp"

#include <mutex>

#include "action.hpp"
#include "logging.hpp"
#include "utils.hpp"

std::mutex dbMutex;

DBReplyData::DBReplyData(int count, ...) : data_(1)
{
    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; i++) {
        auto key = std::string(va_arg(args, char *));

        data_.at(0).insert({key, ""});
    }

    va_end(args);
}

DBReplyData::DBReplyData(const std::vector<std::string> &keys) : data_(1)
{
    for (auto &key : keys)
        data_.at(0).insert({key, ""});
}

std::vector<std::string> DBReplyData::buildDataStringVector()
{
    std::vector<std::string> result {};

    for (auto &element : data_) {
        std::string representation { "" };

        for (auto &pair : element)
            representation += getStringByFormat("{0}: {1}, ", pair.first, pair.second);

        result.push_back(representation);
    }

    return result;
}

void DBReplyData::clear()
{
    data_.clear();
}

bool DBReplyData::isEmpty()
{
    return data_.empty();
}

void DBReplyData::fill(std::size_t count)
{
    std::size_t size = data_.size();

    for (int i = 0; i < count - size; i++)
        data_.push_back(data_.at(0));
}

std::size_t DBReplyData::count(std::size_t position, std::string key)
{
    // TODO return optional
    return data_.at(position).count(key);
}

void DBReplyData::push(std::size_t position, std::string key, const std::string &value)
{
    data_.at(position).at(key) = value;
}

std::size_t DBReplyData::size()
{
    return data_.size();
}

std::string DBReplyData::getValue(std::size_t position, std::string key)
{
    return data_.at(position).at(key);
}

bool DBReplyData::findValue(const std::string &key, const std::string &value)
{
    auto findLambda = [&](auto &iterator)
    {
        return iterator.at(key) == value;
    };

    auto findMapResult = std::find_if (data_.begin(), data_.end(), findLambda);

    return !(findMapResult == data_.end());
}

std::optional<std::string> getValueFromEnvironmentVariable(std::string &&variableName)
{
    auto value = std::getenv(variableName.data());

    if (!value) {
        logArgsError("Did not find environment variable", variableName);

        return std::nullopt;
    }

    return std::string(value);
}

const char *getValueFromEnvironmentVariableAlt1(std::string &&variableName)
{
    auto value = getValueFromEnvironmentVariable(std::forward<std::string>(variableName));

    return getPQArg(value);
}

const char  *getPQArg(const std::optional<std::string> &value)
{
    return (value) ? value.value().data() : nullptr;
}

std::shared_ptr<PGconn> getDBConnection()
{
    // TODO make connection to db secure

    std::shared_ptr<PGconn> dbConnection = nullptr;

    try {
        auto dbAddress = getValueFromEnvironmentVariableAlt1("CHATAPP_DB_ADDRESS");
        auto dbPort = getValueFromEnvironmentVariableAlt1("CHATAPP_DB_PORT");
        auto dbLogin = getValueFromEnvironmentVariableAlt1("CHATAPP_DB_USER");
        auto dbPassword = getValueFromEnvironmentVariableAlt1("CHATAPP_DB_PASSWORD");

        for (const auto &value : { dbAddress, dbPort, dbLogin, dbPassword }) {
            if (!value)
                return nullptr;
        }

        // TODO create formatters
        logArgsWithTemplate(
            "DB connection; address: {0}, port: {1}, login: {2}, password: {3}",
            dbAddress, dbPort, dbLogin, dbPassword);

        // create connection
        std::string infoTemplate { "dbname={0} user={1} password={2} host={3}" };
        const char *dbName = "postgres";  // TODO add var

        auto info = getStringByFormat(infoTemplate, dbName, dbLogin, dbPassword, dbAddress);
        dbConnection = std::shared_ptr<PGconn>(PQconnectdb(info.data()), PQfinish);

        // check connection status
        ConnStatusType status = PQstatus(dbConnection.get());
        std::string mark = (status == 0) ? "OK" : "?";

        logArgsWithTemplate("DB connection status: {0} {1}", std::to_string(status), mark);

        if (status == ConnStatusType::CONNECTION_BAD)  // TODO raise exception
            logArgsError("DB connection status: CONNECTION_BAD");
    }
    catch(const std::exception &e) {
        logArgsException(e.what());
    }

    return dbConnection;
}

std::shared_ptr<PGresult> sendDBRequestAndReturnResult(
    std::shared_ptr<PGconn> connection, bool log, const std::string &command)
{
    if (log)
        logArgs(command);

    std::lock_guard<std::mutex> lock(dbMutex);

    auto lambda = [](PGresult *result)
    {
        if (result)
            PQclear(result);
    };

    std::shared_ptr<PGresult> result(PQexec(connection.get(), command.data()), lambda);

    return result;
}

std::shared_ptr<PGresult> getCompanionsDBResult(std::shared_ptr<PGconn> connection, bool log)
{
    std::string command { "SELECT id, name FROM companions" };

    return sendDBRequestAndReturnResult(connection, log, command);
}

std::shared_ptr<PGresult> getCompanionByNameDBResult(
    std::shared_ptr<PGconn> connection, bool log, const std::string &name)
{
    auto command = getStringByFormat("SELECT id FROM companions WHERE name = '{}'", name);

    return sendDBRequestAndReturnResult(connection, log, command);
}

std::shared_ptr<PGresult> getCompanionAndSocketDBResult(
    std::shared_ptr<PGconn> connection, bool log, const int &id)
{
    auto command = getStringByFormat(
        "SELECT companions.name, sockets.ipaddress, sockets.client_port "
        "FROM companions JOIN sockets ON companions.id = sockets.id "
        "WHERE companions.id = {}", id);

    return sendDBRequestAndReturnResult(connection, log, command);
}

std::shared_ptr<PGresult> getSocketInfoDBResult(
    std::shared_ptr<PGconn> connection, bool log, const int &id)
{
    auto command = getStringByFormat(
        "SELECT ipaddress, server_port, client_port FROM sockets WHERE id = {}", id);

    return sendDBRequestAndReturnResult(connection, log, command);
}

std::shared_ptr<PGresult> getSocketByIpAddressAndPortDBResult(
    std::shared_ptr<PGconn> connection, bool log, const std::string &ipAddress,
    const std::string &port)
{
    auto command = getStringByFormat(
        "SELECT id FROM sockets WHERE ipaddress = '{0}' AND client_port = '{1}'",
        ipAddress, port);

    return sendDBRequestAndReturnResult(connection, log, command);
}

std::shared_ptr<PGresult> getMessagesDBResult(
    std::shared_ptr<PGconn> connection, bool log, const uint8_t &companionId)
{
    auto command = getStringByFormat(
        "WITH select_id AS "
        "(SELECT id FROM companion_messages WHERE companion_id = {0} "
        "ORDER BY timestamp_tz DESC LIMIT {1}) "
        "SELECT id, companion_id, author_id, timestamp_tz, message, is_sent, is_received "
        "FROM companion_messages WHERE id IN (SELECT id FROM select_id) "
        "ORDER BY timestamp_tz ASC", companionId, numberOfMessagesToGetFromDB);

    return sendDBRequestAndReturnResult(connection, log, command);
}

std::shared_ptr<PGresult> getAllMessagesByCompanionIdDBResult(
    std::shared_ptr<PGconn> connection, bool log, const int &companionId)
{
    auto command = getStringByFormat(
        "SELECT author_id, timestamp_tz, message "
        "FROM companion_messages WHERE companion_id = {} "
        "ORDER BY timestamp_tz ASC", companionId);

    return sendDBRequestAndReturnResult(connection, log, command);
}

std::shared_ptr<PGresult> getEarlyMessagesByMessageIdDBResult(
    std::shared_ptr<PGconn> connection, bool log, const int &companionId, const uint32_t &messageId)
{
    auto command = getStringByFormat(
        "WITH select_id AS "
        "(SELECT id FROM companion_messages WHERE companion_id = {0} AND id < {1} "
        "ORDER BY timestamp_tz DESC LIMIT {2}) "
        "SELECT id, companion_id, author_id, timestamp_tz, message, is_sent, is_received "
        "FROM companion_messages WHERE id IN (SELECT id FROM select_id) "
        "ORDER BY timestamp_tz ASC", companionId, messageId, numberOfMessagesToGetFromDB);

    return sendDBRequestAndReturnResult(connection, log, command);
}

std::shared_ptr<PGresult> getMessageByCompanionIdAndTimestampDBResult(
    std::shared_ptr<PGconn> connection, bool log, const uint8_t &companionId,
    const std::string &timestamp)
{
    auto command = getStringByFormat(
        "SELECT id FROM companion_messages WHERE companion_id = {0} AND timestamp_tz = '{1}'",
        companionId, timestamp);

    return sendDBRequestAndReturnResult(connection, log, command);
}

std::shared_ptr<PGresult> getUnsentMessagesByCompanionNameDBResult(
    std::shared_ptr<PGconn> connection, bool log, const std::string &companionName)
{
    auto command = getStringByFormat(
        "SELECT id, author_id, companion_id, timestamp_tz, message, is_received "
        "FROM companion_messages WHERE companion_id = (SELECT id FROM companions WHERE name = '{}') "
        "AND author_id = (SELECT id FROM companions WHERE name = 'me') "
        "AND is_sent IS false", companionName);

    return sendDBRequestAndReturnResult(connection, log, command);
}

std::shared_ptr<PGresult> getPasswordDBResult(std::shared_ptr<PGconn> connection, bool log)
{
    std::string command { "SELECT password FROM passwords" };

    return sendDBRequestAndReturnResult(connection, log, command);
}

std::shared_ptr<PGresult> setMessageIsSentInDbAndReturn(
    std::shared_ptr<PGconn> connection, bool log, const uint32_t &messageId)
{
    auto command = getStringByFormat(
        "UPDATE messages SET is_sent = 'true' WHERE id = {} RETURNING id", messageId);

    return sendDBRequestAndReturnResult(connection, log, command);
}

std::shared_ptr<PGresult> setMessageIsReceivedInDbAndReturn(
    std::shared_ptr<PGconn> connection, bool log, const uint32_t &messageId)
{
    auto command = getStringByFormat(
        "UPDATE messages SET is_received = 'true' WHERE id = {} RETURNING id", messageId);

    return sendDBRequestAndReturnResult(connection, log, command);
}

std::shared_ptr<PGresult> pushCompanionToDBAndReturn(
    std::shared_ptr<PGconn> connection, bool log, const std::string &companionName)
{
    auto command = getStringByFormat(
        "INSERT INTO companions (name) VALUES ('{}') RETURNING id", companionName);

    return sendDBRequestAndReturnResult(connection, log, command);
}

std::shared_ptr<PGresult> updateCompanionAndReturn(  // TODO change function names
    std::shared_ptr<PGconn> connection, bool log, const std::string &companionName)
{
    auto command = getStringByFormat(
        "INSERT INTO companions (name) VALUES ('{}') RETURNING id", companionName);

    return sendDBRequestAndReturnResult(connection, log, command);
}

std::shared_ptr<PGresult> updateCompanionAndSocketAndReturn(
    std::shared_ptr<PGconn> connection, bool log, const CompanionAction &action)
{
    auto command = getStringByFormat(
        "WITH update_name AS (UPDATE companions SET name = '{0}' WHERE id = {1} "
        "RETURNING id) UPDATE sockets SET ipaddress = '{2}', client_port = '{3}' "
        "WHERE id IN (SELECT id FROM update_name) RETURNING id",
        action.getName(), action.getCompanionId(), action.getIpAddress(), action.getClientPort());

    return sendDBRequestAndReturnResult(connection, log, command);
}

std::shared_ptr<PGresult> pushSocketToDBAndReturn(
    std::shared_ptr<PGconn> connection, bool log, const std::string &companionName,
    const std::string &ipAddress, const std::string &serverPort, const std::string &clientPort)
{
    auto command = getStringByFormat(
        "INSERT INTO sockets (id, ipaddress, server_port, client_port) "
        "VALUES ((SELECT id FROM companions WHERE name = '{0}'), '{1}', {2}, {3}) RETURNING id",
        companionName, ipAddress, serverPort, clientPort);

    return sendDBRequestAndReturnResult(connection, log, command);
}

std::shared_ptr<PGresult> pushMessageToDBAndReturn(
    std::shared_ptr<PGconn> connection, bool log, const std::string &companionName,
    const std::string &authorName, const std::string &timestamp,
    const std::string &returningFieldName, const std::string &message, const bool &isSent,
    const bool &isReceived)
{
    auto command = getStringByFormat(
        "INSERT INTO messages "
        "(companion_id, author_id, timestamp_tz, message, is_sent, is_received) "
        "VALUES ((SELECT id FROM companions WHERE name = '{0}'), "
        "(SELECT id FROM companions WHERE name = '{1}'), '{2}', '{3}', {4}, {5}) "
        "RETURNING id, %7, timestamp_tz",
        companionName, authorName, timestamp, message, isSent, isReceived,
        returningFieldName);

    return sendDBRequestAndReturnResult(connection, log, command);
}

std::shared_ptr<PGresult> pushMessageToDBWithAuthorIdAndReturn(
    std::shared_ptr<PGconn> connection, bool log, const std::string &companionName,
    const std::string &authorIdString, const std::string &timestamp,
    const std::string &returningFieldName, const std::string &message, const bool &isSent,
    const bool &isReceived)
{
    auto command = getStringByFormat(
        "INSERT INTO messages "
        "(companion_id, author_id, timestamp_tz, message, is_sent, is_received) "
        "VALUES ((SELECT id FROM companions WHERE name = '{0}'), {1}, '{2}', '{3}', {4}, {5}) "
        "RETURNING id, %7, timestamp_tz", companionName, authorIdString, timestamp,
        message, isSent, isReceived, returningFieldName);

    return sendDBRequestAndReturnResult(connection, log, command);
}

std::shared_ptr<PGresult> pushPasswordToDBAndReturn(
    std::shared_ptr<PGconn> connection, bool log, const std::string &password)
{
    auto command = getStringByFormat(
        "INSERT INTO passwords (password) VALUES ('{}') RETURNING id", password);

    return sendDBRequestAndReturnResult(connection, log, command);
}

std::shared_ptr<PGresult> deleteMessagesFromDBAndReturn(
    std::shared_ptr<PGconn> connection, bool log, const CompanionAction &action)
{
    auto command = getStringByFormat(
        "DELETE FROM companion_messages WHERE companion_id = {} RETURNING companion_id",
        action.getCompanionId());

    return sendDBRequestAndReturnResult(connection, log, command);
}

std::shared_ptr<PGresult> deleteCompanionAndSocketAndReturn(
    std::shared_ptr<PGconn> connection, bool log, const CompanionAction &action)
{
    auto command = getStringByFormat(
        "WITH delete_socket AS (DELETE FROM sockets WHERE id = {} RETURNING id) "
        "DELETE FROM companions WHERE id IN (SELECT id FROM delete_socket) RETURNING id",
        action.getCompanionId());

    return sendDBRequestAndReturnResult(connection, log, command);
}

int getDataFromDBResult(
    bool log, std::shared_ptr<DBReplyData> data, std::shared_ptr<PGresult> result, int maxTuples)
{
    int dataIsOk = 0;

    int ntuples = PQntuples(result.get());
    int nfields = PQnfields(result.get());

    if (log)
        logArgsWithTemplate("ntuples: {0}, nfields: {1}", ntuples, nfields);

    if (ntuples == 0) {
        data->clear();
        return dataIsOk;
    }

    if (maxTuples == 1 and ntuples > 1)
        logTemplateError("{} lines from OneToOne DB request", ntuples);

    // create additional elements in result vector
    data->fill(ntuples);

    dataIsOk = 1;

    for (int i = 0; i < ntuples; i++) {
        std::string logString;

        for (int j = 0; j < nfields; j++) {
            const char *fname = PQfname(result.get(), j);
            std::string fnameString = (fname) ? std::string(fname) : "nullptr";

            auto found = data->count(i, fnameString);

            if (found == 1) {
                const char *value = PQgetvalue(result.get(), i, j);
                data->push(i, fnameString, value);
                logString += getStringByFormat("{0}: {1} ", fnameString, value);
            }
            else {
                dataIsOk = -1;  // TODO return ?
                logDBResultUnknownField(result, i, j);
            }
        }

        // logArgs(logString);
    }

    return dataIsOk;
}
