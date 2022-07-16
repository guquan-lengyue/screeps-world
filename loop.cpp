#include <Screeps/Context.hpp>

#include <emscripten.h>
#include <emscripten/bind.h>
#include <iostream>
#include "system/system.h"
#include "system/structuresSystem.hpp"

EMSCRIPTEN_KEEPALIVE
extern "C" void updateData() {
    std::cout << "loading" << std::endl;
}

EMSCRIPTEN_KEEPALIVE
extern "C" void loop() {
    Screeps::Context::update();
    sys::check_creep();
    sys::check_structures();
    sys::spawns_spawn_creep();
    sys::creep();
}

EMSCRIPTEN_BINDINGS(loop) {
    emscripten::function("loop", &loop);
}

EMSCRIPTEN_BINDINGS(updateData) {
    emscripten::function("updateData", &updateData);
}