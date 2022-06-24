//
// Created by 19278 on 2022/6/14.
//

#ifndef EXAMPLE_UPGRADER_H
#define EXAMPLE_UPGRADER_H

#include <Screeps/Creep.hpp>
#include <Screeps/Structure.hpp>

#define UPGRADER_ACTION  "upgrading"
#define SAY_HARVEST "🔄"
#define SAY_BUILD "🚧"

class Upgrader : public Screeps::Creep {
public:
    Upgrader(JS::Value creep);

    void work(Screeps::RoomObject &source, Screeps::StructureController &target);

    static std::string namePre() { return "Upgrader_"; };

    static std::vector<std::string> bodyParts();
};

Upgrader::Upgrader(JS::Value creep) : Screeps::Creep(std::move(creep)) {

}

void Upgrader::work(Screeps::RoomObject &source, Screeps::StructureController &target) {
    JSON memory = this->memory();
    if (!memory.contains(UPGRADER_ACTION)) {
        memory[UPGRADER_ACTION] = false;
    }
    bool isUpgrading;
    memory[UPGRADER_ACTION].get_to(isUpgrading);
    if (isUpgrading && this->store().getUsedCapacity() == 0) {
        isUpgrading = false;
        this->say(SAY_HARVEST);
    }
    if (!isUpgrading && this->store().getFreeCapacity() == 0) {
        isUpgrading = true;
        this->say(SAY_BUILD);
    }

    memory[UPGRADER_ACTION] = isUpgrading;
    this->setMemory(memory);
    
    if (isUpgrading) {
        if (this->upgradeController(target) == Screeps::ERR_NOT_IN_RANGE) {
            this->moveTo(target);
        }
    } else {
        if (this->withdraw(source, Screeps::RESOURCE_ENERGY) == Screeps::ERR_NOT_IN_RANGE) {
            this->moveTo(source);
        }
    }

}

std::vector<std::string> Upgrader::bodyParts() {
    return std::vector<std::string>{
            Screeps::WORK,
            Screeps::CARRY,
            Screeps::CARRY,
            Screeps::CARRY,
            Screeps::CARRY,
            Screeps::MOVE,
            Screeps::MOVE,
            Screeps::MOVE
    };
}


#endif //EXAMPLE_UPGRADER_H
