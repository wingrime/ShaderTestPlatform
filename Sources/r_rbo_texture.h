#pragma once
#include <string>

/*serialization*/
#include <cereal/access.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/string.hpp>
#include <cereal/archives/binary.hpp>


class SRBOTexture {
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
    enum BorderType {
        TEX_CLAMP = 0,
        TEX_REPEAT
    };
    /* simple empty texture */
    SRBOTexture(int _x, int _y)
    :SRBOTexture(_x,_y,RT_TEXTURE_FLOAT) {}
    SRBOTexture(int _x, int _y, RTType t, unsigned int miplevel);
    SRBOTexture(int _x, int _y, RTType t) :SRBOTexture(_x,_y,t,1) {}
    SRBOTexture(const SRBOTexture&) = delete;
     ~SRBOTexture();
    int Bind(unsigned int sampler) const; /*bind as texture to sampler*/
    int BindImage(unsigned int unit);
    unsigned int getGLId() const;
    int x,y;

    bool IsReady = false;
    RTType type;
    /*serialize support */
    friend class cereal::access;
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar(CEREAL_NVP(x),CEREAL_NVP(y),CEREAL_NVP(IsReady),CEREAL_NVP(type));
    }
    bool IsMSAA();

    static RTType getRelatedDepthType(RTType t );
    static bool isDepthType(RTType t);
    static unsigned int getRelatedGLType (RTType t) ;
    /*NOT IMPL*/
    static RTType getRelatedTextureType(RTType t );

private:
    unsigned int tex;
    int ConfigureTexture(const BorderType t) const;
    bool d_isMSAA = false;

  
};
