#ifndef SYSTEM_HPP
#define SYSTEM_HPP

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
#include <string>
#include "utils/worker.hpp"

JSON getMoveToOpt() {
    auto opt = JS::Value::object();
    opt.set("fill", "red");
    opt.set("stroke", "#fff");
    opt.set("lineStyle", "dashed");
    opt.set("strokeWidth", 0.5);
    opt.set("opacity", 0.5);
    return JS::toJSON(opt);
}

std::vector<std::string> get_worker_body(int level) {
    std::vector<std::string> bodys;
    for (int i = 0; i < level; ++i) {
        bodys.emplace_back(Screeps::WORK);
        bodys.emplace_back(Screeps::CARRY);
        bodys.emplace_back(Screeps::MOVE);
        bodys.emplace_back(Screeps::MOVE);
    }
    return bodys;
}

std::vector<std::string> get_soldier_body(int level) {
    std::vector<std::string> bodys;
    for (int i = 0; i < level; ++i) {
        bodys.emplace_back(Screeps::ATTACK);
        bodys.emplace_back(Screeps::CARRY);
        bodys.emplace_back(Screeps::MOVE);
        bodys.emplace_back(Screeps::MOVE);
    }
    return bodys;
}

namespace sys {
    void check_creep() {
        for (auto &spawn: Screeps::Game.spawns()) {
            int harvester_num = 0;
            int upgrader_num = 0;
            int repairer_num = 0;
            int builder_num = 0;
            auto s = (Spawn) spawn.second;
            auto room = (Screeps::Room) s.room();
            auto creeps = room.find(Screeps::FIND_MY_CREEPS);
            for (const auto &creep: creeps) {
                auto c = ((Creep) *creep);
                std::string role = c.getMemory("role");
                if (role == "HARVESTER") {
                    ++harvester_num;
                } else if (role == "UPGRADER") {
                    ++upgrader_num;
                } else if (role == "REPAIRER") {
                    ++repairer_num;
                } else if (role == "BUILDER") {
                    ++builder_num;
                }
            }
            s.setMemory("harvester_num", std::to_string(harvester_num));
            s.setMemory("upgrader_num", std::to_string(upgrader_num));
            s.setMemory("repairer_num", std::to_string(repairer_num));
            s.setMemory("builder_num", std::to_string(builder_num));
        }
    }

    void spawns_spawn_creep() {
        for (auto &spawn: Screeps::Game.spawns()) {
            auto s = (Spawn) spawn.second;
            auto construction_sizes = s.room().find(Screeps::FIND_CONSTRUCTION_SITES);
            std::string role;
            if (std::stoi(s.getMemory("harvester_num")) < 10) {
                role = "HARVESTER";
            } else if (std::stoi(s.getMemory("upgrader_num")) < 4) {
                role = "UPGRADER";
            } else if (std::stoi(s.getMemory("repairer_num")) < 2) {
                role = "REPAIRER";
            } else if (!construction_sizes.empty() && std::stoi(s.getMemory("builder_num")) < 4) {
                role = "BUILDER";
            }
            for (int i = 6; i > 0 && !role.empty(); --i) {
                auto rst = s.spawnCreep(get_worker_body(i),
                                        s.name() + role + std::to_string(Screeps::Game.time()),
                                        roleOpt(role));
                if (rst >= 0) {
                    break;
                }
            }
        }
    }

    void creep() {
        for (auto &spawn: Screeps::Game.spawns()) {
            Spawn s = (Spawn) spawn.second;
            auto room = s.room();
            auto sources = s.room().find(Screeps::FIND_SOURCES);
            auto creeps = room.find(Screeps::FIND_MY_CREEPS);
            auto construction_sizes = s.room().find(Screeps::FIND_CONSTRUCTION_SITES);
            auto damageRoomObject = s.room().find(Screeps::FIND_STRUCTURES, [](const JS::Value value) {
                return value["hits"].as<float>() / value["hitsMax"].as<float>() < 0.7f;
            });
            int i = 0;
            for (const auto &creep: creeps) {
                Creep c = (Creep) (*creep);
                std::string role = c.getMemory("role");
                auto source = (Screeps::Source) (*(sources[++i % sources.size()]));
                if (role == "HARVESTER") {
                    harvester(c, source, s);
                } else if (role == "UPGRADER") {
                    auto controller = room.controller().value();
                    upgrade(c, s, controller);
                } else if (role == "REPAIRER") {
                    if (!damageRoomObject.empty()) {
                        auto damage = (Screeps::Structure) (*damageRoomObject[0]);
                        repairer(c, s, damage);
                    }
                } else if (role == "BUILDER") {
                    if (construction_sizes.empty()) {
                        c.setMemory("role", "REPAIRER");
                        continue;
                    }
                    auto constructionSize = (Screeps::ConstructionSite) (*construction_sizes[0]);
                    build(c, s, constructionSize);
                }
            }
        }
    }
}

#endif