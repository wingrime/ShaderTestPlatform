#pragma once
#include "UILabel.h"
#include "UIConsole.h"
#include "mat_math.h"
class SScene;
class DebugUI {
public:

        RectSizeInt v;
        std::shared_ptr<ConsoleCommandHandler> d_console_cmd_handler;
        UIConsole *con;


    DebugUI(SScene *_s, RectSizeInt &v);
    int Draw();
    int InitDebugCommands();

    bool d_toggle_fps_view = true;
    bool d_toggle_cfg_view = true;


    UIConsoleErrorHandler *err_con;

    /*on screen UI shold be rid from there*/
    int UpdateCfgLabel();
    int UpdateViewSelLabel();
    inline int Init() { UpdateViewSelLabel();InitDebugCommands();}
    int upCfgItem();
    int downCfgItem();
    int incCfgItem();
    int decCfgItem();
    int upViewItem();
    int downViewItem();

    /* imGUI integration code*/
    int DrawGUI();
    int DrawIntrospectionGUI(bool *opened);

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

    float d_cfg_step = 0.001;

    SScene *sc;


};
