#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <filesystem>
#include <map>
#include <QDialogButtonBox>
#include <QString>  // TODO delete
#include <string>

const std::string logDelimiter { "############################" };
const std::string logCustomDelimiter { "?????????????????????????" };

const std::size_t maxBufferSize = 1024;

const int numberOfMessagesToGetFromDB = 10;

const bool logDBInteraction = false;

static const char alphanum[] =
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";

const uint32_t sleepDurationInitial = 1000;
const float sleepDurationIncreaseRate = 1.2f;

const std::filesystem::path homePath("~");

enum class MessageType {
    TEXT,
    FILE
};

enum class NetworkMessageType {
    NO_ACTION,
    TEXT,
    FILE_PROPOSAL,
    FILE_REQUEST,
    FILE_DATA,
    FILE_DATA_CHECK_SUCCESS,
    FILE_DATA_CHECK_FAILURE,
    FILE_DATA_TRANSMISSON_END,
    FILE_DATA_TRANSMISSON_FAILURE,
    RECEIVE_CONFIRMATION,
    RECEIVE_CONFIRMATION_REQUEST,
    CHAT_HISTORY_REQUEST,
    CHAT_HISTORY_DATA
};

enum class DialogType {
    INFO,
    WARNING,
    ERROR
};

enum class ChatActionType {
    CREATE,
    // READ,
    UPDATE,
    DELETE,
    CLEAR_HISTORY,
    SEND_HISTORY
};

enum class PasswordActionType {
    CREATE,
    GET
};

enum class FileActionType {
    SEND,
    SAVE
};

enum class MainWindowContainerPosition {
    LEFT,
    CENTRAL,
    RIGHT
};

const std::map<ChatActionType, QString> companionActionTypeStringRepresentation {
    { ChatActionType::CREATE, "Add new companion" },
    { ChatActionType::UPDATE, "Edit companion" }
};

const std::map<ChatActionType, QString> groupChatActionTypeStringRepresentation {
    { ChatActionType::CREATE, "Add new group chat" }
};

const std::map<DialogType, QString> dialogTypeStringRepresentation {
    { DialogType::INFO, "INFO" },
    { DialogType::WARNING, "WARNING" },
    { DialogType::ERROR, "ERROR" }
};

const std::map<FileActionType, QString> fileDialogTypeQStringRepresentation {
    { FileActionType::SEND, "Send file" },
    { FileActionType::SAVE, "Save file" }
};

const QString connectButtonConnectLabel { "Connect" };
const QString connectButtonDisconnectLabel { "Disconnect" };

const std::vector<QString> connectButtonLabels {
    connectButtonConnectLabel,  // initial
    connectButtonDisconnectLabel
};

const std::string deleteCompanionDialogText { "Companion will be deleted with chat history." };
const std::string clearCompanionHistoryDialogText { "Companion chat history will be deleted." };
constexpr std::string_view sendChatHistoryToCompanionDialogText { "Companion {} requested chat history sending." };
const std::string socketInfoStubWidget { "No companion info from DB..." };

// new group chat dialog
const QString newGroupChatDialogLabel { "Choose companions to add to new group chat" };

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
const std::string sentMessageColor = "#115e00";
const std::string receivedMessageColor = "#00115e";

#endif // CONSTANTS_HPP
