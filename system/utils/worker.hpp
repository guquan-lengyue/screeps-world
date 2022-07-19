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
#include <Screeps/ConstructionSite.hpp>
#include <Screeps/JS.hpp>
#include <Screeps/JSON.hpp>
#include <string>

#define SAY_HARVEST "🔄"
#define SAY_BUILD "🚧"

JSON roleOpt(const std::string &str) {
    auto memory = JS::Value::object();
    auto role = JS::Value::object();
    role.set("role", str.c_str());
    role.set("beforeRole", str.c_str());
    memory.set("memory", role);
    return JS::toJSON(memory);
}

namespace util {

    void harvester(Screeps::Creep &creep, Screeps::Source &source, Screeps::StructureContainer &target) {
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

    void renew(Screeps::Creep &creep, Screeps::StructureSpawn &spawn) {
        if (spawn.renewCreep(creep) == Screeps::ERR_NOT_IN_RANGE) {
            if (((Screeps::StructureContainer) spawn)
                        .store()
                        .getFreeCapacity(Screeps::RESOURCE_ENERGY)
                        .value_or(-1) > 0) {
                creep.moveTo(spawn);
            }
        }
    }

    void recycle(Screeps::Creep &creep, Screeps::StructureSpawn &spawn) {
        if (creep.store().getUsedCapacity(Screeps::RESOURCE_ENERGY).value_or(-1) > 0) {
            if (creep.transfer(spawn, Screeps::RESOURCE_ENERGY) == Screeps::ERR_NOT_IN_RANGE) {
                if (((Screeps::StructureContainer) spawn)
                            .store()
                            .getFreeCapacity(Screeps::RESOURCE_ENERGY)
                            .value_or(-1) > 0) {
                    creep.moveTo(spawn);
                }
            }
        } else if (spawn.recycleCreep(creep) == Screeps::ERR_NOT_IN_RANGE) {
            if (((Screeps::StructureContainer) spawn)
                        .store()
                        .getFreeCapacity(Screeps::RESOURCE_ENERGY)
                        .value_or(-1) > 0) {
                creep.moveTo(spawn);
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
                if (Screeps::StructureContainer(source.value()).store().getUsedCapacity(
                        Screeps::RESOURCE_ENERGY).value_or(-1) > 40) {
                    creep.moveTo(source);
                }
            }
        }
    }

    void build(Screeps::Creep &creep, Screeps::RoomObject &source, Screeps::ConstructionSite &target) {
        JSON memory = creep.memory();
        if (!memory.contains("working")) {
            memory["working"] = true;
        }
        bool isBuilding;
        memory["working"].get_to(isBuilding);
        if (isBuilding && creep.store().getUsedCapacity(Screeps::RESOURCE_ENERGY).value_or(-1) == 0) {
            isBuilding = false;
            creep.say(SAY_HARVEST);
        }
        if (!isBuilding && creep.store().getFreeCapacity(Screeps::RESOURCE_ENERGY).value_or(-1) == 0) {
            isBuilding = true;
            creep.say(SAY_BUILD);
        }

        memory["working"] = isBuilding;
        creep.setMemory(memory);
        if (isBuilding) {
            if (creep.build(target) == Screeps::ERR_NOT_IN_RANGE) {
                creep.moveTo(target);
            }
        } else {
            if (creep.ticksToLive() < 10) {
                creep.suicide();
                return;
            }
            if (creep.withdraw(source, Screeps::RESOURCE_ENERGY) == Screeps::ERR_NOT_IN_RANGE) {
                if (Screeps::StructureContainer(source.value())
                            .store()
                            .getUsedCapacity(Screeps::RESOURCE_ENERGY)
                            .value_or(-1) > 40) {
                    creep.moveTo(source);
                }
            }
        }
    }

    void repairer(Screeps::Creep &creep, Screeps::RoomObject &source, Screeps::Structure &target) {
        JSON memory = creep.memory();
        if (!memory.contains("working")) {
            memory["working"] = false;
        }
        bool working;
        memory["working"].get_to(working);
        if (working && creep.store().getUsedCapacity(Screeps::RESOURCE_ENERGY).value_or(-1) == 0) {
            working = false;
            creep.say(SAY_HARVEST);
        }
        if (!working && creep.store().getFreeCapacity(Screeps::RESOURCE_ENERGY).value_or(-1) == 0) {
            working = true;
            creep.say(SAY_BUILD);
        }

        memory["working"] = working;
        creep.setMemory(memory);

        if (working) {
            if (creep.repair(target) == Screeps::ERR_NOT_IN_RANGE) {
                creep.moveTo(target);
            }
        } else {
            if (creep.ticksToLive() < 10) {
                creep.suicide();
                return;
            }
            if (creep.withdraw(source, Screeps::RESOURCE_ENERGY) == Screeps::ERR_NOT_IN_RANGE) {
                if (Screeps::StructureContainer(source.value())
                            .store()
                            .getUsedCapacity(Screeps::RESOURCE_ENERGY)
                            .value_or(-1) > 40) {
                    creep.moveTo(source);
                }
            }
        }
    }

}
#endif // EXAMPLE_WORKER_HPP
