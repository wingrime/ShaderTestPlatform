#pragma once
#include "mat_math.h"
#include "r_projmat.h"

/*serialization*/
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>
#include <fstream>

class SCamera {
public:
    
    SCamera(float x, float y, float z)
    :proj(  SPerspectiveProjectionMatrix(10.0f, 5000.0f,1.0f,toRad(26.0)))
    ,xPos(x)
    ,yPos(y)
    ,zPos(z)
     { buildViewMatrix();};
    SCamera(float x, float y, float z,float rx, float ry, float rz)
    :proj(  SPerspectiveProjectionMatrix(10.0f, 5000.0f,1.0f,toRad(26.0)))
    ,xPos(x)
    ,yPos(y)
    ,zPos(z)
    ,xRot(x)
    ,yRot(y)
    ,zRot(z)
     { buildViewMatrix();};
     SCamera(float x, float y, float z,float rx, float ry, float rz, SMat4x4 proj)
    :proj(proj)
    ,xPos(x)
    ,yPos(y)
    ,zPos(z)
    ,xRot(x)
    ,yRot(y)
    ,zRot(z)
     { buildViewMatrix();};
          SCamera(SMat4x4 view, SMat4x4 proj)
    :proj(proj)
    ,view(view)
     {};
    SCamera()
    :SCamera(0,0,0)
     {};


    SMat4x4 buildViewMatrix();


    const SMat4x4& getViewMatrix() const;
    const SMat4x4& getProjMatrix() const;
    SVec4 getPosition() const;


    int goForward(float s);
    int setEulerX(float x);
    int setEulerY(float y);
    int setEulerZ(float z);
    int goPosition(float x, float y ,float z);
    int goPosition(const SVec4& v);
    int setEuler(const SVec4& v);
    int LookAt(const SVec4 &at, const SVec4 &eye, const SVec4 &up);

    int SyncFromCamera(const SCamera& s);

    int Reflect() const;

    /*serialize support */
    friend class cereal::access;
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar( 

            CEREAL_NVP(xPos),
            CEREAL_NVP(yPos),
            CEREAL_NVP(zPos),

            CEREAL_NVP(xRot),
            CEREAL_NVP(yRot),
            CEREAL_NVP(zRot),

            CEREAL_NVP(view),
            CEREAL_NVP(proj)
            );
    }



private:
    SMat4x4 view;
    SMat4x4 proj;
  


    float xRot = 0.0;
    float yRot = 0.0;
    float zRot = 0.0;

    float xPos = 0.0;
    float yPos = 0.0;
    float zPos = 0.0;


};
int SCamera::Reflect() const{
    printf("camera: %f %f %f <_> %f %f %f\n", xPos,yPos,zPos, xRot,yRot,zRot);
    return 0;
}
int SCamera::goPosition(float x, float y ,float z) {
    xPos = x;
    yPos = y;
    zPos = z;
    buildViewMatrix();
    return 0;
}

int SCamera::goPosition(const SVec4 &v)
{
    xPos = v.vec.x;
    yPos = v.vec.y;
    zPos = v.vec.z;
    buildViewMatrix();
    return 0;
}

int SCamera::setEuler(const SVec4& v)
{
    xRot = v.vec.x;
    yRot = v.vec.y;
    zRot = v.vec.z;
    buildViewMatrix();
    return 0;
}

int SCamera::LookAt(const SVec4& at,const  SVec4& eye,const SVec4& up)
{
    SVec4 zaxis = (at - eye).Normalize();
    zaxis.Reflect();
    SVec4 xaxis = (SVec4::Cross(up,zaxis)).Normalize();
    xaxis.Reflect();
    SVec4 yaxis(zaxis*xaxis);// SVec4::Cross(zaxis,yaxis);
    yaxis.Reflect();
    SVec4 v1 (xaxis.vec.x , yaxis.vec.x , zaxis.vec.x,0.0);
    SVec4 v2 (xaxis.vec.y , yaxis.vec.y , zaxis.vec.y,0.0);
    SVec4 v3 (xaxis.vec.z , yaxis.vec.z , zaxis.vec.z,0.0);
    SVec4 v4(SVec4(-SVec4::Dot(xaxis,eye), -SVec4::Dot(yaxis,eye),-SVec4::Dot(zaxis,eye),1.0));

    /*clean */
    xPos = 0.0;
    yPos = 0.0;
    zPos = 0.0;
    xRot = 0.0;
    yRot = 0.0;
    zRot = 0.0;
    view = SMat4x4(v1,v2,v3,v4);
    return 0;
}

int SCamera::SyncFromCamera(const SCamera &s)
{
    xRot = s.xRot;
    yRot = s.yRot;
    zRot = s.zRot;


    xPos = s.xPos;
    yPos = s.yPos;
    zPos = s.zPos;
    buildViewMatrix();

    return 0;
}

int SCamera::setEulerX(float x) {
    xRot = x;
    buildViewMatrix();
    return 0;
}

int SCamera::setEulerY(float y) {
    yRot = y;
    buildViewMatrix();
    return 0;
}

int SCamera::setEulerZ(float z) {
    zRot = z;
    buildViewMatrix();
    return 0;
}
int SCamera::goForward(float s) {

    // that makes camera info outdated
    //buildViewMatrix();

    xPos += view.mat.a31*s;
    yPos += view.mat.a32*s;
    zPos += view.mat.a33*s;
    buildViewMatrix();

   
    //view.Reflect();
    return 0;
}

SMat4x4 SCamera::buildViewMatrix(){
    SMat4x4 a;

    a = a.Move(xPos,yPos,zPos);

    a = a.RotY(yRot);
    a = a.RotX(xRot);
    a = a.RotZ(zRot);
    view = a;
    return a;
}
const SMat4x4& SCamera::getViewMatrix() const{
    return view;
}

const SMat4x4& SCamera::getProjMatrix() const{
    return proj;
}
SVec4 SCamera::getPosition() const {
    return SVec4(xPos,zPos,yPos,1.0);
}
