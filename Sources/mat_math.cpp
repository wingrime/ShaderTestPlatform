#include <math.h>
#include <stdio.h>
#include <string.h>
#include <memory>
#include <cmath>
#include "string_format.h"
#include "mat_math.h"
#include "MAssert.h"

SVec4 SVec4::Normalize() const {
    float norm = Length();
    MASSERT(norm==0.0);
    return SVec4(x/norm,y/norm, z/norm, w/norm);
}

SVec4 SVec4::Normalize(const SVec4 &a)
{
    float l = a.Length();
    MASSERT(l==0.0);
    return SVec4(a.x / l , a.y /l , a.z /l , a.w / l);
}

bool SVec4::Eq(const SVec4& a,const SVec4& b) {
    return ( fabs(a.x - b.x) < 0.0001 && fabs(a.y - b.y) < 0.0001 && fabs(a.z - b.z) < 0.0001 && fabs(a.w - b.w) < 0.0001);
}
float SVec4::Length() const{
    return sqrt(x*x + y*y + z*z  + w*w);
}
float SVec4::Dot(const SVec4& a,const SVec4& b) {
    return (a.x *b.x + a.y *b.y + a.z *b.z + a.w*a.w);
}
SVec4::SVec4(){
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
    w = 0.0f;
}
SVec4::SVec4(const SVec4& v){
    x = v.x;
    y = v.y;
    z = v.z;
    w = v.w;
}
SVec4::SVec4(float _x, float _y, float _z,float _w){
    x = _x;
    y = _y;
    z = _z;
    w = _w;
}
SVec4::SVec4(const std::string& str){
    const char *s = str.c_str();
    int c = sscanf(s, " %f,%f,%f,%f ",
            &x,&y,&z,&w);
    MASSERT(c != 4);

    //if (c != 4 )
        // better error handling
        //throw VectorError(std::string("Unable construct vector from string:")+str);
}
/*not fully correct*/
/*for 4D you need 3 vectors to do cross!, but if we set 3rd to (0,0,0,1)*/
//SVec4 SVec4::operator*(const SVec4& v) const {
//return (SVec4(vec.y * v.z - vec.z * v.y,
//             vec.z * v.x - vec.x * v.z,
//             vec.x * v.y - vec.y * v.x,
//            0.0f));
//}
SVec4 SVec4::Cross3(const SVec4& a, const SVec4& b)  {
return (SVec4(a.y * b.z - a.z * b.y,
              a.z * b.x - a.x * b.z,
              a.x * b.y - a.y * b.x,
              0.0f));
}

SVec4 SVec4::operator+(const SVec4& v) const {
return (SVec4(x + v.x,
              y + v.y,
              z + v.z,
              w + v.w));
}

SVec4 operator-(const SVec4& v1,const SVec4& v2){
return (SVec4(v1.x - v2.x,
              v1.y - v2.y,
              v1.z - v2.z,
              v1.w - v2.w));
}



void SVec4::Reflect() const{
    printf("%f %f %f %f\n",x,y,z,w);
}

void SVec4::DivW()
{
    MASSERT(w == 0.0);//LOGE("Attempt SVec4::DivW when w is zero, nothing done");
    x /= w;
    y /= w;
    z /= w;
    w = 1.0;

}

void SVec4::Abs()
{
    x = fabs(x);
    y = fabs(y);
    z = fabs(z);
    w = fabs(w);

}



SMat4x4::~SMat4x4(){
}


SMat4x4 SMat4x4::operator+(const SMat4x4& i) const {
return (SMat4x4(a11+i.a11,a12+i.a12,a13+i.a13,a14+i.a14,
               a21+i.a21,a22+i.a22,a23+i.a23,a24+i.a24,
               a31+i.a31,a32+i.a32,a33+i.a33,a34+i.a34,
               a41+i.a41,a42+i.a42,a43+i.a43,a44+i.a44
               ));    
}
SMat4x4 SMat4x4::operator-(const SMat4x4& i) const {
return (SMat4x4(a11-i.a11,a12-i.a12,a13-i.a13,a14-i.a14,
               a21-i.a21,a22-i.a22,a23-i.a23,a24-i.a24,
               a31-i.a31,a32-i.a32,a33-i.a33,a34-i.a34,
               a41-i.a41,a42-i.a42,a43-i.a43,a44-i.a44
                ));
}

SMat4x4 SMat4x4::Move(const Point &p) const
{
    SMat4x4 a =  SMat4x4(0.0);
    a.a14 = p.x;
    a.a24 = p.y;
    a.a34 = p.z;
    return (* this)+a;

}
SMat4x4 SMat4x4::operator*(const SMat4x4& i) const {
    return (SMat4x4(
                a11 * i.a11 + a12 * i.a21 + a13 * i.a31 + a14 * i.a41,
                a11 * i.a12 + a12 * i.a22 + a13 * i.a32 + a14 * i.a42,
                a11 * i.a13 + a12 * i.a23 + a13 * i.a33 + a14 * i.a43,
                a11 * i.a14 + a12 * i.a24 + a13 * i.a34 + a14 * i.a44,
                
                a21 * i.a11 + a22 * i.a21 + a23 * i.a31 + a24 * i.a41,
                a21 * i.a12 + a22 * i.a22 + a23 * i.a32 + a24 * i.a42,
                a21 * i.a13 + a22 * i.a23 + a23 * i.a33 + a24 * i.a43,
                a21 * i.a14 + a22 * i.a24 + a23 * i.a34 + a24 * i.a44,

                a31 * i.a11 + a32 * i.a21 + a33 * i.a31 + a34 * i.a41,
                a31 * i.a12 + a32 * i.a22 + a33 * i.a32 + a34 * i.a42,
                a31 * i.a13 + a32 * i.a23 + a33 * i.a33 + a34 * i.a43,
                a31 * i.a14 + a32 * i.a24 + a33 * i.a34 + a34 * i.a44,

                a41 * i.a11 + a42 * i.a21 + a43 * i.a31 + a44 * i.a41,
                a41 * i.a12 + a42 * i.a22 + a43 * i.a32 + a44 * i.a42,
                a41 * i.a13 + a42 * i.a23 + a43 * i.a33 + a44 * i.a43,
                a41 * i.a14 + a42 * i.a24 + a43 * i.a34 + a44 * i.a44
                ));
}


void SMat4x4::Reflect() const{
    printf("%f %f %f %f\n",a11,a12,a13,a14);
    printf("%f %f %f %f\n",a21,a22,a23,a24);
    printf("%f %f %f %f\n",a31,a32,a33,a34);
    printf("%f %f %f %f\n",a41,a42,a43,a44);
}
std::string SMat4x4::ReflectStr() const{
    
    return string_format("Matrix4x4\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n", 
                            a11,a12,a13,a14,
                            a21,a22,a23,a24,
                            a31,a32,a33,a34,
                            a41,a42,a43,a44 );
}

SMat4x4::SMat4x4(SVec4 a1,
                 SVec4 a2,
                 SVec4 a3,
                 SVec4 a4 ) {

    a11 = a1.x; a12 = a2.x;a13 = a3.x; a14 = a4.x;
    a21 = a1.y; a22 = a2.y;a23 = a3.y; a24 = a4.y;
    a31 = a1.z; a32 = a2.z;a33 = a3.z; a34 = a4.z;
    a41 = a1.w; a42 = a2.w;a43 = a3.w; a44 = a4.w;
    //printf("mat\n");
}
SMat4x4::SMat4x4(const std::string &str) {
    const char *s = str.c_str();
    int c = sscanf(s, " %f %f %f %f ; %f %f %f %f ; %f %f %f %f ; %f %f %f %f ", 
            &a11,&a12,&a13,&a14,
            &a21,&a22,&a23,&a24,
            &a31,&a32,&a33,&a34,
            &a41,&a42,&a43,&a44);
    MASSERT(c!=16);
   // if (c != 16 )
   //     throw MatrixError(std::string("Unable construct matrix from string:")+str);
    //better error handing
}

SMat4x4::SMat4x4(float v) {

    a11 = v;    a12 = 0.0f; a13 = 0.0f; a14 = 0.0f;
    a21 = 0.0f; a22 = v;    a23 = 0.0f; a24 = 0.0f;
    a31 = 0.0f; a32 = 0.0f; a33 = v;    a34 = 0.0f;
    a41 = 0.0f; a42 = 0.0f; a43 = 0.0f; a44 = v;
}
SMat4x4::SMat4x4(float _a11,float _a12,float _a13,float _a14,
            float _a21,float _a22,float _a23,float _a24,
            float _a31,float _a32,float _a33,float _a34,
            float _a41,float _a42,float _a43,float _a44){

    a11 = _a11; a12 = _a12; a13 = _a13; a14 = _a14;
    a21 = _a21; a22 = _a22; a23 = _a23; a24 = _a24;
    a31 = _a31; a32 = _a32; a33 = _a33; a34 = _a34;
    a41 = _a41; a42 = _a42; a43 = _a43; a44 = _a44;
}

SMat4x4::SMat4x4( const SMat4x4& o){
    for (int i = 0; i< 16 ;i++ ) {
        raw[i] = o.raw[i];
    }
}
/*https://www.opengl.org/discussion_boards/showthread.php/178484-Extracting-camera-position-from-a-ModelView-Matrix*/
SVec4 SMat4x4::ExtractPositionNoScale() const
{
    SVec4 d(-this->a14,-this->a24,-this->a34,1.0);
    SMat4x4 rot((*this).Transpose());
    rot.a14 = 0;
    rot.a24 = 0;
    rot.a34 = 0;
    SVec4 res((rot)*(d));
    res.w = 1.0; //??
    return res;
}
// TODO make delta standart
bool SMat4x4::Eq(const SMat4x4& a,const SMat4x4& b){
    for ( int i = 0 ; i < 16 ; i++ ) {
        if ( abs(a.raw[i] - b.raw[i]) > 0.00001 )
            return false;
    }
    return true;
}
/*not much optimal*/
SMat4x4 SMat4x4::Move(float x,float y,float z) const{
    SMat4x4 a =  SMat4x4(0.0);
    a.a14 = x;
    a.a24 = y;
    a.a34 = z;
    return (* this)+a;
}
/*optimize*/
SMat4x4 SMat4x4::Translate(const SVec4& vec) const{
    SMat4x4 a =  SMat4x4(0.0);
    a.a14 = vec.x;
    a.a24 = vec.y;
    a.a34 = vec.z;
    a.a44 = vec.w;

    return (* this)+(a);
}

SMat4x4 SMat4x4::Scale(const float sz) const
{
    SMat4x4 a =  SMat4x4();
    a.a11 = sz;
    a.a22 = sz;
    a.a33 = sz;
    return (a)*(* this);
}

SMat4x4 SMat4x4::Scale(float x,float y,float z) const {
    SMat4x4 a =  SMat4x4();
    a.a11 = x;
    a.a22 = y;
    a.a33 = z;
    return (a)*(* this);
}


SMat4x4 SMat4x4::RotX(float ang) const{
    SMat4x4 op =  SMat4x4();
    float ang_c,ang_s;
    ang_c = cos(ang);
    ang_s = sin (ang);
    op.a22 = ang_c;
    op.a23 =-ang_s;
    op.a32 = ang_s;
    op.a33 = ang_c;
    return (op)*(*this); 
}

SMat4x4 SMat4x4::RotY(float ang) const{
    SMat4x4 op =  SMat4x4();
    float ang_c,ang_s;
    ang_c = cos(ang);
    ang_s = sin(ang);
    op.a11 = ang_c;
    op.a31 = -ang_s;
    op.a13 = ang_s;
    op.a33 =  ang_c;
    return (op)*(*this); 
}
SMat4x4 SMat4x4::RotZ(float ang) const{
    SMat4x4 op =  SMat4x4();
    float ang_c,ang_s;
    ang_c = cos(ang);
    ang_s = sin (ang);
    op.a11 = ang_c;
    op.a12 = -ang_s;
    op.a22 = ang_c;
    op.a21 = ang_s;
    return (op)*(*this); 
}
SMat4x4 SMat4x4::Transpose() const {
    return (SMat4x4(a11,a21,a31,a41,
                                a12,a22,a32,a42,
                                a13,a23,a33,a43,
                    a14,a24,a34,a44));
}
/*from MESA float version*/
bool MESA_gluInvertMatrix(const float m[16], float invOut[16])
{
    float inv[16], det;
    int i;

    inv[0] = m[5]  * m[10] * m[15] -
             m[5]  * m[11] * m[14] -
             m[9]  * m[6]  * m[15] +
             m[9]  * m[7]  * m[14] +
             m[13] * m[6]  * m[11] -
             m[13] * m[7]  * m[10];

    inv[4] = -m[4]  * m[10] * m[15] +
              m[4]  * m[11] * m[14] +
              m[8]  * m[6]  * m[15] -
              m[8]  * m[7]  * m[14] -
              m[12] * m[6]  * m[11] +
              m[12] * m[7]  * m[10];

    inv[8] = m[4]  * m[9] * m[15] -
             m[4]  * m[11] * m[13] -
             m[8]  * m[5] * m[15] +
             m[8]  * m[7] * m[13] +
             m[12] * m[5] * m[11] -
             m[12] * m[7] * m[9];

    inv[12] = -m[4]  * m[9] * m[14] +
               m[4]  * m[10] * m[13] +
               m[8]  * m[5] * m[14] -
               m[8]  * m[6] * m[13] -
               m[12] * m[5] * m[10] +
               m[12] * m[6] * m[9];

    inv[1] = -m[1]  * m[10] * m[15] +
              m[1]  * m[11] * m[14] +
              m[9]  * m[2] * m[15] -
              m[9]  * m[3] * m[14] -
              m[13] * m[2] * m[11] +
              m[13] * m[3] * m[10];

    inv[5] = m[0]  * m[10] * m[15] -
             m[0]  * m[11] * m[14] -
             m[8]  * m[2] * m[15] +
             m[8]  * m[3] * m[14] +
             m[12] * m[2] * m[11] -
             m[12] * m[3] * m[10];

    inv[9] = -m[0]  * m[9] * m[15] +
              m[0]  * m[11] * m[13] +
              m[8]  * m[1] * m[15] -
              m[8]  * m[3] * m[13] -
              m[12] * m[1] * m[11] +
              m[12] * m[3] * m[9];

    inv[13] = m[0]  * m[9] * m[14] -
              m[0]  * m[10] * m[13] -
              m[8]  * m[1] * m[14] +
              m[8]  * m[2] * m[13] +
              m[12] * m[1] * m[10] -
              m[12] * m[2] * m[9];

    inv[2] = m[1]  * m[6] * m[15] -
             m[1]  * m[7] * m[14] -
             m[5]  * m[2] * m[15] +
             m[5]  * m[3] * m[14] +
             m[13] * m[2] * m[7] -
             m[13] * m[3] * m[6];

    inv[6] = -m[0]  * m[6] * m[15] +
              m[0]  * m[7] * m[14] +
              m[4]  * m[2] * m[15] -
              m[4]  * m[3] * m[14] -
              m[12] * m[2] * m[7] +
              m[12] * m[3] * m[6];

    inv[10] = m[0]  * m[5] * m[15] -
              m[0]  * m[7] * m[13] -
              m[4]  * m[1] * m[15] +
              m[4]  * m[3] * m[13] +
              m[12] * m[1] * m[7] -
              m[12] * m[3] * m[5];

    inv[14] = -m[0]  * m[5] * m[14] +
               m[0]  * m[6] * m[13] +
               m[4]  * m[1] * m[14] -
               m[4]  * m[2] * m[13] -
               m[12] * m[1] * m[6] +
               m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] +
              m[1] * m[7] * m[10] +
              m[5] * m[2] * m[11] -
              m[5] * m[3] * m[10] -
              m[9] * m[2] * m[7] +
              m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] -
             m[0] * m[7] * m[10] -
             m[4] * m[2] * m[11] +
             m[4] * m[3] * m[10] +
             m[8] * m[2] * m[7] -
             m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] +
               m[0] * m[7] * m[9] +
               m[4] * m[1] * m[11] -
               m[4] * m[3] * m[9] -
               m[8] * m[1] * m[7] +
               m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] -
              m[0] * m[6] * m[9] -
              m[4] * m[1] * m[10] +
              m[4] * m[2] * m[9] +
              m[8] * m[1] * m[6] -
              m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0)
        return false;

    det = 1.0 / det;

    for (i = 0; i < 16; i++)
        invOut[i] = inv[i] * det;

    return true;
}
SMat4x4 SMat4x4::Inverse() const
{
    SMat4x4 inv_res;
    MESA_gluInvertMatrix(this->raw,inv_res.raw);
    return inv_res;
}

SVec4 SMat4x4::ExtractAtVector() const
{
    return SVec4(a11,a12,a13,1.0);
}

SVec4 SMat4x4::ExtractLeftVector() const
{
    return SVec4(a21,a22,a23,1.0);
}

SVec4 SMat4x4::ExtractUpVector() const
{
    return SVec4(a31,a32,a33,1.0);
}




SVec4 operator* (const SMat4x4& m, const SVec4& v){
    return SVec4(m.a11 * v.x + m.a12 * v.y + m.a13 * v.z+ m.a14 * v.w ,
                 m.a21 * v.x + m.a22 * v.y + m.a23 * v.z+ m.a24 * v.w ,
                 m.a31 * v.x + m.a32 * v.y + m.a33 * v.z+ m.a34 * v.w ,
                 m.a41 * v.x + m.a42 * v.y + m.a43 * v.z+ m.a44 * v.w );

}
template <typename T>
SVec4 operator/ (const SVec4& v,T num){
    MASSERT(num = 0.0);
    return SVec4(v.x / num , v.y / num  , v.z/ num , v.w/ num);
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
    q.x = sin_fi*v.x;
    q.y = sin_fi*v.y;
    q.z = sin_fi*v.z;
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

/*SVec2 impl*/
SVec2::SVec2(const SVec2& v){
    x = v.x;
    y = v.y;
}
SVec2::SVec2(float _x, float _y){
    x = _x;
    y = _y;
}
SVec2::SVec2(){
    x = 0;
    y = 0;
}
SVec2 operator-(const SVec2& v1,const SVec2& v2){
return (SVec2(v1.x - v2.x,
              v1.y - v2.y));
}
SVec2 operator+(const SVec2& v1,const SVec2& v2){
return (SVec2(v1.x + v2.x,
              v1.y + v2.y));
}




SMat4x4 LookAtMatrix(const SVec4 &at, const SVec4 &eye, const SVec4 &up)
{
        SVec4 zaxis = SVec4::Normalize( eye -at); /*direction to view point*/
        SVec4 xaxis = SVec4::Normalize(SVec4::Cross3(up,zaxis)); /* left vector*/
        SVec4 yaxis(SVec4::Cross3(zaxis,xaxis)); /* new up vector */
        SVec4 v1 (xaxis.x , yaxis.x , zaxis.x,0.0);
        SVec4 v2 (xaxis.y , yaxis.y , zaxis.y,0.0);
        SVec4 v3 (xaxis.z , yaxis.z , zaxis.z,0.0);
        SVec4 v4(-SVec4::Dot(xaxis,eye),-SVec4::Dot(yaxis,eye),-SVec4::Dot(zaxis,eye),1.0);
        return SMat4x4(v1,v2,v3,v4);
}


SVec4 operator*(const SVec4 &v1, float v2)
{
    return SVec4(v1.x*v2,v1.y*v2,v1.z*v2,v1.w*v2);
}
SVec4 operator/(const SVec4 &v1, float v2)
{
    return SVec4(v1.x/v2,v1.y/v2,v1.z/v2,v1.w/v2);
}



Point operator-(const Point& v1,const Point& v2){
return (Point(v1.x - v2.x,
              v1.y - v2.y,
              v1.z - v2.z));
}
Point operator-(const Point& v1){
return (Point(-v1.x,
              -v1.y,
              -v1.z));
}
Point operator+(const Point& v1,const Point& v){
return (Point(v1.x + v.x,
              v1.y + v.y,
              v1.z + v.z));
}


