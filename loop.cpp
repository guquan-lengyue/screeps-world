#include <Screeps/Context.hpp>

#include <emscripten.h>
#include <emscripten/bind.h>
#include <iostream>
#include "system/system.h"

EMSCRIPTEN_KEEPALIVE
extern "C" void updateData() {
    std::cout << "loading" << std::endl;
    Screeps::Context::update();
    scrsys::update_spawn();
}

EMSCRIPTEN_KEEPALIVE
extern "C" void loop() {
//    scrsys::source_check();
//    scrsys::creep_check();
//    scrsys::spawn_check_creep();
//    scrsys::spawn_creep();
    scrsys::creep_work();
}

EMSCRIPTEN_BINDINGS(loop) {
    emscripten::function("loop", &loop);
}

EMSCRIPTEN_BINDINGS(updateData) {
    emscripten::function("updateData", &updateData);
}