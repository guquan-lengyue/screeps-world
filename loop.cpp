#include <Screeps/Context.hpp>
#include <Screeps/Creep.hpp>
#include <Screeps/Room.hpp>
#include <Screeps/RoomObject.hpp>
#include <Screeps/RoomPosition.hpp>
#include <Screeps/Source.hpp>
#include <Screeps/Constants.hpp>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <iostream>
EMSCRIPTEN_KEEPALIVE
extern "C" void loop()
{
	Screeps::Context::update();

	auto creeps = Screeps::Game.creeps();
	auto creeps_it = creeps.find("Harvester1");
	auto creep = (*creeps_it).second;
	creep.say("hello world");
	Screeps::Room room = creep.room();

	std::vector<std::unique_ptr<Screeps::RoomObject>> sources = room.find(Screeps::FIND_SOURCES);
	// Screeps::Source source(sources[0].get()->value());
	// if (creep.harvest(source) == Screeps::ERR_NOT_IN_RANGE)
	// {
	// 	creep.moveTo(source);
	// }
}

EMSCRIPTEN_BINDINGS(loop)
{
	emscripten::function("loop", &loop);
}
