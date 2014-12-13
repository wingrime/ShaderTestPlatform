#pragma once

#include "c_filebuffer.h"

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/wglew.h>

#include <string>

/* Compute shader stand-alone module*/

class SSBuffer :public AbstractBuffer {
public:
    SSBuffer(int sz);
    ~SSBuffer();
private:
    GLuint d_ssbo;
};
SSBuffer::SSBuffer(int sz) {
    glGenBuffers(1, &d_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,d_ssbo);
}

class SCProg {
public:
    SCProg(const std::string& src_file)  : SCProg(new FileBuffer(src_file)) {}
    SCProg( AbstractBuffer* src);
    ~SCProg();

    /* Data */
    int DataBuffer(SSBuffer* dat);

    /* Control */
    int Dispatch()  {return Dispatch (1,1,1);}
    int Dispatch(int numg_x , int numg_y , int numg_z);

    int Barrier();

    bool d_ready;
private:
    GLuint d_program;
    GLuint d_shader;
};


SCProg::SCProg(AbstractBuffer* src)
{
    d_ready  = false;
    d_program = glCreateProgram();
    d_shader  = glCreateShader (GL_COMPUTE_SHADER);
    const char * source = (const char *) src->buffer;
    int len = strlen(source);

    glShaderSource (d_shader,1 , &source, &len);
    glCompileShader(d_shader);

    GLint compileStatus;
    GLint compileLogsz;
    glGetShaderiv ( d_shader, GL_COMPILE_STATUS, &compileStatus );
    glGetShaderiv ( d_shader, GL_INFO_LOG_LENGTH, &compileLogsz );
    char *errorLog;
    if ( compileStatus == 0 )
    {
        GLint logRecived;
        errorLog = (char *)calloc(compileLogsz,sizeof(char));
        glGetShaderInfoLog(d_shader,compileLogsz,&logRecived, errorLog);
        std::string e_log(errorLog);
        free(errorLog);
        EMSGS(std::string("Compute shader build failed:\n")+e_log);
        /*TODO proper cleanup*/
        return;
    }

    glAttachShader(d_program, d_shader);
    GLint linkStatus;
    GLint linkLogsz;
    glLinkProgram (d_program);
    glGetProgramiv ( d_program, GL_LINK_STATUS, &linkStatus );
    glGetProgramiv ( d_program, GL_INFO_LOG_LENGTH, &linkLogsz);


    if ( linkStatus == 0 )
    {
        GLint logRecived;
        errorLog = (char *)calloc(linkLogsz,sizeof(char));
        glGetProgramInfoLog(d_program,linkLogsz,&logRecived, errorLog);
        std::string e_log(errorLog);
        free(errorLog);
        EMSGS(std::string("Compute shader link failed:\n")+e_log);
        return;
    }
    d_ready = true;
}
int SCProg::Dispatch(int numg_x , int numg_y , int numg_z) {
    glUseProgram(d_program);
    glDispatchCompute(numg_x, numg_y, numg_z);
    return ESUCCESS;
}
int SCProg::Barrier() {
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    return ESUCCESS;
}
