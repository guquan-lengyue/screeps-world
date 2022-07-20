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

JSON getMoveToOpt()
{
    auto opt = JS::Value::object();
    opt.set("fill", "red");
    opt.set("stroke", "#fff");
    opt.set("lineStyle", "dashed");
    opt.set("strokeWidth", 0.5);
    opt.set("opacity", 0.5);
    return JS::toJSON(opt);
}

std::vector<std::string> get_worker_body(int level)
{
    std::vector<std::string> bodys;
    for (int i = 0; i < level; ++i)
    {
        bodys.emplace_back(Screeps::WORK);
        bodys.emplace_back(Screeps::CARRY);
        bodys.emplace_back(Screeps::MOVE);
        bodys.emplace_back(Screeps::MOVE);
    }
    return bodys;
}

std::vector<std::string> get_soldier_body(int level)
{
    std::vector<std::string> bodys;
    for (int i = 0; i < level; ++i)
    {
        bodys.emplace_back(Screeps::ATTACK);
        bodys.emplace_back(Screeps::CARRY);
        bodys.emplace_back(Screeps::MOVE);
        bodys.emplace_back(Screeps::MOVE);
    }
    return bodys;
}

namespace sys
{
    void spawns_spawn_creep()
    {
        if (Screeps::Game.time() % 50 != 0)
        {
            return;
        }
        for (auto &spawn : Screeps::Game.spawns())
        {
            auto s = (Spawn)spawn.second;
            auto construction_sizes = s.room().find(Screeps::FIND_CONSTRUCTION_SITES);
            auto damageRoomObject = s.room().find(Screeps::FIND_STRUCTURES, [](const JS::Value &value) {
                return value["hits"].as<float>() / value["hitsMax"].as<float>() < 0.7f;
            });
            auto spawnCreep = [&](const std::string &role, int num)
            {
                for (int i = 4; i > 0 && !role.empty(); --i)
                {
                    int rst = -1;
                    if (role == std::string("SOLDIER"))
                    {
                        rst = s.spawnCreep(get_soldier_body(i),
                                           s.name() + "_" + role + "_" + std::to_string(num),
                                           roleOpt(role));
                    }
                    else
                    {
                        rst = s.spawnCreep(get_worker_body(i),
                                           s.name() + "_" + role + "_" + std::to_string(num),
                                           roleOpt(role));
                    }
                    if (rst >= 0)
                    {
                        return true;
                    }
                }
                return false;
            };
            bool spawnFlag = false;
            for (int i = 0; i < 14; ++i)
            {
                if (spawnFlag = spawnCreep("HARVESTER", i), spawnFlag)
                {
                    break;
                }
            }
            if (spawnFlag)
            {
                return;
            }
            if (std::stoi(s.getMemoryOr("harvester_num", "0")) > 6)
            {
                for (int i = 0; i < 6; ++i)
                {
                    if (spawnFlag = spawnCreep("UPGRADER", i), spawnFlag)
                    {
                        break;
                    }
                }
                if (spawnFlag)
                {
                    return;
                }
                for (int i = 0; i < 6; ++i)
                {
                    if (spawnFlag = spawnCreep("UPGRADER", i), spawnFlag)
                    {
                        break;
                    }
                }
                if (spawnFlag)
                {
                    return;
                }
                for (int i = 0; i < 2 && !damageRoomObject.empty(); ++i)
                {
                    if (spawnFlag = spawnCreep("REPAIRER", i), spawnFlag)
                    {
                        break;
                    }
                }
                if (spawnFlag)
                {
                    return;
                }
                for (int i = 0; i < 2 && !construction_sizes.empty(); ++i)
                {
                    if (spawnFlag = spawnCreep("BUILDER", i), spawnFlag)
                    {
                        break;
                    }
                }
                if (spawnFlag)
                {
                    return;
                }
                for (int i = 0; i < 2; ++i)
                {
                    if (spawnFlag = spawnCreep("SOLDIER", i), spawnFlag)
                    {
                        break;
                    }
                }
                if (spawnFlag)
                {
                    return;
                }
            }
        }
    }


}

#endif