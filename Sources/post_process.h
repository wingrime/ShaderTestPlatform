#pragma once


#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/wglew.h>

#include <memory>


class SRBOTexture;
class RBO;
class SShader;


/* Prost Process */
class SPostProcess {
public:
    /*refactoring in process*/
    SPostProcess (SShader *prog, int w, int h, const std::shared_ptr<SRBOTexture>& texSRC1, \
                                               const std::shared_ptr<SRBOTexture>& texSRC2, \
                                               const std::shared_ptr<SRBOTexture>& texSRC3,
                                               const std::shared_ptr<SRBOTexture>& texSRC4);

    SPostProcess(SShader *prog,int w, int h,   const std::shared_ptr<SRBOTexture> &texSRC1,
                                               const std::shared_ptr<SRBOTexture> &texSRC2,
                                               const std::shared_ptr<SRBOTexture> &texSRC3)
                    :SPostProcess(prog,w,h,texSRC1,texSRC2, texSRC3,0) {}
    SPostProcess(SShader *prog,int w, int h,   const std::shared_ptr<SRBOTexture> &texSRC1,
                                               const std::shared_ptr<SRBOTexture> &texSRC2)
                    :SPostProcess(prog,w,h,texSRC1,texSRC2, 0,0) {}
    SPostProcess(SShader *prog,int w, int h,   const std::shared_ptr<SRBOTexture> &texSRC1)
                     :SPostProcess(prog,w,h,texSRC1, 0, 0,0) {}
    SPostProcess(SShader *prog,int w, int h) :SPostProcess(prog,w,h, 0, 0, 0,0) {}
    /*new interface*/
    SPostProcess (SShader *prog, std::shared_ptr<RBO> resultRBO,
                  std::shared_ptr<RBO> srcRBO1,
                  std::shared_ptr<RBO> srcRBO2,
                  std::shared_ptr<RBO> srcRBO3,
                  std::shared_ptr<RBO> srcRBO4
                  );

    SPostProcess (SShader *prog, std::shared_ptr<RBO> resultRBO,
                  std::shared_ptr<RBO> srcRBO1)
                :SPostProcess(prog,resultRBO,srcRBO1,0,0,0) {}
    SPostProcess (SShader *prog,std::shared_ptr<RBO> resultRBO,
                  std::shared_ptr<RBO> srcRBO1,
                  std::shared_ptr<RBO> srcRBO2)
                :SPostProcess(prog,resultRBO,srcRBO1,srcRBO2,0,0) {}
    SPostProcess (SShader *prog,std::shared_ptr<RBO> resultRBO,
                  std::shared_ptr<RBO> srcRBO1,
                  std::shared_ptr<RBO> srcRBO2,
                  std::shared_ptr<RBO> srcRBO3)
                :SPostProcess(prog,resultRBO,srcRBO1,srcRBO2,srcRBO3,0) {}

    SPostProcess(const SPostProcess&) = delete;
    void Draw();
    void DrawRBO(bool redraw); /*Draw to rbo selected on creation*/
    void inline DrawRBO() { DrawRBO(true); }


    SShader * getShader();

    std::shared_ptr<SRBOTexture> texSRC(int id);


    std::shared_ptr<RBO> getResultRBO();

    int setTexSrc1(std::shared_ptr<SRBOTexture> r);
private:
    /*working shader */
    SShader * p_prog;
    void InitQuard();

    GLuint vbo;
    GLuint vao;
    GLuint ibo;


    std::shared_ptr<SRBOTexture> d_texSRC[4];

    /*Result RBO*/
    std::shared_ptr<RBO> d_resultRBO;
    /*SRC RBO*/
    std::shared_ptr<RBO> d_RBO[4];

};
