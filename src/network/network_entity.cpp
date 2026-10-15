#include "network_entity.hpp"

#include "data.hpp"
#include "utils.hpp"

SocketInfo::SocketInfo(const SocketInfo &object)
{
    ipAddress_ = object.ipAddress_;
    port_ = object.port_;
}

std::string SocketInfo::ipAddress() const
{
    return ipAddress_;
}

uint16_t SocketInfo::port() const
{
    return port_;
}

void SocketInfo::updateData(std::shared_ptr<CompanionData> data)
{
    ipAddress_ = data->ipAddress();
    port_ = getIntFromString(PORT_BAD_VALUE, data->clientPort());
}

NetworkEntity::NetworkEntity(
    int companionId, std::shared_ptr<SocketInfo> clientSocket,
    std::shared_ptr<SocketInfo> serverSocket)
    : companionId_(companionId), clientSocket_(clientSocket), serverSocket_(serverSocket),
    client_(nullptr), server_(nullptr) {}

std::string NetworkEntity::ipAddress()
{
    return clientSocket_->ipAddress();
}

uint16_t NetworkEntity::clientPort()
{
    return clientSocket_->port();
}

uint16_t NetworkEntity::serverPort()
{
    return serverSocket_->port();
}

bool NetworkEntity::start()
{
    bool result = true;

    if (!startServer()) {
        logArgsError("problem with server start for companion id", companionId_);

        result = false;
    }

    if (!createClient()) {
        logArgsError("problem with client creation for companion id", companionId_);

        result = false;
    }

    return result;
}

bool NetworkEntity::startServer()
{
    bool started = false;

    auto lambda = [&]()
    {
        server_ = std::make_shared<ChatServer>(companionId_, serverSocket_->port());
        server_->run();
        started = true;
    };

    runAndLogException(lambda);

    return started;
}

bool NetworkEntity::createClient()
{
    bool created = false;

    auto lambda = [&]()
    {
        client_ = std::make_shared<ChatClient>(clientSocket_->ipAddress(), clientSocket_->port());
        created = true;
    };

    runAndLogException(lambda);

    return created;
}

bool NetworkEntity::connectClient()
{
    return client_->connect();
}

bool NetworkEntity::disconnectClient()
{
    return client_->disconnect();
}

bool NetworkEntity::sendJsonViaClient(const std::string &json)
{
    // check client
    if (!client_ || !client_->isConnected())
        return false;

    auto result = client_->send(json);

    if (!result)
        logArgsError("client message sending error");

    return result;
}

void NetworkEntity::updateClientSocketInfo(std::shared_ptr<CompanionData> data)
{
    clientSocket_->updateData(data);
}

int NetworkEntity::companionId() const
{
    return companionId_;
}

std::shared_ptr<SocketInfo> NetworkEntity::clientSocket() const
{
    return clientSocket_;
}

std::shared_ptr<SocketInfo> NetworkEntity::serverSocket() const
{
    return serverSocket_;
}

std::shared_ptr<ChatClient> NetworkEntity::client() const
{
    return client_;
}

std::shared_ptr<ChatServer> NetworkEntity::server() const
{
    return server_;
}
