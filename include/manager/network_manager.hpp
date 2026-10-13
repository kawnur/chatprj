#ifndef NETWORK_MANAGER_HPP
#define NETWORK_MANAGER_HPP

#include <memory>
#include <unordered_map>

#include "network_entity.hpp"

using NetworkManagerMap = std::unordered_map<int, std::shared_ptr<NetworkEntity>>;

class NetworkManager
{
public:
    NetworkManager();
    ~NetworkManager() = default;

    std::pair<NetworkManagerMap::iterator, bool> addToMap(
        int companionId, std::shared_ptr<SocketInfo> clientSocket,
        std::shared_ptr<SocketInfo> serverSocket);

    std::shared_ptr<NetworkEntity> getValue(int id);

private:
    NetworkManagerMap map_;
};

#endif // NETWORK_MANAGER_HPP
