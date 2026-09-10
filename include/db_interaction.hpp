#ifndef DB_INTERACTION_HPP
#define DB_INTERACTION_HPP

#include <libpq-events.h>
#include <libpq-fe.h>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <QString>

class Companion;
class CompanionAction;

class DBReplyData
{
public:
    DBReplyData(int count, ...);
    DBReplyData(const std::vector<std::string> &keys);
    ~DBReplyData() = default;

    std::vector<std::string> buildDataStringVector();
    void clear();
    bool isEmpty();
    void fill(std::size_t);
    std::size_t count(std::size_t, std::string);
    void push(std::size_t, std::string, const std::string &value);
    std::size_t size();
    std::string getValue(std::size_t, std::string);
    bool findValue(const std::string&, const std::string&);    

private:
    std::vector<std::map<std::string, std::string>> data_;
};

std::optional<std::string> getValueFromEnvironmentVariable(std::string &&variableName);
const char *getValueFromEnvironmentVariableAlt1(std::string &&variableName);
const char *getPQArg(const std::optional<std::string> &value);
std::shared_ptr<PGconn> getDBConnection();

std::shared_ptr<PGresult> sendDBRequestAndReturnResult(
    std::shared_ptr<PGconn> dbConnection, const bool &logging, const std::string &command);

std::shared_ptr<PGresult> getCompanionsDBResult(
    std::shared_ptr<PGconn> dbConnection, const bool &logging);

std::shared_ptr<PGresult> getCompanionByNameDBResult(
    std::shared_ptr<PGconn> dbConnection, const bool &logging, const std::string &name);

std::shared_ptr<PGresult> getCompanionAndSocketDBResult(
    std::shared_ptr<PGconn> dbConnection, const bool &logging, const int &id);

std::shared_ptr<PGresult> getSocketInfoDBResult(
    std::shared_ptr<PGconn> dbConnection, const bool &logging, const int &id);

std::shared_ptr<PGresult> getSocketByIpAddressAndPortDBResult(
    std::shared_ptr<PGconn> dbConnection, const bool &logging, const std::string &ipAddress,
    const std::string &port);

std::shared_ptr<PGresult> getMessagesDBResult(
    std::shared_ptr<PGconn> dbConnection, const bool &logging, const uint8_t &companionId);

std::shared_ptr<PGresult> getAllMessagesByCompanionIdDBResult(
    std::shared_ptr<PGconn> dbConnection, const bool &logging, const int &companionId);

std::shared_ptr<PGresult> getEarlyMessagesByMessageIdDBResult(
    std::shared_ptr<PGconn> dbConnection, const bool &logging,
    const int &companionId,const uint32_t &messageId);

std::shared_ptr<PGresult> getMessageByCompanionIdAndTimestampDBResult(
    std::shared_ptr<PGconn> dbConnection, const bool &logging, const uint8_t &companionId,
    const std::string &timestamp);

std::shared_ptr<PGresult> getUnsentMessagesByCompanionNameDBResult(
    std::shared_ptr<PGconn> dbConnection, const bool &logging, const std::string &companionName);

std::shared_ptr<PGresult> getPasswordDBResult(
    std::shared_ptr<PGconn> dbConnection, const bool &logging);

std::shared_ptr<PGresult> setMessageIsSentInDbAndReturn(
    std::shared_ptr<PGconn> dbConnection, const bool &logging, const uint32_t &messageId);

std::shared_ptr<PGresult> setMessageIsReceivedInDbAndReturn(
    std::shared_ptr<PGconn> dbConnection, const bool &logging, const uint32_t &messageId);

std::shared_ptr<PGresult> pushCompanionToDBAndReturn(
    std::shared_ptr<PGconn> dbConnection, const bool &logging, const std::string &companionName);

std::shared_ptr<PGresult> updateCompanionAndReturn(
    std::shared_ptr<PGconn> dbConnection, const bool &logging, const std::string &companionName);

std::shared_ptr<PGresult> updateCompanionAndSocketAndReturn(
    std::shared_ptr<PGconn> dbConnection, const bool &logging,
    const CompanionAction &companionAction);

std::shared_ptr<PGresult> pushSocketToDBAndReturn(
    std::shared_ptr<PGconn> dbConnection, const bool &logging, const std::string &companionName,
    const std::string &ipAddress, const std::string &serverPort, const std::string &clientPort);

std::shared_ptr<PGresult> pushMessageToDBAndReturn(
    std::shared_ptr<PGconn> dbConnection, const bool &logging, const std::string &companionName,
    const std::string &authorName, const std::string &timestamp,
    const std::string &returningFieldName, const std::string &message, const bool &isSent,
    const bool &isReceived);

std::shared_ptr<PGresult> pushMessageToDBWithAuthorIdAndReturn(
    std::shared_ptr<PGconn> dbConnection, const bool &logging, const std::string &companionName,
    const std::string &authorIdString, const std::string &timestamp,
    const std::string &returningFieldName, const std::string &message, const bool &isSent,
    const bool &isReceived);

std::shared_ptr<PGresult> pushPasswordToDBAndReturn(
    std::shared_ptr<PGconn> dbConnection, const bool &logging, const std::string &password);

std::shared_ptr<PGresult> deleteMessagesFromDBAndReturn(
    std::shared_ptr<PGconn> dbConnection, const bool &logging, const CompanionAction &action);

std::shared_ptr<PGresult> deleteCompanionAndSocketAndReturn(
    std::shared_ptr<PGconn> dbConnection, const bool &logging, const CompanionAction &action);

int getDataFromDBResult(
    const bool &logging, std::shared_ptr<DBReplyData> data, std::shared_ptr<PGresult> result,
    int maxTuples);

#endif // DB_INTERACTION_HPP
