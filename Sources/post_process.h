#pragma once


#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/wglew.h>

/* shader class*/
#include "r_sprog.h"

#include "mat_math.h"

#include "RBO.h"

#include "r_texture.h"


/* Prost Process */
class SPostProcess {
public:
    /*refactoring in process*/
    SPostProcess (SShader *prog, int w, int h, const std::shared_ptr<SRBOTexture>& texSRC1, const std::shared_ptr<SRBOTexture>& texSRC2, const std::shared_ptr<SRBOTexture>& texSRC3);
    SPostProcess(SShader *prog,int w, int h, const std::shared_ptr<SRBOTexture> &texSRC1, const std::shared_ptr<SRBOTexture> &texSRC2) :SPostProcess(prog,w,h,texSRC1,texSRC2, 0) {}
    SPostProcess(SShader *prog,int w, int h, const std::shared_ptr<SRBOTexture> &texSRC1) :SPostProcess(prog,w,h,texSRC1, 0, 0) {}
    SPostProcess(SShader *prog,int w, int h) :SPostProcess(prog,w,h, 0, 0, 0) {} 

    SPostProcess(const SPostProcess&) = delete;
    void Draw();



    /*working shader */
    SShader * p_prog;

    const std::shared_ptr<SRBOTexture> texSRC1();
    const std::shared_ptr<SRBOTexture> texSRC2();
    const std::shared_ptr<SRBOTexture> texSRC3();
private:
    GLuint vbo;
    GLuint vao;
    GLuint ibo;

    const std::shared_ptr<SRBOTexture> d_texSRC1;
    const std::shared_ptr<SRBOTexture> d_texSRC2;
    const std::shared_ptr<SRBOTexture> d_texSRC3;
};
