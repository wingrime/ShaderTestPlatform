#include "r_perfman.h"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/wglew.h>
#include "ErrorCodes.h"
SPerfMan::SPerfMan() {
	glGenQueries(1,&q);
}
SPerfMan::~SPerfMan() {
	glDeleteQueries(1,&q);
}
int SPerfMan::Begin(){
     /*try save last result that should be avaliable from last frame ,
      *   if data is not aval, don't modify res
      */
    if (!d_first_run)
        glGetQueryObjectuiv(q, GL_QUERY_RESULT_NO_WAIT, &res);
    else
        d_first_run = false;
	glBeginQuery(GL_TIME_ELAPSED, q);
	return ESUCCESS;
}
int SPerfMan::End(){


	glEndQuery(GL_TIME_ELAPSED );
    glGetQueryObjectuiv(q, GL_QUERY_RESULT_NO_WAIT, &res);
	return ESUCCESS;
}
unsigned int SPerfMan::getTime(){
    //stall pipeline
    //glGetQueryObjectuiv(q, GL_QUERY_RESULT, &res);
	return res;
}
