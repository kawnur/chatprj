#include "message_state.hpp"

#include "logging.hpp"
#include "priority_task_queue.hpp"

MessageState::MessageState(bool isSent, bool isReceived)
    : isSent_(isSent), isReceived_(isReceived) {}

MessageState::MessageState(MessageStatePtr state)
{
    isSent_ = state->isSent_;
    isReceived_ = state->isReceived_;
}

bool MessageState::setFields(CompanionPtr companion, const nlohmann::json &data)
{
    std::vector<PriorityTask> taskVector
        {
            { 0, "received", [&]() { isReceived_ = data.at("received"); }, false }
        };

    PriorityTaskQueueWrapper wrapper(taskVector);
    bool result = wrapper.runTasksAndReturnResult();

    return result;
}

bool MessageState::isSent() const
{
    return isSent_;
}

void MessageState::setIsSent(bool newIsSent)
{
    isSent_ = newIsSent;
}

bool MessageState::isReceived() const
{
    return isReceived_;
}

void MessageState::setIsReceived(bool newIsReceived)
{
    isReceived_ = newIsReceived;
}

void MessageState::log()
{
    auto entryTemplate= "isSent: {0}, isReceived: {1}"s;

    logTemplateInfo(entryTemplate, isSent_, isReceived_);
}

IncomingTextMessageState::IncomingTextMessageState(bool isSent, bool isReceived, bool isRead)
    : MessageState(isSent, isReceived), isRead_(isRead) {}

bool IncomingTextMessageState::isRead()
{
    return isRead_;
}

void IncomingTextMessageState::setIsRead(bool newIsRead)
{
    isRead_ = newIsRead;
}

ReceiveConfirmationMessageState::ReceiveConfirmationMessageState(bool isSent, bool isReceived)
    : MessageState(isSent, isReceived) {}

nlohmann::json ReceiveConfirmationMessageState::getJsonData()
{
    nlohmann::json data;

    data["received"] = isReceived_;

    return data;
}
