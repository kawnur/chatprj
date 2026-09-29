#include "task.hpp"

#include "logging.hpp"

Task::Task() : result_(false) {}

Task::~Task() { logArgsInfo(Q_FUNC_INFO); }

TaskChain::TaskChain() : tasks_() {}

TaskChain::~TaskChain() { logArgsInfo(Q_FUNC_INFO); }

void TaskChain::appendTask(Task &&task)
{
    tasks_.push_back(task);
}
