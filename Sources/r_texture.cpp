#include "r_texture.h"
#include "ErrorCodes.h"
#include "Log.h"
#include "MAssert.h"
#include "ImageBuffer.h"
unsigned int STexture::getGLId() const {
    return tex;
}

unsigned int STexture::resolveGLType(STexture::TextureType t, bool sRGB)
{
    switch (t) {
        case STexture::TEX_RGB:
            return sRGB?GL_SRGB8:GL_RGB8;
        //case STextur::TEX_RGBA:
        //    return sRGB?GL_SRGB8:GL_RGB8;

    }
    MASSERT (true) /* Unknown types should halt*/
}

int STexture::Bind(unsigned int sampler) const {
    if (IsReady) {
        //TODO HANDLE OTHER types
       /* standart
        * glActiveTexture(GL_TEXTURE0+sampler);
        * glBindTexture( GL_TEXTURE_2D,tex);
        */
        /*EXT_direct_state_access*/
        glBindMultiTextureEXT( GL_TEXTURE0 + sampler,GL_TEXTURE_2D,tex);
        return ESUCCESS;
	}
    else {
        LOGE("Texture not ready for bind");
        return EFAIL;
        }

}

int STexture::BindImage(unsigned int unit)
{
    glBindImageTexture(unit, tex, 0,GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);
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
    glTexStorage2D(GL_TEXTURE_2D, num_mipmaps, internalformat, x, y);
    ConfigureTexture(TEX_CLAMP);

    glBindTexture(GL_TEXTURE_2D,0);
    return 0;
}
STexture::STexture(int _x, int _y, TextureType t)
    :type(t) , x(_x) ,y (_y)
 {
    CreateTexture(1,STexture::resolveGLType(type,false));
    IsReady = true;
}
STexture::STexture(const std::string& fname, bool sRGB) 
:d_fname(fname), d_issRGB(sRGB) {
    type = TEX_RGB;
    ImageBuffer img(fname);

    if (!img.IsReady)
    	return;
    x = img.x();
    y = img.y();


    glGenTextures(1, &tex); 
    glBindTexture(GL_TEXTURE_2D,tex);    
    GLsizei num_mipmaps = 4;


    /*gl 4.2 required */
    glTexStorage2D(GL_TEXTURE_2D, num_mipmaps, STexture::resolveGLType(type,sRGB) ,(GLsizei)  x, (GLsizei)y);
    glTexSubImage2D(GL_TEXTURE_2D,0, 0, 0,x,y, GL_RGB, GL_UNSIGNED_BYTE, img.buffer());
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




