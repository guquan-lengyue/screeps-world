//
// Created by 19278 on 2022/7/16.
//

#ifndef EXAMPLE_STRUCTURESSYSTEM_HPP
#define EXAMPLE_STRUCTURESSYSTEM_HPP

#include <Screeps/Constants.hpp>
#include <Screeps/RoomObject.hpp>
#include <Screeps/Structure.hpp>
#include <memory>
#include "component/component.h"

namespace sys {
    void check_structures() {
        comp::emptyContainer.clear();
        for (auto &spawn: Screeps::Game.spawns()) {
            auto s = (Spawn) spawn.second;
            auto room = s.room();
            auto structures = room.find(Screeps::FIND_MY_STRUCTURES);
            for (auto &structure: structures) {
                auto stru = (Screeps::StructureContainer) (*structure);
                std::string structureType = stru.structureType();
                if ((int) structureType.find(Screeps::STRUCTURE_EXTENSION) >= 0 ||
                    (int) structureType.find(Screeps::STRUCTURE_CONTAINER) >= 0 ||
                    (int) structureType.find(Screeps::STRUCTURE_STORAGE) >= 0 ||
                    (int) structureType.find(Screeps::STRUCTURE_SPAWN) >= 0) {
                    std::cout << stru.structureType() << "/" << stru.store().getFreeCapacity().value_or(-1)
                              << std::endl;
                    if (stru.store().getFreeCapacity().value_or(-1) >= 0) {
                        comp::emptyContainer[s.name()] = std::move(
                                std::make_unique<Screeps::StructureContainer>(structure->value()));
                    }
                }
            }
        }
    }
}

#endif //EXAMPLE_STRUCTURESSYSTEM_HPP
