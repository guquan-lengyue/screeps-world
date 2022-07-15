#include <Screeps/Context.hpp>

#include <emscripten.h>
#include <emscripten/bind.h>
#include <iostream>
#include "system/system.h"

EMSCRIPTEN_KEEPALIVE
extern "C" void updateData() {
    std::cout << "loading" << std::endl;
    Screeps::Context::update();
}

EMSCRIPTEN_KEEPALIVE
extern "C" void loop() {
    std::cout << "loop" << std::endl;
    scrsys::update_spawn();
    scrsys::spawn_check_creep();
    scrsys::spawn_creep();
//    scrsys::source_check();
//    scrsys::creep_check();
//    scrsys::spawn_creep();
//
//    scrsys::creep_work();
}

EMSCRIPTEN_BINDINGS(loop) {
    emscripten::function("loop", &loop);
}

EMSCRIPTEN_BINDINGS(updateData) {
    emscripten::function("updateData", &updateData);
}