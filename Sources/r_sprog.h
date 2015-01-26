//TODO: get rid many of this using templates
#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/wglew.h>


class SProg;
class SMat4x4;
class SVec4;

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
    unsigned int getUniformLocation(const std::string& name);
    ~SShader();
    int Bind();


    /*Shader variables defered change*/
    int SetUniform(const std::string& var,float i );
    int SetUniform(const std::string& var,int i );
    int SetUniform(const std::string& var,const SMat4x4& mat );
    int SetUniform(const std::string& var,const SVec4& vec );
    /*via location*/
    int SetUniform(int loc, float i );
    int SetUniform(int loc,int i );
    int SetUniform(int loc,const SVec4& vec );
    int SetUniform(int loc,const SMat4x4& mat );

    int SetAttrib(const std::string& name, int numComponents, GLsizei stride, unsigned int offset, GLenum type);


    bool IsReady = false;
private:
    /* Link to low level interface*/
    SProg * prog;

    std::vector< std::pair<int, const SMat4x4> > d_matrix_update_list;
    std::vector< std::pair<int, int> >     d_int_update_list;
    std::vector< std::pair<int, float > >  d_float_update_list;
    std::vector< std::pair<int, SVec4 >  > d_vector_update_list;

};



