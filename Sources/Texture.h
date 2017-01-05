#pragma once

#include <string.h>
#include <fstream>
#include "OGL.h"
/*serialization*/
#include <cereal/access.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/string.hpp>
#include <cereal/archives/binary.hpp>
#include "GenericTexture.h"
class STexture :public SGenericTexture{
public:
    /* from image sRGB is default*/
    enum TextureType {
        TEX_RGBA = 0,
        TEX_RGB,
        TEX_R,
    };
    STexture(const std::string& fname, bool sRGB);
    STexture(const std::string& fname)
    : STexture( fname, true) {}
    /* simple empty texture */
    STexture(int _x, int _y)
    :STexture(_x,_y,TEX_RGB)
    {};
    STexture(int _x, int _y, TextureType t);
    STexture(const STexture&) = delete;
     ~STexture();
    int Bind(unsigned int sampler) const;
    int setInterpolationMode(InterpolationType t);
    int BindImage(unsigned int unit);
    bool IsReady = false;



    bool d_issRGB;

    friend class cereal::access;
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar(CEREAL_NVP(x),CEREAL_NVP(y),CEREAL_NVP(IsReady),CEREAL_NVP(d_fname),CEREAL_NVP(type));
    }

    unsigned int static resolveGLType(TextureType t,bool sRGB);
private:
    int CreateTexture(GLsizei num_mipmaps, GLenum internalformat);
    int ConfigureTexture(const BorderType t) const;

    std::string d_fname;
    TextureType type;      /*serialize support */
    int x,y;


};
