#ifndef CHAT_CLIENT
#define CHAT_CLIENT

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>

//#include "application.hpp"
#include "mainwindow.hpp"
#include "utils.hpp"

using boost::asio::ip::tcp;

enum { max_length = 1024 };
//short max_length = 1024;

class ChatClient
{
private:
    std::string ipAddress_;
    uint16_t port_;

    boost::asio::io_context io_context_;
    tcp::socket socket_;
    tcp::resolver resolver_;

public:
    ChatClient(std::string&&, uint16_t&&);
    ~ChatClient() = default;

    bool connect();
    bool disconnect();

    bool send(std::string);
};

#endif // CHAT_CLIENT
