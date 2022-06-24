#include <Screeps/Context.hpp>
#include <Screeps/Creep.hpp>
#include <Screeps/Room.hpp>
#include <Screeps/RoomObject.hpp>
#include <Screeps/RoomPosition.hpp>
#include <Screeps/Source.hpp>
#include <Screeps/Resource.hpp>
#include <Screeps/StructureSpawn.hpp>
#include <Screeps/StructureStorage.hpp>
#include <Screeps/StructureExtension.hpp>
#include <Screeps/Constants.hpp>
#include <Screeps/ConstructionSite.hpp>
#include <Screeps/StructureTower.hpp>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <optional>
#include <Screeps/StructureController.hpp>
#include "creep/Harvester.hpp"
#include "creep/Upgrader.hpp"
#include "creep/Builder.hpp"
#include "creep/Repairer.hpp"
#include <iostream>
#include <emscripten.h>
#include <iterator>
#define HARVESTER_NUM 10
#define UPGRADER_NUM 4
#define BUILDER_NUM 2
#define REPAIRER_NUM 2
#define HOME_SCREEP "Spawn1"

int HARVESTER_HAVE = 0;
int UPGRADER_HAVE = 0;
int BUILDER_HAVE = 0;
int REPAIRER_HAVE = 0;
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
getInRoom(Screeps::Room &room, const int type, std::function<bool(const JS::Value &)> predicate = {})
{
    auto roomObjects = room.find(type, predicate);

    std::vector<std::unique_ptr<T>> list;
    for (const auto &item : roomObjects)
    {
        std::unique_ptr<T> object(new T(item->value()));
        list.push_back(std::move(object));
    }
    return list;
}

std::optional<Screeps::StructureController>
getController(Screeps::Room &room)
{
    return room.controller();
}

std::shared_ptr<Screeps::Structure>
getContainer(Screeps::Room &room, bool empty)
{
    auto structures = getInRoom<Screeps::StructureExtension>(room, Screeps::FIND_MY_STRUCTURES);
    for (const auto &item : structures)
    {
        if ((int)item->structureType().find(Screeps::STRUCTURE_EXTENSION) >= 0 ||
            (int)item->structureType().find(Screeps::STRUCTURE_CONTAINER) >= 0 ||
            (int)item->structureType().find(Screeps::STRUCTURE_STORAGE) >= 0 ||
            (int)item->structureType().find(Screeps::STRUCTURE_TOWER) >= 0)
        {
            if (empty)
            {
                if (item->store().getFreeCapacity(Screeps::RESOURCE_ENERGY).value_or(-1) > 0)
                {
                    return std::shared_ptr<Screeps::Structure>(new Screeps::Structure(item->value()));
                }
            }
            else
            {
                if (item->store().getUsedCapacity(Screeps::RESOURCE_ENERGY).value_or(-1) > 0 &&
                    (int)item->structureType().find(Screeps::STRUCTURE_STORAGE) >= 0)
                {
                    return std::shared_ptr<Screeps::Structure>(new Screeps::Structure(item->value()));
                }
            }
        }
    }
    return nullptr;
}

std::unique_ptr<Screeps::Creep> getEnemy(Screeps::Room &room)
{
    auto creeps = getInRoom<Screeps::Creep>(room, Screeps::FIND_CREEPS);

    for (const auto &creep : creeps)
    {
        if (!creep->my())
        {
            return std::unique_ptr<Screeps::Creep>(new Screeps::Creep(creep->value()));
        }
    }
    return nullptr;
}

std::unique_ptr<Screeps::Structure>
getDamageStructure(Screeps::Room &room)
{
    auto structures = getInRoom<Screeps::Structure>(room, Screeps::FIND_STRUCTURES);
    if (structures.empty())
    {
        return nullptr;
    }
    std::unique_ptr<Screeps::Structure> maxDamageStructure = std::move(structures[0]);
    for (int i = 1; i < structures.size(); i++)
    {
        if (structures[i]->hits() < structures[i]->hitsMax())
        {
            if (maxDamageStructure->hits() < structures[i]->hits())
            {
                maxDamageStructure = std::move(structures[i]);
            }
        }
    }
    if (maxDamageStructure->hits() < maxDamageStructure->hitsMax())
    {
        return maxDamageStructure;
    }
    else
    {
        return nullptr;
    }
}
void spawnHarvester(Screeps::StructureSpawn &spawn, int number)
{
    for (int i = 0; i < number; i++)
    {
        spawn.spawnCreep(Harvester::bodyParts(), Harvester::namePre() + std::to_string(i));
    }
}

void spawnUpgrader(Screeps::StructureSpawn &spawn, int number)
{
    for (int i = 0; i < number; i++)
    {
        spawn.spawnCreep(Upgrader::bodyParts(), Upgrader::namePre() + std::to_string(i));
    }
}

void spawnBuilder(Screeps::StructureSpawn &spawn, int number)
{
    for (int i = 0; i < number; i++)
    {
        spawn.spawnCreep(Builder::bodyParts(), Builder::namePre() + std::to_string(i));
    }
}

void spawnRepairer(Screeps::StructureSpawn &spawn, int number)
{
    for (int i = 0; i < number; i++)
    {
        spawn.spawnCreep(Repairer::bodyParts(), Repairer::namePre() + std::to_string(i));
    }
}

EMSCRIPTEN_KEEPALIVE
extern "C" void loop()
{
    Screeps::Context::update();
    auto home = getHomeSpawn();
    auto room = getRoom(home);
    auto sources = getInRoom<Screeps::Source>(*room, Screeps::FIND_SOURCES);
    auto droppedResource = getInRoom<Screeps::Resource>(*room, Screeps::FIND_DROPPED_RESOURCES);
    auto damageStructure = getDamageStructure(*room);
    auto controller = getController(*room);
    auto fullContainer = getContainer(*room, false);
    auto emptyContainer = getContainer(*room, true);

    if (Screeps::Game.time() % 50 == 0)
    {
        std::cout << "home contain :  U: " << home->store().getUsedCapacity(Screeps::RESOURCE_ENERGY).value_or(0) << " / f: " << home->store().getFreeCapacity(Screeps::RESOURCE_ENERGY).value_or(-1) << std::endl;
        std::cout << "HARVESTER :" << HARVESTER_HAVE << std::endl;
        std::cout << "UPGRADER :" << UPGRADER_HAVE << std::endl;
        std::cout << "BUILDER :" << BUILDER_HAVE << std::endl;
        std::cout << "REPAIRER :" << REPAIRER_HAVE << std::endl;
    }
    if (fullContainer == nullptr)
    {
        fullContainer = home;
    }
    if (emptyContainer == nullptr)
    {
        emptyContainer = home;
    }

    auto constructionSites = getInRoom<Screeps::ConstructionSite>(*room, Screeps::FIND_CONSTRUCTION_SITES);
    if (HARVESTER_HAVE > HARVESTER_NUM / 2)
    {
        if (damageStructure != nullptr)
        {
            spawnRepairer(*home, REPAIRER_NUM);
        }
        if (!constructionSites.empty())
        {
            spawnBuilder(*home, BUILDER_NUM);
        }
        spawnUpgrader(*home, UPGRADER_NUM);
    }
    spawnHarvester(*home, HARVESTER_NUM);

    HARVESTER_HAVE = 0;
    UPGRADER_HAVE = 0;
    BUILDER_HAVE = 0;
    REPAIRER_HAVE = 0;

    for (const auto &item : Screeps::Game.creeps())
    {
        auto creep = item.second;
        if ((int)creep.name().find(Harvester::namePre()) >= 0)
        {
            const auto &source = sources[++HARVESTER_HAVE % 2];

            auto store = home->store().getFreeCapacity(Screeps::RESOURCE_ENERGY).value() > 0 ? home : emptyContainer;
            if (!droppedResource.empty())
            {
                Harvester(creep.value()).work(*(droppedResource[0]), *store);
            }
            else
            {
                Harvester(creep.value()).work(*source, *store);
            }
        }
        else if ((int)creep.name().find(Upgrader::namePre()) >= 0)
        {
            Upgrader(creep.value()).work(*fullContainer, *controller);
            ++UPGRADER_HAVE;
        }
        else if ((int)creep.name().find(Builder::namePre()) >= 0 && !constructionSites.empty())
        {
            Builder(creep.value()).work(*fullContainer, *(constructionSites[0]));
            ++BUILDER_HAVE;
        }
        else if ((int)creep.name().find(Repairer::namePre()) >= 0 && damageStructure != nullptr)
        {
            Repairer(creep.value()).work(*fullContainer, *damageStructure);
            ++REPAIRER_HAVE;
        }
    }
    auto towers = getInRoom<Screeps::StructureTower>(*room, Screeps::FIND_MY_STRUCTURES);
    auto enemy = getEnemy(*room);
    for (const auto &item : towers)
    {
        if ((int)item->structureType().find(Screeps::STRUCTURE_TOWER) >= 0)
        {
            if (enemy != nullptr)
            {
                item->attack(*enemy);
            }
        }
    }
}

EMSCRIPTEN_BINDINGS(loop)
{
    emscripten::function("loop", &loop);
}
