
#include <string.h>
#include "stb_image.h"
#include "c_filebuffer.h"
class ImageBuffer: public AbstractBuffer {
public:
    ImageBuffer(const std::string& fname);
    ImageBuffer(const ImageBuffer&) = delete;
    ~ImageBuffer();

    int x;
    int y;
    int comp; // color components

    bool IsReady = false;
};
ImageBuffer::ImageBuffer(const std::string& fname) {
    buffer = stbi_load(fname.c_str(), &x, &y, &comp, 4); //fix me ARGB only for now
    if (buffer)
    	IsReady = true;
}
ImageBuffer::~ImageBuffer() {
    stbi_image_free(buffer);
}
