//
// Created by 19278 on 2022/7/15.
//

#ifndef EXAMPLE_CREEP_HPP
#define EXAMPLE_CREEP_HPP

#include <Screeps/Creep.hpp>

class Creep : public Screeps::Creep {
public:
    Creep(JS::Value creep);

    std::string getMemory(const std::string &key);

    std::string getMemoryOr(const std::string &key, const std::string &defaultValue);

    bool setMemory(std::string key, const std::string &value);
};

Creep::Creep(JS::Value creep) : Screeps::Creep(creep) {};

std::string Creep::getMemory(const std::string &key) {
    JSON memory = this->memory();
    if (!memory.contains(key)) {
        std::cout << "error" << this->name() << "memory not contains key :" << key << std::endl;
        return "";
    }
    std::string value;
    memory[key].get_to(value);
    return value;
}

bool Creep::setMemory(std::string key, const std::string &value) {
    JSON memory = this->memory();
    memory[key] = value;
    Screeps::Creep::setMemory(memory);
    return true;
}

std::string Creep::getMemoryOr(const std::string &key, const std::string &defaultValue) {
    JSON memory = this->memory();
    if (!memory.contains(key)) {
        return defaultValue;
    }
    std::string value;
    memory[key].get_to(value);
    return value;
}

#endif //EXAMPLE_CREEP_HPP
