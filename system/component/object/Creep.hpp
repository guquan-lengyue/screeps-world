//
// Created by 19278 on 2022/7/15.
//

#ifndef EXAMPLE_CREEP_HPP
#define EXAMPLE_CREEP_HPP

#include <Screeps/Creep.hpp>

class Creep : public Screeps::Creep {
public:
    Creep(JS::Value creep);

    std::string getMemory(std::string key);

    bool setMemory(std::string key, std::string value);
};

Creep::Creep(JS::Value creep) : Screeps::Creep(creep) {};

std::string Creep::getMemory(std::string key) {
    JSON memory = this->memory();
    std::string value;
    memory[key].get_to(value);
    return value;
}

bool Creep::setMemory(std::string key, std::string value) {
    JSON memory = this->memory();
    memory[key] = value;
    Screeps::Creep::setMemory(memory);
    return true;
}

#endif //EXAMPLE_CREEP_HPP
