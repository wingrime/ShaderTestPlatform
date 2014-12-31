#include "UILabel.h"

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
