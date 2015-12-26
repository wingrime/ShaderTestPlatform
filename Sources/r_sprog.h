//TODO: get rid many of this using templates
#pragma once

#include <string>
#include <vector>
#include <list>
#include <unordered_map>

#include <GL/glew.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#include <GL/wglew.h>
#endif


#include "r_shader.h"
class SMat4x4;
class vec4;

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

    SShader (const SShader &) = delete;

    unsigned int getUniformLocation(const std::string& name);
    ~SShader();
    int Bind();


    /*Shader variables defered change*/
    int SetUniform(const std::string& var,float i );
    int SetUniform(const std::string& var,int i );
    int SetUniform(const std::string& var,const SMat4x4& mat );
    int SetUniform(const std::string& var,const vec4& vec );
    /*via location*/
    int SetUniform(int loc, float i );
    int SetUniform(int loc,int i );
    int SetUniform(int loc,const vec4& vec );
    int SetUniform(int loc,const SMat4x4& mat );

    int SetAttrib(const std::string& name, int numComponents, GLsizei stride, unsigned int offset, GLenum type);


    /*Request Information*/
    std::string GetVertexShaderFileName() {return prog->GetVertexShaderFileName(); }
    std::string GetFragmentShaderFileName() {return prog->GetFragmentShaderFileName(); }
    std::string GetGeometryShaderFileName() {return prog->GetVertexShaderFileName(); }

    bool IsReady = false;

    inline SProg * getDirect() {return prog;}

    static std::list<SShader *> getShaderSharedList();




private:
    /*Shared Information */
static std::list<SShader *> s_d_shaders_enum;
    int RegisterEnum();
    /* Link to low level interface*/
    SProg * prog;


    std::vector< std::pair<int, const SMat4x4> > d_matrix_update_list;
    std::vector< std::pair<int, int> >     d_int_update_list;
    std::vector< std::pair<int, float > >  d_float_update_list;
    std::vector< std::pair<int, vec4 >  > d_vector_update_list;

};
