#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <format>
#include <memory>
#include <string>
#include <type_traits>

#include <QString>
#include <QTime>

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

std::shared_ptr<GraphicManager> getGraphicManager();

template<typename T>
concept IsArithmetic =
    std::is_arithmetic_v<std::remove_const_t<std::remove_reference_t<T>>>;

template<typename T>
concept IsNotArithmetic =
    !std::is_arithmetic_v<std::remove_const_t<std::remove_reference_t<T>>>;

template<typename T>
QString getQString(std::shared_ptr<T> value) {
    std::stringstream ss;
    ss << (std::shared_ptr<void>)value;
    return QString::fromStdString(ss.str());
}

template<IsArithmetic T>
QString getQString(T&& value) {
    return QString::fromStdString(std::to_string(std::forward<T>(value)));
}

template<IsNotArithmetic T>
QString getQString(T&& value) {
    return QString::fromStdString(std::forward<T>(value));
}

QString getQString(const std::string&);
QString getQString(std::shared_ptr<char>);
QString getQString(const bool&);
QString getQString(std::nullptr_t);
QString getQString(QString);
QString getQString(std::filesystem::path&);

template<typename... Ts>
QString getArgumentedQString(const QString& templateString, Ts&&... args)
{
    return templateString.arg(getQString(std::forward<Ts>(args))...);
}

template<typename... Ts>
void logArgs(Ts&&... args)
{
    QTime time;
    QString text("- ");

    text += time.currentTime().toString() + QString(" - ");

    ((text += (getQString(args) + QString(" "))), ...);

    getGraphicManager()->addTextToAppLogWidget(text);
    coutArgsWithSpaceSeparator(text);
}

template<typename... Ts>
void logArgsWithTemplate(const std::format_string<Ts...>& templateString, Ts&&... args)
{
    logArgs(std::format(templateString, std::forward<Ts>(args)...));
}

template<typename... Ts>
void logArgsInfo(Ts&&... args)
{
    logArgs("INFO:", args...);
}

template<typename... Ts>
void logArgsException(Ts&&... args)
{
    logArgs("EXCEPTION:", args...);
}

template<typename... Ts>
void logArgsWarning(Ts&&... args)
{
    logArgs("WARNING:", args...);
}

template<typename... Ts>
void logArgsError(Ts&&... args)
{
    logArgs("ERROR:", args...);
}

template<typename... Ts>
void logTemplateInfo(const std::format_string<Ts...>& templateString, Ts&&... args)
{
    logArgsInfo(std::format(templateString, std::forward<Ts>(args)...));
}

template<typename... Ts>
void logTemplateWarning(const std::format_string<Ts...>& templateString, Ts&&... args)
{
    logArgsWarning(std::format(templateString, std::forward<Ts>(args)...));
}

template<typename... Ts>
void logTemplateError(const std::format_string<Ts...>& templateString, Ts&&... args)
{
    logArgsError(std::format(templateString, std::forward<Ts>(args)...));
}

template<typename T>
void logLine(const T& string)
{
    getGraphicManager()->addTextToAppLogWidget(QString(string));
}

void logLine(const QString&);
void logLine(const std::string&string);

template<typename... Ts>
void logArgsWithCustomMark(Ts&&... args)
{
    logArgs(logCustomDelimiter, args...);
}

void logSocketInfoData(std::shared_ptr<SocketInfo>);
void logDBResultUnknownField(std::shared_ptr<PGresult>, int, int);
void logDBReplyData(std::shared_ptr<DBReplyData>);
void logSocketInfoWidget(std::shared_ptr<SocketInfoWidget>);

#endif // LOGGING_HPP
