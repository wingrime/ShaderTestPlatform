#pragma once 
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/wglew.h>
#include "r_texture.h" // FIXME
#include "r_rbo_texture.h"

/*RenderTarget Type*/


class Viewport {

public:
    //TODO: texIMG1 and texIMG2 not creates by default 

	/*base constructor*/
    Viewport(int w, int h,RTType type, std::shared_ptr<SRBOTexture> texIMG,std::shared_ptr<SRBOTexture> texIMG1,std::shared_ptr<SRBOTexture> texIMG2, std::shared_ptr<SRBOTexture> texDEPTH);
    Viewport(int w, int h,RTType type, std::shared_ptr<SRBOTexture> texIMG, std::shared_ptr<SRBOTexture> texDEPTH)
		:Viewport(w, h , type , texIMG,nullptr,nullptr, texDEPTH ){};
    Viewport(int w, int h,RTType type, std::shared_ptr<SRBOTexture> texIMG ) :Viewport(w, h , type , texIMG, nullptr ){};
	Viewport(int w, int h, RTType type) :Viewport(w,h, type , nullptr , nullptr ){};
    Viewport(int w, int h) :Viewport(w,h,RT_SCREEN) {};
    ~Viewport();

    Viewport(const Viewport&) = delete;

    int ResolveMSAA(const Viewport& dst);

    int Bind(bool clear) const;
    int Bind() const {return Bind(true);}

    RTType type;

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
