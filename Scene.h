#pragma once
/*Low-level render impl
* experimental educational project
*/
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/json.hpp>
#include <memory>
#include "Singltone.h"

#include "RBO.h"
#include "ObjModel.h"
#include "post_process.h"
#include "r_camera.h"
#include "r_perfman.h"
#include "UILabel.h"
#include "UIConsole.h"
#include "Log.h"
#include "Command.h"

#include "DebugDraw.h"
#include "MAssert.h"
#include "RenderState.h"
#include "DebugUI.h"

class SScene {
public:
    friend class DebugUI;
    SScene(RBO *v);
    int Render();

    int Reshape(int w, int h);

    std::vector< std::shared_ptr <SObjModel> > d_render_list;

    std::shared_ptr<SObjModel> sky_dome_model;

    SCamera cam;
    SCamera sky_cam;
    SCamera d_shadowmap_cam;
    SPostProcess *pp_stage_ssao;
    SPostProcess *pp_stage_ssao_blur_hor;
    SPostProcess *pp_stage_ssao_blur_vert;

    SPostProcess *pp_stage_hdr_bloom;
    SPostProcess *pp_stage_hdr_blur_hor;
    SPostProcess *pp_stage_hdr_blur_vert;

    /*for ping pong bloor*/
    SPostProcess *pp_stage_hdr_blur_hor2; 
    SPostProcess *pp_stage_hdr_blur_vert2;

    SPostProcess *pp_stage_hdr_tonemap;

    /*lumenance key*/
    SPostProcess *pp_stage_hdr_lum_key;
    SPostProcess *pp_stage_hdr_lum_log;


    SPostProcess *pp_stage_volumetric;
    std::shared_ptr<RBO> rtSCREEN;
    std::shared_ptr<RBO> rtHDRScene;
    std::shared_ptr<RBO> rtHDRScene_MSAA;
    std::shared_ptr<RBO> rtSSAOHorBlurResult;
    std::shared_ptr<RBO> rtShadowMap;
    std::shared_ptr<RBO> rtSSAOVertBlurResult;
    std::shared_ptr<RBO> rtHDRBloomResult;
    std::shared_ptr<RBO> rtHDRHorBlurResult;
    std::shared_ptr<RBO> rtHDRVertBlurResult;

    std::shared_ptr<RBO> rtHDRLogLum; /*Downsample input for total lum calculation*/
    std::shared_ptr<RBO> rtHDRLumKey; /* Out 1x1 texture with lum*/

    std::shared_ptr<RBO> rtVolumetric;


    std::shared_ptr<RBO> rtCubemap;

    std::shared_ptr<SRBOTexture> rtConvoledCubemap;

    SPerfMan rtime;
    SPerfMan ui_time;
    SPerfMan pp_time;

    /* flags*/
    bool rWireframe = false;
    bool rSSAO  = false;

    int toggleBrightPass(bool b);
    int toggleMSAA(bool b);


private:
    float step;
    bool d_first_render;

    /* main prog*/
    SShader *r_prog;
    /*shadow prog*/
    SShader *cam_prog;
    /*Sky*/
    SShader *sky_dome_prog;

    SShader *pp_prog_hdr_blur_kawase;

    SShader *pp_prog_hdr_tonemap; /* final tonemap*/

    SShader *cubemap_prog_generator;

    /*main scene render*/
    int RenderDirect(const RBO& v);
    /*render shadow map*/
    int RenderShadowMap(const RBO& v);
    int RenderCubemap();


private:

   RenderPass normal_pass;
   RenderPass msaa_pass;
   RenderPass ui_pass;



   bool d_toggle_fullscreen = false;
   bool d_toggle_MSAA = true;
   bool d_toggle_brightpass = true;
public:
    int UpdateScene(float dt);
private:

    DebugDraw d_debugDrawMgr;

public:
    DebugUI dbg_ui;

};
class MainScene :public Singltone<SScene>  ,public SScene{
public:
    MainScene(RBO *t) :SScene (t), Singltone(this){}
};
