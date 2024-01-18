#ifndef MANAGER_HPP
#define MANAGER_HPP

#include "db_interaction.hpp"

class SocketInfo {

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

class Message {

public:
    Message(int, std::tm, const QString&, bool);
    ~Message() = default;

private:
    int companion_id_;
    std::tm time_;  // TODO add timezone support
    QString text_;
    bool isSent_;
};

class Companion {
public:
    Companion(int, QString&);
    ~Companion() = default;

    int getId();
    QString getName() const;
    SocketInfo* getSocketInfo() const;
    QString* getInputBuffer() const;
    void setInputBuffer(QString&);
    void setSocketInfo(SocketInfo*);
    void addMessage(int, std::tm, const QString&, bool);

private:
    int id_;
    QString name_;
    SocketInfo* socketInfo_;
    QString* inputBuffer_;

    std::vector<Message> messages_;
};

class Manager : public QObject{
public:
    Manager();
    ~Manager() {};

    Companion* currentCompanion_;

    void set();

private:
    void connectToDb();
    void buildCompanions();

    PGconn* dbConnection_;
    std::vector<Companion*> companions_;
};

Manager* getManagerPtr();

#endif // MANAGER_HPP
