#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <cstddef>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>

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

template<typename T>
concept IsArithmetic = std::is_arithmetic_v<std::remove_const_t<std::remove_reference_t<T>>>;

template<typename T>
concept IsNotArithmetic = !std::is_arithmetic_v<std::remove_const_t<std::remove_reference_t<T>>>;

QString getQString(const std::string &value);
QString getQString(const char *value);
QString getQString(const bool &value);
QString getQString(std::nullptr_t value);
QString getQString(const std::filesystem::path &value);

template<typename T>
QString getQString(std::shared_ptr<T> value)
{
    std::stringstream ss;
    ss << (std::shared_ptr<void>)value;
    return QString::fromStdString(ss.str());
}

template<IsArithmetic T>
QString getQString(T &&value)
{
    return QString::fromStdString(std::to_string(std::forward<T>(value)));
}

template<IsNotArithmetic T>
QString getQString(T &&value)
{
    if constexpr (std::is_same_v<std::remove_cvref_t<T>, QString>)
        return std::forward<T>(value);
    else
        return QString::fromStdString(std::forward<T>(value));
}

template<typename T>
QString getQString(const std::optional<T> &value)
{
    return (value) ? getQString(value.value()) : "EMPTY OPTIONAL";
}

template<typename... Ts>
QString getArgumentedQString(const QString &templateString, Ts&&... args)
{
    return templateString.arg(getQString(std::forward<Ts>(args))...);
}

template<typename... Ts>
void logArgs(Ts &&...args)
{
    QTime time;
    QString text("- ");

    text += time.currentTime().toString() + QString(" - ");

    ((text += (getQString(args) + QString(" "))), ...);

    getGraphicManager()->addTextToAppLogWidget(text);
    coutArgsWithSpaceSeparator(text);
}

template<typename T, typename... Ts>
void logArgsWithTemplate(T &&templateString, Ts &&...args)
{
    logArgs(getStringByFormat(templateString, args...));
}

template<typename... Ts>
void logArgsInfo(Ts &&...args)
{
    logArgs("INFO:", args...);
}

template<typename... Ts>
void logArgsException(Ts &&...args)
{
    logArgs("EXCEPTION:", args...);
}

template<typename... Ts>
void logArgsWarning(Ts &&...args)
{
    logArgs("WARNING:", args...);
}

template<typename... Ts>
void logArgsError(Ts &&...args)
{
    logArgs("ERROR:", args...);
}

template<typename T, typename... Ts>
void logTemplateInfo(T &&templateString, Ts &&...args)
{
    logArgsInfo(getStringByFormat(templateString, args...));
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

template<typename T>
void logLine(const T &string)
{
    getGraphicManager()->addTextToAppLogWidget(QString(string));
}

void logLine(const QString &string);
void logLine(const std::string &string);

template<typename... Ts>
void logArgsWithCustomMark(Ts &&...args)
{
    logArgs(logCustomDelimiter, args...);
}

void logSocketInfoData(std::shared_ptr<SocketInfo> object);
void logDBResultUnknownField(std::shared_ptr<PGresult> result, int row, int column);
void logDBReplyData(std::shared_ptr<DBReplyData> object);
void logSocketInfoWidget(std::shared_ptr<SocketInfoWidget> object);

#endif // LOGGING_HPP
