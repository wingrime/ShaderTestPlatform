#include "RenderState.h"
#include "ErrorCodes.h"
RenderPass::RenderPass(PassDepthMode mode, PassToggle test,PassToggle multisample):d_depth_mode(mode),
                                                                                   d_depth_test(test),
                                                                                   d_multisample(multisample) {}


int  RenderPass::Bind() {
    /*depth test*/
    if (d_depth_test == PassToggle::ENABLED )
        glEnable(GL_DEPTH_TEST);
    else if (d_depth_test == PassToggle::DISABLED )
        glDisable(GL_DEPTH_TEST);
    /*depth mode*/
    if (d_depth_mode == PassDepthMode::LESS_OR_EQUAL )
        glDepthFunc(GL_LEQUAL);
    else if (d_depth_mode == PassDepthMode::NEVER)
        glDepthFunc(GL_NEVER);
    /*multisample*/
    if (d_multisample == PassToggle::ENABLED )
        glEnable( GL_MULTISAMPLE );
    else if (d_multisample == PassToggle::DISABLED )
        glDisable( GL_MULTISAMPLE );
    return ESUCCESS;
}
