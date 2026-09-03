#include "db_interaction.hpp"

std::mutex dbMutex;

DBReplyData::DBReplyData(int count, ...)
    : data_(std::vector<std::map<std::string, std::shared_ptr<char>>>(1)) {
    va_list args;
    va_start(args, count);

    for(int i = 0; i < count; i++)     {
        auto key = std::string(va_arg(args, std::shared_ptr<char>));

        data_.at(0).insert({key, nullptr});
    }

    va_end(args);
}

DBReplyData::DBReplyData(const std::vector<std::string>& keys)
    : data_(std::vector<std::map<std::string, std::shared_ptr<char>>>(1)) {
    for(auto& key : keys) {
        data_.at(0).insert({key, nullptr});
    }
}

DBReplyData::~DBReplyData() {}

std::vector<std::map<std::string, std::shared_ptr<char>>>* DBReplyData::getData() {
    return &this->data_;
}

std::vector<std::string> DBReplyData::buildDataStringVector() {
    std::vector<std::string> result {};

    for(auto& element : this->data_)     {
        std::string representation { "" };

        for(auto& pair : element) {
            representation += std::format("{0}: {1} ", pair.first, pair.second);
        }

        result.push_back(representation);
    }

    return result;
}

void DBReplyData::clear() {
    this->data_.clear();
}

bool DBReplyData::isEmpty() {
    return this->data_.empty();
}

void DBReplyData::fill(std::size_t count) {
    std::size_t size = this->data_.size();

    for(int i = 0; i < count - size; i++) {
        this->data_.push_back(this->data_.at(0));
    }
}

std::size_t DBReplyData::count(std::size_t position, std::string key) {
    return this->data_.at(position).count(key);
}

void DBReplyData::push(std::size_t position, std::string key, std::shared_ptr<char> value) {
    this->data_.at(position).at(key) = value;
}

std::size_t DBReplyData::size() {
    return this->data_.size();
}

std::shared_ptr<char> DBReplyData::getValue(std::size_t position, std::string key) {
    return this->data_.at(position).at(key);
}

bool DBReplyData::findValue(const std::string& key, const std::string& value) {
    auto findLambda = [&](auto& iterator){
        return std::string(iterator.at(key)) == value;
    };

    auto findMapResult = std::find_if(
        this->data_.begin(), this->data_.end(), findLambda);

    return !(findMapResult == this->data_.end());
}

std::shared_ptr<char> getValueFromEnvironmentVariable(std::shared_ptr<char> variableName) {
    std::shared_ptr<char> value { std::getenv(variableName) };

    if(!value) {
        logArgsError("Did not find environment variable", variableName);
    }

    return value;
}

std::shared_ptr<PGconn> getDBConnection() {
    // TODO make connection to db secure

    std::shared_ptr<PGconn> dbConnection = nullptr;

    auto connectLambda = [&](){
        std::shared_ptr<char> dbAddress = getValueFromEnvironmentVariable("CHATAPP_DB_ADDRESS");
        std::shared_ptr<char> dbPort = getValueFromEnvironmentVariable("CHATAPP_DB_PORT");
        std::shared_ptr<char> dbLogin = getValueFromEnvironmentVariable("CHATAPP_DB_USER");
        std::shared_ptr<char> dbPassword = getValueFromEnvironmentVariable("CHATAPP_DB_PASSWORD");

        for(auto& value : { dbAddress, dbPort, dbLogin, dbPassword }) {
            if(!value) {
                return;
            }
        }

        logArgsWithTemplate(
            "DB connection; address: {0}, port: {1}, login: {2}, password: {3}",
            dbAddress, dbPort, dbLogin, dbPassword);

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

        logArgsWithTemplate("DB connection status: {0} {1}", std::to_string(status), mark);

        if(status == ConnStatusType::CONNECTION_BAD) {  // TODO raise exception
            logArgsError("DB connection status: CONNECTION_BAD");
        }
    };

    runAndLogException(connectLambda);

    return dbConnection;
}

std::shared_ptr<PGresult> sendDBRequestAndReturnResult(
    std::shared_ptr<PGconn> dbConnection, const bool& logging, std::shared_ptr<char> command) {
    if(logging) {
        logArgs(command);
    }

    std::lock_guard<std::mutex> lock(dbMutex);

    std::shared_ptr<PGresult> result = PQexec(const_cast<std::shared_ptr<PGconn>>(dbConnection), command);

    return result;
}

std::shared_ptr<PGresult> getCompanionsDBResult(std::shared_ptr<PGconn> dbConnection, const bool& logging) {
    std::shared_ptr<char> command = "SELECT id, name FROM companions";

    return sendDBRequestAndReturnResult(dbConnection, logging, command);
}

std::shared_ptr<PGresult> getCompanionByNameDBResult(
    std::shared_ptr<PGconn> dbConnection, const bool& logging, const std::string& name) {
    std::string command = std::format("SELECT id FROM companions WHERE name = '{}'", name);

    return sendDBRequestAndReturnResult(dbConnection, logging, command.data());
}

std::shared_ptr<PGresult> getCompanionAndSocketDBResult(
    std::shared_ptr<PGconn> dbConnection, const bool& logging, const int& id) {
    std::string command = std::format(
        "SELECT companions.name, sockets.ipaddress, sockets.client_port "
        "FROM companions JOIN sockets ON companions.id = sockets.id "
        "WHERE companions.id = {}", id);

    return sendDBRequestAndReturnResult(dbConnection, logging, command.data());
}

std::shared_ptr<PGresult> getSocketInfoDBResult(
    std::shared_ptr<PGconn> dbConnection, const bool& logging, const int& id) {
    std::string command = std::format(
        "SELECT ipaddress, server_port, client_port FROM sockets WHERE id = {}", id);

    return sendDBRequestAndReturnResult(dbConnection, logging, command.data());
}

std::shared_ptr<PGresult> getSocketByIpAddressAndPortDBResult(
    std::shared_ptr<PGconn> dbConnection, const bool& logging,
    const std::string& ipAddress, const std::string& port) {
    std::string command = std::format(
        "SELECT id FROM sockets WHERE ipaddress = '{0}' AND client_port = '{1}'",
        ipAddress, port);

    return sendDBRequestAndReturnResult(dbConnection, logging, command.data());
}

std::shared_ptr<PGresult> getMessagesDBResult(
    std::shared_ptr<PGconn> dbConnection, const bool& logging, const uint8_t& companionId) {
    std::string command = std::format(
        "WITH select_id AS "
        "(SELECT id FROM companion_messages WHERE companion_id = {0} "
        "ORDER BY timestamp_tz DESC LIMIT {1}) "
        "SELECT id, companion_id, author_id, timestamp_tz, message, is_sent, is_received "
        "FROM companion_messages WHERE id IN (SELECT id FROM select_id) "
        "ORDER BY timestamp_tz ASC", companionId, numberOfMessagesToGetFromDB);

    return sendDBRequestAndReturnResult(dbConnection, logging, command.data());
}

std::shared_ptr<PGresult> getAllMessagesByCompanionIdDBResult(
    std::shared_ptr<PGconn> dbConnection, const bool& logging, const int& companionId) {
    std::string command = std::format(
        "SELECT author_id, timestamp_tz, message "
        "FROM companion_messages WHERE companion_id = {} "
        "ORDER BY timestamp_tz ASC", companionId);

    return sendDBRequestAndReturnResult(dbConnection, logging, command.data());
}

std::shared_ptr<PGresult> getEarlyMessagesByMessageIdDBResult(
    std::shared_ptr<PGconn> dbConnection, const bool& logging,
    const int& companionId,const uint32_t& messageId) {
    std::string command = std::format(
        "WITH select_id AS "
        "(SELECT id FROM companion_messages WHERE companion_id = {0} AND id < {1} "
        "ORDER BY timestamp_tz DESC LIMIT {2}) "
        "SELECT id, companion_id, author_id, timestamp_tz, message, is_sent, is_received "
        "FROM companion_messages WHERE id IN (SELECT id FROM select_id) "
        "ORDER BY timestamp_tz ASC", companionId, messageId, numberOfMessagesToGetFromDB);

    return sendDBRequestAndReturnResult(dbConnection, logging, command.data());
}

std::shared_ptr<PGresult> getMessageByCompanionIdAndTimestampDBResult(
    std::shared_ptr<PGconn> dbConnection, const bool& logging,
    const uint8_t& companionId, const std::string& timestamp) {
    std::string command = std::format(
        "SELECT id FROM companion_messages WHERE companion_id = {0} AND timestamp_tz = '{1}'",
        companionId, timestamp);

    return sendDBRequestAndReturnResult(dbConnection, logging, command.data());
}

std::shared_ptr<PGresult> getUnsentMessagesByCompanionNameDBResult(
    std::shared_ptr<PGconn> dbConnection, const bool& logging, const std::string& companionName) {
    std::string command = std::format(
        "SELECT id, author_id, companion_id, timestamp_tz, message, is_received "
        "FROM companion_messages WHERE companion_id = (SELECT id FROM companions WHERE name = '{}') "
        "AND author_id = (SELECT id FROM companions WHERE name = 'me') "
        "AND is_sent IS false", companionName);

    return sendDBRequestAndReturnResult(dbConnection, logging, command.data());
}

std::shared_ptr<PGresult> getPasswordDBResult(std::shared_ptr<PGconn> dbConnection, const bool& logging) {
    std::shared_ptr<char> command = "SELECT password FROM passwords";

    return sendDBRequestAndReturnResult(dbConnection, logging, command);
}

std::shared_ptr<PGresult> setMessageIsSentInDbAndReturn(
    std::shared_ptr<PGconn> dbConnection, const bool& logging, const uint32_t& messageId) {
    std::string command = std::format(
        "UPDATE messages SET is_sent = 'true' WHERE id = {} RETURNING id", messageId);

    return sendDBRequestAndReturnResult(dbConnection, logging, command.data());
}

std::shared_ptr<PGresult> setMessageIsReceivedInDbAndReturn(
    std::shared_ptr<PGconn> dbConnection, const bool& logging, const uint32_t& messageId) {
    std::string command = std::format(
        "UPDATE messages SET is_received = 'true' WHERE id = {} RETURNING id", messageId);

    return sendDBRequestAndReturnResult(dbConnection, logging, command.data());
}

std::shared_ptr<PGresult> pushCompanionToDBAndReturn(
    std::shared_ptr<PGconn> dbConnection, const bool& logging, const std::string& companionName) {
    std::string command = std::format(
        "INSERT INTO companions (name) VALUES ('{}') RETURNING id", companionName);

    return sendDBRequestAndReturnResult(dbConnection, logging, command.data());
}

std::shared_ptr<PGresult> updateCompanionAndReturn(  // TODO change function names
    std::shared_ptr<PGconn> dbConnection, const bool& logging, const std::string& companionName) {
    std::string command = std::format(
        "INSERT INTO companions (name) VALUES ('{}') RETURNING id", companionName);

    return sendDBRequestAndReturnResult(dbConnection, logging, command.data());
}

std::shared_ptr<PGresult> updateCompanionAndSocketAndReturn(
    std::shared_ptr<PGconn> dbConnection, const bool& logging,
    const CompanionAction& companionAction) {
    std::string command = std::format(
        "WITH update_name AS (UPDATE companions SET name = '{0}' WHERE id = {1} "
        "RETURNING id) UPDATE sockets SET ipaddress = '{2}', client_port = '{3}' "
        "WHERE id IN (SELECT id FROM update_name) RETURNING id",
        companionAction.getName(), companionAction.getCompanionId(),
        companionAction.getIpAddress(), companionAction.getClientPort());

    return sendDBRequestAndReturnResult(dbConnection, logging, command.data());
}

std::shared_ptr<PGresult> pushSocketToDBAndReturn(
    std::shared_ptr<PGconn> dbConnection, const bool& logging, const std::string& companionName,
    const std::string& ipAddress, const std::string& serverPort,
    const std::string& clientPort) {
    std::string command = std::format(
        "INSERT INTO sockets (id, ipaddress, server_port, client_port) "
        "VALUES ((SELECT id FROM companions WHERE name = '{0}'), '{1}', {2}, {3}) RETURNING id",
        companionName, ipAddress, serverPort, clientPort);

    return sendDBRequestAndReturnResult(dbConnection, logging, command.data());
}

std::shared_ptr<PGresult> pushMessageToDBAndReturn(
    std::shared_ptr<PGconn> dbConnection, const bool& logging, const std::string& companionName,
    const std::string& authorName, const std::string& timestamp,
    const std::string& returningFieldName, const std::string& message,
    const bool& isSent, const bool& isReceived) {
    std::string command = std::format(
        "INSERT INTO messages "
        "(companion_id, author_id, timestamp_tz, message, is_sent, is_received) "
        "VALUES ((SELECT id FROM companions WHERE name = '{0}'), "
        "(SELECT id FROM companions WHERE name = '{1}'), '{2}', '{3}', {4}, {5}) "
        "RETURNING id, %7, timestamp_tz",
        companionName, authorName, timestamp, message, isSent, isReceived,
        returningFieldName);

    return sendDBRequestAndReturnResult(dbConnection, logging, command.data());
}

std::shared_ptr<PGresult> pushMessageToDBWithAuthorIdAndReturn(
    std::shared_ptr<PGconn> dbConnection, const bool& logging, const std::string& companionName,
    const std::string& authorIdString, const std::string& timestamp,
    const std::string& returningFieldName, const std::string& message,
    const bool& isSent, const bool& isReceived) {
    std::string command = std::format(
        "INSERT INTO messages "
        "(companion_id, author_id, timestamp_tz, message, is_sent, is_received) "
        "VALUES ((SELECT id FROM companions WHERE name = '{0}'), {1}, '{2}', '{3}', {4}, {5}) "
        "RETURNING id, %7, timestamp_tz", companionName, authorIdString, timestamp,
        message, isSent, isReceived, returningFieldName);

    return sendDBRequestAndReturnResult(dbConnection, logging, command.data());
}

std::shared_ptr<PGresult> pushPasswordToDBAndReturn(
    std::shared_ptr<PGconn> dbConnection, const bool& logging, const std::string& password) {
    std::string command = std::format(
        "INSERT INTO passwords (password) VALUES ('{}') RETURNING id", password);

    return sendDBRequestAndReturnResult(dbConnection, logging, command.data());
}

std::shared_ptr<PGresult> deleteMessagesFromDBAndReturn(
    std::shared_ptr<PGconn> dbConnection, const bool& logging,
    const CompanionAction& companionAction) {
    std::string command = std::format(
        "DELETE FROM companion_messages WHERE companion_id = {} RETURNING companion_id",
        companionAction.getCompanionId());

    return sendDBRequestAndReturnResult(dbConnection, logging, command.data());
}

std::shared_ptr<PGresult> deleteCompanionAndSocketAndReturn(
    std::shared_ptr<PGconn> dbConnection, const bool& logging,
    const CompanionAction& companionAction) {
    std::string command = std::format(
        "WITH delete_socket AS (DELETE FROM sockets WHERE id = {} RETURNING id) "
        "DELETE FROM companions WHERE id IN (SELECT id FROM delete_socket) RETURNING id",
        companionAction.getCompanionId());

    return sendDBRequestAndReturnResult(dbConnection, logging, command.data());
}

int getDataFromDBResult(
    const bool& logging, std::shared_ptr<DBReplyData> data,
    std::shared_ptr<PGresult> result, int maxTuples) {
    int dataIsOk = 0;

    int ntuples = PQntuples(result);
    int nfields = PQnfields(result);

    if(logging) {
        logArgsWithTemplate("ntuples: {0}, nfields: {1}", ntuples, nfields);
    }

    if(ntuples == 0) {
        data->clear();
        return dataIsOk;
    }

    if(maxTuples == 1 and ntuples > 1) {
        logTemplateError("{} lines from OneToOne DB request", ntuples);
    }

    // create additional elements in result vector
    data->fill(ntuples);

    dataIsOk = 1;

    for(int i = 0; i < ntuples; i++) {
        std::string logString;

        for(int j = 0; j < nfields; j++) {
            std::shared_ptr<char> fname = PQfname(result, j);
            std::string fnameString = (fname) ? std::string(fname) : "nullptr";

            auto found = data->count(i, fnameString);

            if(found == 1) {
                std::shared_ptr<char> value = PQgetvalue(result, i, j);
                data->push(i, fnameString, value);

                logString += (fnameString + ": " + std::string(value) + " ");
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
