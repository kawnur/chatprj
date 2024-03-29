//https://live.boost.org/doc/libs/1_84_0/doc/html/boost_asio/example/cpp11/echo/async_tcp_echo_server.cpp

#ifndef ASYNC_TCP_ECHO_SERVER_HPP
#define ASYNC_TCP_ECHO_SERVER_HPP

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>

//#include "mainwindow.hpp"
#include "utils_cout.hpp"


using boost::asio::ip::tcp;

class session
  : public std::enable_shared_from_this<class session>
{
public:
  session(tcp::socket socket) : socket_(std::move(socket)) {}

  void start();

private:
  void do_read();

  void do_write(std::size_t);

  tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
};

class server
{
public:
  server(boost::asio::io_context& io_context, short port)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
  {
    do_accept();
  }

private:
  void do_accept();

  tcp::acceptor acceptor_;
};

int async_tcp_echo_server();

#endif // ASYNC_TCP_ECHO_SERVER_HPP
