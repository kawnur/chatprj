#ifndef CHAT_SERVER_HPP
#define CHAT_SERVER_HPP

#include <memory>
#include <string>

#include <boost/asio.hpp>

#include "constants.hpp"

using boost::asio::ip::tcp;

class Companion;

class ServerSession : public std::enable_shared_from_this<class ServerSession>
{
public:
    ServerSession(std::shared_ptr<Companion> companion, tcp::socket socket);

    void start();

private:
    std::shared_ptr<Companion> companion_;
    tcp::socket socket_;
    char data_[MAX_BUFFER_SIZE];
    std::string previous_;

    void do_read();
};

class ChatServer
{
public:
    // TODO what if port is blocked?
    ChatServer(std::shared_ptr<Companion> companion, uint16_t port);
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
