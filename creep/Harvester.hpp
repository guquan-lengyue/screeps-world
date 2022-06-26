#ifndef CREEP_ROLE_HARVESTER
#define CREEP_ROLE_HARVESTER

#include <Screeps/Creep.hpp>
#include <Screeps/Constants.hpp>
#include <Screeps/Store.hpp>
#include <Screeps/Source.hpp>
#include <Screeps/Structure.hpp>
#include <Screeps/Resource.hpp>
#include <iostream>
#include "opts.hpp"

class Harvester : public Screeps::Creep
{
private:
    /* data */
public:
    static std::vector<std::string> bodyParts();

    explicit Harvester(JS::Value creep);

    ~Harvester() = default;

    void work(Screeps::Source &source, Screeps::Structure &target);
    void work(Screeps::Resource &source, Screeps::Structure &target);

    static std::string namePre() { return "Harvester_"; };
};

Harvester::Harvester(JS::Value creep) : Screeps::Creep(creep)
{
}

std::vector<std::string> Harvester::bodyParts()
{
    return std::vector<std::string>{
        Screeps::WORK,
        Screeps::WORK,
        Screeps::CARRY,
        Screeps::CARRY,
        Screeps::MOVE,
        Screeps::MOVE};
}

void Harvester::work(Screeps::Source &source, Screeps::Structure &target)
{
    if (this->store().getFreeCapacity() > 0)
    {
        if (this->harvest(source) == Screeps::ERR_NOT_IN_RANGE)
        {
            this->moveTo(source, moveToOpt());
        }
    }
    else
    {
        if (this->transfer(target, Screeps::RESOURCE_ENERGY) == Screeps::ERR_NOT_IN_RANGE)
        {
            this->moveTo(target, moveToOpt());
        }
    }
}

void Harvester::work(Screeps::Resource &source, Screeps::Structure &target)
{
    if (this->store().getFreeCapacity() > 0)
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
            this->moveTo(target, moveToOpt());
        }
    }
}

#endif