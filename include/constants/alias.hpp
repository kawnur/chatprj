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
class ClearHistoryCompanionAction;
class CreateCompanionAction;
class CreatePasswordAction;
class DBReplyData;
class DeleteCompanionAction;
class FileAction;
class FileManager;
class FileMessageWidget;
class FileOperator;
class GetPasswordAction;
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
class MessageBodyWidget;
class MessageData;
class MessageInfo;
// class MessageHandler;
class MessageMetaData;
class MessageState;
class MessageWidget;
class NetworkEntity;
class NetworkManager;
class PalettedWidget;
class PasswordAction;
class PriorityTask;
class ReceiveConfirmationMessage;
class ReceiveConfirmationMessageState;
class ReceiverOperator;
class RightPanelWidget;
class SenderOperator;
class SaveFileAction;
class SendFileAction;
class SendHistoryCompanionAction;
class ShowHideWidget;
class SocketInfo;
class SocketInfoBaseWidget;
class StubWidgetGroup;
class TextDialog;
class TextEditWidget;
class TextMessage;
class TextMessageData;
class TextMessageWidget;
class UpdateCompanionAction;
class WidgetGroup;

using namespace std::string_literals;

using CompanionResult = ActionSharedValueResult<Companion>;

using ActionPtr = std::shared_ptr<Action>;
using ActionResultPtr = std::shared_ptr<ActionResult>;
using ClearHistoryCompanionActionPtr = std::shared_ptr<ClearHistoryCompanionAction>;
using CompanionActionPtr = std::shared_ptr<CompanionAction>;
using CompanionDataPtr = std::shared_ptr<CompanionData>;
using CompanionPtr = std::shared_ptr<Companion>;
using CompanionResultPtr = std::shared_ptr<CompanionResult>;
using CreateCompanionActionPtr = std::shared_ptr<CreateCompanionAction>;
using CreatePasswordActionPtr = std::shared_ptr<CreatePasswordAction>;
using DBReplyDataPtr = std::shared_ptr<DBReplyData>;
using DeleteCompanionActionPtr = std::shared_ptr<DeleteCompanionAction>;
using FileActionPtr = std::shared_ptr<FileAction>;
using FileManagerPtr = std::shared_ptr<FileManager>;
using GetPasswordActionPtr = std::shared_ptr<GetPasswordAction>;
using GraphicManagerPtr = std::shared_ptr<GraphicManager>;
using MainWindowPtr = std::shared_ptr<MainWindow>;
using ManagerPtr = std::shared_ptr<Manager>;
using MessageBodyWidgetPtr = std::shared_ptr<MessageBodyWidget>;
using MessageDataPtr = std::shared_ptr<MessageData>;
using MessageInfoPtr = std::shared_ptr<MessageInfo>;
using MessageMetaDataPtr = std::shared_ptr<MessageMetaData>;
using MessagePtr = std::shared_ptr<Message>;
using MessageStatePtr = std::shared_ptr<MessageState>;
using MessageWidgetPtr = std::shared_ptr<MessageWidget>;
using NetworkEntityPtr = std::shared_ptr<NetworkEntity>;
using NetworkManagerPtr = std::shared_ptr<NetworkManager>;
using PalettedWidgetPtr = std::shared_ptr<PalettedWidget>;
using PasswordActionPtr = std::shared_ptr<PasswordAction>;
using PGconnPtr = std::shared_ptr<PGconn>;
using PGresultPtr = std::shared_ptr<PGresult>;
using PriorityTaskComparator = std::function<bool(const PriorityTask&, const PriorityTask&)>;

using PriorityTaskQueue =
    std::priority_queue<PriorityTask, std::vector<PriorityTask>, PriorityTaskComparator>;

using ReceiveConfirmationMessagePtr = std::shared_ptr<ReceiveConfirmationMessage>;
using ReceiveConfirmationMessageStatePtr = std::shared_ptr<ReceiveConfirmationMessageState>;
using ReceiverOperatorPtr = std::shared_ptr<ReceiverOperator>;
using SaveFileActionPtr = std::shared_ptr<SaveFileAction>;
using SendFileActionPtr = std::shared_ptr<SendFileAction>;
using SendHistoryCompanionActionPtr = std::shared_ptr<SendHistoryCompanionAction>;
using SocketInfoBaseWidgetPtr = std::shared_ptr<SocketInfoBaseWidget>;
using SocketInfoPtr = std::shared_ptr<SocketInfo>;
using TextMessagePtr = std::shared_ptr<TextMessage>;
using TextMessageDataPtr = std::shared_ptr<TextMessageData>;
using QWidgetPtr = std::shared_ptr<QWidget>;
using UpdateCompanionActionPtr = std::shared_ptr<UpdateCompanionAction>;
using WidgetGroupPtr = std::shared_ptr<WidgetGroup>;

#endif // ALIAS_HPP
