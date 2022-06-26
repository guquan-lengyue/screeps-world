//
// Created by 19278 on 2022/6/26.
//

#ifndef EXAMPLE_GCONFIG_HPP
#define EXAMPLE_GCONFIG_HPP

#include <Screeps/Memory.hpp>
#include <Screeps/JS.hpp>
#include <map>
#include <string>

using namespace std;
map<string, map<string, int>> config;

class GConfig {
private:
    static void initConfig();

public:
    void update();

    int getHarvesterNum();

    int getUpgraderNum();

    int getBuilderNum();

    int getRepairerNum();


};

void GConfig::update() {
    Screeps::Memory["G_CONFIG"].get_to<map<string, map<string, int>>>(config);
}

void GConfig::initConfig() {
    map<string, int> screepsNum;
    screepsNum["HARVESTER_NUM"] = 12;
    screepsNum["UPGRADER_NUM"] = 4;
    screepsNum["BUILDER_NUM"] = 4;
    screepsNum["REPAIRER_NUM"] = 4;
    config["G_CONFIG"] = screepsNum;
    Screeps::Memory.set(JS::mapToJSObject(config));
}

int GConfig::getHarvesterNum() {
    return config["G_CONFIG"]["HARVESTER_NUM"];
}

int GConfig::getUpgraderNum(){
    return config["G_CONFIG"]["UPGRADER_NUM"];
}

int GConfig::getBuilderNum(){
    return config["G_CONFIG"]["BUILDER_NUM"];
}

int GConfig::getRepairerNum(){
    return config["G_CONFIG"]["REPAIRER_NUM"];
}


#endif //EXAMPLE_GCONFIG_HPP
