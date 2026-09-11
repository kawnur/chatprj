#ifndef MANAGER_HPP
#define MANAGER_HPP

#include <filesystem>
#include <map>
#include <memory>
#include <mutex>
#include <string>

#include <libpq-fe.h>

#include <QString>
#include <QWidget>

#include "constants.hpp"

class Action;
class Companion;
class CompanionAction;
class DBReplyData;
class Message;
class MessageState;
class PasswordAction;
class SocketInfoBaseWidget;
class WidgetGroup;

int getDataFromDBResult(
    bool log, std::shared_ptr<DBReplyData> data, std::shared_ptr<PGresult> result, int maxTuples);

template<typename... Ts>
void logArgs(Ts &&...args);

void logDBReplyData(std::shared_ptr<DBReplyData> object);
void showInfoDialogAndLogInfo(QString &&message, std::shared_ptr<QWidget> parent);
void showWarningDialogAndLogWarning(const QString &message, std::shared_ptr<QWidget> parent);
void showErrorDialogAndLogError(QString &&message);

class Manager : public QObject // TODO do we need inheritance?
{
public:
    Manager();
    ~Manager();

    std::shared_ptr<Companion> getSelectedCompanion();
    bool userIsAuthenticated();
    void set();

    // std::shared_ptr<Companion> getMappedCompanionBySocketInfoBaseWidget(std::shared_ptr<SocketInfoBaseWidget>) const;
    std::shared_ptr<Companion> getMappedCompanionBySocketInfoBaseWidget(
        SocketInfoBaseWidget *widget) const;

    std::shared_ptr<WidgetGroup> getMappedWidgetGroupByCompanion(
        std::shared_ptr<Companion> companion) const;

    void sendMessage(
        MessageType type, std::shared_ptr<Companion> companion, std::shared_ptr<Action> action,
        const std::string &text);

    void sendFile(std::shared_ptr<Companion> companion, const std::filesystem::path &path);
    void receiveMessage(std::shared_ptr<Companion> companion, const std::string &json);
    void addEarlyMessages(std::shared_ptr<Companion> companion);
    void resetSelectedCompanion(std::shared_ptr<Companion> companion);
    void createCompanion(std::shared_ptr<CompanionAction> action);
    void updateCompanion(std::shared_ptr<CompanionAction> action);
    void deleteCompanion(std::shared_ptr<CompanionAction> action);
    void clearChatHistory(std::shared_ptr<Companion> companion);
    void clearCompanionHistory(std::shared_ptr<CompanionAction> action);
    void createUserPassword(std::shared_ptr<PasswordAction> action);
    void authenticateUser(std::shared_ptr<PasswordAction> action);
    void hideSelectedCompanionCentralPanel();
    void showSelectedCompanionCentralPanel();
    void startUserAuthentication();
    void sendUnsentMessages(std::shared_ptr<Companion> companion);
    void requestHistoryFromCompanion(std::shared_ptr<Companion> companion);
    void sendChatHistoryToCompanion(std::shared_ptr<Companion> companion);
    // bool isInitialised();
    std::filesystem::path getLastOpenedPath();
    void setLastOpenedPath(const std::filesystem::path &path);

private:

    std::shared_ptr<Companion> getMappedCompanionByWidgetGroup(
        std::shared_ptr<WidgetGroup> group) const;

    void fillCompanionMessageMapping(std::shared_ptr<Companion> companion, bool containersNotEmpty);
    bool connectToDb();
    bool buildCompanions();
    void buildWidgetGroups();
    std::shared_ptr<Companion> addCompanionObject(int id, const std::string &name);
    void createWidgetGroupAndAddToMapping(std::shared_ptr<Companion> companion);
    void deleteCompanionObject(std::shared_ptr<Companion> companion);
    void deleteWidgetGroupAndDeleteFromMapping(std::shared_ptr<Companion> companion);
    bool companionDataValidation(std::shared_ptr<CompanionAction> action);
    bool passwordDataValidation(std::shared_ptr<PasswordAction> action);
    bool checkCompanionDataForExistanceAtCreation(std::shared_ptr<CompanionAction> action);
    bool checkCompanionDataForExistanceAtUpdate(std::shared_ptr<CompanionAction> action);

    void waitForMessageReceptionConfirmation(
        std::shared_ptr<Companion> companion, std::shared_ptr<MessageState> state,
        std::shared_ptr<Message> message);

    bool markMessageAsSent(std::shared_ptr<Companion> companion, std::shared_ptr<Message> message);

    bool markMessageAsReceived(
        std::shared_ptr<Companion> companion, std::shared_ptr<Message> message);

    std::tuple<uint32_t, uint8_t, std::string> pushMessageToDB(
        const std::string &companionName, const std::string &authorName,
        const std::string &timestamp, const std::string &text, const bool &isSent,
        const bool &isReceived);

    // TODO use std::function instead of function ptr
    template<typename T, typename... Ts>
    std::shared_ptr<DBReplyData> getDBData(
        bool log, std::string &&mark,
        std::shared_ptr<PGresult>(*func)(std::shared_ptr<PGconn>, bool, const Ts&...),
        T &&keys, const Ts&... args)
    {
        std::shared_ptr<PGresult> dbResult = func(dbConnection_, log, args...);

        if (log) {
            logArgs(logDelimiter);
            logArgs(mark);
            logArgs("dbResult:", dbResult);
        }

        if (!dbResult) {
            showErrorDialogAndLogError("Database request error, dbResult is nullptr");

            return nullptr;
        }

        auto dbData = std::make_shared<DBReplyData>(std::forward<T>(keys));

        if (getDataFromDBResult(log, dbData, dbResult, 0) == -1) {
            showErrorDialogAndLogError("Error getting data from dbResult");

            return nullptr;
        }

        if (log) {
            // logArgs("dbData->size():", dbData->size());
            logDBReplyData(dbData);
            logArgs(logDelimiter);
        }

        return dbData;
    }

    // bool initialized_;
    std::mutex messageStateToMessageMapMutex_;
    std::shared_ptr<PGconn> dbConnection_;
    bool userIsAuthenticated_;
    std::shared_ptr<Companion> selectedCompanion_;

    std::map<int, std::pair<std::shared_ptr<Companion>, std::shared_ptr<WidgetGroup>>>
        mapCompanionToWidgetGroup_;

    std::filesystem::path lastOpenedPath_;
};

std::shared_ptr<Manager> getManager();

#endif // MANAGER_HPP
