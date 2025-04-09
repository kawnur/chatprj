#include "logging.hpp"

QString getQString(const std::string& value) {
    return QString::fromStdString(value);
}

QString getQString(const char* value) {
    return QString(value);
}

QString getQString(const bool& value) {
    return (value) ? "true" : "false";
}

QString getQString(std::nullptr_t value) {
    return "nullptr_t";
}

QString getQString(QString value) {
    return value;
}

QString getQString(std::filesystem::path& value) {
    return QString::fromStdString(value.string());
}

void logLine(const QString& string) {
    getGraphicManagerPtr()->addTextToAppLogWidget(string);
}

void logLine(const std::string& string) {
    getGraphicManagerPtr()->addTextToAppLogWidget(getQString(string));
}

void logSocketInfoData(const SocketInfo* objectPtr) {
    logArgsWithTemplate(
        "ipAddress: {0}, serverPort_: {1}, clientPort_: {2}",
        objectPtr->getIpAddress(), objectPtr->getServerPort(),
        objectPtr->getClientPort());
}

void logDBResultUnknownField(const PGresult* result, int row, int column) {
    char* value = PQgetvalue(result, row, column);
    auto logMark = (value) ? std::string(value) : "nullptr";

    logArgsError("unknown field name:", logMark);
}

void logDBReplyData(std::shared_ptr<DBReplyData>& objectPtr) {
    logArgs(logDelimiter);

    for(auto& element : objectPtr->buildDataStringVector()) {
        logArgs(element);
    }

    logArgs(logDelimiter);
}

void logSocketInfoWidget(const SocketInfoWidget* objectPtr) {
    logArgsWithTemplate(
        "name: {0}, ipAddress: {1}, serverPort_: {2}, clientPort_: {3}",
        objectPtr->getName().toStdString(), objectPtr->getIpAddress().toStdString(),
        objectPtr->getServerPort(), objectPtr->getClientPort());
}
