#ifndef GENERIG_TEXTURE_H
#define GENERIG_TEXTURE_H
#pragma once

class SGenericTexture {
public:
    enum BorderType {
        TEX_CLAMP = 0,
        TEX_REPEAT
    };
    enum InterpolationType  {
        TEX_NEAREST = 0,
        TEX_LINERAL
    };
    virtual int Bind(unsigned int sampler) const = 0;
    virtual int setInterpolationMode(InterpolationType t) = 0;
    unsigned int inline getGLId() const {
        return d_glTexID;
    }
protected:
    unsigned int d_glTexID;
};



#endif
