#include "Scene.h"
#include "r_cprog.h"
#include <chrono>
#include "c_config.h"
#include "RenderContext.h"
#include "math.h"
#include "mat_math.h"
#include "RenderState.h"
#include "Texture.h"


/////PROTO//////
/*serialization*/
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>

#include <type_traits>
#include <memory.h>
#include <limits>
#include "r_sprog.h"


SScene::SScene(RectSizeInt v)
    :rtSCREEN(new RBO(std::string("rtSCREEN"), v.w,v.h,RBO::RBO_SCREEN))
    ,cam(SMat4x4(),SPerspectiveProjectionMatrix(100.0f, 7000.0f,1.0f,toRad(26.0)))
    //,cam(SMat4x4(),SOrtoProjectionMatrix(100.0f, 7000.0f,1.0f,100.0,-100.0,-100.0))

    ,step(0.0f)
    ,normal_pass(RenderPass::LESS_OR_EQUAL,RenderPass::ENABLED,RenderPass::DISABLED )
    ,msaa_pass(RenderPass::LESS_OR_EQUAL,RenderPass::ENABLED,RenderPass::ENABLED)
    ,ui_pass(RenderPass::NEVER, RenderPass::DISABLED,RenderPass::DISABLED)
    
    ,rtShadowMap(new RBO(std::string("ShadowMap"),v.w*4,(v.h)*4, RBO::RBO_DEPTH_ARRAY_ONLY))
    ,rtPrepass(new RBO(std::string("DepthPrePass"),v.w/2,v.h/2, RBO::RBO_RGBA))
    ,rtHDRScene_MSAA(new RBO(std::string("HDR Mainpass MSAA"),v.w,v.h ,RBO::RBO_MSAA,SRBOTexture::RT_TEXTURE_MSAA,1,
                                                   SRBOTexture::RT_NONE,1,
                                                   SRBOTexture::RT_NONE, 1 ))
    ,rtHDRScene(new RBO(std::string("HDR Mainpass"),v.w,v.h ,RBO::RBO_FLOAT,SRBOTexture::RT_TEXTURE_FLOAT,1,
                                                   SRBOTexture::RT_NONE,1,
                                                   SRBOTexture::RT_NONE, 1 ))
    ,rtHDRBloomResult( new RBO(std::string("rtHDRBloomResult"),v.w/2,v.h/2, RBO::RBO_FLOAT_RED)) /* is it better HDR */
    ,rtHDRHorBlurResult(new RBO(std::string("rtHDRHorBlurResult"),v.w/4,v.h/4, RBO::RBO_FLOAT_RED))
    ,rtHDRVertBlurResult(new RBO(std::string("rtHDRVertBlurResult"),v.w/4,v.h/4, RBO::RBO_FLOAT_RED))
    ,rtSSAOVertBlurResult( new RBO(std::string("rtSSAOVertBlurResult"),v.w/2,v.h/2, RBO::RBO_RED))
    ,rtSSAOHorBlurResult(new RBO(std::string("rtSSAOHorBlurResult"),v.w/2,v.h/2, RBO::RBO_RED))
    ,rtVolumetric(new RBO(std::string("rtVolumetric"),v.w,v.h, RBO::RBO_RGBA))
    ,w_sky(new SWeatherSky())
    ,rtCubemap(new RBO(std::string("rtCubemap"),512, 512, RBO::RBO_CUBEMAP))
   ,rtConvoledCubemap(new SRBOTexture(RectSizeInt(10,4),SRBOTexture::RT_TEXTURE_FLOAT_RED))
    ,rtHDRLogLum(new RBO(std::string("rtHDRLogLum"),16,16,RBO::RBO_FLOAT)) /*Downsampled source for lumeneace*/
    ,rtHDRLumKey(new RBO(std::string("rtHDRLumKey"),1,1,RBO::RBO_FLOAT)) /*Lum key out*/
    ,dbg_ui(this,v)
    
{
    /*load configuration*/
    int w = rtHDRScene->getSize().w;
    int h = rtHDRScene->getSize().h;
    /*bloom shaders */
    pp_prog_hdr_tonemap.reset(new SShader("PostProcessing/PostProccessQuard.vert",\
                                      "PostProcessing/Tonemap/Filmic.frag"));

    pp_prog_hdr_blur_kawase.reset(new SShader("PostProcessing/PostProccessQuard.vert",\
                                          "PostProcessing/Bloor/Kawase.frag"));

    pp_stage_ssao.reset(new SPostProcess(new SShader("PostProcessing/PostProccessQuard.vert",\
                                                  "PostProcessing/SSAO/SimpleSSAO.frag") \
                                      ,w/2,h/2,rtPrepass->texIMG(0),rtPrepass->texDEPTH()));

    pp_stage_ssao_blur_hor.reset(new SPostProcess(new SShader("PostProcessing/PostProccessQuard.vert",\
                                                          "PostProcessing/Bloor/GaussHorizontal.frag"),\
                                              rtSSAOHorBlurResult,rtSSAOVertBlurResult));
    pp_stage_ssao_blur_vert.reset(new SPostProcess(new SShader("PostProcessing/PostProccessQuard.vert", \
                                                            "PostProcessing/Bloor/GaussVertical.frag"),\
                                                rtSSAOVertBlurResult,rtSSAOHorBlurResult));

    pp_stage_hdr_bloom.reset(new SPostProcess(new SShader("PostProcessing/PostProccessQuard.vert",\
                                                       "PostProcessing/Bloom/Clamp.frag" ),\
                                                rtHDRBloomResult,rtHDRScene,rtHDRLumKey));
    pp_stage_hdr_blur_hor.reset(new SPostProcess(pp_prog_hdr_blur_kawase.get(), rtHDRHorBlurResult ,rtHDRBloomResult));
    pp_stage_hdr_blur_vert.reset(new SPostProcess(pp_prog_hdr_blur_kawase.get(), rtHDRVertBlurResult ,rtHDRHorBlurResult));
    /*ping pong*/
    pp_stage_hdr_blur_hor2.reset(new SPostProcess(pp_prog_hdr_blur_kawase.get(),rtHDRHorBlurResult ,rtHDRVertBlurResult));
    pp_stage_hdr_blur_vert2.reset(new SPostProcess(pp_prog_hdr_blur_kawase.get(),rtHDRVertBlurResult,rtHDRHorBlurResult));

    pp_stage_hdr_lum_log.reset(new SPostProcess(new SShader ("PostProcessing/PostProccessQuard.vert",\
                                                         "PostProcessing/Tonemap/LumLog.frag")
                                            ,rtHDRLogLum,rtHDRScene));

    pp_stage_hdr_lum_key.reset(new SPostProcess(new SShader ("PostProcessing/PostProccessQuard.vert",\
                                                        "PostProcessing/Tonemap/LumKey.frag")
                                          ,rtHDRLumKey,rtHDRLogLum,rtHDRLumKey));

    /*final tonemap*/
    pp_stage_hdr_tonemap.reset(new SPostProcess(pp_prog_hdr_tonemap.get(),rtSCREEN,rtHDRVertBlurResult,rtHDRScene,rtHDRLumKey,rtSSAOVertBlurResult));

    /* img depth | shadow depth | shadow world pos*/
    pp_stage_volumetric.reset(new SPostProcess(new SShader("PostProcessing/PostProccessQuard.vert", \
                                                        "PostProcessing/Volumetric/Test.frag") \
                                            ,w,h,rtHDRScene->texDEPTH(),rtShadowMap->texDEPTH(),rtShadowMap->texIMG(2)));

    /*main prog*/
    main_pass_shader.reset(new SShader("Main/Main.vert","Main/Main.frag"));

    prepass_prog.reset(new SShader("DepthNormalPrepass/DepthNormalPrepass.vert","DepthNormalPrepass/DepthNormalPrepass.frag"));

    cam_prog.reset(new SShader("Shadow/Cascade.vert","Shadow/Cascade.frag","Shadow/Cascade.geom"));


    cubemap_prog_generator.reset(new SShader("Cubemap/cubemap_gen.vert","Cubemap/cubemap_gen.frag","Cubemap/cubemap_gen.geom"));

    shaderViewAsIs.reset(new SShader("PostProcessing/PostProccessQuard.vert","PostProcessing/View/AsIs.frag"));

    postProcessDebugOutput.reset(new SPostProcess(shaderViewAsIs.get(),w,h));


    rtShadowMap->texDEPTH()->setInterpolationMode(SRBOTexture::InterpolationType::TEX_NEAREST);

    texRandom.reset(new STexture("noise.png"));
    texRandom->setInterpolationMode(STexture::InterpolationType::TEX_NEAREST);
    dbg_ui.Init();

    UpdateCfgLabel();

}

SScene::~SScene()
{

}

int SScene::Reshape(int w, int h) {

        rtSCREEN->Resize(SVec2(w,h));
        /* there should be internal buffer recreation code but ?? */
        glViewport ( 0, 0, (GLsizei)w, (GLsizei)h );
        /*fix aspect ratio*/
        //cam.setProjMatrix( SPerspectiveProjectionMatrix(100.0f, 10000.0f,(float)h / (float)w,toRad(26.0)) );

        return ESUCCESS;
}

int SScene::AddObjectToRender(std::shared_ptr<SObjModel> obj)
{
    obj->ConfigureProgram(*main_pass_shader);
    d_render_list.push_back(obj);
    return 0;

}

int SScene::toggleBrightPass(bool b)
{
    d_toggle_brightpass = b;
}

int SScene::toggleMSAA(bool b)
{
    d_toggle_MSAA = b;
}

/*todo: move to some place*/

AABB FrustrumSize(const SMat4x4& r) {

    SMat4x4 invPV = r.Inverse();
    SVec4 f1 = invPV*SVec4(1.0,-1.0,-1.0,1.0);
    f1.DivW();
    SVec4 f2 = invPV*SVec4(-1.0,1.0,-1.0,1.0);
    f2.DivW();
    SVec4 f3 = invPV*SVec4(1.0,1.0,-1.0,1.0);
    f3.DivW();
    SVec4 f4 = invPV*SVec4(-1.0,-1.0,-1.0,1.0);
    f4.DivW();

    SVec4 b1 = invPV*SVec4(1.0,-1.0,1.0,1.0);
    b1.DivW();
    SVec4 b2 = invPV*SVec4(-1.0,1.0,1.0,1.0);
    b2.DivW();
    SVec4 b3 = invPV*SVec4(1.0,1.0,1.0,1.0);
    b3.DivW();
    SVec4 b4 = invPV*SVec4(-1.0,-1.0,1.0,1.0);
    b4.DivW();

    float x_m;
    x_m = fmax(f1.x,f2.x);
    x_m = fmax(x_m,f3.x);
    x_m = fmax(x_m,f4.x);
    x_m = fmax(x_m,b1.x);
    x_m = fmax(x_m,b2.x);
    x_m = fmax(x_m,b3.x);
    x_m = fmax(x_m,b4.x);

    float x_mi;
    x_mi = fmin(f1.x,f2.x);
    x_mi = fmin(x_mi,f3.x);
    x_mi = fmin(x_mi,f4.x);
    x_mi = fmin(x_mi,b1.x);
    x_mi = fmin(x_mi,b2.x);
    x_mi = fmin(x_mi,b3.x);
    x_mi = fmin(x_mi,b4.x);

    float y_m;
    y_m = fmax(f1.y,f2.y);
    y_m = fmax(y_m,f3.y);
    y_m = fmax(y_m,f4.y);
    y_m = fmax(y_m,b1.y);
    y_m = fmax(y_m,b2.y);
    y_m = fmax(y_m,b3.y);
    y_m = fmax(y_m,b4.y);

    float y_mi;
    y_mi = fmin(f1.y,f2.y);
    y_mi = fmin(y_mi,f3.y);
    y_mi = fmin(y_mi,f4.y);
    y_mi = fmin(y_mi,b1.y);
    y_mi = fmin(y_mi,b2.y);
    y_mi = fmin(y_mi,b3.y);
    y_mi = fmin(y_mi,b4.y);

    float z_m;
    z_m = fmax(f1.z,f2.z);
    z_m = fmax(z_m,f3.z);
    z_m = fmax(z_m,f4.z);
    z_m = fmax(z_m,b1.z);
    z_m = fmax(z_m,b2.z);
    z_m = fmax(z_m,b3.z);
    z_m = fmax(z_m,b4.z);

    float z_mi;
    z_mi = fmin(f1.z,f2.z);
    z_mi = fmin(z_mi,f3.z);
    z_mi = fmin(z_mi,f4.z);
    z_mi = fmin(z_mi,b1.z);
    z_mi = fmin(z_mi,b2.z);
    z_mi = fmin(z_mi,b3.z);
    z_mi = fmin(z_mi,b4.z);

    AABB a;
    a.min_point = Point(x_mi,y_mi,z_mi);
    a.max_point = Point(x_m,y_m,z_m);

    return a;
}
Point2d ProjectPoint(const Point &in, const SMat4x4 &sm_mvp) {
    SVec4 v = sm_mvp*SVec4(in.x,in.y,in.z,1.0);
    v = v / v.w;
    return Point2d(v.x,v.y);
}
Point TransformPoint(const Point &in, const SMat4x4 &sm_mvp) {
    SVec4 v = sm_mvp*SVec4(in.x,in.y,in.z,1.0);
    v = v / v.w;
    v.w = 1.0;
    return Point(v.x,v.y,v.z);
}
AABB PSRProjectionAABB(const AABB &a, const SMat4x4 &sm_mvp) {
    Point p[8];

    p[0] = TransformPoint(Point(a.max_point.x,a.max_point.y,a.max_point.z),sm_mvp);
    p[1] = TransformPoint(Point(a.max_point.x,a.max_point.y,a.min_point.z),sm_mvp);
    p[2] = TransformPoint(Point(a.max_point.x,a.min_point.y,a.max_point.z),sm_mvp);
    p[3] = TransformPoint(Point(a.max_point.x,a.min_point.y,a.min_point.z),sm_mvp);
    p[4] = TransformPoint(Point(a.min_point.x,a.max_point.y,a.max_point.z),sm_mvp);
    p[5] = TransformPoint(Point(a.min_point.x,a.max_point.y,a.min_point.z),sm_mvp);
    p[6] = TransformPoint(Point(a.min_point.x,a.min_point.y,a.max_point.z),sm_mvp);
    p[7] = TransformPoint(Point(a.min_point.x,a.min_point.y,a.min_point.z),sm_mvp);
    AABB res;
    res.max_point.x = std::numeric_limits<float>::min();
    res.max_point.y = std::numeric_limits<float>::min();
    res.max_point.z = std::numeric_limits<float>::min();
    res.min_point.x = std::numeric_limits<float>::max();
    res.min_point.y = std::numeric_limits<float>::max();
    res.min_point.z = std::numeric_limits<float>::max();
    for (int i = 0 ; i < 8 ; i++) {
        if (res.max_point.x < p[i].x )
            res.max_point.x = p[i].x;
        if (res.max_point.y < p[i].y )
            res.max_point.y = p[i].y;
        if (res.max_point.z < p[i].z )
            res.max_point.z = p[i].z;

        if (res.min_point.x > p[i].x )
            res.min_point.x = p[i].x;
        if (res.min_point.y > p[i].y )
            res.min_point.y = p[i].y;
        if (res.min_point.z > p[i].z )
            res.min_point.z = p[i].z;
     }


    return res;
}
BBox PSRProjectionBoundingBox(const AABB &a, const SMat4x4 &sm_mvp) {
    Point2d p[8];

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

SMat4x4 LookAtMatrix(const SVec4 &forward, const SVec4 &up ) {

    SVec4 zaxis = SVec4::Normalize(forward); /*direction to view point*/
    SVec4 xaxis = SVec4::Normalize(SVec4::Cross3(SVec4::Normalize(up),zaxis)); /* left vector*/
    SVec4 yaxis(SVec4::Normalize(SVec4::Cross3(zaxis,xaxis))); /* new up vector */
    SVec4 v1 (xaxis.x , yaxis.x , zaxis.x,0.0);
    SVec4 v2 (xaxis.y , yaxis.y , zaxis.y,0.0);
    SVec4 v3 (xaxis.z , yaxis.z , zaxis.z,0.0);
    SVec4 v4 (0.0,0.0,0.0,1.0);
    SMat4x4 l = SMat4x4(v1,v2,v3,v4);
    return l;

}
int SScene::UpdateScene(float dt) {
    main_pass_shader->Bind();
    /*shadowmap*/
    SMat4x4 Bias = SMat4x4( 0.5,0.0,0.0,0.0,
                            0.0,0.5,0.0,0.0,
                            0.0,0.0,0.5,0.0,
                            0.5,0.5,0.5,1.0).Transpose(); //small todo

    /*fit AABB*/
    float shadowCascadeDiv[] = { 100.0,290.0, 840.0, 2420.0,7000.0};
    for (int i = 0 ; i < 4 ; i++) {
        cameraFrustrumAABB[i] = FrustrumSize(SPerspectiveProjectionMatrix(100.0, 7000.0,1.0f,toRad(26.0))*(cam.getViewMatrix()));
    }

   // AABB cameraFrustrumAABBFull = FrustrumSize(SPerspectiveProjectionMatrix(100.0f, 7000.0f,1.0f,toRad(26.0))*cam.getViewMatrix());
    //camera focus
    //from view matrix
    SVec4 pos = cam.getViewMatrix().ExtractPositionNoScale();//+cam.getViewMatrix().ExtractLeftVector()*((7000.0f-100.0f)/2.0);
    //d_debugDrawMgr.AddCross(Point(pos),1000);
    //d_debugDrawMgr.Update();
    ////from aabb ???
    //SVec4 pos = SVec4(r.min_point.x+0.5f*(r.max_point.x - r.min_point.x),r.min_point.y+0.5f*(r.max_point.y - r.min_point.y),r.min_point.z+0.5f*(r.max_point.z - r.min_point.z),1.0);


    SVec4 cam_up = SVec4::Normalize(cam.getViewMatrix().ExtractUpVector());
    //works badly..
    // non uniform scale gives vector new direction, not correct fully, so, for make it somehow correct, use biggest scale, but it produce overkill...
    //float max_scale = (r.max_point.x - r.min_point.x);
    //max_scale = fmax (max_scale , (r.max_point.y - r.min_point.y));
    //max_scale = fmax (max_scale , (r.max_point.z - r.min_point.z));

    //SMat4x4 scale_nonuniform = SMat4x4().Scale  (2.0/fabs(r.min_point.x - r.max_point.x),2.0/fabs(r.min_point.y - r.max_point.y),2.0/fabs(r.min_point.z - r.max_point.z));
    //non uniform scale change direction!!!!
    //SMat4x4 scale_uniform = SMat4x4().Scale(2.0/max_scale);



    //SMat4x4 m =  SMat4x4().Move(-pos.x,-pos.y,-pos.z);
    SMat4x4 shadowMapViewMatrix = LookAtMatrix(w_sky->GetSunDirection(),cam_up);
    //SMat4x4 shadowMapViewMatrix = LookAtMatrix(w_sky->GetSunDirection(),SVec4(1.0,0.0,0.0,1.0));
    //SMat4x4 shadowMapProjectionMatrix = SOrtoProjectionMatrix(100.0f, 7000.0f,1000.0f,1000.0,-1000.0,-1000.0);

    //TODO: fixup
    SMat4x4 shadowMapFinalProjectionMatrix[4];
    for (int i = 0; i < 4; i++ ){
        AABB psrAABB = PSRProjectionAABB (cameraFrustrumAABB[i],shadowMapViewMatrix);
        //cameraTransformFrustrumAABB[i] = psrAABB; //debug
        SMat4x4 shadowMapProjectionMatrix = SOrtoProjectionMatrix((psrAABB.min_point.z), (psrAABB.max_point.z - psrAABB.min_point.z)+1000.0f,1.0f,1.0,-1.0,-1.0);
        //SMat4x4 shadowMapProjectionMatrix = SOrtoProjectionMatrix(100.0, 7000.0,1.0f,1.0,-1.0,-1.0);

        SMat4x4 shadowMapPVMatrix = shadowMapProjectionMatrix*shadowMapViewMatrix*SMat4x4().Move(-pos.x,-pos.y,-pos.z);

        SMat4x4 shadowPostPerspectiveScale = PSRFocusSMSTransformMatrix(PSRProjectionBoundingBox(cameraFrustrumAABB[i],shadowMapPVMatrix));

        shadowMapFinalProjectionMatrix[i] = shadowPostPerspectiveScale*shadowMapProjectionMatrix;
    }

    for (int i = 0; i < 4; i++ ){
        d_shadowmap_cam[i].setViewMatrix(shadowMapViewMatrix);
        d_shadowmap_cam[i].setProjMatrix(shadowMapFinalProjectionMatrix[i]);
    }




    main_pass_shader->SetUniform("shadowMVPB0",Bias*d_shadowmap_cam[0].getViewProjectMatrix());
    main_pass_shader->SetUniform("shadowMVPB1",Bias*d_shadowmap_cam[1].getViewProjectMatrix());
    main_pass_shader->SetUniform("shadowMVPB2",Bias*d_shadowmap_cam[2].getViewProjectMatrix());
    main_pass_shader->SetUniform("shadowMVPB3",Bias*d_shadowmap_cam[3].getViewProjectMatrix());
    /*Set primary shader direction*/
     main_pass_shader->SetUniform("main_light_dir",w_sky->GetSunDirection());
    /*update SSAO projection matrix*/
    pp_stage_ssao->getShader()->SetUniform("m_P",cam.getProjMatrix() );
    return 0;

}

int SScene::debugSetFinalRenderOutput(std::shared_ptr<RBO> r)
{
    debugFinalRenderOutput = r;
    return 0;
}

int SScene::debugSetDebugRenderOutputFlag(bool flag)
{
    debugRenderOutputFlag = flag;
}




int inline SScene::RenderShadowMap(const RBO& v) {
    v.Bind(true);
    //incapsulation fail :(
    //todo: incapsulate uniform values to camera!
    cam_prog->SetUniform("MVP0",d_shadowmap_cam[0].getProjMatrix()*d_shadowmap_cam[0].getViewMatrix());
    cam_prog->SetUniform("MVP1",d_shadowmap_cam[1].getProjMatrix()*d_shadowmap_cam[1].getViewMatrix());
    cam_prog->SetUniform("MVP2",d_shadowmap_cam[2].getProjMatrix()*d_shadowmap_cam[2].getViewMatrix());
    cam_prog->SetUniform("MVP3",d_shadowmap_cam[3].getProjMatrix()*d_shadowmap_cam[3].getViewMatrix());
    RenderContext r_ctx(cam_prog.get());
    for (auto& r : d_render_list ) {
        r->Render(r_ctx);
    }
    return ESUCCESS;
}

int SScene::RenderCubemap()
{
    //glClearColor(0.0,0.0,0.0,1.0);
    //glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    //glClampColor(GL_CLAMP_READ_COLOR, GL_FALSE);
    if (d_toggle_MSAA)
       msaa_pass.Bind();
    else
       normal_pass.Bind();

    rtCubemap->Bind();
    SMat4x4 pos = SMat4x4().Move(0,-500.0,0.0);
    SMat4x4 cube_projection = SPerspectiveProjectionMatrix(10,10000,1,toRad(90.0));
    RenderContext r_ctx(cubemap_prog_generator.get() ,pos,cube_projection);

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
    rtConvoledCubemap->BindImage(2);
    cs.SetUniform("dstSHBands",2);

    cs.SetUniform("dstSamplesTotal",100*100);
    cs.SetUniform("dstSHBandY",1);

    cs.Dispatch(100,1 ,1);
    cs.Barrier();

}

int SScene::RenderPrepass(const RBO &v)
{
    v.Bind(true);
    /*submit geometry for prepass*/
    RenderContext r_ctx_prepass(prepass_prog.get() ,cam.getViewMatrix(),cam.getProjMatrix());
    for (auto& r : d_render_list ) {
        r->Render(r_ctx_prepass);
    }
    return 0;
}

int SScene::BlurKawase()
{
    float factor = d_cfg[6];
    int blurSizeLoc = pp_prog_hdr_blur_kawase->getUniformLocation("blurSize");
    pp_prog_hdr_blur_kawase->SetUniform(blurSizeLoc,(float)0.0);
    pp_stage_hdr_blur_hor->DrawRBO(false);
    pp_prog_hdr_blur_kawase->SetUniform(blurSizeLoc,(float)(factor*1.0));
    pp_stage_hdr_blur_vert->DrawRBO(false);

    /*ping pong 1*/
    pp_prog_hdr_blur_kawase->SetUniform(blurSizeLoc,(float)(factor*2.0));
    pp_stage_hdr_blur_hor2->DrawRBO(false);
    pp_prog_hdr_blur_kawase->SetUniform(blurSizeLoc,(float)(factor*2.0));
    pp_stage_hdr_blur_vert2->DrawRBO(false);
    /*ping pong 2*/
    pp_prog_hdr_blur_kawase->SetUniform(blurSizeLoc,(float)(factor*3.0));
    pp_stage_hdr_blur_hor2->DrawRBO(false);
    pp_prog_hdr_blur_kawase->SetUniform(blurSizeLoc,(float)(factor*3.0));
    pp_stage_hdr_blur_vert2->DrawRBO(false);
    return 0;
}
int inline SScene::RenderDirect(const RBO& v) {
    v.Bind(true);    
        RenderContext r_ctx(main_pass_shader.get() ,cam.getViewMatrix(), cam.getProjMatrix() ,
                            rtShadowMap->texDEPTH(),
                            rtShadowMap->texIMG(1),
                            rtCubemap->texIMG(0),
                            rtShadowMap->texIMG(0),
                            rtConvoledCubemap,
                            texRandom);
        for (auto& r : d_render_list ) {
            r->Render(r_ctx);
        }
        RenderContext r_ctx2(w_sky->GetSkyShader() ,cam.getViewMatrix(),w_sky->GetSkyProjectionMatrix());
        w_sky->Draw(r_ctx2);

    d_debugDrawMgr.Render(SInfinityFarProjectionMatrix(100,1.0,toRad(26.0))*cam.getViewMatrix());

    return 0;
}
int SScene::Render() {
    auto start = std::chrono::steady_clock::now();
    step  += 0.002f;
    glClearColor(0.0,0.0,0.0,1.0);
    //glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glClampColor(GL_CLAMP_READ_COLOR, GL_FALSE);
    UpdateScene(0.01); /*add real ms*/
    if (d_toggle_MSAA)
       msaa_pass.Bind();
    else
       normal_pass.Bind();
    if (!d_first_render ) {
        d_first_render = true;
        RenderCubemap();
       //  RenderShadowMap( *rtShadowMap);
    }
    rtime.Begin();
    if (dbg_ui.d_v_sel_current == DebugUI::V_DIRECT) {        
        RenderDirect( *rtSCREEN);

    } else {
        /*On request mode*/
        glCullFace(GL_FRONT);
        RenderShadowMap( *rtShadowMap);
        glCullFace(GL_BACK);
        RenderPrepass( *rtPrepass);
        if (d_toggle_MSAA) {
            RenderDirect( *rtHDRScene_MSAA);
            rtHDRScene_MSAA->ResolveMSAA(*rtHDRScene);
        } else {
            RenderDirect( *rtHDRScene);
        }
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
        BlurKawase();
        } else {
            pp_stage_hdr_blur_vert2->getResultRBO()->Bind(true); /*cleanup*/
        }
        /*SSAO*/
        rtSSAOVertBlurResult->Bind(false);
        pp_stage_ssao->Draw();
        pp_stage_ssao_blur_hor->DrawRBO(false);
        pp_stage_ssao_blur_vert->DrawRBO(false);
        /*normaly do tonemap and, if debug enabled set required buffer*/
        if (debugRenderOutputFlag) {
            rtSCREEN->Bind(true);
            postProcessDebugOutput->setTexSrc1(debugFinalRenderOutput->texIMG(0));
            postProcessDebugOutput->Draw();
            }
        else
            pp_stage_hdr_tonemap->DrawRBO(false);

    } else if (dbg_ui.d_v_sel_current == DebugUI::V_SHADOW_MAP) {
        rtSCREEN->Bind(true); 
        RenderShadowMap( *rtSCREEN);
    } else if (dbg_ui.d_v_sel_current == DebugUI::V_VOLUMETRIC) {
        rtSCREEN->Bind(true); 

        pp_stage_volumetric->getShader()->SetUniform("matrixShadowMapProjection",d_shadowmap_cam[0].getProjMatrix());
        pp_stage_volumetric->getShader()->SetUniform("matrixShadowMapView",d_shadowmap_cam[0].getViewMatrix());
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

    char buf [120];
    auto end = std::chrono::steady_clock::now();

    auto diff = end- start;

    SVec4 pos = cam.getPosition();

    sprintf(buf,"DRAW:%4.3f ms\nUI: %4.3f ms\nPP: %4.3f ms\nCPU: %4.3f\nX:%4.3f\nY:%4.3f\nZ:%4.3f\n",      (float)rtime.getTime()*(1.0/ 1000000.0), \
                                                                    (float)ui_time.getTime()*(1.0/1000000.0),\
                                                                    (float)pp_time.getTime()*(1.0/1000000.0),\
                                                                      std::chrono::duration <float, std::milli> (diff).count(),pos.x,pos.y,pos.z );
    dbg_ui.fps_label->setText(buf);
   
    return true;
}
int SScene::UpdateCfgLabel() {

    pp_stage_ssao_blur_hor->getShader()->SetUniform("blurSize",d_cfg[0]);
    pp_stage_ssao_blur_vert->getShader()->SetUniform("blurSize",d_cfg[0]);
    /*dbg*/
    pp_prog_hdr_tonemap->SetUniform("aoStrength",d_cfg[0]);

    SShader * s = pp_stage_ssao->getShader();
    s->SetUniform("ssaoSize",d_cfg[1]);
    s->SetUniform("ssaoLevelClamp",d_cfg[2]);
    s->SetUniform("ssaoDepthClamp",d_cfg[3]);
    pp_stage_hdr_bloom->getShader()->SetUniform("hdrBloomClamp",d_cfg[4]);
    pp_stage_hdr_bloom->getShader()->SetUniform("hdrBloomMul",d_cfg[5]);


    pp_prog_hdr_tonemap->SetUniform("A",d_cfg[7]);
    pp_prog_hdr_tonemap->SetUniform("B",d_cfg[8]);
    pp_prog_hdr_tonemap->SetUniform("C",d_cfg[9]);
    pp_prog_hdr_tonemap->SetUniform("D",d_cfg[10]);
    pp_prog_hdr_tonemap->SetUniform("E",d_cfg[11]);
    pp_prog_hdr_tonemap->SetUniform("F",d_cfg[12]);
    pp_prog_hdr_tonemap->SetUniform("LW",float((d_cfg[13])));

    main_pass_shader->SetUniform("lightIntensity",d_cfg[14]);

    return ESUCCESS;
}
