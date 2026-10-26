#include "chat_client.hpp"

#include "logging.hpp"
#include "utils.hpp"

using boost::asio::ip::tcp;

ChatClient::ChatClient(std::string &&ipAddress, uint16_t &&port)
    : isConnected_(false), ipAddress_(ipAddress), port_(port), io_context_(), socket_(io_context_),
    resolver_(io_context_) {}

bool ChatClient::isConnected() const
{
    return isConnected_;
}

bool ChatClient::connect()
{
    auto lambda = [this]()
    {
        logTemplateInfo("client connects to '{0}' '{1}'", ipAddress_, port_);

        boost::asio::connect(socket_, resolver_.resolve(ipAddress_, std::to_string(port_)));

        logTemplateInfo("socket is open: {}", socket_.is_open());

        isConnected_ = true;
    };

    runAndLogException(lambda);

    return isConnected_;
}

bool ChatClient::disconnect()
{
    auto lambda = [this]()
    {
        logTemplateInfo("client disconnects from '{0}' '{1}'", ipAddress_, port_);

        socket_.close();

        logTemplateInfo("socket is open: {}", socket_.is_open());

        isConnected_ = false;
    };

    runAndLogException(lambda);

    return !isConnected_;
}

bool ChatClient::send(std::string text)
{
    bool sent = false;

    auto lambda = [&]()
    {
        auto written = boost::asio::write(socket_, boost::asio::buffer(text.data(), text.size()));
        sent = true;
    };

    runAndLogException(lambda);
    logTemplateInfo("client sent message: {}", text);

    return sent;
}
