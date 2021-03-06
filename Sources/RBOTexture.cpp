#include "RBOTexture.h"
#include "ErrorCodes.h"
#include "OGL.h"

#include "Texture.h"
#include "string_format.h"
#include "MAssert.h"
#include "Log.h"
bool SRBOTexture::IsMSAA()
{
    return d_isMSAA;
}

SRBOTexture::RTType SRBOTexture::getRelatedDepthType(SRBOTexture::RTType t)
{
    switch (t) {
        case SRBOTexture::RT_SCREEN:
            return RT_SCREEN_DEPTH;
        case SRBOTexture::RT_NONE:
            return RT_NONE;
        case SRBOTexture::RT_TEXTURE_DEPTH:
            MASSERT(true);/* nonsence*/
            return RT_TEXTURE_DEPTH;
        case SRBOTexture::RT_SCREEN_DEPTH:
            MASSERT(true);/* nonsence*/
            return RT_SCREEN_DEPTH;
        case SRBOTexture::RT_TEXTURE_DEPTH_MSAA:
        case SRBOTexture::RT_TEXTURE_DEPTH_CUBEMAP:
            MASSERT(true);/* nonsence*/
            return RT_TEXTURE_DEPTH;
        case SRBOTexture::RT_TEXTURE_DEPTH_ARRAY:
            MASSERT(true);/* nonsence*/
            return RT_TEXTURE_DEPTH_ARRAY;

        case SRBOTexture::RT_TEXTURE_CUBEMAP:
            return RT_TEXTURE_DEPTH_CUBEMAP;
        case SRBOTexture::RT_TEXTURE_MSAA:
            return RT_TEXTURE_DEPTH_MSAA;
        case SRBOTexture::RT_TEXTURE_RED:
        case SRBOTexture::RT_TEXTURE_FLOAT:
        case SRBOTexture::RT_TEXTURE_FLOAT_RED:
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
         t == SRBOTexture::RT_TEXTURE_DEPTH_MSAA || \
         t == SRBOTexture::RT_TEXTURE_DEPTH_ARRAY)
        return true;
    return false;
}
/*thats come form settings (runtime ?)*/
unsigned int SRBOTexture::getRelatedGLType(SRBOTexture::RTType t)
{
    switch (t) {
        case SRBOTexture::RT_NONE:
            MASSERT(true);/*you do something wrong*/
            return 0;
        case SRBOTexture::RT_SCREEN:
            return GL_RGBA8;
        case SRBOTexture::RT_TEXTURE_CUBEMAP:
            return GL_RGBA16F;
        case SRBOTexture::RT_TEXTURE_MSAA:
            return GL_RGBA16F;
        case SRBOTexture::RT_TEXTURE_RED:
            return GL_R8;
        case SRBOTexture::RT_TEXTURE_FLOAT:
            return GL_RGBA16F;
        case SRBOTexture::RT_TEXTURE_RGBA:
            return GL_RGBA8;
        case SRBOTexture::RT_TEXTURE_FLOAT_RED:
            return GL_R32F;
        case SRBOTexture::RT_SCREEN_DEPTH: /*??*/
            return GL_DEPTH_COMPONENT32;
        case SRBOTexture::RT_TEXTURE_DEPTH:
            return GL_DEPTH_COMPONENT32;
        case SRBOTexture::RT_TEXTURE_DEPTH_CUBEMAP:
            return GL_DEPTH_COMPONENT32;
        case SRBOTexture::RT_TEXTURE_DEPTH_MSAA:
            return GL_DEPTH_COMPONENT32;
        case SRBOTexture::RT_TEXTURE_DEPTH_ARRAY:
            return GL_DEPTH_COMPONENT32;
    }
    MASSERT (true) /* Unknown types should halt*/
    return EFAIL;
}
int SRBOTexture::Bind(unsigned int sampler) const {
    if (IsReady) {

        glActiveTexture(GL_TEXTURE0+sampler);
        if (d_isMSAA) {
            glBindTexture( GL_TEXTURE_2D_MULTISAMPLE,d_glTexID);
        } else {
            if (type ==  RT_TEXTURE_CUBEMAP )
                    glBindTexture( GL_TEXTURE_CUBE_MAP,d_glTexID);
            else if (type == RT_TEXTURE_DEPTH_ARRAY) {
                    //glBindTexture(GL_TEXTURE_2D,tex);
                    glBindTexture(GL_TEXTURE_2D_ARRAY,d_glTexID);
            }
            else
                    glBindTexture( GL_TEXTURE_2D,d_glTexID);
        }
        return ESUCCESS;
	}
    else {
        LOGE("unable bind non-exists texture\n");
        return EFAIL;
        }

}

int SRBOTexture::BindImage(unsigned int unit)
{
    #ifndef __APPLE__
    glBindImageTexture(unit, d_glTexID, 0,GL_TRUE, 0, GL_READ_WRITE, SRBOTexture::getRelatedGLType(type));
    #else
    //todo gl3 way
    #endif 
    return ESUCCESS;
}

int SRBOTexture::setInterpolationMode(SRBOTexture::InterpolationType t)
{
    /*Bind*/
    if (d_isMSAA) {
        glBindTexture( GL_TEXTURE_2D_MULTISAMPLE,d_glTexID);
    } else {
        if (type ==  RT_TEXTURE_CUBEMAP )
                glBindTexture( GL_TEXTURE_CUBE_MAP,d_glTexID);
        else if (type == RT_TEXTURE_DEPTH_ARRAY) {
                glBindTexture(GL_TEXTURE_2D_ARRAY,d_glTexID);
        }
        else
                glBindTexture( GL_TEXTURE_2D,d_glTexID);
    }

    if (t == InterpolationType::TEX_LINERAL) {

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    } else if (t == InterpolationType::TEX_NEAREST) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }

    return ESUCCESS;

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
SRBOTexture::SRBOTexture(RectSizeInt s, RTType t, unsigned int miplevel)
    :type(t) , d_s(s)
 {
    glGenTextures(1, &d_glTexID);
    if (t  ==  RT_TEXTURE_DEPTH ||
        t == RT_TEXTURE_FLOAT ||
        t == RT_TEXTURE_RGBA ||
        t == RT_TEXTURE_RED  ||
        t == RT_TEXTURE_FLOAT_RED) {
        glBindTexture(GL_TEXTURE_2D,d_glTexID);
        #ifndef __APPLE__
        //gl4
        glTexStorage2D(GL_TEXTURE_2D, miplevel, SRBOTexture::getRelatedGLType(t), s.w, s.h);
        #else
        int width = s.w;
        int height = s.h;
        for (int i = 0; i < miplevel; i++) {

            glTexImage2D(GL_TEXTURE_2D, i,GL_RGB, width, height, 0, GL_RGB, SRBOTexture::getRelatedGLType(t), NULL);
            width = fmax(1, (width / 2));
            height =fmax(1, (height / 2));
        }
        #endif
        ConfigureTexture(TEX_CLAMP);
        /* enable comparea mode for glsl sampler2Shadow*/
        //if (t == RT_TEXTURE_DEPTH) {
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        //}
        glBindTexture(GL_TEXTURE_2D,0);
        d_isMSAA  = false;
    } else if (t == RT_TEXTURE_MSAA || t  ==  RT_TEXTURE_DEPTH_MSAA) {
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE,d_glTexID);
        #ifndef __APPLE__
        glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 16, SRBOTexture::getRelatedGLType(t), s.w, s.h,true);
        #else
        //TODO
        //glTexImage2D!!
        #endif
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE,0);
        d_isMSAA  = true;
    }else if (t == RT_TEXTURE_CUBEMAP ) {
        glBindTexture(GL_TEXTURE_CUBE_MAP , d_glTexID);
        /*configure */
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        for (unsigned int face = 0; face < 6; face++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, SRBOTexture::getRelatedGLType(t),
                s.w, s.h, 0, GL_RGBA, GL_FLOAT, NULL);
        }
        glBindTexture(GL_TEXTURE_CUBE_MAP,0);

    } else if (t == RT_TEXTURE_DEPTH_CUBEMAP ) {
        glBindTexture(GL_TEXTURE_CUBE_MAP , d_glTexID);
        /*configure */
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        for (unsigned int face = 0; face < 6; face++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, SRBOTexture::getRelatedGLType(t),
               s.w, s.h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        }
        glBindTexture(GL_TEXTURE_CUBE_MAP,0);

    }else if (t == RT_TEXTURE_DEPTH_ARRAY ) {
        glBindTexture(GL_TEXTURE_2D_ARRAY , d_glTexID);
        /*configure */
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

       // glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        //glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);


        glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, SRBOTexture::getRelatedGLType(t), s.w, s.h, 4);
        //glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32,x,y, 4, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);


        //for shadowmap

        glBindTexture(GL_TEXTURE_2D_ARRAY,0);
    } else
        MASSERT(true); /* You should forget define new type there*/
    IsReady = true;
}
SRBOTexture::~SRBOTexture() {
    glDeleteTextures(1,&d_glTexID);


}
