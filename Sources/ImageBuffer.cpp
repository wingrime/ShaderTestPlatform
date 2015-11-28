#include <string>
#include "ImageBuffer.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

ImageBuffer::ImageBuffer(const std::string& fname) {
    d_buffer = stbi_load(fname.c_str(), &d_x, &d_y, &d_comp, 3);
    if (d_buffer)
        IsReady = true;
}
ImageBuffer::~ImageBuffer() {
    stbi_image_free(d_buffer);
}

int ImageBuffer::x()
{
    return d_x;
}

int ImageBuffer::y()
{
    return d_y;
}

int ImageBuffer::sBytes()
{
    return d_comp;
}
