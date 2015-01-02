#pragma once 
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/wglew.h>
#include "r_texture.h" // FIXME
#include "r_rbo_texture.h"
#include "RBO.h"

/*RenderTarget Type*/


class RBO {

public:
    //TODO: texIMG1 and texIMG2 not creates by default 

	/*base constructor*/
    RBO(int w, int h,SRBOTexture::RTType type, std::shared_ptr<SRBOTexture> texIMG,std::shared_ptr<SRBOTexture> texIMG1,std::shared_ptr<SRBOTexture> texIMG2, std::shared_ptr<SRBOTexture> texDEPTH);
    RBO(int w, int h,SRBOTexture::RTType type, std::shared_ptr<SRBOTexture> texIMG, std::shared_ptr<SRBOTexture> texDEPTH)
		:RBO(w, h , type , texIMG,nullptr,nullptr, texDEPTH ){};
    RBO(int w, int h,SRBOTexture::RTType type, std::shared_ptr<SRBOTexture> texIMG ) :RBO(w, h , type , texIMG, nullptr ){};
    RBO(int w, int h, SRBOTexture::RTType type) :RBO(w,h, type , nullptr , nullptr ){};
    RBO(int w, int h) :RBO(w,h,SRBOTexture::RT_SCREEN) {};
    ~RBO();

    RBO(const RBO&) = delete;

    int ResolveMSAA(const RBO& dst);

    int Bind(bool clear) const;
    int Bind() const {return Bind(true);}

    SRBOTexture::RTType type;

    std::shared_ptr<SRBOTexture> texIMG; /*color attachment 0*/
    std::shared_ptr<SRBOTexture> texIMG1; /*color attachment 1*/
    std::shared_ptr<SRBOTexture> texIMG2; /*color attachment 2*/
    std::shared_ptr<SRBOTexture> texDEPTH;

    /*required sizes*/
    int w;
    int h;
    bool IsReady = false;
private:
    GLuint depthrenderbuffer = 0;
    GLuint d_fbo = 0;

    bool d_isMSAA = false;

    void initDepthRenderBuffer() ;

};
