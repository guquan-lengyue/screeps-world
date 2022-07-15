#ifndef OBJECT_SPAWN_HPP
#define OBJECT_SPAWN_HPP
#include <Screeps/StructureSpawn.hpp>
#include <JSON.hpp>
#include <string>
class Spawn : public Screeps::StructureSpawn
{
public:
    Spawn(JS::Value creep);
    std::string getMemory(std::string key);
    bool setMemory(std::string key, std::string value);
};

Spawn::Spawn(JS::Value creep) : Screeps::StructureSpawn(creep){};

std::string Spawn::getMemory(std::string key)
{
    JSON memory = this->memory();
    std::string value;
    memory[key].get_to(value);
    return value;
}

#endif