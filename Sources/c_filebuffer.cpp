/*
	c++ wrapper for file loaded into memory
*/

#include "c_filebuffer.h"

FileBuffer::FileBuffer(const std::string& srcfile) {
    FILE *f_file;
    f_file = fopen(srcfile.c_str(),"rb");
    if (f_file == NULL)
    {
        //EMSGS(string_format("%s: File not found! %s",__func__, srcfile.c_str()));
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

