#pragma once
#include "mat_math.h"

#include "c_filebuffer.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/wglew.h>


/*LowLevel interface to shader program*/
class SProg {
    public:


        SProg(const std::string& vprog, const std::string& fprog, const std::string &gprog);
        SProg(const std::string& vprog , const std::string& fprog) :SProg(vprog,fprog,"") {}
        ~SProg();

        SProg(const SProg&) = delete;

        int SetUniform(const std::string& name,const SVec4& vec );
        int SetUniform(const std::string& name,float i);
        int SetUniform(const std::string& name,int i );
        int SetUniform(const std::string& name,const SMat4x4& mat );

        int SetUniform(int loc,const SVec4& vec );
        int SetUniform(int loc,float i);
        int SetUniform(int loc,int i );
        int SetUniform(int loc,const SMat4x4& mat );

        int LookupUniformLocation(const std::string& name);

        int Bind();
        //TODO: Make this hidden
        int SetAttrib(const std::string& name, int numComponents, GLsizei stride, unsigned int offset, GLenum type);
        bool IsReady = false;

    private:

        std::unordered_map<std::string, int> d_location_lookup;
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
