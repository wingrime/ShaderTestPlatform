#pragma once

#include "Log.h"
#include "Singltone.h"
/*overide assert*/
#ifndef RAPIDJSON_ASSERT
#define RAPIDJSON_ASSERT(x)  if (!(x)) {LOGE("JSON Parsing error");}
#endif
#include "cereal/external/rapidjson/document.h"

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

class MainConfig :public Singltone<MainConfig>  ,public Config {
public:
    MainConfig() :Config("config.json") , Singltone(this){}
};
