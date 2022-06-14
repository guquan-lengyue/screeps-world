//
// Created by 19278 on 2022/6/14.
//

#ifndef EXAMPLE_UPGRADER_H
#define EXAMPLE_UPGRADER_H

#include <Screeps/Creep.hpp>
#include <Screeps/Structure.hpp>
#include <utility>

#define ACTION  "action"
#define SAY_HARVEST "🔄"
#define SAY_BUILD "🚧"

class Upgrader : public Screeps::Creep {
public:
    Upgrader(JS::Value creep);

    int work(Screeps::RoomObject &source, Screeps::StructureController &target);
    static std::string namePre() { return "Upgrader_"; };
    static std::vector<std::string> bodyParts();
};

Upgrader::Upgrader(JS::Value creep) : Screeps::Creep(std::move(creep)) {

}

int Upgrader::work(Screeps::RoomObject &source, Screeps::StructureController &target) {
    JSON memory = this->memory();
    if (!memory.contains(ACTION)) {
        memory[ACTION] = true;
    }
    bool isActioning = memory[ACTION].get<bool>();
    if (isActioning && this->store().getUsedCapacity() == 0) {
        isActioning = false;
        this->say(SAY_HARVEST);
    }
    if (!isActioning && this->store().getFreeCapacity() == 0) {
        isActioning = true;
        this->say(SAY_BUILD);
    }
    memory[ACTION] = isActioning;
    this->setMemory(memory);
    if (isActioning) {
        if (this->upgradeController(target) == Screeps::ERR_NOT_IN_RANGE) {
            this->moveTo(target);
        } else {
            if (this->withdraw(source, Screeps::RESOURCE_ENERGY)) {
                this->moveTo(source);
            }
        }
    }
}

std::vector<std::string> Upgrader::bodyParts() {
    return std::vector<std::string>{
            Screeps::WORK,
            Screeps::CARRY,
            Screeps::MOVE
    };
}


#endif //EXAMPLE_UPGRADER_H
