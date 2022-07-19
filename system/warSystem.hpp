#ifndef SYSTEM_WAR_HPP
#define SYSTEM_WAR_HPP
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
    bool soldier()
    {
        for (auto &spawn : Screeps::Game.spawns())
        {
            Spawn s = (Spawn)spawn.second;
            auto room = s.room();
            auto creeps = room.find(Screeps::FIND_CREEPS);
            std::unique_ptr<Screeps::RoomObject> enermy;
            for (auto &creep : creeps)
            {
                Creep c = (Creep)(*creep);
                if (!c.my())
                {
                    enermy = std::move(creep);
                }
                if (enermy)
                {
                    c.attack(*enermy);
                }
            }
        }
        return true;
    }
} // namespace sys

#endif