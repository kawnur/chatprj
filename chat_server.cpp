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

    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        [this, self](boost::system::error_code ec, std::size_t length)
        {
            if (!ec)
            {
                std::string str(data_, length);

                if(length > 0)
                {
                    logArgs("server got message:", str);

                    Manager* managerPtr = getManagerPtr();
                    managerPtr->receiveMessage(this->companionPtr_, str);
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

//ChatServer::ChatServer(short port) {
//    boost::asio::io_context io_context_;
//    tcp::acceptor acceptor_(io_context_, tcp::endpoint(tcp::v4(), port));

//    do_accept();
//}

void ChatServer::run()
{
    // this->io_context_.run();
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
//                std::make_shared<class session>(std::move(socket))->start();
                std::make_shared<ServerSession>(this->companionPtr_, std::move(socket))->start();
            }

            do_accept();
        }
    );
}

//int async_tcp_echo_server()
//{
//  try
//  {

//    coutWithEndl("Echo server started");

//    boost::asio::io_context io_context;

//    class server s(io_context, std::atoi("5002"));

//    io_context.run();
//  }
//  catch (std::exception& e)
//  {
//    coutArgsWithSpaceSeparator("Exception:", e.what());
//  }

//  return 0;
//}
