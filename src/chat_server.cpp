#include "chat_server.hpp"

using boost::asio::ip::tcp;

void ServerSession::start()
{
    do_read();
}

void ServerSession::do_read()
{
    logArgs("do_read");

    auto self(shared_from_this());

    socket_.async_read_some(boost::asio::buffer(data_, maxBufferSize),
        [this, self](boost::system::error_code ec, std::size_t length)
        {
            if (!ec)
            {
                std::string str(data_, length);

                if(length > 0)
                {
                    logArgs("server got message:", str);

                    // split messages
                    int openCounter = 0;
                    int closeCounter = 0;
                    auto currentIterator = str.begin();

                    for(auto iterator = str.begin(); iterator != str.end(); iterator++)
                    {
                        if(*iterator == '{')
                        {
                            openCounter++;
                        }

                        if(*iterator == '}')
                        {
                            closeCounter++;

                            if(openCounter == closeCounter)
                            {
                                std::string message(currentIterator, iterator + 1);
                                currentIterator = iterator + 1;
                                openCounter = 0;
                                closeCounter = 0;

                                getManagerPtr()->receiveMessage(this->companionPtr_, message);
                            }
                        }
                    }
                }                

                do_write(length);
            }
        }
    );
}

void ServerSession::do_write(std::size_t length)
{
    logArgs("do_write");

    auto self(shared_from_this());

    boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
            if (!ec)
            {
                do_read();
            }
        }
    );
}

void ChatServer::run()
{
    std::thread([this](){ this->io_context_.run(); }).detach();
}

void ChatServer::do_accept()
{
    logArgs("ChatServer starts on", port_);

    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket)
        {
            if(!ec)
            {
                std::make_shared<ServerSession>(
                    this->companionPtr_, std::move(socket))->start();
            }

            do_accept();
        }
    );
}
