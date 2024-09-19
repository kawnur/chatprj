#ifndef MANAGER_HPP
#define MANAGER_HPP

#include "db_interaction.hpp"
#include "mainwindow.hpp"

class WidgetGroup;

class SocketInfo
{
public:
//    SocketInfo() = default;
    SocketInfo() {};
//    SocketInfo(QString&, QString&, QString&);
    SocketInfo(QString&, QString&);
//    SocketInfo(QString&&, QString&&, QString&&);
    SocketInfo(QString&&, QString&&);
    SocketInfo(const SocketInfo&);
    SocketInfo(SocketInfo&&) {};
//    SocketInfo(std::string&, std::string&, std::string&);
    SocketInfo(std::string&, std::string&);
    ~SocketInfo() = default;

    void print();
//    QString getName();
    QString getIpaddress() const;
    QString getPort() const;

private:
//    QString name_;
    QString ipaddress_;
    QString port_;
};

class Message
{
public:
//    Message(int, int, std::tm, const QString&, bool);
    Message(int, int, const QString&, const QString&, bool);
    ~Message() = default;

    int getCompanionId() const;
    int getAuthorId() const;
    QString getTime() const;
    QString getText() const;
    bool getIsSent() const;

private:
    int companion_id_;
    int author_id_;
//    std::tm time_;  // TODO add timezone support
    QString time_;  // TODO add timezone support
    QString text_;
    bool isSent_;
};

class Companion
{
public:
    Companion(int, QString&&);
    ~Companion() = default;

    int getId();
    QString getName() const;
    SocketInfo* getSocketInfo() const;
    void setSocketInfo(SocketInfo*);
//    void addMessage(int, int, std::tm, const QString&, bool);
    void addMessage(int, int, const QString&, const QString&, bool);
    const std::vector<Message>* getMessagesPtr() const;

private:
    int id_;
    QString name_;
    SocketInfo* socketInfo_;

    std::vector<Message> messages_;
};

class Manager : public QObject
{
public:
    Manager();
    ~Manager() {};

    void set();
    void sendMessage(const Companion*, WidgetGroup*);

private:
    void connectToDb();
    void buildCompanions();

    PGconn* dbConnection_;
    std::vector<Companion*> companions_;
};

Manager* getManagerPtr();

#endif // MANAGER_HPP
