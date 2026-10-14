#include "companion.hpp"

#include "action.hpp"
#include "constants.hpp"
#include "data.hpp"
#include "db_interaction.hpp"
#include "file_info.hpp"
#include "manager.hpp"
#include "message.hpp"
#include "utils.hpp"
#include "widgets.hpp"
#include "widgets_message.hpp"

Companion::Companion(int id, const std::string &name)
    : /*mutex_(std::mutex()), */id_(id), name_(name)/*, messageMapping_()*/ {}

int Companion::id() const
{
    return id_;
}

std::string Companion::name() const
{
    return name_;
}

// MessageMappingIterator Companion::getMessageMappingIteratorByMessage(
//     std::shared_ptr<Message> message)
// {
//     std::lock_guard<std::mutex> lock(mutex_);

//     auto lambda = [&](const auto &iter)
//     {
//         return iter.second->getMessage() == message;
//     };

//     return std::ranges::find_if(messageMapping_, lambda);
// }

// std::shared_ptr<MessageWidget> Companion::getMappedMessageWidgetByMessage(
//     std::shared_ptr<Message> message)
// {
//     auto result = getMessageMappingIteratorByMessage(message);

//     return (result == messageMapping_.end()) ? nullptr : result->second->getWidget();
// }

// std::shared_ptr<Message> Companion::getMappedMessageByMessageWidget(
//     bool lock, std::shared_ptr<MessageWidget> widget)
// {
//     return getMappedMessageByMessageWidget(lock, widget.get());
// }

// std::shared_ptr<Message> Companion::getMappedMessageByMessageWidget(
//     bool lock, MessageWidget *widget)
// {
//     if (lock)
//         std::lock_guard<std::mutex> lockObject(mutex_);

//     auto lambda = [&](const auto &iter) { return iter.second->getWidget().get() == widget; };
//     auto result = std::ranges::find_if(messageMapping_, lambda);

//     return (result == messageMapping_.end()) ? nullptr : result->second->getMessage();
// }

// std::shared_ptr<MessageState> Companion::getMappedMessageStateByMessageWidget(
//     bool lock, std::shared_ptr<MessageWidget> widget)
// {
//     if (lock)
//         std::lock_guard<std::mutex> lockObject(mutex_);

//     auto lambda = [&](const auto &iter)
//     {
//         return iter.second->getWidget() == widget;
//     };

//     auto result = std::ranges::find_if(messageMapping_, lambda);

//     return (result == messageMapping_.end()) ? nullptr : result->second->getMessage()->state();
// }

// std::shared_ptr<MessageInfo> Companion::getMessageInfoByMessageId(uint32_t messageId)
// {
//     std::lock_guard<std::mutex> lock(mutex_);

//     auto lambda = [&](const auto &iter)
//     {
//         return iter.second->getMessage()->getId() == messageId;
//     };

//     auto result = std::ranges::find_if(messageMapping_, lambda);

//     return (result == messageMapping_.end()) ? nullptr : result->second;
// }

// std::shared_ptr<MessageInfo> Companion::getMessageInfoByNetworkId(const std::string &networkId)
// {
//     std::lock_guard<std::mutex> lock(mutex_);

//     auto lambda = [&](const auto &iter)
//     {
//         return iter.second->getMessage()->getNetworkId() == networkId;
//     };

//     auto result = std::ranges::find_if(messageMapping_, lambda);

//     return (result == messageMapping_.end()) ? nullptr : result->second;
// }

// std::shared_ptr<Message> Companion::getEarliestMessage() const
// {
//     auto lambda = [&](const auto &iterator1, const auto &iterator2)
//     {
//         return iterator1.second->getMessage()->getId() < iterator2.second->getMessage()->getId();
//     };

//     auto min = std::ranges::min_element(messageMapping_, lambda);

//     return min->second->getMessage();
// }

// std::shared_ptr<Message> Companion::createMessage(
//     // MessageType type, /*uint32_t messageId, uint8_t authorId, const std::string &messageTime,*/
//     // std::shared_ptr<MessageMetaData> meta, const std::string &messageText, bool isAntecedent,
//     // bool isSent, bool isReceived, std::string networkId)
//     std::shared_ptr<MessageMetaData> meta, std::shared_ptr<MessageData> data,
//     std::shared_ptr<MessageState> state)
// {
//     std::lock_guard<std::mutex> lock(mutex_);

//     if (meta->networkId_.size() == 0)
//         meta->networkId_ = generateNetworkId(false);

//     // auto messageState = std::make_shared<MessageState>(
//     //     companionId, isAntecedent, isSent, isReceived, networkId);

//     auto message = std::make_shared<Message>(meta, data, state);
//     auto info = std::make_shared<MessageInfo>(message, nullptr);
//     auto result = messageMapping_.emplace(meta->messageId_, info);

//     // return result;
//     return (result.second) ? message : nullptr;
// }

// std::shared_ptr<MessageInfo> Companion::createMessageAndAddToMapping(
//     std::shared_ptr<DBReplyData> messagesData, std::size_t index)
// {
//     std::lock_guard<std::mutex> lock(mutex_);

//     auto id = id_;

//     auto meta = std::make_shared<MessageMetaData>(
//         MessageType::TEXT, NetworkMessageType::UNKNOWN,
//         getIntFromString(ID_BAD_VALUE, messagesData->getValue(index, "id")), id, name_,
//         getIntFromString(ID_BAD_VALUE, messagesData->getValue(index, "author_id")), "",
//         messagesData->getValue(index, "timestamp_tz"), "", generateNetworkId(false));

//     auto data = std::make_shared<MessageData>(messagesData->getValue(index, "message"), "");

//     auto state = std::make_shared<MessageState>(
//         false,
//         getBoolFromDBValue(messagesData->getValue(index, "is_sent")),
//         getBoolFromDBValue(messagesData->getValue(index, "is_received")));

//     auto message = std::make_shared<Message>(meta, data, state);
//     auto info = std::make_shared<MessageInfo>(message, nullptr);
//     auto result = messageMapping_.emplace(std::make_pair(message->getId(), info));

//     if (!result.second)
//         logArgsError("message info map emplacing error");

//     return (result.second) ? info : nullptr;
// }

void Companion::setMappedMessageWidget(
    std::shared_ptr<Message> message, std::shared_ptr<MessageWidget> widget)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto id = message->getId();
    auto result = messageMapping_.find(id);

    if (result == messageMapping_.end())
        logTemplateError("message with id {} was not found in messageMapping_", id);
    else
        result->second->setWidget(widget);
}

void Companion::updateData(std::shared_ptr<CompanionData> data)
{
    name_ = data->name();
    getManager()->updateSocketInfo(id_, data);
}

// std::shared_ptr<Message> Companion::findMessage(uint32_t messageId)
// {
//     std::lock_guard<std::mutex> lock(mutex_);

//     auto result = messageMapping_.find(messageId);

//     return (result == messageMapping_.end()) ? nullptr : result->second->getMessage();
// }

void Companion::addMessageWidgetsToChatHistory()
{
    auto group = getManager()->getMappedWidgetGroupByCompanion(shared_from_this());

    addMessageWidgetsToChatHistory(group);
}

void Companion::addMessageWidgetsToChatHistory(std::shared_ptr<WidgetGroup> group)
{
    std::lock_guard<std::mutex> lock(mutex_);

    for (auto &iterator : messageMapping_)
        group->addMessageWidgetToCentralPanelChatHistory(iterator.second->getMessage());
}

// void Companion::clearMessageMapping()
// {
//     std::lock_guard<std::mutex> lock(mutex_);

//     messageMapping_.clear();
// }

// std::string Companion::generateNetworkId(bool lock)
// {
//     if (lock)
//         std::lock_guard<std::mutex> lockObject(mutex_);

//     std::string networkId { "" };

//     if (networkId.size() == 0) {  // TODO ???
//         networkId = getNetworkId();

//         auto lambda = [&]()
//         {
//             auto lambdaInternal = [&](const auto &iter)
//             {
//                 return iter.second->getMessage()->getNetworkId() == networkId;
//             };

//             auto iterator = std::ranges::find_if(messageMapping_, lambdaInternal);

//             return !(iterator == messageMapping_.end());
//         };

//         // loop while generated key is not unique
//         while (lambda())
//             networkId = getNetworkId();
//     }

//     return networkId;
// }
