#ifndef MESSAGE_HANDLER_HPP
#define MESSAGE_HANDLER_HPP

#include <memory>
#include <string>

#include <nlohmann/json.hpp>

#include "constants.hpp"
#include "logging.hpp"

class Action;
// class ActionResult;

// template<typename T>
// class ActionSharedValueResult;

class Companion;
// class CompanionAction;
// class DBReplyData;
// class DBRequester;
class Manager;
// class Message;
class MessageData;
// class MessageInfo;
// class MessageHandler;
class MessageMetaData;
class MessageState;
// class PasswordAction;
// class SocketInfoBaseWidget;
// class WidgetGroup;

using CompanionPtr = std::shared_ptr<Companion>;
// using CompanionResult = ActionSharedValueResult<Companion>;
// using ActionResultPtr = std::shared_ptr<ActionResult>;
using ActionPtr = std::shared_ptr<Action>;
// using CompanionActionPtr = std::shared_ptr<CompanionAction>;
// using PasswordActionPtr = std::shared_ptr<PasswordAction>;
// using FileActionPtr = std::shared_ptr<FileAction>;
// using MessagePtr = std::shared_ptr<Message>;
using MessageMetaDataPtr = std::shared_ptr<MessageMetaData>;
using MessageDataPtr = std::shared_ptr<MessageData>;
using MessageStatePtr = std::shared_ptr<MessageState>;

template <typename T, typename...Ts>
std::shared_ptr<T> buildObjectFromJson(const nlohmann::json &data, Ts &&...args)
{
    auto object = std::make_shared<T>();
    auto result = object->setFields(data, args...);

    if (!result)
        logTemplateError("{}, error parsing jsonData", __FUNCTION__);

    return (result) ? object : nullptr;
}

class MessageHandler
{
public:
    MessageHandler() {}
    ~MessageHandler() {}

    void setManager(std::shared_ptr<Manager> manager);

    void sendMessage(
        MessageType type, CompanionPtr companion, ActionPtr action, const std::string &text);

    void receiveMessage(CompanionPtr companion, const std::string &json);

    void receiveTextMessage(
        CompanionPtr companion, MessageMetaDataPtr meta, MessageDataPtr data, MessageStatePtr state);

    void receiveFileProposalMessage(
        CompanionPtr companion, MessageMetaDataPtr meta, MessageDataPtr data, MessageStatePtr state);

    void receiveConfirmation(
        CompanionPtr companion, MessageMetaDataPtr meta, MessageDataPtr data, MessageStatePtr state);

    void receiveConfirmationRequest(
        CompanionPtr companion, MessageMetaDataPtr meta, MessageDataPtr data, MessageStatePtr state);

    void reciveChatHistoryRequest(CompanionPtr companion);
    void receiveChatHistoryData(CompanionPtr companion, const nlohmann::json &data);
    void receiveFileRequest(CompanionPtr companion, MessageMetaDataPtr meta);
    void receiveFileData(CompanionPtr companion, MessageMetaDataPtr meta, MessageDataPtr data);
    void receiveFileDataCheck(CompanionPtr companion, MessageMetaDataPtr meta, bool success);
    void receiveFileDataTransmissionEnd(CompanionPtr companion, MessageMetaDataPtr meta);
    void receiveFileDataTransmissionFailure(CompanionPtr companion, MessageMetaDataPtr meta);

    MessageDataPtr buildMessageDataFromJson(const nlohmann::json &jsonData);
    MessageMetaDataPtr buildMessageMetaDataFromJson(const nlohmann::json &jsonData);
    MessageStatePtr buildMessageStateFromJson(const nlohmann::json &jsonData);




private:
    std::shared_ptr<Manager> manager_;

};

#endif // MESSAGE_HANDLER_HPP
