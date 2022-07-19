//
// Created by 19278 on 2022/7/17.
//

#ifndef EXAMPLE_TOWERSYSTEM_HPP
#define EXAMPLE_TOWERSYSTEM_HPP

#include "component/component.h"
#include "Screeps/StructureContainer.hpp"
#include <Screeps/Room.hpp>
#include <Screeps/RoomObject.hpp>
#include <Screeps/Game.hpp>
#include <Screeps/Constants.hpp>
#include <Screeps/Creep.hpp>
#include "component/object/Creep.hpp"
#include <Screeps/Store.hpp>
#include <Screeps/RoomPosition.hpp>
#include <Screeps/StructureTower.hpp>
#include <string>
#include "utils/worker.hpp"

namespace sys {
    void check_tower() {

    }

    void tower() {
        for (auto &spawn: Screeps::Game.spawns()) {
            auto structure = spawn.second.room().find(Screeps::FIND_MY_STRUCTURES);
            auto creeps = spawn.second.room().find(Screeps::FIND_CREEPS);
            for (const auto &item: structure) {
                auto tower = (Screeps::StructureTower) *item;
                if (tower.structureType() != Screeps::STRUCTURE_TOWER) {
                    continue;
                }
                for (const auto &creep: creeps) {
                    auto c = (Screeps::Creep) *creep;
                    if (c.my()) {
                        continue;
                    }
                    tower.attack(c);
                }
            }
        }

    }
}

#endif //EXAMPLE_TOWERSYSTEM_HPP
