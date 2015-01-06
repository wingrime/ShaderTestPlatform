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
    enum RBOType {
    RBO_SCREEN = 0,
    RBO_CUBEMAP,
    RBO_FLOAT,
    RBO_RGBA,
    RBO_RED,
    RBO_MIXED, /* by texture type*/
    RBO_MSAA,
    RBO_FLOAT_RED, /*single 32 bit float */
    /*RBO_DEPTH ??*/
    };

	/*base constructor*/
    RBO(int w, int h,RBOType type, std::shared_ptr<SRBOTexture> texIMG, \
                                               std::shared_ptr<SRBOTexture> texIMG1,\
                                               std::shared_ptr<SRBOTexture> texIMG2,\
                                               std::shared_ptr<SRBOTexture> texDEPTH);

    RBO(int w, int h,RBOType type, std::shared_ptr<SRBOTexture> texIMG, \
                                               std::shared_ptr<SRBOTexture> texDEPTH)
		:RBO(w, h , type , texIMG,nullptr,nullptr, texDEPTH ){};
    RBO(int w, int h,RBOType type, std::shared_ptr<SRBOTexture> texIMG ) :RBO(w, h , type , texIMG, nullptr ){};

    RBO(int def_w, int def_h,RBOType type);

    RBO(int w, int h) :RBO(w,h,RBO_SCREEN) {};
    ~RBO();

    /*new interface*/
    RBO(int def_w, int def_h,RBOType type, SRBOTexture::RTType t0_type, int t0_s, \
                                           SRBOTexture::RTType t1_type, int t1_s, \
                                           SRBOTexture::RTType t2_type, int t2_s );

    RBO(const RBO&) = delete;

    int ResolveMSAA(const RBO& dst);

    int Bind(bool clear) const;
    int Bind() const {return Bind(true);}



    std::shared_ptr<SRBOTexture> texIMG(); /*color attachment 0*/
    std::shared_ptr<SRBOTexture> texIMG1(); /*color attachment 1*/
    std::shared_ptr<SRBOTexture> texIMG2(); /*color attachment 2*/
    std::shared_ptr<SRBOTexture> texDEPTH();

    /*required sizes*/
    int w;
    int h;
    bool IsReady = false;
private:
    GLuint depthrenderbuffer = 0;
    GLuint d_fbo = 0;

    bool d_isMSAA = false;

    std::shared_ptr<SRBOTexture> d_texIMG; /*color attachment 0*/
    std::shared_ptr<SRBOTexture> d_texIMG1; /*color attachment 1*/
    std::shared_ptr<SRBOTexture> d_texIMG2; /*color attachment 2*/
    std::shared_ptr<SRBOTexture> d_texDEPTH;

    RBOType d_type;

    void initDepthRenderBuffer() ;

    int attachRBOTextures();

    static SRBOTexture::RTType  getRelatedRBOTextueTypeFromRBOType( RBOType t);
    static SRBOTexture::RTType  getRelatedDepthRBOTextueTypeFromRBOType( RBOType t);
};
