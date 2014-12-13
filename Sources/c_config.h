#pragma once
#include <string>
#include "e_base.h"

/*overide assert*/
#define RAPIDJSON_ASSERT(x)  if (!(x)) {throw JSONError("Mailformed JSON");} 

#include "cereal/external/rapidjson/document.h"
#include "c_filebuffer.h"




class Config : public rapidjson::Document {
public:
    Config(const std::string& fname);
    Config() : Config("config.json") {};

    ~Config();
    bool IsReady = false;
private:
    FileBuffer *raw;
};
Config::Config(const std::string& fname) {
    raw = new FileBuffer(fname);
    if (!raw->IsReady){
        EMSGS("Can't open config file");
        return;
    }
    //TODO add error check;
    if (Parse<0>( (char *) raw->buffer).HasParseError()) {
        puts((char *) raw->buffer);
        EMSGS("Can't parse config file(JSON mailformed)");
        return;
        }
    IsReady = true;
};
Config::~Config() {
    delete raw;
}