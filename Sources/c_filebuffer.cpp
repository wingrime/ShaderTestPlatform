/*
	c++ wrapper for file loaded into memory
*/
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <vector>
#include "string_format.h"
#include "c_filebuffer.h"
#include "Log.h"

FileBuffer::FileBuffer(const std::string& srcfile)
    :d_path(srcfile)
{
    FILE *f_file;
    f_file = fopen(srcfile.c_str(),"rb");
    if (f_file == NULL)
    {
        LOGE(string_format("%s: File not found! %s",__func__, srcfile.c_str()));
        return;
    }
    fseek(f_file,0,SEEK_END);
    d_size  = ftell(f_file);
    rewind(f_file);
    d_buffer = (char *)calloc((d_size+1),sizeof(char));
    fread((void *)d_buffer, sizeof(char), d_size,f_file);
    fclose(f_file);
    IsReady = true;
}
FileBuffer::~FileBuffer() {
    free((void *)d_buffer);
}

std::string FileBuffer::getPath()
{
    return d_path;
}

