#ifndef MESSAGE_HANDLER_HPP
#define MESSAGE_HANDLER_HPP

#include <memory>
#include <string>

#include <nlohmann/json.hpp>

#include "alias.hpp"
#include "constants.hpp"
#include "logging.hpp"
#include "message_builder.hpp"
#include "message_storage.hpp"

template <typename T>
std::shared_ptr<T> buildObjectFromJson(CompanionPtr companion, const nlohmann::json &data)
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

    void setManager(ManagerPtr manager);

    MessageInfoPtr getMessageInfo(CompanionPtr companion, MessagePtr message);

    void sendMessage(
        MessageType type, CompanionPtr companion, ActionPtr action, const std::string &text);

    bool sendChatHistory(
        CompanionPtr companion, DBReplyDataPtr data, std::vector<std::string> &keys);

    bool sendFileBlock(
        CompanionPtr companion, const std::string &networkId, const std::string &data);

    bool sendFileRequest(CompanionPtr companion, const std::string &networkId);

    bool sendFileDataTransmissionResultMessage(  // NOT USED
        CompanionPtr companion, NetworkMessageType type, const std::string &networkId);

    void receiveMessage(CompanionPtr companion, const std::string &json);
    void addEarlyMessages(CompanionPtr companion);
    void sendUnsentMessages(CompanionPtr companion);
    void fillMessageStorageForCompanion(CompanionPtr companion, bool containersAreEmpty);
    void requestHistoryFromCompanion(CompanionPtr companion);

    bool setMappedMessageWidget(
        CompanionPtr companion, MessagePtr message, MessageWidgetPtr widget);

    bool clearMessageMapping(CompanionPtr companion);
    bool addCompanionToMessageStorage(CompanionPtr companion);

private:
    void addMessage1(CompanionPtr companion, DBReplyDataPtr messagesData, std::size_t index);
    bool addToStorage(CompanionPtr companion, MessagePtr message);
    bool buildAndSendConfirmation(CompanionPtr companion, MessagePtr message);
    bool sendMessageByNetwork(CompanionPtr companion, MessagePtr message);

    void receiveTextMessage(CompanionPtr companion, MessagePtr message);
    void receiveConfirmation(CompanionPtr companion, MessagePtr message);
    void receiveConfirmationRequest(CompanionPtr companion, MessagePtr message);
    void reciveChatHistoryRequest(CompanionPtr companion);
    void receiveChatHistoryData(CompanionPtr companion, const nlohmann::json &data);
    void receiveFileProposal(CompanionPtr companion, MessagePtr message);
    void receiveFileRequest(CompanionPtr companion, MessagePtr message);
    void receiveFileData(CompanionPtr companion, MessagePtr message);
    void receiveFileDataCheck(CompanionPtr companion, MessagePtr message);
    void receiveFileDataTransmissionEnd(CompanionPtr companion, MessagePtr message);
    void receiveFileDataTransmissionFailure(CompanionPtr companion, MessagePtr message);

    void waitForMessageReceptionConfirmation(CompanionPtr companion, MessagePtr message);

    ManagerPtr manager_;
    MessageStorage storage_;
    MessageBuilder builder_;
};

std::string buildMessageJSONString(CompanionPtr companion, MessagePtr message);

std::string buildFileBlockJSONString(
    CompanionPtr companion, const std::string &networkId, const std::string &data);

#endif // MESSAGE_HANDLER_HPP
