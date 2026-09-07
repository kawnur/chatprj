#include "data.hpp"

CompanionData::CompanionData(
    const std::string& name, const std::string& ipAddress,
    const std::string& serverPort, const std::string& clientPort) :
    name_(name), ipAddress_(ipAddress),
    serverPort_(serverPort), clientPort_(clientPort) {}

std::string CompanionData::getName() const {
    return name_;
}

std::string CompanionData::getIpAddress() const {
    return ipAddress_;
}

std::string CompanionData::getServerPort() const {
    return serverPort_;
}

std::string CompanionData::getClientPort() const {
    return clientPort_;
}

GroupChatData::GroupChatData() : members_(std::vector<std::shared_ptr<Companion>>()) {}
