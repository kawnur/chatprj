#ifndef CHAT_CLIENT
#define CHAT_CLIENT

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>
#include <QObject>
#include <QString>

//#include "application.hpp"
#include "mainwindow.hpp"


using boost::asio::ip::tcp;

enum { max_length = 1024 };
//short max_length = 1024;

class ChatClient : public QObject {

    Q_OBJECT

private:

public:
    ChatClient();
    ~ChatClient() = default;

    int send(QString);
};

#endif // CHAT_CLIENT
