#include "logging.hpp"

QString getQString(const std::string& value) {
    return QString::fromStdString(value);
}

QString getQString(std::shared_ptr<char> value) {
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
    getGraphicManager()->addTextToAppLogWidget(string);
}

void logLine(const std::string& string) {
    getGraphicManager()->addTextToAppLogWidget(getQString(string));
}

void logSocketInfoData(std::shared_ptr<SocketInfo> object) {
    logArgsWithTemplate(
        "ipAddress: {0}, serverPort_: {1}, clientPort_: {2}",
        object->getIpAddress(), object->getServerPort(),
        object->getClientPort());
}

void logDBResultUnknownField(std::shared_ptr<PGresult> result, int row, int column) {
    std::shared_ptr<char> value = PQgetvalue(result, row, column);
    auto logMark = (value) ? std::string(value) : "nullptr";

    logArgsError("unknown field name:", logMark);
}

void logDBReplyData(std::shared_ptr<DBReplyData> object) {
    logArgs(logDelimiter);

    for(auto& element : object->buildDataStringVector()) {
        logArgs(element);
    }

    logArgs(logDelimiter);
}

void logSocketInfoWidget(std::shared_ptr<SocketInfoWidget> object) {
    logArgsWithTemplate(
        "name: {0}, ipAddress: {1}, serverPort_: {2}, clientPort_: {3}",
        object->getName().toStdString(), object->getIpAddress().toStdString(),
        object->getServerPort(), object->getClientPort());
}
