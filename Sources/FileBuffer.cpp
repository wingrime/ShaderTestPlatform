/*
	c++ wrapper for file loaded into memory
*/
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <vector>
#include "string_format.h"
#include "FileBuffer.h"
#include "Log.h"


FileBuffer::FileBuffer(const std::string& srcfile)
    :d_path(srcfile)
{
	std::ifstream file(srcfile, std::ios::binary | std::ios::ate);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	d_buffer = (void *)new char[size+1];
	if (file.read((char *)d_buffer, size))
	{
		IsReady = true;
		d_size = size;
		((char *)d_buffer)[size] = '\0';
	}
	else
		d_buffer = 0;
    
}
FileBuffer::~FileBuffer() {
	if (d_buffer)
		delete[](char *)d_buffer;
}

std::string FileBuffer::getPath()
{
    return d_path;
}

bool CheckFileExists(std::string fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}
