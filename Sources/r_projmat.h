#pragma once
#include "mat_math.h"
#include <cmath>
/*TODO add constructors based on frustrum planes*/

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

    };
};

class SOrtoProjectionMatrix: public SMat4x4 {
public: 
        /*
         * Here: n - near plane f - far plane
        */
        SOrtoProjectionMatrix(float n, float f,float r,float t, float l , float b) : SMat4x4() {
         a11 = 2.0 / (r-l); a12 = 0.0;         a13 = 0.0;       a14 = -(r+l)/(r-l);
         a21 = 0.0;         a22 = 2.0/(t-b);   a23 = 0.0;       a24 = -(t+b)/(t-b);
         a31 = 0.0;         a32 = 0.0;         a33 = -2.0/(f-n);a34 = -(f+n)/(f-n);
         a41 = 0.0;         a42 = 0.0;         a43 = 0.0;       a44 = 1.0;
    };
};
class SInfinityFarProjectionMatrix: public SMat4x4 {
public:
    SInfinityFarProjectionMatrix(float Near,float Aspect,float FOV) : SMat4x4() {

        float focal_length = 1.0 / tan(  FOV / 2.0);
         a11 = focal_length;
         a22  = focal_length/Aspect;
         a33 = -1;
         a34 =  -2 * Near;
         a43 = -1.0f;
         a44 = 0.0f;

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
