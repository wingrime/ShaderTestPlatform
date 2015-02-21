#pragma once


#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

/*serialization*/
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>

#include <type_traits>



//TODO
// && operator
// == operator
//vector cross product can become unstable, such situations should be traced
template <typename T>
T inline toRad(T deg){

    static_assert(std::is_floating_point<T>::value , "It's Error using non floating point here");
    return (M_PI/180.0f) * deg;
}

template <typename T>
T inline toDeg(T rad){
    static_assert(std::is_floating_point<T>::value , "It's Error using non floating point here");
    return (180.0f/M_PI) * rad;
}


class SVec4 {
public:
    SVec4(float _x, float _y, float _z, float _w);
    SVec4(const SVec4& v);
    SVec4();
    SVec4(const std::string& str);

    union  {
        struct {
            float x;float y;float z;float w;
        };
        struct {
            float r;float g;float b;float a;
        };
        struct {
            float raw[4];
        };
    } ;

    //SVec4 operator*(const SVec4& v) const;
    // not corrent, in R4 it will require 3 vector for cross product
    SVec4 operator+(const SVec4& v) const;
    SVec4 Normalize() const;
    static SVec4 Normalize(const SVec4& a);

    static float Dot(const SVec4& a,const SVec4& b);
    static bool Eq(const SVec4& a,const SVec4& b);

    void Reflect() const;
    /*divide by w after projection*/
    void DivW();

    /*serialize support */
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar( CEREAL_NVP(x),
            CEREAL_NVP(y),
            CEREAL_NVP(z),
            CEREAL_NVP(w)
            );
    }

    static SVec4 Cross3(const SVec4 &a, const SVec4 &b);
    float Length() const;
};
/*free from operator*/
class SMat4x4;
SVec4 operator* (const SMat4x4& m, const SVec4& v);
SVec4 operator-(const SVec4& v1,const SVec4& v2);
SVec4 operator*(const SVec4& v1,float v2);



class SMat4x4 {
public:
    SMat4x4(float _a11, float _a12, float _a13, float _a14,
            float _a21, float _a22, float _a23, float _a24,
            float _a31, float _a32, float _a33, float _a34,
            float _a41, float _a42, float _a43, float _a44);
    
    SMat4x4(SVec4 a1,
            SVec4 a2,
            SVec4 a3,
            SVec4 a4);
    SMat4x4(const std::string& str);
    /*indent matrix fill*/
	SMat4x4() : SMat4x4(1.0f) {}

    
	SMat4x4(float a);
    SMat4x4( const SMat4x4& i);

    /*extract transform only for uniform matrix*/
    SVec4 ExtractPositionNoScale() const;

    //reflect to stdout
    void Reflect() const;
    // out to string
    std::string ReflectStr() const;


    //internals
    union  {
        struct {
            float a11;float a12;float a13;float a14;
            float a21;float a22;float a23;float a24;
            float a31;float a32;float a33;float a34;
            float a41;float a42;float a43;float a44;
        };
        //struct {
        float raw[16];
        //};
    };

    //operators
    SMat4x4 operator+(const SMat4x4& i) const;
    SMat4x4 operator*(const SMat4x4& i) const;
    SMat4x4 operator-(const SMat4x4& i) const;
    // Move/Scale
    SMat4x4 Move(const float x,const float y,const float z) const;
    SMat4x4 Translate(const SVec4& vec) const;
    //Not Rigid;
    SMat4x4 Scale(const float x,const float y,const float z) const;
    SMat4x4 Scale(const float sz) const;
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
    static SMat4x4 LookAt(const SVec4& at,const  SVec4& eye,const SVec4& up);
    
    virtual ~SMat4x4();

    /*serialize support */
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar( CEREAL_NVP(a11),CEREAL_NVP(a12),CEREAL_NVP(a13),CEREAL_NVP(a14),
           CEREAL_NVP(a21),CEREAL_NVP(a22),CEREAL_NVP(a23),CEREAL_NVP(a24),
           CEREAL_NVP(a31),CEREAL_NVP(a32),CEREAL_NVP(a33),CEREAL_NVP(a34),
           CEREAL_NVP(a41),CEREAL_NVP(a42),CEREAL_NVP(a43),CEREAL_NVP(a44));
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

/*Raw GL primitives*/
struct Point  {
    Point(float _x,float _y,float _z) :x(_x),y(_y),z(_z) {}
    Point (SVec4 a) :x(a.x),y(a.y),z(a.z) {}
    float x,y,z;
};
struct Line {
    Point p1;
    Point p2;
};
struct AABB {
    Point p1;
    Point p2;
};
class SVec2 {
public:
    SVec2(float x,float y);
    SVec2(const SVec2& v);
    SVec2();
    //SVec2(const std::string& str);
    union  {
        struct {
            float x;float y;
        };
        struct {
            float r;float g;
        };
        struct {
            float w;float h;
        };
        struct {
            float raw[2];
        };
    };

    /*serialize support */
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar( x,y);
    }
};
/*free from operators*/
SVec2 operator-(const SVec2& v1,const SVec2& v2);
SVec2 operator+(const SVec2& v1,const SVec2& v2);

