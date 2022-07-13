#ifndef EXAMPLE_MYSCREEPS_H
#define EXAMPLE_MYSCREEPS_H
#include <Screeps/Creep.hpp>
#include <Screeps/StructureSpawn.hpp>

class MyScreeps : public Screeps::Creep
{
private:
    /* data */
public:
    MyScreeps(JS::Value creep);
    void renew(Screeps::StructureSpawn &spawn);
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
        this->moveTo(spawn);
    }
}
#endif // EXAMPLE_BUILDER_H