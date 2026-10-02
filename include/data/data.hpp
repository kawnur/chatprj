#ifndef DATA_HPP
#define DATA_HPP

#include <memory>
#include <string>
#include <vector>

#include "logging.hpp"

class Companion;

class CompanionData
{
public:
    CompanionData(
        const std::string &name, const std::string &ipAddress, const std::string &serverPort,
        const std::string &clientPort);

    ~CompanionData() { logArgsInfo(Q_FUNC_INFO); }

    std::string getName() const;
    std::string getIpAddress() const;
    std::string getServerPort() const;
    std::string getClientPort() const;

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
    std::vector<std::shared_ptr<Companion>> members_;
};

bool compareCompanionData(
    std::shared_ptr<CompanionData> data1, std::shared_ptr<CompanionData> data2);

#endif // DATA_HPP
