#pragma once
#include <string>

/*serialization*/
#include <cereal/access.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/string.hpp>
#include <cereal/archives/binary.hpp>

#include "mat_math.h"
#include "GenericTexture.h"
class SRBOTexture :public SGenericTexture {
public:
    enum RTType  {
        RT_NONE, /* empty slot*/
        RT_SCREEN, /* render to screen*/
        RT_SCREEN_DEPTH,
        RT_TEXTURE_FLOAT,
        RT_TEXTURE_RGBA,
        RT_TEXTURE_RED, /* r only 8 bit*/
        RT_TEXTURE_FLOAT_RED, /* 32 bit signle float*/
        RT_TEXTURE_DEPTH,
        RT_TEXTURE_MSAA, /*float always ??*/
        RT_TEXTURE_DEPTH_MSAA,
        RT_TEXTURE_CUBEMAP,
        RT_TEXTURE_DEPTH_CUBEMAP,
        RT_TEXTURE_DEPTH_ARRAY
    };


    /* simple empty texture */
    SRBOTexture(RectSizeInt s)
    :SRBOTexture(s,RT_TEXTURE_FLOAT) {}
    SRBOTexture(RectSizeInt s, RTType t, unsigned int miplevel);
    SRBOTexture(RectSizeInt s, RTType t) :SRBOTexture(s,t,1) {}
    SRBOTexture(const SRBOTexture&) = delete;
     ~SRBOTexture();

    int Bind(unsigned int sampler) const; /*bind as texture to sampler*/
    int BindImage(unsigned int unit);

    int setInterpolationMode(InterpolationType t);

    RTType type;
    RectSizeInt d_s;

    bool IsReady = false;

    /*serialize support */
    friend class cereal::access;
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar(CEREAL_NVP(d_s.w),CEREAL_NVP(d_s.h),CEREAL_NVP(IsReady),CEREAL_NVP(type));
    }
    bool IsMSAA();

    static RTType getRelatedDepthType(RTType t );
    static bool isDepthType(RTType t);
    static unsigned int getRelatedGLType (RTType t) ;
    /*NOT IMPL*/
    static RTType getRelatedTextureType(RTType t );

private:

    int ConfigureTexture(const BorderType t) const;
    bool d_isMSAA = false;

  
};
