#ifndef DATA_CHECKER_HPP
#define DATA_CHECKER_HPP

#include <string>
#include <vector>

#include "alias.hpp"
#include "db_constants.hpp"
#include "utils.hpp"

class DataChecker
{
public:
    DataChecker();
    ~DataChecker() = default;

    void setManager(ManagerPtr manager);

    template<typename... Ts>
    auto getDBData(Ts &&...args);

    template<typename... Ts>
    bool checkDataForExistance(DBRequestType type, const std::string &entryTemplate, Ts &&...args)
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

    // std::vector<std::string> errors() const;
    std::vector<std::string> &&moveErrors();
    void coutErrorsState();

    bool checkCompanionNameForExistanceAtCreation(CreateCompanionActionPtr action);
    bool checkCompanionSocketForExistanceAtCreation(CreateCompanionActionPtr action);
    bool checkCompanionDataForExistanceAtCreation(CreateCompanionActionPtr action);

    bool checkCompanionNameForExistanceAtUpdate(UpdateCompanionActionPtr action);
    bool checkCompanionSocketForExistanceAtUpdate(UpdateCompanionActionPtr action);
    bool checkCompanionDataForExistanceAtUpdate(UpdateCompanionActionPtr action);

    bool checkPasswordForExistanceAtAuthentication(GetPasswordActionPtr action);

    ActionResultPtr checkCreateCompanion(CreateCompanionActionPtr action);
    ActionResultPtr checkUpdateCompanion(UpdateCompanionActionPtr action);
    ActionResultPtr checkGetPassword(GetPasswordActionPtr action);

private:
    ActionResultPtr getActionResult(bool result);

    ManagerPtr manager_;
    bool status_;
    std::vector<std::string> errors_;
};

#include "manager.hpp"

template<typename... Ts>
auto DataChecker::getDBData(Ts &&...args)
{
    auto data = manager_->getDBData(args...);

    if (!data) {
        errors_.push_back(DB_REPLY_NULL);
        status_ = false;
    }

    return data;
}

#endif // DATA_CHECKER_HPP
