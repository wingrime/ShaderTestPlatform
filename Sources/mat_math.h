#pragma once
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <memory>
#include "e_base.h"
#include "string_format.h"
#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif
#include <cmath>
/*serialization*/
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>
#include <fstream>
#include "MAssert.h"
//TODO
// && operator
// == operator
//vector cross product can become unstable, such situations should be traced
template <typename T>
T inline toRad(T deg){
    return (M_PI/180.0f) * deg;
}
template <typename T>
T inline toDeg(T rad){
    return (180.0f/M_PI) * rad;
}

class UnitTestError {
public: 
    UnitTestError() : message("UNK") {puts("UNK ERROR \n");}
    UnitTestError(std::string msg) : message(msg) {puts(msg.c_str());}
    std::string message ;
};



class SVec4 {
public:
    SVec4(float x,float y, float z, float w);
    SVec4(const SVec4& v);
    SVec4();
    SVec4(const std::string& str);

    union vec {
        struct {
            float x;float y;float z;float w;
        };
        struct {
            float r;float g;float b;float a;
        };
        struct {
            float raw[4];
        };
    } vec;

    SVec4 operator*(const SVec4& v) const;
    SVec4 operator+(const SVec4& v) const;
    SVec4 operator -(const SVec4 &v) const;
    SVec4 Normalize() const;
    static SVec4 Normalize(const SVec4& a);

    static float Dot(const SVec4& a,const SVec4& b);
    static bool Eq(const SVec4& a,const SVec4& b);

    void Reflect() const;
    /* self test */
    static void utest();

    /*serialize support */
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar( vec);
    }


    static SVec4 Cross(const SVec4 &a, const SVec4 &b);

    float Length() const;
};
SVec4 SVec4::Normalize() const {
    float norm = Length();
    MASSERT(norm==0.0);
    return SVec4(vec.x/norm,vec.y/norm, vec.z/norm, vec.w/norm);
}

SVec4 SVec4::Normalize(const SVec4 &a)
{
    float l = a.Length();
    MASSERT(l==0.0);
    return SVec4(a.vec.x / l , a.vec.y /l , a.vec.z /l , a.vec.w / l);
}
/*just too stupid I need add some delta*/
bool SVec4::Eq(const SVec4& a,const SVec4& b) {
    return ( a.vec.x == b.vec.x && a.vec.y == b.vec.y && a.vec.z == b.vec.z && a.vec.w == b.vec.w);
}
float SVec4::Length() const{
    return sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z + + vec.w*vec.w);
}
float SVec4::Dot(const SVec4& a,const SVec4& b) {
    return (a.vec.x *b.vec.x + a.vec.y *b.vec.y + a.vec.z *b.vec.z);
}
SVec4::SVec4(){
    vec.x = 0.0f;
    vec.y = 0.0f;
    vec.z = 0.0f;
    vec.w = 0.0f;
}
SVec4::SVec4(const SVec4& v){
    vec.x = v.vec.x;
    vec.y = v.vec.y;
    vec.z = v.vec.z;
    vec.w = v.vec.w;
}
SVec4::SVec4(float x, float y, float z,float w){
    vec.x = x;
    vec.y = y;
    vec.z = z;
    vec.w = w;
}
SVec4::SVec4(const std::string& str){
    const char *s = str.c_str();
    int c = sscanf(s, " %f,%f,%f,%f ",
            &vec.x,&vec.y,&vec.z,&vec.w);
    if (c != 4 )
        throw VectorError(std::string("Unable construct vector from string:")+str);
}
/*not fully correct*/
/*for 4D you need 3 vectors to do cross!, but if we set 3rd to (0,0,0,1)*/
SVec4 SVec4::operator*(const SVec4& v) const {
return (SVec4(vec.y * v.vec.z - vec.z * v.vec.y,
             vec.z * v.vec.x - vec.x * v.vec.z,
             vec.x * v.vec.y - vec.y * v.vec.x,
            0.0f));
}
SVec4 SVec4::Cross(const SVec4& a, const SVec4& b)  {
return (SVec4(a.vec.y * b.vec.z - a.vec.z * b.vec.y,
              a.vec.z * b.vec.x - a.vec.x * b.vec.z,
              a.vec.x * b.vec.y - a.vec.y * b.vec.x,
              0.0f));
}

SVec4 SVec4::operator+(const SVec4& v) const {
return (SVec4(vec.x + v.vec.x,
              vec.y + v.vec.y,
              vec.z + v.vec.z,
              vec.w + v.vec.w));
}
SVec4 SVec4::operator-(const SVec4& v) const {
return (SVec4(vec.x - v.vec.x,
              vec.y - v.vec.y,
              vec.z - v.vec.z,
              vec.w - v.vec.w));
}



void SVec4::Reflect() const{
    printf("%f %f %f %f\n",vec.x,vec.y,vec.z,vec.w);
}




class SMat4x4 {
public:
    SMat4x4(float a11,float a12,float a13,float a14,
            float a21,float a22,float a23,float a24,
            float a31,float a32,float a33,float a34,
            float a41,float a42,float a43,float a44);
    
    SMat4x4(SVec4 a1,
            SVec4 a2,
            SVec4 a3,
            SVec4 a4);
    SMat4x4(const std::string& str);
    /*indent matrix fill*/
	SMat4x4() : SMat4x4(1.0f) {}

    
	SMat4x4(float a);
    SMat4x4( const SMat4x4& i);



    //reflect to stdout
    void Reflect() const;
    // out to string
    std::string ReflectStr() const;
    //simple self test
    static void utest();


    //internals
    union mat {
        struct {
            float a11;float a12;float a13;float a14;
            float a21;float a22;float a23;float a24;
            float a31;float a32;float a33;float a34;
            float a41;float a42;float a43;float a44;
        };
        struct {
        float raw[16];
        };
    } mat;

    //operators
    SMat4x4 operator+(const SMat4x4& i) const;
    SMat4x4 operator-(const SMat4x4& i) const;
    SMat4x4 operator*(const SMat4x4& i) const;

    // Move/Scale
    SMat4x4 Move(const float x,const float y,const float z) const;
    SMat4x4 Translate(const SVec4& vec) const;
    SMat4x4 Scale(const float x,const float y,const float z) const;
    //rotation
    SMat4x4 RotX(const float ang) const;
	SMat4x4 RotY(const float ang) const;
    //SMat4x4 RotZ(const float ang) const;
	//transpose
	SMat4x4 Transpose() const;
	//det
	float Det() const;
	//inverse
	SMat4x4 Inverse() const;

    //check

    static bool Eq(const SMat4x4& a,const SMat4x4& b);
    
    virtual ~SMat4x4();

    /*serialize support */
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar( CEREAL_NVP(mat.a11),CEREAL_NVP(mat.a12),CEREAL_NVP(mat.a13),CEREAL_NVP(mat.a14),
           CEREAL_NVP(mat.a21),CEREAL_NVP(mat.a22),CEREAL_NVP(mat.a23),CEREAL_NVP(mat.a24),
           CEREAL_NVP(mat.a31),CEREAL_NVP(mat.a32),CEREAL_NVP(mat.a33),CEREAL_NVP(mat.a34),
           CEREAL_NVP(mat.a41),CEREAL_NVP(mat.a42),CEREAL_NVP(mat.a43),CEREAL_NVP(mat.a44));
    }

    template <typename T>
    SMat4x4 RotZ(T ang) const;
};

bool SMat4x4::Eq(const SMat4x4& a,const SMat4x4& b){
    return (!memcmp(a.mat.raw,b.mat.raw,sizeof(float)*16));
}

SMat4x4::~SMat4x4(){
    //printf("~mat\n");
    //nothing to do
};


SMat4x4 SMat4x4::operator+(const SMat4x4& i) const {
return (SMat4x4(mat.a11+i.mat.a11,mat.a12+i.mat.a12,mat.a13+i.mat.a13,mat.a14+i.mat.a14,
               mat.a21+i.mat.a21,mat.a22+i.mat.a22,mat.a23+i.mat.a23,mat.a24+i.mat.a24,
               mat.a31+i.mat.a31,mat.a32+i.mat.a32,mat.a33+i.mat.a33,mat.a34+i.mat.a34,
               mat.a41+i.mat.a41,mat.a42+i.mat.a42,mat.a43+i.mat.a43,mat.a44+i.mat.a44
               ));    
}
SMat4x4 SMat4x4::operator-(const SMat4x4& i) const {
return (SMat4x4(mat.a11-i.mat.a11,mat.a12-i.mat.a12,mat.a13-i.mat.a13,mat.a14-i.mat.a14,
               mat.a21-i.mat.a21,mat.a22-i.mat.a22,mat.a23-i.mat.a23,mat.a24-i.mat.a24,
               mat.a31-i.mat.a31,mat.a32-i.mat.a32,mat.a33-i.mat.a33,mat.a34-i.mat.a34,
               mat.a41-i.mat.a41,mat.a42-i.mat.a42,mat.a43-i.mat.a43,mat.a44-i.mat.a44
               ));    
}
SMat4x4 SMat4x4::operator*(const SMat4x4& i) const {
    return (SMat4x4(
                mat.a11 * i.mat.a11 + mat.a12 * i.mat.a21 + mat.a13 * i.mat.a31 + mat.a14 * i.mat.a41, 
                mat.a11 * i.mat.a12 + mat.a12 * i.mat.a22 + mat.a13 * i.mat.a32 + mat.a14 * i.mat.a42, 
                mat.a11 * i.mat.a13 + mat.a12 * i.mat.a23 + mat.a13 * i.mat.a33 + mat.a14 * i.mat.a43,              
                mat.a11 * i.mat.a14 + mat.a12 * i.mat.a24 + mat.a13 * i.mat.a34 + mat.a14 * i.mat.a44, 
                
                mat.a21 * i.mat.a11 + mat.a22 * i.mat.a21 + mat.a23 * i.mat.a31 + mat.a24 * i.mat.a41,              
                mat.a21 * i.mat.a12 + mat.a22 * i.mat.a22 + mat.a23 * i.mat.a32 + mat.a24 * i.mat.a42, 
                mat.a21 * i.mat.a13 + mat.a22 * i.mat.a23 + mat.a23 * i.mat.a33 + mat.a24 * i.mat.a43, 
                mat.a21 * i.mat.a14 + mat.a22 * i.mat.a24 + mat.a23 * i.mat.a34 + mat.a24 * i.mat.a44, 

                mat.a31 * i.mat.a11 + mat.a32 * i.mat.a21 + mat.a33 * i.mat.a31 + mat.a34 * i.mat.a41, 
                mat.a31 * i.mat.a12 + mat.a32 * i.mat.a22 + mat.a33 * i.mat.a32 + mat.a34 * i.mat.a42, 
                mat.a31 * i.mat.a13 + mat.a32 * i.mat.a23 + mat.a33 * i.mat.a33 + mat.a34 * i.mat.a43, 
                mat.a31 * i.mat.a14 + mat.a32 * i.mat.a24 + mat.a33 * i.mat.a34 + mat.a34 * i.mat.a44, 

                mat.a41 * i.mat.a11 + mat.a42 * i.mat.a21 + mat.a43 * i.mat.a31 + mat.a44 * i.mat.a41, 
                mat.a41 * i.mat.a12 + mat.a42 * i.mat.a22 + mat.a43 * i.mat.a32 + mat.a44 * i.mat.a42, 
                mat.a41 * i.mat.a13 + mat.a42 * i.mat.a23 + mat.a43 * i.mat.a33 + mat.a44 * i.mat.a43,               
                mat.a41 * i.mat.a14 + mat.a42 * i.mat.a24 + mat.a43 * i.mat.a34 + mat.a44 * i.mat.a44
                ));
}


void SMat4x4::Reflect() const{
    printf("%f %f %f %f\n",mat.a11,mat.a12,mat.a13,mat.a14);
    printf("%f %f %f %f\n",mat.a21,mat.a22,mat.a23,mat.a24);
    printf("%f %f %f %f\n",mat.a31,mat.a32,mat.a33,mat.a34);
    printf("%f %f %f %f\n",mat.a41,mat.a42,mat.a43,mat.a44);
}
std::string SMat4x4::ReflectStr() const{
    
    return string_format("Matrix4x4\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n", 
                            mat.a11,mat.a12,mat.a13,mat.a14,
                            mat.a21,mat.a22,mat.a23,mat.a24, 
                            mat.a31,mat.a32,mat.a33,mat.a34, 
                            mat.a41,mat.a42,mat.a43,mat.a44 );
}

SMat4x4::SMat4x4(SVec4 a1,
                 SVec4 a2,
                 SVec4 a3,
                 SVec4 a4 ) {

    mat.a11 = a1.vec.x; mat.a12 = a2.vec.x;mat.a13 = a3.vec.x; mat.a14 = a4.vec.x;
    mat.a21 = a1.vec.y; mat.a22 = a2.vec.y;mat.a23 = a3.vec.y; mat.a24 = a4.vec.y;
    mat.a31 = a1.vec.z; mat.a32 = a2.vec.z;mat.a33 = a3.vec.z; mat.a34 = a4.vec.z;
    mat.a41 = a1.vec.w; mat.a42 = a2.vec.w;mat.a43 = a3.vec.w; mat.a44 = a4.vec.w;
    //printf("mat\n");
}
SMat4x4::SMat4x4(const std::string &str) {
    const char *s = str.c_str();
    int c = sscanf(s, " %f %f %f %f ; %f %f %f %f ; %f %f %f %f ; %f %f %f %f ", 
            &mat.a11,&mat.a12,&mat.a13,&mat.a14,
            &mat.a21,&mat.a22,&mat.a23,&mat.a24,
            &mat.a31,&mat.a32,&mat.a33,&mat.a34,
            &mat.a41,&mat.a42,&mat.a43,&mat.a44);
    if (c != 16 )
        throw MatrixError(std::string("Unable construct matrix from string:")+str);
}

SMat4x4::SMat4x4(float v) {

    mat.a11 = v;    mat.a12 = 0.0f; mat.a13 = 0.0f; mat.a14 = 0.0f;
    mat.a21 = 0.0f; mat.a22 = v;    mat.a23 = 0.0f; mat.a24 = 0.0f;
    mat.a31 = 0.0f; mat.a32 = 0.0f; mat.a33 = v;    mat.a34 = 0.0f;
    mat.a41 = 0.0f; mat.a42 = 0.0f; mat.a43 = 0.0f; mat.a44 = v;
    //printf("mat\n");
}
SMat4x4::SMat4x4(float a11,float a12,float a13,float a14,
            float a21,float a22,float a23,float a24,
            float a31,float a32,float a33,float a34,
            float a41,float a42,float a43,float a44){

    mat.a11 = a11; mat.a12 = a12; mat.a13 = a13; mat.a14 = a14;
    mat.a21 = a21; mat.a22 = a22; mat.a23 = a23; mat.a24 = a24;
    mat.a31 = a31; mat.a32 = a32; mat.a33 = a33; mat.a34 = a34;
    mat.a41 = a41; mat.a42 = a42; mat.a43 = a43; mat.a44 = a44;
    //printf("mat\n");
}

SMat4x4::SMat4x4( const SMat4x4& i){
    mat.a11 = i.mat.a11; mat.a12 = i.mat.a12; mat.a13 = i.mat.a13; mat.a14 = i.mat.a14;
    mat.a21 = i.mat.a21; mat.a22 = i.mat.a22; mat.a23 = i.mat.a23; mat.a24 = i.mat.a24;
    mat.a31 = i.mat.a31; mat.a32 = i.mat.a32; mat.a33 = i.mat.a33; mat.a34 = i.mat.a34;
    mat.a41 = i.mat.a41; mat.a42 = i.mat.a42; mat.a43 = i.mat.a43; mat.a44 = i.mat.a44;
    //printf("mat\n");
}

SMat4x4 SMat4x4::Move(float x,float y,float z) const{
    SMat4x4 a =  SMat4x4();
    a.mat.a14 = x;
    a.mat.a24 = y;
    a.mat.a34 = z;
    return (a)*(* this); 
}

SMat4x4 SMat4x4::Translate(const SVec4& vec) const{
    SMat4x4 a =  SMat4x4();
    a.mat.a14 = vec.vec.x;
    a.mat.a24 = vec.vec.y;
    a.mat.a34 = vec.vec.z;
    a.mat.a44 = vec.vec.w;
    return (a)*(* this); 
}

SMat4x4 SMat4x4::Scale(float x,float y,float z) const {
    SMat4x4 a =  SMat4x4();
    a.mat.a11 = x;
    a.mat.a22 = y;
    a.mat.a33 = z;
    return (a)*(* this); 
}


SMat4x4 SMat4x4::RotX(float ang) const{
    SMat4x4 op =  SMat4x4();
    float ang_c,ang_s;
    ang_c = cos(ang);
    ang_s = sin (ang);
    op.mat.a22 = ang_c;
    op.mat.a23 =-ang_s;
    op.mat.a32 = ang_s; 
    op.mat.a33 = ang_c;
    return (op)*(*this); 
}

SMat4x4 SMat4x4::RotY(float ang) const{
    SMat4x4 op =  SMat4x4();
    float ang_c,ang_s;
    ang_c = cos(ang);
    ang_s = sin(ang);
    op.mat.a11 = ang_c;
    op.mat.a31 = -ang_s;
    op.mat.a13 = ang_s; 
    op.mat.a33 =  ang_c;
    return (op)*(*this); 
}
template <typename T>
SMat4x4 SMat4x4::RotZ(T ang) const{
    SMat4x4 op =  SMat4x4();
    T ang_c,ang_s;
    ang_c = cos(ang);
    ang_s = sin (ang);
    op.mat.a11 = ang_c;
    op.mat.a12 = -ang_s;
    op.mat.a22 = ang_c; 
    op.mat.a21 = ang_s;
    return (op)*(*this); 
}
SMat4x4 SMat4x4::Transpose() const {
	return (SMat4x4(mat.a11,mat.a21,mat.a31,mat.a41, 
								mat.a12,mat.a22,mat.a32,mat.a42,
								mat.a13,mat.a23,mat.a33,mat.a43,
								mat.a14,mat.a24,mat.a34,mat.a44));
}




SVec4 operator* (const SMat4x4& m, const SVec4& v){
    return SVec4(m.mat.a11 * v.vec.x + m.mat.a12 * v.vec.y + m.mat.a13 * v.vec.z+ m.mat.a14 * v.vec.w ,
                 m.mat.a21 * v.vec.x + m.mat.a22 * v.vec.y + m.mat.a23 * v.vec.z+ m.mat.a24 * v.vec.w ,
                 m.mat.a31 * v.vec.x + m.mat.a32 * v.vec.y + m.mat.a33 * v.vec.z+ m.mat.a34 * v.vec.w ,
                 m.mat.a41 * v.vec.x + m.mat.a42 * v.vec.y + m.mat.a43 * v.vec.z+ m.mat.a44 * v.vec.w );

}
template <typename T>
SVec4 operator/ (const SVec4& v,T num){
    MASSERT(num = 0.0);
    return SVec4(v.vec.x / num , v.vec.y / num  , v.vec.z/ num , v.vec.w/ num);
}

#define CHECK_QU() {if (!CheckUnit()) EMSGS("UnitQuaterion not unit lenght!")}
class UnitQuaterion {
public:
    UnitQuaterion();
    UnitQuaterion(float x,float y, float z,float w);
    UnitQuaterion(const UnitQuaterion& quat);
    UnitQuaterion(const SVec4& v, float fi);

    UnitQuaterion(const SMat4x4& m); /*TODO*/
    UnitQuaterion(const std::string& str); /*TODO */

    UnitQuaterion Conjugate() const;

    UnitQuaterion Inverse() const;

    UnitQuaterion Normalize() const;
    float Norm() const;

    UnitQuaterion Rotate(const SVec4& axis, float fi) const;
    UnitQuaterion Rotate(const UnitQuaterion& qaxis) const;
    SMat4x4 toMatrix() const;



    static UnitQuaterion SLERP(const UnitQuaterion& a,const UnitQuaterion& b, float t);


    union q {
        struct {
            float x;float y;float z;float w;
        };
        struct {
            float raw[4];
        };
    } q ;

    bool Eq(const UnitQuaterion& a,const UnitQuaterion& b) const;
    bool CheckUnit() const;
    void Reflect() const;
    UnitQuaterion operator*(const UnitQuaterion& qt) const;
    UnitQuaterion operator*(float s) const;
    UnitQuaterion operator+(const UnitQuaterion& qt) const;
    constexpr static float CheckDelta = 1e-6;
};
UnitQuaterion UnitQuaterion::Normalize() const {
    float norm = Norm();
    return UnitQuaterion(q.x/norm,q.y/norm, q.z/norm, q.w/norm);
}
bool UnitQuaterion::CheckUnit() const {
   return ((Norm()-1.0f) < CheckDelta);
} 
void UnitQuaterion::Reflect() const{
    printf("UnitQuaterion: %f %f %f %f\n",q.x,q.y,q.z,q.w);
}
UnitQuaterion::UnitQuaterion(const UnitQuaterion& quat) {
    q.x = quat.q.x;
    q.y = quat.q.y;
    q.z = quat.q.z;
    q.w = quat.q.w;
    CHECK_QU();
}
UnitQuaterion UnitQuaterion::SLERP(const UnitQuaterion& a,const UnitQuaterion& b, float t) {
    float cos_fi = a.q.x * b.q.x + a.q.y * b.q.y + a.q.z * b.q.z + a.q.w * b.q.w;
    float fi  = acos(cos_fi);
    float new_a = sin(fi*(1-t)) / sin(fi);
    float new_b = sin(fi*t)/ sin(fi);
    return UnitQuaterion(a)*new_a + UnitQuaterion(b)*new_b;
}
/*Simplifed for Unit quaterion*/
SMat4x4 UnitQuaterion::toMatrix() const {
    return SMat4x4( 1.0f-2.0f*(q.y*q.y + q.z*q.z) ,     2.0f*(q.x*q.y + q.w*q.z) ,          2.0f*(q.x*q.z - q.w*q.y) , 0.0f, 
                         2.0f*(q.x*q.y - q.w*q.z) ,1.0f-2.0f*(q.x*q.x + q.z*q.z) ,          2.0f*(q.y*q.z + q.w*q.x) , 0.0f,
                         2.0f*(q.x*q.z + q.w*q.y) ,     2.0f*(q.y*q.z - q.w*q.x) ,     1.0f-2.0f*(q.x*q.x + q.y*q.y) , 0.0f,
                         0.0f                     ,     0.0f                     ,          0.0f                     , 1.0f);
}
/* WARNING: vector.w will be ignored */
UnitQuaterion UnitQuaterion::Rotate(const SVec4& axis, float fi) const {
    return (*this)*UnitQuaterion(axis,fi/2)*(*this).Inverse();
}
UnitQuaterion UnitQuaterion::Rotate(const UnitQuaterion& qaxis) const {
    return (*this)*qaxis*((*this).Inverse());
}
/* Should be one as it UnitQuaterion , but sometime it need to be relormal*/
float UnitQuaterion::Norm() const {
    return sqrt(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
}
bool UnitQuaterion::Eq(const UnitQuaterion& a,const UnitQuaterion& b) const {
    return ( a.q.x == b.q.x && a.q.y == b.q.y && a.q.z == b.q.z && a.q.w == b.q.w);
}
UnitQuaterion::UnitQuaterion(){
    q.x = 0.0f;
    q.y = 0.0f;
    q.z = 0.0f;
    q.w = 1.0f;
}
UnitQuaterion::UnitQuaterion(float x, float y, float z,float w){
    q.x = x;
    q.y = y;
    q.z = z;
    q.w = w;
    CHECK_QU();
}
/* WARNING: vector.w will be ignored */
UnitQuaterion::UnitQuaterion(const SVec4& v, float fi){
    float sin_fi = sin(fi/2);
    q.x = sin_fi*v.vec.x;
    q.y = sin_fi*v.vec.y;
    q.z = sin_fi*v.vec.z;
    q.w = cos(fi/2);
    CHECK_QU();
}
UnitQuaterion UnitQuaterion::operator*(float s) const {
    return (UnitQuaterion(q.x*s , q.y*s , q.z*s, q.w*s ) );
}
UnitQuaterion UnitQuaterion::operator*(const UnitQuaterion& v) const {

    float cross_x = q.y * v.q.z - q.z * v.q.y;
    float cross_y = q.z * v.q.x - q.x * v.q.z;
    float cross_z = q.x * v.q.y - q.y * v.q.x;

    cross_x += v.q.w*q.x  + q.w*v.q.x;
    cross_y += v.q.w*q.y  + q.w*v.q.y;
    cross_z += v.q.w*q.z  + q.w*v.q.z;

    float dot = q.x *v.q.x + v.q.y *q.y + v.q.z *q.z;

    return (UnitQuaterion(cross_x, cross_y , cross_z , q.w * v.q.w - dot));
}

UnitQuaterion UnitQuaterion::operator + (const UnitQuaterion& v) const {
    return (UnitQuaterion(q.x + v.q.x , q.y + v.q.y, q.z + v.q.z, q.w + v.q.w));
}
UnitQuaterion UnitQuaterion::Conjugate() const{
    return UnitQuaterion(-q.x ,-q.y,-q.z , q.w);
}
/* for unit quaterion same as conjugate, in general case it should be renormalized*/
UnitQuaterion UnitQuaterion::Inverse() const{
    return UnitQuaterion(-q.x ,-q.y,-q.z , q.w);
}
#define U_TEST_START(nm) int _ut_number = 0; int _ut_error = 0; printf("UnitTest: %s\n",nm)
#define U_TEST(test_name,res1,res2) printf(" Test %d : %s\n ",_ut_number++,test_name); if (res1 == res2) printf("PASS\n"); else { _ut_error++;printf("FAIL\n");}
#define U_TEST_REPORT printf("Test results\n ------------\n "); printf(" %s: %d/%d\n",_ut_error?"FAILED":"PASSED" , (_ut_number - _ut_error),_ut_number);

void SVec4::utest() {
    U_TEST_START("SVec4");
    U_TEST("Eq", SVec4::Eq(SVec4(),SVec4()),true );
    U_TEST("Eq", SVec4::Eq(SVec4(1,2,3,4),SVec4(1,2,3,4)),true );
    U_TEST("Eq", SVec4::Eq(SVec4(4,3,2,1),SVec4(1,2,3,4)),false );
    U_TEST("CrossProduct", SVec4::Eq(SVec4(1,2,3,0)*SVec4(3,2,1,0),SVec4(-4,8,-4,0)),true);
    U_TEST("CrossProduct", SVec4::Eq(SVec4(3,2,1,0)*SVec4(1,2,3,0),SVec4(4,-8,4,0)),true);
    U_TEST_REPORT;
}
void SMat4x4::utest()
{
    U_TEST_START("SMat4x4");
    U_TEST("Eq",SMat4x4::Eq(SMat4x4(),SMat4x4()),true);
    U_TEST("Eq",SMat4x4::Eq(SMat4x4(),SMat4x4(2.0)),false);
    SMat4x4().Reflect();
    U_TEST("Eq",SMat4x4::Eq(SMat4x4(2.0),SMat4x4(2.0)),true);
    U_TEST("Eq",SMat4x4::Eq(SMat4x4(),SMat4x4(2.0)),false);

    U_TEST("Add",SMat4x4::Eq(SMat4x4(1.0)+SMat4x4(1.0),SMat4x4(2.0)),true);
    U_TEST("Mul",SMat4x4::Eq(SMat4x4(1.0)*SMat4x4(1.0),SMat4x4(1.0)),true);
    U_TEST("Move",SMat4x4::Eq(SMat4x4(1.0).Move(1.0,2.0,3.0),SMat4x4(1.0)),false);
    U_TEST("Move",SMat4x4::Eq(SMat4x4(1.0).Move(1.0,2.0,3.0),SMat4x4(1,0,0,0 ,0,1,0,0 ,0,0,1,0 , 1,2,3,1 )),true);
    U_TEST("Move",SMat4x4::Eq(SMat4x4(1.0).Move(1.0,2.0,3.0).Move(-1.0,-2.0,-3.0),SMat4x4(1.0)),true);
    //U_TEST("Rotate",SMat4x4::Eq(SMat4x4(1.0).RotX(1.0).RotX(-1.0),SMat4x4(1.0)),true);
	U_TEST("Mul",SMat4x4::Eq(SMat4x4(1,2,3,4, 5,6,7,8,9, 10,11,12, 13,14,15,16)*SMat4x4(1,2,3,4, 5,6,7,8,9, 10,11,12, 13,14,15,16), \
								SMat4x4(90,100,110,120,202,228,254,280,314,356,398,440,426,484,542,600)),true);

	U_TEST("Mul",SMat4x4::Eq(SMat4x4(1,2,3,4, 5,6,7,8,9, 10,11,12, 13,14,15,16)*SMat4x4(16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1), \
								SMat4x4(80,70,60,50,240,214,188,162,400,358,316,274,560,502,444,386)),true);

	U_TEST("Mul",SMat4x4::Eq(SMat4x4(16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1)*SMat4x4(1,2,3,4, 5,6,7,8,9, 10,11,12, 13,14,15,16), \
								SMat4x4(386,444,502,560,274,316,358,400,162,188,214,240,50,60,70,80)),true);

	U_TEST("Transpose",SMat4x4::Eq(SMat4x4(1,2,3,4, 5,6,7,8,9, 10,11,12, 13,14,15,16).Transpose(), \
								SMat4x4(1,5,9,13,2,6,10,14,3,7,11,15,4,8,12,16)),true);


    U_TEST_REPORT;

}
