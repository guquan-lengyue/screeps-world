#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include "object/Spawn.hpp"
#include <Screeps/Creep.hpp>
#include <map>
#include <string>
#include <vector>
#include <Screeps/StructureContainer.hpp>

namespace comp {
    std::map<std::string, std::vector<std::unique_ptr<Screeps::StructureContainer>>> spawn2container;
    std::map<std::string, std::unique_ptr<Screeps::StructureContainer>> emptyContainer;
}
#endif