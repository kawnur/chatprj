#ifndef UTILS_HPP
#define UTILS_HPP

#include <QHostAddress>

#include "logging.hpp"

class CompanionAction;

template<typename... Ts> void logArgsException(Ts&&... args);

template<typename F, typename... Ts>
void runAndLogException(F func, Ts&&... args)
{
    try
    {
        func(args...);
    }
    catch(std::exception& e)
    {
        logArgsException(e.what());
    }
}

// data validation

bool validateCompanionName(const std::string&);
bool validateIpAddress(const std::string&);
bool validatePort(const std::string&);

bool validateCompanionData(std::vector<std::string>&, const CompanionAction*);

std::string buildDialogText(std::string&&, const std::vector<std::string>&);

void showWarningDialogAndLogWarning(const std::string&);
void showErrorDialogAndLogError(const std::string&);

#endif // UTILS_HPP
