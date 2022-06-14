#include <Screeps/Context.hpp>
#include <Screeps/Creep.hpp>
#include <Screeps/Room.hpp>
#include <Screeps/RoomObject.hpp>
#include <Screeps/RoomPosition.hpp>
#include <Screeps/Source.hpp>
#include <Screeps/StructureSpawn.hpp>
#include <Screeps/StructureStorage.hpp>
#include <Screeps/Constants.hpp>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <optional>
#include <Screeps/StructureController.hpp>
#include <Screeps/ConstructionSite.hpp>
#include "creep/Harvester.hpp"
#include "creep/Upgrader.h"


EMSCRIPTEN_KEEPALIVE
extern "C" void loop() {
    Screeps::Context::update();

    std::map<std::string, Screeps::Creep> creeps = Screeps::Game.creeps();

    Screeps::StructureSpawn homeSpawn = Screeps::Game.spawns().find("home")->second;
    auto sources = homeSpawn.room().find(Screeps::FIND_SOURCES);
    std::unique_ptr<Screeps::Source> source;

    for (const auto &item: sources) {
        std::unique_ptr<Screeps::Source> s(new Screeps::Source(item->value()));
        if (s->energyCapacity() > 0) {
            source = std::move(s);
            break;
        }
    }

    for (int i = 0; i < 10; i++) {
        homeSpawn.spawnCreep(Harvester::bodyParts(), Harvester::namePre() + std::to_string(i));
    }
    for (int i = 0; i < 5; i++) {
        homeSpawn.spawnCreep(Upgrader::bodyParts(), Upgrader::namePre() + std::to_string(i));
    }
    for (const auto &creep: creeps) {
        if (creep.second.name().find(Harvester::namePre()) != -1) {
            Harvester harvester(creep.second.value());
            harvester.work(*source, homeSpawn);
        }
        if (creep.second.name().find(Upgrader::namePre()) != -1) {
            Upgrader upgrader(creep.second.value());
            Screeps::StructureController homeController = homeSpawn.room().controller().value();
            upgrader.work(homeSpawn, homeController);
        }
    }


}
//
//
//void upgrade(Screeps::Creep &upgrade, Screeps::Source &source, Screeps::StructureController &target) {
//    JSON upgradeMemory = upgrade.memory();
//    std::string action = "isUpgradeing";
//    bool isUpgrading;
//    if (!upgradeMemory.contains(action)) {
//        upgradeMemory[action] = true;
//    }
//    upgradeMemory[action].get_to<bool>(isUpgrading);
//    if (isUpgrading && upgrade.store().getUsedCapacity() == 0) {
//        isUpgrading = false;
//        upgrade.say(SAY_HARVEST);
//    }
//    if (!isUpgrading && upgrade.store().getFreeCapacity() == 0) {
//        isUpgrading = true;
//        upgrade.say(SAY_BUILD);
//    }
//    upgradeMemory[action] = isUpgrading;
//    upgrade.setMemory(upgradeMemory);
//    if (isUpgrading) {
//        if (upgrade.upgradeController(target) == Screeps::ERR_NOT_IN_RANGE) {
//            upgrade.moveTo(target, creepMoveToOpt);
//        }
//    } else {
//        if (upgrade.harvest(source) == Screeps::ERR_NOT_IN_RANGE) {
//            upgrade.moveTo(source, creepMoveToOpt);
//        }
//    }
//}
//
//Screeps::StructureExtension getEmptyExtension(Screeps::Room &room) {
//
//}
//
//void build(Screeps::Creep &builder, Screeps::Source &source, Screeps::ConstructionSite &target) {
//    JSON builderMemory = builder.memory();
//    bool isBuilding;
//    if (!builderMemory.contains("isBuilding")) {
//        builderMemory["isBuilding"] = true;
//    }
//    builderMemory["isBuilding"].get_to(isBuilding);
//    if (isBuilding && builder.store().getUsedCapacity() == 0) {
//        isBuilding = false;
//        builder.say(SAY_HARVEST);
//    }
//    if (!isBuilding && builder.store().getFreeCapacity() == 0) {
//        isBuilding = true;
//        builder.say(SAY_BUILD);
//    }
//
//    builderMemory["isBuilding"] = isBuilding;
//    builder.setMemory(builderMemory);
//    if (isBuilding) {
//        if (builder.build(target) == Screeps::ERR_NOT_IN_RANGE) {
//            builder.moveTo(target, creepMoveToOpt);
//        }
//    } else {
//        if (builder.harvest(source) == Screeps::ERR_NOT_IN_RANGE) {
//            builder.moveTo(source, creepMoveToOpt);
//        }
//    }
//}

EMSCRIPTEN_BINDINGS(loop) {
    emscripten::function("loop", &loop);
}
