#include "logging.hpp"

#include "constants.hpp"

void logDBResultUnknownField(PGresultPtr result, int row, int column)
{
    auto value = PQgetvalue(result.get(), row, column);
    auto logMark = (value) ? std::string(value) : "nullptr";

    logTemplateError("unknown field name: {}", logMark);
}

std::string buildLogEntryPrefix(LogType type)
{
    auto time = getString(QTime().currentTime().toString());

    std::string defaultValue = "UNKNOWN"s;
    auto mark = getMapValue(LOG_TYPE_STRING_REPR, type, defaultValue);

    return getStringByFormat("[{0} {1}]", time, mark);
}
