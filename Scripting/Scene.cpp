#include "Scene.h"
//#include "../Scene.h"
#include <string>
using namespace std;
namespace scripting {
Scene::Scene()
{


}

int Scene::loadModel(string model)
{

}

int Scene::initBindings(sel::State &state)
{
    state["Scene"].SetClass<scripting::Scene>("loadModel",&scripting::Scene::loadModel
                                             );
    return 0;
}
}
