#include "chat_server.hpp"

#include "logging.hpp"
#include "manager.hpp"

using boost::asio::ip::tcp;

ServerSession::ServerSession(std::shared_ptr<Companion> companion, tcp::socket socket)
    : companion_(companion), socket_(std::move(socket)) {}

void ServerSession::start()
{
    do_read();
}

void ServerSession::do_read()
{
    auto self(shared_from_this());

    auto lambda = [this, self](boost::system::error_code ec, std::size_t length)
    {
        if (ec)
            return;

        std::string str(data_, length);

        if (length > 0) {
            if (previous_.size() > 0)
                str = previous_ + str;

            // split messages
            int openCounter = 0;
            int closeCounter = 0;
            auto currentIterator = str.begin();

            for (auto iterator = str.begin(); iterator != str.end(); iterator++) {
                if (*iterator == '{')
                    openCounter++;

                if (*iterator == '}') {
                    closeCounter++;

                    if (openCounter == closeCounter) {
                        std::string message(currentIterator, iterator + 1);
                        currentIterator = iterator + 1;
                        openCounter = 0;
                        closeCounter = 0;
                        previous_ = "";

                        getManager()->receiveMessage(companion_, message);
                    }
                }
            }

            if (currentIterator != str.end())
                previous_ = std::string(currentIterator, str.end());
        }

        do_read();
    };

    socket_.async_read_some(boost::asio::buffer(data_, MAX_BUFFER_SIZE), lambda);
}

ChatServer::ChatServer(std::shared_ptr<Companion> companion, uint16_t port)
    : companion_(companion), port_(port), io_context_(),
    acceptor_(io_context_, tcp::endpoint(tcp::v4(), port))
{
    do_accept();
}

void ChatServer::run()
{
    runInDetachedThread([this](){ io_context_.run(); });
}

void ChatServer::do_accept()
{
    logArgs("ChatServer starts on", port_);

    auto lambda = [this](boost::system::error_code ec, tcp::socket socket)
    {
        if (!ec)
            std::make_shared<ServerSession>(companion_, std::move(socket))->start();

        do_accept();
    };

    acceptor_.async_accept(lambda);
}
