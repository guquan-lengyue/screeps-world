#ifndef CREEP_ROLE_HARVESTER
#define CREEP_ROLE_HARVESTER

#include <Screeps/Creep.hpp>

class Harvester : public Screeps::Creep {
private:
    /* data */
public:
    static std::vector<std::string> bodyParts();

    Harvester() = default;

    explicit Harvester(JS::Value creep);

    ~Harvester() = default;

    int work(Screeps::Source &source, Screeps::Structure &structure);

    static std::string namePre() { return "Harvester_"; };
};

#endif