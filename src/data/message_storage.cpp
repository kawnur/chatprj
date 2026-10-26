#include "message_storage.hpp"

#include <algorithm>
#include <utility>

#include "message.hpp"
#include "utils.hpp"

CompanionMessageStorage::CompanionMessageStorage() : mutex_(), map_() {}

MessageInfoPtr CompanionMessageStorage::getMessageInfo(int id)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto lambda = [&]() { return map_.at(id); };

    return runAndReturnSharedPtr<MessageInfo>(lambda);
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

    if (meta->networkId().empty())
        meta->setNetworkId(generateNetworkId(false));

    auto result = map_.emplace(meta->messageId(), info);

    return result.second;
}

void CompanionMessageStorage::clear()
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

bool CompanionMessageStorage::setMessageWidget(int id, MessageWidgetPtr widget)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto result = map_.find(id);

    if (result == map_.end())
        return false;

    auto info = result->second;

    if (!info)
        return false;

    info->setWidget(widget);

    return true;
}

MessageStorage::MessageStorage() : mutex_(), storage_() {}

MessageInfoPtr MessageStorage::getMessageInfo(int companionId, int messageId)
{
    std::lock_guard<std::mutex> lock(mutex_);

    // TODO use util
    auto result = storage_.find(companionId);

    return (result == storage_.end()) ? nullptr : result->second.getMessageInfo(messageId);
}

MessageInfoPtr MessageStorage::getMessageInfoByNetworkId(int companionId, const std::string &networkId)
{
    std::lock_guard<std::mutex> lock(mutex_);

    // TODO use util
    auto iter = storage_.find(companionId);
    bool result = (iter != storage_.end());

    if (result) {
        return iter->second.getMessageInfoByNetworkId(networkId);
    } else {
        logTemplateError("companion with id '{}' not found", companionId);

        return nullptr;
    }
}

MessagePtr MessageStorage::getEarliestMessage(int companionId)
{
    auto lambda = [&]() { return storage_.at(companionId).getEarliestMessage(); };

    return runAndReturnSharedPtr<Message>(lambda);
}

bool MessageStorage::updateAndInsert(int companionId, MessageInfoPtr info)
{
    auto lambda = [&]() { return storage_.at(companionId).updateAndInsert(info); };

    return runAndReturnBool(lambda);
}

bool MessageStorage::setMessageWidget(int companionId, int messageId, MessageWidgetPtr widget)
{
    auto lambda = [&]() { storage_.at(companionId).setMessageWidget(messageId, widget); };

    return runAndReturnBool(lambda);
}

bool MessageStorage::clearMessageMapping(int companionId)
{
    auto lambda = [&]() { storage_.at(companionId).clear(); };

    return runAndReturnBool(lambda);
}

bool MessageStorage::addCompanion(int companionId)
{
    auto result = storage_.try_emplace(companionId);

    return result.second;
}
