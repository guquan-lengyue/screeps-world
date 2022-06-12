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
#include <Screeps/ConstructionSite.hpp>
#include <Screeps/StructureExtension.hpp>
#define SAY_HARVEST "🔄"
#define SAY_BUILD "🚧"
JSON creepMoveToOpt = {"reusePath", 100};

void miner(Screeps::Creep &creep, Screeps::Source &source, Screeps::Structure &target);
void upgrade(Screeps::Creep &upgrade, Screeps::Source &source, Screeps::StructureController &target);
void build(Screeps::Creep &builder, Screeps::Source &source, Screeps::ConstructionSite &target);

EMSCRIPTEN_KEEPALIVE
extern "C" void loop()
{
	Screeps::Context::update();

	std::map<std::string, Screeps::Creep> creeps = Screeps::Game.creeps();
	Screeps::StructureSpawn homeSpawn = Screeps::Game.spawns().find("home")->second;
	std::vector<std::string> workBodyPart = {Screeps::MOVE, Screeps::CARRY, Screeps::WORK};
	auto sources = homeSpawn.room().find(Screeps::FIND_SOURCES);
	Screeps::StructureController homeController = homeSpawn.room().controller().value();

	Screeps::StructureExtension *homeExtension;
	auto structureList = homeSpawn.room().find(Screeps::FIND_STRUCTURES);
	for (auto it = structureList.begin(); it != structureList.end(); it++)
	{
		auto tmp = new Screeps::StructureExtension(it->get()->value());
		if (tmp->structureType() == Screeps::STRUCTURE_EXTENSION)
		{
			if (tmp->store().getFreeCapacity() > 0)
			{
				homeExtension = tmp;
			}
		}
		else
		{
			delete tmp;
		}
	}

	for (int i = 0; i < 5; i++)
	{
		homeSpawn.spawnCreep(workBodyPart, "upgradetor_" + std::to_string(i));
	}
	for (int i = 0; i < 5; i++)
	{
		homeSpawn.spawnCreep(workBodyPart, "build_" + std::to_string(i));
	}
	for (int i = 0; i < 4; i++)
	{
		homeSpawn.spawnCreep(workBodyPart, "work_" + std::to_string(i));
	}
	auto constructionSites = homeSpawn.room().find(Screeps::FIND_CONSTRUCTION_SITES);
	Screeps::ConstructionSite site(constructionSites[0].get()->value());

	for (auto &c : creeps)
	{
		Screeps::Creep creep = c.second;
		if ((int)creep.name().find("work_") >= 0)
		{
			Screeps::Source source(sources[1].get()->value());
			miner(creep, source, *homeExtension);
			delete homeExtension;
		}
		if ((int)creep.name().find("upgradetor_") >= 0)
		{
			Screeps::Source source(sources[0].get()->value());
			upgrade(creep, source, homeController);
		}
		if ((int)creep.name().find("build_") >= 0)
		{
			Screeps::Source source(sources[0].get()->value());
			build(creep, source, site);
		}
	}
}

void miner(Screeps::Creep &creep, Screeps::Source &source, Screeps::Structure &target)
{
	if (creep.store().getFreeCapacity() > 0)
	{
		if (creep.harvest(source) == Screeps::ERR_NOT_IN_RANGE)
		{
			creep.moveTo(source, creepMoveToOpt);
		}
	}
	else
	{
		if (creep.transfer(target, Screeps::RESOURCE_ENERGY) == Screeps::ERR_NOT_IN_RANGE)
		{
			creep.moveTo(target, creepMoveToOpt);
		}
	}
}

void upgrade(Screeps::Creep &upgrade, Screeps::Source &source, Screeps::StructureController &target)
{
	JSON upgradeMemory = upgrade.memory();
	std::string action = "isUpgradeing";
	bool isUpgrading;
	if (!upgradeMemory.contains(action))
	{
		upgradeMemory[action] = true;
	}
	upgradeMemory[action].get_to<bool>(isUpgrading);
	if (isUpgrading && upgrade.store().getUsedCapacity() == 0)
	{
		isUpgrading = false;
		upgrade.say(SAY_HARVEST);
	}
	if (!isUpgrading && upgrade.store().getFreeCapacity() == 0)
	{
		isUpgrading = true;
		upgrade.say(SAY_BUILD);
	}
	upgradeMemory[action] = isUpgrading;
	upgrade.setMemory(upgradeMemory);
	if (isUpgrading)
	{
		if (upgrade.upgradeController(target) == Screeps::ERR_NOT_IN_RANGE)
		{
			upgrade.moveTo(target, creepMoveToOpt);
		}
	}
	else
	{
		if (upgrade.harvest(source) == Screeps::ERR_NOT_IN_RANGE)
		{
			upgrade.moveTo(source, creepMoveToOpt);
		}
	}
}
void build(Screeps::Creep &builder, Screeps::Source &source, Screeps::ConstructionSite &target)
{
	JSON builderMemory = builder.memory();
	bool isBuilding;
	if (!builderMemory.contains("isBuilding"))
	{
		builderMemory["isBuilding"] = true;
	}
	builderMemory["isBuilding"].get_to(isBuilding);
	if (isBuilding && builder.store().getUsedCapacity() == 0)
	{
		isBuilding = false;
		builder.say(SAY_HARVEST);
	}
	if (!isBuilding && builder.store().getFreeCapacity() == 0)
	{
		isBuilding = true;
		builder.say(SAY_BUILD);
	}

	builderMemory["isBuilding"] = isBuilding;
	builder.setMemory(builderMemory);
	if (isBuilding)
	{
		if (builder.build(target) == Screeps::ERR_NOT_IN_RANGE)
		{
			builder.moveTo(target, creepMoveToOpt);
		}
	}
	else
	{
		if (builder.harvest(source) == Screeps::ERR_NOT_IN_RANGE)
		{
			builder.moveTo(source, creepMoveToOpt);
		}
	}
}

EMSCRIPTEN_BINDINGS(loop)
{
	emscripten::function("loop", &loop);
}
