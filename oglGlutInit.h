#pragma once
#ifndef __OGL_INIT__
#define __OGL_INIT__
#include <GL/glew.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/wglew.h>
#include <windows.h>
#endif
#include "Log.h"

#ifndef __APPLE__
void APIENTRY openglCallbackFunction(GLenum source,
                                           GLenum type,
                                           GLuint id,
                                           GLenum severity,
                                           GLsizei length,
                                           const GLchar* message,
                                           const void* userParam){

 /* todo find  where it was called */
    UNUSED(source);
    UNUSED(type);
    UNUSED(id);
    UNUSED(userParam);
    UNUSED(length);
    static Log gl_log("gl_log.log");
    if (severity  != GL_DEBUG_SEVERITY_NOTIFICATION)
   {
        LOGW(message);
        /*Force breakpoint on GL debug message usable for finding bugs withing code
        Must not be enabled in production
        */
        //D_TRAP();
    }
}
#endif
int oglInit(int argc, char * argv [] , int w, int h , int ogl_major, int ogl_minor) {
    // initialize glut
    glutInit            ( &argc, argv );
    #ifndef __APPLE__
    glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    #else
    glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE );
    #endif
    glutInitWindowSize  ( w,h);
    // init modern opengl
    #ifndef __APPLE__
    glutInitContextVersion ( ogl_major, ogl_minor );
    glutInitContextFlags   ( GLUT_FORWARD_COMPATIBLE| GLUT_DEBUG);
    glutInitContextProfile ( GLUT_CORE_PROFILE  );
    #endif

    glutCreateWindow ("m_proj Shestacov Alexsey 2014-2015(c)" );
    glewExperimental = GL_TRUE; /*Required for some new glFunctions otherwice will crash*/
    GLenum err= glewInit ();
    if (err != GLEW_OK) {
        return -1;
    }
#ifndef __APPLE__
    if(glDebugMessageCallback){
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(openglCallbackFunction, nullptr);
        GLuint unusedIds = 0;
        glDebugMessageControl(GL_DONT_CARE,
            GL_DONT_CARE,
            GL_DONT_CARE,
            0,
            &unusedIds,
            true);
            }
#endif

    return 0 ;
}
#endif
