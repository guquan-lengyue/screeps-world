#ifndef EXAMPLE_REPAIRER_H
#define EXAMPLE_REPAIRER_H
#include <Screeps/Creep.hpp>
#include <Screeps/Structure.hpp>
#include "opts.hpp"
#define SAY_HARVEST "🔄"
#define SAY_BUILD "🚧"
#define REPAIRER_ACTION "working"
class Repairer : public Screeps::Creep
{
public:
    Repairer(JS::Value creep);

    void work(Screeps::RoomObject &source, Screeps::StructureController &target);

    static std::string namePre() { return "Repairer_"; };
    void work(Screeps::RoomObject &source, Screeps::Structure &target);
    static std::vector<std::string> bodyParts();
};
Repairer::Repairer(JS::Value creep) : Screeps::Creep(std::move(creep))
{
}
void Repairer::work(Screeps::RoomObject &source, Screeps::Structure &target)
{
    JSON memory = this->memory();
    if (!memory.contains(REPAIRER_ACTION))
    {
        memory[REPAIRER_ACTION] = false;
    }
    bool isUpgrading;
    memory[REPAIRER_ACTION].get_to(isUpgrading);
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

    memory[REPAIRER_ACTION] = isUpgrading;
    this->setMemory(memory);

    if (isUpgrading)
    {
        if (this->repair(target) == Screeps::ERR_NOT_IN_RANGE)
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
std::vector<std::string> Repairer::bodyParts()
{
    return std::vector<std::string>{
        Screeps::WORK,
        Screeps::CARRY,
        Screeps::MOVE};
}
#endif