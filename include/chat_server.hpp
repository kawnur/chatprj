#ifndef CHAT_SERVER_HPP
#define CHAT_SERVER_HPP

#include <boost/asio.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>

#include "logging.hpp"
#include "mainwindow.hpp"
#include "manager.hpp"

using boost::asio::ip::tcp;

class Companion;

class ServerSession
        : public std::enable_shared_from_this<class ServerSession> {
public:
    ServerSession(std::shared_ptr<Companion> companion, tcp::socket socket)
        : companion_(companion), socket_(std::move(socket)) {}

    void start();

private:
    std::shared_ptr<Companion> companion_;
    tcp::socket socket_;
    char data_[maxBufferSize];
    std::string previous_;

    void do_read();
};

class ChatServer {
public:
    // TODO what if port is blocked?
    ChatServer(std::shared_ptr<Companion> companion, uint16_t port)
        : companion_(companion), port_(port), io_context_(),
        acceptor_(io_context_, tcp::endpoint(tcp::v4(), port)) {

        do_accept();
    }
    ~ChatServer() = default;

    void run();

private:
    boost::asio::io_context io_context_;
    uint16_t port_;
    std::shared_ptr<Companion> companion_;
    tcp::acceptor acceptor_;

    void do_accept();
};

int async_tcp_echo_server();

#endif // CHAT_SERVER_HPP
