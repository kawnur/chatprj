#ifndef MANAGER_HPP
#define MANAGER_HPP

#include <boost/asio.hpp>
#include <memory>
#include <string>

#include "chat_client.hpp"
#include "chat_server.hpp"
#include "constants.hpp"
#include "data.hpp"
#include "db_interaction.hpp"
#include "logging.hpp"
#include "mainwindow.hpp"
#include "utils.hpp"

class CentralPanelWidget;
class ChatClient;
class ChatServer;
class CompanionDataDialog;
class DBReplyData;
class LeftPanelWidget;
class MainWindow;
class RightPanelWidget;
class SocketInfoBaseWidget;
class StubWidgetGroup;
class TwoButtonsTextDialog;
class WidgetGroup;

int getDataFromDBResult(std::shared_ptr<DBReplyData>&, const PGresult*, int);
template<typename... Ts> void logArgs(Ts&&... args);
void showErrorDialogAndLogError(const std::string&);

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
    Message(int, int, const std::string&, const std::string&, bool);
    ~Message() = default;

    int getCompanionId() const;
    int getAuthorId() const;
    std::string getTime() const;
    std::string getText() const;
    bool getIsSent() const;

private:
    int companion_id_;
    int author_id_;
//    std::tm time_;  // TODO add timezone support
    std::string time_;  // TODO add timezone support
    std::string text_;
    bool isSent_;
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
//    void addMessage(int, int, std::tm, const std::string&, bool);
    void addMessage(int, int, const std::string&, const std::string&, bool);
    const std::vector<Message>* getMessagesPtr() const;
    void sendLastMessage();

    void updateData(const CompanionData*);

private:
    int id_;  // TODO change type
    std::string name_;
    SocketInfo* socketInfoPtr_;

    ChatClient* clientPtr_;

    // boost::asio::io_context* io_contextPtr_;
    ChatServer* serverPtr_;

    std::vector<Message> messages_;
};

class CompanionAction : public QObject
{
    Q_OBJECT

public:
    CompanionAction(CompanionActionType, MainWindow*, Companion*);
    ~CompanionAction();

    void set();

    CompanionActionType getActionType();

    std::string getName() const;
    std::string getIpAddress() const;
    std::string getServerPort() const;
    std::string getClientPort() const;

    int getCompanionId() const;
    Companion* getCompanionPtr() const;

    CompanionDataDialog* getFormDialogPtr();
    void updateCompanionObjectData();

public slots:
    void sendData();

private:
    CompanionActionType actionType_;
    CompanionData* dataPtr_;

    MainWindow* mainWindowPtr_;
    Companion* companionPtr_;

    CompanionDataDialog* formDialogPtr_;
    TwoButtonsTextDialog* deleteDialogPtr_;

    // void sendData();
};

class Manager : public QObject  // TODO do we need inheritance?
{
public:
    Manager();
    ~Manager();

    void set();
    void sendMessage(WidgetGroup*, const std::string&);
    void sendMessage(Companion*, const std::string&);
    void receiveMessage(Companion*, const std::string&);

    const Companion* getMappedCompanionBySocketInfoBaseWidget(SocketInfoBaseWidget*) const;
    // const WidgetGroup* getWidgetGroupByCompanion(Companion*) const;

    void resetSelectedCompanion(const Companion*);

    void createCompanion(CompanionAction*);
    void updateCompanion(CompanionAction*);
    void deleteCompanion(CompanionAction*);
    void clearCompanionHistory(CompanionAction*);

    void hideSelectedCompanionCentralPanel();
    void showSelectedCompanionCentralPanel();

    bool isSelectedCompanionNullptr();

private:
    PGconn* dbConnectionPtr_;

    const Companion* selectedCompanionPtr_;

    std::vector<Companion*> companionPtrs_;  // TODO modify containers
    std::map<const Companion*, WidgetGroup*> mapCompanionToWidgetGroup_;

    const Companion* getMappedCompanionByWidgetGroup(WidgetGroup*) const;

    std::pair<int, std::string> pushMessageToDB(const std::string&, const std::string&, const std::string&);

    bool connectToDb();
    bool buildCompanions();
    void buildWidgetGroups();

    Companion* addCompanionObject(int, const std::string&);
    void createWidgetGroupAndAddToMapping(Companion*);

    void deleteCompanionObject(Companion*);
    void deleteWidgetGroupAndDeleteFromMapping(Companion*);

    bool companionDataValidation(CompanionAction*);
    bool checkCompanionDataForExistanceAtCreation(CompanionAction*);
    bool checkCompanionDataForExistanceAtUpdate(CompanionAction*);

    template<typename... Ts>
    std::shared_ptr<DBReplyData> getDBDataPtr(
        bool logging,
        const char* mark,
        PGresult*(*func)(const PGconn*, const Ts&...),
        std::vector<std::string>&& keys,
        const Ts&... args)
    {
        PGresult* dbResultPtr = func(this->dbConnectionPtr_, args...);

        if(logging)
        {
            logArgs(logDelimiter);
            logArgs(mark);
            logArgs("dbResultPtr:", dbResultPtr);
        }

        if(!dbResultPtr)
        {
            showErrorDialogAndLogError("Database request error, dbResultPtr is nullptr");
            return nullptr;
        }

        std::shared_ptr<DBReplyData> dbDataPtr = std::make_shared<DBReplyData>(keys);

        if(getDataFromDBResult(dbDataPtr, dbResultPtr, 0) == -1)
        {
            showErrorDialogAndLogError("Error getting data from dbResultPtr");
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

    void buildMainWindow();

    void setParentsForStubs(QWidget*, QWidget*);
    void setStubWidgets();

    void sendMessage(WidgetGroup*, const std::string&);
    void sendMessage(Companion*, const std::string&);

    void addTextToAppLogWidget(const QString&);
    // void oldSelectedCompanionActions(const Companion*);
    // void newSelectedCompanionActions(const Companion*);
    size_t getCompanionPanelChildrenSize();
    // void addStubWidgetToCompainonPanel();

    // void addWidgetToMainWindow(QWidget*);

    void hideWidgetGroupCentralPanel(WidgetGroup*);
    void showWidgetGroupCentralPanel(WidgetGroup*);

    void addWidgetToMainWindowContainerAndSetParentTo(
        MainWindowContainerPosition, QWidget*);

    void addWidgetToCompanionPanel(SocketInfoBaseWidget*);
    // void addWidgetToCentralPanel(QWidget*);
    // void setMainWindowLeftPanel(LeftPanelWidget*);
    // void setMainWindowCentralPanel(CentralPanelWidget*);
    // void setMainWindowRightPanel(RightPanelWidget*);

    void removeStubsFromCompanionPanel();
    void removeWidgetFromCompanionPanel(SocketInfoBaseWidget*);

    void createTextDialog(QDialog*, const DialogType, const std::string&);

    void createCompanion();
    void updateCompanion(Companion*);
    void clearCompanionHistory(Companion*);
    void clearChatHistory(WidgetGroup*);
    void deleteCompanion(Companion*);

    void sendCompanionDataToManager(CompanionAction*);
    void showCompanionInfoDialog(CompanionAction*, std::string&&);

    void hideCentralPanelStub();
    void showCentralPanelStub();

    void hideInfo();
    void showInfo();

    MainWindow* getMainWindowPtr();

private:
    StubWidgetGroup* stubWidgetsPtr_;
    MainWindow* mainWindowPtr_;
};

GraphicManager* getGraphicManagerPtr();

#endif // MANAGER_HPP
