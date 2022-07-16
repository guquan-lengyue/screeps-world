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
        comp::spawn2container.clear();
        for (auto &spawn: Screeps::Game.spawns()) {
            auto s = (Spawn) spawn.second;
            auto room = s.room();
            auto structures = room.find(Screeps::FIND_MY_STRUCTURES);
            std::vector<Screeps::StructureContainer> container;
            for (const auto &structure: structures) {
                auto stru = (Screeps::StructureContainer) (*structure);
                std::string structureType = stru.structureType();
                if ((int) structureType.find(Screeps::STRUCTURE_EXTENSION) >= 0 ||
                    (int) structureType.find(Screeps::STRUCTURE_CONTAINER) >= 0 ||
                    (int) structureType.find(Screeps::STRUCTURE_STORAGE) >= 0 ||
                    (int) structureType.find(Screeps::STRUCTURE_SPAWN) >= 0) {
                    container.emplace_back(stru);
                    if (stru.store().getFreeCapacity().value() >= 0) {
                        comp::emptyContainer = std::move(std::make_unique<Screeps::StructureContainer>(stru));
                    }
                }
            }
            std::pair<std::string, std::vector<Screeps::StructureContainer>> content(s.name(), container);
            comp::spawn2container.insert(content);
        }
    }
}

#endif //EXAMPLE_STRUCTURESSYSTEM_HPP
