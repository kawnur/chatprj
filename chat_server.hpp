#ifndef CHAT_SERVER_HPP
#define CHAT_SERVER_HPP

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>

//#include "application.hpp"
#include "mainwindow.hpp"


using boost::asio::ip::tcp;

//class ChatApp;

class ServerSession
        : public std::enable_shared_from_this<class ServerSession> {

public:
    ServerSession(tcp::socket socket) : socket_(std::move(socket)) {}

    void start();

private:
    void do_read();
    void do_write(std::size_t);

    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
};

class ChatServer {

public:
    ChatServer(boost::asio::io_context& io_context, short port)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        do_accept();
    }
//    ChatServer(short);

private:
    void do_accept();

//    boost::asio::io_context io_context_;
    tcp::acceptor acceptor_;

};

int async_tcp_echo_server();

#endif // CHAT_SERVER_HPP
