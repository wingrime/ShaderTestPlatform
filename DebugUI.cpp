#include "DebugUI.h"
#include "Scene.h"
#include "UILabel.h"
#include "UIConsole.h"
#include "r_sprog.h"
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
        sc->d_shadowmap_cam = sc->cam;
        sc->d_first_render = true;

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
        sc->d_shadowmap_cam.LookAt(SVec4( 0.0,0.0, 0.0,1.0),SVec4(0.0,4000,0.0,1.0) ,   SVec4(1.0,0.0,0.0,1.0) );

    }));
        d_console_cmd_handler->AddCommand("dump_smcam", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;

         std::ostringstream os;
        {
            /*use raii */
            cereal::JSONOutputArchive archive( os);
            archive( CEREAL_NVP( sc->d_shadowmap_cam));
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
        sc->d_debugDrawMgr.AddCross({vect.x,vect.y,vect.z},20);

    }));


    d_console_cmd_handler->AddCommand("rec", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;
        sc->d_first_render = false;
    }));

    d_console_cmd_handler->AddCommand("updc", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;
        SMat4x4 m = sc->cam.getViewMatrix();
        sc->d_debugDrawMgr.AddCross({m.a14,m.a24,m.a34},1000);
        sc->d_debugDrawMgr.Update();
    }));

    d_console_cmd_handler->AddCommand("load", ConsoleCommandHandler::StrCommand([=] (const std::string& name, std::vector < std::string > * arg_list ) -> void {
        const std::vector < std::string >& args = *arg_list;
        con->Msg("Load new model..");
        sc->d_render_list[0].reset(new SObjModel(args[1]));
        sc->d_render_list[0]->ConfigureProgram( *(sc->r_prog));
        sc->d_render_list[0]->ConfigureProgram( *(sc->cam_prog));
        sc->d_render_list[0]->ConfigureProgram( *(sc->cubemap_prog_generator));
        sc->d_first_render = true;

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
