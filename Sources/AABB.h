#ifndef __AABB__
#define __AABB__
#pragma once
#include <vector>
#include "mat_math.h"
class AABB {
public:
    AABB() :min_point(),max_point() {}
    AABB(Point min_p,Point max_p) :min_point(min_p),max_point(max_p) {}
    Point Center();
    /*data*/
    Point min_point;
    Point max_point;
};
std::vector <Point> AABBPoints(const AABB &a);
AABB FrustrumSize(const SMat4x4& r);
AABB ProjectionAABB(const AABB &a, const SMat4x4 &sm_mvp);

#endif
