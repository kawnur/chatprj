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
    const bool&, std::shared_ptr<DBReplyData>&, const PGresult*&, int);

template<typename... Ts> void logArgs(Ts&&... args);
void logDBReplyData(std::shared_ptr<DBReplyData>&);

void showInfoDialogAndLogInfo(QString&&, QWidget* /*= nullptr*/);
void showWarningDialogAndLogWarning(const QString&, QWidget* /*= nullptr*/);

// void showErrorDialogAndLogError(const QString&, QWidget* /*= nullptr*/);
// template<typename T> void showErrorDialogAndLogError(QWidget*, T&&);
void showErrorDialogAndLogError(QString&&, QWidget* = nullptr);

class Manager : public QObject { // TODO do we need inheritance?
public:
    Manager();
    ~Manager();

    const Companion* getSelectedCompanionPtr();
    bool getUserIsAuthenticated();

    const Companion* getMappedCompanionBySocketInfoBaseWidget(SocketInfoBaseWidget*) const;
    WidgetGroup* getMappedWidgetGroupPtrByCompanionPtr(const Companion*) const;

    void set();
    void sendMessage(MessageType, Companion*, Action*, const std::string&);
    void sendFile(Companion*, const std::filesystem::path&);
    void receiveMessage(Companion*, const std::string&);
    void addEarlyMessages(const Companion*);
    void resetSelectedCompanion(const Companion*);
    void createCompanion(CompanionAction*);
    void updateCompanion(CompanionAction*);
    void deleteCompanion(CompanionAction*);
    void clearChatHistory(Companion*);
    void clearCompanionHistory(CompanionAction*);
    void createUserPassword(PasswordAction*);
    void authenticateUser(PasswordAction*);
    void hideSelectedCompanionCentralPanel();
    void showSelectedCompanionCentralPanel();
    void startUserAuthentication();
    void sendUnsentMessages(const Companion*);
    void requestHistoryFromCompanion(const Companion*);
    void sendChatHistoryToCompanion(const Companion*);
    bool isInitialised();
    std::filesystem::path getLastOpenedPath();
    void setLastOpenedPath(const std::filesystem::path&);

private:
    bool initialized_;
    std::mutex messageStateToMessageMapMutex_;
    PGconn* dbConnectionPtr_;
    bool userIsAuthenticated_;
    const Companion* selectedCompanionPtr_;
    std::map<int, std::pair<Companion*, WidgetGroup*>> mapCompanionIdToCompanionInfo_;
    std::filesystem::path lastOpenedPath_;

    const Companion* getMappedCompanionByWidgetGroup(WidgetGroup*) const;
    void fillCompanionMessageMapping(Companion*, bool);
    bool connectToDb();
    bool buildCompanions();
    void buildWidgetGroups();
    Companion* addCompanionObject(int, const std::string&);
    void createWidgetGroupAndAddToMapping(Companion*);
    void deleteCompanionObject(Companion*);
    void deleteWidgetGroupAndDeleteFromMapping(const Companion*);
    bool companionDataValidation(CompanionAction*);
    bool passwordDataValidation(PasswordAction*);
    bool checkCompanionDataForExistanceAtCreation(CompanionAction*);
    bool checkCompanionDataForExistanceAtUpdate(CompanionAction*);
    void waitForMessageReceptionConfirmation(Companion*, MessageState*, const Message*);
    bool markMessageAsSent(Companion*, const Message*);
    bool markMessageAsReceived(Companion*, const Message*);

    std::tuple<uint32_t, uint8_t, std::string> pushMessageToDB(
        const std::string&, const std::string&, const std::string&,
        const std::string&, const bool&, const bool&);

    template<typename T, typename... Ts>
    std::shared_ptr<DBReplyData> getDBDataPtr(
        const bool& logging,
        const char* mark,
        PGresult*(*func)(const PGconn*, const bool&, const Ts&...),
        T&& keys, const Ts&... args) {

        const PGresult* dbResultPtr = func(this->dbConnectionPtr_, logging, args...);

        if(logging) {
            logArgs(logDelimiter);
            logArgs(mark);
            logArgs("dbResultPtr:", dbResultPtr);
        }

        if(!dbResultPtr) {
            showErrorDialogAndLogError(
                "Database request error, dbResultPtr is nullptr");

            return nullptr;
        }

        std::shared_ptr<DBReplyData> dbDataPtr =
            std::make_shared<DBReplyData>(std::forward<T>(keys));

        if(getDataFromDBResult(logging, dbDataPtr, dbResultPtr, 0) == -1) {
            showErrorDialogAndLogError("Error getting data from dbResultPtr");
            return nullptr;
        }

        if(logging) {
            // logArgs("dbDataPtr->size():", dbDataPtr->size());
            logDBReplyData(dbDataPtr);
            logArgs(logDelimiter);
        }

        return dbDataPtr;
    }
};

Manager* getManagerPtr();

#endif // MANAGER_HPP
