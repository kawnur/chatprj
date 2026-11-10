#ifndef MESSAGE_BUILDER_HPP
#define MESSAGE_BUILDER_HPP

#include <nlohmann/json.hpp>

#include "alias.hpp"
#include "constants.hpp"
#include "logging.hpp"

template <typename T>
std::shared_ptr<T> buildObjectFromJson(CompanionPtr companion, const nlohmann::json &data)
{
    auto object = std::make_shared<T>();
    auto result = object->setFields(companion, data);

    if (!result)
        logTemplateError("{}, error parsing jsonData", __FUNCTION__);

    return (result) ? object : nullptr;
}

class MessageBuilder
{
public:
    MessageBuilder() = default;
    ~MessageBuilder() = default;

    MessagePtr buildMessage(CompanionPtr companion, const std::string &json);

    MessageMetaDataPtr buildMessageMetaData(
        CompanionPtr companion, DBReplyDataPtr data, std::size_t index);

    MessageMetaDataPtr buildUnsentMessageMetaData(
        CompanionPtr companion, DBReplyDataPtr data, std::size_t index);

    MessageMetaDataPtr buildMyTextMessageMetaData(CompanionPtr companion);
    MessageDataPtr buildMessageData(DBReplyDataPtr data, std::size_t index);
    MessageStatePtr buildMessageState(bool isAntecedent, DBReplyDataPtr data, std::size_t index);

    MessagePtr buildMessageFromDbData(
        CompanionPtr companion, DBReplyDataPtr dbData, std::size_t index);

    MessagePtr buildMyTextMessage(CompanionPtr companion, const std::string &text);

    MessagePtr buildUnsentTextMessageFromDbData(
        CompanionPtr companion, DBReplyDataPtr dbData, std::size_t index);

    MessagePtr buildReceiveConfirmationMessage(MessagePtr message);
    MessagePtr buildReceiveConfirmationRequestMessage(MessagePtr message);
    MessagePtr buildFileDataCheckResultMessage(NetworkMessageType type, MessagePtr message);

    MessagePtr buildFileDataTransmissionResultMessage(
        NetworkMessageType type, const std::string &networkId);

    MessagePtr buildFileProposalReplyMessage(MessagePtr message);
    MessagePtr buildChatHistoryRequestMessage(CompanionPtr companion);
    MessagePtr buildFileRequestMessage(const std::string &networkId);
};

#endif // MESSAGE_BUILDER_HPP
