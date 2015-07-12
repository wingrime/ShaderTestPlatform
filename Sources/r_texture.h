#pragma once

#include <string.h>
#include <fstream>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/wglew.h>

/*serialization*/
#include <cereal/access.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/string.hpp>
#include <cereal/archives/binary.hpp>

class STexture {
public:
    /* from image sRGB is default*/
    enum TextureType {
        TEX_RGBA = 0,
        TEX_RGB,
        TEX_R,
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
    :STexture(_x,_y,TEX_RGB)
    {};
    STexture(int _x, int _y, TextureType t);
    STexture(const STexture&) = delete;
     ~STexture();
    int Bind(unsigned int sampler) const;

    int BindImage(unsigned int unit);
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

    unsigned int static resolveGLType(TextureType t,bool sRGB);
private:
    unsigned int d_glTexID;
    int CreateTexture(GLsizei num_mipmaps, GLenum internalformat);
    int ConfigureTexture(const BorderType t) const;

  
};




