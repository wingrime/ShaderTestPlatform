#include "r_cprog.h"
#include "ErrorCodes.h"
#include "Log.h"
#include "MAssert.h"

SSBuffer::SSBuffer(int sz) {
    UNUSED(sz);/*not impl*/
    glGenBuffers(1, &d_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,d_ssbo);
}
SCProg::SCProg(const std::string &src_file)
{
     AbstractBuffer * src = new FileBuffer(std::string(".\\shaders\\")+src_file);

    d_ready  = false;
    d_program = glCreateProgram();
    d_shader  = glCreateShader (GL_COMPUTE_SHADER);
    const char * source = (const char *) src->buffer();
    int len = strlen(source);

    glShaderSource (d_shader,1 , &source, &len);

    delete src; // bad

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
        LOGE(std::string("Compute shader build failed:\n")+e_log);
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
        LOGE(std::string("Compute shader link failed:\n")+e_log);
        return;
    }
    d_ready = true;
}



SCProg::~SCProg()
{
    //TODO
}
int SCProg::SetUniform(const std::string& name,int i) {
    int loc = LookupUniformLocation(name);
    if ( loc == EFAIL ) {
        LOGW("No uniform int(or sampler) in compute shader!" );
        return EFAIL;
    }

    glUniform1i ( loc,i);
    return 0;
}
int SCProg::LookupUniformLocation(const std::string &name) {
    int loc = glGetUniformLocation ( d_program, name.c_str() );
    return loc;
}

int SCProg::Use()
{
    glUseProgram      ( d_program );
    return ESUCCESS;
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
