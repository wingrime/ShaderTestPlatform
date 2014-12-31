#pragma once

#include "viewport.h"
#include <string>
#include <vector>
#include "UIFont.h"


class UILabel {
public:
    UILabel(Viewport *v) :UILabel(v, 0.0, 0.5) {};
    UILabel(Viewport *v, float x, float y) ;
    /*Draw console*/
    void Draw();
    /*Add simple message*/
    void setText(const std::string& msg);
private:
    UIFontFactory ft;
    Viewport *v_port;

    UIFont * fnt;
    std::string con;
    float d_x = 0;
    float d_y = 0;
};
