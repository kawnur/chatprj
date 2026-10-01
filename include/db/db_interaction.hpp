#ifndef DB_INTERACTION_HPP
#define DB_INTERACTION_HPP

#include <libpq-events.h>
#include <libpq-fe.h>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

#include <QString>
#include <QWidget>

#include "db_constants.hpp"
#include "logging.hpp"
#include "utils_widgets.hpp"

class Companion;
class CompanionAction;

class DBConnectionPrameters
{
public:
    DBConnectionPrameters();
    ~DBConnectionPrameters() = default;

    bool isValid();
    void log();
    std::string getConnectionInfo();

private:
    std::optional<std::string> address_;
    std::optional<std::string> port_;
    std::optional<std::string> login_;
    std::optional<std::string> password_;
    std::optional<std::string> dbName_;
};

class DBRequestData
{
public:
    DBRequestData(DBRequestType type);
    ~DBRequestData() { /*logArgsInfo(__FUNCTION__);*/ }

    std::string getLogMark() const;
    std::vector<std::string> getReplyKeys() const;

    bool isValid();
    std::string getReplyKeysString();

    template<typename... Ts>
    std::string buildCommand(Ts &&...args)
    {
        return getStringByFormat(requestTemplate_, getReplyKeysString(), args...);
    }

private:
    std::string logMark_;
    std::string requestTemplate_;
    std::vector<std::string> replyKeys_;
};

class DBReplyData
{
public:
    DBReplyData(bool log, int count, ...);
    DBReplyData(bool log, const std::vector<std::string> &keys);
    ~DBReplyData() = default;

    std::vector<std::string> buildDataStringVector();
    void clear();
    bool isEmpty();
    void fill(std::size_t);
    std::size_t count(std::size_t, std::string);
    void push(std::size_t, std::string, const std::string &value);
    std::size_t size();
    std::string getValue(std::size_t, std::string);
    bool findValue(const std::string&, const std::string &);    

    int getDataFromResult(std::shared_ptr<PGresult> result, int maxTuples);
    void log();

private:
    bool log_;
    std::vector<std::map<std::string, std::string>> data_;
};

class DBRequester
{
public:
    DBRequester(bool log);
    ~DBRequester() = default;

    template<typename... Ts>
    std::shared_ptr<DBReplyData> getDBData(DBRequestData &requestData, Ts &&...args)
    {
        if (!requestData.isValid()) {
            logArgsError("requestData is invalid");

            return nullptr;
        }

        auto command = requestData.buildCommand(args...);
        std::shared_ptr<PGresult> dbResult = sendRequestAndReturnResult(command);

        if (log_) {
            logArgs(logDelimiter);
            logArgs(requestData.getLogMark());
            logArgs("dbResult:", dbResult.get());
        }

        if (!dbResult) {
            showErrorDialogAndLogError("Database request error, dbResult is nullptr");

            return nullptr;
        }

        auto replyData = std::make_shared<DBReplyData>(log_, requestData.getReplyKeys());

        if (replyData->getDataFromResult(dbResult, 0) == -1) {
            showErrorDialogAndLogError("Error getting data from dbResult");

            return nullptr;
        }

        if (log_) {
            // logArgs("dbData->size():", dbData->size());
            replyData->log();
            logArgs(logDelimiter);
        }

        return replyData;
    }

    std::shared_ptr<PGresult> sendRequestAndReturnResult(const std::string &command);
    bool isReady();

private:
    bool log_;
    std::mutex mutex_;  // ???
    std::shared_ptr<PGconn> connection_;
};

std::optional<std::string> getValueFromEnvironmentVariable(std::string &&variableName);
const char *getPQArg(const std::optional<std::string> &value);
std::shared_ptr<PGconn> getDBConnection();

std::string buildChatHistoryJSONString(
    std::shared_ptr<DBReplyData> data, std::vector<std::string> &keys);

bool getConnectionStatus(std::shared_ptr<PGconn> connection);

#endif // DB_INTERACTION_HPP
