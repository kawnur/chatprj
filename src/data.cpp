#include "data.hpp"

CompanionData::CompanionData(
    const std::string& name, const std::string& ipAddress,
    const std::string& serverPort, const std::string& clientPort) :
    name_(name), ipAddress_(ipAddress),
    serverPort_(serverPort), clientPort_(clientPort) {}

std::string CompanionData::getName() const {
    return this->name_;
}

std::string CompanionData::getIpAddress() const {
    return this->ipAddress_;
}

std::string CompanionData::getServerPort() const {
    return this->serverPort_;
}

std::string CompanionData::getClientPort() const {
    return this->clientPort_;
}

GroupChatData::GroupChatData() : members_(std::vector<Companion*>()) {}
