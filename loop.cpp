#include <Screeps/Context.hpp>

#include <emscripten.h>
#include <emscripten/bind.h>
#include <iostream>
#include "system/system.h"

EMSCRIPTEN_KEEPALIVE
extern "C" void updateData()
{
    std::cout << "loading" << std::endl;
    Screeps::Context::update();
    scrsys::update_spawn();
}

EMSCRIPTEN_KEEPALIVE
extern "C" void loop()
{
    scrsys::spawn_check_creep();
    scrsys::spawn_creep();
}

EMSCRIPTEN_BINDINGS(loop)
{
    emscripten::function("loop", &loop);
}
EMSCRIPTEN_BINDINGS(updateData)
{
    emscripten::function("updateData", &updateData);
}