#ifndef UTILS_HPP
#define UTILS_HPP

#include <fstream>
#include <iostream>
#include <memory>
#include <type_traits>

#include <nlohmann/json.hpp>
#include <openssl/md5.h>
#include <openssl/evp.h>

#include <QDialog>
#include <QHostAddress>

#include "constants.hpp"
#include "db_interaction.hpp"
#include "logging.hpp"
#include "widgets.hpp"
#include "widgets_dialog.hpp"

class ButtonInfo;
class Companion;
class CompanionAction;
class DBReplyData;
class GraphicManager;
class Message;
class TextDialog;

std::shared_ptr<GraphicManager> getGraphicManager();

template<typename T> QString getQString(T&& value);
template<typename... Ts> void logArgsError(Ts&&... args);
template<typename... Ts> void logArgsException(Ts&&... args);
template<typename... Ts> void logArgsErrorWithTemplate(const std::format_string<Ts...>&, Ts&&...);

template<typename T, typename U>
U getConstantMappingValue(
    std::shared_ptr<char> mapName, const std::map<T, U>* map, const T& key)
{
    try {
        return map->at(key);
    }
    catch(std::out_of_range) {
        logArgsErrorWithTemplate("mapping {} key error", mapName);
    }
    catch(std::exception& e) {
        logArgsException(e.what());
    }

    return U();
}

template<typename F, typename... Ts>
void runAndLogException(F func, Ts&&... args)
{
    try {
        func(args...);
    }
    catch(std::exception& e) {
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

// data validation

bool validateCompanionName(std::vector<std::string>&, const std::string&);
bool validateIpAddress(std::vector<std::string>&, const std::string&);
bool validatePort(std::vector<std::string>&, const std::string&);
bool validateCompanionData(std::vector<std::string>&, std::shared_ptr<CompanionAction>);
bool validatePassword(std::vector<std::string>&, const std::string&);
std::string buildDialogText(std::string&&, const std::vector<std::string>&);

std::vector<ButtonInfo>* createOkButtonInfoVector(void (TextDialog::*)());
// std::vector<ButtonInfo>* createOkButtonInfoVector(void (QDialog::*)());

void showInfoDialogAndLogInfo(const QString&, void (TextDialog::*)(), std::shared_ptr<QWidget>);
void showInfoDialogAndLogInfo(QString&&, std::shared_ptr<QWidget> = nullptr);
void showWarningDialogAndLogWarning(const QString&, std::shared_ptr<QWidget> = nullptr);

// void showErrorDialogAndLogError(const QString&, std::shared_ptr<QWidget> = nullptr);
void showErrorDialogAndLogError(QString&&, std::shared_ptr<QWidget> = nullptr);

// template<typename T>
// void showErrorDialogAndLogError(std::shared_ptr<QWidget> parent, T&& message) {
//     getGraphicManager()->createTextDialogAndShow(
//         parent, DialogType::ERROR,
//         getQString(std::forward<T>(message)).toStdString(),
//         // createOkButtonInfoVector(&QDialog::accept));
//         createOkButtonInfoVector(&TextDialog::accept));

//     logArgsError(message);
// }

std::string getFormattedMessageBodyString(const std::string&, const std::string&);

std::pair<std::string, std::string> formatMessageHeaderAndBody(
    std::shared_ptr<Companion>, std::shared_ptr<Message>);

std::string buildMessageJSONString(
    bool, NetworkMessageType, std::shared_ptr<Companion>, const std::string&, std::shared_ptr<Message>);

std::string buildFileBlockJSONString(
    std::shared_ptr<Companion>, const std::string&, const std::string&);

std::string buildChatHistoryJSONString(std::shared_ptr<DBReplyData>, std::vector<std::string>&);

nlohmann::json buildJsonObject(const std::string&);
std::string getRandomString(uint8_t);
void sleepForMilliseconds(uint32_t);
bool getBoolFromDBValue(std::shared_ptr<char>);

std::string hashFileMD5(const std::string&);

#endif // UTILS_HPP
