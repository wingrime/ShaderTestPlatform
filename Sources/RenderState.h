#pragma once
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/wglew.h>
/* incapsulate Render Stage*/
class RenderPass {
public:
    /*Render State */
    enum PassDepthMode {
        NEVER  = 0,
        LESS_OR_EQUAL = 1
        /*add when required*/
    } ;
    enum PassToggle {
        ENABLED = 1,
        DISABLED = 0
    };
    /*BASIC*/
    RenderPass(PassDepthMode mode, PassToggle test, PassToggle multisample);
    int Bind();



private:
    PassDepthMode d_depth_mode;
    PassToggle d_depth_test;
    PassToggle d_multisample;
};
