// https://live.boost.org/doc/libs/1_83_0/doc/html/boost_asio/example/cpp11/echo/blocking_tcp_echo_server.cpp

#ifndef BLOCKING_TCP_ECHO_SERVER
#define BLOCKING_TCP_ECHO_SERVER

#include <cstdlib>
#include <iostream>
#include <thread>
#include <utility>
#include <boost/asio.hpp>

//#include "mainwindow.hpp"
#include "utils_cout.hpp"


using boost::asio::ip::tcp;

//const int max_length = 1024;

void session(tcp::socket);

void server(boost::asio::io_context&, unsigned short);

//int main(int argc, char* argv[])
int blocking_tcp_echo_server();

#endif
