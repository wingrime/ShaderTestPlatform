#pragma once
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/wglew.h>

int oglInit(int argc, char * argv [] , int w, int h , int ogl_major, int ogl_minor) {
    // initialize glut
    glutInit            ( &argc, argv );
    glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize  ( w,h);
    // init modern opengl
    glutInitContextVersion ( ogl_major, ogl_minor );
    glutInitContextFlags   ( GLUT_FORWARD_COMPATIBLE| GLUT_DEBUG);
    glutInitContextProfile ( GLUT_CORE_PROFILE  );

    glutCreateWindow ("m_proj Shestacov Alexsey 2014-2015(c)" );
    glewExperimental = GL_TRUE; /*Required for some new glFunctions otherwice will crash*/
    GLenum err= glewInit ();
    if (err != GLEW_OK) {
        return -1;
    }
    return 0 ;
}
