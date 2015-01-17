//TODO: get rid many of this using templates
#pragma once
#include "mat_math.h"

#include "c_filebuffer.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/wglew.h>
#include <unordered_map>
#include <string>
#include <vector>

#include "r_shader.h"
#include "MAssert.h"
#include "ErrorCodes.h"
/*Hi-level interface to shader program*/
/*
Objectives:
    -   Load Shader config variables form json file
    -   Setting shader variables with limitations from
    json file
    -   Automaticaly handle variables update when they 
    are changed.
*/

class SShader {
public:

    /*Shader in line creation*/
    SShader(const std::string& vprog,const std::string& fprog, const std::string& gprog);
    SShader(const std::string& vprog,const std::string& fprog) :SShader(vprog, fprog, "") {}
    ~SShader();
    int Bind();


    /*Shader variables defered change*/
    int SetUniform(const std::string& var,float i );
    int SetUniform(const std::string& var,int i );
    int SetUniform(const std::string& var,const SMat4x4& mat );
    int SetUniform(const std::string& var,const SVec4& vec );

    //mesh settings
    int SetAttrib(const std::string& name, int numComponents, GLsizei stride, unsigned int offset, GLenum type){
        if(ESUCCESS == prog->Bind()) {
            prog->SetAttrib(name, numComponents, stride, offset,  type);
            return ESUCCESS;
        } else {
            return EFAIL;
        }
    }


    bool IsReady = false;
private:
    /* Link to low level interface*/
    SProg * prog;

    std::vector< std::pair<int, const SMat4x4> > d_matrix_update_list;
    std::vector< std::pair<int, int> >     d_int_update_list;
    std::vector< std::pair<int, float > >  d_float_update_list;
    std::vector< std::pair<int, SVec4 >  > d_vector_update_list;

};



