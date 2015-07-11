#pragma once

#include "RBO.h"
#include <string>
#include <vector>
/*factory - compile time depedency it need to be remaked*/
#include "UIFont.h"
#include "mat_math.h"

class UILabel {
public:
    UILabel(RectSizeInt &v) :UILabel(v, 0.0, 0.5) {}
    UILabel(RectSizeInt &v, float x, float y) ;
    /*Draw console*/
    void Draw();
    /*Add simple message*/
    void setText(const std::string& msg);
private:
    UIFontFactory ft;
    RectSizeInt v_port;

    UIFont * fnt;
    std::string con;
    float d_x = 0;
    float d_y = 0;
};
