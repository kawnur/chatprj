#ifndef NETWORK_MANAGER_HPP
#define NETWORK_MANAGER_HPP

#include <memory>
#include <unordered_map>

#include "alias.hpp"
#include "network_entity.hpp"

using NetworkManagerMap = std::unordered_map<int, std::shared_ptr<NetworkEntity>>;

class NetworkManager
{
public:
    NetworkManager();
    ~NetworkManager() = default;

    std::pair<NetworkManagerMap::iterator, bool> addToMap(
        int companionId, SocketInfoPtr clientSocket,
        SocketInfoPtr serverSocket);

    std::shared_ptr<NetworkEntity> getValue(int id);
    void updateSocketInfo(int id, CompanionDataPtr data);
    CompanionDataPtr getCompanionData(CompanionPtr companion);

    bool connectClient(CompanionPtr companion);
    bool disconnectClient(CompanionPtr companion);

private:
    NetworkManagerMap map_;
};

#endif // NETWORK_MANAGER_HPP
