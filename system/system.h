#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include "component/component.h"
#include "Screeps/StructureContainer.hpp"
#include <Screeps/Room.hpp>
#include <Screeps/RoomObject.hpp>
#include <Screeps/Game.hpp>
#include <Screeps/Constants.hpp>
#include <Screeps/Creep.hpp>
#include <Screeps/Store.hpp>
#include <random>

JSON getMoveToOpt() {
    auto opt = JS::Value::object();
    opt.set("fill", "transparent");
    opt.set("stroke", "#fff");
    opt.set("lineStyle", "dashed");
    opt.set("strokeWidth", 0.15);
    opt.set("opacity", 0.1);
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

namespace scrsys {
    void update_spawn() {
        spawns.clear();
        for (const auto &item: Screeps::Game.spawns()) {
            spawns.insert(std::pair<std::string, Spawn>(item.first, (Spawn) item.second));
        }
    }

    void spawn_check_creep() {
        for (auto &spawn: spawns) {
            int countWork = 0;
            int countSoldier = 0;
            auto creeps = spawn.second.room().find(Screeps::FIND_MY_CREEPS);
            for (auto &creep: creeps) {
                std::string name = ((Screeps::Creep) (*creep)).name();
                if ((int) name.find("Work") >= 0) {
                    ++countWork;
                } else if ((int) name.find("Soldier") >= 0) {
                    ++countSoldier;
                }
            }
            std::cout << countWork << std::endl;
            std::cout << countSoldier << std::endl;
            if (countWork < 16) {
                spawn.second.setMemory("spawn", "Work");
            } else if (countSoldier < 4) {
                spawn.second.setMemory("spawn", "Soldier");
            } else {
                spawn.second.setMemory("spawn", "");
            }
        }
    }

    void spawn_creep() {
        for (auto &spawn: spawns) {
            std::string spawnTask = spawn.second.getMemory("spawn");
            std::cout << "spawnTask : " << spawnTask << std::endl;
            if (spawnTask == "Work") {
                for (int i = 6; i > 0; i--) {
                    auto rst = spawn.second.spawnCreep(get_worker_body(i), "Work" + spawn.second.name());
                    std::cout << "spawn rst :" << rst << std::endl;
                }
            } else if (spawnTask == "Soldier") {
                for (int i = 6; i > 0; i--) {
                    spawn.second.spawnCreep(get_soldier_body(i),
                                            "Soldier" + spawn.second.name() + std::to_string(random()));
                }
            }
        }
    }

    void creep_check() {
        creeps.clear();
        for (const auto &item: Screeps::Game.creeps()) {
            creeps.push_back(item.second);
        }
    }

    void source_check() {
        sources.clear();
        for (const auto &spawn: spawns) {
            for (const auto &item: spawn.second.room().find(Screeps::FIND_SOURCES)) {
                sources.emplace_back(Screeps::Source(item->value()));
            }
        }
    }

    void creep_work() {
        auto target = spawns.begin()->second;
        const auto &source = sources[0];
        std::cout << creeps.size() << "creeps num" << std::endl;
        for (auto &item: creeps) {
            if (item.name().find("Work") >= 0) {
                if (item.store().getFreeCapacity().value_or(-1) > 0) {
                    if (item.harvest(source) == Screeps::ERR_NOT_IN_RANGE) {
                        item.moveTo(source);
                    }
                } else {
                    if (item.transfer(target, Screeps::RESOURCE_ENERGY) == Screeps::ERR_NOT_IN_RANGE) {
                        if (target.store().getFreeCapacity(Screeps::RESOURCE_ENERGY).value_or(-1) > 0) {
                            item.moveTo(target, getMoveToOpt());
                        }
                    }
                }
            }
        }
    }
}

#endif