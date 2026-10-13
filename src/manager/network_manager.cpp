#include "network_manager.hpp"

#include "logging.hpp"
#include "utils.hpp"

NetworkManager::NetworkManager() : map_() {}

std::pair<NetworkManagerMap::iterator, bool> NetworkManager::addToMap(
    int companionId, std::shared_ptr<SocketInfo> clientSocket,
    std::shared_ptr<SocketInfo> serverSocket)
{
    auto entity = std::make_shared<NetworkEntity>(companionId, clientSocket, serverSocket);

    auto result = map_.emplace(companionId, entity);

    // if (!result.second)
    //     logTemplateError("adding to network manager map error, companion id '{}'", companionId);

    return result;
}

std::shared_ptr<NetworkEntity> NetworkManager::getValue(int id)
{
    return runAndReturnSharedPtr<NetworkEntity>([&]() { return map_.at(id); });
}
