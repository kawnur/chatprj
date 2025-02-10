#ifndef UTILS_HPP
#define UTILS_HPP

#include <fstream>
#include <iostream>
#include <QHostAddress>
#include <nlohmann/json.hpp>
#include <openssl/md5.h>
#include <openssl/evp.h>
#include <type_traits>

#include "constants.hpp"
#include "db_interaction.hpp"
#include "logging.hpp"

class ButtonInfo;
class Companion;
class CompanionAction;
class DBReplyData;
class Message;
class TextDialog;

template<typename... Ts> void logArgsError(Ts&&... args);
template<typename... Ts> void logArgsException(Ts&&... args);

template<typename... Ts> void logArgsErrorByArgumentedTemplate(
    const QString& templateString, Ts&&... args);

template<typename T, typename U>
U getConstantMappingValue(
    const char* mapName, const std::map<T, U>* mapPtr, const T& key)
{
    try
    {
        return mapPtr->at(key);
    }
    catch(std::out_of_range)
    {
        logArgsErrorByArgumentedTemplate("mapping %1 key error", mapName);
    }
    catch(std::exception& e)
    {
        logArgsException(e.what());
    }

    return U("");
}

template<typename F, typename... Ts>
void runAndLogException(F func, Ts&&... args)
{
    try
    {
        func(args...);
    }
    catch(std::exception& e)
    {
        logArgsException(e.what());
    }
}

template<typename... Ts>
std::vector<std::string> buildStringVector(Ts... args)
{
    std::vector<std::string> result;

    (result.emplace_back(args), ...);

    return result;
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

QString getQString(const char*);
QString getQString(QString);
QString getQString(bool);

template<typename... Ts> QString getArgumentedQString(
    const QString& templateString, Ts&&... args)
{
    return templateString.arg(getQString(std::forward<Ts>(args))...);
}

// data validation

bool validateCompanionName(std::vector<std::string>&, const std::string&);
bool validateIpAddress(std::vector<std::string>&, const std::string&);
bool validatePort(std::vector<std::string>&, const std::string&);
bool validateCompanionData(std::vector<std::string>&, const CompanionAction*);
bool validatePassword(std::vector<std::string>&, const std::string&);
QString buildDialogText(std::string&&, const std::vector<std::string>&);
std::vector<ButtonInfo>* createOkButtonInfoVector(void (TextDialog::*)());
void showInfoDialogAndLogInfo(QWidget*, const QString&, void (TextDialog::*)());
void showInfoDialogAndLogInfo(QWidget*, QString&&);
void showWarningDialogAndLogWarning(QWidget*, const QString&);
void showErrorDialogAndLogError(QWidget*, const QString&);
void showErrorDialogAndLogError(QWidget*, QString&&);

QString getFormattedMessageBodyQString(const QString&, const QString&);

std::pair<QString, QString> formatMessageHeaderAndBody(
    const Companion*, const Message*);

std::string buildMessageJSONString(
    bool, NetworkMessageType, const Companion*, const std::string&, const Message*);

std::string buildFileBlockJSONString(
    const Companion*, const std::string&, const std::string&);

std::string buildChatHistoryJSONString(
    std::shared_ptr<DBReplyData>&, std::vector<std::string>&);

nlohmann::json buildJsonObject(const std::string&);
std::string getRandomString(uint8_t);
void sleepForMilliseconds(uint32_t);
bool getBoolFromDBValue(const char*);

std::string hashFileMD5(const std::string&);

#endif // UTILS_HPP
