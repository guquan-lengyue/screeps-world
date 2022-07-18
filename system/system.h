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
#include <Screeps/Memory.hpp>
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
            int before_harvester_num = 0;
            auto s = (Spawn) spawn.second;
            auto room = (Screeps::Room) s.room();
            auto creeps = room.find(Screeps::FIND_MY_CREEPS);
            auto construction_sizes = s.room().find(Screeps::FIND_CONSTRUCTION_SITES);
//            auto damageRoomObject = s.room().find(Screeps::FIND_STRUCTURES, [](const JS::Value &value) {
//                return value["hits"].as<float>() / value["hitsMax"].as<float>() < 0.7f;
//            });
            int renewNum = 0;
            for (const auto &creep: creeps) {
                auto c = ((Creep) *creep);
                std::string role = c.getMemoryOr("role", "");
                if (role.empty()) {
                    c.setMemory("RECYCLE", "true");
                }
                std::string renew = c.getMemoryOr("RENEW", "false");
                std::string before_role = c.getMemory("beforeRole");
                if (before_role == "HARVESTER") {
                    ++before_harvester_num;
                }
                if (renew == "true") {
                    ++renewNum;
                }
                if (renew == "false" && c.ticksToLive() < 300 && renewNum <= 1) {
                    c.setMemory("RENEW", "true");
                }
                if (renew == "true" && c.ticksToLive() > 1400) {
                    c.setMemory("RENEW", "false");
                }
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
            s.setMemory("before_harvester_num", std::to_string(before_harvester_num));
        }
    }

    void spawns_spawn_creep() {
        if (Screeps::Game.time() % 50 != 0) {
            return;
        }
        for (auto &spawn: Screeps::Game.spawns()) {
            auto s = (Spawn) spawn.second;
            auto construction_sizes = s.room().find(Screeps::FIND_CONSTRUCTION_SITES);

            auto spawnCreep = [&](const std::string &role, int num) {
                for (int i = 6; i > 0 && !role.empty(); --i) {
                    auto rst = s.spawnCreep(get_worker_body(i),
                                            s.name() + "_" + role + "_" + std::to_string(num),
                                            roleOpt(role));
                    if (rst >= 0) {
                        return true;
                    }
                }
                return false;
            };
            bool spawnFlag = false;
            for (int i = 0; i < 14; ++i) {
                if (spawnFlag = spawnCreep("HARVESTER", i), spawnFlag) {
                    break;
                }
            }
            if (spawnFlag) {
                return;
            }
            if (std::stoi(s.getMemoryOr("harvester_num", "0")) > 6) {
                for (int i = 0; i < 6; ++i) {
                    if (spawnFlag = spawnCreep("UPGRADER", i), spawnFlag) {
                        break;
                    }
                }
                if (spawnFlag) {
                    return;
                }
                for (int i = 0; i < 6; ++i) {
                    if (spawnFlag = spawnCreep("UPGRADER", i), spawnFlag) {
                        break;
                    }
                }
                if (spawnFlag) {
                    return;
                }
                for (int i = 0; i < 2; ++i) {
                    if (spawnFlag = spawnCreep("REPAIRER", i), spawnFlag) {
                        break;
                    }
                }
                if (spawnFlag) {
                    return;
                }
                for (int i = 0; i < 2; ++i) {
                    if (spawnFlag = spawnCreep("BUILDER", i), spawnFlag) {
                        break;
                    }
                }
            }
        }
    }

    bool creep() {
        for (auto &spawn: Screeps::Game.spawns()) {
            Spawn s = (Spawn) spawn.second;
            auto room = s.room();
            auto sources = s.room().find(Screeps::FIND_SOURCES);
            auto creeps = room.find(Screeps::FIND_MY_CREEPS);
            auto construction_sizes = s.room().find(Screeps::FIND_CONSTRUCTION_SITES);
            auto damageRoomObject = s.room().find(Screeps::FIND_STRUCTURES, [](const JS::Value &value) {
                return value["hits"].as<float>() / value["hitsMax"].as<float>() < 0.7f;
            });

            Screeps::StructureContainer emptyContainer = ((Screeps::StructureContainer) s);
            Screeps::StructureContainer fullContainer = ((Screeps::StructureContainer) s);
            auto &emptyContainers = comp::emptyContainers[s.name()];
            auto &fullContainers = comp::fullContainers[s.name()];
            for (int i = 0; i < creeps.size(); ++i) {
                Creep c = (Creep) (*creeps[i]);
                std::string role = c.getMemory("role");
                std::string renew = c.getMemoryOr("RENEW", "false");
                std::string recycle = c.getMemoryOr("RECYCLE", "false");
                auto source = (Screeps::Source) (*(sources[i % sources.size()]));
                if (!emptyContainers.empty()) {
                    emptyContainer = *emptyContainers[i % emptyContainers.size()];
                }
                if (!fullContainers.empty()) {
                    fullContainer = *fullContainers[i % fullContainers.size()];
                }
                if (renew == "true") {
                    util::renew(c, s);
                    continue;
                }
                if (recycle == "true") {
                    util::recycle(c, s);
                    continue;
                }
                if (role == "HARVESTER") {
                    util::harvester(c, source, emptyContainer);
                } else if (role == "UPGRADER") {
                    auto controller = room.controller().value();
                    util::upgrade(c, fullContainer, controller);
                } else if (role == "REPAIRER") {
                    if (!damageRoomObject.empty()) {
                        auto damage = (Screeps::Structure) (*damageRoomObject[0]);
                        util::repairer(c, fullContainer, damage);
                    }
                } else if (role == "BUILDER") {
                    if (!construction_sizes.empty()) {
                        auto constructionSize = (Screeps::ConstructionSite) (*construction_sizes[0]);
                        util::build(c, fullContainer, constructionSize);
                    }
                }
            }
        }
        return true;
    }
}

#endif