#pragma once
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/wglew.h>




#include "string_format.h"

#include <string.h>

/*serialization*/
#include <cereal/access.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/string.hpp>
#include <cereal/archives/binary.hpp>
#include <fstream>

#include "ImageBuffer.h"
#include "e_base.h"




class STexture {
public:
    /* from image sRGB is default*/
    enum TextureType {
        TEX_RGBA = 0,
        TEX_LDR,
        TEX_DEPTH,
        TEX_CUBIEMAP
    };
    enum BorderType {
        TEX_CLAMP = 0,
        TEX_REPEAT
    };
    STexture(const std::string& fname, bool sRGB);
    STexture(const std::string& fname)
    : STexture( fname, true) {}
    /* simple empty texture */
    STexture(int _x, int _y) 
    :STexture(_x,_y,TEX_RGBA)
    {};
    STexture(int _x, int _y, TextureType t);
    STexture(const STexture&) = delete;
     ~STexture();
    int Bind(unsigned int sampler) const;
    unsigned int getGLId() const;
    int x,y;

    std::string d_fname;

    bool IsReady = false;
    bool d_issRGB;
    TextureType type;      /*serialize support */
    friend class cereal::access;
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar(CEREAL_NVP(x),CEREAL_NVP(y),CEREAL_NVP(IsReady),CEREAL_NVP(d_fname),CEREAL_NVP(type));
    }
private:
    unsigned int tex;
    int CreateTexture(GLsizei num_mipmaps, GLenum internalformat);
    int ConfigureTexture(const BorderType t) const;

  
};




