#ifndef CREEP_ROLE_HARVESTER
#define CREEP_ROLE_HARVESTER
#include <Screeps/Creep.hpp>
class Harvester : public Screeps::Creep
{
private:
    /* data */
public:
    std::vector<std::string> bodyParts();
    Harvester() = default;
    ~Harvester() = default;
    int work();
};
#endif