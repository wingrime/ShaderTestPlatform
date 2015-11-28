#pragma once
/*OpenGL performance monitor*/
class SPerfMan {
public: 
	SPerfMan();
	~SPerfMan();

	int Begin();
	int End();

	unsigned int getTime();
private:
	unsigned int res; /* nanoseconds*/
	unsigned int q;

    bool d_first_run = true;
};

