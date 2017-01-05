#ifndef __SCRIPTING_H__
#define __SCRIPTING_H__
#pragma once
#include <string>
#include "lua.h"
#include "selene.h"
using namespace std;

/*Singletons */
/*
 * Must be moved to Singltone interface as be ready
*/

/* Global configuration*/



namespace scripting {
class Scripting {
public:
    Scripting(string script);
    ~Scripting();
private:
    int initLuaBindings(sel::State& state);
    sel::State state;
};
}

#endif
