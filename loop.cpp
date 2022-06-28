#include <Screeps/Context.hpp>

#include <emscripten.h>
#include <emscripten/bind.h>
#include <Screeps/StructureSpawn.hpp>
#include <Screeps/Room.hpp>
#include <Screeps/Store.hpp>
#include <Screeps/Source.hpp>
#include "GConfig.hpp"
#include "creep/header.h"
#include "creep/Worker.h"
#include "creep/Task.h"
#define HOME_SCREEP "Spawn1"

Screeps::StructureSpawn
getHomeSpawn()
{
    return Screeps::Game.spawns().find(HOME_SCREEP)->second;
}
template <typename T>
std::vector<std::unique_ptr<T>>
getInRoom(Screeps::Room &room, const int type, std::function<bool(const JS::Value &)> predicate = {})
{
    auto roomObjects = room.find(type, std::move(predicate));

    std::vector<std::unique_ptr<T>> list;
    for (const auto &item : roomObjects)
    {
        std::unique_ptr<T> object(new T(item->value()));
        list.push_back(std::move(object));
    }
    return list;
}

EMSCRIPTEN_KEEPALIVE
extern "C" void loop()
{
    Screeps::Context::update();

    auto home = getHomeSpawn();
    auto room = home.room();
    auto sources = getInRoom<Screeps::Source>(room, Screeps::FIND_SOURCES);
    auto source = std::move(sources[0]);
    for (int i = getWorkerNum(); i < 15; i++)
    {
        home.spawnCreep(Worker::bodyParts(), "worker_" + std::to_string(i));
    }
    if (getWorkerNum() == 0)
    {
        auto creeps = getInRoom<Worker>(room, Screeps::FIND_CREEPS);
        initWorkerQueue(creeps);
    }

    // 收集 energy 到 home
    auto harvesterAction = [&](Worker &worker)
    {
        if (worker.store().getFreeCapacity() > 0)
        {
            if (worker.harvest(*source) == Screeps::ERR_NOT_IN_RANGE)
            {
                worker.moveTo(*source);
            }
        }
        else
        {
            if (worker.transfer(home, Screeps::RESOURCE_ENERGY) == Screeps::ERR_NOT_IN_RANGE)
            {
                worker.moveTo(home);
            }
            else
            {
                return true;
            }
        }
        return false;
    };
    if (home.store().getUsedCapacity(Screeps::RESOURCE_ENERGY).value_or(-1) > 0)
    {
        std::string name = "home";
        Task task(name, 0, harvesterAction);
        task_queue.push(task);
    }
    while (!task_queue.empty())
    {
        auto &task = task_queue.top();
        takeTask(task);
    }
    runTask();
}

EMSCRIPTEN_BINDINGS(loop)
{
    emscripten::function("loop", &loop);
}
