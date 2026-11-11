#include "message_state.hpp"

#include "logging.hpp"
#include "priority_task_queue.hpp"

MessageState::MessageState(bool isAntecedent, bool isSent, bool isReceived)
    : isAntecedent_(isAntecedent), isSent_(isSent), isReceived_(isReceived) {}

MessageState::MessageState(MessageStatePtr state)
{
    isAntecedent_ = state->isAntecedent_;
    isSent_ = state->isSent_;
    isReceived_ = state->isReceived_;
}

bool MessageState::setFields(CompanionPtr companion, const nlohmann::json &data)
{
    std::vector<PriorityTask> taskVector
        {
            { 0, "antecedent", [&]() { isAntecedent_ = data.at("antecedent"); }, false },
            { 0, "received", [&]() { isReceived_ = data.at("received"); }, false }
        };

    PriorityTaskQueueWrapper wrapper(taskVector);
    bool result = wrapper.runTasksAndReturnResult();

    return result;
}

bool MessageState::isAntecedent() const
{
    return isAntecedent_;
}

void MessageState::setIsAntecedent(bool newIsAntecedent)
{
    isAntecedent_ = newIsAntecedent;
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
    auto entryTemplate = "isAntecedent: {0}, isSent: {1}, isReceived: {2}"s;

    logTemplateInfo(entryTemplate, isAntecedent_, isSent_, isReceived_);
}

ReceiveConfirmationMessageState::ReceiveConfirmationMessageState(
    bool isAntecedent, bool isSent, bool isReceived)
    : MessageState(isAntecedent, isSent, isReceived) {}

nlohmann::json ReceiveConfirmationMessageState::getJsonData()
{
    nlohmann::json data;

    data["received"] = isReceived_;

    return data;
}
