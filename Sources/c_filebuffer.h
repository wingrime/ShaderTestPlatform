/*
	c++ wrapper for file loaded into memory
*/
#pragma once



#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <vector>


#include "string_format.h"
#include "AbstractBuffer.h"


class FileBuffer :public AbstractBuffer{
    public:
        FileBuffer(const std::string& srcfile);
        FileBuffer(const FileBuffer&) = delete;
        virtual ~FileBuffer();
        bool IsReady = false;

};

