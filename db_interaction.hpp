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

const char* getValueFromEnvironmentVariable(const char*);
PGconn* getDBConnection();
PGresult* sendDBRequestAndReturnResult(const PGconn*, const char*);
PGresult* getCompanionsDBResult(const PGconn*);
PGresult* getSocketInfoDBResult(const PGconn*, int);
PGresult* getMessagesDBResult(const PGconn*, int);
PGresult* pushMessageToDBAndReturn(const PGconn*, const std::string&, const std::string&);

void logUnknownField(const PGresult*, int, int);

bool getDataFromDBResult(
    std::vector<std::map<std::string, const char*>>&, const PGresult*, int);

#endif // DB_INTERACTION_HPP
