#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <map>
#include <QString>
#include <string>

const std::string logDelimiter { "############################" };
const std::string logCustomDelimiter { "?????????????????????????" };

static const char alphanum[] =
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";

enum class NetworkMessageType
{
    SEND_DATA,
    RECEIVE_CONFIRMATION
};

enum class DialogType
{
    INFO,
    WARNING,
    ERROR
};

enum class CompanionActionType
{
    CREATE,
    // READ,
    UPDATE,
    DELETE,
    CLEAR_HISTORY
};

enum class PasswordActionType
{
    CREATE,
    GET
};

enum class MainWindowContainerPosition
{
    LEFT,
    CENTRAL,
    RIGHT
};

const std::map<CompanionActionType, std::string> actionTypeStringRepresentation
{
    { CompanionActionType::CREATE, std::string("Add new companion") },
    { CompanionActionType::UPDATE, std::string("Edit companion") }
};

const std::map<DialogType, std::string> dialogTypeStringRepresentation
{
    { DialogType::INFO, std::string("INFO") },
    { DialogType::WARNING, std::string("WARNING") },
    { DialogType::ERROR, std::string("ERROR") }
};

const QString connectButtonConnectLabel { "Connect" };
const QString connectButtonDisconnectLabel { "Disconnect" };

const QString deleteCompanionDialogText { "Companion will be deleted with chat history." };
const QString clearCompanionHistoryDialogText { "Companion chat history will be deleted." };
const QString socketInfoStubWidget { "No companion info from DB..." };

// new password dialog
const QString newPasswordDialogTitle { "New password creation" };
const QString newPasswordDialogFirstLabel { "Enter password:" };
const QString newPasswordDialogSecondLabel { "Reenter password:" };

// get password dialog
const QString getPasswordDialogTitle { "Authentication" };
const QString getPasswordDialogLabel { "Enter password:" };

// authentication dialog
const QString authenticationDialogTitle { "Authentication" };
const QString authenticationDialogLabel { "Enter password:" };

// info dialogs
const QString newPasswordCreatedLabel { "New password created" };

const QString okButtonText { "OK" };
const QString cancelButtonText { "Cancel" };
const QString clearHistoryButtonText { "Clear history" };
const QString deleteCompanionButtonText { "Delete companion" };


// colors
const uint32_t mainWindowMenuBarBackgroundColor = 0x777777;
const uint32_t leftPanelBackgroundColor = 0xd1d1d1;
const uint32_t companionNameLabelBackgroundColor = 0xa9a9a9;
const uint32_t indicatorMeColor = 0x6a6a6a;
const uint32_t showHideBackGroundColor = 0x7a7a7a;
const uint32_t textEditBackgroundColor = 0xdadada;
const uint32_t appLogBackgroundColor = 0xcccaca;
const uint32_t newMessageEditColor = 0xdcdc07;
const QString sentMessageColor = "#115e00";
const QString receivedMessageColor = "#00115e";


#endif // CONSTANTS_HPP
