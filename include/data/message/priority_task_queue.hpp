#ifndef PRIORITY_TASK_QUEUE_HPP
#define PRIORITY_TASK_QUEUE_HPP

#include <cstdint>
#include <string>

#include "alias.hpp"

class PriorityTask
{
public:
    PriorityTask(
        uint8_t priority, const std::string &key, std::function<void()> function,
        bool isMandatory = true);

    ~PriorityTask();

    uint8_t priority() const;
    std::string key() const;
    std::function<void()> func() const;
    bool isMandatory() const;

private:
    uint8_t priority_;
    std::string key_;
    std::function<void()> func_;
    bool isMandatory_;
};

class PriorityTaskQueueWrapper
{
public:
    PriorityTaskQueueWrapper(const std::vector<PriorityTask> &tasks);
    ~PriorityTaskQueueWrapper();

    bool runTasksAndReturnResult();

private:
    PriorityTaskQueue queue_;
};

#endif // PRIORITY_TASK_QUEUE_HPP
