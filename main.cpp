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

#include "oglGlutInit.h"

#include <imgui.h>
/*imGUI data*/
static GLuint       g_FontTexture = 0;
static int          g_ShaderHandle = 0, g_VertHandle = 0, g_FragHandle = 0;
static int          g_AttribLocationTex = 0, g_AttribLocationProjMtx = 0;
static int          g_AttribLocationPosition = 0, g_AttribLocationUV = 0, g_AttribLocationColor = 0;
static size_t       g_VboSize = 0;
static unsigned int g_VboHandle = 0, g_VaoHandle = 0;
static void ImGui_Impl_RenderDrawLists(ImDrawList** const cmd_lists, int cmd_lists_count)
{
    if (cmd_lists_count == 0)
        return;

    // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
    GLint last_program, last_texture;
    glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glActiveTexture(GL_TEXTURE0);

    // Setup orthographic projection matrix
    const float width = ImGui::GetIO().DisplaySize.x;
    const float height = ImGui::GetIO().DisplaySize.y;
    const float ortho_projection[4][4] =
    {
        { 2.0f/width,	0.0f,			0.0f,		0.0f },
        { 0.0f,			2.0f/-height,	0.0f,		0.0f },
        { 0.0f,			0.0f,			-1.0f,		0.0f },
        { -1.0f,		1.0f,			0.0f,		1.0f },
    };
    glUseProgram(g_ShaderHandle);
    glUniform1i(g_AttribLocationTex, 0);
    glUniformMatrix4fv(g_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);

    // Grow our buffer according to what we need
    size_t total_vtx_count = 0;
    for (int n = 0; n < cmd_lists_count; n++)
        total_vtx_count += cmd_lists[n]->vtx_buffer.size();
    glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
    size_t needed_vtx_size = total_vtx_count * sizeof(ImDrawVert);
    if (g_VboSize < needed_vtx_size)
    {
        g_VboSize = needed_vtx_size + 5000 * sizeof(ImDrawVert);  // Grow buffer
        glBufferData(GL_ARRAY_BUFFER, g_VboSize, NULL, GL_STREAM_DRAW);
    }

    // Copy and convert all vertices into a single contiguous buffer
    unsigned char* buffer_data = (unsigned char*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    if (!buffer_data)
        return;
    for (int n = 0; n < cmd_lists_count; n++)
    {
        const ImDrawList* cmd_list = cmd_lists[n];
        memcpy(buffer_data, &cmd_list->vtx_buffer[0], cmd_list->vtx_buffer.size() * sizeof(ImDrawVert));
        buffer_data += cmd_list->vtx_buffer.size() * sizeof(ImDrawVert);
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(g_VaoHandle);

    int cmd_offset = 0;
    for (int n = 0; n < cmd_lists_count; n++)
    {
        const ImDrawList* cmd_list = cmd_lists[n];
        int vtx_offset = cmd_offset;
        const ImDrawCmd* pcmd_end = cmd_list->commands.end();
        for (const ImDrawCmd* pcmd = cmd_list->commands.begin(); pcmd != pcmd_end; pcmd++)
        {
            if (pcmd->user_callback)
            {
                pcmd->user_callback(cmd_list, pcmd);
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->texture_id);
                glScissor((int)pcmd->clip_rect.x, (int)(height - pcmd->clip_rect.w), (int)(pcmd->clip_rect.z - pcmd->clip_rect.x), (int)(pcmd->clip_rect.w - pcmd->clip_rect.y));
                glDrawArrays(GL_TRIANGLES, vtx_offset, pcmd->vtx_count);
            }
            vtx_offset += pcmd->vtx_count;
        }
        cmd_offset = vtx_offset;
    }

    // Restore modified state
    glBindVertexArray(0);
    glUseProgram(last_program);
    glDisable(GL_SCISSOR_TEST);
    glBindTexture(GL_TEXTURE_2D, last_texture);
}

void ImGui_CreateFontsTexture()
{
    ImGuiIO& io = ImGui::GetIO();

    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.

    glGenTextures(1, &g_FontTexture);
    glBindTexture(GL_TEXTURE_2D, g_FontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // Store our identifier
    io.Fonts->TexID = (void *)(intptr_t)g_FontTexture;
}

bool ImGui_Impl_CreateDeviceObjects()
{
    const GLchar *vertex_shader =
        "#version 330\n"
        "uniform mat4 ProjMtx;\n"
        "in vec2 Position;\n"
        "in vec2 UV;\n"
        "in vec4 Color;\n"
        "out vec2 Frag_UV;\n"
        "out vec4 Frag_Color;\n"
        "void main()\n"
        "{\n"
        "	Frag_UV = UV;\n"
        "	Frag_Color = Color;\n"
        "	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
        "}\n";

    const GLchar* fragment_shader =
        "#version 330\n"
        "uniform sampler2D Texture;\n"
        "in vec2 Frag_UV;\n"
        "in vec4 Frag_Color;\n"
        "out vec4 Out_Color;\n"
        "void main()\n"
        "{\n"
        "	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
        "}\n";

    g_ShaderHandle = glCreateProgram();
    g_VertHandle = glCreateShader(GL_VERTEX_SHADER);
    g_FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(g_VertHandle, 1, &vertex_shader, 0);
    glShaderSource(g_FragHandle, 1, &fragment_shader, 0);
    glCompileShader(g_VertHandle);
    glCompileShader(g_FragHandle);
    glAttachShader(g_ShaderHandle, g_VertHandle);
    glAttachShader(g_ShaderHandle, g_FragHandle);
    glLinkProgram(g_ShaderHandle);

    g_AttribLocationTex = glGetUniformLocation(g_ShaderHandle, "Texture");
    g_AttribLocationProjMtx = glGetUniformLocation(g_ShaderHandle, "ProjMtx");
    g_AttribLocationPosition = glGetAttribLocation(g_ShaderHandle, "Position");
    g_AttribLocationUV = glGetAttribLocation(g_ShaderHandle, "UV");
    g_AttribLocationColor = glGetAttribLocation(g_ShaderHandle, "Color");

    glGenBuffers(1, &g_VboHandle);

    glGenVertexArrays(1, &g_VaoHandle);
    glBindVertexArray(g_VaoHandle);
    glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
    glEnableVertexAttribArray(g_AttribLocationPosition);
    glEnableVertexAttribArray(g_AttribLocationUV);
    glEnableVertexAttribArray(g_AttribLocationColor);

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
    glVertexAttribPointer(g_AttribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, pos));
    glVertexAttribPointer(g_AttribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, uv));
    glVertexAttribPointer(g_AttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    ImGui_CreateFontsTexture();

    return true;
}
int imGuiSetup() {
    // Set up ImGui
        ImGuiIO& guiIO = ImGui::GetIO();
        guiIO.DisplaySize.x = 1000.0f;
        guiIO.DisplaySize.y =  1000.0f;
        guiIO.DeltaTime = 1.0f / 60.0f;
        //guiIO.IniFilename = "imgui.ini";
        guiIO.RenderDrawListsFn = ImGui_Impl_RenderDrawLists;
        guiIO.KeyMap[0] = 9;    // tab
        guiIO.KeyMap[1] = GLUT_KEY_LEFT;    // Left
        guiIO.KeyMap[2] = GLUT_KEY_RIGHT;   // Right
        guiIO.KeyMap[3] = GLUT_KEY_UP;      // Up
        guiIO.KeyMap[4] = GLUT_KEY_DOWN;    // Down
        guiIO.KeyMap[5] = GLUT_KEY_HOME;    // Home
        guiIO.KeyMap[6] = GLUT_KEY_END;     // End
        guiIO.KeyMap[7] = 127;  // Delete
        guiIO.KeyMap[8] = 8;    // Backspace
        guiIO.KeyMap[9] = 13;   // Enter
        guiIO.KeyMap[10] = 27;  // Escape
        guiIO.KeyMap[11] = 1;   // ctrl-A
        guiIO.KeyMap[12] = 3;   // ctrl-C
        guiIO.KeyMap[13] = 22;  // ctrl-V
        guiIO.KeyMap[14] = 24;  // ctrl-X
        guiIO.KeyMap[15] = 25;  // ctrl-Y
        guiIO.KeyMap[16] = 26;  // ctrl-Z


    return 0;
}
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
   float dt = std::chrono::duration<float, std::milli> ( (std::chrono::steady_clock::now() - last_t) ).count();\
   float dt_seconds = std::chrono::duration<float, std::ratio<1>> ( (std::chrono::steady_clock::now() - last_t) ).count();
   // update time
   last_t = std::chrono::steady_clock::now();

   g_frameNumber ++;

    SScene * sc = MainScene::GetInstance();
    auto start = std::chrono::steady_clock::now();
    if (!g_FontTexture)
        ImGui_Impl_CreateDeviceObjects();
    sc->Render();

    ImGuiIO& io = ImGui::GetIO();
    io.DeltaTime = dt_seconds;
    ImGui::NewFrame();

     ///ImGui::End();
    sc->dbg_ui.DrawGUI();
    ImGui::Render();


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
    /*ImGui hook*/
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize.x = (float)w;
    io.DisplaySize.y =  (float)h;

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
            sc->dbg_ui.con->Msg("Console close\n");
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
    /*backspace or plus goes to console*/
else if (key == 8 || key == 43) {console_mode = 1; sc->dbg_ui.con->Cls(); sc->dbg_ui.con->Msg("Debug console, [ESC] for exit\n"); }
else
    s_input->HandleInputKey(key);

}

void mouse_move_passive (  int x , int y) {

    ImGuiIO& io = ImGui::GetIO();
    /*imgui hook*/
    io.MousePos = ImVec2((float)x, (float)y);

}
void mouse_move (  int x , int y) {
    SScene * sc = MainScene::GetInstance();

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

void APIENTRY openglCallbackFunction(GLenum source,
                                           GLenum type,
                                           GLuint id,
                                           GLenum severity,
                                           GLsizei length,
                                           const GLchar* message,
                                           const void* userParam){
 
 /* todo find  where it was called */
    static Log gl_log("gl_log.log");
    if (severity  != GL_DEBUG_SEVERITY_NOTIFICATION)
   {
        gl_log.LogW(message);
        // nice feature;
       //D_TRAP();
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

int initHooks() {

    /*glut callbacks*/
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(refresh);
    glutMouseFunc(mouse);
    glutMotionFunc(mouse_move);
    glutKeyboardFunc(key);
    glutSpecialFunc(special);
    glutPassiveMotionFunc(mouse_move_passive);
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


    int h = config["launch.h"].GetInt();
    int w = config["launch.w"].GetInt();
    int ogl_major = config["launch.ogl_major"].GetInt();
    int ogl_minor = config["launch.ogl_minor"].GetInt();

    LOGV("Init gl context");
    oglInit(argc,argv,w,h,ogl_major,ogl_minor);
    initHooks();



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

    MainScene msc(RectSize(h,w));

    imGuiSetup();

    //sc.reset(new SScene(&v));
    SScene * sc = MainScene::GetInstance();
    s_input.reset(new InputCommandHandler());

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
    //sc->AddObjectToRender(std::shared_ptr<SObjModel> (new SObjModel("sky_dome.obj")) );
    /*remove me please*/
    //sc->d_render_list[1]->SetModelMat(SMat4x4().Scale(2.0,2.0,2.0).Move(0.0,200.0,0.0));

    /* main loop */
    LOGV("Entering main loop");
    sc->dbg_ui.con->Msg("git revision: " GIT_SHA1 "\n");
    sc->dbg_ui.con->Msg("Model View\nShestacov Alexsey 2014-2015(c)\n");

    glutMainLoop ();
    LOGV("Leave main loop");

}
