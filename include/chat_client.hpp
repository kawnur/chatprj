#ifndef CHAT_CLIENT
#define CHAT_CLIENT

#include <boost/asio.hpp>
#include <string>

using boost::asio::ip::tcp;

class ChatClient
{
public:
    ChatClient(std::string&&, uint16_t&&);
    ~ChatClient() = default;

    bool connect();
    bool disconnect();
    bool send(std::string);
    bool isConnected() const;

private:
    bool isConnected_;
    std::string ipAddress_;
    uint16_t port_;
    boost::asio::io_context io_context_;
    tcp::socket socket_;
    tcp::resolver resolver_;
};

#endif // CHAT_CLIENT
