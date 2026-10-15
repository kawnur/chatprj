#ifndef COMPANION_HPP
#define COMPANION_HPP

#include <cstdint>
#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>

#include "logging.hpp"
#include "file_info.hpp"

class CentralPanelWidget;
class ChatClient;
class ChatServer;
class CompanionData;
class DBReplyData;
class FileManager;
class FileMessageWidget;
class Message;
class MessageData;
class MessageInfo;
class MessageMetaData;
class MessageState;
class MessageWidget;
class WidgetGroup;

using MessageMap = std::unordered_map<uint32_t, std::shared_ptr<MessageInfo>>;
using MessageMappingIterator = MessageMap::iterator;

class Companion : public std::enable_shared_from_this<Companion>
{
public:
    Companion(int id, const std::string &name);
    ~Companion() = default;

    friend bool operator<(const Companion &object1, const Companion &object2)
    {
        return object1.id() < object2.id();
    }

    int id() const;
    std::string name() const;

    // MessageMappingIterator getMessageMappingIteratorByMessage(std::shared_ptr<Message> message);
    // std::shared_ptr<MessageWidget> getMappedMessageWidgetByMessage(std::shared_ptr<Message> message);

    // std::shared_ptr<Message> getMappedMessageByMessageWidget(
    //     bool lock, std::shared_ptr<MessageWidget> widget);

    // std::shared_ptr<Message> getMappedMessageByMessageWidget(bool lock, MessageWidget *widget);

    // std::shared_ptr<MessageState> getMappedMessageStateByMessageWidget(
    //     bool lock, std::shared_ptr<MessageWidget> widget);

    // std::shared_ptr<MessageInfo> getMessageInfoByMessageId(uint32_t messageId);
    // std::shared_ptr<MessageInfo> getMessageInfoByNetworkId(const std::string &networkId);
    // std::shared_ptr<Message> getEarliestMessage() const;

    // std::shared_ptr<Message> createMessage(
    //     // MessageType type, /*uint32_t messageId, uint8_t authorId, const std::string &messageTime,*/
    //     // std::shared_ptr<MessageMetaData> meta, const std::string &messageText, bool isAntecedent,
    //     // bool isSent, bool isReceived, std::string networkId);
    //     std::shared_ptr<MessageMetaData> meta, std::shared_ptr<MessageData> data,
    //     std::shared_ptr<MessageState> state);

    // std::shared_ptr<MessageInfo> createMessageAndAddToMapping(
    //     std::shared_ptr<DBReplyData> messagesData, std::size_t index);

    // void setMappedMessageWidget(
    //     std::shared_ptr<Message> message, std::shared_ptr<MessageWidget> widget);

    void updateData(std::shared_ptr<CompanionData> data);
    // std::shared_ptr<Message> findMessage(uint32_t messageId);

    // void addMessageWidgetsToChatHistory();
    // void addMessageWidgetsToChatHistory(std::shared_ptr<WidgetGroup> group);
    // void clearMessageMapping();
    // std::string generateNetworkId(bool);

private:
    // std::mutex mutex_;
    int id_;  // TODO change type
    std::string name_;
    // MessageMap messageMapping_;
};

#endif // COMPANION_HPP
