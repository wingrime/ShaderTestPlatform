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
#include "UIConsole.h"
#include "UILabel.h"

/* shader class*/
#include "r_sprog.h"

#include "ObjModel.h"
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
#include "RBO.h"
#include <windows.h>
#include "MAssert.h"
#include "Log.h"
#include "Command.h"
#include "Scene.h"

/*Singleton */
std::shared_ptr<SScene> sc; 
std::shared_ptr<InputCommandHandler> s_input;
/* Global configuration*/

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
            sc->con->HandleExitConsole();
            return;
        } else  if (key == 43) {
            sc->con->HandlePrevHistoryCommand();
            return;
        }
        // disable due "-" key better accessible
        //} else  if (key == 43) {
        //    sc->con->HandleNextHistoryCommand();
       //     return;
        //}
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
    sc->cam.rotEulerY(toRad(x_rm));
    sc->cam.rotEulerX(toRad(y_rm));
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
    static Log gl_log("gl_log.log");
    if (severity  == GL_DEBUG_SEVERITY_HIGH)
    {
        gl_log.LogW(message);
        // nice feature;
       //D_TRAP();
    }



}


int main ( int argc, char * argv [] )
{
    std::ios::sync_with_stdio(false);
    MainLog log;
    Log gl_log("gl_log.log");

    /*backtrace on windows*/
    LoadLibraryA("backtrace.dll");
    RBO v(1000,1000);




    // initialize glut
    glutInit            ( &argc, argv );
    glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize  ( v.getSize().w,v.getSize().h);


    // init modern opengl
    glutInitContextVersion ( 4, 2 );
    glutInitContextFlags   ( GLUT_FORWARD_COMPATIBLE| GLUT_DEBUG);
    glutInitContextProfile ( GLUT_CORE_PROFILE  );

    glutCreateWindow ("m_proj Shestacov Alexsey 2014-2015(c)" );
    glewExperimental = GL_TRUE;
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

    glutDisplayFunc  ( display );
    glutReshapeFunc  ( reshape );
    glutIdleFunc ( refresh     );
   

    glutMouseFunc(mouse);
    glutMotionFunc (mouse_move);

    sc.reset(new SScene(&v));

    s_input.reset(new InputCommandHandler());

    glutKeyboardFunc (key   );
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
        sc->con->Cls();
    }));
    s_input->BindKey('c',"clear_console");   
    /* main loop */
    glutMainLoop ();

}
