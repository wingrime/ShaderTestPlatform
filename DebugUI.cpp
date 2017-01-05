#include "DebugUI.h"
#include "Scene.h"
#include "UILabel.h"
#include "UIConsole.h"
#include "Shader.h"
#include <imgui.h>
#include <RBO.h>
#include "MAssert.h"
#include "IMGuiHooks.h"
#include "c_config.h"
#pragma warning(disable : 4996)  
extern  GlobalInputState g_InputState;

DebugUI::DebugUI(RectSize &_v)
    :fps_label(new UILabel(_v,0.8f,0.02f))
{
    /*Setup error handler*/
    d_console_cmd_handler = new ConsoleCommandHandler();
    err_con = new UIConsoleErrorHandler(con);
    con = new UIConsole(_v,  d_console_cmd_handler );
    Singltone<Log>::GetInstance()->SetCallback([=](Log::Verbosity v, const std::string &s)-> void {UNUSED(v); con->Msg(s); });
    InitDebugCommands();

    sc = Singltone<SScene>::GetInstance();
    imGuiSetup();
}
int DebugUI::ToggleFPSCounter(bool b) 
{
    d_toggle_fps_view = b;
	return 0;
}
int DebugUI::Draw()
{
    if (d_toggle_fps_view)
    {
        char buf [120];
        float pptime = sc->debugGetPostProcessingTime();
        float rtime = sc->debugGetRenderTime();
        float fps = 1000.0f/(pptime+rtime);
        long int frameNumber = sc->debugGetFrameNumber();
        sprintf(buf,"DRAW:%4.3f ms\nPP: %4.3f ms\nFPS: %4.3f\nframe:%d\n", rtime ,pptime, fps, frameNumber );
        fps_label->setText(buf);
        fps_label->Draw();
    }
    if (d_toggleConsoleOverlay)
        con->Draw();
    DrawGUI();




    return ESUCCESS;

}

int DebugUI::Reshape(const RectSize &v)
{
    /*ImGui hook*/
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize.x = (float)v.w;
    io.DisplaySize.y =  (float)v.h;
    return ESUCCESS;

}
int DebugUI::InitDebugCommands()
{
    d_console_cmd_handler->AddCommand("cls", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(arg_list);
        UNUSED(name);
        con->Cls();

    }));

    d_console_cmd_handler->AddCommand("Main.FloatUniform", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(name);
        const std::vector < std::string >& args = *arg_list;
        float val_f = std::stof(args[2]);
        sc->debugGetMainStageShader()->SetUniform(args[1],val_f);

    }));
    d_console_cmd_handler->AddCommand("Main.IntegerUniform", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(name);
        const std::vector < std::string >& args = *arg_list;
        int val_f = std::stoi(args[2]);
        sc->debugGetMainStageShader()->SetUniform(args[1],val_f);

    }));
       d_console_cmd_handler->AddCommand("Sky.FloatUniform", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(name);
        const std::vector < std::string >& args = *arg_list;
        float val_f = std::stof(args[2]);
        sc->w_sky->GetSkyShader()->SetUniform(args[1],val_f);

    }));

    d_console_cmd_handler->AddCommand("RegenerateEnvCubemap", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(name);
        UNUSED(arg_list);
        sc->regenerateEnvCubeMap();

    }));
   d_console_cmd_handler->AddCommand("DumpCamera", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(name);
        UNUSED(arg_list);
         std::ostringstream os;
       {
            /*use raii */
            cereal::JSONOutputArchive archive( os);
            archive( CEREAL_NVP( sc->cam));
        }
        con->Msg(os.str());

     }));
    d_console_cmd_handler->AddCommand("DumpModel", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
       UNUSED(arg_list);
       UNUSED(name);

         std::ofstream os("model.json");
        {
            /*use raii */
            cereal::JSONOutputArchive archive( os);
            //TODO
            archive( CEREAL_NVP( sc->d_render_list[0] ));
        }

    }));
    d_console_cmd_handler->AddCommand("MemoryStats", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(arg_list);
        UNUSED(name);
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

    }));




        d_console_cmd_handler->AddCommand("ResetShadowMap", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
            UNUSED(arg_list);
            UNUSED(name);
        sc->d_shadowmap_cam[0].LookAt(vec4( 0.0,0.0, 0.0,1.0),vec4(0.0,4000,0.0,1.0) ,   vec4(1.0,0.0,0.0,1.0) );

    }));
        d_console_cmd_handler->AddCommand("DumpShadowMap", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
            UNUSED(arg_list);
            UNUSED(name);

       std::ostringstream os;
        {
            /*use raii */
            cereal::JSONOutputArchive archive( os);
            archive( CEREAL_NVP( sc->d_shadowmap_cam[0]));
        }
        con->Msg(os.str());
    }));

    d_console_cmd_handler->AddCommand("DBGUI.FoggleFPS", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(arg_list);
        UNUSED(name);
        d_toggle_fps_view = !d_toggle_fps_view;

    }));
    d_console_cmd_handler->AddCommand("MsaaEnable", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(arg_list);
        UNUSED(name);
        sc->toggleMSAA(true);

    }));
    d_console_cmd_handler->AddCommand("MsaaDisable", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(arg_list);
        UNUSED(name);
        sc->toggleMSAA(false);

    }));
    d_console_cmd_handler->AddCommand("BrightpassEnable", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(arg_list);
        UNUSED(name);
        sc->toggleBrightPass(true);

    }));
    d_console_cmd_handler->AddCommand("BrightpassDisable", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(arg_list);
        UNUSED(name);
        sc->toggleBrightPass(false);

    }));
    d_console_cmd_handler->AddCommand("Recorder.Start", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(arg_list);
        UNUSED(name);
        sc->rec.Begin();

    }));
    d_console_cmd_handler->AddCommand("Recorder.Finish", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(arg_list);
        UNUSED(name);
        sc->rec.End();

    }));
    d_console_cmd_handler->AddCommand("Recorder.Play", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(arg_list);
        UNUSED(name);
        sc->rec.End();
        sc->d_play = true;

    }));
    d_console_cmd_handler->AddCommand("Recorder.Save", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(name);
        const std::vector < std::string >& args = *arg_list;
        sc->rec.Save(args[1]);
    }));

    d_console_cmd_handler->AddCommand("Actor.Move", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(name);
        const std::vector < std::string >& args = *arg_list;
        vec4 vect(args[1]);
        sc->cam.goPosition(vect);
        sc->cam.rotEuler(vec4(0,0,0,0));
    }));
    d_console_cmd_handler->AddCommand("Model.Scale", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(name);
        const std::vector < std::string >& args = *arg_list;
        float val_f = std::stof(args[1]);
        std::shared_ptr<SObjModel> m = sc->d_render_list[0];
        m->SetModelMat(SMat4x4().Scale(val_f));
    }));

    d_console_cmd_handler->AddCommand("Actor.RotateX", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(name);
        const std::vector < std::string >& args = *arg_list;
        float val_f = std::stof(args[1]);
        sc->cam.goPosition(vec4(0.0,0.0,0.0,0.0));
        sc->cam.rotEulerX(toRad(val_f));
    }));
    d_console_cmd_handler->AddCommand("Actor.RotateY", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(name);
        const std::vector < std::string >& args = *arg_list;
        float val_f = std::stof(args[1]);
        sc->cam.goPosition(vec4(0.0,0.0,0.0,0.0));
        sc->cam.rotEulerY(toRad(val_f));
    }));
    d_console_cmd_handler->AddCommand("Actor.RotateZ", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(name);
        const std::vector < std::string >& args = *arg_list;
        float val_f = std::stof(args[1]);
        sc->cam.goPosition(vec4(0.0,0.0,0.0,0.0));
        sc->cam.rotEulerZ(toRad(val_f));
    }));

    d_console_cmd_handler->AddCommand("Weather.SetTime", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(name);
        const std::vector < std::string >& args = *arg_list;
        float val_f = std::stof(args[1]);
        sc->w_sky->SetTime(val_f);

    }));

    d_console_cmd_handler->AddCommand("DBG.DumpFrustrum", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(name);
        UNUSED(arg_list);
        sc->d_debugDrawMgr.AddCameraFrustrum(sc->cam.getViewProjectMatrix());
        sc->d_debugDrawMgr.Update();

    }));
    d_console_cmd_handler->AddCommand("DBG.DumpShadowmapCascades", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(name);
        UNUSED(arg_list);
        for (int i = 0 ; i < 4 ; i++)
            sc->d_debugDrawMgr.AddCameraFrustrum(sc->d_shadowmap_cam[i].getViewProjectMatrix());

        sc->d_debugDrawMgr.Update();

    }));

    d_console_cmd_handler->AddCommand("DBG.DumpCameraAABB", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(name);
        UNUSED(arg_list);

        //sc->d_debugDrawMgr.AddAABB(FrustrumSize2(sc->cam.getViewProjectMatrix()));
        //sc->d_debugDrawMgr.Update();

        for (int i = 0 ; i < 4; i++)
            sc->d_debugDrawMgr.AddAABB(sc->cameraFrustrumAABB[i] );
        sc->d_debugDrawMgr.Update();

    }));
    d_console_cmd_handler->AddCommand("DBG.DumpShadowmapCascadesAABB", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(name);
        UNUSED(arg_list);
        for (int i = 0 ; i < 4; i++)
            sc->d_debugDrawMgr.AddAABB(sc->cameraTransformFrustrumAABB[i] );
        sc->d_debugDrawMgr.Update();

    }));

    d_console_cmd_handler->AddCommand("DBG.MarkActorPosition", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(arg_list);
        UNUSED(name);
        SMat4x4 m = sc->cam.getViewMatrix();
        sc->d_debugDrawMgr.AddCross(m.ExtractPositionNoScale(),1000);
        sc->d_debugDrawMgr.Update();
    }));

    d_console_cmd_handler->AddCommand("Model.Load", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(name);
        const std::vector < std::string >& args = *arg_list;
        con->Msg("Load new model..");
        std::shared_ptr<SObjModel> m =  std::shared_ptr<SObjModel>(new SObjModel(args[1]));
        m->ConfigureProgram(*sc->lookupStageShader("Main"));
        m->ConfigureProgram(*sc->lookupStageShader("MainMSAA"));
        m->ConfigureProgram(*sc->lookupStageShader("Shadowmap"));
        m->ConfigureProgram( *sc->cubemap_prog_generator);
        sc->d_render_list[0] = m;
        sc->regenerateEnvCubeMap();

    }));
    d_console_cmd_handler->AddCommand("Model.Add", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(name);
        const std::vector < std::string >& args = *arg_list;
        con->Msg("Load new model..");
        std::shared_ptr<SObjModel> m =  std::shared_ptr<SObjModel>(new SObjModel(args[1]));

        sc->AddObjectToRender(m);
        sc->regenerateEnvCubeMap();

    }));
    /*
    d_console_cmd_handler->AddCommand("script", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(arg_list);
        UNUSED(name);
        const std::vector < std::string >& args = *arg_list;
        con->Msg("Load scripst..");
        //state.Load(args[1]);
        //state["init"]();

    }));
    */
    return ESUCCESS;
}




int DebugUI::DrawGUI()
{
    SShader *mp = sc->debugGetMainStageShader();
    /*imgui new debug interface */
    static bool config_wnd = true;
    ImGui::Begin("MProject config",&config_wnd, ImGuiWindowFlags_AlwaysAutoResize);
    /*Graphics flags*/
    static bool cfg_brightpass = false;
    static bool cfg_msaa = true;
    static bool cfg_ssao = true;
    if (ImGui::Checkbox("Blum", &cfg_brightpass)) {
        sc->toggleBrightPass(cfg_brightpass);
    }
    ImGui::SameLine(150);
    if (ImGui::Checkbox("MSAA", &cfg_msaa)) {
        sc->toggleMSAA(cfg_msaa);
    }
    ImGui::SameLine(250);
    if (ImGui::Checkbox("SSAO", &cfg_ssao)) {
        sc->toggleSSAO(cfg_ssao);
    }

    /*Base post-processing configuration*/
    static float tmc_A =  0.22f;
    static float tmc_B = 0.30f;
    static float tmc_C = 0.10f;
    static float tmc_D = 0.20f;
    static float tmc_E = 0.01f;
    static float tmc_F = 0.3f;
    static float tmc_LW = 1.2f;
    static float eyeAdoptSpeed = 0.008f;
    if (ImGui::CollapsingHeader("Tonemapping"))
    {
        ImGui::Text("Fimlic curve");
        if (ImGui::SliderFloat("A", &tmc_A, 0.0f, 1.0f)) {
            sc->debugSetStageShaderUniform("Tonemap","A",tmc_A);
        }
        if (ImGui::SliderFloat("B", &tmc_B, 0.0f, 1.0f)) {
            sc->debugSetStageShaderUniform("Tonemap","B",tmc_B);
        }
        if (ImGui::SliderFloat("C", &tmc_C, 0.0f, 1.0f)) {
            sc->debugSetStageShaderUniform("Tonemap","C",tmc_C);
        }
        if (ImGui::SliderFloat("D", &tmc_D, 0.0f, 1.0f)) {
            sc->debugSetStageShaderUniform("Tonemap","D",tmc_D);
        }
        if (ImGui::SliderFloat("E", &tmc_E, 0.0f, 1.0f)) {
            sc->debugSetStageShaderUniform("Tonemap","E",tmc_E);
        }
        if (ImGui::SliderFloat("F", &tmc_F, 0.0f, 1.0f)) {
            sc->debugSetStageShaderUniform("Tonemap","F",tmc_F);
        }
        if (ImGui::SliderFloat("LW", &tmc_LW, 0.0f, 3.0f)) {
            sc->debugSetStageShaderUniform("Tonemap","LW",tmc_LW);
        }
        ImGui::Text("Eye adaptation");
        if (ImGui::SliderFloat("eyeAdoptSpeed", &eyeAdoptSpeed, 0.0f, 0.5f)) {
            sc->pp_stage_hdr_lum_key->getShader()->SetUniform("eyeAdoptSpeed",eyeAdoptSpeed);
        }
        if (ImGui::Button("Reset Tonemapping"))
            {
                tmc_A = 0.22f;
                tmc_B = 0.30f;
                tmc_C = 0.10f;
                tmc_D = 0.20f;
                tmc_E = 0.01f;
                tmc_F = 0.30f;
                tmc_LW = 1.2f;
                eyeAdoptSpeed = 0.008f;
                sc->debugSetStageShaderUniform("Tonemap","A",tmc_A);
                sc->debugSetStageShaderUniform("Tonemap","B",tmc_B);
                sc->debugSetStageShaderUniform("Tonemap","C",tmc_C);
                sc->debugSetStageShaderUniform("Tonemap","D",tmc_D);
                sc->debugSetStageShaderUniform("Tonemap","E",tmc_E);
                sc->debugSetStageShaderUniform("Tonemap","F",tmc_F);
                sc->debugSetStageShaderUniform("Tonemap","LW",tmc_LW);
                sc->pp_stage_hdr_lum_key->getShader()->SetUniform("eyeAdoptSpeed",eyeAdoptSpeed);
            }

    }
    static float ssao_Size = 0.3f;
    static float ssaoLevelClamp = 0.21f;
    static float ssaoblurSize = 0.021f;
    if (ImGui::CollapsingHeader("SSAO Config")) {
        if (ImGui::SliderFloat("ssaoSize", &ssao_Size, 0.0f, 2.0f)) {
            sc->pp_stage_ssao->getShader()->SetUniform("ssaoSize",ssao_Size);
        }
        if (ImGui::SliderFloat("ssaoLevelClamp", &ssaoLevelClamp, 0.0f, 1.0f)) {
            sc->pp_stage_ssao->getShader()->SetUniform("ssaoLevelClamp",ssaoLevelClamp);
        }
        if (ImGui::SliderFloat("blurSize", &ssaoblurSize, 0.0f, 0.2f)) {
            sc->pp_stage_ssao_blur_hor->getShader()->SetUniform("blurSize",ssaoblurSize);
            sc->pp_stage_ssao_blur_vert->getShader()->SetUniform("blurSize",ssaoblurSize);
        }
        if (ImGui::Button("Reset SSAO"))
        {
            ssao_Size = 0.3f;
            ssaoLevelClamp = 0.21f;
            ssaoblurSize = 0.021f;
            sc->pp_stage_ssao->getShader()->SetUniform("ssaoSize",ssao_Size);
            sc->pp_stage_ssao->getShader()->SetUniform("ssaoLevelClamp",ssaoLevelClamp);

            sc->pp_stage_ssao_blur_hor->getShader()->SetUniform("blurSize",ssaoblurSize);
            sc->pp_stage_ssao_blur_vert->getShader()->SetUniform("blurSize",ssaoblurSize);
         }

    }
    static float bloomClamp = 0.75;
    static float bloomMul = 2.0;
    if (ImGui::CollapsingHeader("Bloom")) {
        if (ImGui::SliderFloat("hdrBloomClamp", &bloomClamp, 0.0f, 1.2f)) {
            sc->pp_stage_hdr_bloom->getShader()->SetUniform("hdrBloomClamp",bloomClamp);
        }
        if (ImGui::SliderFloat("hdrBloomMul", &bloomMul, 0.0f, 5.0f)) {
            sc->pp_stage_hdr_bloom->getShader()->SetUniform("hdrBloomMul",bloomMul);
        }
        if (ImGui::Button("Reset Bloom"))
        {
            bloomClamp = 0.75;
            bloomMul = 2.0;
            sc->pp_stage_hdr_bloom->getShader()->SetUniform("hdrBloomClamp",bloomClamp);
            sc->pp_stage_hdr_bloom->getShader()->SetUniform("hdrBloomMul",bloomMul);
         }
    }
    static float shadowBias = 10.0;
    static float shadowEpsMult = 1.0;
    static float shadowPenumbra = 1500.0;
    if (ImGui::CollapsingHeader("Shadows"))
    {
        ImGui::Text("Bias");
        if (ImGui::SliderFloat("Bias", &shadowBias, 0.0f,25.0f)) {
            mp->SetUniform("shadowEps", 0.0001f*shadowBias);
        }
        if (ImGui::SliderFloat("Penumbra", &shadowPenumbra, 0.0f,2200.0f)) {
           mp->SetUniform("shadowPenumbra", shadowPenumbra);
        }
        if (ImGui::SliderFloat("shadowEpsMult", &shadowEpsMult,0.0f,25.0f)) {
            mp->SetUniform("shadowEpsMult", 0.0001f*shadowEpsMult);
        }
        if (ImGui::Button("Reset Shadows"))
        {
            shadowBias = 10.0;
            shadowPenumbra = 1500.0;
            shadowEpsMult = 1.0;
            mp->SetUniform("shadowEps",0.0001f*shadowBias);
            mp->SetUniform("shadowEpsMult",0.0001f*shadowEpsMult);
            mp->SetUniform("shadowPenumbra",shadowPenumbra);
        }
    }
    static float WeatherLocalTime = 1.0;
    static float WeatherSkyTurbidity = 2.0;
    static float WearherSunSize = 100.0;
    if (ImGui::CollapsingHeader("Weather")) {
        if (ImGui::SliderFloat("LocalTime", &WeatherLocalTime, 0.0f,25.0f)) {
            sc->w_sky->SetTime(WeatherLocalTime);
            sc->regenerateEnvCubeMap();
        }
        if (ImGui::SliderFloat("SkyTurbidity", &WeatherSkyTurbidity, 0.0f,25.0f)) {
            sc->w_sky->SetTurbidity(WeatherSkyTurbidity);
            sc->regenerateEnvCubeMap();
        }
        if (ImGui::SliderFloat("SunSize", &WearherSunSize, 0.0f,1000.0f)) {
            sc->w_sky->SetSunSize(WearherSunSize);
            sc->regenerateEnvCubeMap();
        }
        if (ImGui::Button("Reset Weather"))
        {
            sc->regenerateEnvCubeMap();
            WeatherLocalTime = 1.0;
            WeatherSkyTurbidity = 2.0;
            WearherSunSize = 100.0;
            sc->w_sky->SetTime(WeatherLocalTime);
            sc->w_sky->SetTurbidity(WeatherSkyTurbidity);
            sc->w_sky->SetSunSize(WearherSunSize);
        }
    }


    static float shIntensity = 5.2f;
    static float materialBRDFAlpha = 0.4f;
    static float materialBRDFressnel = 0.04f;
    static float lightIntensity = 1.0f;

    if (ImGui::CollapsingHeader("Lighting")) {
        if (ImGui::SliderFloat("shIntensity", &shIntensity, 0.0f, 20.2f)) {
            mp->SetUniform("shIntensity",shIntensity);
        }
        if (ImGui::SliderFloat("materialBRDFAlpha", &materialBRDFAlpha, 0.0f, 1.0f)) {
            mp->SetUniform("materialBRDFAlpha",materialBRDFAlpha);
        }
        if (ImGui::SliderFloat("materialBRDFressnel", &materialBRDFressnel, 0.0f, 1.0f)) {
            mp->SetUniform("materialBRDFressnel",materialBRDFressnel);
        }
        if (ImGui::SliderFloat("lightIntensity", &lightIntensity, 0.0f, 100.0f)) {
            mp->SetUniform("lightIntensity",lightIntensity);
        }


        if (ImGui::Button("Reset Lighting"))
        {
            shIntensity = 5.2f;
            materialBRDFAlpha = 0.4f;
            materialBRDFressnel = 0.04f;
            lightIntensity = 1.0f;
            mp->SetUniform("shIntensity",shIntensity);
            mp->SetUniform("materialBRDFAlpha",materialBRDFAlpha);
            mp->SetUniform("materialBRDFressnel",materialBRDFressnel);
            mp->SetUniform("lightIntensity",lightIntensity);


         }
    }


    /*Sample code*/
    static bool gImGuiTest = false;
    ImGui::Checkbox("GuiTest", &gImGuiTest);
    if (gImGuiTest) {
        ImGui::ShowTestWindow(&gImGuiTest);
    }
    static bool s_guiWndIs = false;
    ImGui::Checkbox("Introspection",&s_guiWndIs);
    if (s_guiWndIs) {
        DrawIntrospectionGUI(&s_guiWndIs);
    }
    /*Console */
    static bool s_guiConsleIs = false;
	if (s_guiConsleIs || gImGuiTest) {
		g_InputState = GlobalInputState::DEBUG_CONTROL;
	}
	else
	{
		g_InputState = GlobalInputState::ACTOR_CONTROL;
	}
    ImGui::Checkbox("Debug Console",&s_guiConsleIs);
    if (s_guiConsleIs) {
        DrawConsoleUI(&s_guiConsleIs);
    }

    ImGui::End();
    ImGui::Render();
    return ESUCCESS;
}

int DebugUI::DrawIntrospectionGUI(bool *opened)
{
    ImGui::Begin("Introspection",opened, ImGuiWindowFlags_AlwaysAutoResize);

    if (ImGui::TreeNode("Shaders"))
    {
        auto sh = SShader::getShaderSharedList();
        for (auto &s : sh) {
         if (ImGui::TreeNode((s->GetVertexShaderFileName() + std::string("  ")+ s->GetFragmentShaderFileName()).c_str())) {
            //ImGui::TreeNode("Source");
            //ImGui::TreeNode("Uniforms");
            ImGui::TreePop();
         }
        }


        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Pipeline"))
    {
        ImGui::Text("TODO");
        ImGui::TreePop();
    }
    static int rbSelect = 0;
    if (ImGui::TreeNode("Render Buffers"))
    {

        ImGui::RadioButton("Normal output", &rbSelect, 0);
        auto rlist = RBO::debugGetRenderOutputList();
        int it = 0;
        for (auto &r : rlist) {
            it++;
            ImGui::RadioButton(r->getName().c_str(), &rbSelect, it);
            if (it == rbSelect) {
            sc->debugSetFinalRenderOutput(r);
            }
        }
        sc->debugSetDebugRenderOutputFlag(!!rbSelect);

        ImGui::TreePop();
    }
    ImGui::End();
    return ESUCCESS;

}

int DebugUI::DrawConsoleUI(bool *opened) {
    static bool ScrollToBottom =  false;
    ImGui::SetNextWindowSize(ImVec2(520,600), ImGuiSetCond_FirstUseEver);
    if (!ImGui::Begin("Debug Console", opened))
        {
            ImGui::End();
            return 0;
        }

        ImGui::TextWrapped("Basic debug console.");

        if (ImGui::SmallButton("Clear")) con->Cls();
        
        ImGui::Separator();

        
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0,0));
        static ImGuiTextFilter filter;
        filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
        ImGui::PopStyleVar();
        ImGui::Separator();
        

       ImGui::BeginChild("ScrollingRegion", ImVec2(0,-ImGui::GetItemsLineHeightWithSpacing()));
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::Selectable("Clear")) con->Cls();
            ImGui::EndPopup();
        }
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4,1)); // Tighten spacing
        const std::vector<std::string> consoleData = *(con->getConsoleData());
        for (unsigned int i = 0; i < consoleData.size() - 1; i++)
        {
            const char* item = consoleData[i].c_str();
            if (!filter.PassFilter(item))
                continue;
            ImVec4 col = ImColor(255,255,255);
            if (strstr(item, "[ERR]")) col = ImColor(255,100,100);
            else if (strstr(item, "[VERB]")) col = ImColor(100,255,100);
            else if (strstr(item, "[WARN]")) col = ImColor(160,100,100);
            ImGui::PushStyleColor(ImGuiCol_Text, col);
            ImGui::TextWrapped(item);
            ImGui::PopStyleColor();
        }
		ImGui::SetScrollHere();
        ImGui::PopStyleVar();
        ImGui::EndChild();
        ImGui::Separator();
		// Command-line
		char InputBuf[1024];
		
     
        if (ImGui::InputText("Input", InputBuf, 1024, ImGuiInputTextFlags_EnterReturnsTrue))
        {
			con->HandleCommand(InputBuf);
			strcpy(InputBuf, "");
        }

        if (ImGui::IsItemHovered() || (ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)))
            ImGui::SetKeyboardFocusHere(-1); // Auto focus

        ImGui::End();
        return 0;
}