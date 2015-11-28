#include "PostProcess.h"
#include "MAssert.h"
#include "RBO.h"
#include "r_sprog.h"
#include "mat_math.h"
#include "GenericTexture.h"

SPostProcess::SPostProcess(SShader *prog, int w, int h,
                           const SGenericTexture* texSRC1,
                           const SGenericTexture *texSRC2,
                           const SGenericTexture* texSRC3,
                           const SGenericTexture* texSRC4)

    :p_prog(prog)
{
    for (int i = 0 ; i < SRC_TEXTURES_MAX ; i++){
        d_texSRC[i] = 0;
        d_RBO[i] = 0; //??

    }
    d_resultRBO = 0;
    d_texSRC[0] = texSRC1;
    d_texSRC[1] = texSRC2;
    d_texSRC[2] = texSRC3;
    d_texSRC[3] = texSRC4;


    InitQuard();
    InitUniforms(RectSizeInt(w,h));



}


SPostProcess::SPostProcess(SShader *prog,  RBO * resultRBO,
                                           RBO * srcRBO1,
                                           RBO * srcRBO2,
                                           RBO * srcRBO3,
                                           RBO * srcRBO4)
:p_prog(prog),d_resultRBO(resultRBO)
{
    for (int i = 0 ; i < SRC_TEXTURES_MAX ; i++){
        d_texSRC[i] = 0;
    }
    d_RBO[0] = srcRBO1;
    d_RBO[1] = srcRBO2;
    d_RBO[2] = srcRBO3;
    d_RBO[3] = srcRBO4;

    for (int i = 0 ; i < SRC_TEXTURES_MAX ; i++){
        if (d_RBO[i])
            d_texSRC[i] = d_RBO[i]->texIMG(0);
    }

    InitQuard();
    RectSizeInt sz = resultRBO->getSize();
    InitUniforms(sz);
}

void SPostProcess::InitUniforms(RectSizeInt s)
{
    for (int i = 0 ; i < SRC_TEXTURES_MAX ; i++){
        char  buf[20];
        sprintf(buf,"texSRC%d",i+1);
        if (d_texSRC[i])
            p_prog->SetUniform(buf,i);
    }
    /*configure outbuffer size*/
    p_prog->SetUniform("vp",vec4(s.w,s.h,0,0));

}
void SPostProcess::Draw() {
    if (p_prog->IsReady)
    {
        glBindVertexArray ( vao );
        p_prog-> Bind();
        for (int i = 0 ; i < SRC_TEXTURES_MAX ; i++){
            if (d_texSRC[i])
                d_texSRC[i]->Bind(i);
        }

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, (GLvoid *)0);
        glBindVertexArray ( 0 );
    }
    

}

void SPostProcess::DrawRBO(bool redraw)
{
    if (p_prog->IsReady)
    {
        MASSERT(!d_RBO);
        MASSERT(!d_resultRBO);
        d_resultRBO->Bind(redraw);
        Draw();
    }
}

void SPostProcess::Clean()


{
    MASSERT(!d_resultRBO);
    d_resultRBO->Clean();
}

SShader *SPostProcess::getShader()
{
    return p_prog;
}

const SGenericTexture* SPostProcess::texSRC(int id)
{
    return d_texSRC[id];

}

RBO * SPostProcess::getResultRBO()
{
    return d_resultRBO;

}

int SPostProcess::setTexSrc1(SGenericTexture* r)
{
    d_texSRC[0] = r;
    return 0;
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

