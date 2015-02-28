#pragma once
#include "UILabel.h"
#include "UIConsole.h"
class SScene;
class DebugUI {
public:

        RBO *v;
        std::shared_ptr<ConsoleCommandHandler> d_console_cmd_handler;
        UIConsole *con;


    DebugUI(SScene *_s, RBO* _v);
    int Draw();
    int InitDebugCommands();
    bool d_toggle_fps_view = true;
    bool d_toggle_cfg_view = true;


    UIConsoleErrorHandler *err_con;

    /*on screen UI shold be rid from there*/
    int UpdateCfgLabel();
    int UpdateViewSelLabel();
    inline int Init() { UpdateCfgLabel();UpdateViewSelLabel();InitDebugCommands();}
    int upCfgItem();
    int downCfgItem();
    int incCfgItem();
    int decCfgItem();
    int upViewItem();
    int downViewItem();


    enum {
        V_NORMAL,
        V_BLOOM,
        V_BLOOM_BLEND,
        V_SSAO,
        V_DIRECT,
        V_SHADOW_MAP,
        V_VOLUMETRIC,
        V_CUBEMAPTEST,
        V_MAX,
    };

    /*view selecor*/
    int d_v_sel_current = 0;
    int d_v_sel [V_MAX] = {1 };
    const int d_v_sel_max = V_MAX-1;


    const inline std::string C_I(int n);
    const inline std::string V_I(int n);

    std::shared_ptr<UILabel> fps_label;
    std::shared_ptr<UILabel> cfg_label;
    std::shared_ptr<UILabel> v_sel_label;

    int d_cfg_current = 0;
    int d_cfg_max = 14;
    /*default parameter set*/
    float  d_cfg [15] = {0.020, 0.40,0.00,0.26,2.0,0.75,0.015,0.22,0.30,0.10,0.20,0.01,0.30,1.12,4.6};
    float d_cfg_step = 0.001;

    SScene *sc;


};
