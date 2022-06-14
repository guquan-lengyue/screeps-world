//
// Created by 19278 on 2022/6/14.
//

#include <Screeps/Creep.hpp>
#include <Screeps/ConstructionSite.hpp>
#include <Screeps/Structure.hpp>

#ifndef EXAMPLE_BUILDER_H
#define EXAMPLE_BUILDER_H

#define BUILDER_ACTION "isBuilding"

class Builder : public Screeps::Creep {
public:
    explicit Builder(JS::Value creep);

    static std::string namePre() { return "Builder_"; };

    static std::vector<std::string> bodyParts();

    void work(Screeps::RoomObject &source, Screeps::ConstructionSite &target);
};

Builder::Builder(JS::Value creep) : Screeps::Creep(std::move(creep)) {

}

std::vector<std::string> Builder::bodyParts() {
    return {
            Screeps::WORK,
            Screeps::CARRY,
            Screeps::MOVE
    };
}

void Builder::work(Screeps::RoomObject &source, Screeps::ConstructionSite &target) {
    JSON builderMemory = this->memory();
    bool isBuilding;
    if (!builderMemory.contains(BUILDER_ACTION)) {
        builderMemory[BUILDER_ACTION] = true;
    }
    builderMemory[BUILDER_ACTION].get_to(isBuilding);
    if (isBuilding && this->store().getUsedCapacity() == 0) {
        isBuilding = false;
        this->say(SAY_HARVEST);
    }
    if (!isBuilding && this->store().getFreeCapacity() == 0) {
        isBuilding = true;
        this->say(SAY_BUILD);
    }

    builderMemory[BUILDER_ACTION] = isBuilding;
    this->setMemory(builderMemory);
    if (isBuilding) {
        if (this->build(target) == Screeps::ERR_NOT_IN_RANGE) {
            this->moveTo(target);
        }
    } else {
        if (this->withdraw(source, Screeps::RESOURCE_ENERGY) == Screeps::ERR_NOT_IN_RANGE) {
            this->moveTo(source);
        }
    }
}


#endif //EXAMPLE_BUILDER_H
