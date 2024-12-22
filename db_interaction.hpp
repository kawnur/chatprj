#ifndef DB_INTERACTION_HPP
#define DB_INTERACTION_HPP

#include <libpq-events.h>
#include <libpq-fe.h>
#include <string>

#include "logging.hpp"
#include "mainwindow.hpp"
#include "manager.hpp"
#include "utils_cout.hpp"

class Companion;
class CompanionAction;

class DBReplyData
{
public:
    DBReplyData(int count, ...);
    DBReplyData(const std::vector<std::string>&);
    ~DBReplyData();

    void clear();
    bool isEmpty();
    void fill(size_t);
    size_t count(size_t, std::string);
    void push(size_t, std::string, const char*);
    size_t size();
    const char* getValue(size_t, std::string);
    bool findValue(const std::string&, const std::string&);
    void logData();

private:
    std::vector<std::map<std::string, const char*>> data_;
};

const char* getValueFromEnvironmentVariable(const char*);
PGconn* getDBConnection();
PGresult* sendDBRequestAndReturnResult(const PGconn*, const char*);

PGresult* getCompanionsDBResult(const PGconn*);
PGresult* getCompanionByNameDBResult(const PGconn*, const std::string&);
PGresult* getCompanionAndSocketDBResult(const PGconn*, const int&);

PGresult* getSocketInfoDBResult(const PGconn*, const int&);

PGresult* getSocketByIpAddressAndPortDBResult(
    const PGconn*, const std::string&, const std::string&);

PGresult* getMessagesDBResult(const PGconn*, const int&);

PGresult* getPasswordDBResult(const PGconn*);

PGresult* pushCompanionToDBAndReturn(const PGconn*, const std::string&);
PGresult* updateCompanionAndReturn(const PGconn*, const std::string&);
PGresult* updateCompanionAndSocketAndReturn(const PGconn*, const CompanionAction&);

PGresult* pushSocketToDBAndReturn(
    const PGconn*, const std::string&, const std::string&,
    const std::string&, const std::string&);

PGresult* pushMessageToDBAndReturn(
    const PGconn*, const std::string&, const std::string&,
    const std::string&, const std::string&);

PGresult* deleteMessagesFromDBAndReturn(const PGconn*, const CompanionAction&);
PGresult* deleteCompanionAndSocketAndReturn(const PGconn*, const CompanionAction&);

void logUnknownField(const PGresult*, int, int);

int getDataFromDBResult(std::shared_ptr<DBReplyData>&, const PGresult*, int);

#endif // DB_INTERACTION_HPP
