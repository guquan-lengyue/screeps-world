#include "Harvester.h"
#include <Screeps/Constants.hpp>
#include <Screeps/Store.hpp>
#include <Screeps/Source.hpp>
#include <Screeps/Structure.hpp>

std::vector<std::string> Harvester::bodyParts() {
    return std::vector<std::string>{
            Screeps::WORK,
            Screeps::CARRY,
            Screeps::MOVE
    };
}

int Harvester::work(Screeps::Source &source, Screeps::Structure &target) {
    if (this->store().getFreeCapacity() > 0) {
        this->moveTo(source);
    } else {
        if (this->transfer(target, Screeps::RESOURCE_ENERGY) == Screeps::ERR_NOT_IN_RANGE) {
            this->moveTo(target);
        }
    }
}

Harvester::Harvester(JS::Value creep) : Screeps::Creep(creep) {

}
