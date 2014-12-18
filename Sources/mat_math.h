#pragma once


#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

/*serialization*/
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>

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
    SMat4x4 RotZ(const float ang) const;
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

};

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

