#include "r_shader.h"
#include "ErrorCodes.h"
#include "Log.h"
#include "mat_math.h"
#include "string_format.h"

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
        LOGE(std::string("Shader compile failed:")+fname + std::string("\n")+e_log);
        return EFAIL;      
        
    }

    return shader;
}

int SProg::LookupUniformLocation(const std::string &name) {
    auto it = d_location_lookup.find(name);
    if ( it == d_location_lookup.end() ) {
        int loc = glGetUniformLocation ( d_program, name.c_str() );
        if ( loc < 0 ) {
            d_location_lookup[name] =EFAIL;
            return EFAIL;
        }
        d_location_lookup[name] = loc;
        return loc;

    } else
        return (*it).second;
}

int SProg::LookupAttribLocation(const std::string &name)
{
    auto it = d_attrib_lookup.find(name);
    if ( it == d_attrib_lookup.end() ) {
        int loc = glGetAttribLocation ( d_program, name.c_str() );
        if ( loc < 0 ) {
            d_attrib_lookup[name] = EFAIL;
            return EFAIL;
        }
        d_attrib_lookup[name] = loc;
        return loc;

    } else
        return (*it).second;

}
/*fragment with vertext prog*/
SProg::SProg(const std::string& vprog,const std::string& fprog, const std::string& gprog)
    :   v_pname(vprog) ,
        f_pname(fprog) ,
        g_pname(gprog)
    {
    LOGV(string_format("Load shader: %s,%s,%s",vprog.c_str(),fprog.c_str(),gprog.c_str()));

    //shaders source
    GLint   linked;
    char * error_log ;
    int ret_len;
    int log_size;
    bool gs_used = false;
    d_program = glCreateProgram ();
    //fragement shader
    FileBuffer *frag = new FileBuffer(std::string(".\\shaders\\")+fprog);
    FileBuffer *vert = new FileBuffer(std::string(".\\shaders\\")+vprog);

    if (!vert || !vert->IsReady) {
        LOGE("Vertex Shader Open Failed");
        IsReady = false;
        return;
    }

    if (!frag || !frag->IsReady) {
        LOGE("Fragment Shader Open Failed");
        IsReady = false;
        return;
    }
    vs = LoadShader ( (const char *)vert->buffer(), GL_VERTEX_SHADER, vprog  );
    if (vs == EFAIL) {
        LOGE("Vertex Shader Build Failed");
        IsReady = false;
        return;
    }
    fs = LoadShader ( (const char *)frag->buffer(),GL_FRAGMENT_SHADER , fprog);
    if (fs == EFAIL) {
        LOGE("Fragment Shader Build Failed");
        IsReady = false;
        return;
    }
    if (!gprog.empty())
    {
        gs_used = true;
        FileBuffer *geom = new FileBuffer(std::string(".\\shaders\\")+gprog);
        gs = LoadShader ( (const char *)geom->buffer(), GL_GEOMETRY_SHADER, gprog  );
        if (vs == EFAIL) {
            LOGE("Geometry Shader Build Failed");
            IsReady = false;
            return;
        }
    }
    glAttachShader ( d_program, vs );
    glAttachShader ( d_program, fs );
    if (gs_used)
        glAttachShader( d_program, gs);
    

    glLinkProgram  ( d_program );
    glGetProgramiv ( d_program, GL_LINK_STATUS, &linked );
    glGetProgramiv ( d_program, GL_INFO_LOG_LENGTH, &log_size );

    if ( !linked )
    {   
        error_log = (char *)calloc( log_size,sizeof(char));
        glGetProgramInfoLog(d_program,log_size,&ret_len, error_log);
        std::string err_m = std::string(error_log);
        free(error_log);
        LOGE(std::string("Shader Link Failed, vfile:")+vprog+std::string(",ffile:")+fprog+std::string("\n")+err_m);
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
        LOGW(std::string("No uniform float ")+name + std::string(" in program: ") +v_pname + std::string(", ") + f_pname );
        return EFAIL;
    }

    glUniform1f ( loc,i);
    return 0;
}
int SProg::SetUniform(const std::string& name,int i) {
    int loc = LookupUniformLocation(name);
    if ( loc == EFAIL ) {
        LOGW(std::string("No uniform int(or sampler) ")+name + std::string(" in  program: ") +v_pname + std::string(", ") + f_pname );
        return EFAIL;
    }

    glUniform1i ( loc,i);
    return 0;
}
int SProg::SetUniform( const std::string& name ,const SMat4x4& mat )
{

    int loc = LookupUniformLocation(name);
    if ( loc == EFAIL ) {
        LOGW(string_format("No uniform mat4 \"%s\"", name.c_str()) + std::string(" in program: ") +v_pname + std::string(", ") + f_pname );
        return EFAIL;
    }


    /*gl use row-major notation*/
    glUniformMatrix4fv(loc, 1, GL_TRUE, (float *) mat.raw);

    return ESUCCESS;
}

int SProg::SetUniform(int loc, const SVec4 &vec)
{
    glUniform4fv(loc, 1, (float *)vec.raw);
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
    /*callback can slow program when gl report error repitily*/
    if (loc != EFAIL)
        /*gl use row-major notation*/
        glUniformMatrix4fv(loc, 1, GL_TRUE, (float *) mat.raw);
    return ESUCCESS;
}
int SProg::SetUniform(const std::string& name, const SVec4& vec)
{
    int loc = LookupUniformLocation(name);
    if ( loc == EFAIL ) {
        LOGW(string_format("No uniform vec4 \"%s\"", name.c_str())+ std::string(" in program: ") +v_pname + std::string(", ") + f_pname );
        return EFAIL;
    }
    glUniform4fv(loc, 1, (float *)vec.raw);

    return ESUCCESS;
}





int SProg::SetAttrib (const std::string& name, int numComponents, GLsizei stride, unsigned int offset, GLenum type )
{
    int loc = glGetAttribLocation ( d_program, name.c_str() );

    if ( loc < 0 )
    {
        LOGW("No Vertex Attibute Find in:"+ name + std::string(" in program: ") +v_pname + std::string(", ") + f_pname );
         return EFAIL;
    }   

    glVertexAttribPointer ( loc,                  // index
                            numComponents,        // number of intues per vertex
                           	type,                 // type (GL_FLOAT)
                            GL_FALSE,       /*fixed point data not used*/
                            stride,               // stride (offset to next vertex data)
                            (const GLvoid*) offset );
        
    glEnableVertexAttribArray ( loc );
    
    return ESUCCESS;
}

int SProg::SetAttrib(unsigned int location, int numComponents, GLsizei stride, unsigned int offset, GLenum type)
{
    glVertexAttribPointer ( location,                  // index
                            numComponents,        // number of intues per vertex
                            type,                 // type (GL_FLOAT)
                            GL_FALSE,       /*fixed point data not used*/
                            stride,               // stride (offset to next vertex data)
                            (const GLvoid*) offset );

    glEnableVertexAttribArray ( location );

    return ESUCCESS;
}
