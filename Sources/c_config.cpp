#include "c_config.h"
#include "Log.h"
#include "FileBuffer.h"
#include "Error.h"
Config::Config(const std::string& fname) {
    raw = new FileBuffer(fname);
    if (!raw->IsReady){
        LOGE("Can't open config file");
        return;
    }
    //TODO add error check;
    if (Parse<0>( (char *) raw->buffer()).HasParseError()) {
        puts((char *) raw->buffer());
        LOGE("Can't parse config file(JSON mailformed)");
        return;
        }
    IsReady = true;
}

Config::~Config() {
    delete raw;
}
