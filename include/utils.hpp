#ifndef UTILS_HPP
#define UTILS_HPP

#include <format>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#include <nlohmann/json.hpp>

#include <QString>
#include <QWidget>

#include "constants.hpp"

class ButtonInfo;
class Companion;
class CompanionAction;
class DBReplyData;
class GraphicManager;
class Message;
class MessageMetaData;
class TextDialog;

std::shared_ptr<GraphicManager> getGraphicManager();

template<typename T, typename... Ts>
std::string getStringByFormat(T &&formatString, Ts &&...args)
{
    return std::vformat(formatString, std::make_format_args(args...));
}

template<typename T>
QString getQString(T &&value);

template<typename... Ts>
void logArgsError(Ts &&...args);

template<typename... Ts>
void logArgsException(Ts &&...args);

template<typename T, typename... Ts>
void logTemplateError(T &&templateString, Ts &&...args);

template<typename T, typename U>
U getConstantMappingValue(std::string mapName, const std::map<T, U> *map, const T &key)
{
    try {
        return map->at(key);
    }
    catch(std::out_of_range) {
        logTemplateError("mapping {} key error", mapName);
    }
    catch(const std::exception &e) {
        logArgsException(e.what());
    }

    return U();
}

template<typename T, typename U>
U getMappingValueOrDefault(const std::map<T, U> &map, const T &key, const U &defaultValue)
{
    try {
        return map.at(key);
    }
    catch(std::out_of_range) {
        return defaultValue;
    }
    catch(const std::exception &e) {
        logArgsException(e.what());
    }

    return U();
}

template<typename F, typename... Ts>
void runInDetachedThread(F &&func, Ts&&... args)
{
    std::thread(func).detach();
}

template<typename F, typename... Ts>
void runAndCoutException(F &&func, Ts&&... args)
{
    try {
        func(args...);
    }
    catch(const std::exception &e) {
        std::cout << e.what() << std::endl;
    }
}

template<typename F, typename... Ts>
void runAndLogException(F &&func, Ts&&... args)
{
    try {
        func(args...);
    }
    catch(const std::exception &e) {
        logArgsException(e.what());
    }
}

template<typename F, typename... Ts>
bool runAndReturnBool(F &&func, Ts&&... args)
{
    try {
        func(args...);

        return true;
    }
    catch(const std::exception &e) {
        logArgsException(e.what());

        return false;
    }
}

template<typename T, typename F, typename... Ts>
std::optional<T> runAndReturnOptionalResult(F &&func, Ts&&... args)
{
    try {
        return func(args...);
    }
    catch(const std::exception &e) {
        logArgsException(e.what());

        return std::nullopt;
    }
}

// TODO get rid of T template parameter
template<typename T, typename F, typename... Ts>
std::shared_ptr<T> runAndReturnSharedPtr(F &&func, Ts&&... args)
{
    try {
        return func(args...);
    }
    catch(const std::exception &e) {
        logArgsException(e.what());

        return nullptr;
    }
}

template<typename F, typename FH, typename... Ts>
void runAndHandleException(F &&func, FH &&handler, Ts&&... args)
{
    try {
        func(args...);
    }
    catch(const std::exception &e) {
        logArgsException(e.what());
        handler(e);
    }
}

template<typename... Ts>
std::vector<std::string> buildStringVector(const Ts &...args)
{
    std::vector<std::string> result;

    (result.emplace_back(args), ...);

    return result;
}

// data validation

bool validateCompanionName(std::vector<std::string> &errors, const std::string &name);
bool validateIpAddress(std::vector<std::string> &errors, const std::string &ipAddress);
bool validatePort(std::vector<std::string> &errors, const std::string &port);

bool validateCompanionData(
    std::vector<std::string> &errors, std::shared_ptr<CompanionAction> action);

bool validatePassword(std::vector<std::string> &errors, const std::string &password);
std::string buildDialogText(std::string &&header, const std::vector<std::string> &messages);

// std::shared_ptr<std::vector<ButtonInfo>> createOkButtonInfoVector(void (TextDialog:: *)());
std::shared_ptr<std::vector<ButtonInfo>> createOkButtonInfoVector(std::function<void(TextDialog &)> function);
// std::vector<ButtonInfo> *createOkButtonInfoVector(void (QDialog:: *)());

void showInfoDialogAndLogInfo(
    const QString &message, void (TextDialog::*function)(),
    std::shared_ptr<QWidget> parent = nullptr);

void showInfoDialogAndLogInfo(QString &&message, std::shared_ptr<QWidget> parent = nullptr);
void showWarningDialogAndLogWarning(const QString &message, std::shared_ptr<QWidget> parent = nullptr);

// void showErrorDialogAndLogError(const QString&, std::shared_ptr<QWidget> = nullptr);
// void showErrorDialogAndLogError(QString &&message, std::shared_ptr<QWidget> parent);

void showErrorDialogAndLogError(QString &&message);
void showErrorDialogAndLogError(QString &&message, std::shared_ptr<QWidget> parent);

// template<typename T>
// void showErrorDialogAndLogError(std::shared_ptr<QWidget> parent, T &&message) {
//     getGraphicManager()->createTextDialogAndShow(
//         parent, DialogType::ERROR,
//         getQString(std::forward<T>(message)).toStdString(),
//         // createOkButtonInfoVector(&QDialog::accept));
//         createOkButtonInfoVector(&TextDialog::accept));

//     logArgsError(message);
// }

std::string getFormattedMessageBodyString(const std::string &color, const std::string &text);

std::pair<std::string, std::string> formatMessageHeaderAndBody(
    std::shared_ptr<Companion> companion, std::shared_ptr<Message> message);

std::string buildMessageJSONString(
    // bool isAntecedent, NetworkMessageType type, std::shared_ptr<Companion> companion,
    // const std::string &networkId, std::shared_ptr<Message> message);
    std::shared_ptr<Companion> companion, std::shared_ptr<Message> message,
    std::shared_ptr<MessageMetaData> meta);

std::string buildFileBlockJSONString(
    std::shared_ptr<Companion> companion, const std::string &networkId, const std::string &data);

std::string buildChatHistoryJSONString(
    std::shared_ptr<DBReplyData> data, std::vector<std::string> &keys);

nlohmann::json buildJsonObject(const std::string &jsonString);
std::string getRandomString(uint8_t length);
void sleepForMS(uint32_t duration);
bool getBoolFromDBValue(const std::string &value);
std::string hashFileMD5(const std::string &filename);

#endif // UTILS_HPP
