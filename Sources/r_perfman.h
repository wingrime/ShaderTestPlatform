#pragma once
/*OpenGL performance monitor*/
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
	unsigned int res; /* nanoseconds*/
	unsigned int mtime = 0;
	unsigned int avgtime = 0 ;
	unsigned int q;
};

