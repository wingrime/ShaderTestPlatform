#pragma once

#include "r_sprog.h"
#include "mat_math.h"
#include "RBO.h"
#include "r_texture.h"
#include "r_rbo_texture.h"

#include "r_projmat.h"
#include "r_camera.h"
/*
Static object for storage current render context set:
Current Shader,
Current Camera Matrix,
Current ViewPort 
Current Object Matrix ???

*/
class RenderContext {
    public:
        RenderContext(const RBO* _v,SShader* _s, const SMat4x4& _V, const SMat4x4 &_P)
            :shader(_s),
            viewport(_v),
            d_V(_V),
            d_P(_P)
         {
            initUniforms();
        }
        RenderContext(const RBO* v,SShader* s,const SMat4x4& _V, const SMat4x4& _P, std::shared_ptr<SRBOTexture> sm_tex)
            :shader(s),
         	viewport(v),
            d_V(_V),
            d_P(_P)
        {
            d_RBOTexture[0] = sm_tex;
            initUniforms();
        }
        RenderContext(const RBO* v,SShader* s, const SMat4x4& _V, const SMat4x4& _P,
                    std::shared_ptr<SRBOTexture> sm_tex,
                    std::shared_ptr<SRBOTexture> tex1,
                    std::shared_ptr<SRBOTexture> tex2,
                    std::shared_ptr<SRBOTexture> tex3)
            :shader(s),
            viewport(v),
            d_V(_V),
            d_P(_P)
        {
            d_RBOTexture[0] = sm_tex;
            d_RBOTexture[1] = tex1;
            d_RBOTexture[2] = tex2;
            d_RBOTexture[3] = tex3;
            initUniforms();

        }
        RenderContext(const RenderContext&) = delete;
    	SShader *shader;
    	const RBO *viewport;
        const SMat4x4 d_V;
        const SMat4x4 d_P;
        static constexpr int MAX_RBO_TEXTURES = 5;
        std::shared_ptr<SRBOTexture> d_RBOTexture[MAX_RBO_TEXTURES];
        /**/
        inline int initUniforms() {
            d_viewMatrixLoc = shader->getUniformLocation("view");
            d_projMatrixLoc = shader->getUniformLocation("cam_proj");
            d_modelMatrixLoc = shader->getUniformLocation("model");
            d_uniformMVP = shader->getUniformLocation("MVP");
            d_uniformMV = shader->getUniformLocation("MV");
        }
        /*speedup test*/
        int d_viewMatrixLoc;
        int d_projMatrixLoc;
        int d_modelMatrixLoc;
        /*optimize*/
        int d_uniformMVP;
        int d_uniformMV;
};
