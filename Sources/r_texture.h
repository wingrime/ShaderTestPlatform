#pragma once
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/wglew.h>


#include "e_base.h"

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

#include "image_buffer.h"

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


class STexture {
public:
    /* from image sRGB is default*/
    
    STexture(const std::string& fname, bool sRGB);
    STexture(const std::string& fname)
    : STexture( fname, true) {};
    /* simple empty texture */
    STexture(int _x, int _y) 
    :STexture(_x,_y,TEX_RGBA)
    {};
    STexture(int _x, int _y, TextureType t);
    STexture(const STexture&) = delete;
     ~STexture();
    int Bind(uint sampler) const;
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
    GLuint tex;
    int CreateTexture(GLsizei num_mipmaps, GLenum internalformat);
    int ConfigureTexture(const BorderType t) const;

  
};
unsigned int STexture::getGLId() const {
    return tex;
}

int STexture::Bind(uint sampler) const {
    if (IsReady) {

        glActiveTexture(GL_TEXTURE0+sampler);
       glBindTexture( GL_TEXTURE_2D,tex);
        return ESUCCESS;
	}
    else {
        printf("unable bind non-exists texture\n");
        return EFAIL;
        }

}
int STexture::ConfigureTexture(const BorderType t) const {
        if (t == TEX_REPEAT) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        } else  { /*TEX_CLAMP */
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        }
        /* interpolation settings */
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//GL_NEAREST
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   return 0;
}
int STexture::CreateTexture(GLsizei num_mipmaps,GLenum internalformat) {
    glGenTextures(1, &tex);

        glBindTexture(GL_TEXTURE_2D,tex);
        glTexStorage2D(GL_TEXTURE_2D, num_mipmaps, internalformat, x, y); /*GL_RGBA8 for LDR*/


    ConfigureTexture(TEX_CLAMP);

    glBindTexture(GL_TEXTURE_2D,0);
    return 0;
}
STexture::STexture(int _x, int _y, TextureType t)
    :type(t) , x(_x) ,y (_y)
 {  if (type == TEX_LDR)
        CreateTexture(1,GL_RGBA8);
    else
        CreateTexture(1,GL_RGBA16F);
    IsReady = true;
}
STexture::STexture(const std::string& fname, bool sRGB) 
:d_fname(fname), d_issRGB(sRGB) {

    ImageBuffer img(fname);

    if (!img.IsReady)
    	return;
    x = img.x;
    y = img.y;


    glGenTextures(1, &tex); 
    glBindTexture(GL_TEXTURE_2D,tex);    
    GLsizei num_mipmaps = 4;


    /*gl 4.2 required */
    glTexStorage2D(GL_TEXTURE_2D, num_mipmaps, sRGB?GL_SRGB8:GL_RGB8,(GLsizei)  x, (GLsizei)y);
    glTexSubImage2D(GL_TEXTURE_2D,0, 0, 0,x,y, GL_RGB, GL_UNSIGNED_BYTE, img.buffer);
    ConfigureTexture(TEX_REPEAT);
    glGenerateMipmap(GL_TEXTURE_2D);


    /*old gl*/
    /*
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA8, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //mipmap here will faill
*/
	IsReady = true;
}
STexture::~STexture() {
    glDeleteTextures(1,&tex);
}




