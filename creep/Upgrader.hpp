//
// Created by 19278 on 2022/6/14.
//

#ifndef EXAMPLE_UPGRADER_H
#define EXAMPLE_UPGRADER_H

#include <Screeps/Creep.hpp>
#include <Screeps/Structure.hpp>
#include <Screeps/Constants.hpp>
#include <Screeps/Store.hpp>
#include "opts.hpp"
#define UPGRADER_ACTION "upgrading"
#define SAY_HARVEST "🔄"
#define SAY_BUILD "🚧"

class Upgrader : public Screeps::Creep
{
public:
    Upgrader(JS::Value creep);

    void work(Screeps::RoomObject &source, Screeps::StructureController &target);

    static std::string namePre() { return "Upgrader_"; };

    static std::vector<std::string> bodyParts();
};

Upgrader::Upgrader(JS::Value creep) : Screeps::Creep(std::move(creep))
{
}

void Upgrader::work(Screeps::RoomObject &source, Screeps::StructureController &target)
{
    JSON memory = this->memory();
    if (!memory.contains(UPGRADER_ACTION))
    {
        memory[UPGRADER_ACTION] = false;
    }
    bool isUpgrading;
    memory[UPGRADER_ACTION].get_to(isUpgrading);
    if (isUpgrading && this->store().getUsedCapacity(Screeps::RESOURCE_ENERGY).value_or(-1) == 0)
    {
        isUpgrading = false;
        this->say(SAY_HARVEST);
    }
    if (!isUpgrading && this->store().getFreeCapacity(Screeps::RESOURCE_ENERGY).value_or(-1) == 0)
    {
        isUpgrading = true;
        this->say(SAY_BUILD);
    }

    memory[UPGRADER_ACTION] = isUpgrading;
    this->setMemory(memory);

    if (isUpgrading)
    {
        if (this->upgradeController(target) == Screeps::ERR_NOT_IN_RANGE)
        {
            this->moveTo(target, moveToOpt());
        }
    }
    else
    {
        if (this->withdraw(source, Screeps::RESOURCE_ENERGY) == Screeps::ERR_NOT_IN_RANGE)
        {
            if (Screeps::StructureContainer(source.value()).store().getUsedCapacity(Screeps::RESOURCE_ENERGY).value_or(-1) > 40)
            {
                this->moveTo(source, moveToOpt());
            }
        }
    }
}

std::vector<std::string> Upgrader::bodyParts()
{
    return std::vector<std::string>{
        Screeps::WORK,
        Screeps::CARRY,
        Screeps::MOVE};
}

#endif // EXAMPLE_UPGRADER_H
