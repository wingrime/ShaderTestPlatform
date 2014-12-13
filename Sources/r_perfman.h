#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/wglew.h>
#include "e_base.h"
/*OpenGL performant monitor*/
class SPerfMan {
public: 
	SPerfMan();
	~SPerfMan();

	int Begin();
	int End();

	unsigned int getTime();
	unsigned int getMaxTime();
	unsigned int getAvgTime();
private:
	GLuint res; /* nanoseconds*/
	GLuint mtime = 0;
	GLuint avgtime = 0 ;
	GLuint q;
};
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