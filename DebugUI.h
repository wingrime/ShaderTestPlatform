#pragma once
#include "UILabel.h"
#include "UIConsole.h"
#include "mat_math.h"
class SScene;
class DebugUI {
public:
        ConsoleCommandHandler *d_console_cmd_handler;
        UIConsoleErrorHandler *err_con;
        UIConsole *con;


    DebugUI(SScene *_s, RectSizeInt &v);
    int Draw();
    int InitDebugCommands();

    bool d_toggle_fps_view = true;

    /*on screen UI shold be rid from there*/
    int UpdateCfgLabel();
    int UpdateViewSelLabel();
    int upViewItem();
    int downViewItem();

    /* imGUI code*/
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

    std::shared_ptr<UILabel> fps_label;

    SScene *sc;


};
