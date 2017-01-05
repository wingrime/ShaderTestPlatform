#include "Scriting.h"

#include "selene.h"
#include "Log.h"
#include "Scene.h"
namespace scripting  {
Scripting::Scripting(string script)
{
    /*init scripting subsystem*/
    if (state.Load(script)) {
        state["init"]();
    } else
        LOGE("No init.lua script find or load failed!");
    state.OpenLib("math",luaopen_math);
    state.OpenLib("table",luaopen_table);
    initLuaBindings(state);

}

int Scripting::initLuaBindings(sel::State& state)
{

    struct Env {
        Env() {}
        int loge(std::string msg) {LOGE(msg);return 0;}
        int logw(std::string msg) {LOGW(msg);return 0;}
        int logv(std::string msg) {LOGV(msg);return 0;}
    };
    state["Env"].SetClass<Env>("loge",&Env::loge,
                               "logw",&Env::logw,
                               "logv",&Env::logv
                               );

    scripting::Scene::initBindings(state);
    return 0;
}
}
