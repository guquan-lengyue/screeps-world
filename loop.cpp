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

#define HARVESTER_NUM 16
#define UPGRADER_NUM 5
#define BUILDER_NUM 5
#define HOME_SCREEP "home"

std::shared_ptr<Screeps::StructureSpawn>
getHomeSpawn()
{
    auto home = Screeps::Game.spawns().find(HOME_SCREEP)->second;
    return std::unique_ptr<Screeps::StructureSpawn>(new Screeps::StructureSpawn(home.value()));
}
std::shared_ptr<Screeps::Room>
getRoom(std::shared_ptr<Screeps::StructureSpawn> spawn)
{
    return std::shared_ptr<Screeps::Room>(new Screeps::Room(spawn->room().value()));
}

template <typename T>
std::vector<std::unique_ptr<T>>
getInRoom(std::shared_ptr<Screeps::Room> room, int type, std::function<bool(const JS::Value &)> predicate = {})
{
    auto roomObjects = room->find(type, predicate);
    std::vector<std::unique_ptr<T>> list(roomObjects.size());
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

    std::shared_ptr<Screeps::StructureSpawn> homeSpawn = getHomeSpawn();
    auto room = getRoom(homeSpawn);

    auto constructionSites = getInRoom<Screeps::ConstructionSite>(room, Screeps::FIND_CONSTRUCTION_SITES);
    std::unique_ptr<Screeps::ConstructionSite> constructionSite;
    if (!constructionSites.empty())
    {
        std::unique_ptr<Screeps::ConstructionSite> s(
            new Screeps::ConstructionSite(constructionSites.begin()->get()->value()));
        constructionSite = std::move(s);
    }

    std::vector<std::unique_ptr<Screeps::Resource>> sources = getInRoom<Screeps::Resource>(room, Screeps::FIND_SOURCES);
    std::unique_ptr<Screeps::Source> source;
    for (const auto &item : sources)
    {
        std::unique_ptr<Screeps::Source> s(new Screeps::Source(item->value()));
        if (s->energyCapacity() > 0)
        {
            source = std::move(s);
            break;
        }
    }

    auto structurePredtion = [&](const JS::Value &value)
    {
        std::string structureType = value["structureType"].as<std::string>();
        return structureType == Screeps::STRUCTURE_EXTENSION ||
               structureType == Screeps::STRUCTURE_CONTAINER;
    };
    auto structures = getInRoom<Screeps::Structure>(room, Screeps::FIND_STRUCTURES, structurePredtion);

    std::shared_ptr<Screeps::Structure> emptyContainer = homeSpawn;
    std::shared_ptr<Screeps::Structure> fullContainer = homeSpawn;

    if (!structures.empty())
    {
        Screeps::StructureSpawn store(structures.begin()->get()->value());
        if (store.store().getFreeCapacity() > 0)
        {
            std::shared_ptr<Screeps::Structure> s(new Screeps::Structure(store.value()));
            emptyContainer = std::move(s);
        }
        if (store.store().getUsedCapacity() > 50)
        {
            std::shared_ptr<Screeps::Structure> s(new Screeps::Structure(store.value()));
            fullContainer = std::move(s);
        }
    }
    for (int i = 0; i < BUILDER_NUM; i++)
    {
        homeSpawn->spawnCreep(Builder::bodyParts(), Builder::namePre() + std::to_string(i));
    }
    for (int i = 0; i < UPGRADER_NUM; i++)
    {
        homeSpawn->spawnCreep(Upgrader::bodyParts(), Upgrader::namePre() + std::to_string(i));
    }
    for (int i = 0; i < HARVESTER_NUM; i++)
    {
        homeSpawn->spawnCreep(Harvester::bodyParts(), Harvester::namePre() + std::to_string(i));
    }

    if (emptyContainer == nullptr)
    {
        std::cout << "emptyContainer is null";
    }
    int harvesterIndex = 0;

    std::map<std::string, Screeps::Creep> creeps = Screeps::Game.creeps();
    for (const auto &creep : creeps)
    {
        std::string creepName = creep.second.name();
        if (creepName.find(Harvester::namePre()) != -1)
        {
            ++harvesterIndex;
            Harvester harvester(creep.second.value());
            Screeps::Source s(sources[harvesterIndex % 2]->value());
            harvester.work(s, *emptyContainer);
        }
    }
    Screeps::StructureController homeController = room->controller().value();
    if (harvesterIndex > HARVESTER_NUM / 2)
        for (const auto &creep : creeps)
        {
            std::string creepName = creep.second.name();
            if (creepName.find(Upgrader::namePre()) != -1)
            {
                Upgrader upgrader(creep.second.value());
                upgrader.work(*fullContainer, homeController);
            }
            if (creepName.find(Builder::namePre()) != -1)
            {
                Builder builder(creep.second.value());
                builder.work(*fullContainer, *constructionSite);
            }
        }
}

EMSCRIPTEN_BINDINGS(loop)
{
    emscripten::function("loop", &loop);
}
