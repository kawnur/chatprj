#ifndef MESSAGE_BUILDER_HPP
#define MESSAGE_BUILDER_HPP

#include "alias.hpp"
#include "constants.hpp"

class MessageBuilder
{
public:
    MessageBuilder();
    ~MessageBuilder();

    MessageMetaDataPtr buildMessageMetaData(
        CompanionPtr companion, DBReplyDataPtr data, std::size_t index);

    MessageMetaDataPtr buildMyTextMessageMetaData(MessageType type, CompanionPtr companion);
    MessageDataPtr buildMessageData(DBReplyDataPtr data, std::size_t index);
    MessageStatePtr buildMessageState(DBReplyDataPtr data, std::size_t index);

    MessagePtr buildMessageFromDbData(CompanionPtr companion, DBReplyDataPtr dbData, std::size_t index);

    MessagePtr buildMyTextMessage(
        MessageType type, CompanionPtr companion, const std::string &text);
};

#endif // MESSAGE_BUILDER_HPP
