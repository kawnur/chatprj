#ifndef COMPANION_HPP
#define COMPANION_HPP

#include <cstdint>
#include <mutex>
#include <string>
#include <utility>

#include "constants.hpp"
#include "data.hpp"
#include "logging.hpp"
#include "message.hpp"

class ChatClient;
class ChatServer;
class CompanionData;
class Message;

class SocketInfo
{
public:
    SocketInfo() {}
    SocketInfo(std::string&, uint16_t&, uint16_t&);  // TODO remove
    SocketInfo(std::string&&, uint16_t&&, uint16_t&&);  // TODO remove

    template<typename T, typename U, typename P> SocketInfo
        (T&& ipAddress, U&& serverPort, P&& clientPort) :
        ipAddress_(std::forward<T>(ipAddress)),
        serverPort_(std::forward<U>(serverPort)),
        clientPort_(std::forward<P>(clientPort)) {}

    SocketInfo(const SocketInfo&);
    SocketInfo(SocketInfo&&) {}
    ~SocketInfo() = default;

    std::string getIpAddress() const;
    uint16_t getServerPort() const;
    uint16_t getClientPort() const;

    void updateData(const CompanionData*);

private:
    std::string ipAddress_;
    uint16_t serverPort_;  // port number to open server at
    uint16_t clientPort_;  // port number to connect with client to
};

class Companion
{
public:
    Companion(int, const std::string&);
    Companion(int, std::string&&);
    ~Companion();

    int getId();
    std::string getName() const;
    SocketInfo* getSocketInfoPtr() const;
    std::string getSocketIpAddress() const;
    uint16_t getSocketServerPort() const;
    uint16_t getSocketClientPort() const;
    const std::vector<Message*>* getMessagePointersPtr() const;

    void setSocketInfo(SocketInfo*);

    bool startServer();
    bool createClient();
    bool connectClient();
    bool disconnectClient();
    void addMessage(Message*);
    bool sendMessage(bool, NetworkMessageType, std::string, const Message*);
    void updateData(const CompanionData*);
    Message* findMessage(uint32_t);

private:
    std::mutex messagesMutex_;
    int id_;  // TODO change type
    std::string name_;
    SocketInfo* socketInfoPtr_;
    ChatClient* clientPtr_;
    ChatServer* serverPtr_;
    std::vector<Message*>* messagePointersPtr_;
};

#endif // COMPANION_HPP
