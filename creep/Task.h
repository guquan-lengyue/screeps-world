#ifndef EXAMPLE_CREEPS_TASK_H
#define EXAMPLE_CREEPS_TASK_H

#include <string>
#include <functional>
#include <vector>
#include "header.h"
class Task
{
private:
    std::string publisher;
    int level;
    std::function<bool(Worker &)> action = {};

public:
    Task() = default;
    Task(std::string publisher, int level);
    std::string getPublisher();
    Task(std::string publisher, int level, std::function<bool(Worker &)> action);
    friend const bool operator<(const Task &a, const Task &b);

    ~Task();
    bool mission(Worker &creep);
};


#endif