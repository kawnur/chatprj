#ifndef UTILS_HPP
#define UTILS_HPP

#include <nlohmann/json.hpp>
#include <QHostAddress>

#include "constants.hpp"
#include "db_interaction.hpp"
#include "logging.hpp"

class ButtonInfo;
class Companion;
class CompanionAction;
class DBReplyData;
class Message;
class TextDialog;

template<typename... Ts> void logArgsException(Ts&&... args);

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

template<typename T> QString getQString(T&& string)
{
    return QString::fromStdString(std::forward<T>(string));
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

std::pair<QString, QString> formatMessageHeaderAndBody(
    const Companion*, const Message*);

std::string buildMessageJSONString(
    bool, NetworkMessageType, const Companion*, const std::string&, const Message*);

std::string buildChatHistoryJSONString(
    std::shared_ptr<DBReplyData>&, std::vector<std::string>&);

nlohmann::json buildJsonObject(const std::string&);
std::string getRandomString(uint8_t);
void sleepForMilliseconds(uint32_t);
// std::string generateMessageKey(std::string&, uint8_t);
QString getBoolQString(bool);
bool getBoolFromDBValue(const char*);

#endif // UTILS_HPP
