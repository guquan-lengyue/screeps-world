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
	Screeps::StructureSpawn homeSpawn = Screeps::Game.spawns().find("home")->second;
	std::vector<std::string> workBodyPart = {Screeps::MOVE, Screeps::CARRY, Screeps::WORK};
	for (int i = 0; i < 5; i++)
	{
		homeSpawn.spawnCreep(workBodyPart, "work_" + std::to_string(i));
	}

	for (auto &c : creeps)
	{
		Screeps::Creep creep = c.second;

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
			if (creep.transfer(homeSpawn, Screeps::RESOURCE_ENERGY) == Screeps::ERR_NOT_IN_RANGE)
			{
				creep.moveTo(homeSpawn);
			}
		}
	}
}

EMSCRIPTEN_BINDINGS(loop)
{
	emscripten::function("loop", &loop);
}
