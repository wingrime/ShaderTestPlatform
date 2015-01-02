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
	glBeginQuery(GL_TIME_ELAPSED, q);
	return ESUCCESS;
}
int SPerfMan::End(){
	glEndQuery(GL_TIME_ELAPSED );
	glGetQueryObjectuiv(q, GL_QUERY_RESULT, &res);
	if ( res > mtime)
		mtime = res;
	avgtime = (avgtime + res )/ 2.0; /*Moving average*/
	return ESUCCESS;
}
unsigned int SPerfMan::getTime(){
	return res;
}
unsigned int SPerfMan::getMaxTime(){
	return mtime;
}
unsigned int SPerfMan::getAvgTime(){
	return avgtime;
}