#include <math.h>
#include <stdio.h>
#include <string.h>
#include <memory>
#include <cmath>
#include "string_format.h"
#include "mat_math.h"


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

bool SVec4::Eq(const SVec4& a,const SVec4& b) {
    return ( fabs(a.vec.x - b.vec.x) < 0.0001 && fabs(a.vec.y - b.vec.y) < 0.0001 && fabs(a.vec.z - b.vec.z) < 0.0001 && fabs(a.vec.w - b.vec.w) < 0.0001);
}
float SVec4::Length() const{
    return sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z  + vec.w*vec.w);
}
float SVec4::Dot(const SVec4& a,const SVec4& b) {
    return (a.vec.x *b.vec.x + a.vec.y *b.vec.y + a.vec.z *b.vec.z + a.vec.w*a.vec.w);
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
    //if (c != 4 )
        // better error handling
        //throw VectorError(std::string("Unable construct vector from string:")+str);
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



SMat4x4::~SMat4x4(){
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
   // if (c != 16 )
   //     throw MatrixError(std::string("Unable construct matrix from string:")+str);
    //better error handing
}

SMat4x4::SMat4x4(float v) {

    mat.a11 = v;    mat.a12 = 0.0f; mat.a13 = 0.0f; mat.a14 = 0.0f;
    mat.a21 = 0.0f; mat.a22 = v;    mat.a23 = 0.0f; mat.a24 = 0.0f;
    mat.a31 = 0.0f; mat.a32 = 0.0f; mat.a33 = v;    mat.a34 = 0.0f;
    mat.a41 = 0.0f; mat.a42 = 0.0f; mat.a43 = 0.0f; mat.a44 = v;
}
SMat4x4::SMat4x4(float a11,float a12,float a13,float a14,
            float a21,float a22,float a23,float a24,
            float a31,float a32,float a33,float a34,
            float a41,float a42,float a43,float a44){

    mat.a11 = a11; mat.a12 = a12; mat.a13 = a13; mat.a14 = a14;
    mat.a21 = a21; mat.a22 = a22; mat.a23 = a23; mat.a24 = a24;
    mat.a31 = a31; mat.a32 = a32; mat.a33 = a33; mat.a34 = a34;
    mat.a41 = a41; mat.a42 = a42; mat.a43 = a43; mat.a44 = a44;
}

SMat4x4::SMat4x4( const SMat4x4& i){
    mat.a11 = i.mat.a11; mat.a12 = i.mat.a12; mat.a13 = i.mat.a13; mat.a14 = i.mat.a14;
    mat.a21 = i.mat.a21; mat.a22 = i.mat.a22; mat.a23 = i.mat.a23; mat.a24 = i.mat.a24;
    mat.a31 = i.mat.a31; mat.a32 = i.mat.a32; mat.a33 = i.mat.a33; mat.a34 = i.mat.a34;
    mat.a41 = i.mat.a41; mat.a42 = i.mat.a42; mat.a43 = i.mat.a43; mat.a44 = i.mat.a44;
}
bool SMat4x4::Eq(const SMat4x4& a,const SMat4x4& b){
    return (!memcmp(a.mat.raw,b.mat.raw,sizeof(float)*16));
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
SMat4x4 SMat4x4::RotZ(float ang) const{
    SMat4x4 op =  SMat4x4();
    float ang_c,ang_s;
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

//#define CHECK_QU() {if (!CheckUnit()) EMSGS("UnitQuaterion not unit lenght!")}

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
   // CHECK_QU();
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
    //CHECK_QU();
}
/* WARNING: vector.w will be ignored */
UnitQuaterion::UnitQuaterion(const SVec4& v, float fi){
    float sin_fi = sin(fi/2);
    q.x = sin_fi*v.vec.x;
    q.y = sin_fi*v.vec.y;
    q.z = sin_fi*v.vec.z;
    q.w = cos(fi/2);
    //CHECK_QU();
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
