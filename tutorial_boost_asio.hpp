#ifndef TUTORIAL_BOOST_ASIO_H
#define TUTORIAL_BOOST_ASIO_H

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/thread/thread.hpp>
#include <functional>
#include <string>
#include <thread>

#include "async_tcp_echo_server.hpp"
#include "blocking_tcp_echo_client.hpp"
#include "blocking_tcp_echo_server.hpp"
#include "mainwindow.hpp"


class AsioTest : public QObject {

    Q_OBJECT

public:
    static AsioTest* instance();

    void setRole(QString);
//    void runTestFunc(QString);

private:
    QString* role_;

    AsioTest();
    ~AsioTest() = default;

    static AsioTest* _instance;

public slots:
    void runTest();
};

template<typename T> QString getQString(T&& parameter){
    return QString::fromStdString(std::to_string(parameter));
};

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

void ping();

void daytime1();

void run_asio_tutorials();

#endif // TUTORIAL_BOOST_ASIO_H
