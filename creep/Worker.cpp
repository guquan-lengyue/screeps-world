
#include "Worker.h"
#include <Screeps/Constants.hpp>
// 获取worker数量
Worker::Worker(JS::Value creep) : Screeps::Creep(std::move(creep))
{
}
bool Worker::getActionStatus()
{
    if (this->actionStatus < 0)
    {
        JSON memory = this->memory();
        if (memory.contains(ACTION_STATUS))
        {
            this->actionStatus = memory[ACTION_STATUS];
        }
        else
        {
            this->setActionStatus(false);
        }
    }
    return this->actionStatus == 1;
}
void Worker::setActionStatus(bool actionstatus_)
{
    this->actionStatus = actionstatus_;
    JSON memory = this->memory();
    memory[ACTION_STATUS] = actionstatus_ ? 1 : 0;
    this->setMemory(memory);
}
std::vector<std::string> Worker::bodyParts()
{
    return {
        Screeps::MOVE,
        Screeps::CARRY,
        Screeps::WORK};
}
void Worker::setTask(const Task &task)
{
    this->task = std::move(task);
}
bool Worker::work()
{
    return this->task.mission(*this);
}
const bool operator<(const Worker &a, const Worker &b)
{
    return b.ticksToLive() < a.ticksToLive();
}

void initWorkerQueue(std::vector<std::unique_ptr<Worker>> &workers)
{
    for (auto &work : workers)
    {
        if (work->getActionStatus())
        {
            working_queue.push_back(std::move(work));
        }
        else
        {
            ready_queue.push(std::move(work));
        }
    }
}

void takeTask(const Task &task)
{
    const std::unique_ptr<Worker> &worker = ready_queue.top();
    worker->setTask(task);

    working_queue.push_back(std::move(std::make_unique<Worker>(*worker)));
    ready_queue.pop();
}

void runTask()
{

    for (auto bg = working_queue.begin(); bg != working_queue.end(); bg++)
    {
        auto &worker = *bg;
        if (worker->work())
        {
            ready_queue.push(std::move(worker));
            working_queue.erase(bg);
        }
    }
}

// 获取worker数量
int getWorkerNum()
{
    return working_queue.size() + ready_queue.size();
}
