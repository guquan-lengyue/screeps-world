#include <Screeps/Context.hpp>

#include <emscripten.h>
#include <emscripten/bind.h>
#include <iostream>
#include "system/system.h"


EMSCRIPTEN_KEEPALIVE
extern "C" void updateData() {
    std::cout << "loading" << std::endl;
}

EMSCRIPTEN_KEEPALIVE
extern "C" void loop() {
    Screeps::Context::update();
    std::cout << "loop" << std::endl;
    sys::check_creep();
    sys::spawns_spawn_creep();
    sys::creep();
}

EMSCRIPTEN_BINDINGS(loop) {
    emscripten::function("loop", &loop);
}

EMSCRIPTEN_BINDINGS(updateData) {
    emscripten::function("updateData", &updateData);
}