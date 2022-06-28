#ifndef EXAMPLE_CREEPS_TASK_H
#define EXAMPLE_CREEPS_TASK_H
#include <string>
#include <functional>
#include "Worker.hpp"
#include <queue>

namespace ct
{
    class Task;
    std::priority_queue<Task> task_queue;
    const bool operator<(const Task &a, const Task &b)
    {
        return a.level < b.level;
    }
    class Task
    {
    private:
        std::string publisher;
        int level;
        std::function<bool(ct::Worker &)> action;

    public:
        Task(std::string publisher, int level);
        std::string getPublisher();
        Task(std::string publisher, int level, std::function<bool(ct::Worker &)> action);
        friend const bool operator<(const Task &a, const Task &b);

        ~Task();
        void mission(ct::Worker &creep);
    };

    Task::Task(std::string publisher, int level, std::function<bool(ct::Worker &)> action)
    {
        this->publisher = publisher;
        this->level = level;
        this->action = action;
    }
    Task::Task(std::string publisher, int level)
    {
        this->publisher = publisher;
        this->level = level;
    }

    Task::~Task()
    {
    }

    std::string Task::getPublisher()
    {
        return this->publisher;
    }
    void Task::mission(ct::Worker &creep)
    {
        this->action(creep);
    }

}
#endif
