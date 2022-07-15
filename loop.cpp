#include <Screeps/Context.hpp>

#include <emscripten.h>
#include <emscripten/bind.h>
#include <iostream>
#include "system/system.h"

void check() {
    scrsys::update_spawn();
    scrsys::spawn_check_creep();
    scrsys::source_check();
    scrsys::creep_check();
}

EMSCRIPTEN_KEEPALIVE
extern "C" void updateData() {
    std::cout << "loading" << std::endl;
    check();
}

EMSCRIPTEN_KEEPALIVE
extern "C" void loop() {
    Screeps::Context::update();
    std::cout << Screeps::Game.time() << std::endl;
    if (Screeps::Game.time() % 10 == 0) {
        check();
    }
    std::cout << "loop" << std::endl;
    scrsys::spawn_creep();
    scrsys::creep_work();
}

EMSCRIPTEN_BINDINGS(loop) {
    emscripten::function("loop", &loop);
}

EMSCRIPTEN_BINDINGS(updateData) {
    emscripten::function("updateData", &updateData);
}