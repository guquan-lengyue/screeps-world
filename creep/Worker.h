#ifndef EXAMPLE_CREEPS_WORKER_H
#define EXAMPLE_CREEPS_WORKER_H
#include <Screeps/Creep.hpp>
#include <queue>
#include "header.h"
#include "Task.h"
class Worker : public Screeps::Creep
{
private:
    int actionStatus = -1;
    const std::string ACTION_STATUS = "actionStatus";
    Task task;

public:
    Worker() = delete;
    explicit Worker(JS::Value creep);
    Worker(JS::Value creep, Task task);
    friend const bool operator<(const Worker &a, const Worker &b);
    bool getActionStatus();
    void setActionStatus(bool actionStatus);
    static std::vector<std::string> bodyParts();
    void setTask(const Task &task);
    bool work();

    ~Worker();
};
#endif
