#pragma once

#include "c_filebuffer.h"

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/wglew.h>

#include <string>

#include "Log.h"

/* Compute shader stand-alone module*/

class SSBuffer :public AbstractBuffer {
public:
    SSBuffer(int sz);
    ~SSBuffer();
private:
    GLuint d_ssbo;
};


class SCProg {
public:
    SCProg(const std::string& src_file)  : SCProg(new FileBuffer(std::string(".\\shaders\\")+src_file)) {}
    SCProg( AbstractBuffer* src);
    ~SCProg();

    /* Data */
    int DataBuffer(SSBuffer* dat);

    /* Control */
    int Dispatch()  {return Dispatch (1,1,1);}
    int Dispatch(int numg_x , int numg_y , int numg_z);

    int Barrier();

    int SetUniform(const std::string& name,int i);

    int LookupUniformLocation(const std::string &name) ;
    int Use();
    bool d_ready;
private:
    GLuint d_program;
    GLuint d_shader;
};

