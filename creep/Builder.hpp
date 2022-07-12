//
// Created by 19278 on 2022/6/14.
//

#include <Screeps/Creep.hpp>
#include <Screeps/ConstructionSite.hpp>
#include <Screeps/Structure.hpp>
#include <Screeps/Constants.hpp>
#include <Screeps/StructureContainer.hpp>
#include "opts.hpp"
#ifndef EXAMPLE_BUILDER_H
#define EXAMPLE_BUILDER_H

#define BUILDER_ACTION "isBuilding"

class Builder : public Screeps::Creep
{
public:
    explicit Builder(JS::Value creep);

    static std::string namePre() { return "Builder_"; };

    static std::vector<std::string> bodyParts(int level);

    void work(Screeps::RoomObject &source, Screeps::ConstructionSite &target);
};

Builder::Builder(JS::Value creep) : Screeps::Creep(std::move(creep))
{
}

std::vector<std::string> Builder::bodyParts(int level)
{
    std::vector<std::string> bodys;
    for (int i = 0; i < level; ++i)
    {
        bodys.push_back(Screeps::WORK);
        bodys.push_back(Screeps::CARRY);
        bodys.push_back(Screeps::MOVE);
        bodys.push_back(Screeps::MOVE);
    }
    return bodys;
}

void Builder::work(Screeps::RoomObject &source, Screeps::ConstructionSite &target)
{
    JSON memory = this->memory();
    if (!memory.contains(BUILDER_ACTION))
    {
        memory[BUILDER_ACTION] = true;
    }
    bool isBuilding;
    memory[BUILDER_ACTION].get_to(isBuilding);
    if (isBuilding && this->store().getUsedCapacity(Screeps::RESOURCE_ENERGY).value_or(-1) == 0)
    {
        isBuilding = false;
        this->say(SAY_HARVEST);
    }
    if (!isBuilding && this->store().getFreeCapacity(Screeps::RESOURCE_ENERGY).value_or(-1) == 0)
    {
        isBuilding = true;
        this->say(SAY_BUILD);
    }

    memory[BUILDER_ACTION] = isBuilding;
    this->setMemory(memory);
    if (isBuilding)
    {
        if (this->build(target) == Screeps::ERR_NOT_IN_RANGE)
        {
            this->moveTo(target, moveToOpt());
        }
    }
    else
    {
        if (this->ticksToLive() < 10)
        {
            this->suicide();
            return;
        }
        if (this->withdraw(source, Screeps::RESOURCE_ENERGY) == Screeps::ERR_NOT_IN_RANGE)
        {
            if (Screeps::StructureContainer(source.value()).store().getUsedCapacity(Screeps::RESOURCE_ENERGY).value_or(-1) > 40)
            {
                this->moveTo(source, moveToOpt());
            }
        }
    }
}

#endif // EXAMPLE_BUILDER_H
