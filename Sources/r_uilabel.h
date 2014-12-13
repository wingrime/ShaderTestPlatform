#pragma once

#include "viewport.h"
#include <string>
#include <vector>
#include "r_uibase.h"


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
UILabel::UILabel(Viewport *v,float x ,float y) {

    fnt = ft.Construct(16);
    v_port = v;
    d_x  = x;
    d_y  = y;
}
void UILabel::Draw() {

    if (!con.empty()) {

        float sx = ((v_port->w));
        float sy = ((v_port->h));
        fnt->RenderText(con,d_x, d_y, sx, sy);    
    }
}
void UILabel::setText(const std::string& label) {
	con = std::string(label);
}