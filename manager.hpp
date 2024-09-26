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
//    SocketInfo() = default;
    SocketInfo() {};
//    SocketInfo(std::string&, std::string&, std::string&);
    SocketInfo(std::string&, std::string&);
//    SocketInfo(std::string&&, std::string&&, std::string&&);
    SocketInfo(std::string&&, std::string&&);
    SocketInfo(const SocketInfo&);
    SocketInfo(SocketInfo&&) {};
//    SocketInfo(std::string&, std::string&, std::string&);
    // SocketInfo(std::string&, std::string&);
    ~SocketInfo() = default;

    void print();
//    std::string getName();
    std::string getIpaddress() const;
    std::string getPort() const;
    uint16_t getPortInt() const;

private:
//    std::string name_;
    std::string ipaddress_;
    std::string port_;
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

    bool initMessaging();

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
