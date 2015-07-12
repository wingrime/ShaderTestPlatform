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
        RenderContext(const RBO* _v,SShader* _s, SMat4x4 _V, SMat4x4 _P)
            :shader(_s),
            viewport(_v),
            d_V(_V),
            d_P(_P),
         	sm_map(false)
         {
            initUniforms();
        }
        RenderContext(const RBO* v,SShader* s,SMat4x4 _V, SMat4x4 _P, std::shared_ptr<SRBOTexture> sm_tex)
        	:sm_texture(sm_tex),
        	shader(s),
         	viewport(v),
            d_V(_V),
            d_P(_P),
         	sm_map(true)
        {
            initUniforms();
        }
        RenderContext(const RBO* v,SShader* s, SMat4x4 _V, SMat4x4 _P,
                    std::shared_ptr<SRBOTexture> sm_tex,
                    std::shared_ptr<SRBOTexture> rsm_normal_tex,
                    std::shared_ptr<SRBOTexture> rsm_vector_tex,
                    std::shared_ptr<SRBOTexture> rsm_albedo_tex)
        	:sm_texture(sm_tex),
        	shader(s),
         	viewport(v),
            d_V(_V),
            d_P(_P),
         	sm_map(true),
         	rsm_normal_texture(rsm_normal_tex),
         	rsm_vector_texture(rsm_vector_tex),
         	rsm_albedo_texture(rsm_albedo_tex)
        {
            initUniforms();

        }
        RenderContext(const RenderContext&) = delete;
    	SShader *shader;
    	const RBO *viewport;
        SMat4x4 d_V;
        SMat4x4 d_P;
        std::shared_ptr<SRBOTexture> sm_texture;
        std::shared_ptr<SRBOTexture> rsm_normal_texture;
        std::shared_ptr<SRBOTexture> rsm_vector_texture;
        std::shared_ptr<SRBOTexture> rsm_albedo_texture;
        std::shared_ptr<SRBOTexture> sh_bands;
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
    	bool sm_map;
};
