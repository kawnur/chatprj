#ifndef CHAT_CLIENT
#define CHAT_CLIENT

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>

//#include "application.hpp"
#include "mainwindow.hpp"


using boost::asio::ip::tcp;

enum { max_length = 1024 };
//short max_length = 1024;

class ChatClient
{
private:
    std::string ipaddress_;
    std::string port_;

    boost::asio::io_context io_context_;
    tcp::socket socket_;
    tcp::resolver resolver_;

public:
    ChatClient(std::string&&, std::string&&);
    ~ChatClient() = default;

    bool connect();

    int send(std::string);
};

#endif // CHAT_CLIENT
