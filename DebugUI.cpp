#include "DebugUI.h"
#include "Scene.h"
#include "UILabel.h"
#include "UIConsole.h"
#include "r_sprog.h"
#include <imgui.h>
DebugUI::DebugUI(SScene *_s, RBO *_v)
    :v(_v)
    ,con(new UIConsole(v,  d_console_cmd_handler ))
    ,err_con(new UIConsoleErrorHandler(con))
    ,d_console_cmd_handler(new ConsoleCommandHandler())
    ,fps_label(new UILabel(v,0.85,0.1))
    ,cfg_label(new UILabel(v,0.0, 0.6))
    ,v_sel_label(new UILabel(v,0.75, 0.6))
    ,sc(_s)
{
    /*Setup error handler*/
    MainLog::GetInstance()->SetCallback([=](Log::Verbosity v, const std::string &s)-> void { con->Msg(s); });
    InitDebugCommands();
}

int DebugUI::Draw()
{
    if (d_toggle_fps_view)
        fps_label->Draw();
    if (d_toggle_cfg_view) {
        cfg_label->Draw();
        v_sel_label->Draw();
    }
    con->Draw();

}
AABB FrustrumSize2(const SMat4x4& r) {

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
int DebugUI::InitDebugCommands()
{
    d_console_cmd_handler->AddCommand("cls", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        con->Cls();
    }));

    d_console_cmd_handler->AddCommand("r_set_f", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;
        float val_f = std::stof(args[2]);
        sc->r_prog->SetUniform(args[1],val_f);

    }));
    d_console_cmd_handler->AddCommand("r_set_i", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;
        int val_f = std::stoi(args[2]);
        sc->r_prog->SetUniform(args[1],val_f);

    }));
       d_console_cmd_handler->AddCommand("sky_set_f", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;
        float val_f = std::stof(args[2]);
        sc->sky_dome_prog->SetUniform(args[1],val_f);

    }));

    d_console_cmd_handler->AddCommand("sm_cam_set", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;
        sc->d_shadowmap_cam0 = sc->cam;
        sc->d_shadowmap_cam1 = sc->cam;
        sc->d_shadowmap_cam2 = sc->cam;
        sc->d_shadowmap_cam3 = sc->cam;
        sc->d_first_render = false;

    }));
    d_console_cmd_handler->AddCommand("dump_cam", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;

         std::ostringstream os;
        {
            /*use raii */
            cereal::JSONOutputArchive archive( os);
            archive( CEREAL_NVP( sc->cam));
        }
        con->Msg(os.str());

    }));
    d_console_cmd_handler->AddCommand("dump_model", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;

         std::ofstream os("model.json");
        {
            /*use raii */
            cereal::JSONOutputArchive archive( os);
            //TODO
            //archive( CEREAL_NVP( sc->model));
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
        sc->d_shadowmap_cam0.LookAt(SVec4( 0.0,0.0, 0.0,1.0),SVec4(0.0,4000,0.0,1.0) ,   SVec4(1.0,0.0,0.0,1.0) );

    }));
        d_console_cmd_handler->AddCommand("dump_smcam", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;

         std::ostringstream os;
        {
            /*use raii */
            cereal::JSONOutputArchive archive( os);
            archive( CEREAL_NVP( sc->d_shadowmap_cam0));
        }
        con->Msg(os.str());
    }));

    d_console_cmd_handler->AddCommand("toggle_ui", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        d_toggle_fps_view = !d_toggle_fps_view;
        d_toggle_cfg_view = !d_toggle_cfg_view;

    }));
    d_console_cmd_handler->AddCommand("toggle_msaa", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        sc->d_toggle_MSAA= !sc->d_toggle_MSAA;

    }));
    d_console_cmd_handler->AddCommand("toggle_brightpass", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        sc->d_toggle_brightpass= !sc->d_toggle_brightpass;

    }));

    d_console_cmd_handler->AddCommand("goto", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;
        SVec4 vect(args[1]);
        sc->cam.goPosition(vect);
        sc->cam.rotEuler(SVec4(0,0,0,0));
    }));

    d_console_cmd_handler->AddCommand("rot_x", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;
        float val_f = std::stof(args[1]);
        sc->cam.goPosition(SVec4(0.0,0.0,0.0,0.0));
        sc->cam.rotEulerX(toRad(val_f));
    }));
    d_console_cmd_handler->AddCommand("rot_y", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;
        float val_f = std::stof(args[1]);
        sc->cam.goPosition(SVec4(0.0,0.0,0.0,0.0));
        sc->cam.rotEulerY(toRad(val_f));
    }));
    d_console_cmd_handler->AddCommand("rot_z", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;
        float val_f = std::stof(args[1]);
        sc->cam.goPosition(SVec4(0.0,0.0,0.0,0.0));
        sc->cam.rotEulerZ(toRad(val_f));
    }));

    d_console_cmd_handler->AddCommand("li", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;
        SVec4 vect(args[1]);
        sc->r_prog->SetUniform("main_light_dir", vect);
        sc->d_prim_light_dir = vect;

    }));

    d_console_cmd_handler->AddCommand("vis_frustrum", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;
        sc->d_debugDrawMgr.AddCameraFrustrum(sc->cam.getViewProjectMatrix());
        sc->d_debugDrawMgr.Update();

    }));
    d_console_cmd_handler->AddCommand("vis_cascades", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;
        sc->d_debugDrawMgr.AddCameraFrustrum(sc->d_shadowmap_cam0.getViewProjectMatrix());
        sc->d_debugDrawMgr.AddCameraFrustrum(sc->d_shadowmap_cam1.getViewProjectMatrix());
        sc->d_debugDrawMgr.AddCameraFrustrum(sc->d_shadowmap_cam2.getViewProjectMatrix());
        sc->d_debugDrawMgr.AddCameraFrustrum(sc->d_shadowmap_cam3.getViewProjectMatrix());

        sc->d_debugDrawMgr.Update();

    }));

    d_console_cmd_handler->AddCommand("vis_camaabb", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;
        sc->d_debugDrawMgr.AddAABB(FrustrumSize2(sc->cam.getViewProjectMatrix()));
        sc->d_debugDrawMgr.Update();

    }));


    d_console_cmd_handler->AddCommand("rec", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;
        sc->d_first_render = false;
    }));

    d_console_cmd_handler->AddCommand("updc", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;
        SMat4x4 m = sc->cam.getViewMatrix();
        sc->d_debugDrawMgr.AddCross(m.ExtractPositionNoScale(),1000);
        sc->d_debugDrawMgr.Update();
    }));

    d_console_cmd_handler->AddCommand("load", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;
        con->Msg("Load new model..");
        sc->d_render_list[0].reset(new SObjModel(args[1]));
        sc->d_render_list[0]->ConfigureProgram( *(sc->r_prog));
        sc->d_render_list[0]->ConfigureProgram( *(sc->cam_prog));
        sc->d_render_list[0]->ConfigureProgram( *(sc->cubemap_prog_generator));
        sc->d_first_render = false;

    }));
    d_console_cmd_handler->AddCommand("script", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;
        //con->Msg("Load scripst..");
        //state.Load(args[1]);
        //state["init"]();

    }));

}

int DebugUI::upCfgItem() {
    if (d_cfg_current > 0)
        d_cfg_current --;
    return 0;
}
int DebugUI::downCfgItem() {
    if (d_cfg_current < d_cfg_max )
        d_cfg_current++;
    return 0;
}
int DebugUI::incCfgItem() {
    d_cfg[d_cfg_current] +=d_cfg_step;
    return 0;
}

int DebugUI::decCfgItem() {
    d_cfg[d_cfg_current] -=d_cfg_step;
    return 0;
}

const inline std::string DebugUI::C_I(int n) {
    const std::string s("[>]");
    const std::string ns("[ ]");
    return (n==d_cfg_current?s:ns);
}
const inline std::string DebugUI::V_I(int n) {

    const std::string s("[+]");
    const std::string ns("[ ]");
    return (n==d_v_sel_current?s:ns);
}


int DebugUI::upViewItem() {
    if (d_v_sel_current > 0)
        d_v_sel_current --;
    UpdateViewSelLabel();
    return 0;
}
int DebugUI::downViewItem() {
    if (d_v_sel_current < d_v_sel_max )
        d_v_sel_current++;
    UpdateViewSelLabel();
    return 0;
}

int DebugUI::DrawGUI()
{
    /*imgui new debug interface */
    bool test;
    static bool config_wnd = true;
    ImGui::Begin("MProject config",&config_wnd, ImGuiWindowFlags_AlwaysAutoResize);
    /*Graphics flags*/
    static bool cfg_brightpass = false;
    static bool cfg_msaa = true;
    if (ImGui::Checkbox("Brightpass", &cfg_brightpass)) {
        sc->toggleBrightPass(cfg_brightpass);
    }
    ImGui::SameLine(150);
    if (ImGui::Checkbox("MSAA", &cfg_msaa)) {
        sc->toggleMSAA(cfg_msaa);
    }

    /*Base post-processing configuration*/
    static float tmc_A =  0.22;
    static float tmc_B = 0.30f;
    static float tmc_C = 0.10f;
    static float tmc_D = 0.20f;
    static float tmc_E = 0.01f;
    static float tmc_F = 0.3f;
    static float tmc_LW = 1.2f;
    if (ImGui::CollapsingHeader("Tonemapping"))
    {
        ImGui::Text("Tonemapping curve");
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

        if (ImGui::Button("Reset"))
            {
                tmc_A = 0.22;
                tmc_B = 0.30;
                tmc_C = 0.10;
                tmc_D = 0.20;
                tmc_E = 0.01;
                tmc_F = 0.30;
                tmc_LW = 1.2;
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
    if (ImGui::CollapsingHeader("SSAO")) {
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
        if (ImGui::Button("Reset"))
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
        if (ImGui::Button("Reset"))
        {
            bloomClamp = 0.75;
            bloomMul = 2.0;
            sc->pp_stage_hdr_bloom->getShader()->SetUniform("hdrBloomClamp",bloomClamp);
            sc->pp_stage_hdr_bloom->getShader()->SetUniform("hdrBloomMul",bloomMul);
         }
    }
    ImGui::End();


}



int DebugUI::UpdateViewSelLabel() {
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
int DebugUI::UpdateCfgLabel() {



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

    sc->pp_stage_ssao_blur_hor->getShader()->SetUniform("blurSize",d_cfg[0]);
    sc->pp_stage_ssao_blur_vert->getShader()->SetUniform("blurSize",d_cfg[0]);
    /*dbg*/
    sc->pp_prog_hdr_tonemap->SetUniform("aoStrength",d_cfg[0]);

    SShader * s = sc->pp_stage_ssao->getShader();
    s->SetUniform("ssaoSize",d_cfg[1]);
    s->SetUniform("ssaoLevelClamp",d_cfg[2]);
    s->SetUniform("ssaoDepthClamp",d_cfg[3]);
    sc->pp_stage_hdr_bloom->getShader()->SetUniform("hdrBloomClamp",d_cfg[4]);
    sc->pp_stage_hdr_bloom->getShader()->SetUniform("hdrBloomMul",d_cfg[5]);


    //pp_prog_hdr_blur_hor->SetUniform("blurSize",d_cfg[6]);
    //pp_prog_hdr_blur_vert->SetUniform("blurSize",d_cfg[6]);


    sc->pp_prog_hdr_tonemap->SetUniform("A",d_cfg[7]);
    sc->pp_prog_hdr_tonemap->SetUniform("B",d_cfg[8]);
    sc->pp_prog_hdr_tonemap->SetUniform("C",d_cfg[9]);
    sc->pp_prog_hdr_tonemap->SetUniform("D",d_cfg[10]);
    sc->pp_prog_hdr_tonemap->SetUniform("E",d_cfg[11]);
    sc->pp_prog_hdr_tonemap->SetUniform("F",d_cfg[12]);
    sc->pp_prog_hdr_tonemap->SetUniform("LW",float((d_cfg[13])));

    sc->r_prog->SetUniform("lightIntensity",d_cfg[14]);

    return ESUCCESS;
}
