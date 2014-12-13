//TODO: get rid many of this using templates
#pragma once
#include "mat_math.h"

#include "c_filebuffer.h"

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/wglew.h>
#include <unordered_map>
#include <string>
#include <vector>

#include "r_shader.h"
#include "MAssert.h"
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
    SShader(const std::string& vprog,const std::string& fprog);
    ~SShader();
    int Bind();

    /* Link to low level interface*/
    SProg * prog;
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
    std::vector< std::pair<int, SMat4x4> > d_matrix_update_list;
    std::vector< std::pair<int, int> >     d_int_update_list;
    std::vector< std::pair<int, float > >  d_float_update_list;
    std::vector< std::pair<int, SVec4 >  > d_vector_update_list;

};


SShader::SShader(const std::string &vprog, const std::string &fprog)
    :prog(new SProg(vprog, fprog))
{
    IsReady =  prog->IsReady;
}

int SShader::Bind()
{
    prog->Bind();
    if (prog->IsReady) {
   for (auto element : d_matrix_update_list ) {
       prog->SetUniform(element.first, element.second);
    }
   d_matrix_update_list.clear();
   for (auto element : d_int_update_list ) {
       prog->SetUniform(element.first, element.second);
    }
   d_int_update_list.clear();
   for (auto element : d_float_update_list ) {
      prog->SetUniform(element.first, element.second);
    }
   d_float_update_list.clear();
   for (auto element : d_vector_update_list ) {
       prog->SetUniform(element.first, element.second);
    }
   d_vector_update_list.clear();
    }
    else
        return EFAIL;
   return ESUCCESS;
}

int SShader::SetUniform(const std::string &var, float i)
{
    int loc = prog->LookupUniformLocation(var);
    if (loc == EFAIL)
        return EFAIL;
    d_float_update_list.push_back( std::pair<int, float >( loc, i) );
    //return prog->SetUniform(var, i);
    return ESUCCESS;
}

int SShader::SetUniform(const std::string &var, int i)
{
    int loc = prog->LookupUniformLocation(var);
    if (loc == EFAIL)
        return EFAIL;
    d_int_update_list.push_back( std::pair<int, int >( loc, i) );
    //return prog->SetUniform(var, i);
    return ESUCCESS;
}

int SShader::SetUniform(const std::string &var, const SMat4x4 &mat)
{
    int loc = prog->LookupUniformLocation(var);
    if (loc == EFAIL)
        return EFAIL;
    d_matrix_update_list.push_back( std::pair<int, SMat4x4 >( loc, mat) );
    //return prog->SetUniform(loc, mat);
    return ESUCCESS;
}

int SShader::SetUniform(const std::string &var, const SVec4 &vec)
{
    int loc = prog->LookupUniformLocation(var);
    if (loc == EFAIL)
        return EFAIL;
    d_vector_update_list.push_back( std::pair<int,SVec4 >( loc, vec) );
    //return prog->SetUniform(var, vec);
    return ESUCCESS;
}



