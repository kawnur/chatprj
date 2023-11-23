#include <boost/asio.hpp>
#include <iostream>

#include "utils_cout.hpp"
#include "utils_gpgme.hpp"


void timer1() {
//    boost::asio::io_context io;
//    boost::asio::steady_timer t(io, boost::asio::chrono::seconds(5));
//    t.wait();
    coutWithEndl("Test");

}



int main() {
    timer1();











    return 0;
}
