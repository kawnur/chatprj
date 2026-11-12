#include "data_checker.hpp"

#include "action.hpp"
#include "action_result.hpp"
#include "companion.hpp"

DataChecker::DataChecker() : status_(true), errors_() {}

void DataChecker::setManager(ManagerPtr manager)
{
    manager_ = manager;
}

// std::vector<std::string> DataChecker::errors() const
// {
//     return errors_;
// }

void DataChecker::coutErrorsState()
{
    coutVectorState(errors_);
}

bool DataChecker::checkCompanionNameForExistanceAtCreation(CreateCompanionActionPtr action)
{
    // check if companion with such name already exists

    return checkDataForExistance(
        DBRequestType::GET_COMPANION_BY_NAME, "companion with name '{}' already exists"s,
        action->name());
}

bool DataChecker::checkCompanionSocketForExistanceAtCreation(CreateCompanionActionPtr action)
{
    // check if such socket already exists

    return checkDataForExistance(
        DBRequestType::GET_SOCKET_BY_IP_ADDRESS_AND_PORT,
        "companion with address '{0}' and port '{1}' already exists"s, action->ipAddress(),
        action->clientPort());
}

bool DataChecker::checkCompanionDataForExistanceAtCreation(CreateCompanionActionPtr action)
{
    return checkCompanionNameForExistanceAtCreation(action)
        && checkCompanionSocketForExistanceAtCreation(action);
}

bool DataChecker::checkCompanionNameForExistanceAtUpdate(UpdateCompanionActionPtr action)
{
    // check if companion with such name already exists

    return checkIdDataForExistance(
        DBRequestType::GET_COMPANION_BY_NAME, "companion with name '{}' already exists"s,
        action->companionId(), action->name());
}

bool DataChecker::checkCompanionSocketForExistanceAtUpdate(UpdateCompanionActionPtr action)
{
    // check if such socket already exists

    return checkIdDataForExistance(
        DBRequestType::GET_SOCKET_BY_IP_ADDRESS_AND_PORT,
        "companion with address '{0}' and port '{1}' already exists"s, action->companionId(),
        action->ipAddress(), action->clientPort());
}

bool DataChecker::checkCompanionDataForExistanceAtUpdate(UpdateCompanionActionPtr action)
{
    return checkCompanionNameForExistanceAtUpdate(action)
    && checkCompanionSocketForExistanceAtUpdate(action);
}


bool DataChecker::checkPasswordForExistanceAtAuthentication(GetPasswordActionPtr action)
{
    return checkPasswordDataForExistance(
        DBRequestType::GET_PASSWORD, "password was not set"s, "password is incorrect"s,
        action->password());
}

ActionResultPtr DataChecker::checkCreateCompanion(CreateCompanionActionPtr action)
{
    bool result = checkCompanionDataForExistanceAtCreation(action);

    return getActionResult(result);
}

ActionResultPtr DataChecker::checkUpdateCompanion(UpdateCompanionActionPtr action)
{
    bool result = checkCompanionDataForExistanceAtUpdate(action);

    return getActionResult(result);
}

ActionResultPtr DataChecker::checkGetPassword(GetPasswordActionPtr action)
{
    bool result = checkPasswordForExistanceAtAuthentication(action);

    return getActionResult(result);
}

ActionResultPtr DataChecker::getActionResult(bool result)
{
    if (result)
        return std::make_shared<ActionResult>(true, ""s);
    else
        return std::make_shared<ActionResult>(false, std::move(errors_));
}
