#include "RBO.h"
#include "ErrorCodes.h"
#include "MAssert.h"
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

std::shared_ptr<SRBOTexture> RBO::texIMG()
{
    return d_texIMG;
}

std::shared_ptr<SRBOTexture> RBO::texIMG1()
{
    return d_texIMG1;
}

std::shared_ptr<SRBOTexture> RBO::texIMG2()
{
    return d_texIMG2;
}

std::shared_ptr<SRBOTexture> RBO::texDEPTH()
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


    int buffers = 1;
    /*Create FBO*/
    glGenFramebuffers(1, &d_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, d_fbo);
    /* attach */
    glFramebufferTexture(GL_FRAMEBUFFER,    GL_DEPTH_ATTACHMENT,   d_texDEPTH->getGLId(), 0);
    glFramebufferTexture(GL_FRAMEBUFFER,    GL_COLOR_ATTACHMENT0,  d_texIMG->getGLId(), 0);

    if (d_texIMG1 != nullptr) {
        glFramebufferTexture(GL_FRAMEBUFFER,    GL_COLOR_ATTACHMENT1,   d_texIMG1->getGLId(), 0);
        buffers++;
    }

    if (d_texIMG2 != nullptr) {
        glFramebufferTexture(GL_FRAMEBUFFER,    GL_COLOR_ATTACHMENT2,    d_texIMG2->getGLId(), 0);
        buffers++;
    }
    GLenum  buffers1 [] = { GL_COLOR_ATTACHMENT0 };
    GLenum  buffers2 [] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1 };
    GLenum  buffers3 [] = { GL_COLOR_ATTACHMENT0,  GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2};
    switch (buffers)
    {

    case 1:
            glDrawBuffers (1, buffers1 );
        break;
    case 2:
            glDrawBuffers (2, buffers2 );
        break;
    case 3:
            glDrawBuffers (3, buffers3 );
        break;
    }

    d_buffers =  buffers;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    int rcode = glCheckFramebufferStatus(GL_FRAMEBUFFER) ;
    if(rcode != GL_FRAMEBUFFER_COMPLETE) {
        //EMSGS(string_format("FBO incomplite error %d\n",rcode));
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return EFAIL;
    }
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
    }
    MASSERT(true); /*Unknown types should halt*/
    return SRBOTexture::RTType::RT_NONE;
}
/* Constructor from ptr's*/
RBO::RBO(int _w, int _h,RBOType _type,
        std::shared_ptr<SRBOTexture> _texIMG,
        std::shared_ptr<SRBOTexture> _texIMG1,
        std::shared_ptr<SRBOTexture> _texIMG2,
        std::shared_ptr<SRBOTexture> _texDEPTH)
:d_w(_w),d_h(_h), d_type(_type) ,d_texIMG(_texIMG),d_texIMG1(_texIMG1),d_texIMG2(_texIMG2),d_texDEPTH(_texDEPTH)
 {
    unsigned int mip = 1;

    if (d_w > 128, d_h > 128 )
        mip = 4;

    if (d_type == RBO_SCREEN) {
        IsReady = true;
        return;
    }

    if (d_texIMG == nullptr) {
            d_texIMG.reset(new SRBOTexture(d_w,d_h,RBO::getRelatedRBOTextueTypeFromRBOType(d_type),mip ));

    }
    if (d_texDEPTH == nullptr) {
            d_texDEPTH.reset(new SRBOTexture(d_w,d_h,RBO::getRelatedDepthRBOTextueTypeFromRBOType(d_type),mip ));
    }

    d_isMSAA = d_texIMG->IsMSAA() ;


    if (attachRBOTextures() == ESUCCESS)
        IsReady = true;
}
/*single buffer*/
RBO::RBO(int def_w, int def_h, RBO::RBOType type)
{

    d_w = def_w;
    d_h = def_h;
    d_type = type;
    /*automatic mip level detection TODO*/
    unsigned int mip = 1;
    if (d_w > 256, d_h > 256 )
        mip = 4;
    if (type == RBO_SCREEN) {
        IsReady = true;
        return;
    }

    if (type == RBO_CUBEMAP){

        MASSERT(d_w != d_h);/*cubemap are should be cube*/
        MASSERT( d_w % 2 != 0); /*should be power of two*/
    }

    d_texIMG.reset(new SRBOTexture(d_w,d_h,RBO::getRelatedRBOTextueTypeFromRBOType(type),mip ));
    d_texDEPTH.reset(new SRBOTexture(d_w,d_h,RBO::getRelatedDepthRBOTextueTypeFromRBOType(type),mip));
    d_isMSAA = d_texIMG->IsMSAA() ; /* ??? */
    if (attachRBOTextures() == ESUCCESS)
        IsReady = true;
}

RBO::~RBO()
{
    glDeleteFramebuffers(1,&d_fbo);
}
/* new constructor*/
RBO::RBO(int def_w, int def_h, RBO::RBOType type, SRBOTexture::RTType t0_type, int t0_s, SRBOTexture::RTType t1_type, int t1_s, SRBOTexture::RTType t2_type, int t2_s)
{
    d_w = def_w;
    d_h = def_h;
    d_type = type;
    //TODO: add type check
    unsigned int mip = 1;
    if (d_w > 128, d_h > 128 )
        mip = 2;
    MASSERT(t0_type == SRBOTexture::RTType::RT_NONE);
    MASSERT(SRBOTexture::isDepthType(t0_type));
    MASSERT(SRBOTexture::isDepthType(t1_type));
    MASSERT(SRBOTexture::isDepthType(t2_type));
    MASSERT(t0_s <= 0);

    d_texIMG.reset(new SRBOTexture(d_w/t0_s,d_h/t0_s,t0_type,mip));
    /*depth*/
    d_texDEPTH.reset(new SRBOTexture(d_w/t0_s,d_h/t0_s, SRBOTexture::getRelatedDepthType(t0_type) ,mip));


    if (t1_type != SRBOTexture::RTType::RT_NONE) {
        MASSERT(t1_s <= 0);
        d_texIMG1.reset(new SRBOTexture(d_w/t1_s,d_h/t1_s,t1_type,mip));
    }
    if (t2_type != SRBOTexture::RTType::RT_NONE) {
        MASSERT(t2_s <= 0);
        d_texIMG2.reset(new SRBOTexture(d_w/t2_s,d_h/t2_s,t2_type,mip));
    }
    d_isMSAA = d_texIMG->IsMSAA() ;

    if (attachRBOTextures() == ESUCCESS)
        IsReady = true;

}

int RBO::ResolveMSAA(const RBO &dst)
{
      //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst.d_fbo);   // Make sure no FBO is set as the draw framebuffer
      glBindFramebuffer(GL_READ_FRAMEBUFFER, d_fbo); // Make sure your multisampled FBO is the read framebuffer
      //glDrawBuffer(GL_BACK);                       // Set the back buffer as the draw buffer
      GLenum  buffers1 [] = { GL_COLOR_ATTACHMENT0};
      GLenum  buffers2 [] = { GL_COLOR_ATTACHMENT1};
      GLenum  buffers3 [] = { GL_COLOR_ATTACHMENT2};
      if (d_buffers >= 1) {
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glDrawBuffers ( 1, buffers1 );
        glBlitFramebuffer(0, 0, d_w, d_h, 0, 0, dst.d_w, dst.d_h, GL_COLOR_BUFFER_BIT |  GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        if (d_buffers >= 2) {
            glReadBuffer(GL_COLOR_ATTACHMENT1);
            glDrawBuffers ( 1, buffers2 );
            glBlitFramebuffer(0, 0, d_w, d_h, 0, 0, dst.d_w, dst.d_h, GL_COLOR_BUFFER_BIT , GL_NEAREST);
            if (d_buffers >= 3) {
                glReadBuffer(GL_COLOR_ATTACHMENT2);
                glDrawBuffers ( 1, buffers3 );
                glBlitFramebuffer(0, 0, d_w, d_h, 0, 0, dst.d_w, dst.d_h, GL_COLOR_BUFFER_BIT , GL_NEAREST);
            }
        }
      }
      //Reset procedure
      glReadBuffer(GL_COLOR_ATTACHMENT0);
      GLenum  buffers_1 [] = { GL_COLOR_ATTACHMENT0 };
      GLenum  buffers_2 [] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1 };
      GLenum  buffers_3 [] = { GL_COLOR_ATTACHMENT0,  GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2};
      switch (d_buffers)
      {

      case 1:
              glDrawBuffers (1, buffers_1 );
          break;
      case 2:
              glDrawBuffers (2, buffers_2 );
          break;
      case 3:
              glDrawBuffers (3, buffers_3 );
          break;
      }
      return 0;
}
