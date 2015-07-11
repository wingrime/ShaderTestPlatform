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
    view = LookAt(at,eye,up);
    return 0;
}

int SCamera::SyncFromCamera(const SCamera &s)
{
    view = s.view;
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
    xPos += view.a31*s2;
    yPos += view.a32*s2;
    zPos += view.a33*s2;
    buildViewMatrix();

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

SMat4x4 SCamera::getViewProjectMatrix()
{
    return SMat4x4(proj*view);
}



SVec4 SCamera::getPosition() const {
    return view.ExtractPositionNoScale();
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
