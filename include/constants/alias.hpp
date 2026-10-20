#ifndef ALIAS_HPP
#define ALIAS_HPP

#include <functional>
#include <memory>
#include <queue>

#include <QWidget>

#include <libpq-fe.h>

class Action;
class ActionResult;

template<typename T>
class ActionSharedValueResult;

class ButtonBox;
class ButtonInfo;
class CentralPanelWidget;
class Companion;
class CompanionAction;
class CompanionData;
class DBReplyData;
class FileAction;
class FileManager;
class FileOperator;
class GraphicManager;
class GroupChatData;
class IndicatorWidget;
class LeftPanelWidget;
class MainWidget;
class MainWindow;

template<typename T>
class MainWindowContainerWidget;

class Manager;
class Message;
class MessageData;
class MessageInfo;
// class MessageHandler;
class MessageMetaData;
class MessageState;
class MessageWidget;
class NetworkEntity;
class NetworkManager;
class PasswordAction;
class PriorityTask;
class RightPanelWidget;
class ShowHideWidget;
class SocketInfo;
class SocketInfoBaseWidget;
class StubWidgetGroup;
class TextDialog;
class TextEditWidget;
class WidgetGroup;

using namespace std::string_literals;

using ActionPtr = std::shared_ptr<Action>;
using ActionResultPtr = std::shared_ptr<ActionResult>;
using CompanionActionPtr = std::shared_ptr<CompanionAction>;
using CompanionDataPtr = std::shared_ptr<CompanionData>;
using CompanionPtr = std::shared_ptr<Companion>;
using CompanionResult = ActionSharedValueResult<Companion>;
using DBReplyDataPtr = std::shared_ptr<DBReplyData>;
using FileActionPtr = std::shared_ptr<FileAction>;
using FileManagerPtr = std::shared_ptr<FileManager>;
using GraphicManagerPtr = std::shared_ptr<GraphicManager>;
using MainWindowPtr = std::shared_ptr<MainWindow>;
using ManagerPtr = std::shared_ptr<Manager>;
using MessageDataPtr = std::shared_ptr<MessageData>;
using MessageInfoPtr = std::shared_ptr<MessageInfo>;
using MessageMetaDataPtr = std::shared_ptr<MessageMetaData>;
using MessagePtr = std::shared_ptr<Message>;
using MessageStatePtr = std::shared_ptr<MessageState>;
using MessageWidgetPtr = std::shared_ptr<MessageWidget>;
using NetworkEntityPtr = std::shared_ptr<NetworkEntity>;
using NetworkManagerPtr = std::shared_ptr<NetworkManager>;
using PasswordActionPtr = std::shared_ptr<PasswordAction>;
using PGconnPtr = std::shared_ptr<PGconn>;
using PGresultPtr = std::shared_ptr<PGresult>;
using PriorityTaskComparator = std::function<bool(const PriorityTask&, const PriorityTask&)>;

using PriorityTaskQueue =
    std::priority_queue<PriorityTask, std::vector<PriorityTask>, PriorityTaskComparator>;

using SocketInfoBaseWidgetPtr = std::shared_ptr<SocketInfoBaseWidget>;
using SocketInfoPtr = std::shared_ptr<SocketInfo>;
using QWidgetPtr = std::shared_ptr<QWidget>;
using WidgetGroupPtr = std::shared_ptr<WidgetGroup>;

#endif // ALIAS_HPP
