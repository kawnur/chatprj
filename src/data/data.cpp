#include "data.hpp"

#include "logging.hpp"

DataWrapper::DataWrapper() : map_() {}

DataWrapper::DataWrapper(DataMap map) :map_(map) {}

std::string DataWrapper::get(DataType type)
{
    return getMapValue(map_, type, ""s);
}

void DataWrapper::set(DataType type, const std::string &value)
{
    map_[type] = value;
}

void DataWrapper::log()
{
    auto entry = ""s;

    for (const auto &pair : map_)
        entry += getStringByFormat("{0}: {1}, ");

    entry.erase(entry.end() - 2, entry.end() - 1);

    logArgsInfo(DATA_LOG_DELIMITER);
    logArgsInfo(entry);
    logArgsInfo(DATA_LOG_DELIMITER);
}

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

std::string CompanionData::getData(DataType type)
{
    switch(type) {
    case DataType::NAME:
        return name_;
    case DataType::IP_ADDRESS:
        return ipAddress_;
    case DataType::SERVER_PORT:
        return serverPort_;
    case DataType::CLIENT_PORT:
        return clientPort_;
    default:
        return ""s;
    }
}

bool CompanionData::checkAsUpdateResult()
{
    for (const auto &item : { name_, ipAddress_, clientPort_ }) {
        if (item.empty())
            return false;
    }

    return true;
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
