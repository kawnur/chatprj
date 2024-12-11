#include "utils.hpp"

bool validateCompanionName(std::vector<std::string>& validationErrors, const std::string& name)
{
    bool result = (name.size() <= 30);

    if(!result)
    {
        validationErrors.push_back(std::string("companion name length is greater than 30"));
    }

    logArgs("validateCompanionName result:", result);

    return result;
}

bool validateIpAddress(std::vector<std::string>& validationErrors, const std::string& ipAddress)
{
    QHostAddress address { QString::fromStdString(ipAddress) };

    bool result = !(address.isNull());

    if(!result)
    {
        validationErrors.push_back(std::string("companion ipaddress is invalid"));
    }

    logArgs("validateIpAddress result:", result);

    return result;
}

bool validatePort(std::vector<std::string>& validationErrors, const std::string& port)
{
    long long portNumber = std::stoll(port, nullptr, 10);

    bool result = (portNumber >= 0) && (portNumber <= 65535);

    if(!result)
    {
        validationErrors.push_back(std::string("port number must be greater than 0 and lower than 65536"));
    }

    logArgs("validatePort result:", result);

    return result;
}

bool validateCompanionData(
    std::vector<std::string>& validationErrors,
    const std::string& name,
    const std::string& ipAddress,
    const std::string& port)
{
    bool result = validateCompanionName(validationErrors, name) &&
        validateIpAddress(validationErrors, ipAddress) &&
        validatePort(validationErrors, port);

    logArgs("validateCompanionData result:", result);

    return result;
}
