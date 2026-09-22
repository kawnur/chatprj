#include "logging.hpp"

#include "constants.hpp"

void logDBResultUnknownField(std::shared_ptr<PGresult> result, int row, int column)
{
    auto value = PQgetvalue(result.get(), row, column);
    auto logMark = (value) ? std::string(value) : "nullptr";

    logArgsError("unknown field name:", logMark);
}

std::string buildLogEntryPrefix(LogType type)
{
    auto time = getString(QTime().currentTime().toString());

    std::string defaultValue = "UNKNOWN"s;
    auto mark = getMapValue(LOG_TYPE_STRING_REPRESENTATION, type, defaultValue);

    return getStringByFormat("[{0} {1}]", time, mark);
}
