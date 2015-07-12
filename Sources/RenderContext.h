#pragma once

#include "r_sprog.h"
#include "mat_math.h"
#include "GenericTexture.h"
#include "r_projmat.h"
#include "r_camera.h"
/*
Static object for storage current render context set:
Current Shader,
Required

*/
class RenderContext {
    public:
        RenderContext(SShader* _s)
            :shader(_s)
        {
            initUniforms();
        }
        RenderContext(SShader* _s, const SMat4x4& _V, const SMat4x4 &_P)
            :shader(_s),
            d_V(_V),
            d_P(_P)
         {
            initUniforms();
        }
        RenderContext(SShader* s,const SMat4x4& _V, const SMat4x4& _P,
                      std::shared_ptr<SGenericTexture> tex0)
            :shader(s),
            d_V(_V),
            d_P(_P)
        {
            d_RBOTexture[0] = tex0;
            initUniforms();
        }
        RenderContext(SShader* s, const SMat4x4& _V, const SMat4x4& _P,
                             std::shared_ptr<SGenericTexture> tex0,
                             std::shared_ptr<SGenericTexture> tex1)
                     :shader(s),
                     d_V(_V),
                     d_P(_P)
                 {
                     d_RBOTexture[0] = tex0;
                     d_RBOTexture[1] = tex1;
                     initUniforms();

        }
        RenderContext(SShader* s, const SMat4x4& _V, const SMat4x4& _P,
                             std::shared_ptr<SGenericTexture> tex0,
                             std::shared_ptr<SGenericTexture> tex1,
                             std::shared_ptr<SGenericTexture> tex2)
                     :shader(s),
                     d_V(_V),
                     d_P(_P)
                 {
                     d_RBOTexture[0] = tex0;
                     d_RBOTexture[1] = tex1;
                     d_RBOTexture[2] = tex2;
                     initUniforms();

        }
        RenderContext(SShader* s, const SMat4x4& _V, const SMat4x4& _P,
                    std::shared_ptr<SGenericTexture> tex0,
                    std::shared_ptr<SGenericTexture> tex1,
                    std::shared_ptr<SGenericTexture> tex2,
                    std::shared_ptr<SGenericTexture> tex3)
            :shader(s),
            d_V(_V),
            d_P(_P)
        {
            d_RBOTexture[0] = tex0;
            d_RBOTexture[1] = tex1;
            d_RBOTexture[2] = tex2;
            d_RBOTexture[3] = tex3;
            initUniforms();

        }
        RenderContext(SShader* s, const SMat4x4& _V, const SMat4x4& _P,
                    std::shared_ptr<SGenericTexture> tex0,
                    std::shared_ptr<SGenericTexture> tex1,
                    std::shared_ptr<SGenericTexture> tex2,
                    std::shared_ptr<SGenericTexture> tex3,
                    std::shared_ptr<SGenericTexture> tex4)
            :shader(s),
            d_V(_V),
            d_P(_P)
        {
            d_RBOTexture[0] = tex0;
            d_RBOTexture[1] = tex1;
            d_RBOTexture[2] = tex2;
            d_RBOTexture[3] = tex3;
            d_RBOTexture[4] = tex4;
            initUniforms();

        }
        RenderContext(SShader* s, const SMat4x4& _V, const SMat4x4& _P,
                    std::shared_ptr<SGenericTexture> tex0,
                    std::shared_ptr<SGenericTexture> tex1,
                    std::shared_ptr<SGenericTexture> tex2,
                    std::shared_ptr<SGenericTexture> tex3,
                    std::shared_ptr<SGenericTexture> tex4,
                    std::shared_ptr<SGenericTexture> tex5)
            :shader(s),
            d_V(_V),
            d_P(_P)
        {
            d_RBOTexture[0] = tex0;
            d_RBOTexture[1] = tex1;
            d_RBOTexture[2] = tex2;
            d_RBOTexture[3] = tex3;
            d_RBOTexture[4] = tex4;
            d_RBOTexture[5] = tex5;
            initUniforms();

        }
        RenderContext(const RenderContext&) = delete;
    	SShader *shader;
        const SMat4x4 d_V;
        const SMat4x4 d_P;
        static constexpr int MAX_RBO_TEXTURES = 6;
        std::shared_ptr<SGenericTexture> d_RBOTexture[MAX_RBO_TEXTURES];
        /**/
        inline int initUniforms() {
            d_viewMatrixLoc = shader->getUniformLocation("matrixView");
            d_projMatrixLoc = shader->getUniformLocation("matrixProjection");
            d_modelMatrixLoc = shader->getUniformLocation("matrixModel");
            d_uniformMVP = shader->getUniformLocation("MVP");
            d_uniformMV = shader->getUniformLocation("MV");
        }
        /*located uniform storage*/
        int d_viewMatrixLoc;
        int d_projMatrixLoc;
        int d_modelMatrixLoc;
        int d_uniformMVP;
        int d_uniformMV;
};
