#ifndef DATA_HPP
#define DATA_HPP

#include <string>

#include "companion.hpp"

class Companion;

class CompanionData {
public:
    CompanionData(
        const std::string&, const std::string&,
        const std::string&, const std::string&);

    ~CompanionData() = default;

    std::string getName() const;
    std::string getIpAddress() const;
    std::string getServerPort() const;
    std::string getClientPort() const;

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
    std::vector<Companion*> members_;
};

#endif // DATA_HPP
