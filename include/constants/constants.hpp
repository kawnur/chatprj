#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <cstdint>
#include <filesystem>
#include <unordered_map>
#include <string>

#include <QDialogButtonBox>
#include <QString>

const std::string LOG_DELIMITER(28, '#');
const std::string LOG_CUSTOM_DELIMITER(25, '?');
const std::string MESSAGE_LOG_DELIMITER(28, 'M');
const std::string DATA_LOG_DELIMITER(28, 'D');
const std::string LAYOUT_LOG_DELIMITER(28, 'L');
const std::string EXIT_LOG_ENTRY { "Exit..." };

const std::size_t MAX_BUFFER_SIZE = 1024;

const int NUMBER_OF_MESSAGES_TO_GET_FROM_DB = 5;

const bool LOG_DB_INTERACTION = true;

static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

const std::filesystem::path HOME_PATH("~");

const int PORT_BAD_VALUE = 0;
const int ID_BAD_VALUE = -1;
const int DATA_BAD_VALUE = 0;

const std::string DB_REPLY_NULL { "DB interaction error" };
const std::string DB_REPLY_EMPTY { "DB reply is empty" };
const std::string VALUE_BUILDING_ERROR { "Value building error" };
const std::string POINTER_CASTING_ERROR { "Pointer casting error" };

const QString ME_NAME { "me" };
const std::string MESSAGE_WIDGET_ME_NAME { "Me" };
const QString NOT_FOUND { "NOT FOUND" };

const std::size_t NETWORK_ID_SIZE = 5;

// file transmission
enum class FileTransmissionState
{
    UNKNOWN,
    SEND_STARTED,
    SEND_SUCCESS,
    SEND_FAILURE,
    RECEIVE_STARTED,
    RECEIVE_SUCCESS,
    RECEIVE_FAILURE
};

const uint32_t FILE_MANAGER_HANDLER_SLEEP_MS = 500;

// validation
const std::size_t COMPANION_NAME_SIZE_LIMIT = 30;
const std::size_t PASSWORD_SIZE_LIMIT = 30;

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

const std::unordered_map<NetworkMessageType, std::string> NETWORK_MESSAGE_TYPE_STRING_REPR_MAP {
    { NetworkMessageType::UNKNOWN, "UNKNOWN" },
    { NetworkMessageType::NO_ACTION, "NO_ACTION" },
    { NetworkMessageType::TEXT, "TEXT" },
    { NetworkMessageType::FILE_PROPOSAL, "FILE_PROPOSAL" },
    { NetworkMessageType::FILE_REQUEST, "FILE_REQUEST" },
    { NetworkMessageType::FILE_DATA, "FILE_DATA" },
    { NetworkMessageType::FILE_DATA_CHECK_SUCCESS, "FILE_DATA_CHECK_SUCCESS" },
    { NetworkMessageType::FILE_DATA_CHECK_FAILURE, "FILE_DATA_CHECK_FAILURE" },
    { NetworkMessageType::FILE_DATA_TRANSMISSON_END, "FILE_DATA_TRANSMISSON_END" },
    { NetworkMessageType::FILE_DATA_TRANSMISSON_FAILURE, "FILE_DATA_TRANSMISSON_FAILURE" },
    { NetworkMessageType::RECEIVE_CONFIRMATION, "RECEIVE_CONFIRMATION" },
    { NetworkMessageType::RECEIVE_CONFIRMATION_REQUEST, "RECEIVE_CONFIRMATION_REQUEST" },
    { NetworkMessageType::CHAT_HISTORY_REQUEST, "CHAT_HISTORY_REQUEST" },
    { NetworkMessageType::CHAT_HISTORY_DATA, "CHAT_HISTORY_DATA" }
};

const std::string deleteCompanionDialogText { "Companion will be deleted with chat history." };
const std::string clearCompanionHistoryDialogText { "Companion chat history will be deleted." };
const std::string sendChatHistoryToCompanionDialogText { "Companion {} requested chat history sending." };
const std::string EMPTY_FIELD_DIALOG_TEXT { "Empty field" };
const std::string PASSWORDS_ARE_NOT_EQUAL_DIALOG_TEXT { "Passwords are not equal" };
const std::string socketInfoStubWidget { "No companion info from DB..." };

// new group chat dialog
const QString newGroupChatDialogLabel { "Choose companions to add to new group chat" };

const QString DEFAULT_BUTTON_TEXT { "____" };
const QString DEFAULT_FILE_WIDGET_TEXT { "FILE_WIDGET" };
const QString DEFAULT_DATA_TYPE_STRING_REPR { "UNKNOWN" };

const QString OK_BUTTON_TEXT = "OK";
const QString CANCEL_BUTTON_TEXT = "Cancel";
const QString EDIT_BUTTON_TEXT = "Edit";
const QString CONNECT_BUTTON_TEXT = "Connect";
const QString DISCONNECT_BUTTON_TEXT = "Disconnect";
const QString CLEAR_HISTORY_BUTTON_TEXT = "Clear history";
const QString SEND_HISTORY_BUTTON_TEXT = "Send history";
const QString DELETE_COMPANION_BUTTON_TEXT = "Delete companion";
const QString DOWNLOAD_FILE_BUTTON_TEXT = "Download file";
const QString SEND_FILE_BUTTON_TEXT = "Send file";
const QString SEND_TEXT_BUTTON_TEXT = "Send text";
const QString UNKNOWN_BUTTON_TEXT = "_";

const QDialogButtonBox::ButtonRole DEFAULT_BUTTON_ROLE = QDialogButtonBox::NoRole;

// colors
enum class WidgetType
{
    UNKNOWN,
    MAIN_WINDOW_MENU,
    CONTAINER_WIDGET,
    LEFT_PANEL,
    COMPANION_PANEL,
    SOCKET_INFO,
    SOCKET_INFO_SELECTED,
    SOCKET_INFO_UNKNOWN,
    COMPANION_NAME_LABEL,
    SHOW_HIDE,
    CENTRAL_PANEL,
    HISTORY,
    MESSAGE,
    MESSAGE_BODY,
    TEXT_SENT,
    TEXT_RECEIVED,
    FILE_SENT,
    FILE_RECEIVED,
    SOCKET_INFO_BUTTON_PANEL,
    SOCKET_INFO_BUTTON_PANEL_SELECTED,
    HISTORY_BUTTON_PANEL,
    TEXT_EDIT,
    SPLITTER_CENTRAL_RIGHT,
    RIGHT_PANEL,
    APP_LOG,
    CONNECTION_STATE_INDICATOR,
    NEW_MESSAGES_INDICATOR,
    MESSAGE_WIDGET_INDICATOR_PANEL,
    MESSAGE_WIDGET_INDICATOR,
    NEW_MESSAGE_EDIT
};

const uint32_t DEFAULT_WIDGET_LIGHTNESS = 0;  // black
const uint32_t DEFAULT_WIDGET_COLOR = 0x000000;  // black
const uint32_t DEFAULT_INDICATOR_WIDGET_COLOR = 0x5d5c56;

const uint32_t SOCKET_INFO_WIDGET_WIDTH = 500;

const std::unordered_map<WidgetType, QString> FILE_WIDGET_TEXT_MAP {
    { WidgetType::FILE_SENT, "SEND FILE: {}" },
    { WidgetType::FILE_RECEIVED, "RECEIVE FILE: {}" }
};

enum class MessageWidgetState {
    UNKNOWN,
    SENT,
    RECEIVED
};

enum class IndicatorWidgetState {
    UNKNOWN,
    ON,
    OFF
};

enum class DataType
{
    NAME,
    IP_ADDRESS,
    SERVER_PORT,
    CLIENT_PORT,
    PASSWORD,
    PASSWORD_REENTER
};

#endif // CONSTANTS_HPP
