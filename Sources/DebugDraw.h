#ifndef DEBUGDRAW_H
#define DEBUGDRAW_H
#pragma once
#include <vector>
#include "mat_math.h"
class SMat4x4;
class SProg;


class DebugDraw
{
public:
    DebugDraw();
    ~DebugDraw();
    DebugDraw(const DebugDraw&) = delete;
    int AddLine(const Point& p1, const Point& p2) ;
    int AddLine(const Line& p2) ;
    int ClearAll();
    int AddCross(const Point& p1 , float sz);
    int Render(const SMat4x4& MVP);
    int Update(); /*update VBOs*/
    /*Camera debug*/
    int AddCameraFrustrum(const SMat4x4& PV);
private:
    std::vector<Line> d_lines;
    unsigned int d_lines_vao;
    unsigned int d_lines_vbo;
    SProg *d_prog;
    int d_last_update_vbo_size;

};



#endif // DEBUGDRAW_H
