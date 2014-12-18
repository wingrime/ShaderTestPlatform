/*
Global TODOs:
- cubemaps
- more accurate rendering path control for more flexability
- online attribute control
- skybox
*/
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/wglew.h>
#include    <stdio.h>

#include    <string.h>
#include <math.h>
/* STL */
#include <vector>
/*filebuffer interface*/
#include "c_filebuffer.h"
/*hand made matrix math*/
#include "mat_math.h"

/*hand made front render based on ft2*/
#include "r_uiconsole.h"
#include "r_uilabel.h"

/* shader class*/
#include "r_sprog.h"

#include "ObjModel.h"
#include "viewport.h"
#include "sky_model.h"
#include "post_process.h"

/*GL performance measurement */
#include "r_perfman.h"

/*GL compute shader*/
#include "r_cprog.h"

#include "r_camera.h"
#include "r_context.h"
/*lua toolkit*/
//#include <selene.h>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/json.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include "r_texture.h"
#include "r_rbo_texture.h"
#include "viewport.h"
#include <windows.h>
#include "MAssert.h"
class BaseCommand
{
public:
    virtual ~BaseCommand() {}
};

template <class... ArgTypes>
class Command : public BaseCommand
{
  typedef std::function<void(ArgTypes...)> FuncType;
  FuncType f_;
  public:
    Command() {}
    Command(FuncType f) : f_(f) {}
    void operator()(ArgTypes... args) { if(f_) f_(args...); }
};

class BaseCommandHandler :public AbstractCommandHandler {
public:
    
    
    template <class... ArgTypes> int ExecuteCommand(const std::string& cmd_n, ArgTypes... args);
    template <class T> int AddCommand(const std::string& cmd_n, const T& cmd);
private: 
    typedef std::unordered_map< std::string , std::shared_ptr< BaseCommand >  > CmdMapType;
    
     CmdMapType d_cmd_map;
};


template <class T> 
int BaseCommandHandler::AddCommand(const std::string& cmd_n, const T& cmd) {
    d_cmd_map[cmd_n] = std::shared_ptr< BaseCommand > (new T(cmd));
    return 0;
}
template <class... ArgTypes> 
int BaseCommandHandler::ExecuteCommand(const std::string& cmd_n, ArgTypes... args){
    CmdMapType::const_iterator it  = d_cmd_map.find(cmd_n);
    if (it != d_cmd_map.end() ) {
        auto &cmd =   it->second;
        /*EXXXX static_cast??? or dynamic??*/
        Command<ArgTypes...> * c = static_cast<  Command < ArgTypes...>  *   >(cmd.get());
        if (c) {
            (*c)(args...);
        }// else
            //EMSGS("Invalid cmd type!\n");
    }// else
        //EMSGS("No cmd: " + cmd_n + " \n");
    return 0;
}


class ConsoleCommandHandler :public BaseCommandHandler{
public:
    typedef Command< const std::string &, std::vector < std::string > * > StrCommand;
    int HandleCommand(const std::string& str);
private:
    std::vector<std::string> & split(const std::string &s, char delim, std::vector<std::string> &elems);
};

int ConsoleCommandHandler::HandleCommand(const std::string& str) {
    /*tokenize*/
    std::vector<std::string> elems;
    split(str, ' ', elems);
    /*exec*/
    ExecuteCommand<const std::string & ,std::vector < std::string > * >(elems[0],elems[0], &elems );
    return 0; 
}
std::vector<std::string> & ConsoleCommandHandler::split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}
class InputCommandHandler :public BaseCommandHandler {
public:
    typedef Command< > InputCommand;
    int HandleCommand(const std::string& str);
    template <typename  T>
    int HandleInputKey(T key_code);
    template <typename  T>
    int BindKey(T key_code, const std::string& name );

private:
    /*vs map*/
    std::unordered_map< int , std::string > d_key_map;
};
int InputCommandHandler::HandleCommand(const std::string& str) {
    /*exec*/
    ExecuteCommand< >(str);
    return 0; 
}
template < typename  T>
int InputCommandHandler::BindKey(T key_code, const std::string& name ) {
    d_key_map[static_cast<int>(key_code)] = name;
    return 0;
}
template <typename  T>
int InputCommandHandler::HandleInputKey(T key_code) {
    const std::string & command = d_key_map [ static_cast<int>(key_code)];
    /*exec*/
    HandleCommand (command);
    return 0;
}
class SScene {
public:
    SScene(Viewport *v);
    int Render();

    int Reshape(int w, int h);

    int UpdateCfgLabel();
    int UpdateViewSelLabel();


    int upCfgItem();
    int downCfgItem();
    int incCfgItem();
    int decCfgItem();

    int upViewItem();
    int downViewItem();


    
    std::shared_ptr<SObjModel> model;
    std::shared_ptr<SObjModel> sky_dome_model;

    SCamera cam;
    SCamera sky_cam;
    SCamera d_shadowmap_cam;
    SPostProcess *pp_stage_ssao;
    SPostProcess *pp_stage_ssao_blur_hor;
    SPostProcess *pp_stage_ssao_blur_vert;

    SPostProcess *pp_stage_hdr_bloom;
    SPostProcess *pp_stage_hdr_blur_hor;
    SPostProcess *pp_stage_hdr_blur_vert;

    /*for ping pong bloor*/
    SPostProcess *pp_stage_hdr_blur_hor2; 
    SPostProcess *pp_stage_hdr_blur_vert2;

    SPostProcess *pp_stage_hdr_tonemap;

    SPostProcess *pp_stage_volumetric;
    std::shared_ptr<Viewport> rtSCREEN;
    std::shared_ptr<Viewport> rtHDRScene;
    std::shared_ptr<Viewport> rtHDRScene_MSAA;
    std::shared_ptr<Viewport> rtSSAOResult;
    std::shared_ptr<Viewport> rtShadowMap;
    std::shared_ptr<Viewport> rtSSAOBLUR2;
    std::shared_ptr<Viewport> rtHDRBloomResult;
    std::shared_ptr<Viewport> rtHDRHorBlurResult;
    std::shared_ptr<Viewport> rtHDRVertBlurResult;

    std::shared_ptr<Viewport> rtVolumetric;

    SPerfMan rtime;

    SPerfMan ui_time;
    SPerfMan pp_time;

    /* flags*/
    bool rWireframe = false;
    bool rSSAO  = false;

   // lua
    //sel::State state{true};
private:
    float step;
    
    std::shared_ptr<UILabel> fps_label;
    std::shared_ptr<UILabel> cfg_label;
    std::shared_ptr<UILabel> v_sel_label;

    int d_cfg_current = 0;
    int d_cfg_max = 14;
    /*default parameter set*/
    float  d_cfg [15] = {0.012, 0.006,0.720,0.26,1.5,0.75,0.030,0.22,0.30,0.10,0.20,0.01,0.30,11.2,1.6};
    float d_cfg_step = 0.001;


    /* main prog*/
    SShader *r_prog;
    SShader *cam_prog;


    /*Sky*/
     SShader *sky_dome_prog;

    /*SSAO*/
    SShader *pp_prog_ssao_blur_hor;
    SShader *pp_prog_ssao_blur_vert;
    SShader *pp_prog_ssao;

    SShader *pp_prog_hdr_blur_kawase;

    /*HDR stages */
    SShader *pp_prog_hdr_bloom; /*clamp step*/
    SShader *pp_prog_hdr_blur_hor; /* blur pass horizontal*/
    SShader *pp_prog_hdr_blur_vert; /* blur pass vertical */

    SShader *pp_prog_hdr_tonemap; /* final tonemap*/
    SShader *pp_prog_volumetric; /* final tonemap*/
    enum {
    	V_NORMAL,
    	V_BLOOM,
        V_BLOOM_BLEND,
        V_SSAO,
    	V_DIRECT,
        V_SHADOW_MAP,
        V_VOLUMETRIC,

    	V_MAX,
    };

    /*view selecor*/
    int d_v_sel_current = 0;
    int d_v_sel [V_MAX] = {1 };
    const int d_v_sel_max = V_MAX-1;


    const inline std::string C_I(int n);
    const inline std::string V_I(int n);

    /*main scene render*/
    int RenderDirect(const Viewport& v);
    /*render shadow map*/
    int RenderShadowMap(const Viewport& v);


    std::shared_ptr<ConsoleCommandHandler> d_console_cmd_handler;
public:
   UIConsole *con; 
private:
   UIConsoleErrorHandler *err_con;

   bool d_toggle_fps_view = true; 
   bool d_toggle_cfg_view = true;
   bool d_toggle_fullscreen = false;
   bool d_toggle_MSAA = true;
public:
    int UpdateScene();
};

/*Singleton */
std::shared_ptr<SScene> sc; 
std::shared_ptr<InputCommandHandler> s_input;
/* Global configuration*/

SScene::SScene(Viewport *v) 
    :rtSCREEN(v)
    ,con(new UIConsole(v,  d_console_cmd_handler ))
    ,d_console_cmd_handler(new ConsoleCommandHandler())
    ,d_shadowmap_cam(-1344.012,-1744.6,-43.26,0.4625, -1.57,0.0,SPerspectiveProjectionMatrix(100.0f, 5000.0f,1.0f,toRad(26.0)))
    ,cam(0,0,0,0,0,0,SPerspectiveProjectionMatrix(100.0f, 5000.0f,1.0f,toRad(26.0)))
    ,sky_cam(0,0,0,0,0,0,SPerspectiveProjectionMatrix(100.0f, 10000.0f,1.0f,toRad(26.0)))
    ,step(0.0f)
    
    ,err_con(new UIConsoleErrorHandler(con))
    ,fps_label(new UILabel(v))
    ,cfg_label(new UILabel(v,0.0, 0.7))
    ,v_sel_label(new UILabel(v,0.0, 0.55))
    ,rtShadowMap(new Viewport((v->w),(v->h),RT_TEXTURE_RGBA,
            std::shared_ptr<SRBOTexture>(new SRBOTexture(v->w*2,v->h*2,RT_TEXTURE_RGBA)), /*color */
            std::shared_ptr<SRBOTexture>(new SRBOTexture(v->w,v->h,RT_TEXTURE_RGBA)), /*rsm*/
            std::shared_ptr<SRBOTexture>(new SRBOTexture(v->w,v->h,RT_TEXTURE_RGBA)), /*rsm */
            std::shared_ptr<SRBOTexture>(new SRBOTexture(v->w,v->h,RT_TEXTURE_DEPTH) ))) /*shadow map render from light*/
    ,rtHDRScene_MSAA(new Viewport((v->w),(v->h),RT_TEXTURE_FLOAT,
            std::shared_ptr<SRBOTexture>(new SRBOTexture(v->w,v->h,RT_TEXTURE_MSAA)), /*IMG0 color*/
            std::shared_ptr<SRBOTexture>(new SRBOTexture(v->w,v->h,RT_TEXTURE_MSAA)), /*IMG1 normal in world space */
            nullptr,    /*IMG2*/
            std::shared_ptr<SRBOTexture>(new SRBOTexture(v->w,v->h,RT_TEXTURE_DEPTH_MSAA) ))) /*depth for Volumetric*/
    ,rtHDRScene(new Viewport((v->w),(v->h),RT_TEXTURE_FLOAT,
            std::shared_ptr<SRBOTexture>(new SRBOTexture(v->w,v->h,RT_TEXTURE_FLOAT)), /*IMG0 color*/
            std::shared_ptr<SRBOTexture>(new SRBOTexture(v->w,v->h,RT_TEXTURE_FLOAT)), /*IMG1 normal in world space */
            nullptr,    /*IMG2*/
            std::shared_ptr<SRBOTexture>(new SRBOTexture(v->w,v->h,RT_TEXTURE_DEPTH) ))) /*depth for Volumetric*/
    ,rtHDRBloomResult( new Viewport((v->w)/2, (v->h)/2, RT_TEXTURE_RGBA)) /* bloom clamp*/
    ,rtHDRHorBlurResult(new Viewport((v->w)/4, (v->h)/4, RT_TEXTURE_RGBA))
    ,rtHDRVertBlurResult(new Viewport((v->w)/4, (v->h)/4, RT_TEXTURE_RGBA))

    ,rtSSAOResult( new Viewport((v->w), (v->h), RT_TEXTURE_RGBA)) /*fix me some time*/
    ,rtSSAOBLUR2(new Viewport((v->w), (v->h), RT_TEXTURE_RGBA))
    ,rtVolumetric(new Viewport((v->w), (v->h), RT_TEXTURE_RGBA))
    ,sky_dome_model(new SObjModel("sky_dome.obj"))
    ,model(new SObjModel("sponza.obj"))
   
    
{
    /*Setup error handler*/
    //BaseError::DefHandler = err_con;
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



   // model.reset(new SObjModel("sponza.obj"));

    model->ConfigureProgram( *r_prog);
   
    model->ConfigureProgram( *cam_prog);

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

        d_shadowmap_cam.LookAt(SVec4( 0.0,1900.0, 0.0,0.0),SVec4(0.0,1000.0,0.0,0.0) ,   SVec4(0.0,0.0,1.0,0.0) );        

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
        cam.setEuler(SVec4(0,0,0,0));
    }));



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
             V_I(V_VOLUMETRIC) + string_format("Volumetric Only \n",d_v_sel[V_VOLUMETRIC])  
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
int inline SScene::RenderShadowMap(const Viewport& v) {
    v.Bind(true);
    RenderContext r_ctx(&v, cam_prog ,&d_shadowmap_cam);
    model->Render(r_ctx);
    return ESUCCESS;
}
int inline SScene::RenderDirect(const Viewport& v) {
    v.Bind(true);    
    /*connect shadowmap to model sampler*/
    
    if (rWireframe)
    {
        RenderContext r_ctx(&v, cam_prog ,&d_shadowmap_cam);
        model->Render(r_ctx);

    }
    else
    {

        RenderContext r_ctx(&v, r_prog ,&cam,rtShadowMap->texDEPTH,rtShadowMap->texIMG1, rtShadowMap->texIMG2, rtShadowMap->texIMG);
        model->Render(r_ctx);

        RenderContext r_ctx2(&v, sky_dome_prog ,&sky_cam,rtShadowMap->texDEPTH);
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

void display ()
{   
    sc->Render();
    glutSwapBuffers ();
}

void reshape ( int w, int h )
{
    sc->Reshape(w,h);

}
void key ( unsigned char key, int x, int y )
{
    static int wireframe = 0;
    static int fullscreen = 0;
    static int console_mode = 0;
    if (console_mode) {
        if (key == 27)
        {
            console_mode = 0;
            sc->con->Msg("Exited from console\n");
            sc->con->HandleExitConsole();
            return;
        } else  if (key == 45) {
            sc->con->HandlePrevHistoryCommand();
            return;
        } else  if (key == 43) {
            sc->con->HandleNextHistoryCommand();
            return;
        }
        sc->con->HandleInputKey(key);
        return;
    }

    if ( key == 27 || key == 'q' || key == 'Q' )  
        exit ( 0 );
            else if (key == 'p')
                    //wireframe
                if (wireframe) {
                    wireframe = 0;
                    sc->rWireframe = 0;
                }
                else {
                    wireframe = 1;
                    sc->rWireframe = 1;
                }


else if (key == '9' ) sc->upViewItem();
else if (key == '0' ) sc->downViewItem();
else if (key == 'o' ) sc->rSSAO = !sc->rSSAO;

else if (key == 8 ) {console_mode = 1; sc->con->Cls(); sc->con->Msg("Debug console, [ESC] for exit\n"); }
else
    s_input->HandleInputKey(key);

}

void mouse_move (  int x , int y) {
    static int x_base = -10000 ;
    static int y_base = -10000 ;

    int x_o = x ;
    int y_o = y;
    static float x_rm = 0;
    static float y_rm = 0;
    if (x_base == -10000) {
        x_rm = 0;
        y_rm = 0; 
        x_base = x;
        y_base = y;
    }
    else {
    x_rm += (x_o-x_base)*0.5;
    y_rm +=(y_o-y_base)*0.5;
    x_base = x;
    y_base = y;

    }


    sc->cam.setEulerY(toRad(x_rm));
    sc->cam.setEulerX(toRad(y_rm));


}
void special(int key, int x, int y){
    if (key == GLUT_KEY_DOWN) sc->downCfgItem();
    else if (key == GLUT_KEY_UP) sc->upCfgItem();
    else if (key == GLUT_KEY_LEFT) sc->decCfgItem();
    else if (key == GLUT_KEY_RIGHT) sc->incCfgItem();
   // FIXME else s_input->HandleInputKey(key); FIXME

sc->UpdateCfgLabel();
}

void mouse(int button, int state, int x, int y)  {
  
}
int ToggleFullscreen() {
    static bool d_toggle_fullscreen  = false;
    d_toggle_fullscreen  = !d_toggle_fullscreen;
    if (d_toggle_fullscreen) {
        glutFullScreen();
    } else {
        glutReshapeWindow(1000,1000);
    }

    return 0;
}
void refresh() {
    glutPostRedisplay();
}

void APIENTRY openglCallbackFunction(GLenum source,
                                           GLenum type,
                                           GLuint id,
                                           GLenum severity,
                                           GLsizei length,
                                           const GLchar* message,
                                           const void* userParam){
 
 /* todo find  where it was called */
    printf("OpenGL MSG:%s\n", message);
}


int main ( int argc, char * argv [] )
{
    /*backtrace on windows*/
    LoadLibraryA("backtrace.dll");
    Viewport v(1000,1000);


    // initialize glut
    glutInit            ( &argc, argv );
    glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize  ( v.w, v.h);


    // prepare context for 3.3
    glutInitContextVersion ( 4, 4 );
    glutInitContextFlags   ( GLUT_FORWARD_COMPATIBLE| GLUT_DEBUG);
    glutInitContextProfile ( GLUT_CORE_PROFILE  );

    glutCreateWindow ("m_proj Shestacov Alexsey 2014 (c)" );
    glewExperimental = GL_TRUE;
    
    glewInit ();


   if(glDebugMessageCallback){
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(openglCallbackFunction, nullptr);
        GLuint unusedIds = 0;
        glDebugMessageControl(GL_DONT_CARE,
            GL_DONT_CARE,
            GL_DONT_CARE,
            0,
            &unusedIds,
            true);
    }
    else
        printf("debug callback is not available\n");

                       
   //*test compute shader*/

   //SCProg *compute = new SCProg("c_test.cpp");
   //compute->Dispatch(128,1,1);

    glutDisplayFunc  ( display );
    glutReshapeFunc  ( reshape );
    glutIdleFunc ( refresh     );
   

    glutMouseFunc(mouse);
    glutMotionFunc ( mouse_move     );

    sc.reset(new SScene(&v));

    s_input.reset(new InputCommandHandler());

    glutKeyboardFunc (key   );
     glutSpecialFunc(special);

    s_input->AddCommand("forward", InputCommandHandler::InputCommand([=] (void) -> void {
       sc->cam.goForward(25.0);
    }));
    s_input->BindKey('w',"forward");


    s_input->AddCommand("back", InputCommandHandler::InputCommand([=] (void) -> void {
        sc->cam.goForward(-25.1);
    }));
    s_input->BindKey('s',"back");

    s_input->AddCommand("toogle_fullscreen", InputCommandHandler::InputCommand([=] (void) -> void {
        ToggleFullscreen();
    }));

    s_input->BindKey('f',"toogle_fullscreen");

    s_input->AddCommand("clear_console", InputCommandHandler::InputCommand([=] (void) -> void {
        sc->con->Cls();
    }));

    s_input->BindKey('c',"clear_console");   

    glutMainLoop ();

}
