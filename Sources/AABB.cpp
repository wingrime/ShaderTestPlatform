#include "AABB.h"
#include <math.h>
#include "mat_math.h"
#include <limits>

Point AABB::Center()
{
    return Point((max_point.x - min_point.x)/2.0f  ,(max_point.y-min_point.y)/2.0f,(max_point.z-min_point.z)/2.0f);
}

/*todo: move to some place*/

std::vector <Point> AABBPoints(const AABB &a) {
    std::vector <Point> p ;
    p.push_back(Point(a.max_point.x,a.max_point.y,a.max_point.z));
    p.push_back(Point(a.max_point.x,a.max_point.y,a.min_point.z));
    p.push_back(Point(a.max_point.x,a.min_point.y,a.max_point.z));
    p.push_back(Point(a.max_point.x,a.min_point.y,a.min_point.z));
    p.push_back(Point(a.min_point.x,a.max_point.y,a.max_point.z));
    p.push_back(Point(a.min_point.x,a.max_point.y,a.min_point.z));
    p.push_back(Point(a.min_point.x,a.min_point.y,a.max_point.z));
    p.push_back(Point(a.min_point.x,a.min_point.y,a.min_point.z));
    return p;
}
AABB FrustrumSize(const SMat4x4& r) {

    SMat4x4 invPV = r.Inverse();
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

    float x_m;
    x_m = fmax(f1.x,f2.x);
    x_m = fmax(x_m,f3.x);
    x_m = fmax(x_m,f4.x);
    x_m = fmax(x_m,b1.x);
    x_m = fmax(x_m,b2.x);
    x_m = fmax(x_m,b3.x);
    x_m = fmax(x_m,b4.x);

    float x_mi;
    x_mi = fmin(f1.x,f2.x);
    x_mi = fmin(x_mi,f3.x);
    x_mi = fmin(x_mi,f4.x);
    x_mi = fmin(x_mi,b1.x);
    x_mi = fmin(x_mi,b2.x);
    x_mi = fmin(x_mi,b3.x);
    x_mi = fmin(x_mi,b4.x);

    float y_m;
    y_m = fmax(f1.y,f2.y);
    y_m = fmax(y_m,f3.y);
    y_m = fmax(y_m,f4.y);
    y_m = fmax(y_m,b1.y);
    y_m = fmax(y_m,b2.y);
    y_m = fmax(y_m,b3.y);
    y_m = fmax(y_m,b4.y);

    float y_mi;
    y_mi = fmin(f1.y,f2.y);
    y_mi = fmin(y_mi,f3.y);
    y_mi = fmin(y_mi,f4.y);
    y_mi = fmin(y_mi,b1.y);
    y_mi = fmin(y_mi,b2.y);
    y_mi = fmin(y_mi,b3.y);
    y_mi = fmin(y_mi,b4.y);

    float z_m;
    z_m = fmax(f1.z,f2.z);
    z_m = fmax(z_m,f3.z);
    z_m = fmax(z_m,f4.z);
    z_m = fmax(z_m,b1.z);
    z_m = fmax(z_m,b2.z);
    z_m = fmax(z_m,b3.z);
    z_m = fmax(z_m,b4.z);

    float z_mi;
    z_mi = fmin(f1.z,f2.z);
    z_mi = fmin(z_mi,f3.z);
    z_mi = fmin(z_mi,f4.z);
    z_mi = fmin(z_mi,b1.z);
    z_mi = fmin(z_mi,b2.z);
    z_mi = fmin(z_mi,b3.z);
    z_mi = fmin(z_mi,b4.z);

    AABB a;
    a.min_point = Point(x_mi,y_mi,z_mi);
    a.max_point = Point(x_m,y_m,z_m);

    return a;
}
const Point TransformPoint(const Point &in, const SMat4x4 &sm_mvp) {
    vec4 v = sm_mvp*vec4(in.x,in.y,in.z,1.0);
    v = v / v.w;
    v.w = 1.0;
    return Point(v.x,v.y,v.z);
}
AABB ProjectionAABB(const AABB &a, const SMat4x4 &sm_mvp) {
    Point p[8];

    p[0] = TransformPoint(Point(a.max_point.x,a.max_point.y,a.max_point.z),sm_mvp);
    p[1] = TransformPoint(Point(a.max_point.x,a.max_point.y,a.min_point.z),sm_mvp);
    p[2] = TransformPoint(Point(a.max_point.x,a.min_point.y,a.max_point.z),sm_mvp);
    p[3] = TransformPoint(Point(a.max_point.x,a.min_point.y,a.min_point.z),sm_mvp);
    p[4] = TransformPoint(Point(a.min_point.x,a.max_point.y,a.max_point.z),sm_mvp);
    p[5] = TransformPoint(Point(a.min_point.x,a.max_point.y,a.min_point.z),sm_mvp);
    p[6] = TransformPoint(Point(a.min_point.x,a.min_point.y,a.max_point.z),sm_mvp);
    p[7] = TransformPoint(Point(a.min_point.x,a.min_point.y,a.min_point.z),sm_mvp);
    AABB res;
    res.max_point.x = std::numeric_limits<float>::min();
    res.max_point.y = std::numeric_limits<float>::min();
    res.max_point.z = std::numeric_limits<float>::min();
    res.min_point.x = std::numeric_limits<float>::max();
    res.min_point.y = std::numeric_limits<float>::max();
    res.min_point.z = std::numeric_limits<float>::max();
    for (int i = 0 ; i < 8 ; i++) {
        if (res.max_point.x < p[i].x )
            res.max_point.x = p[i].x;
        if (res.max_point.y < p[i].y )
            res.max_point.y = p[i].y;
        if (res.max_point.z < p[i].z )
            res.max_point.z = p[i].z;

        if (res.min_point.x > p[i].x )
            res.min_point.x = p[i].x;
        if (res.min_point.y > p[i].y )
            res.min_point.y = p[i].y;
        if (res.min_point.z > p[i].z )
            res.min_point.z = p[i].z;
     }


    return res;
}
