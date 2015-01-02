#include "post_process.h"
SPostProcess::SPostProcess(SShader *prog,int w, int h, const std::shared_ptr<SRBOTexture> &texSRC1, const std::shared_ptr<SRBOTexture> &texSRC2, const std::shared_ptr<SRBOTexture> &texSRC3)

    :p_prog(prog), d_texSRC1(texSRC1), d_texSRC2(texSRC2),d_texSRC3(texSRC3) {
  
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

    if (d_texSRC1)
        p_prog->SetUniform("texSRC1",0);


    if (d_texSRC2)
        p_prog->SetUniform("texSRC2",1);


    if (d_texSRC3)
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
        if (d_texSRC1)
            d_texSRC1->Bind(0);
        if (d_texSRC2)
            d_texSRC2->Bind(1);
        if (d_texSRC3)
            d_texSRC3->Bind(2);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, (GLvoid *)0);
        glBindVertexArray ( 0 );
    }


    

}

const std::shared_ptr<SRBOTexture> SPostProcess::texSRC1()
{
    return d_texSRC1;

}

const std::shared_ptr<SRBOTexture> SPostProcess::texSRC2()
{
    return d_texSRC2;

}

const std::shared_ptr<SRBOTexture> SPostProcess::texSRC3()
{
    return d_texSRC3;

}
