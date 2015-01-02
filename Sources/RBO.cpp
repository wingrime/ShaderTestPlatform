#include "RBO.h"
#include "ErrorCodes.h"
int RBO::Bind(bool clear) const {
    if (!IsReady)
        return EFAIL;
        /* in case when viewport is not texture target just use 0*/
    //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, d_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, d_fbo);

    if (clear)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport ( 0, 0, (GLsizei)w, (GLsizei)h );
   // glDrawBuffer(GL_BACK);                       // Set the back buffer as the draw buffer
    // glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    return ESUCCESS;
}

void RBO::initDepthRenderBuffer(){
    glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w, h);
    glBindRenderbuffer(GL_RENDERBUFFER,0);
}
/* Constructor from ptr's*/
RBO::RBO(int _w, int _h,SRBOTexture::RTType _type,
        std::shared_ptr<SRBOTexture> _texIMG,
        std::shared_ptr<SRBOTexture> _texIMG1,
        std::shared_ptr<SRBOTexture> _texIMG2,
        std::shared_ptr<SRBOTexture> _texDEPTH)
:w(_w),h(_h), type(_type) ,texIMG(_texIMG),texIMG1(_texIMG1),texIMG2(_texIMG2),texDEPTH(_texDEPTH)
 {
    if (type == SRBOTexture::RT_SCREEN) {
        IsReady = true;
        return;
    }

    /*Create FBO*/
    glGenFramebuffers(1, &d_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, d_fbo);

    if (texIMG == nullptr) {
        if (type == SRBOTexture::RT_TEXTURE_FLOAT)
            texIMG.reset(new SRBOTexture(w,h,SRBOTexture::RT_TEXTURE_FLOAT));
        else  if (type == SRBOTexture::RT_TEXTURE_CUBEMAP) {
            printf("init cubemap tex!\n");
            texIMG.reset(new SRBOTexture(w,h,SRBOTexture::RT_TEXTURE_CUBEMAP));
        }
        else /* RT_TEXTURE_RGBA */
            texIMG.reset(new SRBOTexture(w,h,SRBOTexture::RT_TEXTURE_RGBA));

    }
    if (texDEPTH == nullptr) {
         if (type == SRBOTexture::RT_TEXTURE_CUBEMAP)
                texDEPTH.reset(new SRBOTexture(w,h, SRBOTexture::RT_TEXTURE_DEPTH_CUBEMAP));
            else
                texDEPTH.reset(new SRBOTexture(w,h, SRBOTexture::RT_TEXTURE_DEPTH));
    } else {
    //	if (texDEPTH->type != RT_TEXTURE_DEPTH || ) {
    //		EMSGS("RBO: try attatch non depth texture to depth!");
    //		return;
    //	} FIX WITH MSAA

    }
    d_isMSAA = texIMG->IsMSAA() ;
    /* attach */
    //glFramebufferTexture2D(GL_FRAMEBUFFER,    GL_DEPTH_ATTACHMENT,  (texDEPTH->IsMSAA()) ?GL_TEXTURE_2D_MULTISAMPLE:GL_TEXTURE_2D,  texDEPTH->getGLId(), 0);
    //glFramebufferTexture2D(GL_FRAMEBUFFER,    GL_COLOR_ATTACHMENT0,  (texIMG->IsMSAA()) ?GL_TEXTURE_2D_MULTISAMPLE:GL_TEXTURE_2D,  texIMG->getGLId(), 0);
    glFramebufferTexture(GL_FRAMEBUFFER,    GL_DEPTH_ATTACHMENT,   texDEPTH->getGLId(), 0);
    glFramebufferTexture(GL_FRAMEBUFFER,    GL_COLOR_ATTACHMENT0,  texIMG->getGLId(), 0);

    if (texIMG1 != nullptr) {
            //glFramebufferTexture2D(GL_FRAMEBUFFER,    GL_COLOR_ATTACHMENT1, (texIMG1->IsMSAA())?GL_TEXTURE_2D_MULTISAMPLE:GL_TEXTURE_2D,  texIMG1->getGLId(), 0);
        glFramebufferTexture(GL_FRAMEBUFFER,    GL_COLOR_ATTACHMENT1,   texIMG1->getGLId(), 0);
    }
    if (texIMG2 != nullptr) {
           // glFramebufferTexture2D(GL_FRAMEBUFFER,    GL_COLOR_ATTACHMENT2,  (texIMG2->IsMSAA())?GL_TEXTURE_2D_MULTISAMPLE:GL_TEXTURE_2D,  texIMG2->getGLId(), 0);
        glFramebufferTexture(GL_FRAMEBUFFER,    GL_COLOR_ATTACHMENT2,    texIMG2->getGLId(), 0);
    }

    GLenum  buffers [] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2};

    glDrawBuffers ( 3, buffers );

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    int rcode = glCheckFramebufferStatus(GL_FRAMEBUFFER) ;
    if(rcode != GL_FRAMEBUFFER_COMPLETE) {
        //EMSGS(string_format("FBO incomplite error %d\n",rcode));
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    IsReady = true;

}

RBO::~RBO()
{
    glDeleteFramebuffers(1,&d_fbo);
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
      glReadBuffer(GL_COLOR_ATTACHMENT0);
      glDrawBuffers ( 1, buffers1 );
      glBlitFramebuffer(0, 0, w, h, 0, 0, dst.w, dst.h, GL_COLOR_BUFFER_BIT |  GL_DEPTH_BUFFER_BIT, GL_NEAREST);
      //TODO
      glReadBuffer(GL_COLOR_ATTACHMENT1);
      glDrawBuffers ( 1, buffers2 );
      glBlitFramebuffer(0, 0, w, h, 0, 0, dst.w, dst.h, GL_COLOR_BUFFER_BIT , GL_NEAREST);
      glReadBuffer(GL_COLOR_ATTACHMENT2);
      glDrawBuffers ( 1, buffers3 );
      glBlitFramebuffer(0, 0, w, h, 0, 0, dst.w, dst.h, GL_COLOR_BUFFER_BIT , GL_NEAREST);

      //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
      //glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);


      return 0;
}
