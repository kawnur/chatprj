#ifndef MESSAGE_HANDLER_HPP
#define MESSAGE_HANDLER_HPP

#include <memory>
#include <string>

#include <nlohmann/json.hpp>

#include "constants.hpp"
#include "logging.hpp"
#include "message_storage.hpp"

class Action;
class Companion;
class Manager;
class MessageData;
class MessageMetaData;
class MessageState;

using CompanionPtr = std::shared_ptr<Companion>;
using ActionPtr = std::shared_ptr<Action>;
using MessagePtr = std::shared_ptr<Message>;
using MessageInfoPtr = std::shared_ptr<MessageInfo>;
using MessageMetaDataPtr = std::shared_ptr<MessageMetaData>;
using MessageDataPtr = std::shared_ptr<MessageData>;
using MessageStatePtr = std::shared_ptr<MessageState>;

template <typename T>
std::shared_ptr<T> buildObjectFromJson(
    CompanionPtr companion, const nlohmann::json &data)
{
    auto object = std::make_shared<T>();
    auto result = object->setFields(companion, data);

    if (!result)
        logTemplateError("{}, error parsing jsonData", __FUNCTION__);

    return (result) ? object : nullptr;
}

class MessageHandler
{
public:
    MessageHandler();
    ~MessageHandler() = default;

    void setManager(std::shared_ptr<Manager> manager);

    MessageInfoPtr getMessageInfo(CompanionPtr companion, MessagePtr message);

    MessageMetaDataPtr buildMessageMetaData(
        CompanionPtr companion, std::shared_ptr<DBReplyData> messagesData, std::size_t index);

    MessageDataPtr buildMessageData(
        std::shared_ptr<DBReplyData> messagesData, std::size_t index);

    MessageStatePtr buildMessageState(
        std::shared_ptr<DBReplyData> messagesData, std::size_t index);

    std::shared_ptr<Message> createMessageAndAddToMapping(
        CompanionPtr companion, std::shared_ptr<DBReplyData> messagesData, std::size_t index);

    std::shared_ptr<Message> createMessage(
        CompanionPtr companion, MessageMetaDataPtr meta, MessageDataPtr data,
        MessageStatePtr state);

    void sendMessage(
        MessageType type, CompanionPtr companion, ActionPtr action, const std::string &text);

    bool sendMessageByNetwork(
        CompanionPtr companion, std::shared_ptr<Message> message,
        MessageMetaDataPtr meta);

    bool sendJsonByNetwork(CompanionPtr companion, const std::string &json);

    bool sendChatHistoryByNetwork(
        CompanionPtr companion, std::shared_ptr<DBReplyData> data, std::vector<std::string> &keys);

    bool sendFileBlockByNetwork(
        CompanionPtr companion, const std::string &networkId, const std::string &data);

    void sendFile(CompanionPtr companion, const std::filesystem::path &path);

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
    void receiveFileDataCheck(CompanionPtr companion, MessageMetaDataPtr meta);
    void receiveFileDataTransmissionEnd(CompanionPtr companion, MessageMetaDataPtr meta);
    void receiveFileDataTransmissionFailure(CompanionPtr companion, MessageMetaDataPtr meta);
    void addEarlyMessages(CompanionPtr companion);
    void sendUnsentMessages(CompanionPtr companion);
    void fillCompanionMessageMapping(CompanionPtr companion, bool containersNotEmpty);

    void requestHistoryFromCompanion(CompanionPtr companion);
    void waitForMessageReceptionConfirmation(CompanionPtr companion, MessagePtr message);

    bool setMappedMessageWidget(
        CompanionPtr companion, MessagePtr message, std::shared_ptr<MessageWidget> widget);

    bool clearMessageMapping(CompanionPtr companion);
    bool addCompanionToMessageStorage(CompanionPtr companion);

private:
    std::shared_ptr<Manager> manager_;
    MessageStorage messageStorage_;
};

std::string buildMessageJSONString(
    std::shared_ptr<Companion> companion, std::shared_ptr<Message> message,
    MessageMetaDataPtr meta);

std::string buildFileBlockJSONString(
    std::shared_ptr<Companion> companion, const std::string &networkId, const std::string &data);

#endif // MESSAGE_HANDLER_HPP
