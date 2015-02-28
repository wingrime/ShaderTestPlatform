#include "Scene.h"
#include "r_cprog.h"
#include <chrono>
#include "c_config.h"
#include "r_context.h"
#include "math.h"
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

    ,d_shadowmap_cam0(SMat4x4(),SOrtoProjectionMatrix(100.0f, 7000.0f,1000.0f,1000.0,-1000.0,-1000.0))
    ,d_shadowmap_cam1(SMat4x4(),SOrtoProjectionMatrix(100.0f, 7000.0f,1000.0f,1000.0,-1000.0,-1000.0))
    ,d_shadowmap_cam2(SMat4x4(),SOrtoProjectionMatrix(100.0f, 7000.0f,1000.0f,1000.0,-1000.0,-1000.0))
    ,d_shadowmap_cam3(SMat4x4(),SOrtoProjectionMatrix(100.0f, 7000.0f,1000.0f,1000.0,-1000.0,-1000.0))
    ,cam(SMat4x4(),SPerspectiveProjectionMatrix(100.0f, 7000.0f,1.0f,toRad(26.0)))
    //,cam(SMat4x4(),SOrtoProjectionMatrix(100.0f, 7000.0f,1.0f,100.0,-100.0,-100.0))

    ,sky_cam(SMat4x4(),SInfinityFarProjectionMatrix(100.0f,1.0f,toRad(26.0)))
    ,step(0.0f)
    ,normal_pass(RenderPass::LESS_OR_EQUAL,RenderPass::ENABLED,RenderPass::DISABLED )
    ,msaa_pass(RenderPass::LESS_OR_EQUAL,RenderPass::ENABLED,RenderPass::ENABLED)
    ,ui_pass(RenderPass::NEVER, RenderPass::DISABLED,RenderPass::DISABLED)
    
    ,rtShadowMap(new RBO(v->getSize().w,v->getSize().h, RBO::RBO_DEPTH_ARRAY_ONLY))
    ,rtPrepass(new RBO(v->getSize().w/2,v->getSize().h/2, RBO::RBO_RGBA))
    ,rtHDRScene_MSAA(new RBO(v->getSize().w,v->getSize().h ,RBO::RBO_MSAA,SRBOTexture::RT_TEXTURE_MSAA,1,
                                                   SRBOTexture::RT_NONE,1,
                                                   SRBOTexture::RT_NONE, 1 ))
    ,rtHDRScene(new RBO(v->getSize().w,v->getSize().h ,RBO::RBO_FLOAT,SRBOTexture::RT_TEXTURE_FLOAT,1,
                                                   SRBOTexture::RT_NONE,1,
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
    pp_prog_hdr_tonemap.reset(new SShader("PostProcessing/PostProccessQuard.vert",\
                                      "PostProcessing/Tonemap/Filmic.frag"));

    pp_prog_hdr_blur_kawase.reset(new SShader("PostProcessing/PostProccessQuard.vert",\
                                          "PostProcessing/Bloor/Kawase.frag"));

    pp_stage_ssao.reset(new SPostProcess(new SShader("PostProcessing/PostProccessQuard.vert",\
                                                  "PostProcessing/SSAO/SimpleSSAO.frag") \
                                      ,w/2,h/2,rtPrepass->texIMG(),rtPrepass->texDEPTH()));

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
                                            ,w,h,rtHDRScene->texDEPTH(),rtShadowMap->texDEPTH(),rtShadowMap->texIMG2()));

    /*main prog*/
    r_prog.reset(new SShader("Main/Main.vert","Main/Main.frag"));

    prepass_prog.reset(new SShader("DepthNormalPrepass/DepthNormalPrepass.vert","DepthNormalPrepass/DepthNormalPrepass.frag"));

    cam_prog.reset(new SShader("Shadow/Cascade.vert","Shadow/Cascade.frag","Shadow/Cascade.geom"));

    /*sky dome prog*/
    sky_dome_prog.reset(new SShader("Sky/PerezSky.vert","Sky/PerezSky.frag"));

    cubemap_prog_generator.reset(new SShader("Cubemap/cubemap_gen.vert","Cubemap/cubemap_gen.frag","Cubemap/cubemap_gen.geom"));


    sky_dome_model->ConfigureProgram( *sky_dome_prog);
    sky_dome_model->SetModelMat(SMat4x4().Scale(1000.0,1000.0,1000.0));

    dbg_ui.Init();
    d_shadowmap_cam0.LookAt(SVec4( 0.0,0.0, 0.0,1.0),SVec4(0.0,290,0.0,1.0) ,   SVec4(1.0,0.0,0.0,1.0) );
    d_shadowmap_cam1.LookAt(SVec4( 0.0,0.0, 0.0,1.0),SVec4(0.0,836,0.0,1.0) ,   SVec4(1.0,0.0,0.0,1.0) );
    d_shadowmap_cam2.LookAt(SVec4( 0.0,0.0, 0.0,1.0),SVec4(0.0,2420,0.0,1.0) ,   SVec4(1.0,0.0,0.0,1.0) );
    d_shadowmap_cam3.LookAt(SVec4( 0.0,0.0, 0.0,1.0),SVec4(0.0,6000,0.0,1.0) ,   SVec4(1.0,0.0,0.0,1.0) );
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
    obj->ConfigureProgram(*r_prog);
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
    a.p1 = Point(x_mi,y_mi,z_mi);
    a.p2 = Point(x_m,y_m,z_m);

    return a;
}

int SScene::UpdateScene(float dt) {

   sky_cam.SyncFromCamera(cam);
    r_prog->Bind();
    /*shadowmap*/
    SMat4x4 Bias = SMat4x4( 0.5,0.0,0.0,0.0,
                            0.0,0.5,0.0,0.0,
                            0.0,0.0,0.5,0.0,
                            0.5,0.5,0.5,1.0).Transpose(); //small todo

    SVec4 light_dir =  SVec4(0.0,1.0,0.0,1.0);


    /*fit AABB*/
    AABB r = FrustrumSize(cam.getViewProjectMatrix());

    //update shadow cascade
    //d_shadowmap_cam0.setProjMatrix(SOrtoProjectionMatrix(100,7000,fabs(r.p1.x-r.p2.x)/2.0,fabs(r.p1.y-r.p2.y)/2.0,-fabs(r.p1.y-r.p2.y)/2.0,-fabs(r.p1.z-r.p2.z)/2.0));
    //d_shadowmap_cam1.setProjMatrix(SOrtoProjectionMatrix(100,7000,fabs(r.p1.x-r.p2.x)/2.0,fabs(r.p1.y-r.p2.y)/2.0,-fabs(r.p1.y-r.p2.y)/2.0,-fabs(r.p1.z-r.p2.z)/2.0));
    //d_shadowmap_cam2.setProjMatrix(SOrtoProjectionMatrix(100,7000,fabs(r.p1.x-r.p2.x)/2.0,fabs(r.p1.y-r.p2.y)/2.0,-fabs(r.p1.y-r.p2.y)/2.0,-fabs(r.p1.z-r.p2.z)/2.0));
    //d_shadowmap_cam3.setProjMatrix(SOrtoProjectionMatrix(100,7000,fabs(r.p1.x-r.p2.x)/2.0,fabs(r.p1.y-r.p2.y)/2.0,-fabs(r.p1.y-r.p2.y)/2.0,-fabs(r.p1.z-r.p2.z)/2.0));

    SMat4x4 p =SOrtoProjectionMatrix(-1,1,1,1,-1,-1);
    d_shadowmap_cam0.setProjMatrix(p);
    d_shadowmap_cam1.setProjMatrix(p);
    d_shadowmap_cam2.setProjMatrix(p);
    d_shadowmap_cam3.setProjMatrix(p);

    SVec4 pos = cam.getViewMatrix().ExtractPositionNoScale();
    //SVec4 pos = SVec4((r.p1.x - r.p2.x)/2.0,(r.p1.y - r.p2.y)/2.0,(r.p1.z - r.p2.z)/2.0,1.0 );
    SMat4x4 c;

    c = SMat4x4().Scale(2.0/fabs(r.p1.x - r.p2.x),2.0/fabs(r.p1.y - r.p2.y),2.0/fabs(r.p1.z - r.p2.z));

    //SMat4x4 l =  SMat4x4().Move(-pos.x,-pos.y,-pos.z);
    //pos = pos - SVec4(fabs(r.p1.x - r.p2.x)/2.0,fabs(r.p1.y - r.p2.y)/2.0,fabs(r.p1.z - r.p2.z)/2.0,0.0 );
    SMat4x4 l = SMat4x4::LookAt(pos,pos+light_dir,SVec4(1.0,0,0,1));
  //  pos  = pos + SVec4(,0.0)
    c = c*l;
    //c.Reflect();
    d_shadowmap_cam0.setViewMatrix(c);
    d_shadowmap_cam1.setViewMatrix(c);
    d_shadowmap_cam2.setViewMatrix(c);
    d_shadowmap_cam3.setViewMatrix(c);




    r_prog->SetUniform("shadowMVPB0",Bias*d_shadowmap_cam0.getViewProjectMatrix());
    r_prog->SetUniform("shadowMVPB1",Bias*d_shadowmap_cam1.getViewProjectMatrix());
    r_prog->SetUniform("shadowMVPB2",Bias*d_shadowmap_cam2.getViewProjectMatrix());
    r_prog->SetUniform("shadowMVPB3",Bias*d_shadowmap_cam3.getViewProjectMatrix());



    /*update SSAO projection matrix*/
    pp_stage_ssao->getShader()->SetUniform("m_P",cam.getProjMatrix() );
    //pp_stage_ssao->getShader()->SetUniform("m_P",SMat4x4());

    return 0;

}


int inline SScene::RenderShadowMap(const RBO& v) {
    v.Bind(true);
    //incapsulation fail :(
    //todo: incapsulate uniform values to camera!
    cam_prog->SetUniform("MVP0",d_shadowmap_cam0.getProjMatrix()*d_shadowmap_cam0.getViewMatrix());
    cam_prog->SetUniform("MVP1",d_shadowmap_cam1.getProjMatrix()*d_shadowmap_cam1.getViewMatrix());
    cam_prog->SetUniform("MVP2",d_shadowmap_cam2.getProjMatrix()*d_shadowmap_cam2.getViewMatrix());
    cam_prog->SetUniform("MVP3",d_shadowmap_cam3.getProjMatrix()*d_shadowmap_cam3.getViewMatrix());
    RenderContext r_ctx(&v, cam_prog.get() ,&d_shadowmap_cam0);
    for (auto& r : d_render_list ) {
        r->Render(r_ctx);
    }
    return ESUCCESS;
}

int SScene::RenderCubemap()
{
    glClearColor(1.0,1.0,1.0,1.0);
    //glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glClampColor(GL_CLAMP_READ_COLOR, GL_FALSE);
    if (d_toggle_MSAA)
       msaa_pass.Bind();
    else
       normal_pass.Bind();

    //static int step = 0;
    //step ++;
    rtCubemap->Bind();
    SMat4x4 pos = SMat4x4().Move(100,-200.0,0.0);
    //d_debugDrawMgr.AddCross({(float)(0.0-step*100.0),200,1.0},50);
    d_debugDrawMgr.Update();
    SCamera cubemap_cam(pos,SPerspectiveProjectionMatrix(10,10000,1,toRad(90.0)));
    RenderContext r_ctx(rtCubemap.get() , cubemap_prog_generator.get() ,&cubemap_cam);
    RenderContext r_ctx2(rtCubemap.get() , r_prog.get() ,&cubemap_cam);

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

int SScene::RenderPrepass(const RBO &v)
{
    v.Bind(true);
    /*submit geometry for prepass*/
    RenderContext r_ctx_prepass(&v, prepass_prog.get() ,&cam);
    for (auto& r : d_render_list ) {
        r->Render(r_ctx_prepass);
    }
    return 0;
}
int inline SScene::RenderDirect(const RBO& v) {
    v.Bind(true);    
    if (rWireframe)
    {
        RenderContext r_ctx(&v, cam_prog.get() ,&d_shadowmap_cam0);
        for (auto& r : d_render_list ) {
            r->Render(r_ctx);
        }
    }
    else
    {
        RenderContext r_ctx(&v, r_prog.get() ,&cam,rtShadowMap->texDEPTH(),rtShadowMap->texIMG1(), rtCubemap->texIMG(), rtShadowMap->texIMG());
        r_ctx.sh_bands = rtConvoledCubemap;
        for (auto& r : d_render_list ) {
            r->Render(r_ctx);
        }
        RenderContext r_ctx2(&v, sky_dome_prog.get() ,&sky_cam,rtShadowMap->texDEPTH(),rtShadowMap->texIMG1() ,rtCubemap->texIMG(), rtShadowMap->texIMG());
        sky_dome_model->Render(r_ctx2);
    }

    //d_debugDrawMgr.Render(cam.getProjMatrix()*cam.getViewMatrix());
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
        rtSSAOVertBlurResult->Bind(true);
        pp_stage_ssao->Draw();
        rtSSAOHorBlurResult->Bind(true);
        pp_stage_ssao_blur_hor->Draw();
        rtSCREEN->Bind(true);
        pp_stage_ssao_blur_vert->Draw();
    } else if (dbg_ui.d_v_sel_current == DebugUI::V_SHADOW_MAP) {
        rtSCREEN->Bind(true); 
        RenderShadowMap( *rtSCREEN);
    } else if (dbg_ui.d_v_sel_current == DebugUI::V_VOLUMETRIC) {
        rtSCREEN->Bind(true); 

        pp_stage_volumetric->getShader()->SetUniform("sm_proj_matrix",d_shadowmap_cam0.getProjMatrix());
        pp_stage_volumetric->getShader()->SetUniform("sm_view_matrix",d_shadowmap_cam0.getViewMatrix());
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
