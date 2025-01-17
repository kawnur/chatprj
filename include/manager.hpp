#ifndef MANAGER_HPP
#define MANAGER_HPP

#include <memory>
#include <string>

#include "companion.hpp"
#include "db_interaction.hpp"
#include "utils.hpp"

class Companion;
class CompanionAction;
class DBReplyData;
class Message;
class MessageState;
class PasswordAction;
class SocketInfoBaseWidget;
class WidgetGroup;

int getDataFromDBResult(const bool, std::shared_ptr<DBReplyData>&, const PGresult*, int);
template<typename... Ts> void logArgs(Ts&&... args);
void showErrorDialogAndLogError(QWidget*, const QString&);

class Manager : public QObject  // TODO do we need inheritance?
{
public:
    Manager();
    ~Manager();

    void set();
    void sendMessage(Companion*, const std::string&);
    void receiveMessage(Companion*, const std::string&);

    bool getUserIsAuthenticated();

    const Companion* getMappedCompanionBySocketInfoBaseWidget(SocketInfoBaseWidget*) const;
    WidgetGroup* getMappedWidgetGroupByCompanion(const Companion*) const;
    // WidgetGroup* getMappedWidgetGroupByCompanion(Companion*) const;
    const MessageState* getMappedMessageStateByMessagePtr(const Message*);

    void resetSelectedCompanion(const Companion*);

    void createCompanion(CompanionAction*);
    void updateCompanion(CompanionAction*);
    void deleteCompanion(CompanionAction*);
    void clearCompanionHistory(CompanionAction*);

    void createUserPassword(PasswordAction*);
    void authenticateUser(PasswordAction*);

    void hideSelectedCompanionCentralPanel();
    void showSelectedCompanionCentralPanel();

    bool isSelectedCompanionNullptr();

    void startUserAuthentication();

    void sendUnsentMessages(const Companion*);

    void requestHistoryFromCompanion(const Companion*);

private:
    // std::mutex networkIdToMessageMapMutex_;
    std::mutex messageStateToMessageMapMutex_;

    PGconn* dbConnectionPtr_;

    bool userIsAuthenticated_;
    const Companion* selectedCompanionPtr_;

    std::vector<Companion*> companionPtrs_;  // TODO modify containers
    std::map<const Companion*, WidgetGroup*> mapCompanionToWidgetGroup_;  // TODO use ref to ptr as value
    // std::map<std::string, const Message*> mapNetworkIdToMessage_;
    std::map<const MessageState*, const Message*> mapMessageStateToMessage_;

    // std::string addToNetworkIdToMessageMapping(const Message*);
    bool addToMessageStateToMessageMapping(const MessageState*, const Message*);

    const Companion* getMappedCompanionByWidgetGroup(WidgetGroup*) const;
    // std::string getMappedNetworkIdByMessagePtr(Message*);

    std::pair<const MessageState*, const Message*>
        getMessageStateAndMessageMappingPairByMessageMappingKey(const std::string&);

    std::tuple<uint32_t, uint8_t, std::string> pushMessageToDB(
        const std::string&, const std::string&, const std::string&,
        const std::string&, const bool&, const bool&);

    bool connectToDb();
    bool buildCompanions();
    void buildWidgetGroups();

    Companion* addCompanionObject(int, const std::string&);
    void createWidgetGroupAndAddToMapping(Companion*);

    void deleteCompanionObject(Companion*);
    void deleteWidgetGroupAndDeleteFromMapping(Companion*);

    bool companionDataValidation(CompanionAction*);
    bool passwordDataValidation(PasswordAction*);

    bool checkCompanionDataForExistanceAtCreation(CompanionAction*);
    bool checkCompanionDataForExistanceAtUpdate(CompanionAction*);

    void waitForMessageReceptionConfirmation(Companion*, MessageState*, Message*);

    bool markMessageAsSent(const Message*);
    bool markMessageAsReceived(const Message*);

    template<typename... Ts>
    std::shared_ptr<DBReplyData> getDBDataPtr(
        const bool logging,
        const char* mark,
        PGresult*(*func)(const PGconn*, const bool, const Ts&...),
        std::vector<std::string>&& keys,
        const Ts&... args)
    {
        PGresult* dbResultPtr = func(this->dbConnectionPtr_, logging, args...);

        if(logging)
        {
            logArgs(logDelimiter);
            logArgs(mark);
            logArgs("dbResultPtr:", dbResultPtr);
        }

        if(!dbResultPtr)
        {
            showErrorDialogAndLogError(
                nullptr, QString("Database request error, dbResultPtr is nullptr"));

            return nullptr;
        }

        std::shared_ptr<DBReplyData> dbDataPtr = std::make_shared<DBReplyData>(keys);

        if(getDataFromDBResult(logging, dbDataPtr, dbResultPtr, 0) == -1)
        {
            showErrorDialogAndLogError(nullptr, "Error getting data from dbResultPtr");
            return nullptr;
        }

        if(logging)
        {
            logArgs("dbDataPtr->size():", dbDataPtr->size());
            dbDataPtr->logData();
            logArgs(logDelimiter);
        }

        return dbDataPtr;
    }
};

Manager* getManagerPtr();

#endif // MANAGER_HPP
