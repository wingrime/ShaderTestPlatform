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
#include "PostProcess.h"
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

struct RenderPipelineStageConfig {
    /* pipline stage name */
    std::string stageName;
    /* stage buffer size*/
    RectSizeInt bufferSize;
    /* stage render output type*/
    RBO::RBOType rboType;
    /* shaders */
    std::string vertexShaderFileName;
    std::string fragmentShaderFileName;
    std::string geoShaderFileName;
    /*stage submit rules*/
    bool isPostProcess;
    std::string ppFeedStage1;
    std::string ppFeedStage2;
    std::string ppFeedStage3;
    std::string ppFeedStage4;

};
struct RenderPipelineStageRuntime {
    RenderPipelineStageConfig stageConfig;
    SShader * stageShader;
    RBO * stageRBO;
    SPostProcess * postProcess; //* used only with postprocess stages

};

class SScene {
public:
    /* new code for render stages*/
    RenderPipelineStageRuntime *initStage(RenderPipelineStageConfig *pipelineStage);
    RBO *lookupStageRBO(const std::string & stageName);
    SShader *lookupStageShader(const std::string & stageName);
    int renderPipelineLink() ;// resolve names,

private:
    std::vector<SShader *> d_pipelineShaders;
    std::vector<RenderPipelineStageRuntime* >d_pipelineRuntime;
    std::unordered_map<std::string, int> d_pipelineLookupMap;

public:
    friend class DebugUI;
    SScene(RectSizeInt v);
    ~SScene();
    int Render();

    int Reshape(int w, int h);

    std::vector< std::shared_ptr <SObjModel> > d_render_list;

    int AddObjectToRender(std::shared_ptr <SObjModel> obj);

    SCamera cam;
    SWeatherSky* w_sky;

    SCamera d_shadowmap_cam[4];
    SPostProcess * pp_stage_ssao;
    SPostProcess * pp_stage_ssao_blur_hor;
    SPostProcess * pp_stage_ssao_blur_vert;

    SPostProcess * pp_stage_hdr_bloom;
    SPostProcess * pp_stage_hdr_blur_hor;
    SPostProcess * pp_stage_hdr_blur_vert;

    /*for ping pong bloor*/
    SPostProcess * pp_stage_hdr_blur_hor2;
    SPostProcess * pp_stage_hdr_blur_vert2;

    SPostProcess * pp_stage_hdr_tonemap;

    /*lumenance key*/
    SPostProcess * pp_stage_hdr_lum_key;
    SPostProcess * pp_stage_hdr_lum_log;
    /*debug output*/
    SPostProcess * postProcessDebugOutput;


    SPostProcess * pp_stage_volumetric;
    RBO * rtSCREEN;
    RBO * rtPrepass;
    RBO * rtHDRScene;
    RBO * rtHDRScene_MSAA;
    RBO * rtSSAOHorBlurResult;
    RBO * rtShadowMap;
    RBO * rtSSAOVertBlurResult;
    RBO * rtHDRBloomResult;
    RBO * rtHDRHorBlurResult;
    RBO * rtHDRVertBlurResult;
    RBO * rtHDRLogLum; /*Downsample input for total lum calculation*/
    RBO * rtHDRLumKey; /* Out 1x1 texture with lum*/

    RBO * rtVolumetric;


    RBO * rtCubemap;

    SRBOTexture * texConvoledCubemap;

    STexture * texRandom;

    SPerfMan rtime;
    SPerfMan ui_time;
    SPerfMan pp_time;


    int toggleBrightPass(bool b);
    int toggleMSAA(bool b);
    int toggleSSAO(bool b);
    int regenerateEnvCubeMap();




private:
    bool d_first_render;

    /* prepass prog*/
    SShader *  prepass_prog;


   SShader * pp_prog_hdr_blur_kawase;

    SShader* pp_prog_hdr_tonemap; /* final tonemap*/

    SShader * cubemap_prog_generator;

    SShader * shaderViewAsIs;

    /*shortcuts for stages*/
    RenderPipelineStageRuntime * mainRenderPass;
    RenderPipelineStageRuntime * mainRenderPassMSAA;
    RenderPipelineStageRuntime * mainRenderTonemapPass;
    RenderPipelineStageRuntime * shadowMapPass;
    RenderPipelineStageRuntime * prePass;

    int RenderDirect(const RenderPipelineStageRuntime &runtime);
    int RenderShadowMap(const RBO& v);
    int RenderCubemap();
    int RenderPrepass(const RenderPipelineStageRuntime &runtime);
    int BlurKawase();

private:

   RenderPass normal_pass;
   RenderPass msaa_pass;
   RenderPass ui_pass;

   bool d_toggle_fullscreen = false;
   bool d_toggle_MSAA = true;
   bool d_toggle_brightpass = true;
   bool d_toggle_ssao = true;
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

    int debugSetFinalRenderOutput(RBO * r);
    int debugSetDebugRenderOutputFlag(bool flag);

    RBO * debugFinalRenderOutput;
    bool debugRenderOutputFlag = false;

    Recorder rec;
    bool d_play = false;


    Recorder getRec() const;
    void setRec(const Recorder &value);
};
#define MainScene Singltone<SScene>
