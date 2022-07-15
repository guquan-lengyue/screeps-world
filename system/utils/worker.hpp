//
// Created by 19278 on 2022/7/15.
//

#ifndef EXAMPLE_WORKER_HPP
#define EXAMPLE_WORKER_HPP

#include "include/Screeps/Source.hpp"
#include "include/Screeps/Structure.hpp"
#include "include/Screeps/Creep.hpp"
#include "include/Screeps/Store.hpp"
#include "include/Screeps/StructureContainer.hpp"
#include <Screeps/JS.hpp>
#include <Screeps/JSON.hpp>

JSON roleHarvester() {
    auto role = JS::Value::object();
    role.set("role", "HARVESTER");
    return JS::toJSON(role);
}

void harvester(Screeps::Creep &creep, Screeps::Source &source, Screeps::Structure &target) {
    if (creep.store().getFreeCapacity().value_or(-1) > 0) {
        if (creep.harvest(source) == Screeps::ERR_NOT_IN_RANGE) {
            creep.moveTo(source);
        }
    } else {
        if (creep.transfer(target, Screeps::RESOURCE_ENERGY) == Screeps::ERR_NOT_IN_RANGE) {
            if (((Screeps::StructureContainer) target)
                        .store()
                        .getFreeCapacity(Screeps::RESOURCE_ENERGY)
                        .value_or(-1) > 0) {
                creep.moveTo(target);
            }
        }
    }
}


#endif //EXAMPLE_WORKER_HPP
