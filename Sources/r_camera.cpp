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
    xPos = v.x;
    yPos = v.y;
    zPos = v.z;
    buildViewMatrix();
    return 0;
}

int SCamera::rotEuler(const SVec4& v)
{
    xRot = v.x;
    yRot = v.y;
    zRot = v.z;
    buildViewMatrix();
    return 0;
}

int SCamera::LookAt(const SVec4& at,const  SVec4& eye,const SVec4& up)
{
    SVec4 zaxis = ( eye -at).Normalize(); /*direction to view point*/
    SVec4 xaxis = (SVec4::Cross3(up,zaxis)).Normalize(); /* left vector*/
    SVec4 yaxis(SVec4::Cross3(zaxis,xaxis)); /* new up vector */
    SVec4 v1 (xaxis.x , yaxis.x , zaxis.x,0.0);
    SVec4 v2 (xaxis.y , yaxis.y , zaxis.y,0.0);
    SVec4 v3 (xaxis.z , yaxis.z , zaxis.z,0.0);
    SVec4 v4(-SVec4::Dot(xaxis,eye),-SVec4::Dot(yaxis,eye),-SVec4::Dot(zaxis,eye),1.0);
    //SVec4 v4(eye.vec.x,eye.vec.y,eye.vec.z,1.0);
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
    //xRot = s.xRot;
    //yRot = s.yRot;
    //zRot = s.zRot;


    //xPos = s.xPos;
    //yPos = s.yPos;
    //zPos = s.zPos;
    view = s.view;
    //buildViewMatrix();

    return 0;
}

int SCamera::rotEulerX(float x) {
    xRot = x;
    buildViewMatrix();
    return 0;
}

int SCamera::rotEulerY(float y) {
    yRot = y;
    buildViewMatrix();
    return 0;
}

int SCamera::rotEulerZ(float z) {
    zRot = z;
    buildViewMatrix();
    return 0;
}
int SCamera::goForward(float s) {

    /*Move from current to new offset*/
    float s2 = 1.0+s;
    xPos += view.mat.a31*s2;
    yPos += view.mat.a32*s2;
    zPos += view.mat.a33*s2;
    buildViewMatrix();

   
    //view.Reflect();
    return 0;
}

SMat4x4 SCamera::buildViewMatrix(){
    SMat4x4 a;
    a = a.Move(xPos,yPos,zPos);
    /*gimbal lock ! */
    a = a.RotY(yRot);
    a = a.RotX(xRot);
    a = a.RotZ(zRot);
    view = a;
    return a;
}
SMat4x4 SCamera::getViewMatrix(){
    return view;
}

SMat4x4 SCamera::getProjMatrix(){
    return proj;
}
SVec4 SCamera::getPosition() const {
    return SVec4(xPos,zPos,yPos,1.0);
}

int SCamera::setViewMatrix(const SMat4x4 &m)
{
    view = SMat4x4(m);
    return 0;
}

int SCamera::setProjMatrix(const SMat4x4 &m)
{
    proj = SMat4x4(m);
    return 0;
}
