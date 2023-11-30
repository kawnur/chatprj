#include "tutorial_boost_asio.hpp"
#include "utils_cout.hpp"


void timer1() {
    boost::asio::io_context io;
    boost::asio::steady_timer t(io, boost::asio::chrono::seconds(1));
    t.wait();
    coutWithEndl("Test");
}

void func1(const boost::system::error_code& e) {
    coutWithEndl("Test from func1");
}

void timer2() {
    boost::asio::io_context io;
    boost::asio::steady_timer t(io, boost::asio::chrono::seconds(1));

    t.async_wait(&func1);
    io.run();
}

void func2(const boost::system::error_code& e,
    boost::asio::steady_timer* t, int* count) {

    if(*count < 5) {
        coutArgsWithSpaceSeparator("*count:", *count);
        ++(*count);

        t->expires_at(t->expiry() + boost::asio::chrono::seconds(1));

        t->async_wait(boost::bind(func2,
                  boost::asio::placeholders::error, t, count));
    }
}

void timer3() {
    boost::asio::io_context io;
    int count = 0;
    boost::asio::steady_timer t(io, boost::asio::chrono::seconds(1));

    t.async_wait(boost::bind(func2,
            boost::asio::placeholders::error, &t, &count));

    io.run();

    coutArgsWithSpaceSeparator("Final count is", count);
}

printer4::printer4(boost::asio::io_context& io)
    : timer_(io, boost::asio::chrono::seconds(1)),
      count_(0) {
    timer_.async_wait(boost::bind(&printer4::print, this));
}

printer4::~printer4() {
    coutArgsWithSpaceSeparator("Final count is", count_);
}

void printer4::print() {
    if(count_ < 5) {
        coutArgsWithSpaceSeparator("count_:", count_);
        ++count_;

        timer_.expires_at(timer_.expiry() + boost::asio::chrono::seconds(1));
        timer_.async_wait(boost::bind(&printer4::print, this));
    }
}

void timer4() {
    boost::asio::io_context io;
    printer4 p(io);
    io.run();
}

printer5::printer5(boost::asio::io_context& io)
    : strand_(boost::asio::make_strand(io)),
      timer1_(io, boost::asio::chrono::seconds(1)),
      timer2_(io, boost::asio::chrono::seconds(1)),
      count_(0) {

    timer1_.async_wait(boost::asio::bind_executor(strand_,
        boost::bind(&printer5::print1, this)));

    timer2_.async_wait(boost::asio::bind_executor(strand_,
        boost::bind(&printer5::print2, this)));
}

printer5::~printer5() {
    coutArgsWithSpaceSeparator("Final count is", count_);
}

void printer5::print1() {
    if(count_ < 10) {
        std::cout << "Timer 1: " << count_ << std::endl;
        ++count_;

        timer1_.expires_at(timer1_.expiry() + boost::asio::chrono::seconds(1));

        timer1_.async_wait(boost::asio::bind_executor(strand_,
            boost::bind(&printer5::print1, this)));
    }
}

void printer5::print2() {
    if(count_ < 10) {
        std::cout << "Timer 2: " << count_ << std::endl;
        ++count_;

        timer2_.expires_at(timer2_.expiry() + boost::asio::chrono::seconds(1));

        timer2_.async_wait(boost::asio::bind_executor(strand_,
        boost::bind(&printer5::print2, this)));
    }
}

void timer5() {
    boost::asio::io_context io;
    printer5 p(io);
    boost::thread t(boost::bind(&boost::asio::io_context::run, &io));
    io.run();
    t.join();
}

void run_asio_tutorials() {
//    timer1();
//    timer2();
//    timer3();
//    timer4();
    timer5();

}
