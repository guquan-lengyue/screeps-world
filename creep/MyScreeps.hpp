#ifndef EXAMPLE_MYSCREEPS_H
#define EXAMPLE_MYSCREEPS_H
#include <Screeps/Creep.hpp>
#include <Screeps/StructureSpawn.hpp>
#include <iostream>

class MyScreeps : public Screeps::Creep
{
private:
    /* data */
public:
    MyScreeps(JS::Value creep);
    void renew(Screeps::StructureSpawn &spawn);
    bool recycleCreep(Screeps::StructureSpawn &spawn);
    ~MyScreeps();
};

MyScreeps::MyScreeps(JS::Value creep) : Screeps::Creep(creep)
{
}

MyScreeps::~MyScreeps()
{
}

void MyScreeps::renew(Screeps::StructureSpawn &spawn)
{
    if (spawn.renewCreep(*this) < 0)
    {
        if (Screeps::StructureContainer(spawn.value()).store().getUsedCapacity(Screeps::RESOURCE_ENERGY).value_or(-1) > 40)
        {
            this->moveTo(spawn, moveToOpt());
        }
    }
}
bool MyScreeps::recycleCreep(Screeps::StructureSpawn &spawn)
{
    if (spawn.recycleCreep(*this) < 0)
    {
        std::cout << this->name() << " go to recycle" << std::endl;
        this->moveTo(spawn);
        return false;
    }
    return true;
}
#endif // EXAMPLE_BUILDER_H