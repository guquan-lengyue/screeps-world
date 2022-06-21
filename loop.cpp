#include <Screeps/Context.hpp>
#include <Screeps/Creep.hpp>
#include <Screeps/Room.hpp>
#include <Screeps/RoomObject.hpp>
#include <Screeps/RoomPosition.hpp>
#include <Screeps/Source.hpp>
#include <Screeps/Resource.hpp>
#include <Screeps/StructureSpawn.hpp>
#include <Screeps/StructureStorage.hpp>
#include <Screeps/Constants.hpp>
#include <Screeps/ConstructionSite.hpp>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <optional>
#include <Screeps/StructureController.hpp>
#include "creep/Harvester.hpp"
#include "creep/Upgrader.h"
#include "creep/Builder.h"
#include <iostream>

#define HARVESTER_NUM 10
#define UPGRADER_NUM 5
#define BUILDER_NUM 5
#define HOME_SCREEP "home"

std::shared_ptr<Screeps::StructureSpawn>
getHomeSpawn()
{
    auto home = Screeps::Game.spawns().find(HOME_SCREEP)->second;
    return std::shared_ptr<Screeps::StructureSpawn>(new Screeps::StructureSpawn(home.value()));
}

std::shared_ptr<Screeps::Room>
getRoom(std::shared_ptr<Screeps::StructureSpawn> spawn)
{
    return std::shared_ptr<Screeps::Room>(new Screeps::Room(spawn->room().value()));
}

template <typename T>
std::vector<std::unique_ptr<T>>
getInRoom(std::shared_ptr<Screeps::Room> room, const int type, std::function<bool(const JS::Value &)> predicate = {})
{
    auto roomObjects = room->find(type, predicate);

    std::vector<std::unique_ptr<T>> list;
    for (const auto &item : roomObjects)
    {
        std::unique_ptr<T> object(new T(item->value()));
        list.push_back(std::move(object));
    }
    return list;
}

std::optional<Screeps::StructureController> getController(Screeps::Room &room)
{
    return room.controller();
}

void spawnHarvester(Screeps::StructureSpawn &spawn, int number)
{
    for (short i = 0; i < number; i++)
    {
        spawn.spawnCreep(Harvester::bodyParts(), Harvester::namePre());
    }  
}

void spawnUpgrader(Screeps::StructureSpawn &spawn, int number)
{
    for (short i = 0; i < number; i++)
    {
        spawn.spawnCreep(Upgrader::bodyParts(), Upgrader::namePre());
    }
}

EMSCRIPTEN_KEEPALIVE
extern "C" void loop()
{
    Screeps::Context::update();

    auto home = getHomeSpawn();
    auto room = getRoom(home);
    auto sources = getInRoom<Screeps::Source>(room, Screeps::FIND_SOURCES);
    auto controller = getController(*room);
    // if (controller.has_value())
    // {
    //     spawnUpgrader(*home, UPGRADER_NUM);
    // }
    spawnHarvester(*home, HARVESTER_NUM);

    for (const auto &item : Screeps::Game.creeps())
    {
        auto creep = item.second;
        if (item.first.find(Harvester::namePre()) >= 0)
        {   
            const auto &source = sources[1];
            Harvester(creep.value()).work(*source, *home);
        }
        // else if (controller.has_value() && item.first.find(Upgrader::namePre()) >= 0)
        // {
        //     const auto &source = sources[0];
        //     Upgrader(creep.value()).work(*source, controller.value());
        // }
    }
}

EMSCRIPTEN_BINDINGS(loop)
{
    emscripten::function("loop", &loop);
}
