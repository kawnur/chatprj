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
    bool result = false;

    std::string errorMessage("port number must be greater than 0 and lower than 65536");

    try
    {
        long long portNumber = std::stoll(port, nullptr, 10);

        result = (portNumber >= 0) && (portNumber <= 65535);

        if(!result)
        {
            validationErrors.push_back(errorMessage);
        }
    }
    catch(std::out_of_range)
    {
        validationErrors.push_back(
            errorMessage + std::string(", port number is too big, std::out_of_range"));
    }
    catch(std::invalid_argument)
    {
        validationErrors.push_back(
            errorMessage + std::string(", port number is invalid, std::invalid_argument"));
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
    bool nameValidationResult = validateCompanionName(validationErrors, name);
    bool ipAddressValidationResult = validateIpAddress(validationErrors, ipAddress);
    bool portValidationResult = validatePort(validationErrors, port);

    bool result = nameValidationResult && ipAddressValidationResult && portValidationResult;

    logArgs("validateCompanionData result:", result);

    return result;
}

std::string buildDialogText(std::string&& header, const std::vector<std::string>& messages)
{
    if(messages.empty())
    {
        return std::string("");
    }
    else
    {
        std::string text(header);

        logArgs("messages.size():", messages.size());
        // std::string text { "Error messages:\n\n" };

        for(auto& message : messages)
        {
            text += (std::string("- ") + message + std::string("\n"));
        }

        return text;
    }
}

void showErrorDialogAndLogError(const std::string& message)
{
    getGraphicManagerPtr()->createDialog(nullptr, DialogType::ERROR, message);
    logArgsError(message);
}
