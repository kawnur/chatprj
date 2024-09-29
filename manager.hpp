#ifndef MANAGER_HPP
#define MANAGER_HPP

#include <boost/asio.hpp>

#include "chat_client.hpp"
#include "chat_server.hpp"
#include "db_interaction.hpp"
#include "logging.hpp"
#include "mainwindow.hpp"


class ChatClient;
class ChatServer;
class WidgetGroup;

class SocketInfo
{
public:
    SocketInfo() {};
    SocketInfo(std::string&, uint16_t&, uint16_t&);
    SocketInfo(std::string&&, uint16_t&&, uint16_t&&);
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
    Companion(int, std::string&&);
    ~Companion();

    bool startServer();
    bool createClient();
    bool connectClient();

    int getId();
    std::string getName() const;
    SocketInfo* getSocketInfo() const;
    void setSocketInfo(SocketInfo*);
//    void addMessage(int, int, std::tm, const std::string&, bool);
    void addMessage(int, int, const std::string&, const std::string&, bool);
    const std::vector<Message>* getMessagesPtr() const;
    bool sendLastMessage();

private:
    int id_;
    std::string name_;
    SocketInfo* socketInfoPtr_;

    ChatClient* clientPtr_;

    // boost::asio::io_context* io_contextPtr_;
    ChatServer* serverPtr_;

    std::vector<Message> messages_;
};

class Manager : public QObject
{
public:
    Manager();
    ~Manager();

    void set();
    void sendMessage(const Companion*, WidgetGroup*);

private:
    bool connectToDb();
    bool buildCompanions();

    PGconn* dbConnectionPtr_;
    std::vector<Companion*> companionPtrs_;
};

Manager* getManagerPtr();

#endif // MANAGER_HPP
