#ifndef OBJECT_SPAWN_HPP
#define OBJECT_SPAWN_HPP

#include <Screeps/StructureSpawn.hpp>
#include <Screeps/JSON.hpp>
#include <string>
#include <utility>
#include "BaseInterface.hpp"

class Spawn : public Screeps::StructureSpawn, IMemory {
public:
    using IMemory::setMemory;
    using IMemory::getMemory;
    Spawn(JS::Value creep);
};

Spawn::Spawn(JS::Value creep) : Screeps::StructureSpawn(std::move(creep)) {};
#endif