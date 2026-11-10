#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <string>

#include <libpq-fe.h>

#include <QString>
#include <QTime>

#include "alias.hpp"
#include "graphic_manager.hpp"
#include "utils.hpp"
#include "utils_cout.hpp"

GraphicManagerPtr getGraphicManager();

template<typename... Ts>
void coutArgsWithSpaceSeparator(Ts&&... args);

template<typename... Ts>
void logArgs(Ts &&...args)
{
    std::string text { "" };

    ((text += (getStringByFormat("{} ", args))), ...);

    auto graphicManager = getGraphicManager();

    if (graphicManager)
        graphicManager->addLog(getQString(text));

    coutArgsWithSpaceSeparator(text);
}

template<typename T, typename... Ts>
void logArgsWithTemplate(T &&templateString, Ts &&...args)
{
    logArgs(getStringByFormat(templateString, args...));
}

std::string buildLogEntryPrefix(const std::string &logType);

template<typename... Ts>
void logTypeArgs(const std::string &prefix, Ts &&...args)
{
    logArgs(prefix, args...);
}

template<typename... Ts>
void logArgsInfo(Ts &&...args)
{
    logArgs(buildLogEntryPrefix("INFO"), args...);
}

template<typename... Ts>
void logArgsDebug(Ts &&...args)
{
    logArgs(buildLogEntryPrefix("DEBUG"), args...);
}

template<typename... Ts>
void logArgsException(Ts &&...args)
{
    logArgs(buildLogEntryPrefix("EXCEPTION"), args...);
}

template<typename... Ts>
void logArgsWarning(Ts &&...args)
{
    logArgs(buildLogEntryPrefix("WARNING"), args...);
}

template<typename... Ts>
void logArgsError(Ts &&...args)
{
    logArgs(buildLogEntryPrefix("ERROR"), args...);
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

template<typename T, typename... Ts>
void logTemplateException(T &&templateString, Ts &&...args)
{
    logArgsException(getStringByFormat(templateString, args...));
}

template<typename... Ts>
void logArgsWithCustomMark(Ts &&...args)
{
    logArgs(LOG_CUSTOM_DELIMITER, args...);
}

void logDBResultUnknownField(PGresultPtr result, int row, int column);

#endif // LOGGING_HPP
