#include "Scene.h"

SScene::SScene(RBO *v) 
    :rtSCREEN(v)
    ,con(new UIConsole(v,  d_console_cmd_handler ))
    ,d_console_cmd_handler(new ConsoleCommandHandler())
    ,d_shadowmap_cam(-1344.012,-1744.6,-43.26,0.4625, -1.57,0.0,SPerspectiveProjectionMatrix(100.0f, 5000.0f,1.0f,toRad(26.0)))
    ,cam(0,0,0,0,0,0,SPerspectiveProjectionMatrix(100.0f, 10000.0f,1.0f,toRad(26.0)))
    ,sky_cam(0,0,0,0,0,0,SPerspectiveProjectionMatrix(100.0f, 10000.0f,1.0f,toRad(26.0)))
    ,step(0.0f)
    
    ,err_con(new UIConsoleErrorHandler(con))
    ,fps_label(new UILabel(v))
    ,cfg_label(new UILabel(v,0.0, 0.7))
    ,v_sel_label(new UILabel(v,0.0, 0.55))
    ,rtShadowMap(new RBO((v->w),(v->h),SRBOTexture::RT_TEXTURE_RGBA,
            std::shared_ptr<SRBOTexture>(new SRBOTexture(v->w*2,v->h*2,SRBOTexture::RT_TEXTURE_RGBA)), /*color */
            std::shared_ptr<SRBOTexture>(new SRBOTexture(v->w,v->h,SRBOTexture::RT_TEXTURE_RGBA)), /*rsm*/
            std::shared_ptr<SRBOTexture>(new SRBOTexture(v->w,v->h,SRBOTexture::RT_TEXTURE_RGBA)), /*rsm */
            std::shared_ptr<SRBOTexture>(new SRBOTexture(v->w,v->h,SRBOTexture::RT_TEXTURE_DEPTH) ))) /*shadow map render from light*/
    ,rtHDRScene_MSAA(new RBO((v->w),(v->h),SRBOTexture::RT_TEXTURE_FLOAT,
            std::shared_ptr<SRBOTexture>(new SRBOTexture(v->w,v->h,SRBOTexture::RT_TEXTURE_MSAA)), /*IMG0 color*/
            std::shared_ptr<SRBOTexture>(new SRBOTexture(v->w,v->h,SRBOTexture::RT_TEXTURE_MSAA)), /*IMG1 normal in world space */
            nullptr,    /*IMG2*/
            std::shared_ptr<SRBOTexture>(new SRBOTexture(v->w,v->h,SRBOTexture::RT_TEXTURE_DEPTH_MSAA) ))) /*depth for Volumetric*/
    ,rtHDRScene(new RBO((v->w),(v->h),SRBOTexture::RT_TEXTURE_FLOAT,
            std::shared_ptr<SRBOTexture>(new SRBOTexture(v->w,v->h,SRBOTexture::RT_TEXTURE_FLOAT)), /*IMG0 color*/
            std::shared_ptr<SRBOTexture>(new SRBOTexture(v->w,v->h,SRBOTexture::RT_TEXTURE_FLOAT)), /*IMG1 normal in world space */
            nullptr,    /*IMG2*/
            std::shared_ptr<SRBOTexture>(new SRBOTexture(v->w,v->h,SRBOTexture::RT_TEXTURE_DEPTH) ))) /*depth for Volumetric*/
    ,rtHDRBloomResult( new RBO((v->w)/2, (v->h)/2, SRBOTexture::RT_TEXTURE_RGBA)) /* bloom clamp*/
    ,rtHDRHorBlurResult(new RBO((v->w)/4, (v->h)/4, SRBOTexture::RT_TEXTURE_RGBA))
    ,rtHDRVertBlurResult(new RBO((v->w)/4, (v->h)/4, SRBOTexture::RT_TEXTURE_RGBA))

    ,rtSSAOResult( new RBO((v->w), (v->h), SRBOTexture::RT_TEXTURE_RED)) /*fix me some time*/
    ,rtSSAOBLUR2(new RBO((v->w), (v->h), SRBOTexture::RT_TEXTURE_RED))
    ,rtVolumetric(new RBO((v->w), (v->h), SRBOTexture::RT_TEXTURE_RGBA))
    ,sky_dome_model(new SObjModel("sky_dome.obj"))
    ,model(new SObjModel("sponza.obj"))
    ,rtCubemap(new RBO(1024, 1024, SRBOTexture::RT_TEXTURE_CUBEMAP))
   
    
{
    /*Setup error handler*/
    MainLog::GetInstance()->SetCallback([=](Log::Verbosity v, const std::string &s)-> void { con->Msg(s); });
    int w = rtHDRScene->w;
    int h = rtHDRScene->h;
    /*ssao shaders*/
    pp_prog_ssao = new SShader("pp_quad.v","pp_shader.f");
    pp_prog_ssao_blur_hor = new SShader("pp_quad.v","pp_gauss_hor.f");
    pp_prog_ssao_blur_vert = new SShader("pp_quad.v","pp_gauss_vert.f");
    /*bloom shaders */
    pp_prog_hdr_bloom = new SShader("pp_quad.v", "pp_bloom.f");
    pp_prog_hdr_blur_hor = new SShader("pp_quad.v","pp_gauss_hor.f");
    pp_prog_hdr_blur_vert = new SShader("pp_quad.v","pp_gauss_vert.f");
    pp_prog_hdr_tonemap = new SShader("pp_quad.v","pp_hdr_tonemap.f");

    pp_prog_hdr_blur_kawase = new SShader("pp_quad.v","pp_blur_kawase.f");

    pp_stage_ssao =  new SPostProcess(pp_prog_ssao,w,h,rtHDRScene->texIMG1,rtHDRScene->texDEPTH);

    pp_stage_ssao_blur_hor=  new SPostProcess(pp_prog_ssao_blur_hor,w,h,rtSSAOResult->texIMG,rtSSAOResult->texIMG);
    pp_stage_ssao_blur_vert =  new SPostProcess(pp_prog_ssao_blur_vert,w,h,rtSSAOBLUR2->texIMG,rtHDRScene->texIMG);


   



    pp_stage_hdr_bloom =  new SPostProcess(pp_prog_hdr_bloom,w/2,h/2,rtHDRScene->texIMG);
    pp_stage_hdr_blur_hor =   new SPostProcess(pp_prog_hdr_blur_kawase, w/4.0,h/4.0 ,rtHDRBloomResult->texIMG);
    pp_stage_hdr_blur_vert =  new SPostProcess(pp_prog_hdr_blur_kawase,w/4.0,h/4.0 ,rtHDRHorBlurResult->texIMG);
    /*ping pong*/
    pp_stage_hdr_blur_hor2 =   new SPostProcess(pp_prog_hdr_blur_kawase, w/4.0,h/4.0 ,rtHDRVertBlurResult->texIMG);
    pp_stage_hdr_blur_vert2 =  new SPostProcess(pp_prog_hdr_blur_kawase,w/4.0,h/4.0 ,rtHDRHorBlurResult->texIMG);


    pp_stage_hdr_tonemap =  new SPostProcess(pp_prog_hdr_tonemap,w,h,rtHDRVertBlurResult->texIMG,rtHDRScene->texIMG,rtSSAOResult->texIMG);


    /*volumetric */
    pp_prog_volumetric = new SShader("pp_quad.v","pp_volumetric.f");
    /* img depth | shadow depth | shadow world pos*/
    pp_stage_volumetric =  new SPostProcess(pp_prog_volumetric,w,h,rtHDRScene->texDEPTH,rtShadowMap->texDEPTH,rtShadowMap->texIMG2);

    /*main prog*/
    r_prog = new SShader("shader.v","shader.f");
    /*camera_side prog*/
    cam_prog = new SShader("sm_shader.v","sm_shader.f");

    /*sky dome prog*/
    sky_dome_prog = new SShader("shader.v","shader_sky.f");

    cubemap_prog_generator = new SShader("cubemap_gen.v","cubemap_gen.f","cubemap_gen.g");





   // model.reset(new SObjModel("sponza.obj"));

    model->ConfigureProgram( *r_prog);
   
    model->ConfigureProgram( *cam_prog);

    model->ConfigureProgram( *cubemap_prog_generator);

    //model->ConfigureProgram( *sky_dome_prog);

    sky_dome_model->ConfigureProgram( *sky_dome_prog);
    sky_dome_model->SetModelMat(SMat4x4().Scale(1000.0,1000.0,1000.0));
    //SShader shader("shader_config.json");
    //shader.ReflectUniforms();
    //shader.Bind(); 

    con->Msg("Model View\nShestacov Alexsey 2014 (c)\n"); 
    UpdateCfgLabel();
    UpdateViewSelLabel();

 //   std::ostringstream os;// ("out2_model.json");
 //   {
        /*use raii */
    //cereal::PortableBinaryOutputArchive archive( os);
  //      cereal::JSONOutputArchive archive( os);

//        archive( CEREAL_NVP( model));
   // }//

    //std::ofstream o("sky_dome_test.json");{
    //    cereal::JSONOutputArchive archive( o);
    //    archive( CEREAL_NVP( sky_dome_model));
    //}

    /*
    state.Load("config.lua");
    std::string s = os.str();
    state["engine_view_json"] = s;
    state["main"]();
    */
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

        d_console_cmd_handler->AddCommand("sm_cam", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;
        float val_f = std::stof(args[1]);
        d_shadowmap_cam.LookAt(SVec4( 0.0,0.0, 0.0,1.0),SVec4(0.0,val_f,0.0,1.0) ,   SVec4(1.0,0.0,0.0,1.0) );

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

    d_console_cmd_handler->AddCommand("toggle_fps", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        d_toggle_fps_view = !d_toggle_fps_view;

    }));
    d_console_cmd_handler->AddCommand("toggle_cfg", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        d_toggle_cfg_view = !d_toggle_cfg_view;

    }));
        d_console_cmd_handler->AddCommand("toggle_msaa", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        d_toggle_MSAA= !d_toggle_MSAA;

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
    }));

d_shadowmap_cam.LookAt(SVec4( 0.0,0.0, 0.0,1.0),SVec4(0.0,1000,0.0,1.0) ,   SVec4(1.0,0.0,0.0,1.0) );

RenderCubemap();

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


    v_sel_label->setText(std::string("----------ViewSel-----------\n") + 
            V_I(V_NORMAL) + string_format("Normal Render\n",d_v_sel[V_NORMAL]) +
            V_I(V_BLOOM) + string_format("Bloom  \n",d_v_sel[V_BLOOM]) +
            V_I(V_BLOOM_BLEND) + string_format("Bloom Blend \n",d_v_sel[V_BLOOM_BLEND]) +
             V_I(V_SSAO) + string_format("SSAO Only \n",d_v_sel[V_SSAO]) +
            V_I(V_DIRECT) + string_format("Direct Render Only \n",d_v_sel[V_DIRECT])  +
            V_I(V_SHADOW_MAP) + string_format("Shadow Map Only \n",d_v_sel[V_SHADOW_MAP]) +
             V_I(V_VOLUMETRIC) + string_format("Volumetric Only \n",d_v_sel[V_VOLUMETRIC]) +
             V_I(V_CUBEMAPTEST) + string_format("Cubemap Test \n",d_v_sel[V_CUBEMAPTEST])
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
             C_I(14) + string_format("%f - Base Light Interisty\n",d_cfg[14]) 
            );

    pp_prog_ssao_blur_hor->SetUniform("blurSize",d_cfg[0]);
    pp_prog_ssao_blur_vert->SetUniform("blurSize",d_cfg[0]);
    /*dbg*/
    pp_prog_hdr_tonemap->SetUniform("aoStrength",d_cfg[0]);
    

    pp_prog_ssao->SetUniform("ssaoSize",d_cfg[1]);
    pp_prog_ssao->SetUniform("ssaoLevelClamp",d_cfg[2]);
    pp_prog_ssao->SetUniform("ssaoDepthClamp",d_cfg[3]);

    pp_prog_hdr_bloom->SetUniform("hdrBloomClamp",d_cfg[4]);
    pp_prog_hdr_bloom->SetUniform("hdrBloomMul",d_cfg[5]);


    pp_prog_hdr_blur_hor->SetUniform("blurSize",d_cfg[6]);
    pp_prog_hdr_blur_vert->SetUniform("blurSize",d_cfg[6]);


    pp_prog_hdr_tonemap->SetUniform("A",d_cfg[7]);
    pp_prog_hdr_tonemap->SetUniform("B",d_cfg[8]);
    pp_prog_hdr_tonemap->SetUniform("C",d_cfg[9]);
    pp_prog_hdr_tonemap->SetUniform("D",d_cfg[10]);
    pp_prog_hdr_tonemap->SetUniform("E",d_cfg[11]);
    pp_prog_hdr_tonemap->SetUniform("F",d_cfg[12]);
    pp_prog_hdr_tonemap->SetUniform("LW",d_cfg[13]);

    r_prog->SetUniform("lightIntensity",d_cfg[14]);

    return ESUCCESS;
}
int SScene::Reshape(int w, int h) {
        rtSCREEN->w = w;
        rtSCREEN->h = h;
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
int inline SScene::RenderShadowMap(const RBO& v) {
    v.Bind(true);
    RenderContext r_ctx(&v, cam_prog ,&d_shadowmap_cam);
    model->Render(r_ctx);
    return ESUCCESS;
}

int SScene::RenderCubemap()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc  ( GL_LEQUAL );
    rtCubemap->Bind();

    RenderContext r_ctx(rtCubemap.get() , cubemap_prog_generator ,&cam);
    model->Render(r_ctx);

}
int inline SScene::RenderDirect(const RBO& v) {
    v.Bind(true);    
    /*connect shadowmap to model sampler*/
    
    if (rWireframe)
    {
        RenderContext r_ctx(&v, cam_prog ,&d_shadowmap_cam);
        model->Render(r_ctx);

    }
    else
    {

        RenderContext r_ctx(&v, r_prog ,&cam,rtShadowMap->texDEPTH,rtShadowMap->texIMG1, rtCubemap->texIMG, rtShadowMap->texIMG);
        model->Render(r_ctx);

        RenderContext r_ctx2(&v, sky_dome_prog ,&cam,rtShadowMap->texDEPTH,rtShadowMap->texIMG1 ,rtCubemap->texIMG, rtShadowMap->texIMG);
        //RenderContext r_ctx2(&v, r_prog ,&cam,rtShadowMap->texDEPTH,rtShadowMap->texIMG1, rtShadowMap->texIMG2, rtShadowMap->texIMG); 
        sky_dome_model->Render(r_ctx2);
    }

   // if (rWireframe) 
      //  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE  );
    //else
    //    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    
   
    if (model->IsReady) 
    {
        //model->SetCamera(cam.getMatrix());
        //model->Render(r_ctx);
    }

   // glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    return 0;
}
int SScene::Render() {
    
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
    
    rtime.Begin();
    RenderCubemap();
    if (d_v_sel_current == V_DIRECT) {
        //RenderShadowMap( *rtSCREEN);
        
        RenderDirect( *rtSCREEN);

    }
       
    else {


        RenderShadowMap( *rtShadowMap);
        if (d_toggle_MSAA) {
            RenderDirect( *rtHDRScene_MSAA);
            rtHDRScene_MSAA->ResolveMSAA(*rtHDRScene);
        } else
            RenderDirect( *rtHDRScene);
    }
    glFlush();
    rtime.End();
    pp_time.Begin();

    glDisable(GL_DEPTH_TEST);
    glDepthFunc(GL_NEVER);  
    

    /*Bloom + SSAO + RenderShadowMap*/
    if (d_v_sel_current == V_NORMAL) {
    


        rtHDRBloomResult->Bind(false);
        pp_stage_hdr_bloom->Draw(); 

        rtHDRHorBlurResult->Bind(true);
        pp_prog_hdr_blur_kawase->SetUniform("blurSize",(float)0.0);
        pp_stage_hdr_blur_hor->Draw();
        rtHDRVertBlurResult->Bind(false);
        pp_prog_hdr_blur_kawase->SetUniform("blurSize",(float)(d_cfg[6]*1.0));
        pp_stage_hdr_blur_vert->Draw();

        /*ping pong 1*/
        rtHDRHorBlurResult->Bind(false);
        pp_prog_hdr_blur_kawase->SetUniform("blurSize",(float)(d_cfg[6]*2.0));
        pp_stage_hdr_blur_hor2->Draw();
        rtHDRVertBlurResult->Bind(false);
        pp_prog_hdr_blur_kawase->SetUniform("blurSize",(float)(d_cfg[6]*2.0));
        pp_stage_hdr_blur_vert2->Draw();
        /*ping pong 2*/
        rtHDRHorBlurResult->Bind(false);
        pp_prog_hdr_blur_kawase->SetUniform("blurSize",(float)(d_cfg[6]*3.0));
        pp_stage_hdr_blur_hor2->Draw();
        pp_prog_hdr_blur_kawase->SetUniform("blurSize",(float)(d_cfg[6]*3.0));
        rtHDRVertBlurResult->Bind(false);
        pp_stage_hdr_blur_vert2->Draw();
        /*SSAO*/

        rtSSAOResult->Bind(true);
        pp_stage_ssao->Draw();
        rtSSAOBLUR2->Bind(true);
        pp_stage_ssao_blur_hor->Draw();
        rtSSAOResult->Bind(true);
        pp_stage_ssao_blur_vert->Draw();


        rtSCREEN->Bind(false);
        pp_stage_hdr_tonemap->Draw(); 
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
        rtSSAOResult->Bind(true);
        pp_stage_ssao->Draw();
        rtSSAOBLUR2->Bind(true);
        pp_stage_ssao_blur_hor->Draw();
        rtSCREEN->Bind(true);
        pp_stage_ssao_blur_vert->Draw();
    } else if (d_v_sel_current == V_SHADOW_MAP) {
        rtSCREEN->Bind(true); 
        RenderShadowMap( *rtSCREEN);
    } else if (d_v_sel_current == V_VOLUMETRIC) {

/*
uniform mat4 sm_proj_matrix;
uniform mat4 sm_view_matrix;
uniform mat4 cam_proj_matrix;
uniform mat4 cam_view_matrix;
*/
        rtSCREEN->Bind(true); 
        pp_prog_volumetric->SetUniform("sm_proj_matrix",d_shadowmap_cam.getProjMatrix());
        pp_prog_volumetric->SetUniform("sm_view_matrix",d_shadowmap_cam.getViewMatrix());
        pp_prog_volumetric->SetUniform("cam_proj_matrix",cam.getProjMatrix());
        pp_prog_volumetric->SetUniform("cam_view_matrix",cam.getViewMatrix());
        pp_stage_volumetric->Draw();
    }
    else if (d_v_sel_current == V_CUBEMAPTEST ) {


        rtSCREEN->Bind(true);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc  ( GL_LEQUAL );
        //glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        //glCullFace(GL_FRONT);
        RenderContext r_ctx(rtSCREEN.get(), cubemap_prog_generator ,&cam,rtShadowMap->texDEPTH,rtShadowMap->texIMG1, rtShadowMap->texIMG2, rtShadowMap->texIMG);
        model->Render(r_ctx);
    }

    glFlush();
    pp_time.End();
    ui_time.Begin();
 
    if (d_toggle_fps_view)
        fps_label->Draw();
    if (d_toggle_cfg_view) {
        cfg_label->Draw();
        v_sel_label->Draw();
    }
    con->Draw();
    glFlush();
    ui_time.End();
   
    fps_label->setText(string_format("DRAW:%4.3f ms\nUI: %4.3f ms\nPP: %4.3f ms\n",\
                                                             (float)rtime.getTime()*(1.0/ 1000000.0),
                                                             (float)ui_time.getTime()*(1.0/1000000.0),
                                                             (float)pp_time.getTime()*(1.0/1000000.0)  ));
   
    return true;
}
