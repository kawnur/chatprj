#ifndef TASK_HPP
#define TASK_HPP

#include <list>

class Task
{
public:
    Task();
    ~Task();

private:
    bool result_;

};

class TaskChain
{
public:
    TaskChain();
    ~TaskChain();

    void appendTask(Task &&task);

private:
    std::list<Task> tasks_;
};

#endif // TASK_HPP
