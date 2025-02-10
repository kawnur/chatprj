#include "logging.hpp"

QString argForLogging(const std::string& value)
{
    return QString::fromStdString(value);
}

QString argForLogging(const char* value)
{
    return QString(value);
}

QString argForLogging(const bool& value)
{
    return (value) ? QString("true") : QString("false");
}

QString argForLogging(std::nullptr_t value)
{
    return QString("nullptr_t");
}

QString argForLogging(QString value)
{
    return value;
}

void logLine(const QString& string)
{
    getGraphicManagerPtr()->addTextToAppLogWidget(string);
}

void logLine(const std::string& string)
{
    getGraphicManagerPtr()->addTextToAppLogWidget(getQString(string));
}

void logSocketInfoData(const SocketInfo* objectPtr)
{
    logArgs(
        "ipAddress:", objectPtr->getIpAddress(),
        "serverPort_:", objectPtr->getServerPort(),
        "clientPort_:", objectPtr->getClientPort());
}

void logDBResultUnknownField(const PGresult* result, int row, int column)
{
    char* value = PQgetvalue(result, row, column);
    auto logMark = (value) ? std::string(value) : "nullptr";

    logArgsError("unknown field name:", logMark);
}

void logDBReplyData(std::shared_ptr<DBReplyData>& objectPtr)
{
    logArgs(logDelimiter);

    for(auto& element : objectPtr->buildDataQStringVector())
    {
        logArgs(element);
    }

    logArgs(logDelimiter);
}

void logSocketInfoWidget(const SocketInfoWidget* objectPtr)
{
    logArgs(
        "name:", objectPtr->getName(),
        "ipAddress:", objectPtr->getIpAddress(),
        "serverPort_:", objectPtr->getServerPort(),
        "clientPort_:", objectPtr->getClientPort());
}

