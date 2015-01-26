/*
	c++ wrapper for file loaded into memory
*/
#pragma once

#include "AbstractBuffer.h"

class FileBuffer :public AbstractBuffer{
    public:
        FileBuffer(const std::string& srcfile);
        FileBuffer(const FileBuffer&) = delete;
        virtual ~FileBuffer();
        bool IsReady = false;

        std::string getPath();
   private:
        std::string d_path;

};

