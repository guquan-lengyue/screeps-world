//
// Created by 19278 on 2022/7/15.
//

#ifndef EXAMPLE_BASEINTERFACE_HPP
#define EXAMPLE_BASEINTERFACE_HPP

#include <string>
#include <Screeps/JSON.hpp>
#include <Screeps/Object.hpp>
#include <Screeps/Structure.hpp>

class IMemory : public Screeps::Object {
public:
    std::string getMemory(std::string key);

    bool setMemory(std::string key, std::string value);

    JSON memory() const;

    void setMemory(const JSON &);
};

std::string IMemory::getMemory(std::string key) {
    JSON memory = this->memory();
    std::string value;
    memory[key].get_to(value);
    return value;
}

bool IMemory::setMemory(std::string key, std::string value) {
    JSON memory = this->memory();
    memory[key] = value;
    this->setMemory(memory);
    return true;
}

void IMemory::setMemory(const JSON &memory) {
    value().set("memory", JS::fromJSON(memory));
}

JSON IMemory::memory() const {
    return JS::toJSON(value()["memory"]);
}

#endif //EXAMPLE_BASEINTERFACE_HPP
