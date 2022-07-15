#include <Screeps/Context.hpp>
#include <Screeps/Creep.hpp>

#include <emscripten.h>
#include <emscripten/bind.h>
#include <iostream>
#include "system/system.h"

EMSCRIPTEN_KEEPALIVE
extern "C"

void updateData() {
    std::cout << "updateData ??" << std::endl;
    Screeps::Context::update();
    sys::update_spawn();
}


EMSCRIPTEN_KEEPALIVE
extern "C"

void loop() {
    sys::spawn_check_creep();
    sys::spawn_creep();

}

EMSCRIPTEN_BINDINGS(loop)
{
    emscripten::function("loop", &loop);
    emscripten::function("updateData", &updateData);
}
