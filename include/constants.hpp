#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <map>
#include <QString>
#include <string>

const std::string logDelimiter { "############################" };
const std::string logCustomDelimiter { "?????????????????????????" };

const size_t maxBufferSize = 1024;

const int numberOfMessagesToGetFromDB = 10;

const bool logDBInteraction = false;

static const char alphanum[] =
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";

const uint32_t sleepDurationInitial = 1000;
const float sleepDurationIncreaseRate = 1.2f;

enum class MessageType
{
    TEXT,
    FILE
};

enum class NetworkMessageType
{
    NO_ACTION,
    TEXT,
    FILE_PROPOSAL,
    FILE_REQUEST,
    RECEIVE_CONFIRMATION,
    RECEIVE_CONFIRMATION_REQUEST,
    CHAT_HISTORY_REQUEST,
    CHAT_HISTORY_DATA
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
    CLEAR_HISTORY,
    SEND_HISTORY
};

enum class PasswordActionType
{
    CREATE,
    GET
};

enum class FileActionType
{
    SEND,
    SAVE
};

enum class MainWindowContainerPosition
{
    LEFT,
    CENTRAL,
    RIGHT
};

const std::map<CompanionActionType, QString> actionTypeStringRepresentation
{
    { CompanionActionType::CREATE, "Add new companion" },
    { CompanionActionType::UPDATE, "Edit companion" }
};

const std::map<DialogType, QString> dialogTypeStringRepresentation
{
    { DialogType::INFO, "INFO" },
    { DialogType::WARNING, "WARNING" },
    { DialogType::ERROR, "ERROR" }
};

const std::map<FileActionType, QString> fileDialogTypeQStringRepresentation
{
    { FileActionType::SEND, "Send file" },
    { FileActionType::SAVE, "Save file" }
};

const QString connectButtonConnectLabel { "Connect" };
const QString connectButtonDisconnectLabel { "Disconnect" };

const std::vector<QString> connectButtonLabels
{
    connectButtonConnectLabel,  // initial
    connectButtonDisconnectLabel
};

const QString deleteCompanionDialogText { "Companion will be deleted with chat history." };
const QString clearCompanionHistoryDialogText { "Companion chat history will be deleted." };
const QString sendChatHistoryToCompanionDialogText { "Companion %1 requested chat history sending." };
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

// button text
const QString okButtonText { "OK" };
const QString cancelButtonText { "Cancel" };
const QString clearHistoryButtonText { "Clear history" };
const QString deleteCompanionButtonText { "Delete companion" };
const QString sendChatHistoryButtonText { "Send chat history" };

// colors
const uint32_t mainWindowMenuBarBackgroundColor = 0x777777;
const uint32_t leftPanelBackgroundColor = 0xc9c9c9;
const uint32_t companionNameLabelBackgroundColor = 0xa4a4a4;
const uint32_t indicatorMeColor = 0x6a6a6a;
const uint32_t showHideWidgetBackGroundColor = 0x7a7a7a;
const uint32_t messageWidgetBackGroundColor = 0xd1d1d1;
const uint32_t buttonPanelBackGroundColor = 0x898989;
const uint32_t textEditBackgroundColor = 0xe1e1e1;
const uint32_t newMessageEditColor = 0xdcdc07;
const uint32_t appLogBackgroundColor = 0xcccaca;
const QString sentMessageColor = "#115e00";
const QString receivedMessageColor = "#00115e";

#endif // CONSTANTS_HPP
