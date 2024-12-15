#include "data.hpp"

CompanionData::CompanionData(
    const std::string& name, const std::string& ipAddress,
    const std::string& serverPort, const std::string& clientPort) :
    name_(name), ipAddress_(ipAddress),
    serverPort_(serverPort), clientPort_(clientPort) {}

std::string CompanionData::getName()
{
    return this->name_;
}

std::string CompanionData::getIpAddress()
{
    return this->ipAddress_;
}

std::string CompanionData::getServerPort()
{
    return this->serverPort_;
}

std::string CompanionData::getClientPort()
{
    return this->clientPort_;
}
