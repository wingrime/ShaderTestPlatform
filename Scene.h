#pragma once
#include "RBO.h"
#include "ObjModel.h"
#include "post_process.h"
#include "r_camera.h"
#include "r_perfman.h"
#include "UILabel.h"
#include "UIConsole.h"
#include "Log.h"
#include "Command.h"
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/json.hpp>
#include <memory>
#include "DebugDraw.h"

class SScene {
public:
    SScene(RBO *v);
    int Render();

    int Reshape(int w, int h);

    int UpdateCfgLabel();
    int UpdateViewSelLabel();


    int upCfgItem();
    int downCfgItem();
    int incCfgItem();
    int decCfgItem();

    int upViewItem();
    int downViewItem();


    
    std::shared_ptr<SObjModel> model;
    std::shared_ptr<SObjModel> sky_dome_model;
    std::shared_ptr<SObjModel> test_sphere_model;

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

   // lua
    //sel::State state{true};
private:
    float step;
    bool d_first_render;
    std::shared_ptr<UILabel> fps_label;
    std::shared_ptr<UILabel> cfg_label;
    std::shared_ptr<UILabel> v_sel_label;

    int d_cfg_current = 0;
    int d_cfg_max = 14;
    /*default parameter set*/
    float  d_cfg [15] = {0.012, 0.006,0.720,0.26,2.0,0.75,0.015,0.22,0.30,0.10,0.20,0.01,0.30,1.12,1.6};
    float d_cfg_step = 0.001;


    /* main prog*/
    SShader *r_prog;
    /*shadow prog*/
    SShader *cam_prog;
    /*Sky*/
    SShader *sky_dome_prog;

    SShader *pp_prog_hdr_blur_kawase;

    SShader *pp_prog_hdr_tonemap; /* final tonemap*/

    SShader *cubemap_prog_generator;


    enum {
    	V_NORMAL,
    	V_BLOOM,
        V_BLOOM_BLEND,
        V_SSAO,
    	V_DIRECT,
        V_SHADOW_MAP,
        V_VOLUMETRIC,
        V_CUBEMAPTEST,
    	V_MAX,
    };

    /*view selecor*/
    int d_v_sel_current = 0;
    int d_v_sel [V_MAX] = {1 };
    const int d_v_sel_max = V_MAX-1;


    const inline std::string C_I(int n);
    const inline std::string V_I(int n);

    /*main scene render*/
    int RenderDirect(const RBO& v);
    /*render shadow map*/
    int RenderShadowMap(const RBO& v);

    int RenderCubemap();


    std::shared_ptr<ConsoleCommandHandler> d_console_cmd_handler;
public:
   UIConsole *con; 
private:
   UIConsoleErrorHandler *err_con;

   bool d_toggle_fps_view = true; 
   bool d_toggle_cfg_view = true;
   bool d_toggle_fullscreen = false;
   bool d_toggle_MSAA = true;
public:
    int UpdateScene();

private:
    int InitDebugCommands();

    DebugDraw d_debugDrawMgr;

};
