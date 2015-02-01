#include "Scene.h"
#include "r_cprog.h"
#include <chrono>
#include "c_config.h"
#include "r_context.h"

#include "RenderState.h"

/////PROTO//////
/*serialization*/
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>

#include <type_traits>
#include <memory.h>
#include "r_sprog.h"
struct RenderPipelinePass {
    int pass_number; /*data format reference*/
    int out_buffer_width;
    int out_buffer_height;
    int out_buffer_count; /* count of exit buffers*/
    std::vector<SRBOTexture::RTType> buffer_type; /*out buffer s*/
    RBO::RBOType in_buffer_type;

    int reference_material_number;

    /*serialize support */
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar( CEREAL_NVP(pass_number),
            CEREAL_NVP(out_buffer_width),
            CEREAL_NVP(out_buffer_height),
            CEREAL_NVP(out_buffer_count),
            CEREAL_NVP(in_buffer_type),
            CEREAL_NVP(buffer_type)
            );
    }
};
struct RenderPiplineMaterial {
    int material_number;
    std::string vertex_file;
    std::string fragment_file;
    std::string geom_file;
    /*serialize support */
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar( CEREAL_NVP(material_number),
            CEREAL_NVP(vertex_file),
            CEREAL_NVP(fragment_file),
            CEREAL_NVP(geom_file)
            );
    }
};

class RenderPipeline {
public:
    RenderPipeline(std::vector<RenderPipelinePass> passes ,std::vector<RenderPiplineMaterial> materials )  :d_passes(passes), d_materials(materials) {}
    int InitMaterials();
    int InitPasses();
    int BindPass(int pass_id);
private:
    /*serializable data*/
    std::vector<RenderPipelinePass> d_passes;
    std::vector<RenderPiplineMaterial> d_materials;
    std::vector<std::shared_ptr<SShader> > d_shaders;
    std::vector<std::shared_ptr<RBO> > d_RBOs;
};
int RenderPipeline::InitPasses() {
    for (auto &pass : d_passes) {
        switch (pass.out_buffer_count)
        {
        case 1:
            d_RBOs.push_back(std::shared_ptr<RBO> (new RBO(pass.out_buffer_width, pass.out_buffer_height,pass.in_buffer_type,
                                                      pass.buffer_type[0],1,
                                                      SRBOTexture::RT_NONE,1,
                                                      SRBOTexture::RT_NONE,1)));
            break;
        case 2:
            d_RBOs.push_back(std::shared_ptr<RBO> (new RBO(pass.out_buffer_width, pass.out_buffer_height,pass.in_buffer_type,
                                                      pass.buffer_type[0],1,
                                                      pass.buffer_type[1],1,
                                                      SRBOTexture::RT_NONE,1)));
            break;
        case 3:
            d_RBOs.push_back(std::shared_ptr<RBO> (new RBO(pass.out_buffer_width, pass.out_buffer_height,pass.in_buffer_type,
                                                      pass.buffer_type[0],1,
                                                      pass.buffer_type[1],1,
                                                      pass.buffer_type[2],1)));
            break;
        default:
            MASSERT(true);
        }
    }

}

int RenderPipeline::InitMaterials() {

    for (auto &material : d_materials) {
        d_shaders.push_back(std::shared_ptr<SShader> (new SShader(material.vertex_file, material.fragment_file,material.geom_file)));
    }
    return 0;
}
SScene::SScene(RBO *v) 
    :rtSCREEN(v)

    ,d_shadowmap_cam(SMat4x4(),SPerspectiveProjectionMatrix(100.0f, 10000.0f,1.0f,toRad(26.0)))
    ,cam(SMat4x4(),SPerspectiveProjectionMatrix(100.0f, 10000.0f,1.0f,toRad(26.0)))
    ,sky_cam(SMat4x4(),SPerspectiveProjectionMatrix(100.0f, 10000.0f,1.0f,toRad(26.0)))
    ,step(0.0f)
    ,normal_pass(RenderPass::LESS_OR_EQUAL,RenderPass::ENABLED,RenderPass::DISABLED )
    ,msaa_pass(RenderPass::LESS_OR_EQUAL,RenderPass::ENABLED,RenderPass::ENABLED)
    ,ui_pass(RenderPass::NEVER, RenderPass::DISABLED,RenderPass::DISABLED)
    
    ,rtShadowMap(new RBO(v->getSize().w,v->getSize().h ,RBO::RBO_RGBA,SRBOTexture::RT_TEXTURE_RGBA,1,
                                                    SRBOTexture::RT_NONE,1,
                                                    SRBOTexture::RT_NONE,1 ))
    ,rtHDRScene_MSAA(new RBO(v->getSize().w,v->getSize().h ,RBO::RBO_MSAA,SRBOTexture::RT_TEXTURE_MSAA,1,
                                                   SRBOTexture::RT_TEXTURE_MSAA,1,
                                                   SRBOTexture::RT_NONE, 1 ))
    ,rtHDRScene(new RBO(v->getSize().w,v->getSize().h ,RBO::RBO_FLOAT,SRBOTexture::RT_TEXTURE_FLOAT,1,
                                                   SRBOTexture::RT_TEXTURE_FLOAT,1,
                                                   SRBOTexture::RT_NONE, 1 ))
    ,rtHDRBloomResult( new RBO(v->getSize().w/2,v->getSize().h/2, RBO::RBO_FLOAT_RED)) /* is it better HDR */
    ,rtHDRHorBlurResult(new RBO(v->getSize().w/4,v->getSize().h/4, RBO::RBO_FLOAT_RED))
    ,rtHDRVertBlurResult(new RBO(v->getSize().w/4,v->getSize().h/4, RBO::RBO_FLOAT_RED))
    ,rtSSAOVertBlurResult( new RBO(v->getSize().w/2,v->getSize().h/2, RBO::RBO_RED))
    ,rtSSAOHorBlurResult(new RBO(v->getSize().w/2,v->getSize().h/2, RBO::RBO_RED))
    ,rtVolumetric(new RBO(v->getSize().w,v->getSize().h, RBO::RBO_RGBA))
    ,sky_dome_model(new SObjModel("sky_dome.obj"))
    ,rtCubemap(new RBO(128, 128, RBO::RBO_CUBEMAP))
   ,rtConvoledCubemap(new SRBOTexture(10,4,SRBOTexture::RT_TEXTURE_FLOAT_RED))
    ,rtHDRLogLum(new RBO(16,16,RBO::RBO_FLOAT)) /*Downsampled source for lumeneace*/
    ,rtHDRLumKey(new RBO(1,1,RBO::RBO_FLOAT)) /*Lum key out*/
    ,dbg_ui(this,v)
    
{
    /*load configuration*/
    int w = rtHDRScene->getSize().w;
    int h = rtHDRScene->getSize().h;
    /*bloom shaders */
    pp_prog_hdr_tonemap = new SShader("PostProcessing/PostProccessQuard.vert",\
                                      "PostProcessing/Tonemap/Filmic.frag");

    pp_prog_hdr_blur_kawase = new SShader("PostProcessing/PostProccessQuard.vert",\
                                          "PostProcessing/Bloor/Kawase.frag");

    pp_stage_ssao =  new SPostProcess(new SShader("PostProcessing/PostProccessQuard.vert",\
                                                  "PostProcessing/SSAO/SimpleSSAO.frag") \
                                      ,w/2,h/2,rtHDRScene->texIMG1(),rtHDRScene->texDEPTH());

    pp_stage_ssao_blur_hor=  new SPostProcess(new SShader("PostProcessing/PostProccessQuard.vert",\
                                                          "PostProcessing/Bloor/GaussHorizontal.frag"),\
                                              rtSSAOHorBlurResult,rtSSAOVertBlurResult);
    pp_stage_ssao_blur_vert =  new SPostProcess(new SShader("PostProcessing/PostProccessQuard.vert", \
                                                            "PostProcessing/Bloor/GaussVertical.frag"),\
                                                rtSSAOVertBlurResult,rtSSAOHorBlurResult);

    pp_stage_hdr_bloom =  new SPostProcess(new SShader("PostProcessing/PostProccessQuard.vert",\
                                                       "PostProcessing/Bloom/Clamp.frag" ),\
                                                rtHDRBloomResult,rtHDRScene,rtHDRLumKey);
    pp_stage_hdr_blur_hor =   new SPostProcess(pp_prog_hdr_blur_kawase, rtHDRHorBlurResult ,rtHDRBloomResult);
    pp_stage_hdr_blur_vert =  new SPostProcess(pp_prog_hdr_blur_kawase, rtHDRVertBlurResult ,rtHDRHorBlurResult);
    /*ping pong*/
    pp_stage_hdr_blur_hor2 =   new SPostProcess(pp_prog_hdr_blur_kawase,rtHDRHorBlurResult ,rtHDRVertBlurResult);
    pp_stage_hdr_blur_vert2 =  new SPostProcess(pp_prog_hdr_blur_kawase,rtHDRVertBlurResult,rtHDRHorBlurResult);

    pp_stage_hdr_lum_log = new SPostProcess(new SShader ("PostProcessing/PostProccessQuard.vert",\
                                                         "PostProcessing/Tonemap/LumLog.frag")
                                            ,rtHDRLogLum,rtHDRScene);

    pp_stage_hdr_lum_key = new SPostProcess(new SShader ("PostProcessing/PostProccessQuard.vert",\
                                                        "PostProcessing/Tonemap/LumKey.frag")
                                          ,rtHDRLumKey,rtHDRLogLum,rtHDRLumKey);

    /*final tonemap*/
    pp_stage_hdr_tonemap =  new SPostProcess(pp_prog_hdr_tonemap,rtSCREEN,rtHDRVertBlurResult,rtHDRScene,rtHDRLumKey,rtSSAOVertBlurResult);

    /* img depth | shadow depth | shadow world pos*/
    pp_stage_volumetric =  new SPostProcess(new SShader("PostProcessing/PostProccessQuard.vert", \
                                                        "PostProcessing/Volumetric/Test.frag") \
                                            ,w,h,rtHDRScene->texDEPTH(),rtShadowMap->texDEPTH(),rtShadowMap->texIMG2());

    /*main prog*/
    r_prog = new SShader("Main/Main.vert","Main/Main.frag");

    cam_prog = new SShader("Shadow/Direct.vert","Shadow/Direct.frag");

    /*sky dome prog*/
    sky_dome_prog = new SShader("Sky/PerezSky.vert","Sky/PerezSky.frag");

    cubemap_prog_generator = new SShader("Cubemap/cubemap_gen.vert","Cubemap/cubemap_gen.frag","Cubemap/cubemap_gen.geom");


    d_render_list.push_back(std::shared_ptr<SObjModel> (new SObjModel("sponza.obj")) );
    d_render_list.push_back(std::shared_ptr<SObjModel> (new SObjModel("sky_dome.obj")) );
    d_render_list[1]->SetModelMat(SMat4x4().Scale(2.0,2.0,2.0).Move(0.0,200.0,0.0));

    for (auto& r : d_render_list ) {
        r->ConfigureProgram(*r_prog);
    }



    sky_dome_model->ConfigureProgram( *sky_dome_prog);
    sky_dome_model->SetModelMat(SMat4x4().Scale(1000.0,1000.0,1000.0));

    dbg_ui.Init();
    d_shadowmap_cam.LookAt(SVec4( 0.0,0.0, 0.0,1.0),SVec4(0.0,6000,0.0,1.0) ,   SVec4(1.0,0.0,0.0,1.0) );
    d_first_render = false;
}

int SScene::Reshape(int w, int h) {

        rtSCREEN->Resize(SVec2(w,h));
        /* there should be internal buffer recreation code but ?? */
        glViewport ( 0, 0, (GLsizei)w, (GLsizei)h );
        /*fix aspect ratio*/
        cam.setProjMatrix( SPerspectiveProjectionMatrix(100.0f, 10000.0f,(float)h / (float)w,toRad(26.0)) );

        return ESUCCESS;
}

int SScene::toggleBrightPass(bool b)
{
    d_toggle_brightpass = b;
}

int SScene::toggleMSAA(bool b)
{
    d_toggle_MSAA = b;
}
int SScene::UpdateScene(float dt) {
   sky_cam.SyncFromCamera(cam);
    r_prog->Bind();
    r_prog->SetUniform("sm_projection_mat",d_shadowmap_cam.getProjMatrix());
    r_prog->SetUniform("sm_view_mat",d_shadowmap_cam.getViewMatrix());
    return 0;

}


int inline SScene::RenderShadowMap(const RBO& v) {
    v.Bind(true);
    RenderContext r_ctx(&v, cam_prog ,&d_shadowmap_cam);
    for (auto& r : d_render_list ) {
        r->Render(r_ctx);
    }
    return ESUCCESS;
}

int SScene::RenderCubemap()
{
    glClearColor(1.0,1.0,1.0,1.0);
    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glClampColor(GL_CLAMP_READ_COLOR, GL_FALSE);
    if (d_toggle_MSAA)
       msaa_pass.Bind();
    else
       normal_pass.Bind();

    static int step = 0;
    step ++;
    rtCubemap->Bind();
    SMat4x4 pos = SMat4x4().Move(0.0+step*100,-200.0,0.0);
    d_debugDrawMgr.AddCross({(float)(0.0-step*100.0),200,1.0},50);
    d_debugDrawMgr.Update();
    SCamera cubemap_cam(pos,SPerspectiveProjectionMatrix(10,10000,1,toRad(90.0)));
    RenderContext r_ctx(rtCubemap.get() , cubemap_prog_generator ,&cubemap_cam);
    RenderContext r_ctx2(rtCubemap.get() , r_prog ,&cubemap_cam);

    for (auto& r : d_render_list ) {
        r->Render(r_ctx);
    }
    SCProg cs("Cubemap/cubemap_convolve.comp");
    cs.Barrier();
    cs.Use();
    rtCubemap->texIMG()->Bind(1);
    cs.SetUniform("srcCube",1);
    cs.Use();
    rtConvoledCubemap->BindImage(2);
    cs.SetUniform("dstSHBands",2);

    cs.SetUniform("dstSamplesTotal",100*100);
    cs.SetUniform("dstSHBandY",1);

    cs.Dispatch(100,1 ,1);
    cs.Barrier();

}
int inline SScene::RenderDirect(const RBO& v) {
    v.Bind(true);    
    if (rWireframe)
    {
        RenderContext r_ctx(&v, cam_prog ,&d_shadowmap_cam);
        for (auto& r : d_render_list ) {
            r->Render(r_ctx);
        }
    }
    else
    {
        RenderContext r_ctx(&v, r_prog ,&cam,rtShadowMap->texDEPTH(),rtShadowMap->texIMG1(), rtCubemap->texIMG(), rtShadowMap->texIMG());
        r_ctx.sh_bands = rtConvoledCubemap;
        for (auto& r : d_render_list ) {
            r->Render(r_ctx);
        }
        RenderContext r_ctx2(&v, sky_dome_prog ,&cam,rtShadowMap->texDEPTH(),rtShadowMap->texIMG1() ,rtCubemap->texIMG(), rtShadowMap->texIMG());
        sky_dome_model->Render(r_ctx2);
    }

    d_debugDrawMgr.Render(cam.getProjMatrix()*cam.getViewMatrix());

    return 0;
}
int SScene::Render() {
    auto start = std::chrono::steady_clock::now();
    step  += 0.002f;
    glClearColor(0.0,0.0,0.0,1.0);
    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); 
    glClampColor(GL_CLAMP_READ_COLOR, GL_FALSE);
    UpdateScene(0.01); /*add real ms*/
    if (d_toggle_MSAA)
       msaa_pass.Bind();
    else
       normal_pass.Bind();
    if (!d_first_render ) {
        d_first_render = true;
        RenderCubemap();
    }
    rtime.Begin();
    if (dbg_ui.d_v_sel_current == DebugUI::V_DIRECT) {        
        RenderDirect( *rtSCREEN);

    } else {
        RenderShadowMap( *rtShadowMap);
        if (d_toggle_MSAA) {
            RenderDirect( *rtHDRScene_MSAA);
           rtHDRScene_MSAA->ResolveMSAA(*rtHDRScene);
        } else
            RenderDirect( *rtHDRScene);
    }
    rtime.End();
    pp_time.Begin();
    ui_pass.Bind();

    /*Bloom + SSAO + RenderShadowMap*/
    if (dbg_ui.d_v_sel_current == DebugUI::V_NORMAL) {
        /*LumKEY*/

        pp_stage_hdr_lum_log->DrawRBO(false);
        pp_stage_hdr_lum_key->DrawRBO(false);
        if (d_toggle_brightpass ) {

        pp_stage_hdr_bloom->DrawRBO(false);
        int blurSizeLoc = pp_prog_hdr_blur_kawase->getUniformLocation("blurSize");
        pp_prog_hdr_blur_kawase->SetUniform(blurSizeLoc,(float)0.0);
        pp_stage_hdr_blur_hor->DrawRBO(false);
        pp_prog_hdr_blur_kawase->SetUniform(blurSizeLoc,(float)(dbg_ui.d_cfg[6]*1.0));
        pp_stage_hdr_blur_vert->DrawRBO(false);

        /*ping pong 1*/
        pp_prog_hdr_blur_kawase->SetUniform(blurSizeLoc,(float)(dbg_ui.d_cfg[6]*2.0));
        pp_stage_hdr_blur_hor2->DrawRBO(false);
        pp_prog_hdr_blur_kawase->SetUniform(blurSizeLoc,(float)(dbg_ui.d_cfg[6]*2.0));
        pp_stage_hdr_blur_vert2->DrawRBO(false);
        /*ping pong 2*/
        pp_prog_hdr_blur_kawase->SetUniform(blurSizeLoc,(float)(dbg_ui.d_cfg[6]*3.0));
        pp_stage_hdr_blur_hor2->DrawRBO(false);
        pp_prog_hdr_blur_kawase->SetUniform(blurSizeLoc,(float)(dbg_ui.d_cfg[6]*3.0));
        pp_stage_hdr_blur_vert2->DrawRBO(false);
        } else {
            pp_stage_hdr_blur_vert2->getResultRBO()->Bind(true); /*cleanup*/
        }
        /*SSAO*/
        rtSSAOVertBlurResult->Bind(false);
        pp_stage_ssao->Draw();
        pp_stage_ssao_blur_hor->DrawRBO(false);
        pp_stage_ssao_blur_vert->DrawRBO(false);
        pp_stage_hdr_tonemap->DrawRBO(false);

    } else if (dbg_ui.d_v_sel_current == DebugUI::V_BLOOM) {
        rtSCREEN->Bind(true); 
        pp_stage_hdr_bloom->Draw();
        
    } else if (dbg_ui.d_v_sel_current == DebugUI::V_BLOOM_BLEND) {
      rtHDRBloomResult->Bind(false);
        pp_stage_hdr_bloom->Draw(); 

        rtHDRHorBlurResult->Bind(false);
        pp_stage_hdr_blur_hor->Draw();
        rtHDRVertBlurResult->Bind(false);
        pp_stage_hdr_blur_vert->Draw();
        /*ping pong 1*/
        rtHDRHorBlurResult->Bind(false);
        pp_stage_hdr_blur_hor2->Draw();
        rtHDRVertBlurResult->Bind(false);
        pp_stage_hdr_blur_vert2->Draw();
        /*ping pong 2*/
        rtHDRHorBlurResult->Bind(false);
        pp_stage_hdr_blur_hor2->Draw();
        rtSCREEN->Bind(true);
        pp_stage_hdr_blur_vert2->Draw();
    } else if (dbg_ui.d_v_sel_current == DebugUI::V_SSAO) {
        //rtSSAOVertBlurResult->Bind(true);
        rtSCREEN->Bind(true);
        pp_stage_ssao->Draw();
        //rtSSAOHorBlurResult->Bind(true);
        //pp_stage_ssao_blur_hor->Draw();
        //rtSCREEN->Bind(true);
        //pp_stage_ssao_blur_vert->Draw();
    } else if (dbg_ui.d_v_sel_current == DebugUI::V_SHADOW_MAP) {
        rtSCREEN->Bind(true); 
        RenderShadowMap( *rtSCREEN);
    } else if (dbg_ui.d_v_sel_current == DebugUI::V_VOLUMETRIC) {
        rtSCREEN->Bind(true); 

        pp_stage_volumetric->getShader()->SetUniform("sm_proj_matrix",d_shadowmap_cam.getProjMatrix());
        pp_stage_volumetric->getShader()->SetUniform("sm_view_matrix",d_shadowmap_cam.getViewMatrix());
        pp_stage_volumetric->getShader()->SetUniform("cam_proj_matrix",cam.getProjMatrix());
        pp_stage_volumetric->getShader()->SetUniform("cam_view_matrix",cam.getViewMatrix());
        pp_stage_volumetric->Draw();
    }
    else if (dbg_ui.d_v_sel_current == DebugUI::V_CUBEMAPTEST ) {
        rtSCREEN->Bind(true);
        pp_stage_hdr_lum_log->Draw();
    }
    pp_time.End();
    ui_time.Begin();
    dbg_ui.Draw();
    ui_time.End();

    char buf [90];
    auto end = std::chrono::steady_clock::now();

    auto diff = end- start;

    sprintf(buf,"DRAW:%4.3f ms\nUI: %4.3f ms\nPP: %4.3f ms\nCPU: %4.3f\n",      (float)rtime.getTime()*(1.0/ 1000000.0), \
                                                                    (float)ui_time.getTime()*(1.0/1000000.0),\
                                                                    (float)pp_time.getTime()*(1.0/1000000.0),\
                                                                      std::chrono::duration <float, std::milli> (diff).count() );
    dbg_ui.fps_label->setText(buf);
   
    return true;
}
