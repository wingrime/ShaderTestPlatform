#include "Scene.h"
#include "r_cprog.h"
#include <chrono>
SScene::SScene(RBO *v) 
    :rtSCREEN(v)
    ,con(new UIConsole(v,  d_console_cmd_handler ))
    ,d_console_cmd_handler(new ConsoleCommandHandler())
    ,d_shadowmap_cam(-1344.012,-1744.6,-43.26,0.4625, -1.57,0.0,SPerspectiveProjectionMatrix(10.0f, 10000.0f,1.0f,toRad(26.0)))
    ,cam(0,0,0,0,0,0,SPerspectiveProjectionMatrix(100.0f, 10000.0f,1.0f,toRad(26.0)))
    ,sky_cam(0,0,0,0,0,0,SPerspectiveProjectionMatrix(100.0f, 10000.0f,1.0f,toRad(26.0)))
    ,step(0.0f)
    
    ,err_con(new UIConsoleErrorHandler(con))
    ,fps_label(new UILabel(v,0.85,0.1))
    ,cfg_label(new UILabel(v,0.0, 0.6))
    ,v_sel_label(new UILabel(v,0.75, 0.6))

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
    ,model(new SObjModel("sponza.obj"))
    ,test_sphere_model(new SObjModel("sky_dome.obj"))
    ,rtCubemap(new RBO(128, 128, RBO::RBO_CUBEMAP))
   ,rtConvoledCubemap(new SRBOTexture(10,4,SRBOTexture::RT_TEXTURE_FLOAT_RED))
    ,rtHDRLogLum(new RBO(16,16,RBO::RBO_FLOAT)) /*Downsampled source for lumeneace*/
    ,rtHDRLumKey(new RBO(1,1,RBO::RBO_FLOAT)) /*Lum key out*/
    
{
    /*Setup error handler*/
    MainLog::GetInstance()->SetCallback([=](Log::Verbosity v, const std::string &s)-> void { con->Msg(s); });
    con->Msg("git revision: " GIT_SHA1 "\n");
    con->Msg("Model View\nShestacov Alexsey 2014-2015(c)\n");

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


    model->ConfigureProgram( *r_prog);
   
    model->ConfigureProgram( *cam_prog);

    model->ConfigureProgram( *cubemap_prog_generator);

    test_sphere_model->ConfigureProgram( *r_prog);
    //model->ConfigureProgram( *sky_dome_prog);

    sky_dome_model->ConfigureProgram( *sky_dome_prog);
    sky_dome_model->SetModelMat(SMat4x4().Scale(1000.0,1000.0,1000.0));
    test_sphere_model->SetModelMat(SMat4x4().Scale(2.0,2.0,2.0).Move(0.0,200.0,0.0));


    UpdateCfgLabel();
    UpdateViewSelLabel();
    InitDebugCommands();
    d_shadowmap_cam.LookAt(SVec4( 0.0,0.0, 0.0,1.0),SVec4(0.0,6000,0.0,1.0) ,   SVec4(1.0,0.0,0.0,1.0) );
    d_first_render = false;

}

int SScene::upCfgItem() {
    if (d_cfg_current > 0)
        d_cfg_current --;
    return 0;
}
int SScene::downCfgItem() {
    if (d_cfg_current < d_cfg_max )
        d_cfg_current++;
    return 0;
}
int SScene::incCfgItem() {
    d_cfg[d_cfg_current] +=d_cfg_step;
    return 0;
}

int SScene::decCfgItem() {
    d_cfg[d_cfg_current] -=d_cfg_step;
    return 0;
}

const inline std::string SScene::C_I(int n) {
    const std::string s("[>]");
    const std::string ns("[ ]");
    return (n==d_cfg_current?s:ns);
}
const inline std::string SScene::V_I(int n) {

    const std::string s("[+]");
    const std::string ns("[ ]");
    return (n==d_v_sel_current?s:ns);
}


int SScene::upViewItem() {
    if (d_v_sel_current > 0)
        d_v_sel_current --;
    UpdateViewSelLabel();
    return 0;
}
int SScene::downViewItem() {
    if (d_v_sel_current < d_v_sel_max )
        d_v_sel_current++;
    UpdateViewSelLabel();
    return 0;
}

int SScene::UpdateViewSelLabel() {
    v_sel_label->setText(std::string("------ViewSel-------\n") +
            V_I(V_NORMAL) + string_format("Full Render\n",d_v_sel[V_NORMAL]) +
            V_I(V_BLOOM) + string_format("Bright Pass \n",d_v_sel[V_BLOOM]) +
            V_I(V_BLOOM_BLEND) + string_format("Bloom Blend \n",d_v_sel[V_BLOOM_BLEND]) +
             V_I(V_SSAO) + string_format("SSAO Only \n",d_v_sel[V_SSAO]) +
            V_I(V_DIRECT) + string_format("Direct Render Only \n",d_v_sel[V_DIRECT])  +
            V_I(V_SHADOW_MAP) + string_format("Shadow Map Only \n",d_v_sel[V_SHADOW_MAP]) +
             V_I(V_VOLUMETRIC) + string_format("Volumetric Test \n",d_v_sel[V_VOLUMETRIC]) +
             V_I(V_CUBEMAPTEST) + string_format("LuxKey\n",d_v_sel[V_CUBEMAPTEST])
            );

    return ESUCCESS;
}
int SScene::UpdateCfgLabel() {



    cfg_label->setText(std::string("----------Settings-----------\n") + 
            C_I(0) + string_format("%f - SSAO Bloor Size\n",d_cfg[0]) +
            C_I(1) + string_format("%f - SSAO Size \n",d_cfg[1])  +
            C_I(2) + string_format("%f - SSAO LevelClamp\n",d_cfg[2]) + 
            C_I(3) + string_format("%f - SSAO DepthClamp\n",d_cfg[3]) + 
            C_I(4) + string_format("%f - HDR Bloom Clamp\n",d_cfg[4]) +
            C_I(5) + string_format("%f - HDR Bloom Mul\n",d_cfg[5])   +
            C_I(6) + string_format("%f - HDR Bloor Size\n",d_cfg[6])  +
            C_I(7) + string_format("%f - HDR (A) Shoulder Strength\n",d_cfg[7])  +
            C_I(8) + string_format("%f - HDR (B) Lineral Strength\n",d_cfg[8])  +
            C_I(9) + string_format("%f - HDR (C) Lineral Angle\n",d_cfg[9])  +
            C_I(10) + string_format("%f - HDR (D) Toe Strength\n",d_cfg[10])  +
            C_I(11) + string_format("%f - HDR (E) Toe Numerator\n",d_cfg[11])  +
            C_I(12) + string_format("%f - HDR (F) Toe Numerator\n",d_cfg[12])  +
            C_I(13) + string_format("%f - HDR (LW) Lineral White\n",d_cfg[13])  +
             C_I(14) + string_format("%f - Base Light Power Multipler\n",d_cfg[14])
            );

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


    //pp_prog_hdr_blur_hor->SetUniform("blurSize",d_cfg[6]);
    //pp_prog_hdr_blur_vert->SetUniform("blurSize",d_cfg[6]);


    pp_prog_hdr_tonemap->SetUniform("A",d_cfg[7]);
    pp_prog_hdr_tonemap->SetUniform("B",d_cfg[8]);
    pp_prog_hdr_tonemap->SetUniform("C",d_cfg[9]);
    pp_prog_hdr_tonemap->SetUniform("D",d_cfg[10]);
    pp_prog_hdr_tonemap->SetUniform("E",d_cfg[11]);
    pp_prog_hdr_tonemap->SetUniform("F",d_cfg[12]);
    pp_prog_hdr_tonemap->SetUniform("LW",float((d_cfg[13])));

    r_prog->SetUniform("lightIntensity",d_cfg[14]);

    return ESUCCESS;
}
int SScene::Reshape(int w, int h) {

        rtSCREEN->Resize(SVec2(w,h));
        /*ToDo remake*/
        glViewport ( 0, 0, (GLsizei)w, (GLsizei)h );

    return ESUCCESS;
}
int SScene::UpdateScene() {
   sky_cam.SyncFromCamera(cam);
    r_prog->Bind();
    r_prog->SetUniform("sm_projection_mat",d_shadowmap_cam.getProjMatrix());
    r_prog->SetUniform("sm_view_mat",d_shadowmap_cam.getViewMatrix());
    return 0;

}

int SScene::InitDebugCommands()
{
    d_console_cmd_handler->AddCommand("cls", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        con->Cls();
    }));

    d_console_cmd_handler->AddCommand("r_set_f", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;
        float val_f = std::stof(args[2]);
        r_prog->SetUniform(args[1],val_f);

    }));
    d_console_cmd_handler->AddCommand("r_set_i", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;
        int val_f = std::stoi(args[2]);
        r_prog->SetUniform(args[1],val_f);

    }));
       d_console_cmd_handler->AddCommand("sky_set_f", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;
        float val_f = std::stof(args[2]);
        sky_dome_prog->SetUniform(args[1],val_f);

    }));

    d_console_cmd_handler->AddCommand("sm_cam_set", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;
        d_shadowmap_cam = cam;

    }));
    d_console_cmd_handler->AddCommand("dump_cam", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;

         std::ostringstream os;
        {
            /*use raii */
            cereal::JSONOutputArchive archive( os);
            archive( CEREAL_NVP( cam));
        }
        con->Msg(os.str());

    }));
    d_console_cmd_handler->AddCommand("dump_model", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;

         std::ofstream os("model.json");
        {
            /*use raii */
            cereal::JSONOutputArchive archive( os);
            archive( CEREAL_NVP( model));
        }
        //con->Msg(os.str());

    }));
    d_console_cmd_handler->AddCommand("mem_stats", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;
        /*
         * NVX_gpu_memory_info
         * requied
         */
        int dedVideoMem,totalAvailableMem, curAvailableMem, evictionCount, evictedMem;
            glGetIntegerv(0x9047, &dedVideoMem);
            glGetIntegerv(0x9048, &totalAvailableMem);
            glGetIntegerv(0x9049, &curAvailableMem);
            glGetIntegerv(0x904A, &evictionCount);
            glGetIntegerv(0x904B, &evictedMem);
            con->Msg(string_format("Dedicated mem: %d[kb]\nTotal available mem: %d[kb]\nCurrent available mem: %d[kb]\nEviction count: %d\nEvicted mem: %d[kb]\n",
                            dedVideoMem,totalAvailableMem, curAvailableMem, evictionCount, evictedMem));
        //con->Msg(os.str());

    }));




        d_console_cmd_handler->AddCommand("sm_cam", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;
        d_shadowmap_cam.LookAt(SVec4( 0.0,0.0, 0.0,1.0),SVec4(0.0,4000,0.0,1.0) ,   SVec4(1.0,0.0,0.0,1.0) );

    }));
        d_console_cmd_handler->AddCommand("dump_smcam", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;

         std::ostringstream os;
        {
            /*use raii */
            cereal::JSONOutputArchive archive( os);
            archive( CEREAL_NVP( d_shadowmap_cam));
        }
        con->Msg(os.str());
    }));

    d_console_cmd_handler->AddCommand("toggle_ui", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        d_toggle_fps_view = !d_toggle_fps_view;
        d_toggle_cfg_view = !d_toggle_cfg_view;

    }));
    d_console_cmd_handler->AddCommand("toggle_msaa", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        d_toggle_MSAA= !d_toggle_MSAA;

    }));
    d_console_cmd_handler->AddCommand("toggle_brightpass", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        d_toggle_brightpass= !d_toggle_brightpass;

    }));

    d_console_cmd_handler->AddCommand("goto", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;
        SVec4 vect(args[1]);
        cam.goPosition(vect);
        cam.rotEuler(SVec4(0,0,0,0));
    }));

    d_console_cmd_handler->AddCommand("rot_x", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;
        float val_f = std::stof(args[1]);
        cam.goPosition(SVec4(0.0,0.0,0.0,0.0));
        cam.rotEulerX(toRad(val_f));
    }));
    d_console_cmd_handler->AddCommand("rot_y", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;
        float val_f = std::stof(args[1]);
        cam.goPosition(SVec4(0.0,0.0,0.0,0.0));
        cam.rotEulerY(toRad(val_f));
    }));
    d_console_cmd_handler->AddCommand("rot_z", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;
        float val_f = std::stof(args[1]);
        cam.goPosition(SVec4(0.0,0.0,0.0,0.0));
        cam.rotEulerZ(toRad(val_f));
    }));

    d_console_cmd_handler->AddCommand("li", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;
        SVec4 vect(args[1]);
        r_prog->SetUniform("main_light_dir", vect);
        d_debugDrawMgr.AddCross({vect.x,vect.y,vect.z},20);

    }));


    d_console_cmd_handler->AddCommand("rec", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;
        d_first_render = false;
    }));

    d_console_cmd_handler->AddCommand("updc", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;
        SMat4x4 m = cam.getViewMatrix();
        d_debugDrawMgr.AddCross({m.mat.a14,m.mat.a24,m.mat.a34},1000);
        d_debugDrawMgr.Update();
    }));

    d_console_cmd_handler->AddCommand("load", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;
        con->Msg("Load new model..");
        model.reset(new SObjModel(args[1]));
        model->ConfigureProgram( *r_prog);
        model->ConfigureProgram( *cam_prog);
        model->ConfigureProgram( *cubemap_prog_generator);

    }));

}
int inline SScene::RenderShadowMap(const RBO& v) {
    v.Bind(true);
    RenderContext r_ctx(&v, cam_prog ,&d_shadowmap_cam);
    model->Render(r_ctx);
    test_sphere_model->Render(r_ctx);
    return ESUCCESS;
}

int SScene::RenderCubemap()
{
    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glClampColor(GL_CLAMP_READ_COLOR, GL_FALSE);
    glDepthMask(true);
    glDepthFunc  ( GL_LEQUAL );
    if (d_toggle_MSAA)
       glEnable( GL_MULTISAMPLE );
    else
        glDisable( GL_MULTISAMPLE );
    UpdateScene();

    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    glCullFace(GL_FRONT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc  ( GL_LEQUAL );
    static int step = 0;
    step ++;
    rtCubemap->Bind();
    SMat4x4 pos = SMat4x4().Move(0.0+step*100,-200.0,0.0);
    d_debugDrawMgr.AddCross({0.0-step*100,200,1.0},50);
    d_debugDrawMgr.Update();
    SCamera cubemap_cam(pos,SPerspectiveProjectionMatrix(10,10000,1,toRad(90.0)));
    RenderContext r_ctx(rtCubemap.get() , cubemap_prog_generator ,&cubemap_cam);
    RenderContext r_ctx2(rtCubemap.get() , r_prog ,&cubemap_cam);

    sky_dome_model->Render(r_ctx);
    model->Render(r_ctx);
    // Convolve it !!
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
        model->Render(r_ctx);
    }
    else
    {
        RenderContext r_ctx(&v, r_prog ,&cam,rtShadowMap->texDEPTH(),rtShadowMap->texIMG1(), rtCubemap->texIMG(), rtShadowMap->texIMG());
        r_ctx.sh_bands = rtConvoledCubemap;
        model->Render(r_ctx);
        test_sphere_model->Render(r_ctx);
        RenderContext r_ctx2(&v, sky_dome_prog ,&cam,rtShadowMap->texDEPTH(),rtShadowMap->texIMG1() ,rtCubemap->texIMG(), rtShadowMap->texIMG());
        sky_dome_model->Render(r_ctx2);

    }

    d_debugDrawMgr.Render(cam.getProjMatrix()*cam.getViewMatrix());

    return 0;
}
int SScene::Render() {
    auto start = std::chrono::steady_clock::now();
    step  += 0.002f;
    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); 
    glClampColor(GL_CLAMP_READ_COLOR, GL_FALSE);
    glDepthFunc  ( GL_LEQUAL );
    if (d_toggle_MSAA)
       glEnable( GL_MULTISAMPLE );
    else
        glDisable( GL_MULTISAMPLE );
    UpdateScene();
    glEnable(GL_DEPTH_TEST);
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    glCullFace(GL_FRONT);
    if (!d_first_render ) {
        d_first_render = true;
        RenderCubemap();
    }
    rtime.Begin();
    if (d_v_sel_current == V_DIRECT) {
        //RenderShadowMap( *rtSCREEN);
        
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

    glDisable(GL_DEPTH_TEST);
    glDepthFunc(GL_NEVER);  
    
    /*Bloom + SSAO + RenderShadowMap*/
    if (d_v_sel_current == V_NORMAL) {
        /*LumKEY*/

        pp_stage_hdr_lum_log->DrawRBO(false);
        pp_stage_hdr_lum_key->DrawRBO(false);
        if (d_toggle_brightpass ) {

        pp_stage_hdr_bloom->DrawRBO(false);
        int blurSizeLoc = pp_prog_hdr_blur_kawase->getUniformLocation("blurSize");
        pp_prog_hdr_blur_kawase->SetUniform(blurSizeLoc,(float)0.0);
        pp_stage_hdr_blur_hor->DrawRBO(false);
        pp_prog_hdr_blur_kawase->SetUniform(blurSizeLoc,(float)(d_cfg[6]*1.0));
        pp_stage_hdr_blur_vert->DrawRBO(false);

        /*ping pong 1*/
        pp_prog_hdr_blur_kawase->SetUniform(blurSizeLoc,(float)(d_cfg[6]*2.0));
        pp_stage_hdr_blur_hor2->DrawRBO(false);
        pp_prog_hdr_blur_kawase->SetUniform(blurSizeLoc,(float)(d_cfg[6]*2.0));
        pp_stage_hdr_blur_vert2->DrawRBO(false);
        /*ping pong 2*/
        pp_prog_hdr_blur_kawase->SetUniform(blurSizeLoc,(float)(d_cfg[6]*3.0));
        pp_stage_hdr_blur_hor2->DrawRBO(false);
        pp_prog_hdr_blur_kawase->SetUniform(blurSizeLoc,(float)(d_cfg[6]*3.0));
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

    } else if (d_v_sel_current == V_BLOOM) {
        rtSCREEN->Bind(true); 
        pp_stage_hdr_bloom->Draw();
        
    } else if (d_v_sel_current == V_BLOOM_BLEND) {
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
    } else if (d_v_sel_current == V_SSAO) {
        rtSSAOVertBlurResult->Bind(true);
        pp_stage_ssao->Draw();
        rtSSAOHorBlurResult->Bind(true);
        pp_stage_ssao_blur_hor->Draw();
        rtSCREEN->Bind(true);
        pp_stage_ssao_blur_vert->Draw();
    } else if (d_v_sel_current == V_SHADOW_MAP) {
        rtSCREEN->Bind(true); 
        RenderShadowMap( *rtSCREEN);
    } else if (d_v_sel_current == V_VOLUMETRIC) {
        rtSCREEN->Bind(true); 

        pp_stage_volumetric->getShader()->SetUniform("sm_proj_matrix",d_shadowmap_cam.getProjMatrix());
        pp_stage_volumetric->getShader()->SetUniform("sm_view_matrix",d_shadowmap_cam.getViewMatrix());
        pp_stage_volumetric->getShader()->SetUniform("cam_proj_matrix",cam.getProjMatrix());
        pp_stage_volumetric->getShader()->SetUniform("cam_view_matrix",cam.getViewMatrix());
        pp_stage_volumetric->Draw();
    }
    else if (d_v_sel_current == V_CUBEMAPTEST ) {

        //glEnable(GL_DEPTH_TEST);
        //glDepthFunc  ( GL_LEQUAL );
        //RenderContext r_ctx(rtSCREEN.get(), cubemap_prog_generator ,&cam,rtShadowMap->texDEPTH(),rtShadowMap->texIMG1(), rtShadowMap->texIMG2(), rtShadowMap->texIMG());
        //model->Render(r_ctx);
        rtSCREEN->Bind(true);
        pp_stage_hdr_lum_log->Draw();

        //pp_stage_hdr_lum_key->Draw();
    }
    pp_time.End();
    ui_time.Begin();
 
    if (d_toggle_fps_view)
        fps_label->Draw();
    if (d_toggle_cfg_view) {
        cfg_label->Draw();
        v_sel_label->Draw();
    }
    con->Draw();
    ui_time.End();
    char buf [90];
    auto end = std::chrono::steady_clock::now();

    auto diff = end- start;

    sprintf(buf,"DRAW:%4.3f ms\nUI: %4.3f ms\nPP: %4.3f ms\nCPU: %4.3f\n",      (float)rtime.getTime()*(1.0/ 1000000.0), \
                                                                    (float)ui_time.getTime()*(1.0/1000000.0),\
                                                                    (float)pp_time.getTime()*(1.0/1000000.0),\
                                                                      std::chrono::duration <float, std::milli> (diff).count() );
    fps_label->setText(buf);
   
    return true;
}
