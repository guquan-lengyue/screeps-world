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
#include <Screeps/StructureController.hpp>
#include <Screeps/JS.hpp>
#include <Screeps/JSON.hpp>
#include <string>

#define SAY_HARVEST "🔄"
#define SAY_BUILD "🚧"

JSON roleOpt(std::string str) {
    auto memory = JS::Value::object();
    auto role = JS::Value::object();
    role.set("role", str.c_str());
    memory.set("memory", role);
    return JS::toJSON(memory);
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

void upgrade(Screeps::Creep &creep, Screeps::RoomObject &source, Screeps::StructureController &target) {
    JSON memory = creep.memory();
    if (!memory.contains("working")) {
        memory["working"] = false;
    }
    bool isUpgrading;
    memory["working"].get_to(isUpgrading);
    if (isUpgrading && creep.store().getUsedCapacity(Screeps::RESOURCE_ENERGY).value_or(-1) == 0) {
        isUpgrading = false;
        creep.say(SAY_HARVEST);
    }
    if (!isUpgrading && creep.store().getFreeCapacity(Screeps::RESOURCE_ENERGY).value_or(-1) == 0) {
        isUpgrading = true;
        creep.say(SAY_BUILD);
    }

    memory["working"] = isUpgrading;
    creep.setMemory(memory);

    if (isUpgrading) {
        if (creep.upgradeController(target) == Screeps::ERR_NOT_IN_RANGE) {
            creep.moveTo(target);
        }
    } else {
        if (creep.ticksToLive() < 10) {
            creep.suicide();
            return;
        }
        if (creep.withdraw(source, Screeps::RESOURCE_ENERGY) == Screeps::ERR_NOT_IN_RANGE) {
            if (Screeps::StructureContainer(source.value()).store().getUsedCapacity(Screeps::RESOURCE_ENERGY).value_or(
                    -1) > 40) {
                creep.moveTo(source);
            }
        }
    }
}

#endif //EXAMPLE_WORKER_HPP
