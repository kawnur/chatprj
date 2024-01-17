#ifndef MANAGER_HPP
#define MANAGER_HPP

#include "db_interaction.hpp"

class SocketInfo {

public:
//    SocketInfo() = default;
    SocketInfo() {};
    SocketInfo(QString&, QString&, QString&);
    SocketInfo(QString&&, QString&&, QString&&);
    SocketInfo(const SocketInfo&);
    SocketInfo(SocketInfo&&) {};
    SocketInfo(std::string&, std::string&, std::string&);
    ~SocketInfo() = default;

    void print();
    QString getName();
    QString getIpaddress();
    QString getPort();

private:
    QString name_;
    QString ipaddress_;
    QString port_;
};

class Message {

public:
    Message();
    ~Message() = default;

private:
    QString text_;
};

class Companion {
public:
    Companion();
    ~Companion() = default;

private:
    SocketInfo* socketInfo_;
    QString* inputBuffer_;

    std::vector<Message> messages_;

};

class Manager : public QObject{
public:
    Manager();
    ~Manager();

    void set();

private:
    void connectToDb();
    void buildSockets();

    PGconn* dbConnection_;
    std::vector<SocketInfo> sockets_;
};

#endif // MANAGER_HPP
