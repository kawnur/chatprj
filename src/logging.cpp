#include "logging.hpp"

template<> QString argForLogging<const std::string&>(const std::string& value)
{
    return QString::fromStdString(value);
}

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

void logLine(const QString& string)
{
    getGraphicManagerPtr()->addTextToAppLogWidget(string);
}

void logLine(const std::string& string)
{
    getGraphicManagerPtr()->addTextToAppLogWidget(QString::fromStdString(string));
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
    logArgs("############################");

    for(auto& elem : *objectPtr->getDataPtr())
    {
        for(auto& item : elem)
        {
            logArgs(item.first, item.second);
        }
    }

    logArgs("############################");
}

void logSocketInfoWidget(const SocketInfoWidget* objectPtr)
{
    logArgs(
        "name:", objectPtr->getName(),
        "ipAddress:", objectPtr->getIpAddress(),
        "serverPort_:", objectPtr->getServerPort(),
        "clientPort_:", objectPtr->getClientPort());
}

