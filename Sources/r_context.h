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
        RenderContext(const RBO* v,SShader* s, SCamera *c)
        	:shader(s),
         	viewport(v),
         	camera(c),
         	sm_map(false)
         {
            initUniforms();
        }
        RenderContext(const RBO* v,SShader* s, SCamera *c, std::shared_ptr<SRBOTexture> sm_tex)
        	:sm_texture(sm_tex),
        	shader(s),
         	viewport(v),
         	camera(c),
         	sm_map(true)
        {
            initUniforms();
        }
        RenderContext(const RBO* v,SShader* s, SCamera *c, 
                    std::shared_ptr<SRBOTexture> sm_tex,
                    std::shared_ptr<SRBOTexture> rsm_normal_tex,
                    std::shared_ptr<SRBOTexture> rsm_vector_tex,
                    std::shared_ptr<SRBOTexture> rsm_albedo_tex)
        	:sm_texture(sm_tex),
        	shader(s),
         	viewport(v),
         	camera(c),
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
    	SCamera *camera;
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
            d_MVP = shader->getUniformLocation("MVP");
            d_MV = shader->getUniformLocation("MV");
        }
        /*speedup test*/
        int d_viewMatrixLoc;
        int d_projMatrixLoc;
        int d_modelMatrixLoc;
        /*optimize*/
        int d_MVP;
        int d_MV;
    	bool sm_map;
};
