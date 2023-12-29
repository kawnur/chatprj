#include "tutorial_boost_asio.hpp"
#include "utils_cout.hpp"

#include "icmp_header.hpp"
#include "ipv4_header.hpp"


template<> QString getQString<std::string>(std::string&& parameter){
    return QString::fromStdString(parameter);
};

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

using boost::asio::ip::icmp;
using boost::asio::deadline_timer;
namespace posix_time = boost::posix_time;

class pinger
{
public:
  pinger(boost::asio::io_service& io_service, const char* destination)
    : resolver_(io_service), socket_(io_service, icmp::v4()),
      timer_(io_service), sequence_number_(0), num_replies_(0)
  {
    icmp::resolver::query query(icmp::v4(), destination, "");
    destination_ = *resolver_.resolve(query);

    start_send();
    start_receive();
  }

private:
  void start_send()
  {
    std::string body("\"Hello!\" from Asio ping.");

    // Create an ICMP header for an echo request.
    icmp_header echo_request;
    echo_request.type(icmp_header::echo_request);
    echo_request.code(0);
    echo_request.identifier(get_identifier());
    echo_request.sequence_number(++sequence_number_);
    compute_checksum(echo_request, body.begin(), body.end());

    // Encode the request packet.
    boost::asio::streambuf request_buffer;
    std::ostream os(&request_buffer);
    os << echo_request << body;

    // Send the request.
    time_sent_ = posix_time::microsec_clock::universal_time();
    socket_.send_to(request_buffer.data(), destination_);

    // Wait up to five seconds for a reply.
    num_replies_ = 0;
    timer_.expires_at(time_sent_ + posix_time::seconds(5));
    timer_.async_wait(boost::bind(&pinger::handle_timeout, this));
  }

  void handle_timeout()
  {
    if (num_replies_ == 0)
      std::cout << "Request timed out" << std::endl;

    // Requests must be sent no less than one second apart.
    timer_.expires_at(time_sent_ + posix_time::seconds(1));
    timer_.async_wait(boost::bind(&pinger::start_send, this));
  }

  void start_receive()
  {
    // Discard any data already in the buffer.
    reply_buffer_.consume(reply_buffer_.size());

    // Wait for a reply. We prepare the buffer to receive up to 64KB.
    socket_.async_receive(reply_buffer_.prepare(65536),
        boost::bind(&pinger::handle_receive, this, std::placeholders::_2));
//        boost::bind(&pinger::handle_receive, 65536, std::placeholders::_1));
  }

  void handle_receive(std::size_t length)
  {
    // The actual number of bytes received is committed to the buffer so that we
    // can extract it using a std::istream object.
    reply_buffer_.commit(length);

    // Decode the reply packet.
    std::istream is(&reply_buffer_);
    ipv4_header ipv4_hdr;
    icmp_header icmp_hdr;
    is >> ipv4_hdr >> icmp_hdr;

    // We can receive all ICMP packets received by the host, so we need to
    // filter out only the echo replies that match the our identifier and
    // expected sequence number.
    if (is && icmp_hdr.type() == icmp_header::echo_reply
          && icmp_hdr.identifier() == get_identifier()
          && icmp_hdr.sequence_number() == sequence_number_)
    {
      // If this is the first reply, interrupt the five second timeout.
      if (num_replies_++ == 0)
        timer_.cancel();

      // Print out some information about the reply packet.
      posix_time::ptime now = posix_time::microsec_clock::universal_time();

      std::cout << length - ipv4_hdr.header_length()
        << " bytes from " << ipv4_hdr.source_address()
        << ": icmp_seq=" << icmp_hdr.sequence_number()
        << ", ttl=" << ipv4_hdr.time_to_live()
        << ", time=" << (now - time_sent_).total_milliseconds() << " ms"
        << std::endl;

//      auto getQString = [&](auto&& parameter){
//        return QString::fromStdString(std::to_string(parameter));
//      };

      QString text = getQString(length - ipv4_hdr.header_length())
              + " bytes from " + getQString(ipv4_hdr.source_address().to_string())
              + ": icmp_seq=" + getQString(icmp_hdr.sequence_number())
              + ", ttl=" + getQString(ipv4_hdr.time_to_live())
              + ", time=" + getQString((now - time_sent_).total_milliseconds())
              + " ms";

//      coutArgsWithSpaceSeparator("text:", text.toStdString());

      MainWindow* mainWindow = MainWindow::instance();
      mainWindow->addText(text);

//      std::this_thread::sleep_for(std::chrono::milliseconds(200));

    }

    start_receive();
  }

  static unsigned short get_identifier()
  {
#if defined(BOOST_WINDOWS)
    return static_cast<unsigned short>(::GetCurrentProcessId());
#else
    return static_cast<unsigned short>(::getpid());
#endif
  }

  icmp::resolver resolver_;
  icmp::endpoint destination_;
  icmp::socket socket_;
  deadline_timer timer_;
  unsigned short sequence_number_;
  posix_time::ptime time_sent_;
  boost::asio::streambuf reply_buffer_;
  std::size_t num_replies_;
};

void ping() {
    // https://live.boost.org/doc/libs/1_40_0/doc/html/boost_asio/example/icmp/ping.cpp

    // sudo setcap cap_net_raw,cap_net_admin=eip <path_to_executable>

    try {
        boost::asio::io_service io_service;
//        pinger p(io_service, argv[1]);
        pinger p(io_service, "8.8.8.8");
        io_service.run();
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

void daytime1() {
    // A synchronous TCP daytime client
    using tcp = boost::asio::ip::tcp;

    MainWindow* mainWindow = MainWindow::instance();
//    mainWindow->addText(text);

    try {
//        if(argc != 2) {
//            std::cerr << "Usage: client <host>" << std::endl;
//            return 1;
//        }

//        coutWithEndl("1");
//        std::string_view host { "time.google.com" };
        std::string_view host { "localhost" };
        QString hostRepr = QString("hostname: ") + host.data();
        mainWindow->addText(hostRepr);

//        coutWithEndl("2");
        boost::asio::io_context io_context;
//        coutWithEndl("3");
        tcp::resolver resolver(io_context);

//        coutWithEndl("4");
        tcp::resolver::results_type endpoints =
//                resolver.resolve(argv[1], "daytime");
                resolver.resolve(host, "daytime");

        for(auto it = endpoints.begin(); it != endpoints.end(); it++) {
            mainWindow->addText("Resolved endpoints:");
            mainWindow->addText(
                        QString("host_name:")
                        + QString::fromStdString(it->host_name()));
            mainWindow->addText(
                        QString("service_name:")
                        + QString::fromStdString(it->service_name()));
        }

//        for(auto& endpoint : endpoints) {
//            coutArgsWithSpaceSeparator("endpoint.host_name(): ", endpoint.host_name());
//            coutArgsWithSpaceSeparator("endpoint.service_name(): ", endpoint.service_name());
//        }

//        coutWithEndl("5");
        tcp::socket socket(io_context);

//        coutWithEndl("6");
        boost::asio::connect(socket, endpoints);

//        coutWithEndl("loop");

        for(;;) {
            boost::array<char, 128> buf;
            boost::system::error_code error;

            size_t len = socket.read_some(boost::asio::buffer(buf), error);

            if(error == boost::asio::error::eof) {
                break; // Connection closed cleanly by peer.
            }
            else if(error) {
                throw boost::system::system_error(error); // Some other error.
            }

            std::cout.write(buf.data(), len);

            QString reply;

            for(int i = 0; i < len; i++) {
                reply.push_back(buf.data()[i]);
            }

            mainWindow->addText(QString("Reply:\n" + reply));
        }
    }

    catch (std::exception& e) {
        coutWithEndl("exception");
        std::cerr << e.what() << std::endl;

        mainWindow->addText("exception");
        mainWindow->addText(e.what());
    }
}

std::string make_daytime_string()
{
  using namespace std; // For time_t, time and ctime;
  time_t now = time(0);

  auto result = ctime(&now);

  MainWindow* mainWindow = MainWindow::instance();
  mainWindow->addText(QString("daytime: ") + QString(result));

//  return ctime(&now);
  return result;
}

void daytime2() {
    // A synchronous TCP daytime server
    using boost::asio::ip::tcp;

    MainWindow* mainWindow = MainWindow::instance();
    mainWindow->addText("Server started");

    try
      {
        boost::asio::io_context io_context;

        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 5000));

        for (;;)
        {
          tcp::socket socket(io_context);
          acceptor.accept(socket);

          std::string message = make_daytime_string();

          boost::system::error_code ignored_error;
          boost::asio::write(socket, boost::asio::buffer(message), ignored_error);
        }
      }
      catch (std::exception& e)
      {
        std::cerr << e.what() << std::endl;

        mainWindow->addText("exception");
        mainWindow->addText(e.what());
      }

//      return 0;
}



//void run_asio_tutorials() {
//    timer1();
//    timer2();
//    timer3();
//    timer4();
//    timer5();
//    ping();
//    daytime1();
//}

AsioTest* AsioTest::_instance = nullptr;

AsioTest::AsioTest() {
    role_ = new QString;
}

void AsioTest::setRole(QString role) {
    *(this->role_) = role;
}

AsioTest* AsioTest::instance() {
    if(_instance == nullptr) {
        _instance = new AsioTest();
    }
    return _instance;
}

void AsioTest::runTest() {
//    coutWithEndl("AsioTest::runTest");
//    run_asio_tutorials();

    if(*(this->role_) == "Client") {
//        daytime1();
//        ping();

        MainWindow* mainWindow = MainWindow::instance();
        EchoClient* client = new EchoClient;
        mainWindow->setClient(client);
    }
    else if(*(this->role_) == "Server") {
//        daytime2();
//        ping();
//        blocking_tcp_echo_server();
        async_tcp_echo_server();
    }
}
