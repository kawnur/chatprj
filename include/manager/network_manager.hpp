#ifndef NETWORK_MANAGER_HPP
#define NETWORK_MANAGER_HPP

#include <memory>
#include <unordered_map>

#include "alias.hpp"
#include "network_entity.hpp"

using NetworkManagerMap = std::unordered_map<int, NetworkEntityPtr>;

class NetworkManager : public std::enable_shared_from_this<NetworkManager>
{
public:
    NetworkManager();
    ~NetworkManager() = default;

    void setManager(ManagerPtr manager);

    std::pair<NetworkManagerMap::iterator, bool> addToMap(
        int companionId, SocketInfoPtr clientSocket, SocketInfoPtr serverSocket);

    NetworkEntityPtr getNetworkEntity(int id);
    void updateSocketInfo(int id, CompanionDataPtr data);
    CompanionDataPtr getCompanionData(int companionId);
    bool connectClient(int companionId);
    bool disconnectClient(int companionId);
    void receiveMessage(int companionId, const std::string &json);
    bool sendJson(int companionId, const std::string &json);

private:
    ManagerPtr manager_;
    NetworkManagerMap map_;
};

#endif // NETWORK_MANAGER_HPP
