//
// Created by 19278 on 2022/6/26.
//

#ifndef EXAMPLE_GCONFIG_HPP
#define EXAMPLE_GCONFIG_HPP

#include <Screeps/Memory.hpp>
#include <Screeps/JS.hpp>
#include <Screeps/JSON.hpp>
#include <map>
#include <string>

using namespace std;
map<string, int> config;

class GConfig
{
private:
    static void initConfig();

public:
    static void update();

    static int getHarvesterNum();

    static int getUpgraderNum();

    static int getBuilderNum();

    static int getRepairerNum();
};

void GConfig::update()
{
    if (Screeps::Memory["G_CONFIG"].contains("HARVESTER_NUM"))
    {
        Screeps::Memory["G_CONFIG"].get_to<map<string, int>>(config);
    }
    else
    {
        GConfig::initConfig();
    }
}

void GConfig::initConfig()
{
    map<string, int> screepsNum;
    screepsNum["HARVESTER_NUM"] = 12;
    screepsNum["UPGRADER_NUM"] = 4;
    screepsNum["BUILDER_NUM"] = 4;
    screepsNum["REPAIRER_NUM"] = 4;
    auto config_ = JS::mapToJSObject<std::string, int>(screepsNum);
    Screeps::Memory.set("G_CONFIG", JS::toJSON(config_));
}

int GConfig::getHarvesterNum()
{
    return config["HARVESTER_NUM"];
}

int GConfig::getUpgraderNum()
{
    return config["UPGRADER_NUM"];
}

int GConfig::getBuilderNum()
{
    return config["BUILDER_NUM"];
}

int GConfig::getRepairerNum()
{
    return config["REPAIRER_NUM"];
}

#endif // EXAMPLE_GCONFIG_HPP
