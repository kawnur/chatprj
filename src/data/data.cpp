#include "data.hpp"

#include "logging.hpp"

CompanionData::CompanionData(
    const std::string &name, const std::string &ipAddress, const std::string &serverPort,
    const std::string &clientPort)
    : name_(name), ipAddress_(ipAddress), serverPort_(serverPort), clientPort_(clientPort) {}

std::string CompanionData::name() const
{
    return name_;
}

std::string CompanionData::ipAddress() const
{
    return ipAddress_;
}

std::string CompanionData::serverPort() const
{
    return serverPort_;
}

std::string CompanionData::clientPort() const
{
    return clientPort_;
}

void CompanionData::setName(const std::string &name)
{
    name_ = name;
}

void CompanionData::setServerPort(uint16_t port)
{
    serverPort_ = getString(port);
}

void CompanionData::log()
{
    auto entry = "name: {0}, ipAddress: {1}, serverPort: {2}, clientPort: {3}"s;

    logTemplateInfo(entry, name_, ipAddress_, serverPort_, clientPort_);
}

GroupChatData::GroupChatData() : members_() {}

bool compareCompanionData(
    CompanionDataPtr data1, CompanionDataPtr data2)
{
    // do not check serverPort

    return (data1->name() == data2->name())
           && (data1->ipAddress() == data2->ipAddress())
           && (data1->clientPort() == data2->clientPort());
}