#ifndef VALIDATOR_HPP
#define VALIDATOR_HPP

#include <memory>
#include <string>
#include <vector>

#include "alias.hpp"

class Validator
{
public:
    Validator();
    ~Validator() = default;

    template<class A>
    ActionResultPtr validate(std::shared_ptr<A> action);

    bool validateCompanionName(const std::string &name);
    bool validateIpAddress(const std::string &ipAddress);
    bool validatePort(const std::string &port);
    bool validatePassword(const std::string &password);

    bool validateStringByLength(
        const std::string &mark, const std::string &value, std::size_t limit);

    std::string getErrorsText();
    ActionResultPtr getResult(bool value);

private:
    std::vector<std::string> errors_;
};

template<>
ActionResultPtr Validator::validate<CompanionAction>(
    CompanionActionPtr action);

template<>
ActionResultPtr Validator::validate<PasswordAction>(
    PasswordActionPtr action);

template<typename T>
ActionResultPtr validateActionData(std::shared_ptr<T> action)
{
    Validator validator {};

    return validator.validate<T>(action);
}

#endif // VALIDATOR_HPP
