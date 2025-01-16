#include "chat_client.hpp"

using boost::asio::ip::tcp;

ChatClient::ChatClient(std::string&& ipAddress, uint16_t&& port) :
    isConnected_(false), ipAddress_(ipAddress), port_(port), io_context_(),
    socket_(io_context_), resolver_(io_context_)
{
    // tcp::no_delay option(true);
    // socket_.set_option(option);
}

bool ChatClient::connect()
{
    auto connectLambda = [this]()
    {
        logArgs("ChatClient connects to", this->ipAddress_, this->port_);

        boost::asio::connect(
            this->socket_,
            this->resolver_.resolve(this->ipAddress_, std::to_string(this->port_))
        );

        logArgs("socket_.is_open():", this->socket_.is_open());

        this->isConnected_ = true;
    };

    runAndLogException(connectLambda);

    return this->isConnected_;
}

bool ChatClient::disconnect()
{
    auto disconnectLambda = [this]()
    {
        logArgs("ChatClient disconnects from", this->ipAddress_, this->port_);

        // boost::asio::io_context io_context_;
        // tcp::socket socket_ { io_context_ };
        // tcp::resolver resolver_ { io_context_ };
        //        boost::asio::connect(s, resolver.resolve("localhost", "5002"));
        //        boost::asio::connect(s, resolver.resolve("0.0.0.0", "5002"));
        //        boost::asio::connect(s, resolver.resolve("172.21.0.3", "5002"));
        // boost::asio::connect(s, resolver.resolve("chatprj-server-1", "5002"));
        //        boost::asio::connect(s, resolver.resolve("host.docker.internal", "5002"));

        this->socket_.close();

        logArgs("socket_.is_open():", socket_.is_open());

        this->isConnected_ = false;
    };

    runAndLogException(disconnectLambda);

    return !this->isConnected_;
}

bool ChatClient::send(std::string text)
{
    bool sent = false;

    auto sendLambda = [&, this](bool& value)
    {
        // char request[max_length] = { 0 };

        // logArgs("request:", text);

        // auto textStdString = text.toStdString();
        // size_t request_length = textStdString.size();
        // std::strcpy(request, textStdString.data());
        // std::strcpy(request, text.data());

        // auto completionConditionLambda =
        //     [&](boost::system::error_code& ec, size_t size){ return text.size(); };

        // boost::asio::write(s, boost::asio::buffer(request, request_length));
        // boost::asio::write(this->socket_, boost::asio::buffer(request, text.size()));

        boost::asio::write(
            this->socket_, boost::asio::buffer(text.data(), text.size()));
        // boost::asio::write(
        //     this->socket_,
        //     boost::asio::buffer(text.data(), text.size()),
        //     completionConditionLambda);

        // char reply[max_length] = { 0 };
        // // size_t reply_length = boost::asio::read(s,
        // //                                         boost::asio::buffer(reply, request_length));
        // size_t reply_length = boost::asio::read(
        //     this->socket_,
        //     boost::asio::buffer(reply, text.size()));

        // std::string str(reply, reply_length);
        // logArgs("reply:", str);
        // logArgs("Enter message:");

        value = true;
    };

    runAndLogException(sendLambda, sent);
    logArgs("client sent message:", text);

    return sent;
}

bool ChatClient::getIsConnected() const
{
    return this->isConnected_;
}
