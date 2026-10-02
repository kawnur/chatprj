#ifndef DATA_CHECKER_HPP
#define DATA_CHECKER_HPP

#include <memory>
#include <string>
#include <vector>

#include "manager.hpp"

class CompanionAction;

class DataChecker
{
public:
    DataChecker(std::shared_ptr<CompanionAction> action);
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
    bool checkDataForExistanceAtCreation(
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

    bool checkCompanionNameForExistanceAtCreation();
    bool checkCompanionSocketForExistanceAtCreation();
    bool checkCompanionDataForExistanceAtCreation();

    std::vector<std::string> errors() const;
    std::vector<std::string> &&moveErrors();

    void coutErrorsState();

private:
    bool status_;
    std::shared_ptr<CompanionAction> action_;
    std::vector<std::string> errors_;
};

#endif // DATA_CHECKER_HPP
