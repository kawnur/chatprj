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

bool validateCompanionName(std::vector<std::string>&, const std::string&);
bool validateIpAddress(std::vector<std::string>&, const std::string&);
bool validatePort(std::vector<std::string>&, const std::string&);

bool validateCompanionData(std::vector<std::string>&, const CompanionAction*);

bool validatePassword(std::vector<std::string>&, const std::string&);

std::string buildDialogText(std::string&&, const std::vector<std::string>&);

void showInfoDialogAndLogInfo(QWidget*, const std::string&);
void showInfoDialogAndLogInfo(QWidget*, std::string&&);
void showWarningDialogAndLogWarning(QWidget*, const std::string&);
void showErrorDialogAndLogError(QWidget*, const std::string&);
void showErrorDialogAndLogError(QWidget*, std::string&&);

#endif // UTILS_HPP
