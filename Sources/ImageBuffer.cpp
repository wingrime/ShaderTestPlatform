#include <string>
#include "ImageBuffer.h"
#include "stb_image.h"

ImageBuffer::ImageBuffer(const std::string& fname) {
    d_buffer = stbi_load(fname.c_str(), &x, &y, &comp, 3);
    if (d_buffer)
        IsReady = true;
}
ImageBuffer::~ImageBuffer() {
    stbi_image_free(d_buffer);
}
