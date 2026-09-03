#ifndef MANAGER_HPP
#define MANAGER_HPP

#include <memory>
#include <string>

#include "action.hpp"
#include "companion.hpp"
#include "db_interaction.hpp"
#include "utils.hpp"

class Action;
class Companion;
class CompanionAction;
class DBReplyData;
class FileAction;
class Message;
class MessageState;
class PasswordAction;
class SocketInfoBaseWidget;
class TextDialog;
class WidgetGroup;

int getDataFromDBResult(
    const bool&, std::shared_ptr<DBReplyData>, std::shared_ptr<PGresult>, int);

template<typename... Ts> void logArgs(Ts&&... args);
void logDBReplyData(std::shared_ptr<DBReplyData>);

void showInfoDialogAndLogInfo(QString&&, std::shared_ptr<QWidget>);
void showWarningDialogAndLogWarning(const QString&, std::shared_ptr<QWidget>);

// void showErrorDialogAndLogError(const QString&, std::shared_ptr<QWidget>);
// template<typename T> void showErrorDialogAndLogError(std::shared_ptr<QWidget>, T&&);
void showErrorDialogAndLogError(QString&&, std::shared_ptr<QWidget> = nullptr);

class Manager : public QObject { // TODO do we need inheritance?
public:
    Manager();
    ~Manager();

    std::shared_ptr<Companion> getSelectedCompanion();
    bool getUserIsAuthenticated();

    std::shared_ptr<Companion> getMappedCompanionBySocketInfoBaseWidget(std::shared_ptr<SocketInfoBaseWidget>) const;
    std::shared_ptr<WidgetGroup> getMappedWidgetGroupByCompanion(std::shared_ptr<Companion>) const;

    void set();
    void sendMessage(MessageType, std::shared_ptr<Companion>, std::shared_ptr<Action>, const std::string&);
    void sendFile(std::shared_ptr<Companion>, const std::filesystem::path&);
    void receiveMessage(std::shared_ptr<Companion>, const std::string&);
    void addEarlyMessages(std::shared_ptr<Companion>);
    void resetSelectedCompanion(std::shared_ptr<Companion>);
    void createCompanion(std::shared_ptr<CompanionAction>);
    void updateCompanion(std::shared_ptr<CompanionAction>);
    void deleteCompanion(std::shared_ptr<CompanionAction>);
    void clearChatHistory(std::shared_ptr<Companion>);
    void clearCompanionHistory(std::shared_ptr<CompanionAction>);
    void createUserPassword(std::shared_ptr<PasswordAction>);
    void authenticateUser(std::shared_ptr<PasswordAction>);
    void hideSelectedCompanionCentralPanel();
    void showSelectedCompanionCentralPanel();
    void startUserAuthentication();
    void sendUnsentMessages(std::shared_ptr<Companion>);
    void requestHistoryFromCompanion(std::shared_ptr<Companion>);
    void sendChatHistoryToCompanion(std::shared_ptr<Companion>);
    bool isInitialised();
    std::filesystem::path getLastOpenedPath();
    void setLastOpenedPath(const std::filesystem::path&);

private:
    bool initialized_;
    std::mutex messageStateToMessageMapMutex_;
    std::shared_ptr<PGconn> dbConnection_;
    bool userIsAuthenticated_;
    std::shared_ptr<Companion> selectedCompanion_;

    std::map<int, std::pair<std::shared_ptr<Companion>, std::shared_ptr<WidgetGroup>>>
        mapCompanionIdToCompanionInfo_;

    std::filesystem::path lastOpenedPath_;

    std::shared_ptr<Companion> getMappedCompanionByWidgetGroup(std::shared_ptr<WidgetGroup>) const;
    void fillCompanionMessageMapping(std::shared_ptr<Companion>, bool);
    bool connectToDb();
    bool buildCompanions();
    void buildWidgetGroups();
    std::shared_ptr<Companion> addCompanionObject(int, const std::string&);
    void createWidgetGroupAndAddToMapping(std::shared_ptr<Companion>);
    void deleteCompanionObject(std::shared_ptr<Companion>);
    void deleteWidgetGroupAndDeleteFromMapping(std::shared_ptr<Companion>);
    bool companionDataValidation(std::shared_ptr<CompanionAction>);
    bool passwordDataValidation(std::shared_ptr<PasswordAction>);
    bool checkCompanionDataForExistanceAtCreation(std::shared_ptr<CompanionAction>);
    bool checkCompanionDataForExistanceAtUpdate(std::shared_ptr<CompanionAction>);
    void waitForMessageReceptionConfirmation(std::shared_ptr<Companion>, std::shared_ptr<MessageState>, std::shared_ptr<Message>);
    bool markMessageAsSent(std::shared_ptr<Companion>, std::shared_ptr<Message>);
    bool markMessageAsReceived(std::shared_ptr<Companion>, std::shared_ptr<Message>);

    std::tuple<uint32_t, uint8_t, std::string> pushMessageToDB(
        const std::string&, const std::string&, const std::string&,
        const std::string&, const bool&, const bool&);

    template<typename T, typename... Ts>
    std::shared_ptr<DBReplyData> getDBData(
        const bool& logging, std::shared_ptr<char> mark,
        std::shared_ptr<PGresult>(*func)(std::shared_ptr<PGconn>, const bool&, const Ts&...),
        T&& keys, const Ts&... args) {

        std::shared_ptr<PGresult> dbResult = func(this->dbConnection_, logging, args...);

        if(logging) {
            logArgs(logDelimiter);
            logArgs(mark);
            logArgs("dbResult:", dbResult);
        }

        if(!dbResult) {
            showErrorDialogAndLogError(
                "Database request error, dbResult is nullptr");

            return nullptr;
        }

        std::shared_ptr<DBReplyData> dbData =
            std::make_shared<DBReplyData>(std::forward<T>(keys));

        if(getDataFromDBResult(logging, dbData, dbResult, 0) == -1) {
            showErrorDialogAndLogError("Error getting data from dbResult");
            return nullptr;
        }

        if(logging) {
            // logArgs("dbData->size():", dbData->size());
            logDBReplyData(dbData);
            logArgs(logDelimiter);
        }

        return dbData;
    }
};

std::shared_ptr<Manager> getManager();

#endif // MANAGER_HPP
