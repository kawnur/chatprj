#ifndef DB_INTERACTION_HPP
#define DB_INTERACTION_HPP

#include <libpq-events.h>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "db_constants.hpp"

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

    std::string logMark() const;
    std::vector<std::string> replyKeys() const;

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
    std::string getValue(std::size_t position, const std::string &key);
    bool findValue(const std::string&, const std::string &);    

    int getDataFromResult(std::shared_ptr<PGresult> result, int maxTuples);
    void log();

private:
    bool log_;
    std::vector<std::map<std::string, std::string>> data_;
};

std::string buildChatHistoryJSONString(
    std::shared_ptr<DBReplyData> data, std::vector<std::string> &keys);

#endif // DB_INTERACTION_HPP
