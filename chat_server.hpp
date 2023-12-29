#ifndef CHAT_SERVER_HPP
#define CHAT_SERVER_HPP

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>

#include "mainwindow.hpp"


using boost::asio::ip::tcp;

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

private:
    void do_accept();

    tcp::acceptor acceptor_;
};

int async_tcp_echo_server();

#endif // ASYNC_TCP_ECHO_SERVER_HPP
