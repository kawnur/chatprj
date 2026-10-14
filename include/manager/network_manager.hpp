#ifndef NETWORK_MANAGER_HPP
#define NETWORK_MANAGER_HPP

#include <memory>
#include <unordered_map>

#include "network_entity.hpp"

class Companion;
class CompanionData;

using NetworkManagerMap = std::unordered_map<int, std::shared_ptr<NetworkEntity>>;

using CompanionPtr = std::shared_ptr<Companion>;

class NetworkManager
{
public:
    NetworkManager();
    ~NetworkManager() = default;

    std::pair<NetworkManagerMap::iterator, bool> addToMap(
        int companionId, std::shared_ptr<SocketInfo> clientSocket,
        std::shared_ptr<SocketInfo> serverSocket);

    std::shared_ptr<NetworkEntity> getValue(int id);
    void updateSocketInfo(int id, std::shared_ptr<CompanionData> data);
    std::shared_ptr<CompanionData> getCompanionData(CompanionPtr companion);

    bool connectClient(CompanionPtr companion);
    bool disconnectClient(CompanionPtr companion);

private:
    NetworkManagerMap map_;
};

#endif // NETWORK_MANAGER_HPP
