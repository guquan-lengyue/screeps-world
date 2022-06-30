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
#include <memory>
#include <optional>
#include <Screeps/StructureController.hpp>
#include "creep/Harvester.hpp"
#include "creep/Upgrader.hpp"
#include "creep/Builder.hpp"
#include "creep/Repairer.hpp"
#include <iostream>
#include <iterator>
#include <algorithm>
#include "GConfig.hpp"
#define HOME_SCREEP "Spawn1"

int HARVESTER_HAVE = 0;
int UPGRADER_HAVE = 0;
int BUILDER_HAVE = 0;
int REPAIRER_HAVE = 0;

std::map<std::string, int> containerLevel;
bool containerLevelCmp(const std::unique_ptr<Screeps::StructureExtension> &a, const std::unique_ptr<Screeps::StructureExtension> &b)
{
    if (containerLevel.empty())
    {
        containerLevel.insert(std::make_pair(Screeps::STRUCTURE_EXTENSION, 0));
        containerLevel.insert(std::make_pair(Screeps::STRUCTURE_CONTAINER, 3));
        containerLevel.insert(std::make_pair(Screeps::STRUCTURE_STORAGE, 2));
        containerLevel.insert(std::make_pair(Screeps::STRUCTURE_TOWER, 1));
    }
    return containerLevel[a->structureType()] < containerLevel[b->structureType()];
}

std::shared_ptr<Screeps::StructureSpawn>
getHomeSpawn()
{
    auto home = Screeps::Game.spawns().find(HOME_SCREEP)->second;
    return std::make_shared<Screeps::StructureSpawn>(home.value());
}

std::shared_ptr<Screeps::Room>
getRoom(const std::shared_ptr<Screeps::StructureSpawn> &spawn)
{
    return std::make_shared<Screeps::Room>(spawn->room().value());
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

std::optional<Screeps::StructureController>
getController(Screeps::Room &room)
{
    return room.controller();
}

std::shared_ptr<Screeps::Structure>
getContainer(Screeps::Room &room, bool empty)
{

    auto structures = getInRoom<Screeps::StructureExtension>(room, Screeps::FIND_STRUCTURES);
    std::sort(structures.begin(), structures.end(), containerLevelCmp);
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
                    return std::make_shared<Screeps::Structure>(item->value());
                }
            }
            else if ((int)item->structureType().find(Screeps::STRUCTURE_EXTENSION) < 0 &&
                     (int)item->structureType().find(Screeps::STRUCTURE_TOWER) < 0)
            {
                if (item->store().getUsedCapacity(Screeps::RESOURCE_ENERGY).value_or(-1) > 0)
                {
                    return std::make_shared<Screeps::Structure>(item->value());
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
            return std::make_unique<Screeps::Creep>(creep->value());
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
    std::unique_ptr<Screeps::Structure> maxDamageStructure;
    for (int i = 0; i < structures.size(); i++)
    {
        if (structures[i]->hits() < structures[i]->hitsMax())
        {
            if (maxDamageStructure == nullptr)
            {
                maxDamageStructure = std::move(structures[i]);
            }
            else if (maxDamageStructure->hits() > structures[i]->hits() && structures[i]->hits() >= 1)
            {
                maxDamageStructure = std::move(structures[i]);
            }
        }
    }
    return maxDamageStructure;
}
void spawnHarvester(Screeps::StructureSpawn &spawn, int number)
{
    for (int i = 0; i < number; i++)
    {
        for (int j = 2; j >= 0; j--)
        {
            int result = spawn.spawnCreep(Harvester::bodyParts(j), Harvester::namePre() + std::to_string(i));
            if (result > 0)
            {
                break;
            }
        }
    }
}

void spawnUpgrader(Screeps::StructureSpawn &spawn, int number)
{
    for (int i = 0; i < number; i++)
    {
        for (int j = 2; j >= 0; j--)
        {
            int result = spawn.spawnCreep(Upgrader::bodyParts(j), Upgrader::namePre() + std::to_string(i));
            if (result > 0)
            {
                break;
            }
        }
    }
}

void spawnBuilder(Screeps::StructureSpawn &spawn, int number)
{
    for (int i = 0; i < number; i++)
    {
        for (int j = 2; j >= 0; j--)
        {
            int result = spawn.spawnCreep(Builder::bodyParts(j), Builder::namePre() + std::to_string(i));
            if (result > 0)
            {
                break;
            }
        }
    }
}

void spawnRepairer(Screeps::StructureSpawn &spawn, int number)
{
    for (int i = 0; i < number; i++)
    {
        for (int j = 2; j >= 0; j--)
        {
            int result = spawn.spawnCreep(Repairer::bodyParts(j), Repairer::namePre() + std::to_string(i));
            if (result > 0)
            {
                break;
            }
        }
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
        GConfig::update();
        std::cout << "home contain :  U: " << home->store().getUsedCapacity(Screeps::RESOURCE_ENERGY).value_or(0) << " / f: " << home->store().getFreeCapacity(Screeps::RESOURCE_ENERGY).value_or(-1) << std::endl;
        std::cout << "HARVESTER :" << HARVESTER_HAVE << " / " << GConfig::getHarvesterNum() << std::endl;
        std::cout << "UPGRADER :" << UPGRADER_HAVE << " / " << GConfig::getUpgraderNum() << std::endl;
        std::cout << "BUILDER :" << BUILDER_HAVE << " / " << GConfig::getBuilderNum() << std::endl;
        std::cout << "REPAIRER :" << REPAIRER_HAVE << " / " << GConfig::getRepairerNum() << std::endl;
        if (damageStructure != nullptr)
        {
            std::cout << damageStructure->pos().x() << "," << damageStructure->pos().y() << " hits :" << damageStructure->hits() << std::endl;
        }
        auto storage = getInRoom<Screeps::StructureStorage>(
            *room, Screeps::FIND_MY_STRUCTURES, [](const JS::Value &value)
            { return (int)value["structureType"].as<std::string>().find(Screeps::STRUCTURE_STORAGE) >= 0; });
        for (const auto &item : storage)
        {
            auto u = item->store().getUsedCapacity(Screeps::RESOURCE_ENERGY).value_or(-1);
            auto f = item->store().getFreeCapacity(Screeps::RESOURCE_ENERGY).value_or(-1);
            std::cout << "storage :  U: " << u << " / F: " << f << std::endl;
        }
    }
    if (emptyContainer == nullptr)
    {
        emptyContainer = home;
    }
    if (fullContainer == nullptr && HARVESTER_HAVE > GConfig::getHarvesterNum() / 2)
    {
        fullContainer = home;
    }

    auto constructionSites = getInRoom<Screeps::ConstructionSite>(*room, Screeps::FIND_CONSTRUCTION_SITES);
    if (HARVESTER_HAVE > GConfig::getHarvesterNum() / 2)
    {
        if (damageStructure != nullptr)
        {
            spawnRepairer(*home, GConfig::getRepairerNum());
        }
        if (!constructionSites.empty())
        {
            spawnBuilder(*home, GConfig::getBuilderNum());
        }
        spawnUpgrader(*home, GConfig::getUpgraderNum());
    }

    spawnHarvester(*home, GConfig::getHarvesterNum());

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
            if (!droppedResource.empty() && false)
            {
                Harvester(creep.value()).work(*(droppedResource[0]), *store);
            }
            else
            {
                Harvester(creep.value()).work(*source, *store);
            }
        }
        if (fullContainer == nullptr || creep.store().getUsedCapacity().value_or(-1) > 0)
        {
            continue;
        }
        if ((int)creep.name().find(Upgrader::namePre()) >= 0)
        {
            Upgrader(creep.value()).work(*fullContainer, *controller);
            ++UPGRADER_HAVE;
        }
        if ((int)creep.name().find(Builder::namePre()) >= 0 && !constructionSites.empty())
        {
            Builder(creep.value()).work(*fullContainer, *(constructionSites[0]));
            ++BUILDER_HAVE;
        }
        if ((int)creep.name().find(Repairer::namePre()) >= 0 && damageStructure != nullptr)
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
            else if (damageStructure != nullptr)
            {
                item->repair(*damageStructure);
            }
        }
    }
}

EMSCRIPTEN_BINDINGS(loop)
{
    emscripten::function("loop", &loop);
}
