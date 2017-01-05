#pragma once
#include "OGL.h"

#include <memory>
#include "mat_math.h"


class SGenericTexture;
class RBO;
class SShader;


/* Prost Process */
class SPostProcess {
public:
    /*refactoring in process*/
    SPostProcess (SShader *prog, int w, int h, const SGenericTexture* texSRC1, \
                                               const SGenericTexture* texSRC2, \
                                               const SGenericTexture* texSRC3,
                                               const SGenericTexture* texSRC4);

    SPostProcess(SShader *prog,int w, int h,   const SGenericTexture* texSRC1,
                                              const SGenericTexture* texSRC2,
                                              const SGenericTexture* texSRC3)
                    :SPostProcess(prog,w,h,texSRC1,texSRC2, texSRC3,0) {}
    SPostProcess(SShader *prog,int w, int h,    const SGenericTexture* texSRC1,
                                                const SGenericTexture* texSRC2)
                    :SPostProcess(prog,w,h,texSRC1,texSRC2, 0,0) {}
    SPostProcess(SShader *prog,int w, int h,    const SGenericTexture* texSRC1)
                     :SPostProcess(prog,w,h,texSRC1, 0, 0,0) {}
    SPostProcess(SShader *prog,int w, int h) :SPostProcess(prog,w,h, 0, 0, 0,0) {}
    /*new interface*/
    SPostProcess (SShader *prog, RBO * resultRBO,
                  RBO * srcRBO1,
                  RBO * srcRBO2,
                  RBO * srcRBO3,
                  RBO * srcRBO4
                  );

    SPostProcess (SShader *prog, RBO * resultRBO,
                  RBO * srcRBO1)
                :SPostProcess(prog,resultRBO,srcRBO1,0,0,0) {}
    SPostProcess (SShader *prog,RBO * resultRBO,
                  RBO * srcRBO1,
                  RBO * srcRBO2)
                :SPostProcess(prog,resultRBO,srcRBO1,srcRBO2,0,0) {}
    SPostProcess (SShader *prog,RBO * resultRBO,
                  RBO * srcRBO1,
                  RBO * srcRBO2,
                  RBO * srcRBO3)
                :SPostProcess(prog,resultRBO,srcRBO1,srcRBO2,srcRBO3,0) {}

    SPostProcess(const SPostProcess&) = delete;
    void Draw();
    void DrawRBO(bool redraw); /*Draw to rbo selected on creation*/
    void inline DrawRBO() { DrawRBO(true); }
    void Clean();


    SShader * getShader();

    const SGenericTexture *texSRC(int id);


    RBO * getResultRBO();

    int setTexSrc1(SGenericTexture * r);
private:
    /*working shader */
    SShader * p_prog;
    void InitQuard();
    void InitUniforms(RectSizeInt s);

    GLuint vbo;
    GLuint vao;
    GLuint ibo;

    constexpr static int SRC_TEXTURES_MAX = 4;

    const SGenericTexture*  d_texSRC[SRC_TEXTURES_MAX];

    /*Result RBO*/
    RBO * d_resultRBO;
    /*SRC RBO*/
    RBO * d_RBO[SRC_TEXTURES_MAX];



};
