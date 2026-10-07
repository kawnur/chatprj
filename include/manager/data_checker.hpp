#ifndef DATA_CHECKER_HPP
#define DATA_CHECKER_HPP

#include <memory>
#include <string>
#include <vector>

#include "db_constants.hpp"
#include "manager.hpp"

class CompanionAction;
class DBReplyData;
class Manager;

std::shared_ptr<Manager> getManager();

class DataChecker
{
public:
    // DataChecker(std::shared_ptr<CompanionAction> action);
    DataChecker(std::shared_ptr<Action> action);
    ~DataChecker() = default;

    template<typename... Ts>
    auto getDBData(Ts &&...args)
    {
        auto data = getManager()->getDBData(args...);

        if (!data) {
            errors_.push_back(DB_REPLY_NULL);
            status_ = false;
        }

        return data;
    }

    template<typename... Ts>
    bool checkDataForExistance(
        DBRequestType type, const std::string &entryTemplate, Ts &&...args)
    {
        if (!status_)
            return false;

        auto data = getDBData(type, args...);

        if (!data)
            return false;

        if (!data->isEmpty()) {
            errors_.push_back(getStringByFormat(entryTemplate, args...));

            return false;
        }

        return true;
    }

    template<typename... Ts>
    bool checkIdDataForExistance(
        DBRequestType type, const std::string &entryTemplate, int id, Ts &&...args)
    {
        if (!status_)
            return false;

        auto data = getDBData(type, args...);

        if (!data)
            return false;

        bool result = data->findValue("id"s, getString(id));
        bool found = (result && data->size() > 1) || (!result && data->size() > 0);

        if (found) {
            errors_.push_back(getStringByFormat(entryTemplate, args...));

            return false;
        }

        return true;
    }

    template<typename... Ts>
    bool checkPasswordDataForExistance(
        DBRequestType type, const std::string &emptyDataEntryTemplate,
        const std::string &noMatchEntryTemplate, const std::string &password, Ts &&...args)
    {
        if (!status_)
            return false;

        auto data = getDBData(type, args...);

        if (!data)
            return false;

        if (data->isEmpty()) {
            errors_.push_back(getStringByFormat(emptyDataEntryTemplate, args...));

            return false;
        }

        auto result = data->getValue(0, "password");
        bool match = (result == password);

        if (!match) {
            errors_.push_back(getStringByFormat(noMatchEntryTemplate, args...));

            return false;
        }

        return true;
    }

    std::vector<std::string> errors() const;
    std::vector<std::string> &&moveErrors();
    void coutErrorsState();

    bool checkCompanionNameForExistanceAtCreation();
    bool checkCompanionSocketForExistanceAtCreation();
    bool checkCompanionDataForExistanceAtCreation();

    bool checkCompanionNameForExistanceAtUpdate();
    bool checkCompanionSocketForExistanceAtUpdate();
    bool checkCompanionDataForExistanceAtUpdate();

    bool checkPasswordForExistanceAtAuthentication();
private:
    bool status_;
    // std::shared_ptr<CompanionAction> action_;
    std::shared_ptr<Action> action_;
    std::vector<std::string> errors_;
};

template<typename F>
// std::shared_ptr<ActionResult> checkCompanionDataForExistance(
//     F &&func, std::shared_ptr<CompanionAction> action)
std::shared_ptr<ActionResult> checkDataForExistance(
    F &&func, std::shared_ptr<Action> action)
{
    DataChecker checker(action);

    auto result = func(checker);

    // auto result = checker.checkCompanionDataForExistanceAtCreation();
    // checker.coutErrorsState();

    if (result)
        return std::make_shared<ActionResult>(true, ""s);
    else
        return std::make_shared<ActionResult>(false, checker.moveErrors());
}

#endif // DATA_CHECKER_HPP
