#pragma once
#include <string>
#include "AbstractBuffer.h"

class ImageBuffer: public AbstractBuffer {
public:
    ImageBuffer(const std::string& fname);
    ImageBuffer(const ImageBuffer&) = delete;
    ~ImageBuffer();
    int x();
    int y();
    int sBytes();
private:
    int d_comp;
    int d_x;
    int d_y;
public:
    bool IsReady = false;
};

