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
    const std::shared_ptr<SRBOTexture> texSRC1;
    const std::shared_ptr<SRBOTexture> texSRC2;
    const std::shared_ptr<SRBOTexture> texSRC3;
private:
    GLuint vbo;
    GLuint vao;
    GLuint ibo;



};

SPostProcess::SPostProcess(SShader *prog,int w, int h, const std::shared_ptr<SRBOTexture> &texSRC1, const std::shared_ptr<SRBOTexture> &texSRC2, const std::shared_ptr<SRBOTexture> &texSRC3)

    :p_prog(prog), texSRC1(texSRC1), texSRC2(texSRC2),texSRC3(texSRC3) {
  
    GLfloat vertices[] = { -1, -1, 0, //bottom left corner
                           -1,  1, 0, //top left corner
                            1,  1, 0, //top right corner
                            1, -1, 0}; // bottom right rocner

    GLubyte indices[] = {0,1,2, // first triangle (bottom left - top left - top right)
                         0,2,3}; // second triangle (bottom left - top right - bottom right)
    
    /* quard  */
    glGenVertexArrays ( 1, &vao );
    glBindVertexArray ( vao );

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData ( GL_ARRAY_BUFFER,sizeof(GLfloat)*12, vertices, GL_STATIC_DRAW);




    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,  sizeof(GLubyte)*6, indices, GL_STATIC_DRAW);

 
    glEnableVertexAttribArray(0);

    p_prog->SetAttrib( "vCord", 3, 0,0,GL_FLOAT);
    /*configure windows size*/
    p_prog->SetUniform("vp",SVec4(w,h,0,0));

    if (texSRC1)
        p_prog->SetUniform("texSRC1",0);


    if (texSRC2)
        p_prog->SetUniform("texSRC2",1);


    if (texSRC3)
        p_prog->SetUniform("texSRC3",2);


    glBindVertexArray (0);
}
void SPostProcess::Draw() {
    /*Quard*/
    /*If shader usable other wice we will get error*/
    if (p_prog->IsReady)
    {
        glBindVertexArray ( vao );
        p_prog-> Bind();
        if (texSRC1)
            texSRC1->Bind(0);
        if (texSRC2)
            texSRC2->Bind(1);
        if (texSRC3)
            texSRC3->Bind(2);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, (GLvoid *)0);
        glBindVertexArray ( 0 );
    }


    

}
