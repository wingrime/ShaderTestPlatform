#include "r_sprog.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/wglew.h>

#include <unordered_map>
#include <vector>
#include <string>


/*Hi-level interface to shader program*/
/*
Objectives:
    -   Load Shader config variables form json file
    -   Setting shader variables with limitations from
    json file
    -   Automaticaly handle variables update when they 
    are changed.
*/



SShader::SShader(const std::string &vprog, const std::string &fprog, const std::string &gprog)
    :prog(new SProg(vprog, fprog, gprog))
{
    IsReady =  prog->IsReady;
}

unsigned int SShader::getUniformLocation(const std::string &name)
{
   return prog->LookupUniformLocation(name);
}

int SShader::Bind()
{

    if (prog->IsReady) {
        prog->Bind();
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
    d_matrix_update_list.push_back( std::pair<int, const SMat4x4 >( loc, mat) );
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



