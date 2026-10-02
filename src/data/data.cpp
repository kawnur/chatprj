#include "data.hpp"

#include "logging.hpp"

CompanionData::CompanionData(
    const std::string &name, const std::string &ipAddress, const std::string &serverPort,
    const std::string &clientPort)
    : name_(name), ipAddress_(ipAddress), serverPort_(serverPort), clientPort_(clientPort) {}

std::string CompanionData::getName() const
{
    return name_;
}

std::string CompanionData::getIpAddress() const
{
    return ipAddress_;
}

std::string CompanionData::getServerPort() const
{
    return serverPort_;
}

std::string CompanionData::getClientPort() const
{
    return clientPort_;
}

void CompanionData::setServerPort(uint16_t port)
{
    serverPort_ = getString(port);
}

void CompanionData::log()
{
    std::string entry { "name: {0}, ipAddress: {1}, serverPort: {2}, clientPort: {3}" };

    logTemplateInfo(entry, name_, ipAddress_, serverPort_, clientPort_);
}

GroupChatData::GroupChatData() : members_() {}
