#ifndef MANAGER_HPP
#define MANAGER_HPP

#include <boost/asio.hpp>
#include <memory>
#include <mutex>
#include <QDialogButtonBox>
#include <string>

#include "chat_client.hpp"
#include "chat_server.hpp"
#include "constants.hpp"
#include "data.hpp"
#include "db_interaction.hpp"
#include "logging.hpp"
#include "mainwindow.hpp"
#include "utils.hpp"
#include "widgets.hpp"

class ActionTextDialog;
class ButtonInfo;
class CentralPanelWidget;
class ChatClient;
class ChatServer;
class CompanionDataDialog;
class DBReplyData;
class Dialog;
class LeftPanelWidget;
class MainWindow;
class MessageWidget;
class QWidget;
class RightPanelWidget;
class SocketInfoBaseWidget;
class StubWidgetGroup;
class TextDialog;
class WidgetGroup;

int getDataFromDBResult(const bool, std::shared_ptr<DBReplyData>&, const PGresult*, int);
template<typename... Ts> void logArgs(Ts&&... args);
void showErrorDialogAndLogError(QWidget*, const QString&);

class SocketInfo
{
public:
    SocketInfo() {};
    SocketInfo(std::string&, uint16_t&, uint16_t&);  // TODO remove
    SocketInfo(std::string&&, uint16_t&&, uint16_t&&);  // TODO remove

    template<typename T, typename U, typename P> SocketInfo
        (T&& ipAddress, U&& serverPort, P&& clientPort) :
        ipAddress_(std::forward<T>(ipAddress)),
        serverPort_(std::forward<U>(serverPort)),
        clientPort_(std::forward<P>(clientPort)) {}

    SocketInfo(const SocketInfo&);
    SocketInfo(SocketInfo&&) {};
    ~SocketInfo() = default;

    void print();
    std::string getIpAddress() const;
    uint16_t getServerPort() const;
    uint16_t getClientPort() const;

    void updateData(const CompanionData*);

private:
    std::string ipAddress_;
    uint16_t serverPort_;  // port number to open server at
    uint16_t clientPort_;  // port number to connect with client to
};

class Message
{
public:
//    Message(int, int, std::tm, const std::string&, bool);
    Message(uint32_t, uint8_t, uint8_t, const std::string&, const std::string&);
    ~Message() = default;

    uint32_t getId() const;
    uint8_t getCompanionId() const;
    uint8_t getAuthorId() const;
    bool isMessageFromMe() const;
    std::string getTime() const;
    std::string getText() const;
    // bool getIsSent() const;

private:
    uint32_t id_;
    uint8_t companion_id_;
    uint8_t author_id_;
//    std::tm time_;  // TODO add timezone support
    std::string time_;  // TODO add timezone support
    std::string text_;
    // bool isSent_;
};

class MessageState
{
public:
    MessageState(uint8_t, bool, bool, std::string);
    ~MessageState() = default;

    bool getIsAntecedent() const;
    bool getIsSent() const;
    bool getIsReceived() const;
    std::string getNetworkId() const;
    std::string getNetworkIdUnderscoreCompanionId() const;

    void setIsReceived(bool);
    void setNetworkId(const std::string&);
    void setNetworkIdUnderscoreCompanionId(const std::string&);

private:    
    bool isAntecedent_;
    bool isSent_;
    bool isReceived_;
    std::string networkId_;
    std::string networkIdUnderscoreCompanionId_;

};

class Companion
{
public:
    Companion(int, const std::string&);
    Companion(int, std::string&&);
    ~Companion();

    bool startServer();
    bool createClient();
    bool connectClient();
    bool disconnectClient();

    int getId();
    std::string getName() const;
    std::string getIpAddress() const;
    uint16_t getServerPort() const;
    uint16_t getClientPort() const;

    SocketInfo* getSocketInfoPtr() const;
    void setSocketInfo(SocketInfo*);
    void addMessage(Message*);

    const std::vector<Message*>* getMessagesPtr() const;
    bool sendMessage(bool, NetworkMessageType, std::string, const Message*);

    void updateData(const CompanionData*);

    Message* findMessage(uint32_t);

private:
    std::mutex messagesMutex_;

    int id_;  // TODO change type
    std::string name_;
    SocketInfo* socketInfoPtr_;

    ChatClient* clientPtr_;

    // boost::asio::io_context* io_contextPtr_;
    ChatServer* serverPtr_;

    // std::vector<Message> messages_;
    std::vector<Message*>* messagePointersPtr_;
};

class Action : public QObject
{
    Q_OBJECT

public:
    Action(Dialog* dialogPtr) : dialogPtr_(dialogPtr) {}
    ~Action() = default;

    void set();

    Dialog* getDialogPtr();

    virtual void sendData() {}

protected:
    Dialog* dialogPtr_;
};

class CompanionAction : public Action
{
    Q_OBJECT

public:
    CompanionAction(CompanionActionType, MainWindow*, Companion*);
    ~CompanionAction();

    CompanionActionType getActionType();

    std::string getName() const;
    std::string getIpAddress() const;
    std::string getServerPort() const;
    std::string getClientPort() const;

    int getCompanionId() const;
    Companion* getCompanionPtr() const;

    void updateCompanionObjectData();

public slots:
    void sendData() override;

private:
    CompanionActionType actionType_;
    CompanionData* dataPtr_;

    MainWindow* mainWindowPtr_;
    Companion* companionPtr_;
};

class PasswordAction : public Action
{
    Q_OBJECT

public:
    PasswordAction(PasswordActionType);
    ~PasswordAction();

    std::string getPassword();

    void sendData() override;

private:
    PasswordActionType actionType_;
    const std::string* passwordPtr_;
};

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

    std::tuple<uint32_t, uint8_t, std::string> pushMessageToDB(
        const std::string&, const std::string&, const std::string&, const std::string&);

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

class GraphicManager
{
public:
    GraphicManager();
    ~GraphicManager() = default;

    void set();

    void setParentsForStubs(QWidget*, QWidget*);
    void setStubWidgets();

    void sendMessage(Companion*, const std::string&);

    void addTextToAppLogWidget(const QString&);
    size_t getCompanionPanelChildrenSize();

    void hideWidgetGroupCentralPanel(WidgetGroup*);
    void showWidgetGroupCentralPanel(WidgetGroup*);

    void addWidgetToMainWindowContainerAndSetParentTo(
        MainWindowContainerPosition, QWidget*);

    void addWidgetToCompanionPanel(SocketInfoBaseWidget*);

    void removeWidgetFromCompanionPanel(SocketInfoBaseWidget*);

    void createTextDialogAndShow(QWidget*, DialogType, const QString&, std::vector<ButtonInfo>*);

    void createCompanion();
    void updateCompanion(Companion*);
    void clearCompanionHistory(Companion*);
    void clearChatHistory(WidgetGroup*);
    void deleteCompanion(Companion*);

    void sendCompanionDataToManager(CompanionAction*);
    void showCompanionInfoDialog(CompanionAction*, std::string&&);

    void sendNewPasswordDataToManager(PasswordAction*);
    void sendExistingPasswordDataToManager(PasswordAction*);

    void hideCompanionPanelStub();

    void hideCentralPanelStub();
    void showCentralPanelStub();

    void hideInfoViaBlur();
    void showInfoViaBlur();

    void hideInfoViaStubs();
    void showInfoViaStubs();

    void hideInfo();
    void showInfo();

    MainWindow* getMainWindowPtr();

    void createEntrancePassword();

    void enableMainWindowBlurEffect();
    void disableMainWindowBlurEffect();

    void getEntrancePassword();

    void addToMessageMapping(const Message*, const MessageWidget*);
    void markMessageWidgetAsSent(const Message*);
    void markMessageWidgetAsReceived(const Message*);

    void sortChatHistoryElementsForWidgetGroup(WidgetGroup*);

    // std::string getMappedMessageTimeByMessageWidgetPtr(MessageWidget*);
    const Message* getMappedMessageByMessageWidgetPtr(MessageWidget*);

private:
    std::mutex messageToMessageWidgetMapMutex_;
    std::map<const Message*, const MessageWidget*> mapMessageToMessageWidget_;

    StubWidgetGroup* stubWidgetsPtr_;
    MainWindow* mainWindowPtr_;
};

GraphicManager* getGraphicManagerPtr();

#endif // MANAGER_HPP
