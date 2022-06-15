#include <Screeps/Context.hpp>
#include <Screeps/Creep.hpp>
#include <Screeps/Room.hpp>
#include <Screeps/RoomObject.hpp>
#include <Screeps/RoomPosition.hpp>
#include <Screeps/Source.hpp>
#include <Screeps/StructureSpawn.hpp>
#include <Screeps/StructureStorage.hpp>
#include <Screeps/Constants.hpp>
#include <Screeps/ConstructionSite.hpp>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <optional>
#include <Screeps/StructureController.hpp>
#include "creep/Harvester.hpp"
#include "creep/Upgrader.h"
#include "creep/Builder.h"
#include <iostream>

#define HARVESTER_NUM 16
#define UPGRADER_NUM 5
#define BUILDER_NUM 5

EMSCRIPTEN_KEEPALIVE
extern "C" void loop() {
    Screeps::Context::update();

    std::map<std::string, Screeps::Creep> creeps = Screeps::Game.creeps();

    Screeps::StructureSpawn homeSpawn = Screeps::Game.spawns().find("home")->second;
    auto sources = homeSpawn.room().find(Screeps::FIND_SOURCES);


    auto constructionSites = homeSpawn.room().find(Screeps::FIND_CONSTRUCTION_SITES);
    std::unique_ptr<Screeps::ConstructionSite> constructionSite;

    if (!constructionSites.empty()) {
        std::unique_ptr<Screeps::ConstructionSite> s(
                new Screeps::ConstructionSite(constructionSites.begin()->get()->value()));
        constructionSite = std::move(s);
    }

    std::unique_ptr<Screeps::Source> source;

    for (const auto &item: sources) {
        std::unique_ptr<Screeps::Source> s(new Screeps::Source(item->value()));
        if (s->energyCapacity() > 0) {
            source = std::move(s);
            break;
        }
    }
    auto structures = homeSpawn.room().find(Screeps::FIND_STRUCTURES, [&](const JS::Value &value) {
        std::string structureType = value["structureType"].as<std::string>();
        return structureType == Screeps::STRUCTURE_EXTENSION ||
               structureType == Screeps::STRUCTURE_SPAWN ||
               structureType == Screeps::STRUCTURE_CONTAINER;
    });
    std::unique_ptr<Screeps::Structure> container;
    if (!structures.empty()) {
        std::unique_ptr<Screeps::Structure> s(new Screeps::Structure(structures.begin()->get()->value()));
        container = std::move(s);
    }
    for (int i = 0; i < BUILDER_NUM; i++) {
        homeSpawn.spawnCreep(Builder::bodyParts(), Builder::namePre() + std::to_string(i));
    }
    for (int i = 0; i < UPGRADER_NUM; i++) {
        homeSpawn.spawnCreep(Upgrader::bodyParts(), Upgrader::namePre() + std::to_string(i));
    }
    for (int i = 0; i < HARVESTER_NUM; i++) {
        homeSpawn.spawnCreep(Harvester::bodyParts(), Harvester::namePre() + std::to_string(i));
    }

    int harvesterIndex = 0;
    for (const auto &creep: creeps) {
        std::string creepName = creep.second.name();
        if (creepName.find(Harvester::namePre()) != -1) {
            ++harvesterIndex;
            Harvester harvester(creep.second.value());
            Screeps::Source s(sources[harvesterIndex % 2]->value());
            harvester.work(s, *container);
        }

    }
    if (harvesterIndex > HARVESTER_NUM / 2)
        for (const auto &creep: creeps) {
            std::string creepName = creep.second.name();
            if (creepName.find(Upgrader::namePre()) != -1) {
                Upgrader upgrader(creep.second.value());
                Screeps::StructureController homeController = homeSpawn.room().controller().value();
                upgrader.work(homeSpawn, homeController);
            }
            if (creepName.find(Builder::namePre()) != -1) {
                Builder builder(creep.second.value());
                builder.work(homeSpawn, *constructionSite);
            }
        }


}

EMSCRIPTEN_BINDINGS(loop) {
    emscripten::function("loop", &loop);
}
