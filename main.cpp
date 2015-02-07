/*
Global TODOs:
- cubemaps
- more accurate rendering path control for more flexability
- online attribute control
- skybox
*/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <windows.h>
#include <memory>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/wglew.h>

#include "c_config.h"

#include "Log.h"
#include "Command.h"
#include "Scene.h"
#include <chrono>
#include "selene.h"

/*Singletons */
/*
 * Must be moved to Singltone interface as be ready
*/
sel::State state;
std::shared_ptr<InputCommandHandler> s_input;
/* Global configuration*/
static long int g_scriptCallFrequency = 0;
static long int g_frameNumber = 0;
void display ()
{
    /* main engine loop */
   static std::chrono::steady_clock::time_point last_t;
   //full time difference;
   float dt = std::chrono::duration<float, std::milli> ( (std::chrono::steady_clock::now() - last_t) ).count();
   // update time
   last_t = std::chrono::steady_clock::now();

   g_frameNumber ++;

    SScene * sc = MainScene::GetInstance();
    auto start = std::chrono::steady_clock::now();
    sc->Render();
    if (g_scriptCallFrequency) {
        if ((g_frameNumber % g_scriptCallFrequency) == 0)
        {
            state["update"](dt);
        }

    }

    glutSwapBuffers ();

}

void reshape ( int w, int h )
{
    SScene * sc = MainScene::GetInstance();

    sc->Reshape(w,h);

}
void key ( unsigned char key, int x, int y )
{
    SScene * sc = MainScene::GetInstance();

    static int wireframe = 0;
    static int fullscreen = 0;
    static int console_mode = 0;
    if (console_mode) {
        if (key == 27)
        {
            console_mode = 0;
            sc->dbg_ui.con->HandleExitConsole();
            return;
        } else  if (key == 43) {
            sc->dbg_ui.con->HandlePrevHistoryCommand();
            return;
        }
        // disable due "-" key better accessible
        //} else  if (key == 43) {
        //    sc->con->HandleNextHistoryCommand();
       //     return;
        //}
        sc->dbg_ui.con->HandleInputKey(key);
        return;
    }

    if ( key == 27 || key == 'q' || key == 'Q' )  
    {
         glutLeaveMainLoop();
         return;
    }
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

else if (key == '9' ) sc->dbg_ui.upViewItem();
else if (key == '0' ) sc->dbg_ui.downViewItem();
else if (key == 8 ) {console_mode = 1; sc->dbg_ui.con->Cls(); sc->dbg_ui.con->Msg("Debug console, [ESC] for exit\n"); }
else
    s_input->HandleInputKey(key);

}

void mouse_move (  int x , int y) {
    SScene * sc = MainScene::GetInstance();

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
    sc->cam.rotEulerY(toRad(x_rm));
    sc->cam.rotEulerX(toRad(y_rm));
}
void special(int key, int x, int y){
    SScene * sc = MainScene::GetInstance();

    if (key == GLUT_KEY_DOWN) sc->dbg_ui.downCfgItem();
    else if (key == GLUT_KEY_UP) sc->dbg_ui.upCfgItem();
    else if (key == GLUT_KEY_LEFT) sc->dbg_ui.decCfgItem();
    else if (key == GLUT_KEY_RIGHT) sc->dbg_ui.incCfgItem();
   // FIXME else s_input->HandleInputKey(key); FIXME

sc->dbg_ui.UpdateCfgLabel();
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
    static Log gl_log("gl_log.log");
    if (severity  == GL_DEBUG_SEVERITY_HIGH)
    {
        gl_log.LogW(message);
        // nice feature;
       D_TRAP();
    }



}
int initLuaBindings(sel::State& state)
{
    /*Init lua env*/
    state.OpenLib("math",luaopen_math);
    struct Env {
        Env() {}
        /*basic log*/
       // state.OpenLib(
        int loge(std::string msg) {LOGE(msg);}
        int logw(std::string msg) {LOGW(msg);}
        int logv(std::string msg) {LOGV(msg);}
        int  fwd(double a) {MainScene::GetInstance()->cam.goForward(a);}
        int setUpdateCallFrequency(int f) {g_scriptCallFrequency = f; }

    };
    state["Env"].SetClass<Env>("loge",&Env::loge,
                               "logw",&Env::logw,
                               "logv",&Env::logv,
                               //setup script frequency
                               "setUpdateCallFrequency",&Env::setUpdateCallFrequency,
                               "fwd", &Env::fwd
                               );

    //state[Scene].


    return 0;
}

int main ( int argc, char * argv [] )
{
    std::ios::sync_with_stdio(false);
    MainLog log;
    MainConfig config;

    Log gl_log("gl_log.log");

    LOGV("GIT REVISION:"  GIT_SHA1 );
    /*backtrace on windows*/
    LoadLibraryA("backtrace.dll");
    LOGV("Backtrace dll loaded");

    LOGV("Init gl context");

    int h = config["launch.h"].GetInt();
    int w = config["launch.w"].GetInt();
    int ogl_major = config["launch.ogl_major"].GetInt();
    int ogl_minor = config["launch.ogl_minor"].GetInt();
    //printf("create window: %d , %d\n", h,w);
    RBO  v(w,h);
    // initialize glut
    glutInit            ( &argc, argv );
    glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize  ( v.getSize().w,v.getSize().h);
    // init modern opengl
    glutInitContextVersion ( ogl_major, ogl_minor );
    glutInitContextFlags   ( GLUT_FORWARD_COMPATIBLE| GLUT_DEBUG);
    glutInitContextProfile ( GLUT_CORE_PROFILE  );

    glutCreateWindow ("m_proj Shestacov Alexsey 2014-2015(c)" );
    glewExperimental = GL_TRUE; /*Required for some new glFunctions otherwice will crash*/
    GLenum err= glewInit ();
    if (err != GLEW_OK) {
        LOGE("error create OpenGL context, exiting");
        return -1;
    }

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
        LOGV("OpenGl debug callback installed");
    }
    else
        LOGW("OpenGl debug callback not avaliable");



    LOGV("Create Scene");

    MainScene msc(&v);

    //sc.reset(new SScene(&v));
    SScene * sc = MainScene::GetInstance();
    s_input.reset(new InputCommandHandler());




    /*glut callbacks*/
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(refresh);
    glutMouseFunc(mouse);
    glutMotionFunc(mouse_move);
    glutKeyboardFunc(key);
    glutSpecialFunc(special);



    /*input key handlers*/
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
        sc->dbg_ui.con->Cls();
    }));
    s_input->BindKey('c',"clear_console");


    LOGV("Init Script");

    initLuaBindings(state);

    //state.Load("init.lua");
    //state["init"]();

    if (state.Load("init.lua")) {
        state["init"]();
    } else
        LOGE("No init.lua script find or load failed!");
    Singltone<sel::State>  g_script_state(&state);


    sc->dbg_ui.d_toggle_cfg_view = config["scene.toggle_debug_viewport_cfg"].GetInt();
    sc->dbg_ui.d_toggle_fps_view = config["scene.toggle_debug_viewport_fps"].GetInt();


    sc->toggleMSAA((bool)config["scene.toggle_msaa"].GetInt());
    sc->toggleBrightPass((bool)config["scene.toggle_brightpass"].GetInt());

    /*load models*/
    sc->AddObjectToRender(std::shared_ptr<SObjModel> (new SObjModel("sponza.obj")) );
    sc->AddObjectToRender(std::shared_ptr<SObjModel> (new SObjModel("sky_dome.obj")) );
    /*remove me please*/
    sc->d_render_list[1]->SetModelMat(SMat4x4().Scale(2.0,2.0,2.0).Move(0.0,200.0,0.0));

    /* main loop */
    LOGV("Entering main loop");
    sc->dbg_ui.con->Msg("git revision: " GIT_SHA1 "\n");
    sc->dbg_ui.con->Msg("Model View\nShestacov Alexsey 2014-2015(c)\n");
    glutMainLoop ();
    LOGV("Leave main loop");

}
