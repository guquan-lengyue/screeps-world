#ifndef EXAMPLE_CREEPS_H
#define EXAMPLE_CREEPS_H
#include <queue>
#include <Screeps/Creep.hpp>
#include <vector>
#include <Screeps/Constants.hpp>
namespace ct
{
    class Worker;

    std::vector<std::unique_ptr<Worker>> working_queue;
    std::priority_queue<std::unique_ptr<Worker>> ready_queue;

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

    // 获取worker数量
    int getworkerNum()
    {
        return working_queue.size() + ready_queue.size();
    }
    const bool operator<(const Worker &a, const Worker &b)
    {
        return b.ticksToLive() < a.ticksToLive();
    }

    class Worker : public Screeps::Creep
    {
    private:
        int actionStatus = -1;
        const std::string ACTION_STATUS = "actionStatus";
        std::unique_ptr<Task> task;

    public:
        explicit Worker(JS::Value creep);
        friend const bool operator<(const Worker &a, const Worker &b);
        bool getActionStatus();
        void setActionStatus(bool actionStatus);
        static std::vector<std::string> bodyParts();
        void setTask(std::unique_ptr<Task> &task);

        ~Worker();
    };

    Worker::Worker(JS::Value creep) : Screeps::Creep(std::move(creep))
    {
    }
    Worker::~Worker()
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
    void Worker::setActionStatus(bool actionstatus)
    {
        this->actionStatus = actionStatus;
        JSON memory = this->memory();
        memory[ACTION_STATUS] = actionstatus ? 1 : 0;
        this->setMemory(memory);
    }
    std::vector<std::string> Worker::bodyParts()
    {
        return {
            Screeps::MOVE,
            Screeps::CARRY,
            Screeps::WORK};
    }
}
#endif