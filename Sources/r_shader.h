#pragma once
#include <unordered_map>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/wglew.h>

#include "c_filebuffer.h"

class vec4;
class SMat4x4;
/*LowLevel interface to shader program*/

class SProg {
    public:
        SProg(const std::string& vprog, const std::string& fprog, const std::string &gprog);
        SProg(const std::string& vprog , const std::string& fprog) :SProg(vprog,fprog,"") {}
        ~SProg();

        SProg(const SProg&) = delete;

        int SetUniform(const std::string& name,const vec4& vec );
        int SetUniform(const std::string& name,float i);
        int SetUniform(const std::string& name,int i );
        int SetUniform(const std::string& name,const SMat4x4& mat );

        int SetUniform(int loc,const vec4& vec );
        int SetUniform(int loc,float i);
        int SetUniform(int loc,int i );
        int SetUniform(int loc,const SMat4x4& mat );

        int LookupUniformLocation(const std::string& name);
        int LookupAttribLocation(const std::string& name);

        int Bind();
        //TODO: should be removed,
        int SetAttrib(const std::string& name, int numComponents, GLsizei stride, unsigned int offset, GLenum type);
        int SetAttrib(unsigned int location, int numComponents, GLsizei stride, unsigned int offset, GLenum type);
        bool IsReady = false;

        /*Request Information*/
        std::string GetVertexShaderFileName() {return v_pname;}
        std::string GetFragmentShaderFileName() {return f_pname;}
        std::string GetGeometryShaderFileName() {return g_pname;}

    private:

        std::unordered_map<std::string, int> d_location_lookup;
        std::unordered_map<std::string, int> d_attrib_lookup;
        GLuint d_program;
        GLuint vs;
        GLuint fs;
        GLuint gs;
        std::string v_pname;
        std::string f_pname;
        std::string g_pname;

    private:

        GLuint LoadShader( const char * source, GLenum type, const std::string& fname );


};
