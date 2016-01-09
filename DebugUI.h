#pragma once
#include "UILabel.h"
#include "UIConsole.h"
#include "mat_math.h"
class SScene;
class DebugUI {
public:

   UIConsole *con;

    DebugUI(RectSizeInt &v);

    int Draw();
    int Reshape(const RectSizeInt &v);
    int InitDebugCommands();
    int ToggleFPSCounter(bool b);




    std::shared_ptr<UILabel> fps_label;

    SScene *sc;
private:
    /* imGUI code*/
    int DrawGUI();
    int DrawIntrospectionGUI(bool *opened);
    int DrawConsoleUI(bool *opened);

    bool d_toggle_fps_view = true;
    bool d_toggleConsoleOverlay = false;

    ConsoleCommandHandler *d_console_cmd_handler;
    UIConsoleErrorHandler *err_con;

};
