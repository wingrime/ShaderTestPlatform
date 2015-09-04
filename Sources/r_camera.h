#pragma once
//TODO
//new class cascade camera!
/*serialization*/
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>
/*inhertance, can't avoid*/
#include "r_projmat.h"
//TODO: fine time sampling
class Recorder {
public:
    Recorder();
    int Erase();
    int Begin();
    int End();
    int Add(const SMat4x4& s); /*new sample*/

    bool Empty();

    int Save(const std::string &fname);
    //open
    int Rewind();
    const SMat4x4 Get();
private:
    std::vector<SMat4x4> r;
    bool d_recordEnabled = 0;
    SMat4x4 d_lastSample;
    std::vector<SMat4x4>::iterator d_sampleIterator;
    /*serialize support */
    friend class cereal::access;
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar(

            CEREAL_NVP(r)
        );
    }

};
class SCamera {
public:

    SCamera(const SMat4x4& view,const SMat4x4& proj) :proj(proj) ,view(view) {}
    SCamera() :SCamera(SMat4x4(), SMat4x4()) {}
    SCamera(const SCamera& sc) :proj(sc.proj),view(sc.view) {}


    inline __attribute__((always_inline)) SMat4x4  getViewMatrix() const {return view;}
    inline __attribute__((always_inline)) SMat4x4  getProjMatrix() const {return proj;}
    SMat4x4 getViewProjectMatrix() ;

    int setViewMatrix( const SMat4x4& m);
    int setProjMatrix( const SMat4x4& m);

    SVec4 getPosition() const;
    int goForward(float s);
    int rotEulerX(float x);
    int rotEulerY(float y);
    int rotEulerZ(float z);

    int goPosition(float x, float y ,float z);
    int goPosition(const SVec4& v);
    int rotEuler(const SVec4& v);
    int LookAt(const SVec4 &at, const SVec4 &eye, const SVec4 &up);

    int SyncFromCamera(const SCamera& s);

    int Reflect() const;

private:
    SMat4x4 buildViewMatrix();

    SMat4x4 proj;
    SMat4x4 view;

  


    float xRot = 0.0;
    float yRot = 0.0;
    float zRot = 0.0;

    float xPos = 0.0;
    float yPos = 0.0;
    float zPos = 0.0;


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


};
