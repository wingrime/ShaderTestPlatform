#pragma once
#ifndef __IMGUI_HOOKS__
#define __IMGUI_HOOKS__
/*
	IMGui integration code 
*/

/*
	Setup data, requires valig gl context
*/
int imGuiSetup();
/*
	Render MIGui, requires called within draw context before imGui calls
*/
int imGuiRender();
#endif
