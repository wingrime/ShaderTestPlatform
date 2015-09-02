#pragma once
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/wglew.h>
/* enums can't forward */
#include "RBOTexture.h"
#include <vector>
class SVec2;

/*RenderTarget Type*/
class RBO {

public:
    enum RBOType {
    RBO_NONE = 0,
    RBO_SCREEN,
    RBO_CUBEMAP,
    RBO_FLOAT,
    RBO_RGBA,
    RBO_RED,
    RBO_MIXED, /* by texture type*/
    RBO_MSAA,
    RBO_FLOAT_RED, /*single 32 bit float */
    RBO_DEPTH_ONLY,
    RBO_DEPTH_ARRAY_ONLY
    };

	/*base constructor*/
    RBO(std::string name,int w, int h,RBOType type, \
                                               SRBOTexture * texIMG, \
                                               SRBOTexture * texIMG1,\
                                               SRBOTexture * texIMG2,\
                                               SRBOTexture * texDEPTH);

    RBO(std::string name,int w, int h,RBOType type, \
                                               SRBOTexture * texIMG, \
                                               SRBOTexture * texDEPTH)

        : RBO(name , w, h , type , texIMG,0,0, texDEPTH ){}
    RBO(std::string name,int w, int h,RBOType type, SRBOTexture * texIMG ) :RBO(name,w, h , type , texIMG, 0 ){}

    RBO(std::string name,int def_w, int def_h,RBOType type);

    RBO(std::string name,int w, int h) :RBO(name,w,h,RBO_SCREEN) {}
    ~RBO();

    /*new interface*/
    RBO(std::string name,int def_w, int def_h,RBOType type, SRBOTexture::RTType t0_type, int t0_s, \
                                           SRBOTexture::RTType t1_type, int t1_s, \
                                           SRBOTexture::RTType t2_type, int t2_s );

    RBO(const RBO&) = delete;

    int ResolveMSAA(const RBO& dst);

    int Bind(bool clear) const;
    int Bind() const {return Bind(true);}
    int Clean();

    std::string getName();



    SGenericTexture*  texIMG(int n); /*color attachment 0*/
    SGenericTexture* texDEPTH();

    /*Request size*/
    SVec2 getSize();
    /*Do resize*/
    int Resize(SVec2 new_sz);
    bool IsReady = false;
    RBOType getType();
private:
    static constexpr int MAX_COLOR_ATTACHMENTS = 3;
    int d_w;
    int d_h;
    std::string d_name;
    GLuint depthrenderbuffer = 0;
    GLuint d_fbo = 0;

    bool d_isMSAA = false;

    int d_buffers = 0;

    SRBOTexture * d_texIMG[MAX_COLOR_ATTACHMENTS]; /*color attachments*/

    SRBOTexture * d_texDEPTH;

    RBOType d_type;

    void initDepthRenderBuffer() ;

    int attachRBOTextures();

    static SRBOTexture::RTType  getRelatedRBOTextueTypeFromRBOType( RBOType t);
    static SRBOTexture::RTType  getRelatedDepthRBOTextueTypeFromRBOType( RBOType t);
    static bool isDepthOnlyType(RBOType t);
    /*debug API to RBOList*/
public:
    static std::vector<RBO * > debugGetRenderOutputList();
private:
    int debugRegisterSelf();
    static std::vector< RBO * > debugRenderOutputList;


};

