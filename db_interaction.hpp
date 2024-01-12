#ifndef DB_INTERACTION_HPP
#define DB_INTERACTION_HPP

#include <libpq-events.h>
#include <libpq-fe.h>
#include <string>

#include "mainwindow.hpp"
#include "utils_cout.hpp"


PGconn* getDBConnection();
PGresult* getsSocketsInfo(PGconn*);

#endif // DB_INTERACTION_HPP
