#ifndef DATA_HPP
#define DATA_HPP

#include <string>
#include <vector>
#include <unordered_map>

#include "alias.hpp"
#include "logging.hpp"

using DataMap = std::unordered_map<DataType, std::string>;

class DataWrapper
{
public:
    DataWrapper();
    DataWrapper(DataMap map);
    ~DataWrapper() { logArgsInfo(Q_FUNC_INFO); }

    std::string get(DataType type);
    void set(DataType type, const std::string &value);

    void log();

private:
    DataMap map_;
};

class CompanionData
{
public:
    template<typename T1, typename T2, typename T3, typename T4>
    CompanionData(
        T1 &&name, T2 &&ipAddress, T3 &&serverPort, T4 &&clientPort)
        : name_(getString(std::forward<T1>(name))),
        ipAddress_(getString(std::forward<T2>(ipAddress))),
        serverPort_(getString(std::forward<T3>(serverPort))),
        clientPort_(getString(std::forward<T4>(clientPort))) {}

    ~CompanionData() { logArgsInfo(Q_FUNC_INFO); }

    std::string name() const;
    std::string ipAddress() const;
    std::string serverPort() const;
    std::string clientPort() const;

    void setName(const std::string &name);
    void setServerPort(uint16_t port);

    void log();

    std::string getData(DataType type);
    bool checkAsUpdateResult();

private:
    std::string name_;
    std::string ipAddress_;
    std::string serverPort_;
    std::string clientPort_;
};

class GroupChatData {
public:
    GroupChatData();
    ~GroupChatData() = default;

private:
    std::vector<CompanionPtr> members_;
};

bool compareCompanionData(CompanionDataPtr data1, CompanionDataPtr data2);

#endif // DATA_HPP
