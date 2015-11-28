#include "DebugUI.h"
#include "Scene.h"
#include "UILabel.h"
#include "UIConsole.h"
#include "r_sprog.h"
#include <imgui.h>
#include <RBO.h>
#include "MAssert.h"
#include "IMGuiHooks.h"
DebugUI::DebugUI(RectSizeInt &_v)
    :fps_label(new UILabel(_v,0.85,0.1))
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

int DebugUI::Draw()
{
    if (d_toggle_fps_view)
        fps_label->Draw();
    con->Draw();
    DrawGUI();

    return ESUCCESS;

}

int DebugUI::Reshape(const RectSizeInt &v)
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

    d_console_cmd_handler->AddCommand("r_set_f", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(name);
        const std::vector < std::string >& args = *arg_list;
        float val_f = std::stof(args[2]);
        sc->mainRenderPassMSAA->stageShader->SetUniform(args[1],val_f);

    }));
    d_console_cmd_handler->AddCommand("r_set_i", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(name);
        const std::vector < std::string >& args = *arg_list;
        int val_f = std::stoi(args[2]);
        sc->mainRenderPassMSAA->stageShader->SetUniform(args[1],val_f);

    }));
       d_console_cmd_handler->AddCommand("sky_set_f", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(name);
        const std::vector < std::string >& args = *arg_list;
        float val_f = std::stof(args[2]);
        sc->w_sky->GetSkyShader()->SetUniform(args[1],val_f);

    }));

    d_console_cmd_handler->AddCommand("sm_cam_set", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(name);
        UNUSED(arg_list);
        for (int i = 0; i < 4 ; i++) {
            //sc->d_shadowmap_cam[i] = sc->cam;
        }
        sc->regenerateEnvCubeMap();

    }));
   d_console_cmd_handler->AddCommand("dump_cam", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
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
    d_console_cmd_handler->AddCommand("dump_model", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
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
    d_console_cmd_handler->AddCommand("mem_stats", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
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




        d_console_cmd_handler->AddCommand("sm_cam", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
            UNUSED(arg_list);
            UNUSED(name);
        sc->d_shadowmap_cam[0].LookAt(vec4( 0.0,0.0, 0.0,1.0),vec4(0.0,4000,0.0,1.0) ,   vec4(1.0,0.0,0.0,1.0) );

    }));
        d_console_cmd_handler->AddCommand("dump_smcam", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
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

    d_console_cmd_handler->AddCommand("toggle_fps", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(arg_list);
        UNUSED(name);
        d_toggle_fps_view = !d_toggle_fps_view;

    }));
    d_console_cmd_handler->AddCommand("msaa_enable", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(arg_list);
        UNUSED(name);
        sc->toggleMSAA(true);

    }));
    d_console_cmd_handler->AddCommand("msaa_disable", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(arg_list);
        UNUSED(name);
        sc->toggleMSAA(false);

    }));
    d_console_cmd_handler->AddCommand("brightpass_enable", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(arg_list);
        UNUSED(name);
        sc->toggleBrightPass(true);

    }));
    d_console_cmd_handler->AddCommand("brightpass_disable", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(arg_list);
        UNUSED(name);
        sc->toggleBrightPass(false);

    }));
    d_console_cmd_handler->AddCommand("record", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(arg_list);
        UNUSED(name);
        sc->rec.Begin();

    }));
    d_console_cmd_handler->AddCommand("end", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(arg_list);
        UNUSED(name);
        sc->rec.End();

    }));
    d_console_cmd_handler->AddCommand("play", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(arg_list);
        UNUSED(name);
        sc->rec.End();
        sc->d_play = true;

    }));
    d_console_cmd_handler->AddCommand("save_rec", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(name);
        const std::vector < std::string >& args = *arg_list;
        sc->rec.Save(args[1]);
    }));

    d_console_cmd_handler->AddCommand("goto", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(name);
        const std::vector < std::string >& args = *arg_list;
        vec4 vect(args[1]);
        sc->cam.goPosition(vect);
        sc->cam.rotEuler(vec4(0,0,0,0));
    }));
    d_console_cmd_handler->AddCommand("scale", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(name);
        const std::vector < std::string >& args = *arg_list;
        float val_f = std::stof(args[1]);
        std::shared_ptr<SObjModel> m = sc->d_render_list[0];
        m->SetModelMat(SMat4x4().Scale(val_f));
    }));

    d_console_cmd_handler->AddCommand("rot_x", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(name);
        const std::vector < std::string >& args = *arg_list;
        float val_f = std::stof(args[1]);
        sc->cam.goPosition(vec4(0.0,0.0,0.0,0.0));
        sc->cam.rotEulerX(toRad(val_f));
    }));
    d_console_cmd_handler->AddCommand("rot_y", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(name);
        const std::vector < std::string >& args = *arg_list;
        float val_f = std::stof(args[1]);
        sc->cam.goPosition(vec4(0.0,0.0,0.0,0.0));
        sc->cam.rotEulerY(toRad(val_f));
    }));
    d_console_cmd_handler->AddCommand("rot_z", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(name);
        const std::vector < std::string >& args = *arg_list;
        float val_f = std::stof(args[1]);
        sc->cam.goPosition(vec4(0.0,0.0,0.0,0.0));
        sc->cam.rotEulerZ(toRad(val_f));
    }));

    d_console_cmd_handler->AddCommand("weather_time", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(name);
        const std::vector < std::string >& args = *arg_list;
        float val_f = std::stof(args[1]);
        sc->w_sky->SetTime(val_f);

    }));

    d_console_cmd_handler->AddCommand("vis_frustrum", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(name);
        UNUSED(arg_list);
        sc->d_debugDrawMgr.AddCameraFrustrum(sc->cam.getViewProjectMatrix());
        sc->d_debugDrawMgr.Update();

    }));
    d_console_cmd_handler->AddCommand("vis_cascades", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(name);
        UNUSED(arg_list);
        for (int i = 0 ; i < 4 ; i++)
            sc->d_debugDrawMgr.AddCameraFrustrum(sc->d_shadowmap_cam[i].getViewProjectMatrix());

        sc->d_debugDrawMgr.Update();

    }));

    d_console_cmd_handler->AddCommand("vis_camaabb", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(name);
        UNUSED(arg_list);

        //sc->d_debugDrawMgr.AddAABB(FrustrumSize2(sc->cam.getViewProjectMatrix()));
        //sc->d_debugDrawMgr.Update();

        for (int i = 0 ; i < 4; i++)
            sc->d_debugDrawMgr.AddAABB(sc->cameraFrustrumAABB[i] );
        sc->d_debugDrawMgr.Update();

    }));
    d_console_cmd_handler->AddCommand("vis_cascadeaabb", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(name);
        UNUSED(arg_list);
        for (int i = 0 ; i < 4; i++)
            sc->d_debugDrawMgr.AddAABB(sc->cameraTransformFrustrumAABB[i] );
        sc->d_debugDrawMgr.Update();

    }));

    d_console_cmd_handler->AddCommand("regenerate_envmap", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(name);
        UNUSED(arg_list);
        sc->regenerateEnvCubeMap();
    }));

    d_console_cmd_handler->AddCommand("updc", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(arg_list);
        UNUSED(name);
        SMat4x4 m = sc->cam.getViewMatrix();
        sc->d_debugDrawMgr.AddCross(m.ExtractPositionNoScale(),1000);
        sc->d_debugDrawMgr.Update();
    }));

    d_console_cmd_handler->AddCommand("load", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
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
    d_console_cmd_handler->AddCommand("add", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(name);
        const std::vector < std::string >& args = *arg_list;
        con->Msg("Load new model..");
        std::shared_ptr<SObjModel> m =  std::shared_ptr<SObjModel>(new SObjModel(args[1]));

        sc->AddObjectToRender(m);
        sc->regenerateEnvCubeMap();

    }));
    d_console_cmd_handler->AddCommand("script", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        UNUSED(arg_list);
        UNUSED(name);
        const std::vector < std::string >& args = *arg_list;
        con->Msg("Load scripst..");
        //state.Load(args[1]);
        //state["init"]();

    }));
    return ESUCCESS;
}




int DebugUI::DrawGUI()
{
    SShader *mp;
    if (sc->d_toggle_MSAA)
        mp = sc->mainRenderPassMSAA->stageShader;
    else
         mp = sc->mainRenderPass->stageShader;
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
    static float tmc_A =  0.22;
    static float tmc_B = 0.30f;
    static float tmc_C = 0.10f;
    static float tmc_D = 0.20f;
    static float tmc_E = 0.01f;
    static float tmc_F = 0.3f;
    static float tmc_LW = 1.2f;
    static float eyeAdoptSpeed = 0.008;
    if (ImGui::CollapsingHeader("Tonemapping"))
    {
        ImGui::Text("Fimlic curve");
        if (ImGui::SliderFloat("A", &tmc_A, 0.0f, 1.0f)) {
            sc->pp_prog_hdr_tonemap->SetUniform("A",tmc_A);
        }
        if (ImGui::SliderFloat("B", &tmc_B, 0.0f, 1.0f)) {
            sc->pp_prog_hdr_tonemap->SetUniform("B",tmc_B);
        }
        if (ImGui::SliderFloat("C", &tmc_C, 0.0f, 1.0f)) {
            sc->pp_prog_hdr_tonemap->SetUniform("C",tmc_C);
        }
        if (ImGui::SliderFloat("D", &tmc_D, 0.0f, 1.0f)) {
            sc->pp_prog_hdr_tonemap->SetUniform("D",tmc_D);
        }
        if (ImGui::SliderFloat("E", &tmc_E, 0.0f, 1.0f)) {
            sc->pp_prog_hdr_tonemap->SetUniform("E",tmc_E);
        }
        if (ImGui::SliderFloat("F", &tmc_F, 0.0f, 1.0f)) {
            sc->pp_prog_hdr_tonemap->SetUniform("F",tmc_F);
        }
        if (ImGui::SliderFloat("LW", &tmc_LW, 0.0f, 3.0f)) {
            sc->pp_prog_hdr_tonemap->SetUniform("LW",tmc_LW);
        }
        ImGui::Text("Eye adaptation");
        if (ImGui::SliderFloat("eyeAdoptSpeed", &eyeAdoptSpeed, 0.0f, 0.5f)) {
            sc->pp_stage_hdr_lum_key->getShader()->SetUniform("eyeAdoptSpeed",eyeAdoptSpeed);
        }
        if (ImGui::Button("Reset Tonemapping"))
            {
                tmc_A = 0.22;
                tmc_B = 0.30;
                tmc_C = 0.10;
                tmc_D = 0.20;
                tmc_E = 0.01;
                tmc_F = 0.30;
                tmc_LW = 1.2;
                eyeAdoptSpeed = 0.008;
                sc->pp_prog_hdr_tonemap->SetUniform("A",tmc_A);
                sc->pp_prog_hdr_tonemap->SetUniform("B",tmc_B);
                sc->pp_prog_hdr_tonemap->SetUniform("C",tmc_C);
                sc->pp_prog_hdr_tonemap->SetUniform("D",tmc_D);
                sc->pp_prog_hdr_tonemap->SetUniform("E",tmc_E);
                sc->pp_prog_hdr_tonemap->SetUniform("F",tmc_F);
                sc->pp_prog_hdr_tonemap->SetUniform("LW",tmc_LW);
            }

    }
    static float ssao_Size = 0.3;
    static float ssaoLevelClamp = 0.21;
    static float ssaoblurSize = 0.021;
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
            ssao_Size = 0.3;
            ssaoLevelClamp = 0.21;
            ssaoblurSize = 0.021;
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


    static float shIntensity = 5.2;
    static float materialBRDFAlpha = 0.4;
    static float materialBRDFressnel = 0.04;
    static float lightIntensity = 1.0;

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
            shIntensity = 5.2;
            materialBRDFAlpha = 0.4;
            materialBRDFressnel = 0.04;
            lightIntensity = 1.0;
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

