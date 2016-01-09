#include "Scene.h"
#include "Compute.h"
#include <chrono>
#include "c_config.h"
#include "RenderContext.h"
#include "math.h"
#include "mat_math.h"
#include "RenderState.h"
#include "Texture.h"
#include "AABB.h"

/////PROTO//////
/*serialization*/
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>

#include <type_traits>
#include <memory.h>

#include "Shader.h"
RenderPipelineStageRuntime * SScene::initStage(RenderPipelineStageConfig *pipelineStage)
{
    /*TODO: add RBO with many out buffers*/
    RBO * nRBO = new RBO(pipelineStage->stageName,pipelineStage->bufferSize.w,pipelineStage->bufferSize.h,pipelineStage->rboType);
    SShader * nShader = new SShader(pipelineStage->vertexShaderFileName,pipelineStage->fragmentShaderFileName,pipelineStage->geoShaderFileName);

    RenderPipelineStageRuntime * nRuntime =  new RenderPipelineStageRuntime;
    nRuntime->stageConfig = *pipelineStage;
    nRuntime->stageRBO = nRBO;
    nRuntime->stageShader = nShader;

    d_pipelineRuntime.push_back(nRuntime);
    int runtimeId = d_pipelineRuntime.size() - 1;
    d_pipelineLookupMap[pipelineStage->stageName]  = runtimeId;
    return nRuntime;
}
/*Todo handle errors*/
RBO *SScene::lookupStageRBO(const std::string &stageName)
{
    int idRuntime = d_pipelineLookupMap[stageName];
    return d_pipelineRuntime[idRuntime]->stageRBO;
}

SShader *SScene::lookupStageShader(const std::string &stageName)
{
    int idRuntime = d_pipelineLookupMap[stageName];
    return d_pipelineRuntime[idRuntime]->stageShader;
}

int SScene::renderPipelineLink()
{
     for (auto& rt : d_pipelineRuntime ) {
         if (rt->stageConfig.isPostProcess == true) {
           const std::string &feed1 =  rt->stageConfig.ppFeedStage1;
           const std::string &feed2 =  rt->stageConfig.ppFeedStage2;
           const std::string &feed3 =  rt->stageConfig.ppFeedStage3;
           const std::string &feed4 =  rt->stageConfig.ppFeedStage4;

           RBO*  feed1Stage = (d_pipelineRuntime[d_pipelineLookupMap[feed1]])->stageRBO;
           RBO*  feed2Stage = (d_pipelineRuntime[d_pipelineLookupMap[feed2]])->stageRBO;
           RBO*  feed3Stage = (d_pipelineRuntime[d_pipelineLookupMap[feed3]])->stageRBO;
           RBO*  feed4Stage = (d_pipelineRuntime[d_pipelineLookupMap[feed4]])->stageRBO;

           /*TODO:check for strages*/

           rt->postProcess = new SPostProcess(rt->stageShader,rt->stageRBO, \
                                              feed1Stage,feed2Stage,feed3Stage,feed4Stage);

         }
     }
    return 0;
}

SScene::SScene(RectSizeInt v)
    :cam(SMat4x4(),SPerspectiveProjectionMatrix(100.0f, 7000.0f,1.0f,toRad(26.0)))
    ,normal_pass(RenderPass::LESS_OR_EQUAL,RenderPass::ENABLED,RenderPass::DISABLED )
    ,msaa_pass(RenderPass::LESS_OR_EQUAL,RenderPass::ENABLED,RenderPass::ENABLED)
    ,ui_pass(RenderPass::NEVER, RenderPass::DISABLED,RenderPass::DISABLED)
    ,d_RegenerateCubemap(true)
    ,d_dbgFrameNumber(0)


    //,dbg_ui(this,v)
    
{
    d_BloomFactor = 0.1;
    /*Weather system*/
    w_sky = new SWeatherSky();
    /*scene noise*/
    texRandom = new STexture("AssetBase/noise.png");
    texRandom->setInterpolationMode(STexture::InterpolationType::TEX_NEAREST);

    texConvoledCubemap = new SRBOTexture(RectSizeInt(10,4),SRBOTexture::RT_TEXTURE_FLOAT_RED);
    texConvoledCubemap->setInterpolationMode(STexture::InterpolationType::TEX_NEAREST);
    /*load configuration*/
    int w = v.w;
    int h = v.h;
    RenderPipelineStageConfig shadowMapStage;
    shadowMapStage.stageName = std::string("Shadowmap");
    shadowMapStage.bufferSize = RectSizeInt(h,w);
    shadowMapStage.fragmentShaderFileName = std::string("Shadow/Cascade.frag");
    shadowMapStage.geoShaderFileName = std::string("Shadow/Cascade.geom");
    shadowMapStage.vertexShaderFileName = std::string("Shadow/Cascade.vert");
    shadowMapStage.rboType = RBO::RBO_DEPTH_ARRAY_ONLY;
    shadowMapPass = initStage(&shadowMapStage);
    /*short path*/

    rtShadowMap = lookupStageRBO(shadowMapStage.stageName);

    RenderPipelineStageConfig depthPrePassStage;
    depthPrePassStage.stageName = std::string("DepthPrePass");
    depthPrePassStage.bufferSize = RectSizeInt(h/2,w/2);
    depthPrePassStage.fragmentShaderFileName = std::string("DepthNormalPrepass/DepthNormalPrepass.frag");
    depthPrePassStage.vertexShaderFileName = std::string("DepthNormalPrepass/DepthNormalPrepass.vert");
    depthPrePassStage.rboType = RBO::RBO_RGBA;
    depthPrePassStage.isPostProcess = false;
    prePass = initStage(&depthPrePassStage);
    /*short path*/

    rtPrepass = lookupStageRBO(depthPrePassStage.stageName);
    RenderPipelineStageConfig tonemapStage;
    tonemapStage.stageName = std::string("Tonemap");
    tonemapStage.bufferSize = RectSizeInt(h,w);
    tonemapStage.vertexShaderFileName = std::string("PostProcessing/PostProccessQuard.vert");
    tonemapStage.fragmentShaderFileName= std::string("PostProcessing/Tonemap/Filmic.frag");
    tonemapStage.ppFeedStage1 = std::string("BLUM:KAWASE_PONG_PASS");
    tonemapStage.ppFeedStage2 = std::string("Main");
    tonemapStage.ppFeedStage3 = std::string("lumkey");
    tonemapStage.ppFeedStage4 = std::string("SSAO:VERTICAL_BLUR_PASS");
    tonemapStage.isPostProcess = true;
    tonemapStage.rboType = RBO::RBO_SCREEN;
    mainRenderTonemapPass =initStage(&tonemapStage);
    /*short path*/
    pp_prog_hdr_tonemap = lookupStageShader(tonemapStage.stageName);
    rtSCREEN = lookupStageRBO(tonemapStage.stageName);


    /*Main*/
    RenderPipelineStageConfig mainStage;
    mainStage.stageName = std::string("Main");
    mainStage.bufferSize = RectSizeInt(h,w);
    mainStage.vertexShaderFileName = std::string("Main/Main.vert");
    mainStage.fragmentShaderFileName= std::string("Main/Main.frag");
    mainStage.rboType = RBO::RBO_FLOAT;
    mainStage.isPostProcess = false;
    mainRenderPass = initStage(&mainStage);
    rtHDRScene = lookupStageRBO(mainStage.stageName);
    /*Main MSAA*/
    RenderPipelineStageConfig mainStageMSAA;
    mainStageMSAA.stageName = std::string("MainMSAA");
    mainStageMSAA.bufferSize = RectSizeInt(h,w);
    mainStageMSAA.vertexShaderFileName = std::string("Main/Main.vert"); // TODO: shortcut? switch !
    mainStageMSAA.fragmentShaderFileName= std::string("Main/Main.frag");
    mainStageMSAA.rboType = RBO::RBO_MSAA;
    mainStageMSAA.isPostProcess = false;
    mainRenderPassMSAA = initStage(&mainStageMSAA);
    rtHDRScene_MSAA = lookupStageRBO(mainStageMSAA.stageName);

    RenderPipelineStageConfig bloomStage ;
    bloomStage.stageName = std::string("Bloom");
    bloomStage.bufferSize = RectSizeInt(h/2,w/2);
    bloomStage.vertexShaderFileName = std::string("PostProcessing/PostProccessQuard.vert");
    bloomStage.fragmentShaderFileName= std::string("PostProcessing/Bloom/Clamp.frag" );
    bloomStage.rboType = RBO::RBO_FLOAT_RED;
    bloomStage.isPostProcess = false;
    initStage(&bloomStage);
    rtHDRBloomResult = lookupStageRBO(bloomStage.stageName);
    SShader * bloomShader = lookupStageShader(bloomStage.stageName);

    RenderPipelineStageConfig horBlurStage ;
    horBlurStage.stageName = std::string("BLUM:KAWASE_PING_PASS");
    horBlurStage.bufferSize = RectSizeInt(h/4,w/4);
    horBlurStage.vertexShaderFileName = std::string("PostProcessing/PostProccessQuard.vert");
    horBlurStage.fragmentShaderFileName= std::string("PostProcessing/Bloor/Kawase.frag" );//kawase here
    horBlurStage.rboType = RBO::RBO_FLOAT_RED;
    horBlurStage.isPostProcess = false;
    initStage(&horBlurStage);
    rtHDRHorBlurResult = lookupStageRBO(horBlurStage.stageName);
    SShader * horBlurShader = lookupStageShader(horBlurStage.stageName);
    pp_prog_hdr_blur_kawase = horBlurShader;

    RenderPipelineStageConfig vertBlurStage ;
    vertBlurStage.stageName = std::string("BLUM:KAWASE_PONG_PASS");
    vertBlurStage.bufferSize = RectSizeInt(h/4,w/4);
    vertBlurStage.vertexShaderFileName = std::string("PostProcessing/PostProccessQuard.vert");
    vertBlurStage.fragmentShaderFileName= std::string("PostProcessing/Bloor/Kawase.frag"); //kawase here
    vertBlurStage.rboType = RBO::RBO_FLOAT_RED;
    vertBlurStage.isPostProcess = false;
    initStage(&vertBlurStage);
    rtHDRVertBlurResult = lookupStageRBO(vertBlurStage.stageName);
    //SShader * vertBlurShader = lookupStageShader(vertBlurStage.stageName);


    RenderPipelineStageConfig vertSSAOBlurStage ;
    vertSSAOBlurStage.stageName = std::string("SSAO:VERTICAL_BLUR_PASS");
    vertSSAOBlurStage.bufferSize = RectSizeInt(h/2,w/2);
    vertSSAOBlurStage.vertexShaderFileName = std::string("PostProcessing/PostProccessQuard.vert");
    vertSSAOBlurStage.fragmentShaderFileName= std::string("PostProcessing/Bloor/GaussVerticalFixedTap16.frag");
    vertSSAOBlurStage.rboType = RBO::RBO_RED;
    vertSSAOBlurStage.isPostProcess = false;
    initStage(&vertSSAOBlurStage);
    rtSSAOVertBlurResult = lookupStageRBO(vertSSAOBlurStage.stageName);
    SShader * ssaoVertBlurShader = lookupStageShader(vertSSAOBlurStage.stageName);

    RenderPipelineStageConfig horSSAOBlurStage ;
    horSSAOBlurStage.stageName = std::string("SSAO:HORIZONTAL_BLUR_PASS");
    horSSAOBlurStage.bufferSize = RectSizeInt(h/2,w/2);
    horSSAOBlurStage.vertexShaderFileName = std::string("PostProcessing/PostProccessQuard.vert");
    horSSAOBlurStage.fragmentShaderFileName= std::string("PostProcessing/Bloor/GaussHorizontalFixedTap16.frag");
    horSSAOBlurStage.rboType = RBO::RBO_RED;
    horSSAOBlurStage.isPostProcess = false;
    initStage(&horSSAOBlurStage);
    rtSSAOHorBlurResult = lookupStageRBO(horSSAOBlurStage.stageName);
    SShader * ssaoHorBlurShader = lookupStageShader(horSSAOBlurStage.stageName);
    //pp_prog_hdr_blur_kawase = lookupStageShader(horSSAOBlurStage.stageName);


    RenderPipelineStageConfig cubemapStage ;
    cubemapStage.stageName = std::string("cubemap");
    cubemapStage.bufferSize = RectSizeInt(512,512);
    cubemapStage.vertexShaderFileName = std::string("Cubemap/cubemap_gen.vert");
    cubemapStage.fragmentShaderFileName= std::string("Cubemap/cubemap_gen.frag");
    cubemapStage.geoShaderFileName = std::string("Cubemap/cubemap_gen.geom");
    cubemapStage.rboType = RBO::RBO_CUBEMAP;
    cubemapStage.isPostProcess = false;
    initStage(&cubemapStage);
    rtCubemap = lookupStageRBO(cubemapStage.stageName);
    cubemap_prog_generator = lookupStageShader(cubemapStage.stageName);


    RenderPipelineStageConfig loglumStage ;
    loglumStage.stageName = std::string("loglum");
    loglumStage.bufferSize = RectSizeInt(16,16);
    loglumStage.vertexShaderFileName = std::string("PostProcessing/PostProccessQuard.vert");
    loglumStage.fragmentShaderFileName= std::string("PostProcessing/Tonemap/LumLog.frag");
    loglumStage.rboType = RBO::RBO_FLOAT;
    loglumStage.isPostProcess = false;
    initStage(&loglumStage);
    rtHDRLogLum = lookupStageRBO(loglumStage.stageName);
    SShader * loglumShader = lookupStageShader(loglumStage.stageName);

    RenderPipelineStageConfig lumkeyStage ;
    lumkeyStage.stageName = std::string("lumkey");
    lumkeyStage.bufferSize = RectSizeInt(1,1);
    lumkeyStage.vertexShaderFileName = std::string("PostProcessing/PostProccessQuard.vert");
    lumkeyStage.fragmentShaderFileName= std::string("PostProcessing/Tonemap/LumKey.frag");
    lumkeyStage.rboType = RBO::RBO_FLOAT;
    lumkeyStage.isPostProcess = false;
    initStage(&lumkeyStage);
    rtHDRLumKey = lookupStageRBO(lumkeyStage.stageName);
    SShader * lumkeyShader = lookupStageShader(lumkeyStage.stageName);

    renderPipelineLink();

    rtShadowMap->texDEPTH()->setInterpolationMode(SRBOTexture::InterpolationType::TEX_NEAREST);
    rtPrepass->texIMG(0)->setInterpolationMode(SRBOTexture::InterpolationType::TEX_NEAREST);
    rtPrepass->texIMG(0)->setInterpolationMode(SRBOTexture::InterpolationType::TEX_NEAREST);

    pp_stage_ssao = new SPostProcess(new SShader("PostProcessing/PostProccessQuard.vert", "PostProcessing/SSAO/SimpleSSAO.frag"),w/2,h/2,rtPrepass->texIMG(0),rtPrepass->texDEPTH(),texRandom);

    pp_stage_hdr_tonemap = mainRenderTonemapPass->postProcess; //shoutcut

    pp_stage_ssao_blur_hor = (new SPostProcess(ssaoHorBlurShader, rtSSAOHorBlurResult,rtSSAOVertBlurResult));

    pp_stage_ssao_blur_vert = (new SPostProcess(ssaoVertBlurShader,rtSSAOVertBlurResult,rtSSAOHorBlurResult));

    pp_stage_hdr_bloom = (new SPostProcess(bloomShader, rtHDRBloomResult,rtHDRScene,rtHDRLumKey));

    pp_stage_hdr_blur_hor = (new SPostProcess(pp_prog_hdr_blur_kawase, rtHDRHorBlurResult ,rtHDRBloomResult));

    pp_stage_hdr_blur_vert = (new SPostProcess(pp_prog_hdr_blur_kawase, rtHDRVertBlurResult ,rtHDRHorBlurResult));
    /*ping pong*/
    pp_stage_hdr_blur_hor2 = (new SPostProcess(pp_prog_hdr_blur_kawase,rtHDRHorBlurResult ,rtHDRVertBlurResult));

    pp_stage_hdr_blur_vert2 = (new SPostProcess(pp_prog_hdr_blur_kawase,rtHDRVertBlurResult,rtHDRHorBlurResult));

    pp_stage_hdr_lum_log = (new SPostProcess(loglumShader ,rtHDRLogLum,rtHDRScene));

    pp_stage_hdr_lum_key = (new SPostProcess(lumkeyShader ,rtHDRLumKey,rtHDRLogLum,rtHDRLumKey));

    shaderViewAsIs = new SShader("PostProcessing/PostProccessQuard.vert","PostProcessing/View/AsIs.frag");

    postProcessDebugOutput = (new SPostProcess(shaderViewAsIs,w,h));

}

SScene::~SScene()
{

}

int SScene::Reshape(RectSizeInt v) {

    mainRenderTonemapPass->stageRBO->Resize(v);
        rtSCREEN->Resize(v);
        /* there should be internal buffer recreation code but ?? */
        glViewport( 0, 0, (GLsizei)v.w, (GLsizei)v.h );
        /*fix aspect ratio*/
        cam.setProjMatrix( SPerspectiveProjectionMatrix(100.0f, 10000.0f,(float)v.h / (float)v.w,toRad(26.0)) );

        return ESUCCESS;
}

int SScene::AddObjectToRender(std::shared_ptr<SObjModel> obj)
{
    obj->ConfigureProgram(*lookupStageShader("Main"));
    obj->ConfigureProgram(*lookupStageShader("MainMSAA"));
    obj->ConfigureProgram(*lookupStageShader("Shadowmap"));
    obj->ConfigureProgram(*cubemap_prog_generator);
    d_render_list.push_back(obj);
    return 0;

}

int SScene::toggleBrightPass(bool b)
{
    d_toggle_brightpass = b;
    pp_stage_hdr_tonemap->getShader()->SetUniform("b_brightPass",(int) d_toggle_brightpass);
    return ESUCCESS;
}

int SScene::toggleMSAA(bool b)
{
    d_toggle_MSAA = b;
    return ESUCCESS;
}

int SScene::toggleSSAO(bool b)
{
    d_toggle_ssao = b;
    pp_stage_hdr_tonemap->getShader()->SetUniform("b_SSAO",(int) d_toggle_ssao);
    return ESUCCESS;
}

int SScene::regenerateEnvCubeMap()
{
    d_RegenerateCubemap = true;
    return 0;
}



std::vector <Point> FrustumPoints(const SMat4x4& PV) {
    std::vector <Point> s;
    SMat4x4 invPV = PV.Inverse();

    vec4 f1 = invPV*vec4(1.0,-1.0,-1.0,1.0);
    f1.DivW();
    s.push_back(Point(f1));
    vec4 f2 = invPV*vec4(-1.0,1.0,-1.0,1.0);
    f2.DivW();
    s.push_back(Point(f2));
    vec4 f3 = invPV*vec4(1.0,1.0,-1.0,1.0);
    f3.DivW();
    s.push_back(Point(f3));
    vec4 f4 = invPV*vec4(-1.0,-1.0,-1.0,1.0);
    f4.DivW();
    s.push_back(Point(f4));
    vec4 b1 = invPV*vec4(1.0,-1.0,1.0,1.0);
    b1.DivW();
    s.push_back(Point(b1));
    vec4 b2 = invPV*vec4(-1.0,1.0,1.0,1.0);
    b2.DivW();
    s.push_back(Point(b2));
    vec4 b3 = invPV*vec4(1.0,1.0,1.0,1.0);
    b3.DivW();
    s.push_back(Point(b3));
    vec4 b4 = invPV*vec4(-1.0,-1.0,1.0,1.0);
    b4.DivW();
    s.push_back(Point(b4));
    return s;

}

vec2 ProjectPoint(const Point &in, const SMat4x4 &sm_mvp) {
    vec4 v = sm_mvp*vec4(in.x,in.y,in.z,1.0);
    v = v / v.w;
    return vec2(v.x,v.y);
}


BBox PSRProjectionBoundingBox(const AABB &a, const SMat4x4 &sm_mvp) {
    vec2 p[8];

    p[0] = ProjectPoint(Point(a.max_point.x,a.max_point.y,a.max_point.z),sm_mvp);
    p[1] = ProjectPoint(Point(a.max_point.x,a.max_point.y,a.min_point.z),sm_mvp);
    p[2] = ProjectPoint(Point(a.max_point.x,a.min_point.y,a.max_point.z),sm_mvp);
    p[3] = ProjectPoint(Point(a.max_point.x,a.min_point.y,a.min_point.z),sm_mvp);
    p[4] = ProjectPoint(Point(a.min_point.x,a.max_point.y,a.max_point.z),sm_mvp);
    p[5] = ProjectPoint(Point(a.min_point.x,a.max_point.y,a.min_point.z),sm_mvp);
    p[6] = ProjectPoint(Point(a.min_point.x,a.min_point.y,a.max_point.z),sm_mvp);
    p[7] = ProjectPoint(Point(a.min_point.x,a.min_point.y,a.min_point.z),sm_mvp);
    BBox res;
    res.max_point.x = std::numeric_limits<float>::min();
    res.max_point.y = std::numeric_limits<float>::min();
    res.min_point.x = std::numeric_limits<float>::max();
    res.min_point.y = std::numeric_limits<float>::max();
    for (int i = 0 ; i<8 ; i++) {
        if (res.max_point.x < p[i].x )
              res.max_point.x =p[i].x;
        if (res.max_point.y < p[i].y )
              res.max_point.y =p[i].y;
        if (res.min_point.x > p[i].x )
              res.min_point.x =p[i].x;
        if (res.min_point.y > p[i].y )
              res.min_point.y =p[i].y;
     }
    return res;
}
BBox PSRProjectionPointSet(const std::vector <Point> & p_list , const SMat4x4 &sm_mvp) {

    BBox res;
    res.max_point.x = std::numeric_limits<float>::min();
    res.max_point.y = std::numeric_limits<float>::min();
    res.min_point.x = std::numeric_limits<float>::max();
    res.min_point.y = std::numeric_limits<float>::max();
    for (auto& ps : p_list ) {
        vec2 p = ProjectPoint(ps,sm_mvp);
        if (res.max_point.x < p.x )
              res.max_point.x =p.x;
        if (res.max_point.y < p.y )
              res.max_point.y =p.y;
        if (res.min_point.x > p.x )
              res.min_point.x =p.x;
        if (res.min_point.y > p.y )
              res.min_point.y =p.y;
    }
    return res;
}
/*
 * Generate special scale matrix operator for
 * scaling Shadow Map Space for Potential Shadow Recivers
 *
*/
SMat4x4 PSRFocusSMSTransformMatrix(const BBox &psr ) {
    float s_x = 2.0 / ( psr.max_point.x - psr.min_point.x);
    float o_x = -(s_x *(psr.max_point.x + psr.min_point.x))/2.0;
    float s_y = 2.0 / ( psr.max_point.y - psr.min_point.y);
    float o_y = -(s_y *(psr.max_point.y + psr.min_point.y))/2.0;
    return  SMat4x4(s_x,0.0,0.0,o_x,
                    0.0,s_y,0.0,o_y,
                    0.0,0.0,1.0,0.0,
                    0.0,0.0,0.0,1.0);
}

SMat4x4 LookAtMatrix(const vec4 &forward, const vec4 &up ) {

    vec4 zaxis = vec4::Normalize(forward); /*direction to view point*/
    vec4 xaxis = vec4::Normalize(vec4::Cross3(vec4::Normalize(up),zaxis)); /* left vector*/
    vec4 yaxis(vec4::Normalize(vec4::Cross3(zaxis,xaxis))); /* new up vector */
    vec4 v1 (xaxis.x , yaxis.x , zaxis.x,0.0);
    vec4 v2 (xaxis.y , yaxis.y , zaxis.y,0.0);
    vec4 v3 (xaxis.z , yaxis.z , zaxis.z,0.0);
    vec4 v4 (0.0,0.0,0.0,1.0);
    SMat4x4 l = SMat4x4(v1,v2,v3,v4);
    return l;

}int SScene::UpdateShadowmap()
{
    SMat4x4 Bias = SMat4x4( 0.5,0.0,0.0,0.0,
                            0.0,0.5,0.0,0.0,
                            0.0,0.0,0.5,0.0,
                            0.5,0.5,0.5,1.0).Transpose(); //small todo

    /*fit AABB*/
    //float shadowCascadeDiv[] = { 100.0,290.0, 840.0, 2420.0,7000.0};

    SMat4x4 PV = cam.getViewProjectMatrix();
    std::vector <Point> psrPoints;// = FrustumPoints(PV); - add view-frustum as PSR
    AABB psrAABB =AABB(Point(-2000.0,-100,-1300.0), Point(2000.0,1300.0,1300.0) ); //Scene PSR

    std::vector <Point> aabbPoints = AABBPoints(psrAABB);
    psrPoints.insert(psrPoints.end(),aabbPoints.begin(),aabbPoints.end()); /*JOIN - better solution, convex hull*/

    //vec4 cam_up = vec4::Normalize(cam.getViewMatrix().ExtractUpVector());
    vec4 cam_up = vec4(0.0,1.0,0.0,1.0);
    vec4 sunDirection = w_sky->GetSunDirection();
    SMat4x4 shadowMapViewMatrix = LookAtMatrix(sunDirection,cam_up); /*new basis*/
    int min_idx;
    UNUSED(min_idx);
    int max_idx = 0;
    int i = 0;
    float max_z = std::numeric_limits<float>::min();
    float min_z = std::numeric_limits<float>::max();
    float shadowMapDistance;
    for (auto& p : psrPoints ) {
        vec4 ov = shadowMapViewMatrix*vec4(p.x,p.y,p.z,1.0f);//OPTIMIZE dot with row should be enought;

        if (max_z < ov.z)
        {
            max_z = ov.z;
            max_idx = i;
        }

        if (min_z > ov.z)
        {
            min_z = ov.z;
            min_idx = i;
        }
        i++;
    }
    shadowMapDistance = fabs(max_z-min_z);
    Point p = psrPoints[max_idx];

    vec4 pos = vec4(p.x,p.y,p.z,1.0);


    SMat4x4 shadowMapFinalProjectionMatrix[4];
    for (int i = 0; i < 4; i++ ){

        SMat4x4 shadowMapProjectionMatrix = SOrtoProjectionMatrix(0.1, shadowMapDistance,1.0f,1.0,-1.0,-1.0);

        SMat4x4 shadowMapPVMatrix = shadowMapProjectionMatrix*shadowMapViewMatrix*SMat4x4().Move(-pos);
        SMat4x4 shadowPostPerspectiveScale = PSRFocusSMSTransformMatrix(PSRProjectionPointSet(psrPoints,shadowMapPVMatrix));

        shadowMapFinalProjectionMatrix[i] = shadowPostPerspectiveScale*shadowMapProjectionMatrix;

        d_shadowmap_cam[i].setViewMatrix(shadowMapViewMatrix*SMat4x4().Move(-pos));
        d_shadowmap_cam[i].setProjMatrix(shadowMapFinalProjectionMatrix[i]);
    }


    d_debugDrawMgr.ClearAll();
    d_debugDrawMgr.AddCross(pos,200);
    d_debugDrawMgr.AddAABB(psrAABB);
    d_debugDrawMgr.AddCameraFrustrum(d_shadowmap_cam[0].getViewProjectMatrix());
    d_debugDrawMgr.Update();


    SShader * mp;
    if (this->d_toggle_MSAA)
        mp = this->mainRenderPassMSAA->stageShader;
    else
        mp = this->mainRenderPass->stageShader;
    mp->SetUniform("shadowMVPB0",Bias*d_shadowmap_cam[0].getViewProjectMatrix());
    mp->SetUniform("shadowMVPB1",Bias*d_shadowmap_cam[1].getViewProjectMatrix());
    mp->SetUniform("shadowMVPB2",Bias*d_shadowmap_cam[2].getViewProjectMatrix());
    mp->SetUniform("shadowMVPB3",Bias*d_shadowmap_cam[3].getViewProjectMatrix());
    /*Set primary shader direction*/
     mp->SetUniform("sunLightDirectionVector",w_sky->GetSunDirection());
    /*update SSAO projection matrix*/
    pp_stage_ssao->getShader()->SetUniform("m_P",cam.getProjMatrix() );
}
int SScene::UpdateScene(float dt) {
    /*shadowmap*/
    UNUSED(dt);
    UpdateShadowmap();
    if (d_play) {
        cam.setViewMatrix(rec.Get());
        if (rec.Empty())
            //d_play = false;
            rec.Rewind();
    }
    else
        rec.Add(cam.getViewMatrix());
    return 0;

}

int SScene::debugSetFinalRenderOutput(RBO * r)
{
    debugFinalRenderOutput = r;
    return 0;
}

int SScene::debugSetDebugRenderOutputFlag(bool flag)
{
    debugRenderOutputFlag = flag;
    return ESUCCESS;
}

float SScene::debugGetRenderTime()
{
    return d_dbgRenderTimeMs;
}

float SScene::debugGetPostProcessingTime()
{
    return d_dbgPostProcessingTimeMs;
}
long int SScene::debugGetFrameNumber()
{
    return d_dbgFrameNumber;
}
Recorder SScene::getRec() const
{
    return rec;
}

void SScene::setRec(const Recorder &value)
{
    rec = value;
}

int inline SScene::RenderShadowMap(const RBO& v) {
    v.Bind(true);

    SShader * cam_prog = shadowMapPass->stageShader;
    cam_prog->SetUniform("MVP0",d_shadowmap_cam[0].getProjMatrix()*d_shadowmap_cam[0].getViewMatrix());
    cam_prog->SetUniform("MVP1",d_shadowmap_cam[1].getProjMatrix()*d_shadowmap_cam[1].getViewMatrix());
    cam_prog->SetUniform("MVP2",d_shadowmap_cam[2].getProjMatrix()*d_shadowmap_cam[2].getViewMatrix());
    cam_prog->SetUniform("MVP3",d_shadowmap_cam[3].getProjMatrix()*d_shadowmap_cam[3].getViewMatrix());
    RenderContext r_ctx(cam_prog);
    for (auto r : d_render_list ) {
        r->Render(r_ctx);
    }
    return ESUCCESS;
}

int SScene::RenderCubemap()
{

    if (d_toggle_MSAA)
       msaa_pass.Bind();
    else
       normal_pass.Bind();

    rtCubemap->Bind();
    SMat4x4 pos = SMat4x4().Move(0,-500.0,0.0);
    SMat4x4 cube_projection = SPerspectiveProjectionMatrix(10,10000,1,toRad(90.0));
    RenderContext r_ctx(cubemap_prog_generator,pos,cube_projection);

    /*Sky support*/
    RenderContext r_ctx_sky(w_sky->GetSkyCubemapShader() ,pos,cube_projection);

    w_sky->Draw(r_ctx_sky);

    //for (auto& r : d_render_list ) {
    //    r->Render(r_ctx);
    //}

    SCProg cs("Cubemap/cubemap_convolve.comp");
    cs.Barrier();
    cs.Use();
    rtCubemap->texIMG(0)->Bind(1);
    cs.SetUniform("srcCube",1);
    cs.Use();
    texConvoledCubemap->BindImage(2);
    cs.SetUniform("dstSHBands",2);

    cs.SetUniform("dstSamplesTotal",100*100);
    cs.SetUniform("dstSHBandY",1);

    cs.Dispatch(100,1 ,1);
    cs.Barrier();
    return ESUCCESS;

}

int SScene::RenderPrepass(const RenderPipelineStageRuntime &runtime)
{
    runtime.stageRBO->Bind(true);
    /*submit geometry for prepass*/
    RenderContext r_ctx_prepass(runtime.stageShader,cam.getViewMatrix(),cam.getProjMatrix());
    for (auto& r : d_render_list ) {
        r->Render(r_ctx_prepass);
    }
    return 0;
}

int SScene::BlurKawase(float factor)
{
    int blurSizeLoc = pp_prog_hdr_blur_kawase->getUniformLocation("blurSize");
    pp_prog_hdr_blur_kawase->SetUniform(blurSizeLoc,(float)(factor*1.0));
    pp_stage_hdr_blur_hor->DrawRBO(false);
    pp_prog_hdr_blur_kawase->SetUniform(blurSizeLoc,(float)(factor*2.0));
    pp_stage_hdr_blur_vert->DrawRBO(false);

    /*ping pong 1*/
    pp_prog_hdr_blur_kawase->SetUniform(blurSizeLoc,(float)(factor*2.0));
    pp_stage_hdr_blur_hor2->DrawRBO(false);
    pp_prog_hdr_blur_kawase->SetUniform(blurSizeLoc,(float)(factor*3.0));
    pp_stage_hdr_blur_vert2->DrawRBO(false);
    /*ping pong 2*/
    pp_prog_hdr_blur_kawase->SetUniform(blurSizeLoc,(float)(factor*3.0));
    pp_stage_hdr_blur_hor2->DrawRBO(false);
    pp_prog_hdr_blur_kawase->SetUniform(blurSizeLoc,(float)(factor*4.0));
    pp_stage_hdr_blur_vert2->DrawRBO(false);
    return 0;
}


int inline SScene::RenderDirect(const RenderPipelineStageRuntime &runtime) {
    runtime.stageRBO->Bind(true);
        RenderContext r_ctx(runtime.stageShader ,cam.getViewMatrix(), cam.getProjMatrix() ,
                            rtShadowMap->texDEPTH(),
                            rtShadowMap->texIMG(1),
                            rtCubemap->texIMG(0),
                            rtShadowMap->texIMG(0),
                            texConvoledCubemap,
                            texRandom);
        for (auto& r : d_render_list ) {
            r->Render(r_ctx);
        }
        RenderContext r_ctx2(w_sky->GetSkyShader() ,cam.getViewMatrix(),w_sky->GetSkyProjectionMatrix());
        w_sky->Draw(r_ctx2);
    d_debugDrawMgr.Render(infFarMatrix*cam.getViewMatrix());

    return 0;
}
int SScene::Render() {
    auto start = std::chrono::steady_clock::now();
    glClearColor(0.0,0.0,0.0,1.0);
    //glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glClampColor(GL_CLAMP_READ_COLOR, GL_FALSE);
    UpdateScene(0.01); /*add real ms*/
    if (d_toggle_MSAA)
       msaa_pass.Bind();
    else
       normal_pass.Bind();
    if (d_RegenerateCubemap) {
        d_RegenerateCubemap = false;
        RenderCubemap();
    }
    rtime.Begin();
        /*On request mode*/
    glCullFace(GL_FRONT);
    RenderShadowMap( *rtShadowMap);
    glCullFace(GL_BACK);
    RenderPrepass( *prePass);
    if (d_toggle_MSAA) {
        RenderDirect( *mainRenderPassMSAA);
        rtHDRScene_MSAA->ResolveMSAA(*rtHDRScene);
        } else {
            RenderDirect( *mainRenderPass);
        }
    rtime.End();
    pp_time.Begin();
    ui_pass.Bind();

        /*LumKEY*/
    pp_stage_hdr_lum_log->DrawRBO(false);
    pp_stage_hdr_lum_key->DrawRBO(false);
    if (d_toggle_brightpass ) {
        pp_stage_hdr_bloom->DrawRBO(false);
        BlurKawase(d_BloomFactor);
    } else {
        pp_stage_hdr_blur_vert2->Clean();
    }
        /*SSAO*/
    if (d_toggle_ssao)
    {
        rtSSAOVertBlurResult->Bind(false);
        pp_stage_ssao->Draw();
        pp_stage_ssao_blur_hor->DrawRBO(false);
        pp_stage_ssao_blur_vert->DrawRBO(false);

    }
    /*normaly do tonemap and, if debug enabled set required buffer*/
    if (debugRenderOutputFlag) {
        rtSCREEN->Bind(true);
        postProcessDebugOutput->setTexSrc1(debugFinalRenderOutput->texIMG(0));
        postProcessDebugOutput->Draw();
    }
    else
        pp_stage_hdr_tonemap->DrawRBO(false);

    pp_time.End();



    auto end = std::chrono::steady_clock::now();


    d_dbgRenderTimeMs =(float)rtime.getTime()*(1.0/ 1000000.0);
    d_dbgPostProcessingTimeMs =(float)pp_time.getTime()*(1.0/ 1000000.0);
    d_dbgFrameNumber++;
    return true;
}

int SScene::debugSetStageShaderUniform(const std::string& stage, const std::string& var, float val) {
    SShader *shader = lookupStageShader(stage);
    if (!shader) {
        LOGE("Invalid stage or stage without shader when stage lookup");
        return -1;
    }
    shader->SetUniform(var,val);
    return 0;
}