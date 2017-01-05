#pragma once
#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <GL/glew.h>
#include <OpenGL/gl3.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/wglew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

#endif
 