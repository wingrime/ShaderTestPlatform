#ifndef __SCRIPTING_SCENE_H__
#define __SCRIPTING_SCENE_H__
#pragma once
#include <string>
#include "selene.h"
#include "Scene.h"
using namespace std;
namespace scripting {
class Scene {
public:
    Scene();
    int loadModel(string model);
    static int initBindings(sel::State& state);
};
}
#endif
