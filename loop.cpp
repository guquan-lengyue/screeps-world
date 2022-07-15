#include <Screeps/Context.hpp>
#include <Screeps/Creep.hpp>

#include <emscripten.h>
#include <emscripten/bind.h>
#include <iostream>

EMSCRIPTEN_KEEPALIVE
extern "C"

void initData() {
    std::cout << "initData ??" << std::endl;
    std::cout << "hello world ??    " << std::endl;
}


EMSCRIPTEN_KEEPALIVE
extern "C"

void loop() {
    Screeps::Context::update();
}

EMSCRIPTEN_BINDINGS(loop)
{
    emscripten::function("loop", &loop);
    emscripten::function("initData", &initData);
}
