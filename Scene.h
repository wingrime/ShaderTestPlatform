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
#include "Camera.h"
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
#include "ProjectionMatrix.h"

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
    friend class DebugUI;
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
    SScene(RectSizeInt v);
    ~SScene();
    int Render();

    int Reshape(RectSizeInt v);

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
    bool d_RegenerateCubemap;
    SMat4x4 infFarMatrix = SInfinityFarProjectionMatrix(100,1.0,toRad(26.0));

    /* prepass prog*/
    SShader *  prepass_prog;
    SShader * pp_prog_hdr_blur_kawase;
    SShader * pp_prog_hdr_tonemap; /* final tonemap*/
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
    int BlurKawase(float factor);

    int UpdateShadowmap();

private:

   RenderPass normal_pass;
   RenderPass msaa_pass;
   RenderPass ui_pass;

   bool d_toggle_fullscreen = false;
   bool d_toggle_MSAA = true;
   bool d_toggle_brightpass = true;
   bool d_toggle_ssao = true;
   float d_BloomFactor;
public:
    int ResetCfgLabel();
    int UpdateScene(float dt);
private:
    DebugDraw d_debugDrawMgr;       
public:
    int debugSetFinalRenderOutput(RBO * r);
    int debugSetDebugRenderOutputFlag(bool flag);
    float debugGetRenderTime();
    float debugGetPostProcessingTime();
    long int debugGetFrameNumber();
    int debugSetStageShaderUniform(const std::string& stage, const std::string& var, float val);
    
    /* shadow map debug*/
    AABB cameraFrustrumAABB[4];
    AABB cameraTransformFrustrumAABB[4];
    RBO * debugFinalRenderOutput;
    float d_dbgRenderTimeMs;
    float d_dbgPostProcessingTimeMs;
    long int d_dbgFrameNumber;
    bool debugRenderOutputFlag = false;

    /*Input recoder for debug*/
    Recorder rec;
    bool d_play = false;
    Recorder getRec() const;
    void setRec(const Recorder &value);
};
