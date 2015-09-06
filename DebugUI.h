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

    bool d_toggle_fps_view = true;



    std::shared_ptr<UILabel> fps_label;

    SScene *sc;
private:
    /* imGUI code*/
    int DrawGUI();
    int DrawIntrospectionGUI(bool *opened);

    ConsoleCommandHandler *d_console_cmd_handler;
    UIConsoleErrorHandler *err_con;

};
