#ifndef DATA_HPP
#define DATA_HPP

#include <memory>
#include <string>
#include <vector>

#include "alias.hpp"
#include "logging.hpp"

class Companion;

class CompanionData
{
public:
    CompanionData(
        const std::string &name, const std::string &ipAddress, const std::string &serverPort,
        const std::string &clientPort);

    ~CompanionData() { logArgsInfo(Q_FUNC_INFO); }

    std::string name() const;
    std::string ipAddress() const;
    std::string serverPort() const;
    std::string clientPort() const;

    void setName(const std::string &name);
    void setServerPort(uint16_t port);

    void log();

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

bool compareCompanionData(
    CompanionDataPtr data1, CompanionDataPtr data2);

#endif // DATA_HPP
