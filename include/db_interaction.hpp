#ifndef DB_INTERACTION_HPP
#define DB_INTERACTION_HPP

#include <format>
#include <libpq-events.h>
#include <libpq-fe.h>
#include <mutex>
#include <QString>
#include <string>

#include "action.hpp"
#include "logging.hpp"
#include "mainwindow.hpp"
#include "manager.hpp"
#include "utils_cout.hpp"

class Companion;
class CompanionAction;

class DBReplyData {
public:
    DBReplyData(int count, ...);
    DBReplyData(const std::vector<std::string>&);
    ~DBReplyData();

    std::vector<std::map<std::string, std::shared_ptr<char>>>* getDataPtr();
    // std::vector<QString> buildDataQStringVector();
    std::vector<std::string> buildDataStringVector();

    void clear();
    bool isEmpty();
    void fill(std::size_t);
    std::size_t count(std::size_t, std::string);
    void push(std::size_t, std::string, std::shared_ptr<char>);
    std::size_t size();
    std::shared_ptr<char> getValue(std::size_t, std::string);
    bool findValue(const std::string&, const std::string&);    

private:
    std::vector<std::map<std::string, std::shared_ptr<char>>> data_;
};

std::shared_ptr<char> getValueFromEnvironmentVariable(std::shared_ptr<char>);
std::shared_ptr<PGconn> getDBConnection();
std::shared_ptr<PGresult> sendDBRequestAndReturnResult(std::shared_ptr<PGconn>, const bool&, std::shared_ptr<char>);

std::shared_ptr<PGresult> getCompanionsDBResult(std::shared_ptr<PGconn>, const bool&);
std::shared_ptr<PGresult> getCompanionByNameDBResult(std::shared_ptr<PGconn>, const bool&, const std::string&);
std::shared_ptr<PGresult> getCompanionAndSocketDBResult(std::shared_ptr<PGconn>, const bool&, const int&);

std::shared_ptr<PGresult> getSocketInfoDBResult(std::shared_ptr<PGconn>, const bool&, const int&);

std::shared_ptr<PGresult> getSocketByIpAddressAndPortDBResult(
    std::shared_ptr<PGconn>, const bool&, const std::string&, const std::string&);

std::shared_ptr<PGresult> getMessagesDBResult(std::shared_ptr<PGconn>, const bool&, const uint8_t&);
std::shared_ptr<PGresult> getAllMessagesByCompanionIdDBResult(std::shared_ptr<PGconn>, const bool&, const int&);

std::shared_ptr<PGresult> getEarlyMessagesByMessageIdDBResult(
    std::shared_ptr<PGconn>, const bool&, const int&, const uint32_t&);

std::shared_ptr<PGresult> getMessageByCompanionIdAndTimestampDBResult(
    std::shared_ptr<PGconn>, const bool&, const uint8_t&, const std::string&);

std::shared_ptr<PGresult> getUnsentMessagesByCompanionNameDBResult(
    std::shared_ptr<PGconn>, const bool&, const std::string&);

std::shared_ptr<PGresult> getPasswordDBResult(std::shared_ptr<PGconn>, const bool&);

std::shared_ptr<PGresult> setMessageIsSentInDbAndReturn(std::shared_ptr<PGconn>, const bool&, const uint32_t&);
std::shared_ptr<PGresult> setMessageIsReceivedInDbAndReturn(std::shared_ptr<PGconn>, const bool&, const uint32_t&);

std::shared_ptr<PGresult> pushCompanionToDBAndReturn(std::shared_ptr<PGconn>, const bool&, const std::string&);
std::shared_ptr<PGresult> updateCompanionAndReturn(std::shared_ptr<PGconn>, const bool&, const std::string&);

std::shared_ptr<PGresult> updateCompanionAndSocketAndReturn(
    std::shared_ptr<PGconn>, const bool&, const CompanionAction&);

std::shared_ptr<PGresult> pushSocketToDBAndReturn(
    std::shared_ptr<PGconn>, const bool&, const std::string&, const std::string&,
    const std::string&, const std::string&);

std::shared_ptr<PGresult> pushMessageToDBAndReturn(
    std::shared_ptr<PGconn>, const bool&, const std::string&, const std::string&,
    const std::string&, const std::string&, const std::string&,
    const bool&, const bool&);

std::shared_ptr<PGresult> pushMessageToDBWithAuthorIdAndReturn(
    std::shared_ptr<PGconn>, const bool&, const std::string&, const std::string&,
    const std::string&, const std::string&, const std::string&,
    const bool&, const bool&);

std::shared_ptr<PGresult> pushPasswordToDBAndReturn(std::shared_ptr<PGconn>, const bool&, const std::string&);

std::shared_ptr<PGresult> deleteMessagesFromDBAndReturn(std::shared_ptr<PGconn>, const bool&, const CompanionAction&);

std::shared_ptr<PGresult> deleteCompanionAndSocketAndReturn(
    std::shared_ptr<PGconn>, const bool&, const CompanionAction&);

int getDataFromDBResult(
    const bool&, std::shared_ptr<DBReplyData>&, std::shared_ptr<PGresult>, int);

#endif // DB_INTERACTION_HPP
