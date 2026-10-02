#include "data_checker.hpp"

DataChecker::DataChecker(std::shared_ptr<CompanionAction> action)
    : status_(true), action_(action), errors_() {}

std::vector<std::string> DataChecker::errors() const
{
    return errors_;
}

std::vector<std::string> &&DataChecker::moveErrors()
{
    return std::move(errors_);
}

void DataChecker::coutErrorsState()
{
    coutVectorState(errors_);
}

bool DataChecker::checkCompanionNameForExistanceAtCreation()
{
    // check if companion with such name already exists

    return checkDataForExistanceAtCreation(
        DBRequestType::GET_COMPANION_BY_NAME,
        "companion with name '{}' already exists"s,
        action_->getName());
}

bool DataChecker::checkCompanionSocketForExistanceAtCreation()
{
    // check if such socket already exists

    return checkDataForExistanceAtCreation(
        DBRequestType::GET_SOCKET_BY_IP_ADDRESS_AND_PORT,
        "Companion with address '{0}' and port '{1}' already exists"s,
        action_->getIpAddress(), action_->getClientPort());
}

bool DataChecker::checkCompanionDataForExistanceAtCreation()
{
    return checkCompanionNameForExistanceAtCreation()
        && checkCompanionSocketForExistanceAtCreation();
}