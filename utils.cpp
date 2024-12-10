#include "utils.hpp"

bool validateCompanionName(const std::string& name)
{
    bool result = (name.size() <= 30);

    logArgs("validateCompanionName result:", result);

    return result;
}

bool validateIpAddress(const std::string& ipAddress)
{
    QHostAddress address { QString::fromStdString(ipAddress) };

    bool result = !(address.isNull());

    logArgs("validateIpAddress result:", result);

    return result;
}

bool validatePort(const std::string& port)
{
    long long portNumber = std::stoll(port, nullptr, 10);

    bool result = (portNumber >= 0) && (portNumber <= 65535);

    logArgs("validatePort result:", result);

    return result;
}

bool validateCompanionData(
    const std::string& name,
    const std::string& ipAddress,
    const std::string& port)
{
    bool result = validateCompanionName(name) &&
        validateIpAddress(ipAddress) &&
        validatePort(port);

    logArgs("validateCompanionData result:", result);

    return result;
}
