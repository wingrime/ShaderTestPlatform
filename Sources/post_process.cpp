#include "post_process.h"
#include "MAssert.h"
#include "RBO.h"
#include "r_sprog.h"
#include "mat_math.h"

SPostProcess::SPostProcess(SShader *prog, int w, int h,
                           const std::shared_ptr<SRBOTexture> &texSRC1,
                           const std::shared_ptr<SRBOTexture> &texSRC2,
                           const std::shared_ptr<SRBOTexture> &texSRC3,
                           const std::shared_ptr<SRBOTexture> &texSRC4)

    :p_prog(prog), d_texSRC1(texSRC1), d_texSRC2(texSRC2),d_texSRC3(texSRC3),d_texSRC4(texSRC4) {
  
    InitQuard();


    /*configure windows size*/
    p_prog->SetUniform("vp",SVec4(w,h,0,0));

    if (d_texSRC1)
        p_prog->SetUniform("texSRC1",0);
    if (d_texSRC2)
        p_prog->SetUniform("texSRC2",1);
    if (d_texSRC3)
        p_prog->SetUniform("texSRC3",2);
    if (d_texSRC4)
        p_prog->SetUniform("texSRC4",3);



}

SPostProcess::SPostProcess(SShader *prog,  std::shared_ptr<RBO> resultRBO,
                                           std::shared_ptr<RBO> srcRBO1,
                                           std::shared_ptr<RBO> srcRBO2,
                                           std::shared_ptr<RBO> srcRBO3,
                                           std::shared_ptr<RBO> srcRBO4)
:p_prog(prog),d_RBO(resultRBO), d_RBO1(srcRBO1),d_RBO2(srcRBO2),d_RBO3(srcRBO3),d_RBO4(srcRBO4)
{
    /*configure outbuffer size*/

    SVec2 sz = resultRBO->getSize();

    d_texSRC1 = srcRBO1->texIMG();
    if (srcRBO2)
        d_texSRC2 = srcRBO2->texIMG();
    if (srcRBO3)
        d_texSRC3 = srcRBO3->texIMG();
    if (srcRBO4)
        d_texSRC4 = srcRBO4->texIMG();

    InitQuard();
    p_prog->SetUniform("vp",SVec4(sz.w,sz.h,0,0));
    p_prog->SetUniform("texSRC1",0);
    if (d_texSRC2)
        p_prog->SetUniform("texSRC2",1);
    if (d_texSRC3)
        p_prog->SetUniform("texSRC3",2);
    if (d_texSRC4)
        p_prog->SetUniform("texSRC4",3);
}

void SPostProcess::Draw() {
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
        if (d_texSRC4)
            d_texSRC4->Bind(3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, (GLvoid *)0);
        glBindVertexArray ( 0 );
    }
    

}

void SPostProcess::DrawRBO(bool redraw)
{
    if (p_prog->IsReady)
    {
        MASSERT(!d_RBO);
        d_RBO->Bind(redraw);
        glBindVertexArray ( vao );
        p_prog-> Bind();
        if (d_texSRC1)
            d_texSRC1->Bind(0);
        if (d_texSRC2)
            d_texSRC2->Bind(1);
        if (d_texSRC3)
            d_texSRC3->Bind(2);
        if (d_texSRC4)
            d_texSRC4->Bind(3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, (GLvoid *)0);
        glBindVertexArray ( 0 );
    }
}

SShader *SPostProcess::getShader()
{
    return p_prog;
}

std::shared_ptr<SRBOTexture> SPostProcess::texSRC1()
{
    return d_texSRC1;

}

std::shared_ptr<SRBOTexture> SPostProcess::texSRC2()
{
    return d_texSRC2;

}

std::shared_ptr<SRBOTexture> SPostProcess::texSRC3()
{
    return d_texSRC3;

}

std::shared_ptr<SRBOTexture> SPostProcess::texSRC4()
{
    return d_texSRC4;

}

std::shared_ptr<RBO> SPostProcess::getResultRBO()
{
    return d_RBO;

}

void SPostProcess::InitQuard()
{
    GLfloat vertices[] = { -1, -1, //bottom left corner
                           -1,  1, //top left corner
                            1,  1, //top right corner
                            1, -1 }; // bottom right rocner

    GLubyte indices[] = {0,1,2, // first triangle (bottom left - top left - top right)
                         0,2,3}; // second triangle (bottom left - top right - bottom right)

    /* quard  */
    glGenVertexArrays ( 1, &vao );
    glBindVertexArray ( vao );

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData ( GL_ARRAY_BUFFER,sizeof(GLfloat)*8, vertices, GL_STATIC_DRAW);


    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,  sizeof(GLubyte)*6, indices, GL_STATIC_DRAW);

    p_prog->SetAttrib( "vCord", 2, 0,0,GL_FLOAT);

    //glEnableVertexAttribArray(0);



    glBindVertexArray (0);
}
