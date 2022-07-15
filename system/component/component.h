#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include "object/Spawn.hpp"
#include <Screeps/Creep.hpp>
#include <map>
#include <string>
#include <vector>
#include <Screeps/Source.hpp>

std::map<std::string, Spawn> spawns;
std::vector<Screeps::Creep> creeps;
std::vector<Screeps::Source> sources;
#endif