#ifndef CREEP_ROLE_HARVESTER
#define CREEP_ROLE_HARVESTER
#include "MyScreeps.hpp"
#include <Screeps/Creep.hpp>
#include <Screeps/Constants.hpp>
#include <Screeps/Store.hpp>
#include <Screeps/Source.hpp>
#include <Screeps/Structure.hpp>
#include <Screeps/Resource.hpp>
#include <Screeps/StructureContainer.hpp>
#include <iostream>
#include "opts.hpp"

class Harvester : public MyScreeps
{
private:
    /* data */
public:
    static std::vector<std::string> bodyParts(int level);

    explicit Harvester(JS::Value creep);

    ~Harvester() = default;

    void work(Screeps::Source &source, Screeps::Structure &target);
    void work(Screeps::Resource &source, Screeps::Structure &target);

    static std::string namePre() { return "Harvester_"; };
};

Harvester::Harvester(JS::Value creep) : MyScreeps::MyScreeps(std::move(creep))
{
}

std::vector<std::string> Harvester::bodyParts(int level)
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

void Harvester::work(Screeps::Source &source, Screeps::Structure &target)
{
    if (this->store().getFreeCapacity().value_or(-1) > 0)
    {
        if (this->ticksToLive() < 10)
        {
            this->suicide();
            return;
        }
        if (this->harvest(source) == Screeps::ERR_NOT_IN_RANGE)
        {
            this->moveTo(source, moveToOpt());
        }
    }
    else
    {
        if (this->transfer(target, Screeps::RESOURCE_ENERGY) == Screeps::ERR_NOT_IN_RANGE)
        {
            if (Screeps::StructureContainer(target.value()).store().getFreeCapacity(Screeps::RESOURCE_ENERGY).value_or(-1) > 0)
            {
                this->moveTo(target, moveToOpt());
            }
        }
    }
}

void Harvester::work(Screeps::Resource &source, Screeps::Structure &target)
{
    if (this->store().getFreeCapacity().value_or(-1) > 0)
    {
        if (this->pickup(source) == Screeps::ERR_NOT_IN_RANGE)
        {
            this->moveTo(source, moveToOpt());
        }
    }
    else
    {
        if (this->transfer(target, Screeps::RESOURCE_ENERGY) == Screeps::ERR_NOT_IN_RANGE)
        {
            if (Screeps::StructureContainer(target.value()).store().getFreeCapacity(Screeps::RESOURCE_ENERGY).value_or(-1) > 0)
            {
                this->moveTo(target, moveToOpt());
            }
        }
    }
}

#endif