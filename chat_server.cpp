#include "chat_server.hpp"

using boost::asio::ip::tcp;

void ServerSession::start() {
    do_read();
}

void ServerSession::do_read() {
    MainWindow* mainWindow = MainWindow::instance();

    mainWindow->addText("do_read");
    coutArgsWithSpaceSeparator("do_read");

    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        [this, self](boost::system::error_code ec, std::size_t length) {
            if (!ec) {
                MainWindow* mainWindow = MainWindow::instance();
                std::string str(data_, length);

                if(length > 0) {
                    mainWindow->addText("Got: " + QString::fromStdString(str));
                }

                coutArgsWithSpaceSeparator("Got:", str);

                do_write(length);
            }
        });
}

void ServerSession::do_write(std::size_t length) {
    MainWindow* mainWindow = MainWindow::instance();

    mainWindow->addText("do_write");
    coutArgsWithSpaceSeparator("do_write");

    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
        [this, self](boost::system::error_code ec, std::size_t /*length*/) {
            if (!ec) {
                do_read();
            }
        });
}

void ChatServer::do_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                std::make_shared<class session>(std::move(socket))->start();
            }

            do_accept();
        });
}

int async_tcp_echo_server()
{
  MainWindow* mainWindow = MainWindow::instance();

  try
  {

    mainWindow->addText("Echo server started");
    coutWithEndl("Echo server started");

    boost::asio::io_context io_context;

    class server s(io_context, std::atoi("5002"));

    io_context.run();
  }
  catch (std::exception& e)
  {
    mainWindow->addText("Exception: " + QString(e.what()));
    coutArgsWithSpaceSeparator("Exception:", e.what());
  }

  return 0;
}
