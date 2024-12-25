#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <map>
#include <string>

const std::string logDelimiter { "############################" };

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

// enum class TextDialogAction
// {
//     CLOSE_PARENT_AND_SELF,
//     ACCEPT
// };

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

const std::string deleteCompanionDialogText { "Companion will be deleted with chat history." };
const std::string clearCompanionHistoryDialogText { "Companion chat history will be deleted." };
const std::string socketInfoStubWidget { "No companion info from DB..." };

// new password dialog
const std::string newPasswordDialogTitle { "New password creation" };
const std::string newPasswordDialogFirstLabel { "Enter password:" };
const std::string newPasswordDialogSecondLabel { "Reenter password:" };

// get password dialog
const std::string getPasswordDialogTitle { "Authentication" };
const std::string getPasswordDialogLabel { "Enter password:" };

// authentication dialog
const std::string authenticationDialogTitle { "Authentication" };
const std::string authenticationDialogLabel { "Enter password:" };

// info dialogs
const std::string newPasswordCreatedLabel { "New password created" };

const std::string okButtonText { "OK" };
const std::string cancelButtonText { "Cancel" };
const std::string clearHistoryButtonText { "Clear history" };
const std::string deleteCompanionButtonText { "Delete companion" };


// colors
const uint32_t mainWindowMenuBarBackgroundColor = 0x777777;
const uint32_t leftPanelBackgroundColor = 0xd1d1d1;
const uint32_t companionNameLabelBackgroundColor = 0xa9a9a9;
const uint32_t indicatorMeColor = 0x6a6a6a;
const uint32_t showHideBackGroundColor = 0x7a7a7a;
const uint32_t textEditBackgroundColor = 0xdadada;
const uint32_t appLogBackgroundColor = 0xcccaca;

#endif // CONSTANTS_HPP
