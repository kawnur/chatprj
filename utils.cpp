#include "utils.hpp"

// TODO move all constants to constants.hpp

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
    const CompanionAction* companionActionPtr)
{
    bool nameValidationResult = validateCompanionName(validationErrors, companionActionPtr->getName());
    bool ipAddressValidationResult = validateIpAddress(validationErrors, companionActionPtr->getIpAddress());
    bool portValidationResult = validatePort(validationErrors, companionActionPtr->getClientPort());

    bool result = nameValidationResult && ipAddressValidationResult && portValidationResult;

    logArgs("validateCompanionData result:", result);

    return result;
}

bool validatePassword(std::vector<std::string>& validationErrors, const std::string& password)
{
    bool result = (password.size() <= 30);

    if(!result)
    {
        validationErrors.push_back(std::string("password length is greater than 30"));
    }

    logArgs("validatePassword result:", result);

    return result;
}

QString buildDialogText(std::string&& header, const std::vector<std::string>& messages)
{
    if(messages.empty())
    {
        return QString("");
    }
    else
    {
        std::string text(header);

        logArgs("messages.size():", messages.size());

        for(auto& message : messages)
        {
            text += (std::string("- ") + message + std::string("\n"));
        }

        return QString::fromStdString(text);
    }
}

std::vector<ButtonInfo>* createOkButtonInfoVector(void (TextDialog::*functionPtr)())
{
    std::vector<ButtonInfo>* vectorPtr = new std::vector<ButtonInfo>;

    vectorPtr->emplace_back(
        okButtonText, QDialogButtonBox::AcceptRole, functionPtr);

    return vectorPtr;
}

void showInfoDialogAndLogInfo(
    QWidget* parentPtr, const QString& message, void (TextDialog::*functionPtr)())
{
    getGraphicManagerPtr()->createTextDialogAndShow(
        parentPtr, DialogType::INFO, message,
        createOkButtonInfoVector(functionPtr));

    logArgsInfo(message);
}

void showInfoDialogAndLogInfo(QWidget* parentPtr, QString&& message)
{
    getGraphicManagerPtr()->createTextDialogAndShow(
        parentPtr, DialogType::INFO, std::move(message),
        createOkButtonInfoVector(&QDialog::accept));

    logArgsInfo(message);
}

void showWarningDialogAndLogWarning(QWidget* parentPtr, const QString& message)
{
    getGraphicManagerPtr()->createTextDialogAndShow(
        parentPtr, DialogType::WARNING, message,
        createOkButtonInfoVector(&QDialog::accept));

    logArgsWarning(message);
}

void showErrorDialogAndLogError(QWidget* parentPtr, const QString& message)
{
    getGraphicManagerPtr()->createTextDialogAndShow(
        parentPtr, DialogType::ERROR, message,
        createOkButtonInfoVector(&QDialog::accept));

    logArgsError(message);
}

void showErrorDialogAndLogError(QWidget* parentPtr, QString&& message)
{
    getGraphicManagerPtr()->createTextDialogAndShow(
        parentPtr, DialogType::ERROR, std::move(message),
        createOkButtonInfoVector(&QDialog::accept));

    logArgsError(message);
}
