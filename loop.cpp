#include <Screeps/Context.hpp>

#include <emscripten.h>
#include <emscripten/bind.h>
#include <iostream>
#include "system/system.h"
#include "system/structuresSystem.hpp"

void check() {
    sys::check_creep();
    sys::check_structures();
}


EMSCRIPTEN_KEEPALIVE
extern "C" void updateData() {
    std::cout << "loading" << std::endl;
    check();
}

EMSCRIPTEN_KEEPALIVE
extern "C" void loop() {
    Screeps::Context::update();
    check();
    sys::spawns_spawn_creep();
    sys::creep();
    sys::soldier();
}

EMSCRIPTEN_BINDINGS(loop) {
    emscripten::function("loop", &loop);
}

EMSCRIPTEN_BINDINGS(updateData) {
    emscripten::function("updateData", &updateData);
}