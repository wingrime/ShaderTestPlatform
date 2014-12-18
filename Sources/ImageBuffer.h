#pragma once
#include <string>
#include "AbstractBuffer.h"

class ImageBuffer: public AbstractBuffer {
public:
    ImageBuffer(const std::string& fname);
    ImageBuffer(const ImageBuffer&) = delete;
    ~ImageBuffer();

    int comp;
    int x;
    int y;
    bool IsReady = false;
};

