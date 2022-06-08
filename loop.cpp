#include <Screeps/Context.hpp>
#include <Screeps/Creep.hpp>
#include <Screeps/Room.hpp>
#include <Screeps/RoomObject.hpp>
#include <Screeps/RoomPosition.hpp>
#include <Screeps/Source.hpp>
#include <Screeps/Store.hpp>
#include <Screeps/StructureSpawn.hpp>
#include <Screeps/Constants.hpp>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <iostream>
EMSCRIPTEN_KEEPALIVE
extern "C" void loop()
{
	Screeps::Context::update();

	std::map<std::string, Screeps::Creep> creeps = Screeps::Game.creeps();
	for (auto &c : creeps)
	{
		Screeps::Creep creep = c.second;
		Screeps::StructureSpawn spawn1 = Screeps::Game.spawns().find("Spawn1")->second;
		Screeps::Room room = creep.room();

		if (creep.store().getFreeCapacity() > 0)
		{
			std::vector<std::unique_ptr<Screeps::RoomObject>> sources = room.find(Screeps::FIND_SOURCES);
			Screeps::Source source(sources[0].get()->value());
			if (creep.harvest(source) == Screeps::ERR_NOT_IN_RANGE)
			{
				creep.moveTo(source);
			}
		}
		else
		{
			if (creep.transfer(spawn1, Screeps::RESOURCE_ENERGY) == Screeps::ERR_NOT_IN_RANGE)
			{
				creep.moveTo(spawn1);
			}
		}
	}
}

EMSCRIPTEN_BINDINGS(loop)
{
	emscripten::function("loop", &loop);
}
