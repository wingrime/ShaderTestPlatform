#pragma once
enum class GlobalInputState {
	ACTOR_CONTROL, // all input goes to controls 
	DEBUG_CONTROL // all inputs goes to debug menu
};
#include "Log.h"
#include "Singltone.h"
/*overide assert*/
#ifndef RAPIDJSON_ASSERT
#define RAPIDJSON_ASSERT(x)  if (!(x)) {LOGE("JSON Parsing error");}
#endif
#include "cereal/external/rapidjson/document.h"
/* MSVC fix , TODO:ADD to the project settings*/
#undef min
#undef max
class FileBuffer;

class Config : public rapidjson::Document {
public:
    Config(const std::string& fname);
    Config() : Config("config.json") {}

    ~Config();
    bool IsReady = false;
private:
    FileBuffer *raw;
};

