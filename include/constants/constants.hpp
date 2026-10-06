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
using RequestDialogHeaderMap = std::unordered_map<ActionType, std::string>;

const InfoDialogTitleMap INFO_STUB {};
const DataDialogTitleMap DATA_STUB {};

const DataDialogTitleMap ACTION_TITLE_MAP {
    { ActionType::CREATE_COMPANION, "Add new companion" },
    { ActionType::UPDATE_COMPANION, "Edit companion" },
    { ActionType::CREATE_GROUP, "Add new group chat" },
    { ActionType::CREATE_PASSWORD, "Create new password" },
    { ActionType::GET_PASSWORD, "Authentication" },
    { ActionType::SEND_FILE, "Send file" },
    { ActionType::SAVE_FILE, "Save file" }
};

const InfoDialogTitleMap DIALOG_TYPE_STRING_REPR_MAP {
    { DialogType::INFO, "INFO" },
    { DialogType::WARNING, "WARNING" },
    { DialogType::ERROR, "ERROR" }
};

const std::string ACTION_INFO_DIALOG_DEFAULT_HEADER { "Action" };
const std::string INFO_DIALOG_DEFAULT_HEADER { "Info dialog" };

const InfoDialogHeaderMap ACTION_SUCCESS_HEADER_MAP {
    { ActionType::CREATE_COMPANION, "New companion added" },
    { ActionType::UPDATE_COMPANION, "Companion edited" },
    { ActionType::CREATE_PASSWORD, "New password created" }
};

const InfoDialogHeaderMap ACTION_FAIL_HEADER_MAP {
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
// const std::string newPasswordCreatedLabel { "New password created" };

// button text
enum class Button
{
    OK,
    CANCEL,
    EDIT,
    CLEAR_HISTORY,
    SEND_HISTORY,
    DELETE_COMPANION,
    CONNECT,
    DISCONNECT,
    UNKNOWN
};

const std::vector<Button> CONNECT_BUTTONS {
    Button::CONNECT,  // initial
    Button::DISCONNECT
};

const QString DEFAULT_BUTTON_TEXT { "OK" };

const std::unordered_map<Button, QString> BUTTON_ROLE_TO_TEXT_MAP {
    { Button::OK, "OK" },
    { Button::CANCEL, "Cancel" },
    { Button::EDIT, "Edit" },
    { Button::CLEAR_HISTORY, "Clear history" },
    { Button::SEND_HISTORY, "Send history" },
    { Button::DELETE_COMPANION, "Delete companion" },
    { Button::CONNECT, "Connect" },
    { Button::DISCONNECT, "Disconnect" },
    { Button::UNKNOWN, "_" }
};

// colors
enum class Widget
{
    MAIN_WINDOW_MENU,
    LEFT_PANEL,
    COMPANION_NAME_LABEL,
    SHOW_HIDE,
    MESSAGE,
    MESSAGE_SENT,
    MESSAGE_RECEIVED,
    BUTTON_PANEL,
    TEXT_EDIT,
    APP_LOG,
    INDICATOR_ME,
    NEW_MESSAGE_EDIT
};

const uint32_t DEFAULT_WIDGET_COLOR = 0x000000;  // black

const std::unordered_map<Widget, uint32_t> WIDGET_COLOR_MAP {
    { Widget::MAIN_WINDOW_MENU, 0x777777 },
    { Widget::LEFT_PANEL, 0xc9c9c9 },
    { Widget::COMPANION_NAME_LABEL, 0xa4a4a4 },
    { Widget::SHOW_HIDE, 0x7a7a7a },
    { Widget::MESSAGE, 0xd1d1d1 },
    { Widget::MESSAGE_SENT, 0x115e00 },
    { Widget::MESSAGE_RECEIVED, 0x00115e },
    { Widget::BUTTON_PANEL, 0x898989 },
    { Widget::TEXT_EDIT, 0xe1e1e1 },
    { Widget::APP_LOG, 0xcccaca },
    { Widget::INDICATOR_ME, 0x6a6a6a },
    { Widget::NEW_MESSAGE_EDIT, 0xdcdc07 }
};

#endif // CONSTANTS_HPP
