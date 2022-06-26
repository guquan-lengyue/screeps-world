#ifndef CREEP_OPTS_H
#define CREEP_OPTS_H
#include <Screeps/JS.hpp>
#include <map>
#include <string>
static JSON moveToOpt()
{

    auto opts = JS::Value::object();
    opts.set("reusePath", 100);
    // opts.set("noPathFinding", true);
    return JS::toJSON(opts);
}
#endif