// https://live.boost.org/doc/libs/1_83_0/doc/html/boost_asio/example/cpp11/echo/blocking_tcp_echo_server.cpp

#include "mainwindow.hpp"

using boost::asio::ip::tcp;

//const int max_length = 1024;

void session(tcp::socket sock)
{
  const int max_length = 1024;

  MainWindow* mainWindow = MainWindow::instance();
  mainWindow->addText("session");
  coutArgsWithSpaceSeparator("session");

  try
  {
    for (;;)
    {
      mainWindow->addText("loop step");
      coutArgsWithSpaceSeparator("loop step");

      char data[max_length];

      boost::system::error_code error;
      size_t length = sock.read_some(boost::asio::buffer(data), error);

      mainWindow->addText("Got: ");
//      coutArgsWithSpaceSeparator("Got: ");

      std::string str(data, length);
      mainWindow->addText(QString::fromStdString(str));
      coutArgsWithSpaceSeparator("Got:", str);

      if (error == boost::asio::error::eof)
        break; // Connection closed cleanly by peer.
      else if (error)
        throw boost::system::system_error(error); // Some other error.

      boost::asio::write(sock, boost::asio::buffer(data, length));
    }
  }
  catch (std::exception& e)
  {
//    std::cerr << "Exception in thread: " << e.what() << "\n";
    mainWindow->addText("Exception: " + QString(e.what()));
    coutArgsWithSpaceSeparator("Exception:", e.what());
  }
}

void server(boost::asio::io_context& io_context, unsigned short port)
{
  MainWindow* mainWindow = MainWindow::instance();

  mainWindow->addText("server");
  coutArgsWithSpaceSeparator("server");

  tcp::acceptor a(io_context, tcp::endpoint(tcp::v4(), port));
  for (;;)
  {
    mainWindow->addText("server thread");
    coutArgsWithSpaceSeparator("server thread");

    std::thread(session, a.accept()).detach();
  }
}

//int main(int argc, char* argv[])
int blocking_tcp_echo_server()
{
  MainWindow* mainWindow = MainWindow::instance();

  try
  {
    mainWindow->addText("Echo server started");
    coutWithEndl("Echo server started");
//    if (argc != 2)
//    {
//      std::cerr << "Usage: blocking_tcp_echo_server <port>\n";
//      return 1;
//    }

    boost::asio::io_context io_context;

    server(io_context, std::atoi("5002"));
  }
  catch (std::exception& e)
  {
//    std::cerr << "Exception: " << e.what() << "\n";
    mainWindow->addText("Exception: " + QString(e.what()));
    coutArgsWithSpaceSeparator("Exception:", e.what());
  }

  return 0;
}
