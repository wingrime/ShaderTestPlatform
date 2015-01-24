#pragma once
#include "mat_math.h"
#include <cmath>
class SPerspectiveProjectionMatrix: public SMat4x4 {
public: 
    	SPerspectiveProjectionMatrix(float Near, float Far,float Aspect, float FOV) : SMat4x4() {
         
        //float FOV  = toRad(26.0);/* based on angle*/
        float focal_length = 1.0 / tan(  FOV / 2.0);


         a11 = focal_length;
         a22  = focal_length/Aspect;
         a33 = -( (Far + Near) / (Far - Near));
         a34 =  -((2 *Far * Near) / ( Far- Near));
         a43 = -1.0f;
         a44 = 0.0f;

         /*Orto*/
/*
         float f = 1000;
         float n = 1;
         float r = 1;
         float t = 1;
         float l = -1;
         float b = -1;
         a11 = 2*n / (r - l );
         a13 = (r+l)/ (r-l );
         a22  = 2*n/ (t -b);
         a23 = (t+b) / (t-b);
         a33 = -((f+n)/ (f-n));
         a34 =  -((2 *f*n) / ( f-n));
         a43 = -1.0f;
         a44 = 0.0f;
*/
         /*
        //infinity far;
         a11 = focal_length;
         a22  = (focal_length/Aspect);
         a33 = -1.0;
         a34 = -2.0* (1.0f);
         a43 = -1.0f;
         a44 = 0.0f;
         printf("proj\n");
         Reflect();
*/
    };
};

class SOrtoProjectionMatrix: public SMat4x4 {
public: 
        SOrtoProjectionMatrix(float Near, float Far) : SMat4x4() {
         

         /*Orto*/

         float f = Far;
         float n = Near;
         float r = 1;
         float t = 1;
         float l = -1;
         float b = -1;
         a11 = 2*n / (r - l );
         a13 = (r+l)/ (r-l );
         a22  = 2*n/ (t -b);
         a23 = (t+b) / (t-b);
         a33 = -((f+n)/ (f-n));
         a34 =  -((2 *f*n) / ( f-n));
         a43 = -1.0f;
         a44 = 0.0f;

    };
};
class SInfinityFarMatrix: public SMat4x4 {
public:
    SInfinityFarMatrix(float FOV) : SMat4x4() {

        float focal_length = 1.0 / tan(  FOV / 2.0);
         a11 = focal_length;
         a22  = (focal_length/FOV);
         a33 = -1.0;
         a34 = -2.0* (1.0f);
         a43 = -1.0f;

    };
};

/* http://www.cg.tuwien.ac.at/~scherzer/files/papers/LispSM_survey.pdf */
class SLispSMProjectionMatrix: public SMat4x4 {
public: 
        SLispSMProjectionMatrix(float Near, float Far) : SMat4x4() {
         

         /*Orto*/

         float f = Far;
         float n = Near;

         a22  = (f+n)/ (f-n);
         a42 =  -((2.0 *f*n) / ( f-n));
         a44 = 0.0;
         a24 = 1.0;
    };
};
