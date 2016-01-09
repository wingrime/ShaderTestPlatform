#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/wglew.h>

/* shader class*/
#include "Shader.h"
#include "mat_math.h"
#include "viewport.h"
#include "r_texture.h"
#include "c_config.h"

#include "r_projmat.h"

class SModel {
    public: 
        SModel(const std::string& fname);
        void Render();
        void SetModelMat(const SMat4x4& m);

        void SetCamera(const SMat4x4& m);

        ~SModel();


        //matrices
        SMat4x4 model;
        SMat4x4 view;
        bool UV_aval;
        bool normal_aval;

		SShader * sprog;   
        STexture* texDiffuse;
        STexture* texNormal;

        bool IsReady = false;
   
    private:
        GLuint  vbo, vao;

        GLuint ibo;

        int indexes;
        int vertices;

        FileBuffer *ibuf;
        FileBuffer *vbuf;

};
SModel::SModel(const std::string&  fname) {
    /* load descriptor*/
    Config mesh(fname);
    if (!mesh.IsReady) {
        EMSGS(std::string("mesh json  file not found"));
        return;
    }


        vbuf = new FileBuffer(mesh["vert_file"].GetString());
        ibuf = new FileBuffer(mesh["index_file"].GetString());

    if (!vbuf->IsReady) {
        EMSGS(std::string(mesh["vert_file"].GetString())+std::string("not found, no vertex file"))
        return;
    }
    if (!ibuf->IsReady) {
        EMSGS(std::string(mesh["index_file"].GetString())+std::string("not found, no index file"))
        return;
    }

    texDiffuse = new STexture("pavtexture//08_DIFFUSE.jpg");
    if (!texDiffuse->IsReady) {
        EMSGS(std::string(" diffuse texture file not found"));
        return;
    }
    texNormal = new STexture("pavtexture//08_DIFFUSE.jpg");
    if (!texNormal->IsReady) {
        EMSGS(std::string("normal texture file not found"));
        return;
    }

    vertices =  mesh["vertices"].GetInt(); 
    indexes =  mesh["indexes"].GetInt();

    /*shader */
    sprog = new SShader("shader.v","shader.f");

    /*vertixes*/
    glGenVertexArrays ( 1, &vao );
    glBindVertexArray ( vao );
    glGenBuffers ( 1, &vbo );
    glBindBuffer ( GL_ARRAY_BUFFER, vbo );
    glBufferData ( GL_ARRAY_BUFFER, vertices * mesh["vertex_sz"].GetInt() , vbuf->buffer, GL_STATIC_DRAW);


    /*indexes*/
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,  indexes* mesh["index_sz"].GetInt(), ibuf->buffer, GL_STATIC_DRAW);

    int stride =  mesh["vertex_sz"].GetInt();

    /*configure mesh prop*/
    sprog->SetAttrib( "position", 3, stride, (void *) (0),GL_FLOAT);
    sprog->SetAttrib( "normal", 3, stride, (void *) (sizeof(float)*3),GL_FLOAT );
    sprog->SetAttrib( "UV", 2, stride, (void *) (sizeof(float)*6),GL_FLOAT);
    /**/
    sprog->SetUniform("texIMG",0);
    sprog->SetUniform("texNormal",1);
    sprog->SetUniform("model",model);
    sprog->SetUniform("view",view);
    sprog->SetUniform("cam_proj",SPerspectiveProjectionMatrix(1.0f, 10000.0f,1.0f,toRad(26.0)));
    glBindVertexArray(0);
    IsReady = true;

}
void SModel::SetModelMat(const SMat4x4& m){
	model = SMat4x4(m);
	/*update shader variable*/ 
    sprog->SetUniform("model",model);
}
void SModel::SetCamera(const SMat4x4& m){
    view = SMat4x4(m);
    /*update shader variable*/ 
    sprog->SetUniform("view",view);
}


void SModel::Render() {
    /*activate shader and load model matrix*/
    if (sprog->IsReady) {
        glBindVertexArray ( vao );
        /*shader*/
        sprog->Bind();
        /*texture*/
        texDiffuse->Bind(0);
        texNormal->Bind(1);
        glDrawElements(GL_TRIANGLES, indexes, GL_UNSIGNED_INT, (GLvoid *)0);
        glBindVertexArray ( 0 );
    }
}

SModel::~SModel() {
    
}
