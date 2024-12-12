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

class DBReplyData
{
public:
    DBReplyData(int count, ...);
    ~DBReplyData();

    void clear();
    void fill(size_t);
    size_t count(size_t, std::string);
    void push(size_t, std::string, const char*);
    size_t size();
    const char* getValue(size_t, std::string);
    void logData();

private:
    std::vector<std::map<std::string, const char*>> data_;
};

const char* getValueFromEnvironmentVariable(const char*);
PGconn* getDBConnection();
PGresult* sendDBRequestAndReturnResult(const PGconn*, const char*);

PGresult* getCompanionsDBResult(const PGconn*);
PGresult* getCompanionByNameDBResult(const PGconn*, const std::string&);

PGresult* getSocketInfoDBResult(const PGconn*, int);
PGresult* getSocketByIpAddressAndPortDBResult(const PGconn*, const std::string&, const std::string&);

PGresult* getMessagesDBResult(const PGconn*, int);

PGresult* pushMessageToDBAndReturn(
    const PGconn*, const std::string&, const std::string&, const std::string&, const std::string&);

void logUnknownField(const PGresult*, int, int);

bool getDataFromDBResult(DBReplyData&, const PGresult*, int);

#endif // DB_INTERACTION_HPP
