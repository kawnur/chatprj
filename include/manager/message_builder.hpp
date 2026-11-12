#ifndef MESSAGE_BUILDER_HPP
#define MESSAGE_BUILDER_HPP

#include <variant>

#include <nlohmann/json.hpp>

#include "alias.hpp"
#include "constants.hpp"
#include "logging.hpp"

using MessagePtrVariant = std::variant<std::nullptr_t,
                                       TextMessagePtr,
                                       ReceiveConfirmationMessagePtr,
                                       ReceiveConfirmationRequestMessagePtr>;

template <typename T>
std::shared_ptr<T> buildObjectFromJson(CompanionPtr companion, const nlohmann::json &data)
{
    auto object = std::make_shared<T>();
    auto result = object->setFields(companion, data);

    if (!result) {
        logTemplateError("{}, error parsing jsonData", __FUNCTION__);

        return nullptr;
    }

    return object;
}

class MessageBuilder
{
public:
    MessageBuilder() = default;
    ~MessageBuilder() = default;

    MessagePtrVariant buildMessage(CompanionPtr companion, const std::string &json);

    template<class M>
    MessagePtrVariant buildMessageObject(CompanionPtr companion, const nlohmann::json &json)
    {
        // build meta
        auto meta = buildObjectFromJson<typename M::meta_type>(companion, json);

        if (!meta)
            return nullptr;

        // build data
        auto data = buildObjectFromJson<typename M::data_type>(companion, json);

        if (!data)
            return nullptr;

        // build state
        auto state = buildObjectFromJson<typename M::state_type>(companion, json);

        if (!state)
            return nullptr;

        auto message = std::make_shared<M>(meta, data, state);

        return MessagePtrVariant(message);
    }

    MessageMetaDataPtr buildMessageMetaData(
        CompanionPtr companion, DBReplyDataPtr data, std::size_t index);

    MessageMetaDataPtr buildUnsentMessageMetaData(
        CompanionPtr companion, DBReplyDataPtr data, std::size_t index);

    MessageMetaDataPtr buildMyTextMessageMetaData(CompanionPtr companion);
    MessageDataPtr buildMessageData(DBReplyDataPtr data, std::size_t index);
    MessageStatePtr buildMessageState(bool isAntecedent, DBReplyDataPtr data, std::size_t index);

    MessagePtr buildMessageFromDbData(
        CompanionPtr companion, DBReplyDataPtr dbData, std::size_t index);

    TextMessagePtr buildMyTextMessage(CompanionPtr companion, const std::string &text);

    MessagePtr buildUnsentTextMessageFromDbData(
        CompanionPtr companion, DBReplyDataPtr dbData, std::size_t index);

    ReceiveConfirmationMessagePtr buildReceiveConfirmationMessage(MessagePtr message);
    ReceiveConfirmationRequestMessagePtr buildReceiveConfirmationRequestMessage(MessagePtr message);
    MessagePtr buildFileDataCheckResultMessage(NetworkMessageType type, MessagePtr message);

    MessagePtr buildFileDataTransmissionResultMessage(
        NetworkMessageType type, const std::string &networkId);

    MessagePtr buildFileProposalReplyMessage(MessagePtr message);
    MessagePtr buildChatHistoryRequestMessage(CompanionPtr companion);
    MessagePtr buildFileRequestMessage(const std::string &networkId);
};

#endif // MESSAGE_BUILDER_HPP
