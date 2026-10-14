#include "message_storage.hpp"

#include <algorithm>

#include "message.hpp"
#include "utils.hpp"
// #include "widgets.hpp"

CompanionMessageStorage::CompanionMessageStorage() : mutex_(), map_() {}

MessageInfoPtr CompanionMessageStorage::getMessageInfo(int id)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto result = map_.find(id);

    return (result == map_.end()) ? nullptr : result->second;
}

MessageInfoPtr CompanionMessageStorage::getMessageInfoByNetworkId(const std::string &networkId)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto lambda = [&](const auto &iter)
    {
        return iter.second->message()->networkId() == networkId;
    };

    auto result = std::ranges::find_if(map_, lambda);

    return (result == map_.end()) ? nullptr : result->second;
}

MessagePtr CompanionMessageStorage::getEarliestMessage()
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto lambda = [&](const auto &iterator1, const auto &iterator2)
    {
        return iterator1.second->message()->id() < iterator2.second->message()->id();
    };

    auto min = std::ranges::min_element(map_, lambda);

    return min->second->message();
}

bool CompanionMessageStorage::updateAndInsert(MessageInfoPtr info)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto meta = info->message()->meta();

    if (meta->networkId_.empty())
        meta->networkId_ = generateNetworkId(false);

    auto result = map_.emplace(meta->messageId_, info);

    return result.second;
}

// void CompanionMessageStorage::addMessageWidgetsToChatHistory(std::shared_ptr<WidgetGroup> group)
// {
//     std::lock_guard<std::mutex> lock(mutex_);

//     for (auto &iterator : map_)
//         group->addMessageWidgetToCentralPanelChatHistory(iterator.second->getMessage());
// }

void CompanionMessageStorage::clearMessageMapping()
{
    std::lock_guard<std::mutex> lock(mutex_);

    map_.clear();
}

std::string CompanionMessageStorage::generateNetworkId(bool lock)
{
    if (lock)
        std::lock_guard<std::mutex> lockObject(mutex_);

    std::string networkId { "" };

    if (networkId.size() == 0) {  // TODO ???
        networkId = getNetworkId();

        auto lambda = [&]()
        {
            auto lambdaInternal = [&](const auto &iter)
            {
                return iter.second->message()->networkId() == networkId;
            };

            auto iterator = std::ranges::find_if(map_, lambdaInternal);

            return !(iterator == map_.end());
        };

        // loop while generated key is not unique
        while (lambda())
            networkId = getNetworkId();
    }

    return networkId;
}

MessageStorage::MessageStorage() : mutex_(), storage_() {}

MessageInfoPtr MessageStorage::getMessageInfo(int companionId, int messageId)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto result = storage_.find(companionId);

    return (result == storage_.end()) ? nullptr : result->second.getMessageInfo(messageId);
}

MessageInfoPtr MessageStorage::getMessageInfoByNetworkId(int companionId, const std::string &networkId)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto result = storage_.find(companionId);

    return (result == storage_.end()) ? nullptr : result->second.getMessageInfoByNetworkId(networkId);
}

bool MessageStorage::updateAndInsert(int companionId, MessageInfoPtr info)
{
    auto lambda = [&]() { return storage_.at(companionId).updateAndInsert(info); };

    return runAndReturnBool(lambda);
}
