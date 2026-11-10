#include "logging.hpp"

#include "constants.hpp"

void logDBResultUnknownField(PGresultPtr result, int row, int column)
{
    auto value = PQgetvalue(result.get(), row, column);
    auto logMark = (value) ? std::string(value) : "nullptr";

    logTemplateError("unknown field name: {}", logMark);
}

std::string buildLogEntryPrefix(const std::string &logType)
{
    auto time = getString(QTime().currentTime().toString());

    return getStringByFormat("[{0} {1}]", time, logType);
}
