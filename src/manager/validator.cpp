#include "validator.hpp"

#include <QHostAddress>

#include "action.hpp"
#include "action_result.hpp"
#include "logging.hpp"
#include "utils.hpp"

Validator::Validator() : errors_() {}

bool Validator::validateCompanionName(const std::string &name)
{
    return validateStringByLength("companion name"s, name, COMPANION_NAME_SIZE_LIMIT);
}

bool Validator::validateIpAddress(const std::string &ipAddress)
{
    QHostAddress address { getQString(ipAddress) };
    bool result = !(address.isNull());

    if (!result) {
        auto entry = getStringByFormat("companion ipaddress '{}' is invalid", ipAddress);
        errors_.push_back(entry);
    }

    // logArgs("validateIpAddress result:", result);

    return result;
}

bool Validator::validatePort(const std::string &port)
{
    bool result = false;
    auto entryTemplate = "port number '{0}' must be greater than 0 and lower than 65536"s;
    auto entry = getStringByFormat(entryTemplate, port);

    try {
        long long portNumber = std::stoll(port, nullptr, 10);

        result = (portNumber >= 0) && (portNumber <= 65535);

        if (!result)
            errors_.push_back(entry);
    }
    catch(std::out_of_range) {
        auto entryTemplateException = "{0}, port is too big, std::out_of_range"s;
        errors_.push_back(getStringByFormat(entryTemplateException, entry));
    }
    catch(std::invalid_argument) {
        auto entryTemplateException = "{0}, port is invalid, std::invalid_argument"s;
        errors_.push_back(getStringByFormat(entryTemplateException, entry));
    }

    // logArgs("validatePort result:", result);

    return result;
}

bool Validator::validatePassword(const std::string &password)
{
    return validateStringByLength("password"s, password, PASSWORD_SIZE_LIMIT);
}

bool Validator::validateStringByLength(
    const std::string &mark, const std::string &value, std::size_t limit)
{
    auto size = value.size();
    bool result = (size <= limit);

    if (!result) {
        auto entryTemplate = "'{0}' length '{1}' is greater than '{2}'"s;
        auto entry = getStringByFormat(entryTemplate, mark, size, limit);
        errors_.push_back(entry);
    }

    // logArgs("validatePort result:", result);

    return result;
}

std::string Validator::getErrorsText()
{
    return buildTextAsUnorderedListWithHeader("Validation errors", errors_);
}

std::shared_ptr<ActionResult> Validator::getResult(bool value)
{
    if (value)
        return std::make_shared<ActionResult>(true, ""s);
    else
        return std::make_shared<ActionResult>(false, getErrorsText());
}

template<>
std::shared_ptr<ActionResult> Validator::validate<CompanionAction>(
    std::shared_ptr<CompanionAction> action)
{
    bool nameResult = validateCompanionName(action->getName());
    bool ipAddressResult = validateIpAddress(action->getIpAddress());
    bool portResult = validatePort(action->getClientPort());

    bool result = nameResult && ipAddressResult && portResult;

    logArgs("companion data validation result:", result);

    return getResult(result);
}

template<>
std::shared_ptr<ActionResult> Validator::validate<PasswordAction>(
    std::shared_ptr<PasswordAction> action)
{
    bool result = validatePassword(action->getPassword());

    logArgs("companion data validation result:", result);

    return getResult(result);
}
