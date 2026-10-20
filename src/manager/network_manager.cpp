#include "network_manager.hpp"

#include "companion.hpp"
#include "data.hpp"
#include "logging.hpp"
#include "manager.hpp"
#include "utils.hpp"

NetworkManager::NetworkManager() : manager_(nullptr), map_() {}

void NetworkManager::setManager(ManagerPtr manager)
{
    manager_ = manager;
}

std::pair<NetworkManagerMap::iterator, bool> NetworkManager::addToMap(
    int companionId, SocketInfoPtr clientSocket, SocketInfoPtr serverSocket)
{
    auto entity = std::make_shared<NetworkEntity>(
        shared_from_this(), companionId, clientSocket, serverSocket);

    auto result = map_.emplace(companionId, entity);

    // if (!result.second)
    //     logTemplateError("adding to network manager map error, companion id '{}'", companionId);

    return result;
}

NetworkEntityPtr NetworkManager::getValue(int id)
{
    return runAndReturnSharedPtr<NetworkEntity>([&]() { return map_.at(id); });
}

void NetworkManager::updateSocketInfo(int id, CompanionDataPtr data)
{
    runAndLogException([&]() { map_.at(id)->updateClientSocketInfo(data); });
}

CompanionDataPtr NetworkManager::getCompanionData(CompanionPtr companion)
{
    auto entity = getValue(companion->id());

    if (!entity)
        return nullptr;

    auto name = companion->name();
    auto ipAddress = entity->ipAddress();
    auto serverPort = getString(entity->serverPort());
    auto clientPort = getString(entity->clientPort());

    return std::make_shared<CompanionData>(name, ipAddress, serverPort, clientPort);
}

bool NetworkManager::connectClient(CompanionPtr companion)
{
    auto entity = getValue(companion->id());

    if (!entity)
        return false;

    return entity->connectClient();
}

bool NetworkManager::disconnectClient(CompanionPtr companion)
{
    auto entity = getValue(companion->id());

    if (!entity)
        return false;

    return entity->disconnectClient();
}

void NetworkManager::receiveMessage(int companionId, const std::string &json)
{
    manager_->receiveMessage(companionId, json);
}
