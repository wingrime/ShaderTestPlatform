#include "RBO.h"
#include "ErrorCodes.h"
#include "MAssert.h"
#include "mat_math.h"
#include "string_format.h"
#include "Log.h"
int RBO::Bind(bool clear) const {
    if (!IsReady)
        return EFAIL;
     /* in case when viewport is not texture target just use 0*/
    glBindFramebuffer(GL_FRAMEBUFFER, d_fbo);

    if (clear)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport ( 0, 0, (GLsizei)d_w, (GLsizei)d_h );
    return ESUCCESS;
}

int RBO::Clean()
{
    if (!IsReady)
        return EFAIL;
     /* in case when viewport is not texture target just use 0*/
    glBindFramebuffer(GL_FRAMEBUFFER, d_fbo);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    return 0;
}

std::string RBO::getName()
{
    return d_name;
}

SGenericTexture* RBO::texIMG(int n)
{
    if (n <0 || n > 3)
        return 0;
    else
        return d_texIMG[n];
}


SGenericTexture* RBO::texDEPTH()
{
    return d_texDEPTH;
}

SVec2 RBO::getSize()
{
    return SVec2(d_w,d_h);
}

int RBO::Resize(SVec2 new_sz)
{
    //TODO: full impl
    d_w = new_sz.w;
    d_h = new_sz.h;
    return ESUCCESS;
}

RBO::RBOType RBO::getType()
{
    return d_type;
}

void RBO::initDepthRenderBuffer(){
    glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, d_w, d_h);
    glBindRenderbuffer(GL_RENDERBUFFER,0);
}

int RBO::attachRBOTextures()
{



    /*Create FBO*/
    glGenFramebuffers(1, &d_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, d_fbo);
    /* attach */
    glFramebufferTexture(GL_FRAMEBUFFER,    GL_DEPTH_ATTACHMENT,   d_texDEPTH->getGLId(), 0);

    int buffers = 0;
    for (int i = 0 ; i < MAX_COLOR_ATTACHMENTS; i++) {
        if (d_texIMG[i]) {
            glFramebufferTexture(GL_FRAMEBUFFER,    GL_COLOR_ATTACHMENT0+i,  d_texIMG[i]->getGLId(), 0);
            buffers++;
        }
    }

    int rcode = glCheckFramebufferStatus(GL_FRAMEBUFFER) ;
    if(rcode != GL_FRAMEBUFFER_COMPLETE) {
        LOGE(string_format("FBO error(!GL_FRAMEBUFFER_COMPLETE) %d\n",rcode));
        MASSERT(true);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return EFAIL;
    }
    GLenum  buffers_attachments  [] = { GL_COLOR_ATTACHMENT0,  GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2};
    glDrawBuffers (buffers, buffers_attachments );

    d_buffers =  buffers;

    if (isDepthOnlyType(d_type))
        glClear( GL_DEPTH_BUFFER_BIT);
    else
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return ESUCCESS;
}

SRBOTexture::RTType RBO::getRelatedRBOTextueTypeFromRBOType(RBO::RBOType t)
{
    switch (t) {
    case RBOType::RBO_CUBEMAP:
        return SRBOTexture::RTType::RT_TEXTURE_CUBEMAP;
    case RBOType::RBO_FLOAT:
        return SRBOTexture::RTType::RT_TEXTURE_FLOAT;
    case RBOType::RBO_MIXED:
        return SRBOTexture::RTType::RT_NONE;
    case RBOType::RBO_NONE:
        return SRBOTexture::RTType::RT_NONE;
    case RBOType::RBO_RED:
        return SRBOTexture::RTType::RT_TEXTURE_RED;
    case RBOType::RBO_RGBA:
        return SRBOTexture::RTType::RT_TEXTURE_RGBA;
    case RBOType::RBO_SCREEN:
        return SRBOTexture::RTType::RT_NONE;
    case RBOType::RBO_MSAA:
        return SRBOTexture::RTType::RT_TEXTURE_MSAA;
    case RBOType::RBO_FLOAT_RED:
        return SRBOTexture::RTType::RT_TEXTURE_FLOAT_RED;
    case RBOType::RBO_DEPTH_ONLY:
        return SRBOTexture::RTType::RT_NONE;
    case RBOType::RBO_DEPTH_ARRAY_ONLY:
        return SRBOTexture::RTType::RT_NONE;
    }
    MASSERT(true); /*Unknown types should halt*/
    return SRBOTexture::RTType::RT_NONE;
}

SRBOTexture::RTType RBO::getRelatedDepthRBOTextueTypeFromRBOType(RBO::RBOType t)
{
    switch (t) {
    case RBOType::RBO_CUBEMAP:
        return SRBOTexture::RTType::RT_TEXTURE_DEPTH_CUBEMAP;
    case RBOType::RBO_FLOAT:
        return SRBOTexture::RTType::RT_TEXTURE_DEPTH;
    case RBOType::RBO_MIXED:
        return SRBOTexture::RTType::RT_TEXTURE_DEPTH;/*default ?*/
    case RBOType::RBO_RED:
        return SRBOTexture::RTType::RT_TEXTURE_DEPTH;
    case RBOType::RBO_RGBA:
        return SRBOTexture::RTType::RT_TEXTURE_DEPTH;
    case RBOType::RBO_SCREEN:
        return SRBOTexture::RTType::RT_SCREEN_DEPTH;
    case RBOType::RBO_MSAA:
        return SRBOTexture::RTType::RT_TEXTURE_DEPTH_MSAA;
    case RBOType::RBO_FLOAT_RED:
        return SRBOTexture::RTType::RT_TEXTURE_DEPTH;
    case RBOType::RBO_DEPTH_ONLY:
        return SRBOTexture::RTType::RT_TEXTURE_DEPTH;
    case RBOType::RBO_DEPTH_ARRAY_ONLY:
        return SRBOTexture::RTType::RT_TEXTURE_DEPTH_ARRAY;
    case RBOType::RBO_NONE:
        return SRBOTexture::RTType::RT_NONE;
    }
    MASSERT(true); /*Unknown types should halt*/
    return SRBOTexture::RTType::RT_NONE;
}

bool RBO::isDepthOnlyType(RBO::RBOType t)
{
    return (t == RBO::RBO_DEPTH_ARRAY_ONLY || t == RBO::RBO_DEPTH_ONLY);
}

std::vector<RBO* >  RBO::debugGetRenderOutputList()
{
    return RBO::debugRenderOutputList;
}

int RBO::debugRegisterSelf()
{
    RBO::debugRenderOutputList.push_back(this);
    return 0;
}
/* Constructor from ptr's*/
RBO::RBO(std::string name, int w, int h, RBOType _type,
        SRBOTexture * _texIMG,
        SRBOTexture * _texIMG1,
        SRBOTexture * _texIMG2,
        SRBOTexture * _texDEPTH)
:d_name(name), d_type(_type),d_w(w),d_h(h) ,d_texDEPTH(_texDEPTH)
 {
    for(int i = 0 ; i < MAX_COLOR_ATTACHMENTS;i++)
        d_texIMG[i] = 0;
    d_texDEPTH = 0;
    d_texIMG[0] = _texIMG;
    d_texIMG[1] = _texIMG1;
    d_texIMG[2] = _texIMG2;

    unsigned int mip = 1;

    if (d_w > 128 && d_h > 128 )
        mip = 4;
    debugRegisterSelf();
    if (d_type == RBO_SCREEN) {
        IsReady = true;
        return;
    }

    if (!d_texIMG[0]) {
            d_texIMG[0] = (new SRBOTexture(RectSizeInt(d_w,d_h),RBO::getRelatedRBOTextueTypeFromRBOType(d_type),mip ));

    }
    if (!d_texDEPTH) {
            d_texDEPTH = (new SRBOTexture(RectSizeInt(d_w,d_h),RBO::getRelatedDepthRBOTextueTypeFromRBOType(d_type),mip ));
    }

    d_isMSAA = d_texIMG[0]->IsMSAA() ;


    if (attachRBOTextures() == ESUCCESS)
        IsReady = true;
}
/*single buffer*/
RBO::RBO(std::string name ,int def_w,int def_h, RBO::RBOType type)
    :d_name(name),d_type(type),d_w(def_w),d_h(def_h)
{
    for (int i = 0 ; i < MAX_COLOR_ATTACHMENTS; i++) {
        d_texIMG[i] = 0;
     }
    d_texDEPTH = 0;
    debugRegisterSelf();
    /*automatic mip level detection TODO*/
    unsigned int mip = 1;
    if (d_w > 256 && d_h > 256 )
        mip = 4;
    if (type == RBO_SCREEN) {
        IsReady = true;
        return;
    }

    if (type == RBO_CUBEMAP){

        MASSERT(d_w != d_h);/*cubemap are should be cube*/
        MASSERT( d_w % 2 != 0); /*should be power of two*/
    }
    if (!isDepthOnlyType(type)) {
        d_texIMG[0] = (new SRBOTexture(RectSizeInt(d_w,d_h),RBO::getRelatedRBOTextueTypeFromRBOType(type),mip ));
    }
    d_texDEPTH = (new SRBOTexture(RectSizeInt(d_w,d_h),RBO::getRelatedDepthRBOTextueTypeFromRBOType(type),mip));
    if (!isDepthOnlyType(type)) {
        d_isMSAA = d_texIMG[0]->IsMSAA() ;
    }
    else
        d_isMSAA= false;
    if (attachRBOTextures() == ESUCCESS)
        IsReady = true;
}

RBO::~RBO()
{
    glDeleteFramebuffers(1,&d_fbo);
}
/* new constructor*/
RBO::RBO(std::string name, int def_w, int def_h, RBO::RBOType type, SRBOTexture::RTType t0_type, int t0_s, SRBOTexture::RTType t1_type, int t1_s, SRBOTexture::RTType t2_type, int t2_s)
        :d_name(name),d_type(type),d_w(def_w),d_h(def_h)
{
    MASSERT(t0_type == SRBOTexture::RTType::RT_NONE);
    MASSERT(SRBOTexture::isDepthType(t0_type));
    MASSERT(SRBOTexture::isDepthType(t1_type));
    MASSERT(SRBOTexture::isDepthType(t2_type));
    MASSERT(t0_s <= 0);

    for(int i = 0 ; i < MAX_COLOR_ATTACHMENTS;i++)
        d_texIMG[i] = 0;
    d_texDEPTH = 0;

    SRBOTexture::RTType tex_type[MAX_COLOR_ATTACHMENTS];
    int tex_size_mul[MAX_COLOR_ATTACHMENTS];

    tex_type[0] = t0_type;
    tex_type[1] = t1_type;
    tex_type[2] = t2_type;
    tex_size_mul[0] = t0_s;
    tex_size_mul[1] = t1_s;
    tex_size_mul[2] = t2_s;

    debugRegisterSelf();
    //TODO: add type check
    unsigned int mip = 1;
    if (d_w > 128 && d_h > 128 )
        mip = 2;

    if (!isDepthOnlyType(type))
        d_texIMG[0] = (new SRBOTexture(RectSizeInt(d_w/t0_s,d_h/t0_s),t0_type,mip));
    /*depth*/
    d_texDEPTH = (new SRBOTexture(RectSizeInt(d_w/t0_s,d_h/t0_s), SRBOTexture::getRelatedDepthType(t0_type) ,mip));

    for (int i = 1 ; i < MAX_COLOR_ATTACHMENTS; i++) {
        if (tex_type[i] != SRBOTexture::RTType::RT_NONE) {
            MASSERT(tex_size_mul[i] <= 0);
            d_texIMG[i] = (new SRBOTexture(RectSizeInt(d_w/tex_size_mul[i],d_h/tex_size_mul[i]),tex_type[i],mip));
        }
    }

    if (!isDepthOnlyType(type))
        d_isMSAA = d_texIMG[0]->IsMSAA() ; /*TODO: make external*/
    else
        d_isMSAA = false;

    if (attachRBOTextures() == ESUCCESS)
        IsReady = true;

}

int RBO::ResolveMSAA(const RBO &dst)
{
      //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst.d_fbo);   // Make sure no FBO is set as the draw framebuffer
      glBindFramebuffer(GL_READ_FRAMEBUFFER, d_fbo); // Make sure your multisampled FBO is the read framebuffer
      //glDrawBuffer(GL_BACK);                       // Set the back buffer as the draw buffer

      for (int i = 0 ; i < d_buffers; i++) {
        GLenum  buffers_att [] = { GL_COLOR_ATTACHMENT0+(unsigned int)i};
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glDrawBuffers ( 1, buffers_att );
        glBlitFramebuffer(0, 0, d_w, d_h, 0, 0, dst.d_w, dst.d_h, (i == 0) ? (GL_COLOR_BUFFER_BIT |  GL_DEPTH_BUFFER_BIT) : (GL_COLOR_BUFFER_BIT), GL_NEAREST);
      }

      //Reset procedure
      glReadBuffer(GL_COLOR_ATTACHMENT0);
      GLenum  buffers_attachments [] = { GL_COLOR_ATTACHMENT0,  GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2};

      glDrawBuffers (d_buffers, buffers_attachments );
      return 0;
}
/*shared list for debug */
std::vector<  RBO*  > RBO::debugRenderOutputList = std::vector< RBO* >();
