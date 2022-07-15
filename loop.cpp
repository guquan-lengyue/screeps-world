#include <Screeps/Context.hpp>
#include <Screeps/Creep.hpp>

#include <emscripten.h>
#include <emscripten/bind.h>
#include <iostream>
#include "system/system.h"

EMSCRIPTEN_KEEPALIVE
extern "C"

void initData() {
    std::cout << "initData ??" << std::endl;
    Screeps::Context::update();
    system::update_spawn()
}


EMSCRIPTEN_KEEPALIVE
extern "C"

void loop() {
    system::spawn_check_creep();
    system::spawn_creep()

}

EMSCRIPTEN_BINDINGS(loop)
{
    emscripten::function("loop", &loop);
    emscripten::function("initData", &initData);
}
