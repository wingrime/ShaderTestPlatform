#include "Camera.h"
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>
#include <memory>
#include <ostream>
#include <fstream>
#include "ErrorCodes.h"
#include "string_format.h"
#include "Log.h"
int SCamera::Reflect() const{
    LOGV("camera: %f %f %f <_> %f %f %f\n", xPos,yPos,zPos, xRot,yRot,zRot);
    return 0;
}
int SCamera::goPosition(float x, float y ,float z) {
    xPos = x;
    yPos = y;
    zPos = z;
    buildViewMatrix();
    return 0;
}

int SCamera::goPosition(const vec4 &v)
{
    xPos = v.x;
    yPos = v.y;
    zPos = v.z;
    buildViewMatrix();
    return 0;
}

int SCamera::rotEuler(const vec4& v)
{
    xRot = v.x;
    yRot = v.y;
    zRot = v.z;
    buildViewMatrix();
    return 0;
}

int SCamera::LookAt(const vec4& at,const  vec4& eye,const vec4& up)
{
    view = LookAtMatrix(at,eye,up);
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

const SMat4x4 SCamera::getViewProjectMatrix()
{
    return SMat4x4(proj*view);
}



vec4 SCamera::getPosition() const {
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


Recorder::Recorder()
{
// nothing
}

int Recorder::Erase()
{
    r.clear();
    return ESUCCESS;
}

int Recorder::Begin()
{
    d_recordEnabled = true;
    return ESUCCESS;
}

int Recorder::End()
{
    d_recordEnabled = false;
    d_sampleIterator = r.end();
    return ESUCCESS;
}

int Recorder::Add(double dt,const SMat4x4 &s)
{
    RecordSample sample;
    if (d_recordEnabled)
        sample.dt = dt;
        sample.cam = s;
        r.push_back(sample);
    d_sampleIterator = r.end();
    return ESUCCESS;
}

bool Recorder::Empty()
{
    return d_sampleIterator == r.begin();
}

int Recorder::Save(const std::string &fname)
{
    LOGE("NONIMPL!");
   // std::ofstream os(fname);
   //{
       /*use raii */
    //   cereal::JSONOutputArchive archive( os);
   //    archive( CEREAL_NVP( r));
   //}
    return ESUCCESS;
}

int Recorder::Rewind()
{
    d_sampleIterator = r.end();
    return ESUCCESS;
}

const RecordSample Recorder::Get()
{
    if (!r.empty()) {
        d_lastSample = (*d_sampleIterator);
        d_sampleIterator--;
    }
    return d_lastSample;
}
