#ifndef DATA_HPP
#define DATA_HPP

#include <string>

class CompanionData
{
public:
    CompanionData(const std::string&, const std::string&, const std::string&, const std::string&);
    ~CompanionData() = default;

    std::string getName();
    std::string getIpAddress();
    std::string getServerPort();
    std::string getClientPort();

private:
    std::string name_;
    std::string ipAddress_;
    std::string serverPort_;
    std::string clientPort_;
};

#endif // DATA_HPP
