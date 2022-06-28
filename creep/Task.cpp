
#include <string>
#include <functional>
#include <queue>
#include "Task.h"


Task::Task(std::string publisher, int level, std::function<bool(Worker &)> action)
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
bool Task::mission(Worker &creep)
{
    return this->action(creep);
}
const bool operator<(const Task &a, const Task &b)
{
    return a.level < b.level;
}
