/*
	c++ wrapper for file loaded into memory
*/
#pragma once



#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <vector>
#include    "e_base.h"


#include "string_format.h"


class AbstractBuffer {
public:
    void * buffer;
    int size;

};
class FileBuffer :public AbstractBuffer{
    public:
        FileBuffer(const std::string& srcfile);
        FileBuffer(const FileBuffer&) = delete;
        virtual ~FileBuffer();
        bool IsReady = false;

};
FileBuffer::FileBuffer(const std::string& srcfile) {
    FILE *f_file;
    f_file = fopen(srcfile.c_str(),"rb");
    if (f_file == NULL)
    {
        EMSGS(string_format("%s: File not found! %s",__func__, srcfile.c_str()));
        return;
    }
    fseek(f_file,0,SEEK_END);
    size  = ftell(f_file);
    rewind(f_file);
    buffer = (char *)calloc((size+1),sizeof(char));
    fread((void *)buffer, sizeof(char), size,f_file);
    fclose(f_file);
    IsReady = true;
}
FileBuffer::~FileBuffer() {
    free((void *)buffer);
}

