#include "r_camera.h"

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
