#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <cstdint>
#include <filesystem>
#include <unordered_map>
#include <string>

#include <QString>

const std::string logDelimiter { "############################" };
const std::string logCustomDelimiter { "?????????????????????????" };
const std::string EXIT_LOG_ENTRY { "Exit..." };

const std::size_t MAX_BUFFER_SIZE = 1024;

const int NUMBER_OF_MESSAGES_TO_GET_FROM_DB = 10;

const bool LOG_DB_INTERACTION = true;

static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

const uint32_t SLEEP_DURATION_INITIAL_MS = 1000;
const float SLEEP_DURATION_INCREASE_RATE = 1.2f;

const std::filesystem::path HOME_PATH("~");

const int PORT_BAD_VALUE = 0;
const int ID_BAD_VALUE = -1;
const int DATA_BAD_VALUE = 0;

const std::string DB_REPLY_NULL { "DB interaction error" };
const std::string DB_REPLY_EMPTY { "DB reply is empty" };
const std::string VALUE_BUILDING_ERROR { "Value building error" };
const std::string POINTER_CASTING_ERROR { "Pointer casting error" };

const QString ME_NAME { "me" };

const uint8_t CONNECTION_STATE_INDICATOR_WIDGET_SIZE = 15;
const uint8_t NEW_MESSAGES_INDICATOR_WIDGET_SIZE = 7;

const QString SOCKET_INFO_WIDGET_EDIT_BUTTON_LABEL { "Edit" };

// validation
const std::size_t COMPANION_NAME_SIZE_LIMIT = 30;
const std::size_t PASSWORD_SIZE_LIMIT = 30;

enum class MessageType
{
    UNKNOWN,
    TEXT,
    FILE
};

enum class NetworkMessageType
{
    UNKNOWN,
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

enum class LogType
{
    INFO,
    DEBUG,
    EXCEPTION,
    WARNING,
    ERROR
};

enum class DialogType
{
    INFO,
    WARNING,
    ERROR
};

enum class ActionType
{
    UNKNOWN,
    CREATE_COMPANION,
    // READ,
    UPDATE_COMPANION,
    DELETE_COMPANION,
    CREATE_GROUP,
    CLEAR_HISTORY,
    SEND_HISTORY,
    CREATE_PASSWORD,
    GET_PASSWORD,
    SEND_FILE,
    SAVE_FILE
};

enum class MainWindowContainerPosition
{
    LEFT,
    CENTRAL,
    RIGHT
};

// dialog type to dialog window title mapping
using InfoDialogTitleMap = std::unordered_map<DialogType, QString>;
using DataDialogTitleMap = std::unordered_map<ActionType, QString>;

// action type to first line of info dialog text mapping
using InfoDialogHeaderMap = std::unordered_map<ActionType, std::string>;

const InfoDialogTitleMap INFO_STUB {};
const DataDialogTitleMap DATA_STUB {};

const InfoDialogTitleMap DIALOG_TYPE_STRING_REPR_MAP {
    { DialogType::INFO, "INFO" },
    { DialogType::WARNING, "WARNING" },
    { DialogType::ERROR, "ERROR" }
};

const DataDialogTitleMap COMPANION_ACTION_TITLE_MAP {
    { ActionType::CREATE_COMPANION, "Add new companion" },
    { ActionType::UPDATE_COMPANION, "Edit companion" }
};

const DataDialogTitleMap GROUP_ACTION_TITLE_MAP {
    { ActionType::CREATE_GROUP, "Add new group chat" }
};

const DataDialogTitleMap PASSWORD_ACTION_TITLE_MAP {
    { ActionType::CREATE_PASSWORD, "Create new password" },
    { ActionType::GET_PASSWORD, "Authentication" }
};

const std::unordered_map<ActionType, std::string> fileDialogTypeQStringRepresentation {
    { ActionType::SEND_FILE, "Send file" },
    { ActionType::SAVE_FILE, "Save file" }
};

const std::string COMPANION_ACTION_INFO_DIALOG_DEFAULT_HEADER { "Companion action" };
const std::string GROUP_ACTION_INFO_DIALOG_DEFAULT_HEADER { "Group action" };
const std::string PASSWORD_ACTION_INFO_DIALOG_DEFAULT_HEADER { "Password action" };
const std::string FILE_ACTION_INFO_DIALOG_DEFAULT_HEADER { "File action" };
const std::string INFO_DIALOG_DEFAULT_HEADER { "Info dialog" };

const InfoDialogHeaderMap COMPANION_ACTION_SUCCESS_HEADER_MAP {
    { ActionType::CREATE_COMPANION, "New companion added" },
    { ActionType::UPDATE_COMPANION, "Companion edited" }
};

const InfoDialogHeaderMap COMPANION_ACTION_FAIL_HEADER_MAP {
    { ActionType::CREATE_COMPANION, "Companion addition error" },
    { ActionType::UPDATE_COMPANION, "Companion edition error" }
};

const std::unordered_map<DialogType, LogType> MAP_DIALOG_TYPE_TO_LOG_TYPE {
    { DialogType::INFO, LogType::INFO },
    { DialogType::WARNING, LogType::WARNING },
    { DialogType::ERROR, LogType::ERROR }
};

const std::unordered_map<LogType, std::string> LOG_TYPE_STRING_REPR {
    { LogType::INFO,      "INF" },
    { LogType::DEBUG,     "DEB" },
    { LogType::EXCEPTION, "EXC" },
    { LogType::WARNING,   "WRN" },
    { LogType::ERROR,     "ERR" }
};

const QString connectButtonConnectLabel { "Connect" };
const QString connectButtonDisconnectLabel { "Disconnect" };

const std::vector<QString> connectButtonLabels {
    connectButtonConnectLabel,  // initial
    connectButtonDisconnectLabel
};

const std::string deleteCompanionDialogText { "Companion will be deleted with chat history." };
const std::string clearCompanionHistoryDialogText { "Companion chat history will be deleted." };
const std::string sendChatHistoryToCompanionDialogText { "Companion {} requested chat history sending." };
const std::string socketInfoStubWidget { "No companion info from DB..." };

// new group chat dialog
const QString newGroupChatDialogLabel { "Choose companions to add to new group chat" };

// new password dialog
const QString newPasswordDialogFirstLabel { "Enter password:" };
const QString newPasswordDialogSecondLabel { "Reenter password:" };

// get password dialog
const QString getPasswordDialogLabel { "Enter password:" };

// info dialogs
const std::string newPasswordCreatedLabel { "New password created" };

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
