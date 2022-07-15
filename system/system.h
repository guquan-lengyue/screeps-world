#ifndef SYSTEM_HPP
#define SYSTEM_HPP
#include "../component/component.h"
#include <Screeps/Room.hpp>
#include <Screeps/RoomObject.hpp>
#include <Screeps/Game.hpp>
#include <Screeps/Constants.hpp>
#include <Screeps/Creep.hpp>
std::vector<std::string> get_worker_body(int level)
{
    std::vector<std::string> bodys;
    for (int i = 0; i < level; ++i)
    {
        bodys.push_back(Screeps::WORK);
        bodys.push_back(Screeps::CARRY);
        bodys.push_back(Screeps::MOVE);
        bodys.push_back(Screeps::MOVE);
    }
    return bodys;
}
std::vector<std::string> get_soldier_body(int level)
{
    std::vector<std::string> bodys;
    for (int i = 0; i < level; ++i)
    {
        bodys.push_back(Screeps::ATTACK);
        bodys.push_back(Screeps::CARRY);
        bodys.push_back(Screeps::MOVE);
        bodys.push_back(Screeps::MOVE);
    }
    return bodys;
}

namespace sys
{
    void update_spawn()
    {
        for (const auto &item : Screeps::Game.spawns())
        {
            spawns.insert(std::pair<std::string,Spawn>(item.first,(Spawn)item.second));
        }
    }

    void spawn_check_creep()
    {
        for (auto &spawn : spawns)
        {
            int countWork = 0;
            int countSoldier = 0;
            auto creeps = spawn.second.room().find(Screeps::FIND_MY_CREEPS);
            for (const auto &creep : creeps)
            {
                std::string name = ((Screeps::Creep)(*creep)).name();
                if (name.find("Work") >= 0)
                {
                    ++countWork;
                }
                else if (name.find("Soldier"))
                {
                    ++countSoldier;
                }
            }
            if (countWork < 16)
            {
                spawn.second.setMemory("spawn", "Work");
            }
            else if (countSoldier < 4)
            {
                spawn.second.setMemory("spawn", "Soldier");
            }
            else
            {
                spawn.second.setMemory("spawn", "");
            }
        }
    }

    void spawn_creep()
    {
        for (auto &spawn : spawns)
        {
            std::string spawnTask = spawn.second.getMemory("spawn");
            if (spawnTask == "Work")
            {
                for (int i = 6; i > 0; i--)
                {
                    spawn.second.spawnCreep(get_worker_body(i), "Work" + spawn.second.name());
                }
            }
            else if (spawnTask == "Soldier")
            {
                for (int i = 6; i > 0; i--)
                {
                    spawn.second.spawnCreep(get_worker_body(i), "Soldier" + spawn.second.name());
                }
            }
        }
    }
}

#endif