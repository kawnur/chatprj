#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <QString>
#include <string>
#include <type_traits>

#include "constants.hpp"
#include "mainwindow.hpp"
#include "manager.hpp"
#include "utils_cout.hpp"

class MainWindow;
class GraphicManager;
// MainWindow* getMainWindowPtr();
GraphicManager* getGraphicManagerPtr();

const bool logDBInteraction = false;

template<typename T> QString argForLogging(T* const& value)
{
    std::stringstream ss;
    ss << (void*)value;
    return QString::fromStdString(ss.str());
}

template<typename T, typename std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
QString argForLogging(const T& value)
{
    return QString::fromStdString(std::to_string(value));
}

template<typename T, typename std::enable_if_t<!std::is_arithmetic_v<T>, bool> = true>
QString argForLogging(const T& value)
{
    return QString(value);
}

QString argForLogging(const std::string&);
QString argForLogging(const char*);
QString argForLogging(const bool&);

template<typename... Ts> void logArgs(Ts&&... args)
{
    // MainWindow* mainWindowPtr = getMainWindowPtr();
    QString text;

    text += QString("- ");

    ((text += (argForLogging(args) + QString(" "))), ...);

    getGraphicManagerPtr()->addTextToAppLogWidget(text);
    coutArgsWithSpaceSeparator(args...);
}

template<typename... Ts> void logArgsInfo(Ts&&... args)
{
    logArgs("INFO:", args...);
}

template<typename... Ts> void logArgsException(Ts&&... args)
{
    logArgs("EXCEPTION:", args...);
}

template<typename... Ts> void logArgsWarning(Ts&&... args)
{
    logArgs("WARNING:", args...);
}

template<typename... Ts> void logArgsError(Ts&&... args)
{
    logArgs("ERROR:", args...);
}

template<typename T> void logLine(const T& string)
{
    getGraphicManagerPtr()->addTextToAppLogWidget(QString(string));
}

void logLine(const QString&);
void logLine(const std::string&string);

template<typename... Ts> void logArgsWithCustomMark(Ts&&... args)
{
    logArgs(logCustomDelimiter, args...);
}

#endif // LOGGING_HPP
