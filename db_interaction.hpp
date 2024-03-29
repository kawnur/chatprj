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

PGconn* getDBConnection();
PGresult* getCompanionsDBResult(PGconn*);
PGresult* getSocketInfoDBResult(PGconn*, int);
PGresult* getMessagesDBResult(PGconn*, int);
PGresult* pushMessageToDBAndReturn(PGconn*, const QString&, const QString&);

void logUnknownField(const PGresult*, int, int);

void getCompanionsDataFromDBResult(
        std::vector<std::pair<int, QString>>*, const PGresult*);

std::pair<QString, QString> getSocketInfoDataFromDBResult(const PGresult*);
void getMessagesDataFromDBResultAndAdd(Companion*, const PGresult*);
std::pair<int, QString> getPushedMessageInfo(const PGresult*);

void testQString();

#endif // DB_INTERACTION_HPP
