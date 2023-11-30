#ifndef TUTORIAL_BOOST_ASIO_H
#define TUTORIAL_BOOST_ASIO_H

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/thread/thread.hpp>


void timer1();
void func1(const boost::system::error_code&);
void timer2();
void func2(const boost::system::error_code&,
    boost::asio::steady_timer*, int*);
void timer3();

class printer4 {
public:
    printer4(boost::asio::io_context&);
    ~printer4();
    void print();

    private:
        boost::asio::steady_timer timer_;
        int count_;
};

void timer4();

class printer5 {
    public:
        printer5(boost::asio::io_context& io);
        ~printer5();
        void print1();
        void print2();

    private:
        boost::asio::strand<boost::asio::io_context::executor_type> strand_;
        boost::asio::steady_timer timer1_;
        boost::asio::steady_timer timer2_;
        int count_;
};

void timer5();

void run_asio_tutorials();

#endif // TUTORIAL_BOOST_ASIO_H
