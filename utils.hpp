#ifndef UTILS_HPP
#define UTILS_HPP

#include <QHostAddress>

#include "logging.hpp"

class ButtonInfo;
class CompanionAction;
class TextDialog;

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

QString buildDialogText(std::string&&, const std::vector<std::string>&);

std::vector<ButtonInfo>* createOkButtonInfoVector(void (TextDialog::*)());

void showInfoDialogAndLogInfo(QWidget*, const QString&, void (TextDialog::*)());
void showInfoDialogAndLogInfo(QWidget*, QString&&);
void showWarningDialogAndLogWarning(QWidget*, const QString&);
void showErrorDialogAndLogError(QWidget*, const QString&);
void showErrorDialogAndLogError(QWidget*, QString&&);

#endif // UTILS_HPP
