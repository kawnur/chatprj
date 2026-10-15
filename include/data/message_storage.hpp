#ifndef MESSAGE_STORAGE_HPP
#define MESSAGE_STORAGE_HPP

#include <cstdint>
#include <memory>
#include <mutex>
#include <unordered_map>

class Message;
class MessageInfo;
class MessageWidget;
class WidgetGroup;

using MessagePtr = std::shared_ptr<Message>;
using MessageInfoPtr = std::shared_ptr<MessageInfo>;
using MessageMap = std::unordered_map<uint32_t, MessageInfoPtr>;
using MessageWidgetPtr = std::shared_ptr<MessageWidget>;

class CompanionMessageStorage
{
public:
    CompanionMessageStorage();
    ~CompanionMessageStorage() = default;

    MessageInfoPtr getMessageInfo(int id);
    MessageInfoPtr getMessageInfoByNetworkId(const std::string &networkId);
    MessagePtr getEarliestMessage();
    bool updateAndInsert(MessageInfoPtr info);
    // void addMessageWidgetsToChatHistory(std::shared_ptr<WidgetGroup> group);
    void clear();
    std::string generateNetworkId(bool);
    bool setMessageWidget(int id, MessageWidgetPtr widget);

private:
    std::mutex mutex_;
    MessageMap map_;
};

using Storage = std::unordered_map<uint32_t, CompanionMessageStorage>;

class MessageStorage
{
public:
    MessageStorage();
    ~MessageStorage() = default;

    MessageInfoPtr getMessageInfo(int companionId, int messageId);
    MessageInfoPtr getMessageInfoByNetworkId(int companionId, const std::string &networkId);
    MessagePtr getEarliestMessage(int companionId);
    bool updateAndInsert(int companionId, MessageInfoPtr info);
    bool setMessageWidget(int companionId, int messageId, MessageWidgetPtr widget);
    bool clearMessageMapping(int companionId);
    bool addCompanion(int companionId);

private:
    std::mutex mutex_;
    Storage storage_;
};

#endif // MESSAGE_STORAGE_HPP
