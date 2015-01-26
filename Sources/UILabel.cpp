#include "UILabel.h"
#include "mat_math.h"
UILabel::UILabel(RBO *v,float x ,float y) {

    fnt = ft.Construct(20);
    v_port = v;
    d_x  = x;
    d_y  = y;
}
void UILabel::Draw() {

    if (!con.empty()) {

        SVec2 sz = v_port->getSize();
        fnt->RenderText(con,d_x, d_y, sz.x, sz.y);
    }
}
void UILabel::setText(const std::string& label) {
	con = std::string(label);
}
