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

/*LowLevel interface to shader program*/
class SProg {
    public:


        SProg(const std::string& vprog,const std::string& fprog);
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
        int SetAttrib(const std::string& name, int numComponents, GLsizei stride, void * ptr, GLenum type);
        bool IsReady = false;

    private:

        std::unordered_map<std::string, int> d_location_lookup;
        GLuint d_program;
        GLuint vs;
        GLuint fs;
        std::string v_pname;
        std::string f_pname;

    private:

        GLuint LoadShader( const char * source, GLenum type, const std::string& fname );


};


int SProg::Bind() {
    if (IsReady) {
        glUseProgram      ( d_program ); 
        return ESUCCESS;
    } else 
        return EFAIL;
}
GLuint  SProg::LoadShader (const char * source, GLenum type, const std::string &fname ) {
    GLuint shader = glCreateShader ( type );
    GLint   len   = strlen ( source );
    GLint   compileStatus;
    char * error_log ;
    int ret_len;
    int log_size;


    glShaderSource ( shader, 1, &source,  &len );
    glCompileShader ( shader );

    glGetShaderiv ( shader, GL_COMPILE_STATUS, &compileStatus );
    glGetShaderiv ( shader, GL_INFO_LOG_LENGTH, &log_size );

    if ( compileStatus == 0 )           // some error
    {
        error_log = (char *)calloc( log_size,sizeof(char));
        glGetShaderInfoLog(shader,log_size,&ret_len, error_log);
        std::string e_log(error_log);
        free(error_log);
        EMSGS(std::string("Shader compile failed:")+fname + std::string("\n")+e_log);  
        return EFAIL;      
        
    }

    return shader;
}

int SProg::LookupUniformLocation(const std::string &name) {

    if (d_location_lookup.find(name) == d_location_lookup.end() ) {
        int loc = glGetUniformLocation ( d_program, name.c_str() );
        if ( loc < 0 ) {
            return EFAIL;
        }
        d_location_lookup[name] = loc;
        return loc;

    } else
        return d_location_lookup[name];
}
/*fragment with vertext prog*/
SProg::SProg(const std::string& vprog,const std::string& fprog) 
	: v_pname(vprog) ,
	f_pname(fprog)
    {

    //shaders source
    GLint   linked;
    char * error_log ;
    int ret_len;
    int log_size;
    d_program = glCreateProgram ();
    //fragement shader
    FileBuffer *frag = new FileBuffer(std::string(".\\shaders\\")+fprog);
    FileBuffer *vert = new FileBuffer(std::string(".\\shaders\\")+vprog);
    vs = LoadShader ( (const char *)vert->buffer, GL_VERTEX_SHADER, vprog  );
    if (vs == EFAIL) {
        EMSGS("Vertex Shader Build Failed");
        IsReady = false;
        return;
    }
    fs = LoadShader ( (const char *)frag->buffer,GL_FRAGMENT_SHADER , fprog);
    if (fs == EFAIL) {
        EMSGS("Fragment Shader Build Failed");
        IsReady = false;
        return;
    }
    glAttachShader ( d_program, vs );
    glAttachShader ( d_program, fs );
    

    glLinkProgram  ( d_program );
    glGetProgramiv ( d_program, GL_LINK_STATUS, &linked );
    glGetProgramiv ( d_program, GL_INFO_LOG_LENGTH, &log_size );

    if ( !linked )
    {   
        error_log = (char *)calloc( log_size,sizeof(char));
        glGetProgramInfoLog(d_program,log_size,&ret_len, error_log);
        std::string err_m = std::string(error_log);
        free(error_log);
        EMSGS(std::string("Shader Link Failed, vfile:")+std::string(vprog) +std::string(",ffile:")+std::string(fprog)+std::string("\n")+err_m);
            delete frag;
            delete vert;
            IsReady = false;
            return;
    }
    delete frag;
    delete vert;
    IsReady = true;
}
SProg::~SProg(){
	glDeleteProgram(d_program);
}
int SProg::SetUniform(const std::string& name, float i) {
    int loc = LookupUniformLocation(name);
    if ( loc == EFAIL ) {
        //return false;
        //EMSGS(std::string("No uniform float ")+name + std::string(" in program: ") +v_pname + std::string(", ") + f_pname );
        return EFAIL;
    }

    glUniform1f ( loc,i);
    return 0;
}
int SProg::SetUniform(const std::string& name,int i) {
    int loc = LookupUniformLocation(name);
    if ( loc == EFAIL ) {
        //EMSGS(std::string("No uniform int(or sampler) ")+name + std::string(" in  program: ") +v_pname + std::string(", ") + f_pname );
        return EFAIL;
    }

    glUniform1i ( loc,i);
    return 0;
}
int SProg::SetUniform( const std::string& name ,const SMat4x4& mat )
{

    int loc = LookupUniformLocation(name);
    if ( loc == EFAIL ) {
        //EMSGS(string_format("No uniform mat4 \"%s\"", name.c_str()) + std::string(" in program: ") +v_pname + std::string(", ") + f_pname );
        return EFAIL;
    }


    /*gl use row-major notation*/
    glUniformMatrix4fv(loc, 1, GL_TRUE, (float *) mat.mat.raw);

    return ESUCCESS;
}

int SProg::SetUniform(int loc, const SVec4 &vec)
{
    glUniform4fv(loc, 1, (float *)vec.vec.raw);
    return ESUCCESS;
}

int SProg::SetUniform(int loc, float i)
{

    glUniform1f ( loc,i);
    return ESUCCESS;
}

int SProg::SetUniform(int loc, int i)
{

    glUniform1i ( loc,i);
    return ESUCCESS;
}

int SProg::SetUniform(int loc, const SMat4x4 &mat)
{


    /*gl use row-major notation*/
    glUniformMatrix4fv(loc, 1, GL_TRUE, (float *) mat.mat.raw);
    return ESUCCESS;
}
int SProg::SetUniform(const std::string& name, const SVec4& vec)
{
    int loc = LookupUniformLocation(name);
    if ( loc == EFAIL ) {
        //EMSGS(string_format("No uniform vec4 \"%s\"", name.c_str())+ std::string(" in program: ") +v_pname + std::string(", ") + f_pname );
        return EFAIL;
    }
    glUniform4fv(loc, 1, (float *)vec.vec.raw);

    return ESUCCESS;
}





int SProg::SetAttrib (const std::string& name, int numComponents, GLsizei stride, void * ptr, GLenum type )
{
    MASSERT(!ptr);
    int loc = glGetAttribLocation ( d_program, name.c_str() );

    if ( loc < 0 )
    {
        //EMSGS("No Vertex Attibute Find in:"+ name + std::string(" in program: ") +v_pname + std::string(", ") + f_pname );
         return EFAIL;
    }   

    glVertexAttribPointer ( loc,                  // index
                            numComponents,        // number of intues per vertex
                           	type,                 // type (GL_FLOAT)
                            GL_FALSE,       /*fixed point data not used*/
                            stride,               // stride (offset to next vertex data)
                     		(const GLvoid*) ptr );
        
    glEnableVertexAttribArray ( loc );
    
     return ESUCCESS;
}
