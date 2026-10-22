#ifndef MESSAGE_BUILDER_HPP
#define MESSAGE_BUILDER_HPP

#include "alias.hpp"
#include "constants.hpp"

class MessageBuilder
{
public:
    MessageBuilder() = default;
    ~MessageBuilder() = default;

    MessageMetaDataPtr buildMessageMetaData(
        CompanionPtr companion, DBReplyDataPtr data, std::size_t index);

    MessageMetaDataPtr buildUnsentMessageMetaData(
        CompanionPtr companion, DBReplyDataPtr data, std::size_t index);

    MessageMetaDataPtr buildMyTextMessageMetaData(MessageType type, CompanionPtr companion);
    MessageDataPtr buildMessageData(DBReplyDataPtr data, std::size_t index);
    MessageStatePtr buildMessageState(bool isAntecedent, DBReplyDataPtr data, std::size_t index);

    MessagePtr buildMessageFromDbData(
        CompanionPtr companion, DBReplyDataPtr dbData, std::size_t index);

    MessagePtr buildMyTextMessage(
        MessageType type, CompanionPtr companion, const std::string &text);

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
