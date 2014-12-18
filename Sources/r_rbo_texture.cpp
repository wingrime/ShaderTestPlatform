#include "r_rbo_texture.h"
#include "ErrorCodes.h"
unsigned int SRBOTexture::getGLId() const {
    return tex;
}

bool SRBOTexture::IsMSAA()
{
    return d_isMSAA;
}
int SRBOTexture::Bind(unsigned int sampler) const {
    if (IsReady) {

        glActiveTexture(GL_TEXTURE0+sampler);
        if (d_isMSAA) {
            glBindTexture( GL_TEXTURE_2D_MULTISAMPLE,tex);
        } else {
            glBindTexture( GL_TEXTURE_2D,tex);
        }
        return ESUCCESS;
	}
    else {
        printf("unable bind non-exists texture\n");
        return EFAIL;
        }

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

    if (t  ==  RT_TEXTURE_DEPTH) {

        glBindTexture(GL_TEXTURE_2D,tex);
        glTexStorage2D(GL_TEXTURE_2D, 4, GL_DEPTH_COMPONENT32, x, y);
        ConfigureTexture(TEX_CLAMP); // ???
        glBindTexture(GL_TEXTURE_2D,0);
        d_isMSAA  = false;

    } else if (t == RT_TEXTURE_FLOAT) {
        glBindTexture(GL_TEXTURE_2D,tex);
        glTexStorage2D(GL_TEXTURE_2D, 4, GL_RGBA16F, x, y);
        ConfigureTexture(TEX_CLAMP);
        glBindTexture(GL_TEXTURE_2D,0);
        d_isMSAA  = false;

    } else if (t == RT_TEXTURE_RGBA) {
        glBindTexture(GL_TEXTURE_2D,tex);
        glTexStorage2D(GL_TEXTURE_2D, 4, GL_RGBA8, x, y);
        ConfigureTexture(TEX_CLAMP);
        glBindTexture(GL_TEXTURE_2D,0);

        d_isMSAA  = false;

    } else if (t == RT_TEXTURE_MSAA) {
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE,tex);
        glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 16, GL_RGBA16F, x, y,true);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE,0);
        d_isMSAA  = true;

    }else if  (t  ==  RT_TEXTURE_DEPTH_MSAA) {

        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE,tex);
        glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 16, GL_DEPTH_COMPONENT32, x, y,true);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE,0);
        d_isMSAA  = true;

    }
    IsReady = true;
}
SRBOTexture::~SRBOTexture() {
    glDeleteTextures(1,&tex);
}


