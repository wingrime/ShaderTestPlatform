#pragma once


#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif
#define M_PIf 3.14159265358979f

#include <string>
#define MATH_SERIALIZE
#ifdef MATH_SERIALIZE
/*serialization*/
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>
#endif
#include <type_traits>



//TODO
// && operator
// == operator
//vector cross product can become unstable, such situations should be traced
template <typename T>
T inline toRad(T deg){

    static_assert(std::is_floating_point<T>::value , "It's Error using non floating point here");
    return (M_PIf /180.0f) * deg;
}

template <typename T>
T inline toDeg(T rad){
    static_assert(std::is_floating_point<T>::value , "It's Error using non floating point here");
    return (180.0f/ M_PIf) * rad;
}


class vec4 {
public:
    vec4();
    vec4(float _x, float _y, float _z, float _w);
    vec4(const vec4& v);
    vec4(vec4&& v);
    vec4(const std::string& str);

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

    //vec4 operator*(const vec4& v) const;
    // not corrent, in R4 it will require 3 vector for cross product
    vec4 operator+(const vec4& v) const;
    vec4 &operator=(const vec4 & v);
    vec4 operator -();
    vec4 &operator=(vec4 && v);
    bool operator==(const vec4& p) const;
    vec4 Normalize() const;
    static vec4 Normalize(const vec4& a);

    static float Dot(const vec4& a,const vec4& b);
    static bool Eq(const vec4& a,const vec4& b);

    void Reflect() const;
    /*divide by w after projection*/
    void DivW();
    /*Modulo*/
    void Abs();
    static vec4 Cross3(const vec4 &a, const vec4 &b);
    float Length() const;

#ifdef MATH_SERIALIZE
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
#endif


};
/*free from operator*/
class SMat4x4;
vec4 operator* (const SMat4x4& m, const vec4& v);
vec4 operator-(const vec4& v1,const vec4& v2);
vec4 operator*(const vec4& v1,float v2);
vec4 operator/(const vec4& v1,float v2);

class Point;
class SMat4x4 {
public:
    SMat4x4(float _a11, float _a12, float _a13, float _a14,
            float _a21, float _a22, float _a23, float _a24,
            float _a31, float _a32, float _a33, float _a34,
            float _a41, float _a42, float _a43, float _a44);
    
    SMat4x4(vec4 a1, vec4 a2, vec4 a3,vec4 a4);
    SMat4x4(const std::string& str);
    /*indent matrix fill*/
	SMat4x4() : SMat4x4(1.0f) {}
	SMat4x4(float a);
    SMat4x4( const SMat4x4& i);
    SMat4x4( SMat4x4&& i);
    SMat4x4& operator=(SMat4x4&& o);
    SMat4x4 &operator= (const SMat4x4 & o);

    /*extract transform only for uniform matrix*/
    vec4 ExtractPositionNoScale() const;

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
    SMat4x4& operator+( SMat4x4&& i);
    SMat4x4 operator*(const SMat4x4& i) const;
    //in place matrix mult not defined//
    SMat4x4 operator-(const SMat4x4& i) const;
    SMat4x4& operator-( SMat4x4&& i);
    // Move/Scale
    SMat4x4 Move(float x,float y,float z) const;
    SMat4x4 Move(const Point &p) const;
    SMat4x4 Translate(const vec4& vec) const;
    //Not Rigid;
    SMat4x4 Scale(float x,float y,float z) const;
    SMat4x4 Scale( float sz) const;
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

    /*Extract vector routines*/
    vec4 ExtractAtVector() const;
    vec4 ExtractLeftVector() const;
    vec4 ExtractUpVector() const;


    //check

    static bool Eq(const SMat4x4& a,const SMat4x4& b);

    
    virtual ~SMat4x4();
    #ifdef MATH_SERIALIZE
    /*serialize support */
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar( CEREAL_NVP(a11),CEREAL_NVP(a12),CEREAL_NVP(a13),CEREAL_NVP(a14),
           CEREAL_NVP(a21),CEREAL_NVP(a22),CEREAL_NVP(a23),CEREAL_NVP(a24),
           CEREAL_NVP(a31),CEREAL_NVP(a32),CEREAL_NVP(a33),CEREAL_NVP(a34),
           CEREAL_NVP(a41),CEREAL_NVP(a42),CEREAL_NVP(a43),CEREAL_NVP(a44));
    }
    #endif

};
SMat4x4 LookAtMatrix(const vec4& at,const  vec4& eye,const vec4& up);
class UnitQuaterion {
public:
    UnitQuaterion();
    UnitQuaterion(float x,float y, float z,float w);
    UnitQuaterion(const UnitQuaterion& quat);
    UnitQuaterion(const vec4& v, float fi);

    UnitQuaterion(const SMat4x4& m); /*TODO*/
    UnitQuaterion(const std::string& str); /*TODO */

    UnitQuaterion Conjugate() const;

    UnitQuaterion Inverse() const;

    UnitQuaterion Normalize() const;
    float Norm() const;

    UnitQuaterion Rotate(const vec4& axis, float fi) const;
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
    constexpr static float CheckDelta = 1e-6f;
};
/* integer rectangle size in D2*/
struct RectSizeInt {
	RectSizeInt(int _h, int _w) :h(_h), w(_w) {}
	RectSizeInt() :h(0), w(0) {}
	//RectSizeInt(const RectSize& rs) :h((int)rs.h), w((int)rs.w) {}
	union {
		struct { int h, w; };
		struct { int y, x; };
	};


};
struct RectSize {
	RectSize(float _h, float _w) :h(_h), w(_w) {}
	RectSize() :h(0), w(0) {}
	RectSize(const RectSizeInt& rs) :h((float)rs.h), w((float)rs.w) {}
	union {
		struct { float h, w; };
		struct { float y, x; };
	};


};


/* AABB in 2d */



class vec2 {
public:
    vec2();
    vec2(float x,float y);
    //vec2(const std::string& str);
    vec2(vec2&& o);
    vec2(const vec2& v);

    vec2 &operator=(vec2&& o);
    vec2 &operator=(const vec2& o);
    bool operator!=(const vec2& p) const;
    bool operator==(const vec2& p) const;

    vec2& operator-();
    vec2& operator-(vec2&& o);
    vec2& operator+(vec2&& o);
    vec2 operator*(float a);
    vec2 operator/(float a);
    static float length(const vec2& o);
    static vec2 normalize(const vec2 & o);


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
            float u;float v;
        };
        struct {
            float raw[2];
        };
    };

#ifdef MATH_SERIALIZE
    /*serialize support */
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar( x,y);
    }
#endif


};
/*free from operators*/
vec2 operator-(const vec2& v1,const vec2& v2);
vec2 operator+(const vec2& v1,const vec2& v2);
class vec3 {
public:
    vec3(float _x, float _y, float _z);
    vec3(const vec3& v);
    vec3();
    vec3(vec3&& o);
    vec3& operator=(vec3&& o);
    vec3 &operator=(const vec3& o);
    bool operator!=(const vec3& p) const;
    bool operator==(const vec3& p) const;
    vec3& operator-(vec3 && o);
    vec3& operator+(vec3 && o);
    vec3 operator-();
    vec3 operator* (float a);
    vec3 operator/ (float a);
    //vec2(const std::string& str);
    union  {
        struct {
            float x;
            float y;
            float z;
        };
        struct {
            float r;
            float g;
            float b;
        };
        struct {
            float raw[3];
        };
    };
    static vec3 cross(const vec3 &a, const vec3 &b);
    static vec3 normalize(const vec3 &a);
    static float length(const vec3 &a);

#ifdef MATH_SERIALIZE
    /*serialize support */
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar( x,y,z);
    }
#endif


};
/*free from operators*/
vec3 operator-(const vec3& v1,const vec3& v2);
vec3 operator+(const vec3& v1,const vec3& v2);

class BBox {
public:
    BBox() :min_point(),max_point() {}
    BBox(vec2 _min_p,vec2 _max_p) :min_point(_min_p),max_point(_max_p) {}
    vec2 min_point;
    vec2 max_point;
};
class Point  {
public:
    Point(float _x,float _y,float _z) :x(_x),y(_y),z(_z) {}
    Point (vec4 a) :x(a.x),y(a.y),z(a.z) {}
    Point (vec3 a) :x(a.x),y(a.y),z(a.z) {}
    Point () :x(0.0),y(0.0),z(0.0) {}
    float x,y,z;
};
class Line{
public:
    Line() :p1(),p2() {}
    Line(Point _p1,Point _p2) :p1(_p1),p2(_p2) {}
    Point p1;
    Point p2;
};
Point operator-(const Point& v1,const Point& v2);
Point operator-(const Point& v1);
Point operator+(const Point& v1,const Point& v2);
