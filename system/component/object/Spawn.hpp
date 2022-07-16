#ifndef OBJECT_SPAWN_HPP
#define OBJECT_SPAWN_HPP

#include <Screeps/StructureSpawn.hpp>
#include <Screeps/JSON.hpp>
#include <string>
#include <utility>

class Spawn : public Screeps::StructureSpawn {
public:
    Spawn(JS::Value creep);

    std::string getMemory(const std::string &key);

    bool setMemory(const std::string &key, const std::string &value);

    std::string getMemoryOr(const std::string &key, const std::string &defaultValue);
};

Spawn::Spawn(JS::Value creep) : Screeps::StructureSpawn(std::move(creep)) {};

bool Spawn::setMemory(const std::string &key, const std::string &value) {
    JSON memory = this->memory();
    memory[key] = value;
    Screeps::StructureSpawn::setMemory(memory);
    return true;
}


std::string Spawn::getMemoryOr(const std::string &key, const std::string &defaultValue) {
    JSON memory = this->memory();
    if (!memory.contains(key)) {
        return defaultValue;
    }
    std::string value;
    memory[key].get_to(value);
    return value;
}

std::string Spawn::getMemory(const std::string &key) {
    JSON memory = this->memory();
    if (!memory.contains(key)) {
        std::cout << "error" << this->name() << "memory not contains key :" << key << std::endl;
        return "";
    }
    std::string value;
    memory[key].get_to(value);
    return value;
}


#endif