#include "DebugDraw.h"
#include "r_shader.h"
#include "ErrorCodes.h"
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#endif 
DebugDraw::DebugDraw()
{
    d_prog = new  SProg("Debug/Lines.vert","Debug/Lines.frag");

    glGenVertexArrays ( 1, &d_lines_vao );
    glBindVertexArray ( d_lines_vao );


     glGenBuffers(1, &d_lines_vbo);
     glBindBuffer(GL_ARRAY_BUFFER, d_lines_vbo);

     d_prog->Bind();
     d_prog->SetAttrib( "pos", sizeof(Point)/sizeof(float), sizeof(Point), 0,GL_FLOAT);
     glBindVertexArray ( 0 );
     d_last_update_vbo_size = -1;

}


DebugDraw::~DebugDraw()
{
    //todo
    glDeleteBuffers(1,&d_lines_vbo);

    delete d_prog;
}

int DebugDraw::AddLine(const Point &p1, const Point &p2)
{
    Line l = {p1,p2};
    d_lines.push_back(l);
    return d_lines.size() - 1;
}
int DebugDraw::AddLine(const Line &p2)
{
     d_lines.push_back(p2);
    return d_lines.size() - 1;
}
int DebugDraw::ClearAll()
{
    d_lines.clear();
    return ESUCCESS;
}

int DebugDraw::AddCross(const Point &p1, float sz)
{
    Line l_x = { {(float)(p1.x - sz/2.0), p1.y, p1.z } ,{(float)(p1.x + sz/2.0), p1.y, p1.z }   };
    Line l_y = { {p1.x , (float)(p1.y- sz/2.0), p1.z } ,{p1.x , (float)(p1.y+ sz/2.0), p1.z }   };
    Line l_z = { {p1.x , p1.y, (float)(p1.z - sz/2.0)} ,{p1.x , p1.y, (float)(p1.z + sz/2.0)}   };
    AddLine(l_x);
    AddLine(l_y);
    AddLine(l_z);
    return ESUCCESS;
}

int DebugDraw::Render(const SMat4x4& MVP)
{

    if (!d_lines.empty() ) {
        glBindVertexArray ( d_lines_vao );
        d_prog->Bind();
        d_prog->SetUniform("MVP",MVP);
        glDrawArrays(GL_LINES, 0, d_lines.size()*2);
        glBindVertexArray ( 0 );

    }
    return ESUCCESS;
}

int DebugDraw::Update()
{
    if (!d_lines.empty()) {
        int vbo_sz = d_lines.size()*sizeof(Line);
        glBindVertexArray ( d_lines_vao );
        //if (d_last_update_vbo_size != vbo_sz ) {
        d_last_update_vbo_size = vbo_sz;
        glBindBuffer(GL_ARRAY_BUFFER, d_lines_vbo);
        glBufferData(GL_ARRAY_BUFFER, d_lines.size()*sizeof(Line), 0, GL_DYNAMIC_DRAW);

        //}
        glBufferSubData(GL_ARRAY_BUFFER, 0,d_lines.size()*sizeof(Line), d_lines.data());
        glBindVertexArray ( 0 );
    }
    return ESUCCESS;
}

int DebugDraw::AddCameraFrustrum(const SMat4x4 &PV)
{
    /*position*/
    SMat4x4 invPV = (PV).Inverse();
    vec4 f1 = invPV*vec4(1.0,-1.0,-1.0,1.0);
    f1.DivW();
    vec4 f2 = invPV*vec4(-1.0,1.0,-1.0,1.0);
    f2.DivW();
    vec4 f3 = invPV*vec4(1.0,1.0,-1.0,1.0);
    f3.DivW();
    vec4 f4 = invPV*vec4(-1.0,-1.0,-1.0,1.0);
    f4.DivW();

    vec4 b1 = invPV*vec4(1.0,-1.0,1.0,1.0);
    b1.DivW();
    vec4 b2 = invPV*vec4(-1.0,1.0,1.0,1.0);
    b2.DivW();
    vec4 b3 = invPV*vec4(1.0,1.0,1.0,1.0);
    b3.DivW();
    vec4 b4 = invPV*vec4(-1.0,-1.0,1.0,1.0);
    b4.DivW();


    /*front plane*/
    AddLine(Point(f1),Point(f2));
    AddLine(Point(f2),Point(f3));
    AddLine(Point(f3),Point(f1));
    AddLine(Point(f1),Point(f4));
    AddLine(Point(f2),Point(f4));
    AddLine(Point(f3),Point(f4));
    /*back plane*/
    AddLine(Point(b1),Point(b2));
    AddLine(Point(b2),Point(b3));
    AddLine(Point(b3),Point(b1));
    AddLine(Point(b1),Point(b4));
    AddLine(Point(b2),Point(b4));
    AddLine(Point(b3),Point(b4));
    /*connect*/
    AddLine(Point(f1),Point(b1));
    AddLine(Point(f2),Point(b2));
    AddLine(Point(f3),Point(b3));
    AddLine(Point(f4),Point(b4));
    return ESUCCESS;

}

int DebugDraw::AddAABB(const AABB &a)
{

    AddLine(a.max_point,Point(a.max_point.x,a.min_point.y,a.max_point.z));
    AddLine(a.max_point,Point(a.max_point.x,a.max_point.y,a.min_point.z));
    AddLine(a.max_point,Point(a.min_point.x,a.max_point.y,a.max_point.z));


    AddLine(a.min_point,Point(a.max_point.x,a.min_point.y,a.min_point.z));
    AddLine(a.min_point,Point(a.min_point.x,a.max_point.y,a.min_point.z));
    AddLine(a.min_point,Point(a.min_point.x,a.min_point.y,a.max_point.z));

    AddLine(Point(a.max_point.x,a.min_point.y,a.min_point.z),Point(a.max_point.x,a.min_point.y,a.max_point.z));
    AddLine(Point(a.max_point.x,a.min_point.y,a.max_point.z),Point(a.min_point.x,a.min_point.y,a.max_point.z));

    AddLine(Point(a.min_point.x,a.max_point.y,a.max_point.z),Point(a.min_point.x,a.max_point.y,a.min_point.z));

    AddLine(Point(a.min_point.x,a.max_point.y,a.min_point.z),Point(a.max_point.x,a.max_point.y,a.min_point.z));

    AddLine(Point(a.min_point.x,a.max_point.y,a.max_point.z),Point(a.min_point.x,a.min_point.y,a.max_point.z));

    AddLine(Point(a.max_point.x,a.min_point.y,a.min_point.z),Point(a.max_point.x,a.max_point.y,a.min_point.z));
    return ESUCCESS;
}
