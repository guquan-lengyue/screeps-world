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
#include <optional>
#include <Screeps/StructureController.hpp>
EMSCRIPTEN_KEEPALIVE

void miner(Screeps::Creep &creep, Screeps::Source &source, Screeps::Structure &target);
void upgrade(Screeps::Creep &upgrade, Screeps::Source &source);
extern "C" void loop()
{
	Screeps::Context::update();

	std::map<std::string, Screeps::Creep> creeps = Screeps::Game.creeps();
	Screeps::StructureSpawn homeSpawn = Screeps::Game.spawns().find("home")->second;
	std::vector<std::string> workBodyPart = {Screeps::MOVE, Screeps::CARRY, Screeps::WORK};
	auto sources = homeSpawn.room().find(Screeps::FIND_SOURCES);
	for (int i = 0; i < 4; i++)
	{
		homeSpawn.spawnCreep(workBodyPart, "work_" + std::to_string(i));
	}
	for (int i = 0; i < 20; i++)
	{
		homeSpawn.spawnCreep(workBodyPart, "upgradetor_" + std::to_string(i));
	}

	for (auto &c : creeps)
	{
		Screeps::Creep creep = c.second;
		if ((int)creep.name().find("work_") >= 0)
		{
			Screeps::Source source(sources[0].get()->value());
			miner(creep, source, homeSpawn);
		}
		if ((int)creep.name().find("upgradetor_") >= 0)
		{
			Screeps::Source source(sources[1].get()->value());
			upgrade(creep, source);
		}
	}
}

void miner(Screeps::Creep &creep, Screeps::Source &source, Screeps::Structure &target)
{
	if (creep.store().getFreeCapacity() > 0)
	{
		if (creep.harvest(source) == Screeps::ERR_NOT_IN_RANGE)
		{
			creep.moveTo(source);
		}
	}
	else
	{
		if (creep.transfer(target, Screeps::RESOURCE_ENERGY) == Screeps::ERR_NOT_IN_RANGE)
		{
			creep.moveTo(target);
		}
	}
}

void upgrade(Screeps::Creep &upgrade, Screeps::Source &source)
{
	if (upgrade.store().getFreeCapacity() > 0)
	{
		if (upgrade.harvest(source) == Screeps::ERR_NOT_IN_RANGE)
		{
			upgrade.moveTo(source);
		}
	}
	else
	{
		Screeps::StructureController controller = upgrade.room().controller().value();
		if (upgrade.upgradeController(controller) == Screeps::ERR_NOT_IN_RANGE)
		{
			upgrade.moveTo(controller);
		}
	}
}
EMSCRIPTEN_BINDINGS(loop)
{
	emscripten::function("loop", &loop);
}
