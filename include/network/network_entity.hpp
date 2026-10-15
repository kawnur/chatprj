#ifndef NETWORK_ENTITY_HPP
#define NETWORK_ENTITY_HPP

#include <memory>

#include "chat_client.hpp"
#include "chat_server.hpp"

class CompanionData;

class SocketInfo
{
public:
    SocketInfo() = default;

    template<typename T, typename U>
    SocketInfo(T &&ipAddress, U &&port)
        : ipAddress_(std::forward<T>(ipAddress)), port_(std::forward<U>(port)) {}

    SocketInfo(const SocketInfo &object);
    SocketInfo(SocketInfo &&object) = default;
    ~SocketInfo() = default;

    std::string ipAddress() const;
    uint16_t port() const;

    void updateData(std::shared_ptr<CompanionData> data);

private:
    std::string ipAddress_;
    uint16_t port_;
};

class NetworkEntity
{
public:
    NetworkEntity(
        int companionId, std::shared_ptr<SocketInfo> clientSocket,
        std::shared_ptr<SocketInfo> serverSocket);

    ~NetworkEntity() = default;

    std::string ipAddress();
    uint16_t clientPort();
    uint16_t serverPort();

    bool start();
    bool startServer();
    bool createClient();
    bool connectClient();
    bool disconnectClient();

    bool sendJsonViaClient(const std::string &json);
    void updateClientSocketInfo(std::shared_ptr<CompanionData> data);

    int companionId() const;
    std::shared_ptr<SocketInfo> clientSocket() const;
    std::shared_ptr<SocketInfo> serverSocket() const;
    std::shared_ptr<ChatClient> client() const;
    std::shared_ptr<ChatServer> server() const;

private:
    int companionId_;
    std::shared_ptr<SocketInfo> clientSocket_;
    std::shared_ptr<SocketInfo> serverSocket_;
    std::shared_ptr<ChatClient> client_;
    std::shared_ptr<ChatServer> server_;
};

#endif // NETWORK_ENTITY_HPP
