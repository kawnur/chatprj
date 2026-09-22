#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <memory>
#include <string>

#include <libpq-fe.h>

#include <QString>
#include <QTime>

#include "graphic_manager.hpp"
#include "utils.hpp"
#include "utils_cout.hpp"

class DBReplyData;
class GraphicManager;
class SocketInfo;
class SocketInfoWidget;

std::shared_ptr<GraphicManager> getGraphicManager();

template<typename... Ts>
void coutArgsWithSpaceSeparator(Ts&&... args);

template<typename... Ts>
void logArgs(Ts &&...args)
{
    std::string text { "" };

    ((text += (getStringByFormat("{} ", args))), ...);

    getGraphicManager()->addTextToAppLogWidget(getQString(text));
    coutArgsWithSpaceSeparator(text);
}

template<typename T, typename... Ts>
void logArgsWithTemplate(T &&templateString, Ts &&...args)
{
    logArgs(getStringByFormat(templateString, args...));
}

std::string buildLogEntryPrefix(LogType type);

template<typename... Ts>
void logTypeArgs(LogType type, Ts &&...args)
{
    logArgs(buildLogEntryPrefix(type), args...);
}

template<typename... Ts>
void logArgsInfo(Ts &&...args)
{
    logArgs(buildLogEntryPrefix(LogType::INFO), args...);
}

template<typename... Ts>
void logArgsDebug(Ts &&...args)
{
    logArgs(buildLogEntryPrefix(LogType::DEBUG), args...);
}

template<typename... Ts>
void logArgsException(Ts &&...args)
{
    logArgs(buildLogEntryPrefix(LogType::EXCEPTION), args...);
}

template<typename... Ts>
void logArgsWarning(Ts &&...args)
{
    logArgs(buildLogEntryPrefix(LogType::WARNING), args...);
}

template<typename... Ts>
void logArgsError(Ts &&...args)
{
    logArgs(buildLogEntryPrefix(LogType::ERROR), args...);
}

template<typename T, typename... Ts>
void logTemplateInfo(T &&templateString, Ts &&...args)
{
    logArgsInfo(getStringByFormat(templateString, args...));
}

template<typename T, typename... Ts>
void logTemplateDebug(T &&templateString, Ts &&...args)
{
    logArgsDebug(getStringByFormat(templateString, args...));
}

template<typename T, typename... Ts>
void logTemplateWarning(T &&templateString, Ts &&...args)
{
    logArgsWarning(getStringByFormat(templateString, args...));
}

template<typename T, typename... Ts>
void logTemplateError(T &&templateString, Ts &&...args)
{
    logArgsError(getStringByFormat(templateString, args...));
}

template<typename... Ts>
void logArgsWithCustomMark(Ts &&...args)
{
    logArgs(logCustomDelimiter, args...);
}

void logDBResultUnknownField(std::shared_ptr<PGresult> result, int row, int column);

#endif // LOGGING_HPP
