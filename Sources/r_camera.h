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
