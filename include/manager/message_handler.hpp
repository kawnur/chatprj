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

    MessagePtr createMessageAndAddToMapping(
        CompanionPtr companion, DBReplyDataPtr messagesData, std::size_t index);

    MessagePtr createMessage1(
        CompanionPtr companion, MessageMetaDataPtr meta, MessageDataPtr data,
        MessageStatePtr state);

    bool addToStorage(CompanionPtr companion, MessagePtr message);

    void sendMessage(
        MessageType type, CompanionPtr companion, ActionPtr action, const std::string &text);

    bool sendMessageByNetwork(CompanionPtr companion, MessagePtr message, MessageMetaDataPtr meta);

    bool sendChatHistoryByNetwork(
        CompanionPtr companion, DBReplyDataPtr data, std::vector<std::string> &keys);

    bool sendFileBlockByNetwork(
        CompanionPtr companion, const std::string &networkId, const std::string &data);

    // void sendFile(CompanionPtr companion, const std::filesystem::path &path);
    void receiveMessage(CompanionPtr companion, const std::string &json);

    void receiveTextMessage(
        CompanionPtr companion, MessageMetaDataPtr meta, MessageDataPtr data, MessageStatePtr state);

    void receiveConfirmation(
        CompanionPtr companion, MessageMetaDataPtr meta, MessageDataPtr data, MessageStatePtr state);

    void receiveConfirmationRequest(
        CompanionPtr companion, MessageMetaDataPtr meta, MessageDataPtr data, MessageStatePtr state);

    void reciveChatHistoryRequest(CompanionPtr companion);
    void receiveChatHistoryData(CompanionPtr companion, const nlohmann::json &data);

    void receiveFileProposal(
        CompanionPtr companion, MessageMetaDataPtr meta, MessageDataPtr data, MessageStatePtr state);

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
        CompanionPtr companion, MessagePtr message, MessageWidgetPtr widget);

    bool clearMessageMapping(CompanionPtr companion);
    bool addCompanionToMessageStorage(CompanionPtr companion);

private:
    ManagerPtr manager_;
    MessageStorage storage_;
    MessageBuilder builder_;
};

std::string buildMessageJSONString(CompanionPtr companion, MessagePtr message);

std::string buildFileBlockJSONString(
    CompanionPtr companion, const std::string &networkId, const std::string &data);

#endif // MESSAGE_HANDLER_HPP
