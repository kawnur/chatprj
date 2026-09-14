#include "db_interaction.hpp"

#include "action.hpp"
#include "logging.hpp"
#include "utils.hpp"

DBRequestData::DBRequestData(DBRequestType type) : logMark_(), requestTemplate_(), replyKeys_()
{
    // get value from mapping
    if (!DB_REQUEST_DATA_MAP.contains(type))
        return;

    auto value = DB_REQUEST_DATA_MAP.at(type);
    auto size = value.size();

    // check value size
    if (size < 3) {
        logTemplateError("dbRequestDataMap value size {} is less than expected", size);

        return;
    }

    // set object fields
    logMark_ = value.at(0);
    requestTemplate_ = value.at(1);

    for (std::size_t i = 2; i < size; ++i)
        replyKeys_.push_back(value.at(i));
}

std::string DBRequestData::getLogMark() const
{
    return logMark_;
}

std::vector<std::string> DBRequestData::getReplyKeys() const
{
    return replyKeys_;
}

bool DBRequestData::isValid()
{
    bool logMarkIsNotEmpty = (!logMark_.empty());
    bool requestTemplateIsNotEmpty = (!requestTemplate_.empty());
    bool replyKeysIsNotEmpty = (!replyKeys_.empty());

    logTemplateDebug(
        "{0}, logMarkIsNotEmpty: {1}, requestTemplateIsNotEmpty: {2}, replyKeysIsNotEmpty: {3}",
        __FUNCTION__, logMarkIsNotEmpty, requestTemplateIsNotEmpty, replyKeysIsNotEmpty);

    return logMarkIsNotEmpty && requestTemplateIsNotEmpty && replyKeysIsNotEmpty;
}

std::string DBRequestData::getReplyKeysString()
{
    std::string result { "" };

    auto lastItemIndex = replyKeys_.size() - 1;

    for (std::size_t i = 0; i < lastItemIndex; ++i)
        result += getStringByFormat("{}, ", replyKeys_.at(i));

    result += replyKeys_.at(lastItemIndex);

    return result;
}

DBReplyData::DBReplyData(bool log, int count, ...) : log_(log), data_(1)
{
    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; i++) {
        auto key = std::string(va_arg(args, char *));

        data_.at(0).insert({key, ""});
    }

    va_end(args);
}

DBReplyData::DBReplyData(bool log, const std::vector<std::string> &keys) : log_(log), data_(1)
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
    auto lambda = [&](const auto &iterator)
    {
        return iterator.at(key) == value;
    };

    auto result = std::ranges::find_if(data_, lambda);

    return !(result == data_.end());
}

int DBReplyData::getDataFromResult(std::shared_ptr<PGresult> result, int maxTuples)
{
    int dataIsOk = 0;

    int ntuples = PQntuples(result.get());
    int nfields = PQnfields(result.get());

    if (log_)
        logArgsWithTemplate("ntuples: {0}, nfields: {1}", ntuples, nfields);

    if (ntuples == 0) {
        clear();

        return dataIsOk;
    }

    if (maxTuples == 1 and ntuples > 1)
        logTemplateError("{} lines from OneToOne DB request", ntuples);

    // create additional elements in result vector
    fill(ntuples);

    dataIsOk = 1;

    for (int i = 0; i < ntuples; i++) {
        std::string logString;

        for (int j = 0; j < nfields; j++) {
            const char *fname = PQfname(result.get(), j);
            std::string fnameString = (fname) ? std::string(fname) : "nullptr";

            auto found = count(i, fnameString);

            if (found == 1) {
                const char *value = PQgetvalue(result.get(), i, j);
                push(i, fnameString, value);
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

DBRequester::DBRequester(bool log) : log_(log), mutex_()
{
    connection_ = getDBConnection();
}

std::shared_ptr<PGresult> DBRequester::sendRequestAndReturnResult(const std::string &command)
{
    if (log_)
        logArgs(command);

    std::lock_guard<std::mutex> lock(mutex_);

    auto lambda = [](PGresult *result)
    {
        if (result)
            PQclear(result);
    };

    std::shared_ptr<PGresult> result(PQexec(connection_.get(), command.data()), lambda);

    return result;
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
