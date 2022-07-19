#ifndef SYSTEM_WORKER_HPP
#define SYSTEM_WORKER_HPP
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

namespace sys
{
    void check_worker()
    {
        for (auto &spawn : Screeps::Game.spawns())
        {
            int harvester_num = 0;
            int upgrader_num = 0;
            int repairer_num = 0;
            int builder_num = 0;
            int before_harvester_num = 0;
            auto s = (Spawn)spawn.second;
            auto room = (Screeps::Room)s.room();
            auto creeps = room.find(Screeps::FIND_MY_CREEPS);
            auto construction_sizes = s.room().find(Screeps::FIND_CONSTRUCTION_SITES);
            auto damageRoomObject = s.room().find(Screeps::FIND_STRUCTURES, [](const JS::Value &value)
                                                  { return value["hits"].as<float>() / value["hitsMax"].as<float>() < 0.7f; });
            int renewNum = 0;
            for (const auto &creep : creeps)
            {
                auto c = ((Creep)*creep);
                std::string role = c.getMemoryOr("role", "");
                if (role.empty() || c.ticksToLive() < 300)
                {
                    c.setMemory("RECYCLE", "true");
                }
                std::string renew = c.getMemoryOr("RENEW", "false");
                std::string before_role = c.getMemory("beforeRole");
                if (before_role == "HARVESTER")
                {
                    ++before_harvester_num;
                }
                if (renew == "true")
                {
                    ++renewNum;
                }
                if (renew == "false" && c.ticksToLive() < 800 && renewNum <= 1)
                {
                    c.setMemory("RENEW", "true");
                }
                if (renew == "true" && c.ticksToLive() > 1400)
                {
                    c.setMemory("RENEW", "false");
                }
                if (role == "HARVESTER")
                {
                    ++harvester_num;
                }
                else if (role == "UPGRADER")
                {
                    ++upgrader_num;
                }
                else if (role == "REPAIRER")
                {
                    if (damageRoomObject.empty())
                    {
                        if (damageRoomObject.empty())
                        {
                            c.setMemory("beforeRole", c.getMemory("role"));
                            c.setMemory("role", "BUILDER");
                            ++builder_num;
                            continue;
                        }
                    }
                    ++repairer_num;
                }
                else if (role == "BUILDER")
                {
                    if (construction_sizes.empty())
                    {
                        c.setMemory("beforeRole", c.getMemory("role"));
                        c.setMemory("role", "REPAIRER");
                        ++repairer_num;
                        continue;
                    }
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

    bool worker()
    {
        for (auto &spawn : Screeps::Game.spawns())
        {
            Spawn s = (Spawn)spawn.second;
            auto room = s.room();
            auto sources = s.room().find(Screeps::FIND_SOURCES);
            auto creeps = room.find(Screeps::FIND_MY_CREEPS);
            auto construction_sizes = s.room().find(Screeps::FIND_CONSTRUCTION_SITES);
            auto damageRoomObject = s.room().find(Screeps::FIND_STRUCTURES, [](const JS::Value &value)
                                                  { return value["hits"].as<float>() / value["hitsMax"].as<float>() < 0.7f; });

            Screeps::StructureContainer emptyContainer = ((Screeps::StructureContainer)s);
            Screeps::StructureContainer fullContainer = ((Screeps::StructureContainer)s);
            auto &emptyContainers = comp::emptyContainers[s.name()];
            auto &fullContainers = comp::fullContainers[s.name()];
            for (int i = 0; i < creeps.size(); ++i)
            {
                Creep c = (Creep)(*creeps[i]);
                std::string role = c.getMemory("role");
                std::string renew = c.getMemoryOr("RENEW", "false");
                std::string recycle = c.getMemoryOr("RECYCLE", "false");
                auto source = (Screeps::Source)(*(sources[i % sources.size()]));
                if (!emptyContainers.empty())
                {
                    emptyContainer = *emptyContainers[i % emptyContainers.size()];
                }
                if (!fullContainers.empty())
                {
                    fullContainer = *fullContainers[i % fullContainers.size()];
                }
                if (renew == "true")
                {
                    util::renew(c, s);
                    continue;
                }
                if (recycle == "true")
                {
                    util::recycle(c, s);
                    continue;
                }
                if (role == "HARVESTER")
                {
                    util::harvester(c, source, emptyContainer);
                }
                else if (role == "UPGRADER")
                {
                    auto controller = room.controller().value();
                    util::upgrade(c, fullContainer, controller);
                }
                else if (role == "REPAIRER")
                {
                    if (!damageRoomObject.empty())
                    {
                        auto damage = (Screeps::Structure)(*damageRoomObject[0]);
                        util::repairer(c, fullContainer, damage);
                    }
                }
                else if (role == "BUILDER")
                {
                    if (!construction_sizes.empty())
                    {
                        auto constructionSize = (Screeps::ConstructionSite)(*construction_sizes[0]);
                        util::build(c, fullContainer, constructionSize);
                    }
                }
            }
        }
        return true;
    }
} // namespace sys
#endif
