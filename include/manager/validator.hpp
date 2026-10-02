#ifndef VALIDATOR_HPP
#define VALIDATOR_HPP

#include <memory>
#include <string>
#include <vector>

class ActionResult;
class CompanionAction;
class PasswordAction;

class Validator
{
public:
    Validator();
    ~Validator() = default;

    template<class A>
    std::shared_ptr<ActionResult> validate(std::shared_ptr<A> action);

    bool validateCompanionName(const std::string &name);
    bool validateIpAddress(const std::string &ipAddress);
    bool validatePort(const std::string &port);
    bool validatePassword(const std::string &password);

    bool validateStringByLength(
        const std::string &mark, const std::string &value, std::size_t limit);

    std::string getErrorsText();
    std::shared_ptr<ActionResult> getResult(bool value);

private:
    std::vector<std::string> errors_;
};

template<>
std::shared_ptr<ActionResult> Validator::validate<CompanionAction>(
    std::shared_ptr<CompanionAction> action);

template<>
std::shared_ptr<ActionResult> Validator::validate<PasswordAction>(
    std::shared_ptr<PasswordAction> action);

template<typename T>
std::shared_ptr<ActionResult> validateActionData(std::shared_ptr<T> action)
{
    Validator validator {};

    return validator.validate<T>(action);
}

#endif // VALIDATOR_HPP
