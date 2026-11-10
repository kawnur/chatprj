#include "priority_task_queue.hpp"

#include "logging.hpp"
#include "utils.hpp"

PriorityTask::PriorityTask(
    uint8_t priority, const std::string &key, std::function<void ()> function, bool isMandatory)
    : priority_(priority), isMandatory_(isMandatory), key_(key), func_(function) {}

PriorityTask::~PriorityTask() { /*logArgsInfo(Q_FUNC_INFO, key_);*/ }

uint8_t PriorityTask::priority() const
{
    return priority_;
}

std::string PriorityTask::key() const
{
    return key_;
}

std::function<void ()> PriorityTask::func() const
{
    return func_;
}

bool PriorityTask::isMandatory() const
{
    return isMandatory_;
}

PriorityTaskQueueWrapper::PriorityTaskQueueWrapper(const std::vector<PriorityTask> &tasks)
{
    auto comparator = [](const auto &item1, const auto &item2)
    {
        return item1.priority() < item2.priority();
    };

    queue_ = PriorityTaskQueue(comparator, tasks);
}

PriorityTaskQueueWrapper::~PriorityTaskQueueWrapper() { logArgsInfo(Q_FUNC_INFO); }

bool PriorityTaskQueueWrapper::runTasksAndReturnResult()
{
    bool result = true;

    while (!queue_.empty()) {
        auto item = queue_.top();
        auto func = item.func();
        bool itemResult = (item.isMandatory()) ? runAndReturnBool(func) : runAndReturnTrue(func);
        result &= itemResult;
        queue_.pop();
    }

    return result;
}
