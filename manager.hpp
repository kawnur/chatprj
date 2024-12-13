#ifndef MANAGER_HPP
#define MANAGER_HPP

#include <boost/asio.hpp>
#include <memory>
#include <string>

#include "chat_client.hpp"
#include "chat_server.hpp"
#include "constants.hpp"
#include "db_interaction.hpp"
#include "logging.hpp"
#include "mainwindow.hpp"
#include "utils.hpp"

class ChatClient;
class ChatServer;
class DBReplyData;
class MainWindow;
class SocketInfoBaseWidget;
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
    SocketInfo* getSocketInfoPtr() const;
    void setSocketInfo(SocketInfo*);
//    void addMessage(int, int, std::tm, const std::string&, bool);
    void addMessage(int, int, const std::string&, const std::string&, bool);
    const std::vector<Message>* getMessagesPtr() const;
    void sendLastMessage();

private:
    int id_;
    std::string name_;
    SocketInfo* socketInfoPtr_;

    ChatClient* clientPtr_;

    // boost::asio::io_context* io_contextPtr_;
    ChatServer* serverPtr_;

    std::vector<Message> messages_;
};

class Manager : public QObject  // TODO do we need inheritance?
{
public:
    Manager();
    ~Manager();

    void set();
    void sendMessage(WidgetGroup*, const std::string&);
    void receiveMessage(Companion*, const std::string&);

    const Companion* getMappedCompanionBySocketInfoBaseWidget(SocketInfoBaseWidget*) const;
    // const WidgetGroup* getWidgetGroupByCompanion(Companion*) const;

    void resetSelectedCompanion(const Companion*);

    bool addNewCompanion(const std::string&, const std::string&, const std::string&);

private:
    PGconn* dbConnectionPtr_;

    const Companion* selectedCompanionPtr_;

    std::vector<Companion*> companionPtrs_;
    std::map<const Companion*, WidgetGroup*> mapCompanionToWidgetGroup_;

    const Companion* getMappedCompanionByWidgetGroup(WidgetGroup*) const;

    std::pair<int, std::string> pushMessageToDB(const std::string&, const std::string&, const std::string&);

    bool connectToDb();
    bool buildCompanions();

    void buildWidgetGroups();

    Companion* addCompanionObject(int, const std::string&);
    void createWidgetGroupAndAddToMapping(Companion*);

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
            logArgs(LOG_DELIMITER);
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
            showErrorDialogAndLogError("Error getting db data from dbResultPtr");
            return nullptr;
        }

        if(logging)
        {
            logArgs("dbDataPtr->size():", dbDataPtr->size());
            dbDataPtr->logData();
            logArgs(LOG_DELIMITER);
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

    void addTextToAppLogWidget(const QString&);
    void oldSelectedCompanionActions(const Companion*);
    void newSelectedCompanionActions(const Companion*);
    size_t getLeftPanelChildrenSize();
    void addStubWidgetToLeftPanel();
    void removeStubsFromLeftPanel();
    void addWidgetToLeftPanel(SocketInfoBaseWidget*);
    void addWidgetToCentralPanel(QWidget*);

    void createDialog(QDialog*, const DialogType, const std::string&);

private:
    MainWindow* mainWindowPtr_;

};

GraphicManager* getGraphicManagerPtr();

#endif // MANAGER_HPP
