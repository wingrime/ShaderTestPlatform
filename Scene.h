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
#include "WeatherSky.h"

class SScene {
public:
    friend class DebugUI;
    SScene(RectSizeInt v);
    ~SScene();
    int Render();

    int Reshape(int w, int h);

    std::vector< std::shared_ptr <SObjModel> > d_render_list;

    int AddObjectToRender(std::shared_ptr <SObjModel> obj);


    std::shared_ptr<SWeatherSky> w_sky;

    SCamera cam;
    SCamera d_shadowmap_cam[4];
    std::shared_ptr<SPostProcess> pp_stage_ssao;
    std::shared_ptr<SPostProcess> pp_stage_ssao_blur_hor;
    std::shared_ptr<SPostProcess> pp_stage_ssao_blur_vert;

    std::shared_ptr<SPostProcess> pp_stage_hdr_bloom;
    std::shared_ptr<SPostProcess> pp_stage_hdr_blur_hor;
    std::shared_ptr<SPostProcess> pp_stage_hdr_blur_vert;

    /*for ping pong bloor*/
    std::shared_ptr<SPostProcess> pp_stage_hdr_blur_hor2;
    std::shared_ptr<SPostProcess> pp_stage_hdr_blur_vert2;

    std::shared_ptr<SPostProcess> pp_stage_hdr_tonemap;

    /*lumenance key*/
    std::shared_ptr<SPostProcess> pp_stage_hdr_lum_key;
    std::shared_ptr<SPostProcess> pp_stage_hdr_lum_log;
    /*debug output*/
    std::shared_ptr<SPostProcess> postProcessDebugOutput;


    std::shared_ptr<SPostProcess> pp_stage_volumetric;
    std::shared_ptr<RBO> rtSCREEN;
    std::shared_ptr<RBO> rtPrepass;
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

    /* prepass prog*/
    std::shared_ptr<SShader> prepass_prog;
    /* main prog*/
    std::shared_ptr<SShader> main_pass_shader;
    /*shadow prog*/
    std::shared_ptr<SShader> cam_prog;

    std::shared_ptr<SShader> pp_prog_hdr_blur_kawase;

    std::shared_ptr<SShader> pp_prog_hdr_tonemap; /* final tonemap*/

    std::shared_ptr<SShader> cubemap_prog_generator;

    std::shared_ptr<SShader> shaderViewAsIs;

    int RenderDirect(const RBO& v);
    int RenderShadowMap(const RBO& v);
    int RenderCubemap();
    int RenderPrepass(const RBO& v);
    int BlurKawase();

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
    int UpdateCfgLabel();
    float  d_cfg [15] = {0.020, 0.40,0.00,0.26,2.0,0.75,0.015,0.22,0.30,0.10,0.20,0.01,0.30,1.12,4.6};
public:
    DebugUI dbg_ui;
    /* shadow map debug*/
    AABB cameraFrustrumAABB[4];
    AABB cameraTransformFrustrumAABB[4];

    int debugSetFinalRenderOutput(std::shared_ptr<RBO> r);
    int debugSetDebugRenderOutputFlag(bool flag);

    std::shared_ptr<RBO> debugFinalRenderOutput;
    bool debugRenderOutputFlag = false;


};
class MainScene :public Singltone<SScene>  ,public SScene{
public:
    MainScene(RectSizeInt t) :SScene (t), Singltone(this){}
};
