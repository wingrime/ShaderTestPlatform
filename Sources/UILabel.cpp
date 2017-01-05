#include "UILabel.h"
#include "mat_math.h"
UILabel::UILabel(RectSize &v, float x , float y) {

    fnt = ft.Construct(20);
    v_port = v;
    d_x  = x;
    d_y  = y;
}
void UILabel::Draw() {

    if (!con.empty()) {

        fnt->RenderText(con,d_x, d_y, v_port.w, v_port.h);
    }
}
void UILabel::setText(const std::string& label) {
	con = std::string(label);
}
