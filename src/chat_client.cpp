#include "chat_client.hpp"

using boost::asio::ip::tcp;

ChatClient::ChatClient(std::string&& ipAddress, uint16_t&& port) :
    isConnected_(false), ipAddress_(ipAddress), port_(port), io_context_(),
    socket_(io_context_), resolver_(io_context_) {}

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
        std::size_t written = boost::asio::write(
            this->socket_, boost::asio::buffer(text.data(), text.size()));

        value = true;
    };

    runAndLogException(sendLambda, sent);
    // logArgs("client sent message:", text);

    return sent;
}

bool ChatClient::getIsConnected() const
{
    return this->isConnected_;
}
