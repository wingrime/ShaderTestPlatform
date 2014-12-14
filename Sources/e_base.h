/*
	Error class definition
*/
#pragma once

#include    <stdio.h>
	
#include <string>

#include "string_format.h"

#define EMSG(a) {if (BaseError::DefHandler) {BaseError::DefHandler->MessageOut(string_format(a)); } else {printf(a);puts("\n");}}
#define EMSGS(a) {if (BaseError::DefHandler) {BaseError::DefHandler->MessageOut(a);} else {printf(std::string(a).c_str());puts("\n");}}


#define ESUCCESS 0
#define EFAIL  0xBADC00
/*Proxy class for all error definition*/
class AbstractErrorHandler {
public:
	/*need be virtual*/
	void virtual MessageOut(const std::string& msg) = 0;
};

class BaseError {
public: 
    BaseError() : message("UNK") {puts("UNK ERROR \n");}
    BaseError(std::string msg) {
        message = std::string(msg);
        Msg(msg);
    };
    BaseError(const BaseError& orig) {
        message = std::string(orig.message);
        Msg(message);
    };
    std::string message ;
    static AbstractErrorHandler* DefHandler;

    void Msg(const std::string& msg) {
        if (DefHandler == NULL)
            puts(msg.c_str());
        else
            DefHandler->MessageOut(msg);
    }

};
   

AbstractErrorHandler* BaseError::DefHandler = 0;

//TODO inheritance 
#define IOError BaseError
#define ShaderError BaseError
#define ConfigError BaseError
#define UTestError BaseError 
#define ImageLoadError BaseError 
#define FontLoadError BaseError
#define MatrixError BaseError 
#define VectorError BaseError 
#define JSONError BaseError 
