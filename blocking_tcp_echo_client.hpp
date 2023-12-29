// https://live.boost.org/doc/libs/1_83_0/doc/html/boost_asio/example/cpp11/echo/blocking_tcp_echo_client.cpp

#ifndef BLOCKING_TCP_ECHO_CLIENT
#define BLOCKING_TCP_ECHO_CLIENT

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>
#include <QObject>
#include <QString>

using boost::asio::ip::tcp;

enum { max_length = 1024 };

class EchoClient : public QObject {

    Q_OBJECT

private:

public:
    EchoClient();
    ~EchoClient() = default;

//public slots:
    int send(QString);
};

//int main(int argc, char* argv[])
//int blocking_tcp_echo_client();

#endif
