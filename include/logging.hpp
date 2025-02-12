#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <QString>
#include <QTime>
#include <string>
#include <type_traits>

#include "constants.hpp"
#include "graphic_manager.hpp"
#include "mainwindow.hpp"
#include "manager.hpp"
#include "utils_cout.hpp"

class DBReplyData;
class MainWindow;
class GraphicManager;
class SocketInfo;
class SocketInfoWidget;

GraphicManager* getGraphicManagerPtr();

template<typename T> QString getQString(T* value)
{
    std::stringstream ss;
    ss << (void*)value;
    return QString::fromStdString(ss.str());
}

template<
    typename T,
    std::enable_if_t<std::is_arithmetic_v<std::remove_const_t<std::remove_reference_t<T>>>, bool> = true>
QString getQString(T&& value)
{    
    return QString::fromStdString(std::to_string(std::forward<T>(value)));
}

template<
    typename T,
    std::enable_if_t<!std::is_arithmetic_v<std::remove_const_t<std::remove_reference_t<T>>>, bool> = true>
QString getQString(T&& value)
{
    return QString::fromStdString(std::forward<T>(value));
}

QString getQString(const std::string&);
QString getQString(const char*);
QString getQString(const bool&);
QString getQString(std::nullptr_t);
QString getQString(QString);
QString getQString(std::filesystem::path&);

template<typename... Ts> QString getArgumentedQString(
    const QString& templateString, Ts&&... args)
{    
    return templateString.arg(getQString(std::forward<Ts>(args))...);
}

template<typename... Ts> void logArgs(Ts&&... args)
{
    QTime time;
    QString text("- ");

    text += time.currentTime().toString() + QString(" - ");

    ((text += (getQString(args) + QString(" "))), ...);

    getGraphicManagerPtr()->addTextToAppLogWidget(text);
    coutArgsWithSpaceSeparator(text);
}

template<typename... Ts> void logArgsWithTemplate(
    const QString& templateString, Ts&&... args)
{
    logArgs(templateString.arg(getQString(args)...));
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

template<typename... Ts> void logArgsInfoWithTemplate(
    const QString& templateString, Ts&&... args)
{
    logArgsInfo(templateString.arg(getQString(args)...));
}

template<typename... Ts> void logArgsWarningWithTemplate(
    const QString& templateString, Ts&&... args)
{
    logArgsWarning(templateString.arg(getQString(args)...));
}

template<typename... Ts> void logArgsErrorWithTemplate(
    const QString& templateString, Ts&&... args)
{
    logArgsError(templateString.arg(getQString(args...)));
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

void logSocketInfoData(const SocketInfo*);
void logDBResultUnknownField(const PGresult*, int, int);
void logDBReplyData(std::shared_ptr<DBReplyData>&);
void logSocketInfoWidget(const SocketInfoWidget*);

#endif // LOGGING_HPP
