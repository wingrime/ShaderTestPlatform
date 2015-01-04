#include "r_rbo_texture.h"
#include "ErrorCodes.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/wglew.h>

#include "r_texture.h"
#include "string_format.h"
#include "MAssert.h"
unsigned int SRBOTexture::getGLId() const {
    return tex;
}

bool SRBOTexture::IsMSAA()
{
    return d_isMSAA;
}

SRBOTexture::RTType SRBOTexture::getRelatedDepthType(SRBOTexture::RTType t)
{
    switch (t) {
        case SRBOTexture::RT_SCREEN:
            return RT_SCREEN_DEPTH;
        case SRBOTexture::RT_TEXTURE_ARRAY:
            MASSERT(true); /*NOT IMPL*/
            return RT_NONE;
        case SRBOTexture::RT_TEXTURE_CUBEMAP:
            return RT_TEXTURE_DEPTH_CUBEMAP;
        case SRBOTexture::RT_TEXTURE_MSAA:
            return RT_TEXTURE_DEPTH_MSAA;
        case SRBOTexture::RT_TEXTURE_RED:
            return RT_TEXTURE_DEPTH;
        case SRBOTexture::RT_TEXTURE_FLOAT:
            return RT_TEXTURE_DEPTH;
        case SRBOTexture::RT_TEXTURE_RGBA:
            return RT_TEXTURE_DEPTH;
    }
    MASSERT (true) /* Unknown types should halt*/
    return RT_NONE;
}

bool SRBOTexture::isDepthType(SRBOTexture::RTType t)
{
    if ( t == SRBOTexture::RT_TEXTURE_DEPTH || \
         t == SRBOTexture::RT_SCREEN_DEPTH ||  \
         t == SRBOTexture::RT_TEXTURE_DEPTH_CUBEMAP  || \
         t == SRBOTexture::RT_TEXTURE_DEPTH_MSAA )
        return true;
    return false;
}
/*thats come form settings (runtime ?)*/
unsigned int SRBOTexture::getRelatedGLType(SRBOTexture::RTType t)
{
    switch (t) {
        case SRBOTexture::RT_SCREEN:
            return GL_RGBA8;
        case SRBOTexture::RT_TEXTURE_ARRAY:
            return GL_RGBA8;
        case SRBOTexture::RT_TEXTURE_CUBEMAP:
            return GL_RGBA;
        case SRBOTexture::RT_TEXTURE_MSAA:
            return GL_RGBA16F;
        case SRBOTexture::RT_TEXTURE_RED:
            return GL_R8;
        case SRBOTexture::RT_TEXTURE_FLOAT:
            return GL_RGBA16F;
        case SRBOTexture::RT_TEXTURE_RGBA:
            return GL_RGBA8;
        case SRBOTexture::RT_TEXTURE_DEPTH:
            return GL_DEPTH_COMPONENT32;
        case SRBOTexture::RT_TEXTURE_DEPTH_CUBEMAP:
            return GL_DEPTH_COMPONENT32;
        case SRBOTexture::RT_TEXTURE_DEPTH_MSAA:
            return GL_DEPTH_COMPONENT32;
    }
    MASSERT (true) /* Unknown types should halt*/
}
int SRBOTexture::Bind(unsigned int sampler) const {
    if (IsReady) {

        glActiveTexture(GL_TEXTURE0+sampler);
        if (d_isMSAA) {
            glBindTexture( GL_TEXTURE_2D_MULTISAMPLE,tex);
        } else {
            if (type ==  RT_TEXTURE_CUBEMAP )
                    glBindTexture( GL_TEXTURE_CUBE_MAP,tex);
                else
                    glBindTexture( GL_TEXTURE_2D,tex);
        }
        return ESUCCESS;
	}
    else {
        printf("unable bind non-exists texture\n");
        return EFAIL;
        }

}

int SRBOTexture::BindImage(unsigned int unit)
{
    glBindImageTexture(unit, tex, 0,GL_TRUE, 0, GL_READ_WRITE, SRBOTexture::getRelatedGLType(type));

}
int SRBOTexture::ConfigureTexture(const BorderType t) const {
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
SRBOTexture::SRBOTexture(int _x, int _y, RTType t)
    :type(t) , x(_x) ,y (_y)
 {
    glGenTextures(1, &tex);

    if (t  ==  RT_TEXTURE_DEPTH ||
        t == RT_TEXTURE_FLOAT ||
        t == RT_TEXTURE_RGBA ||
        t == RT_TEXTURE_RED   ) {

        glBindTexture(GL_TEXTURE_2D,tex);
        glTexStorage2D(GL_TEXTURE_2D, 4, SRBOTexture::getRelatedGLType(t), x, y);
        ConfigureTexture(TEX_CLAMP);
        glBindTexture(GL_TEXTURE_2D,0);
        d_isMSAA  = false;
    } else if (t == RT_TEXTURE_MSAA || t  ==  RT_TEXTURE_DEPTH_MSAA) {
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE,tex);
        glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 16, SRBOTexture::getRelatedGLType(t), x, y,true);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE,0);
        d_isMSAA  = true;
    }else if (t == RT_TEXTURE_CUBEMAP ) {
        glBindTexture(GL_TEXTURE_CUBE_MAP , tex);
        /*configure */
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        for (unsigned int face = 0; face < 6; face++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, SRBOTexture::getRelatedGLType(t),
                x, y, 0, GL_RGBA, GL_FLOAT, NULL);
        }

    } else if (t == RT_TEXTURE_DEPTH_CUBEMAP ) {
        glBindTexture(GL_TEXTURE_CUBE_MAP , tex);
        /*configure */
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        for (unsigned int face = 0; face < 6; face++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, SRBOTexture::getRelatedGLType(t),
                x, y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        }

    }else if (t == RT_TEXTURE_ARRAY ) {
        glBindTexture(GL_TEXTURE_2D_ARRAY , tex);
        /*configure */
        glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexStorage3D(GL_TEXTURE_2D_ARRAY, 4, SRBOTexture::getRelatedGLType(t), x, y, 6);

    }


    IsReady = true;
}
SRBOTexture::~SRBOTexture() {
    glDeleteTextures(1,&tex);
}


