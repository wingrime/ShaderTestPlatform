#include <stdio.h>
#include <string.h>
#include <math.h>

#include <memory>
#include "OGL.h"


#include "c_config.h"

#include "Log.h"
#include "Command.h"
#include "Scene.h"
#include <chrono>

#include "oglGlutInit.h"
#include <imgui.h>
#include "IMGuiHooks.h"



GlobalInputState g_InputState = GlobalInputState::ACTOR_CONTROL;

void display ()
{
    /* main engine loop */
    static std::chrono::steady_clock::time_point last_t;
    //full time difference;
    float dt = std::chrono::duration<float, std::milli> ( (std::chrono::steady_clock::now() - last_t) ).count();
    float dt_seconds = std::chrono::duration<float, std::ratio<1>> ( (std::chrono::steady_clock::now() - last_t) ).count();
    // update time
    last_t = std::chrono::steady_clock::now();
    SScene * sc = Singltone<SScene>::GetInstance();
    DebugUI *dbg_ui = Singltone<DebugUI>::GetInstance();
    //auto start = std::chrono::steady_clock::now();
    imGuiRender();
    sc->Render();

    ImGuiIO& io = ImGui::GetIO();
    io.DeltaTime = dt_seconds;
    ImGui::NewFrame();


    dbg_ui->Draw();

    //if (g_scriptCallFrequency) {
    //    if ((g_frameNumber % g_scriptCallFrequency) == 0)
    //    {
    //        state["update"](dt);
    //    }

    //}
    glutSwapBuffers ();
}

void reshape ( int w, int h )
{
    SScene * sc = Singltone<SScene>::GetInstance();
    DebugUI *dbg_ui = Singltone<DebugUI>::GetInstance();

    const RectSizeInt rect(h,w);

    sc->Reshape(rect);
    dbg_ui->Reshape(rect);

}
void key_up ( unsigned char key, int x, int y )
{
    UNUSED(x);
    UNUSED(y);
    ImGuiIO& io = ImGui::GetIO();
    io.KeysDown[key] = false;
}
void special_up ( int key, int x, int y )
{
    UNUSED(x);
    UNUSED(y);
    ImGuiIO& io = ImGui::GetIO();
    io.KeysDown[key] = false;
}
void special(int key, int x, int y){
    UNUSED(x);
    UNUSED(y);
    ImGuiIO& io = ImGui::GetIO();
    io.KeysDown[key] = true;
}
void key ( unsigned char key, int x, int y )
{
    UNUSED(x);
    UNUSED(y);
    ImGuiIO& io = ImGui::GetIO();
	if (key == 27) {
	#ifdef __APPLE__
		exit(0);
	#else
		glutLeaveMainLoop();
	#endif
	}

	if (g_InputState == GlobalInputState::ACTOR_CONTROL) {
		Singltone<InputCommandHandler>::GetInstance()->HandleInputKey(key);
	}
	else if (g_InputState == GlobalInputState::DEBUG_CONTROL ) {
		io.AddInputCharacter(key);
	}
	return;
}

void mouse_move_passive (  int x , int y) {

    ImGuiIO& io = ImGui::GetIO();
    /*imgui hook*/
    io.MousePos = ImVec2((float)x, (float)y);

}
void mouse_move (  int x , int y) {
    SScene * sc = Singltone<SScene>::GetInstance();

    ImGuiIO& io = ImGui::GetIO();

    /*imgui hook*/
    io.MousePos = ImVec2((float)x, (float)y);
    /*don't interact in case we are in window*/
    if (ImGui::IsMouseHoveringAnyWindow())
        return;

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
        x_rm += (x_o-x_base)*0.5f;
        y_rm +=(y_o-y_base)*0.5f;
        x_base = x;
        y_base = y;
    }
    sc->cam.rotEulerY(toRad(x_rm));
    sc->cam.rotEulerX(toRad(y_rm));
}

void mouse(int button, int state, int x, int y)  {
    ImGuiIO& io = ImGui::GetIO();
    /*imgui hook*/
    io.MousePos = ImVec2((float)x, (float)y);
    if (button == GLUT_LEFT_BUTTON)
     io.MouseDown[0] = (state == GLUT_DOWN); // don't know why it's inversed but thats works
    else if (button == GLUT_MIDDLE_BUTTON)
     io.MouseDown[1] = (state == GLUT_DOWN);
    // Wheel reports as button 3(scroll up) and button 4(scroll down)
     else if ((button == 3))
     {
         if (state == GLUT_UP) return;
         io.MouseWheel += 0.5;
     }else if (button == 4){
         if (state == GLUT_UP) return;
         io.MouseWheel -= 0.5;
     }

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

int initGlutHooks() {

    /*glut callbacks*/
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(refresh);
    glutMouseFunc(mouse);
    glutMotionFunc(mouse_move);
    glutKeyboardFunc(key);
    glutKeyboardUpFunc(key_up);
    glutSpecialFunc(special);
    glutSpecialUpFunc(special_up);
    glutPassiveMotionFunc(mouse_move_passive);
    return 0;
}

int initKeybindings() {
	InputCommandHandler *s_input = Singltone<InputCommandHandler>::GetInstance();
    SScene * sc = Singltone<SScene>::GetInstance();
    DebugUI *dbg_ui = Singltone<DebugUI>::GetInstance();


    /*input key handlers*/
    s_input->AddCommand("forward", InputCommandHandler::InputCommand([=] (void) -> void {
       sc->cam.goForward(25.0);
    }));
    s_input->BindKey('w',"forward");

    s_input->AddCommand("back", InputCommandHandler::InputCommand([=] (void) -> void {
        sc->cam.goForward(-25.1f);
    }));
    s_input->BindKey('s',"back");

    s_input->AddCommand("toogle_fullscreen", InputCommandHandler::InputCommand([=] (void) -> void {
        ToggleFullscreen();
    }));

    s_input->BindKey('f',"toogle_fullscreen");

    s_input->AddCommand("clear_console", InputCommandHandler::InputCommand([=] (void) -> void {
        dbg_ui->con->Cls();
    }));
    s_input->BindKey('c',"clear_console");
    return 0;
}

int main ( int argc, char * argv [] )
{
    std::ios::sync_with_stdio(false);
    /*init subsystem singltones*/
    Singltone<Log> main_log;
    Singltone<Config> main_config;

    //Singltone<scripting::Scripting> mainScript("init.lua"); //TODO, from config or argv
    //scripting::Scripting * script = Singltone<scripting::Scripting>::GetInstance();

    Config * config = Singltone<Config>::GetInstance();

    LOGV("GIT REVISION:"  GIT_SHA1 );

    /*backtrace on windows*/
#ifndef __APPLE__
    LoadLibraryA("backtrace.dll");
#endif
    int h = config->operator []("launch.h").GetInt();
    int w = config->operator []("launch.w").GetInt();

    int ogl_major = config->operator []("launch.ogl_major").GetInt();
    int ogl_minor = config->operator []("launch.ogl_minor").GetInt();

    LOGV("Init OpengGL");
    oglInit(argc,argv,w,h,ogl_major,ogl_minor);
    initGlutHooks();

    /* Main Scene */
    Singltone<SScene> msc(RectSizeInt(h,w));
    SScene * sc = Singltone<SScene>::GetInstance();
    Singltone<DebugUI> dbg(RectSize((float)h,(float)w));
    DebugUI *dbg_ui = Singltone<DebugUI>::GetInstance();

    Singltone<InputCommandHandler> input_handler;

    LOGV("Init Keybindings");
    initKeybindings();

    //LOGV("Init Script subsystem");
    //initLuaBindings(state);
    //Singltone<sel::State>  g_script_state(&state);


    dbg_ui->ToggleFPSCounter((bool)config->operator []("scene.toggle_debug_viewport_fps").GetInt());

    sc->toggleMSAA((bool)config->operator []("scene.toggle_msaa").GetInt());
    sc->toggleBrightPass((bool)config->operator []("scene.toggle_brightpass").GetInt());
    LOGV("Enter load scene phase");
    /*load models*/
    sc->AddObjectToRender(std::shared_ptr<SObjModel> (new SObjModel("TestModels/sponza.obj")) );
    /* main loop */
    LOGV("Enter main game loop,hide console");
	FreeConsole();
	dbg_ui->con->Msg("Buildrev: " GIT_SHA1 "\n");
    dbg_ui->con->Msg("ShaderTestPlatform \nShestacov Alexsey 2014-2016(c)\n");

    glutMainLoop ();
    LOGV("Exiting");

}
